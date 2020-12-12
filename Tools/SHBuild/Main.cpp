/*
	© 2014-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Main.cpp
\ingroup MaintenanceTools
\brief 宿主构建工具：递归查找源文件并编译和静态链接。
\version r4354
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-06 14:33:55 +0800
\par 修改时间:
	2020-12-12 02:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Tools::SHBuild::Main

This is a command line tool to build project files simply.
See readme file for details.
*/


#include <YSBuild.h>
#include YFM_YSLib_Core_YStorage // for YSLib::size_t, YSLib::RecordLevel,
//	YSLib::Notice, YSLib::FetchStaticRef, YSLib::string, YSLib::set, YSLib::map,
//	YSLib::vector, YSLib::Logger, YSLib::Warning, YSLib::to_std_string,
//	YSLib::string_view, YSLib::to_pmr_string, YSLib::to_string, YSLib::Debug,
//	YSLib::Informative, YSLib::Err, namespace std::placeholders,
//	std::initializer_list, std::invalid_argument, YSLib::uspawn,
//	YSLib::ifstream, YSLib::uremove, YSLib::istringstream;
#include YFM_YSLib_Core_YFunc // for YSLib::function;
#include YFM_YSLib_Service_FileSystem // for namespace YSLib::IO, IO::Path;
#include YFM_YSLib_Core_YString // for YSLib::String, ystdex::raise_exception,
//	YSLib::FilterExceptions;
#include YFM_YCLib_Host // for namespace platform_ex, platform_ex::Terminal;
#include YFM_YSLib_Service_YTimer // for namespace std::chrono;
#include <ystdex/mixin.hpp> // for ystdex::wrap_mixin_t;
#include YFM_NPL_Dependency // for NPL::DepsEventType, NPL, A1, Forms,
//	TraceException, TraceBacktrace, NPL::DecomposeMakefileDepList,
//	NPL::FilterMakefileDependencies, NPL::Install*;
#include <ystdex/concurrency.h> // for std::mutex, std::lock_guard,
//	ystdex::task_pool;
#include <ystdex/string.hpp> // for ystdex::ston, ystdex::sfmt,
//	ystdex::write_literal, ystdex::rtrim, ystdex::ltrim, ystdex::trim;
#include <iostream> // for std::cout, std::endl;
#include YFM_YSLib_Core_YConsole // for YSLib::Consoles;
#include <ratio> // for std::milli;
#if YCL_Win32
#	include YFM_Win32_YCLib_MinGW32 // for platform_ex::ParseCommandArguments;
#endif
#include <sstream> // for complete istringstream;
#if SHBuild_NoBacktrace
#	define SHBuild_UseBacktrace false
#	define SHBuild_UseSourceInfo false
#else
#	define SHBuild_UseBacktrace true
#	define SHBuild_UseSourceInfo true
#endif

//! \since build 837
namespace SHBuild
{

//! \since build 905
//@{
using YSLib::size_t;
using YSLib::RecordLevel;
using YSLib::Notice;
using YSLib::FetchStaticRef;
using YSLib::string;
using YSLib::set;
using YSLib::map;
using YSLib::vector;
using YSLib::Logger;
using YSLib::function;
using YSLib::Warning;
using YSLib::to_std_string;
using YSLib::string_view;
using YSLib::to_pmr_string;
using YSLib::to_string;
namespace IO = YSLib::IO;
using YSLib::Debug;
using YSLib::Informative;
using IO::Path;
using YSLib::String;
using YSLib::FilterExceptions;
using YSLib::Err;
//@}
//! \since build 547
using namespace platform_ex;
//! \since build 547
using namespace std::placeholders;

namespace
{

/*!
\brief 默认构建根目录路径。
\since build 540
*/
#define OPT_build_path ".shbuild"

//! \since build 477
using IntException = ystdex::wrap_mixin_t<std::runtime_error, int>;
//! \since build 477
using ystdex::raise_exception;
//! \since build 539
template<typename... _tParams>
YB_NORETURN inline PDefH(void, raise_exception, int ret, _tParams&&... args)
	ImplExpr(raise_exception<IntException>({
		std::runtime_error(yforward(args)...), ret}))
//! \since build 522
YB_NORETURN inline PDefH(void, raise_exception, int ret)
	ImplExpr(raise_exception(ret, "Failed calling command."))

//! \since build 648
using ystdex::quote;
//! \since build 648
template<class _tString>
auto
Quote(_tString&& str) -> decltype(quote(yforward(str), '\''))
{
	return quote(yforward(str), '\'');
}

//! \since build 592
enum class BuildMode : yimpl(size_t)
{
	AR = 1,
	LD = 2
};

//! \since build 624
using LogGroup = NPL::DepsEventType;
//! \since build 547
//@{
using opt_uint = unsigned long;
LogGroup LastLogGroup(LogGroup::General);
std::mutex LastLogGroupMutex;
std::bitset<size_t(LogGroup::Max)> LogDisabled;
//@}

//! \since build 861
yconstexpr const struct
{
	YB_NONNULL(2) void
	operator()(const char* line, RecordLevel lv = Notice,
		LogGroup grp = LogGroup::General) const
	{
		DoPrint(line, lv, grp);
	}
	template<class _tStringView>
	void
	operator()(const _tStringView& line, RecordLevel lv = Notice,
		LogGroup grp = LogGroup::General) const
	{
		DoPrint(line.data(), lv, grp);
	}

	YB_NONNULL(1) static void
	DoPrint(const char* line, RecordLevel lv = Notice,
		LogGroup grp = LogGroup::General)
	{
		std::lock_guard<std::mutex> lck(LastLogGroupMutex);

		LastLogGroup = grp;
		FetchStaticRef<Logger>().Log(lv, [&]{
			return line;
		});
	}
} PrintInfo{};


//! \since build 547
//@{
string RequestedCommand;
set<string> IgnoredDirs;
string OutputDir;
size_t MaxJobs(0);
//! \since build 592
BuildMode Mode(BuildMode::AR);
//! \since build 556
string TargetName;
using EnvBindingMap = map<string, string>;
//! \since build 905
EnvBindingMap BuildVariables;

#define OPT_des_mul "Multiple occurrence is allowed."
#define OPT_des_last \
	"If this option occurs more than once, only the last one is effective."

const struct Option
{
	const char *prefix, *name = {}, *option_arg;
	//! \since build 861
	vector<const char*> option_details;
	//! \since build 852
	function<bool(const string&)> filter;

	//! \since build 852
	Option(const char* pfx, const char* n, const char* opt_arg,
		function<void(string&&)> parse,
		std::initializer_list<const char*> il)
		: prefix(pfx), name(n), option_arg(opt_arg), option_details(il),
		filter(std::bind(ystdex::filter_prefix<string, string, decltype(parse)>,
		_1, string(prefix), parse))
	{}
	template<typename _type, typename _func>
	Option(const char* pfx, const char* n, const char* opt_arg, _func f,
		_type threshold, std::initializer_list<const char*> il)
		: Option(pfx, n, opt_arg, [this, f, threshold](string&& val){
			const auto
				print(std::bind(PrintInfo, _1, Warning, LogGroup::General));

			try
			{
				const auto uval(ystdex::ston<_type>(to_std_string(val)));

				if(uval < threshold)
					f(uval);
				else
					print("Warning: Value " + Quote(val) + " of " + name
						+ " is out of range.");
			}
			CatchExpr(std::invalid_argument&,
				print("Warning: Value " + Quote(val) + " of " + name
					+ " is invalid."))
			CatchExpr(std::out_of_range&,
				print("Warning: Value " + Quote(val) + " of " + name
					+ " is out of range."))
		}, il)
	{}

	PDefHOp(bool, (), const string& arg) const
		ImplRet(filter(arg))
} OptionsTable[]{
	// NOTE: There is no restriction of the order of option names. The order
	//	has effects on the help message, so just make it alphabatical to ease
	//	users by default, except that there are some more reasons like the
	//	effects on the execution mode.
	// XXX: '-cmd,' is just at the front alphabatically now. 
	{"-xcmd,", "command", "COMMAND", [](string&& val) ynothrow{
		RequestedCommand = std::move(val);
	}, {"Specify the name of a command to run.", "If this option is set, all"
		" other parameters not recognized as options are treated as arguments"
		" of the command. Currently the following COMMAND name and arguments"
		" combinations are supported:",
		"  EnsureDirectory PATH",
		"    Make PATH available as a directory, as 'mkdir -p PATH'.",
		"  InstallFile DST SRC",
		"    Copy file specified by SRC to DST if the contents are different.",
		"  InstallDirectory DST SRC",
		"    Recursively call of InstallFile for directory contents.",
		"  InstallHardLink DST SRC",
		"    Remove DST and then make hard link to SRC.",
		"  InstallSymbolicLink DST SRC",
		"    Remove DST and then make symbolic link to SRC. This may fail if"
		" the call process has no proper permissions.",
		"  InstallExecutable DST SRC",
		"    Call InstallFile and then make DST be with executable permission"
		" like 'chmod +x DST'. Note it is empty operation, not fully"
		" implemented yet for OS other than Windows.",
		"  RunNPL UNIT",
		"    Read and execute NPLA1 translation unit specified by string"
		" UNIT.",
		"  RunNPLFile SRC [ARGS...]",
		"    Read and execute NPLA1 translation unit specified by file path"
		" SRC with optional arguments ARGS.", OPT_des_mul}},
	{"-xd,", "output directory path", "DIR_PATH", [](string&& val){
		string raw(NPL::Deliteralize(val));

		PrintInfo("Output directory is switched to " + Quote(raw) + '.');
		OutputDir = std::move(raw);
	}, {"The name of output directory (possibly quoted). Default value is '"
		OPT_build_path "'.", OPT_des_mul}},
	{"-xdef,", "defined variables", "VAR_DEF_SPEC", [](const string& spec){
		const auto s(spec.find('='));

		if(s != string::npos)
		{
			if(s != 0)
			{
				using ystdex::sfmt;
				auto name(spec.substr(0, s));
				auto value(NPL::Deliteralize(string_view(&spec[s + 1],
					spec.length() - s - 1)));

				PrintInfo(sfmt("Found build variable '%s' with value '%s'.",
					name.c_str(), value.data()));
				BuildVariables.emplace(std::move(name),
					to_pmr_string(value, spec.get_allocator()));
			}
			else
				throw std::invalid_argument(ystdex::sfmt(
					"Empty variable name found the variable definition '%s'",
					spec.c_str()));
		}
		else
			throw std::invalid_argument(ystdex::sfmt(
				"No separator '=' found in the variable definition '%s'",
				spec.c_str()));
	}, {"Add a variable definition.",
		"The definition VAR_DEF_SPEC shall be in the form of VAR=VALUE, where"
		" VAR and VALUE are the name and the possibly quoted value of the"
		" variable.",
		" There is no check on the NAME, but it should have no"
		" whitespaces (otherwise it is not accepted in a single options"
		" anyway). The NAME can still have characters not portable among"
		" different shell applications.",
		"The variable definitions are used in the build context.",
		"The variables can be used like environment variables. However, they"
		" are not inherited by the environment of the called commands."
		" Moreover, their names are guaranteed case-sensitive regardless of the"
		" host environment of this program."
		"If a definition has specified the same name to a environment variable,"
		" it overrides the value in the context specified previously by the"
		" environment variable."
		OPT_des_mul}},
	{"-xid,", "ignored directories", "DIR_NAME", [](string&& val){
		string raw(NPL::Deliteralize(val));

		PrintInfo("Subdirectory " + Quote(raw) + " should be ignored.");
		IgnoredDirs.emplace(std::move(raw));
	}, {"The name of subdirectory (possibly quoted) which should be ignored"
		" when scanning.", OPT_des_mul}},
	{"-xj,", "job max count", "MAX_JOB_COUNT", [](opt_uint uval){
		PrintInfo("Set job max count = " + to_string(uval) + '.');
		MaxJobs = size_t(uval);
	}, 0x100UL, {"Max count of parallel jobs at the same time.",
		"This number would be used to limit the number of tasks being"
		" spawning. If no valid value is explicitly specified, the implicit"
		" default value is 0. If this value is not more than 1, only one"
		" task would be load and run at each time.", OPT_des_last}},
	{"-xlogfl,", "log filter level", "LOG_LEVEL", [](opt_uint uval){
		FetchStaticRef<Logger>().FilterLevel = Logger::Level(uval);
	}, 0x100UL, {"The unsigned integer log level threshold of the logger.",
		"Only log with level less than this value and whose group is not"
		" disabled would be present in the output stream.",
		"See description of '-xloggd,' for information about log"
		" groups.", OPT_des_last}},
	{"-xloggd,", "disabled log group", "LOG_GROUP", [](opt_uint uval){
		LogDisabled.set(uval);
		PrintInfo("Log group disabled: " + to_string(uval) + '.');
	}, opt_uint(LogGroup::Max), {"The log group should be disabled.",
		"Currently these groups are supported:", "  0: General;",
		"  1: Search;", "  2: Build;", "  3: Command;", "  4: DepsCheck.",
		"Except for group 4, other groups are enabled by default.",
		OPT_des_last}},
	{"-xlogge,", "enabled log group", "LOG_GROUP", [](opt_uint uval){
		LogDisabled.set(uval, {});
		PrintInfo("Log group enabled: " + to_string(uval) + '.');
	}, opt_uint(LogGroup::Max), {"The log group should be enabled.",
		"See description of '-xloggd,'.", OPT_des_last}},
	{"-xmode,", "mode", "MODE", [](opt_uint uval){
		switch(BuildMode(uval))
		{
		case BuildMode::AR:
		case BuildMode::LD:
			Mode = BuildMode(uval);
			break;
		default:
			PrintInfo("Ignored unsupported mode " + Quote(to_string(uval)) + '.',
				Warning);
		}
	}, 0x3UL, {"The target action mode.",
		"Value '1' represents call of AR for the final target, and '2' is LD."
		" Other value is reserved and to be ignored."
		" Default value is '1'.",
		"If this option occurs more than once, only the last one is"
		" effective."}},
	{"-xn,", "Target name", "OBJ_NAME", [](string&& val){
		PrintInfo("Target name is switched to " + Quote(val) + '.');
		TargetName = std::move(val);
	}, {"The base name of final target."
		" Default value is same to top level directory name.", OPT_des_last}},
};

const array<const char*, 3> DeEnvs[]{
	{{"CC", "gcc", "The C compiler executable."}},
	{{"CXX", "g++", "The C++ compiler executable."}},
	{{"AR", "ar", "The archiver executable."}},
	{{"ARFLAGS", "rcs", "The archiver flags."}},
	{{"LD", "ld", "The linker executable."}},
	{{"LDFLAGS", "", "The linker flags."}},
	{{"LIBS", "", "Extra options as the linker options at end."}},
	{{"SHBuild_CFLAGS", "", "Flags used in command options when the C complier"
		" is called."}},
	{{"SHBuild_CXXFLAGS", "", "Flags used in command options when the C++"
		" complier is called."}},
	{{"SHBuild_Epoch", "", "The session epoch."}}
};
//@}

//! \since build 541
void
EnsureOutputDirectory(const string& opath)
{
	try
	{
		PrintInfo("Checking output directory: " + Quote(opath) + " ...");
		IO::EnsureDirectory(opath);
	}
	CatchExpr(std::system_error&, raise_exception(2,
		("Failed creating directory " + Quote(opath) + '.').c_str()))
}

//! \since build 545
//@{
std::chrono::nanoseconds
CheckModification(const string& path)
{
	const auto print(std::bind(PrintInfo, _1, Debug, LogGroup::DepsCheck));

	print("Checking path " + Quote(path) + " ...");

	const auto& file_time(IO::GetFileModificationTimeOf(path.c_str()));

	print("Modification time: " + to_string(file_time.count()) + " .");
	return file_time;
}

bool
CheckBuild(const vector<string>& ipaths, const string& opath)
{
	if(!ipaths.empty())
	{
		const auto print(std::bind(PrintInfo, _1, _2, LogGroup::Build));

		try
		{
			const auto& omod(CheckModification(opath));

			if(std::none_of(ipaths.cbegin(), ipaths.cend(),
				[&](const string& ipath){
				return omod < CheckModification(ipath);
			}))
			{
				print("Output " + Quote(opath) + " is up-to-date, skipped.",
					Informative);
				return {};
			}
		}
		CatchExpr(std::system_error& e, print(e.what(), Debug))
		return true;
	}
	return {};
}
//@}

//! \since build 796
YB_NONNULL(1) void
RunNPLFromStream(const char* name, std::istream&& is)
{
	using namespace NPL;
	using namespace A1;
	using namespace Forms;
	REPLContext context;
	TermNode term{context.Allocator};
#if SHBuild_UseBacktrace
	ContextNode::ReducerSequence backtrace{context.Allocator};
#endif

#if SHBuild_UseSourceInfo
	context.UseSourceLocation = true;
#endif
	// NOTE: Force filter level to avoid uninterested NPLA messages. This is
	//	necessary at least in stage 1.
	context.Root.Trace.FilterLevel = Logger::Level::Informative;
	LoadStandardContext(context);
	context.OutputStreamPtr = NPL::make_observer(&std::cout);

	auto& rctx(context.Root);

	InvokeIn(rctx, [&]{
		context.Root.GetRecordRef().DefineChecked("env_SHBuild_",
			GetModuleFor(rctx, [&]{
			LoadModule_SHBuild(context);
			// XXX: Overriding.
			rctx.GetRecordRef().Define("SHBuild_BaseTerminalHook_",
				ValueObject(function<void(const string&, const string&)>(
				[&](const string& n, const string& val){
					using namespace YSLib::Consoles;
					using IO::StreamPut;
					auto& os(context.GetOutputStreamRef());
					Terminal te;

					{
						const auto t_gd(te.LockForeColor(DarkCyan));

						StreamPut(os, n.c_str());
					}
					ystdex::write_literal(os, " = \"");
					{
						const auto t_gd(te.LockForeColor(DarkRed));

						StreamPut(os, val.c_str());
					}
					os.put('"') << std::endl;
			})));
		}));
		context.ShareCurrentSource(name);
		try
		{
			context.Root.Rewrite(
				NPL::ToReducer(context.Allocator, [&](ContextNode& ctx){
				ctx.SaveExceptionHandler();
				// TODO: Blocked. Use C++14 lambda initializers to simplify the
				//	implementation.
				ctx.HandleException = std::bind([&](std::exception_ptr p,
					const ContextNode::ReducerSequence::const_iterator& i){
					ctx.TailAction = nullptr;
#if SHBuild_UseBacktrace
					ctx.Shift(backtrace, i);
#endif
					YAssertNonnull(p);
					TryExpr(std::rethrow_exception(std::move(p)))
					catch(std::exception& e)
					{
					//	auto& trace(ctx.Trace);
						auto& trace(FetchStaticRef<Logger>());
						std::lock_guard<std::mutex> lck(LastLogGroupMutex);

						LastLogGroup = LogGroup::General;
						TraceException(e, trace);
						trace.TraceFormat(Notice, "Location: %s.",
							context.CurrentSource
							? context.CurrentSource->c_str() : "<unknown>");
#if SHBuild_UseBacktrace
						TraceBacktrace(backtrace, trace);
#endif
						// NOTE: The original exception is traced by
						//	%TraceException, and now the new exception is
						//	constructed to avoid the duplication in the caller
						//	(traced by %YSLib::ExtractException).
						throw NPLException("Error detected in the execution"
							" (see the backtrace for details).");
					}
				}, std::placeholders::_1, ctx.GetCurrent().cbegin());
				term = context.ReadFrom(is);
				// XXX: Is it necessar to change the text color here?
				return A1::ReduceOnce(term, ctx);
			}));
		}
		// NOTE: As %A1::TryLoadSource.
		CatchExpr(..., std::throw_with_nested(NPLException(
			ystdex::sfmt("Failed loading external unit '%s'.",
			name))));
	});
}

} // unnamed namespace;


/*!
\brief 被调用的外部命令。
\since build 837
*/
class Command final
{
private:
	string cmdname;
	vector<string> args{};

public:
	Command(const string& cmd)
		: cmdname(cmd)
	{}
	DefDeCopyMoveCtorAssignment(Command)

	const string&
	GetString() const
	{
		return cmdname;
	}

	static int
	Call(const Command& cmd)
	{
		const auto& cmd_str(cmd.GetString());

		PrintInfo(cmd_str, Debug, LogGroup::Command);
		return YSLib::uspawn(cmd_str.c_str());
	}
};


//! \since build 520
//@{
class BuildContext final : private ystdex::noncopyable
{
private:
	//! \since build 538
	mutable ystdex::task_pool jobs;
	mutable std::mutex job_mtx{};
	mutable int result = 0;
	string flags{};
	//! \since build 624
	mutable vector<std::future<int>> futures{};

public:
	set<string> IgnoredDirs{};
	//! \since build 540
	string OutputDir{OPT_build_path};
	vector<string> Options{};
	//! \since build 905
	EnvBindingMap Vars{};
	//! \since build 592
	BuildMode Mode = BuildMode::AR;
	//! \since build 556
	string TargetName{};

	BuildContext(size_t n)
		: jobs(n)
	{}
	//! \since build 905
	BuildContext(size_t n, EnvBindingMap&& vars)
		: jobs(n), Vars(std::move(vars))
	{}

	//! \since build 545
	DefGetter(const ynothrow, const string&, Flags, flags)
	/*!
	\pre 断言：参数的数据指针非空。
	\since build 837
	*/
	string
	GetFlags(string_view) const;

	//! \since build 538
	int
	GetLastResult() const;
	//! \since build 547
	PDefH(const string&, GetEnv, const string& name) const
		ImplRet(Vars.at(name))

	void
	Build();

	//! \since build 837
	int
	Call(const Command&, size_t n = 0) const;

	//! \since build 837
	PDefH(void, CallWithException, const Command& cmd, size_t n = 0) const
		ImplExpr(CheckResult(Call(cmd, n)))

	//! \since build 539
	static PDefH(void, CheckResult, int ret)
		ImplExpr(ret == 0 ? void() : raise_exception(ret))

	//! \since build 837
	int
	RunTask(const Command&) const;
};


//! \since build 545
//@{
using Key = pair<Path, Path>;
using Value = vector<string>;
using ActionContext = YSLib::GRecursiveCallContext<Key, Value>;
using BuildAction = ActionContext::CallerType;

class Rule
{
public:
	BuildContext& Context;
	Key Source;

	/*!
	\pre 断言：参数的数据指针非空。
	\since build 648
	*/
	//@{
	YB_ATTR_nodiscard YB_PURE PDefH(string, GetCommand, string_view ext) const
		ImplRet(LookupCommand(GetCommandType(ext)))

	//! \since build 837
	YB_ATTR_nodiscard YB_PURE static string_view
	GetCommandType(string_view);

	YB_ATTR_nodiscard YB_PURE string
	LookupCommand(string_view) const;
	//@}
};

string_view
Rule::GetCommandType(string_view ext)
{
	YAssertNonnull(ext.data());

	if(ext == "c")
		return "CC";
	if(ext == "cc" || ext == "cpp" || ext == "cxx")
		return "CXX";
	return "";
}
string
Rule::LookupCommand(string_view name) const
{
	YAssertNonnull(name.data());

	return name.empty() ? string() : Context.GetEnv(string(name));
}


//! \since build 546
namespace
{

Value
BuildFile(const Rule& rule)
{
	const auto& bctx(rule.Context);
	const auto& ipth(rule.Source.first);
	const auto& fullname(ipth.VerifyAsMBCS());
	const auto& cmd_type(rule.GetCommandType(IO::GetExtensionOf(fullname)));
	const auto& cmd(rule.LookupCommand(cmd_type));
	const auto print(std::bind(PrintInfo, _1, _2, LogGroup::Build));

	if(!cmd.empty())
	{
		const auto& ofullname(rule.Source.second.VerifyAsMBCS());
		bool build{true};

		try
		{
			auto dfullname(ofullname);

			YAssert(!dfullname.empty(), "Invalid output name found.");
			// FIXME: Correct replacement when extension of %ofullname is not
			//	1 character.
			dfullname.back() = 'd';
			if(YSLib::ifstream tf{dfullname, std::ios_base::in})
			{
				const auto printd(std::bind(PrintInfo, _1, _2,
					LogGroup::DepsCheck));
				auto lst(NPL::DecomposeMakefileDepList(tf));

				if(NPL::FilterMakefileDependencies(lst))
				{
					printd(to_string(lst.size()) + " dependenc"
						+ (lst.size() == 1 ? "y" : "ies") + " found.", Debug);
					if(!CheckBuild(lst, ofullname))
						build = {};
				}
				else
					printd("Wrong dependencies format found.", Warning);
			}
		}
		CatchIgnore(std::exception&)
		if(build)
		{
			print("Compile file: " + Quote(ipth.back().GetMBCS()) + '.',
				Informative);
			bctx.CallWithException(cmd + " -MMD -c " + bctx.GetFlags(
				cmd_type) + ' ' + quote(fullname) + " -o " + quote(ofullname));
		}
		return {ofullname};
	}
	else
		print("No rule found for " + Quote(fullname) + '.', Warning);
	return {};
}

Value
SearchDirectory(const Rule& rule, const ActionContext& actx)
{
	using IO::NodeCategory;
	const auto& ipth(rule.Source.first);
	const auto& opth(rule.Source.second);
	const auto& path(ipth.VerifyAsMBCS());
	vector<string> subdirs, ofiles;
	vector<pair<string, string>> src_files;
	const auto print(std::bind(PrintInfo, _1, _2, LogGroup::Search));

	print("Searching path: " + Quote(path) + " ...", Notice);
	IO::TraverseChildren(path,
		[&](NodeCategory c, IO::NativePathView npv){
		const auto& name(String(npv).GetMBCS());

		if(name[0] != '.')
		{
			if(bool(c & NodeCategory::Directory))
			{
				if(ystdex::exists(rule.Context.IgnoredDirs, name))
					print("Subdirectory " + Quote(path + name
						+ IO::FetchSeparator<char>()) + " is ignored.",
						Informative);
				else
					subdirs.push_back(name);
			}
			else
			{
				auto cmd(rule.GetCommand(IO::GetExtensionOf(name)));

				if(!cmd.empty())
					src_files.emplace_back(std::move(cmd), name);
				else
					print("Ignored non source file " + Quote(name) + '.',
						Informative);
			}
		}
	});
	for(const auto& name : subdirs)
		ystdex::vector_concat(ofiles,
			actx(make_pair(ipth / name, opth / name)));

	const auto snum(src_files.size());

	print(ystdex::sfmt("%zu file(s) found to be built in path \"%s\".", snum,
		path.c_str()), Informative);
	if(snum != 0)
	{
		EnsureOutputDirectory(opth.VerifyAsMBCS());
		for(const auto& pr : src_files)
		{
			const auto& name(pr.second);
			auto ofile(actx(make_pair(ipth / name, opth / (name + ".o"))));

			// XXX: Check size.
			if(!ofile.empty() && !ofile.front().empty())
				ofiles.push_back(std::move(ofile.front()));
		}
	}
	return ofiles;
}

} // unnamed namespace;


string
BuildContext::GetFlags(string_view cmd_type) const
{
	YAssertNonnull(cmd_type.data());

	string res;

	if(cmd_type == "CC")
		res = GetEnv("SHBuild_CFLAGS");
	else if(cmd_type == "CXX")
		res = GetEnv("SHBuild_CXXFLAGS");
	return ystdex::trim(ystdex::rtrim(res) + ' '
		+ ystdex::ltrim(string(flags)));
}
int
BuildContext::GetLastResult() const
{
	std::lock_guard<std::mutex> lck(job_mtx);

	return result;
}

void
BuildContext::Build()
{
	PrintInfo("Ready to run, job max count: "
		+ to_string(jobs.get_max_task_num()) + '.');
	OutputDir = IO::NormalizeDirectoryPathTail(OutputDir);
	PrintInfo("Normalized output directory: " + Quote(OutputDir) + '.');
	if(Options.empty())
	{
		PrintInfo("No build options found. Stop.", Warning);
		return;
	}

	const auto in(IO::NormalizeDirectoryPathTail(Options[0]));
	const auto ipath(IO::MakeNormalizedAbsolute(Path(in)));

	PrintInfo("Absolute path " + Quote(to_string(ipath).GetMBCS())
		+ " recognized.");
	if(!IO::VerifyDirectory(in))
		raise_exception(1, "SRCPATH is not existed.");
	EnsureOutputDirectory(OutputDir);
	std::for_each(next(Options.begin()), Options.end(), [&](const string& opt){
		flags += ' ' + opt;
	});

	const auto opth(Path(OutputDir)
		/ (TargetName.empty() ? [](const String& n) -> String{
			if(!n.empty())
				return
					n.back() == u':' ? String(n.substr(0, n.length() - 1)) : n;
			throw std::invalid_argument("Empty input path component found.");
		}(ipath.back()) : String(TargetName)));
	const auto print(std::bind(PrintInfo, _1, _2, LogGroup::Build));
	try
	{
		const auto& ofiles(ActionContext([this](const Key& name){
			const shared_ptr<Rule> p_rule(new Rule{*this, name});

			return VerifyDirectory(name.first)
				? BuildAction([=](const ActionContext& actx){
				return SearchDirectory(*p_rule, actx);
			}) : BuildAction([=](const ActionContext&){
				return BuildFile(*p_rule);
			});
		})({Path(in), opth}));

		// TODO: Optimize for job dependency.
		if(jobs.get_max_task_num() > 1)
		{
			print("Waiting for unfinished tasks before linking ...", Notice);
			jobs.reset();
		}
		CheckResult(GetLastResult());

		const auto onum(ofiles.size());

		print("Found " + to_string(onum) + " .o file(s).", Informative);
		if(onum != 0)
		{
			Path pth(opth);

			YAssert(!pth.empty(), "Invalid path found.");
			pth.pop_back();
			EnsureOutputDirectory(pth.VerifyAsMBCS());

			auto target(to_string(opth).GetMBCS());
			const auto& LDFLAGS(GetEnv("LDFLAGS"));
			const auto& cmd(Mode == BuildMode::AR ? GetEnv("AR") + ' '
				+ GetEnv("ARFLAGS") : GetEnv("LD") + ' ' + LDFLAGS + " -o");

			if(Mode == BuildMode::AR)
				target += ".a";
			// TODO: Find extension properly.
			else
				// TODO: Parse %LDFLAGS.
				// TODO: Support cross compiling.
				target += ystdex::exists_substr(LDFLAGS, "-Bdynamic")
					|| ystdex::exists_substr(LDFLAGS, "-shared")
#if YCL_Win32
					? ".dll"
#else
					? ".so"
#endif
					: ".exe";
			if(CheckBuild(ofiles, target))
			{
				auto str(cmd + ' ' + quote(target));

				// FIXME: Prevent path too long.
				for(const auto& ofile : ofiles)
					str += ' ' + quote(ofile);
				if(Mode == BuildMode::LD)
					str += ' ' + GetEnv("LIBS");
				print("Link file: " + Quote(target) + '.', Informative);
				if(Mode == BuildMode::AR)
				{
					// NOTE: Since the return value might be
					//	implementation-defined and the next operations might be
					//	still meaningful, it is not intended to throw an
					//	exception.
					if(YSLib::uremove(target.c_str()) != 0)
						PrintInfo("Failed deleting file " + Quote(target) + '.',
							Warning);
					PrintInfo("Deleted file " + Quote(target) + '.', Debug);
				}
				CallWithException(str, 1);
			}
		}
		else
			print("No files to be built.", Warning);
	}
	catch(...)
	{
		FilterExceptions([print, this]{
			size_t succ(0), fail(1);

			print("Waiting for unfinished tasks ...", Warning);
			for(auto& fut : futures)
				if(FilterExceptions([&]{
					if(fut.get() == 0)
						++succ;
					else
						++fail;
				}, "futures stat"))
					++fail;
			// TODO: Show statistics also on success?
			print(ystdex::sfmt("%zu task(s) succeeded, %zu task(s) failed.",
				succ, fail), Informative);
			jobs.reset();
		});
		throw;
	}
}

int
BuildContext::Call(const Command& cmd, size_t n) const
{
	if(n == 0)
		n = jobs.get_max_task_num();
	if(n <= 1)
		return Command::Call(cmd);
	else
	{
		PrintInfo("Task enqueued.", Debug, LogGroup::Command);
		return RunTask(cmd);
	}
}

int
BuildContext::RunTask(const Command& cmd) const
{
	{
		std::lock_guard<std::mutex> lck(job_mtx);

		if(result != 0)
			return result;
	}

	// TODO: Blocked. Use ISO C++14 lambda initializers to simplify
	//	implementation and optimize copy of %cmd.
	// TODO: Reduce memory footprint.
	futures.push_back(jobs.wait([&, cmd]{
		const int res(Command::Call(cmd));
		{
			std::lock_guard<std::mutex> lck(job_mtx);

			if(result == 0)
				result = res;
		}
		return res;
	}));
	return 0;
}
//@}

} // namespace SHBuild;


int
main(int argc, char* argv[])
{
	using namespace std::chrono;
	using namespace SHBuild;
	// XXX: Assume this is in 'unsigned long long' and wrap (but not overlow)
	//	most at maximum milliseconds.
	using ms = duration<unsigned long long, std::milli>;
	auto epoch(YSLib::Timers::FetchEpoch<steady_clock>());
	Terminal te, te_err(stderr);
	
	return FilterExceptions([&]{
		string epoch_var;

		YSLib::FetchEnvironmentVariable(epoch_var, "SHBuild_Epoch");
		if(epoch_var == "0")
		{
			epoch_var = YSLib::sfmt("%llu",
				duration_cast<ms>(epoch - steady_clock::time_point()).count());
			YSLib::SetEnvironmentVariable("SHBuild_Epoch", epoch_var.c_str());
		}
		else if(!epoch_var.empty())
			TryExpr(epoch = steady_clock::time_point(
				ms(std::stoull(to_std_string(epoch_var)))))
			CatchIgnore(std::invalid_argument&)

		auto& logger(FetchStaticRef<Logger>());

		LogDisabled.set(size_t(LogGroup::DepsCheck));
		yunseq(YSLib::FetchCommonLogger().FilterLevel = Informative,
			logger.FilterLevel = Debug);
		logger.SetFilter([](Logger::Level lv, Logger& l){
			return !ystdex::qualify(LogDisabled)[size_t(LastLogGroup)]
				&& Logger::DefaultFilter(lv, l);
		});
		logger.SetSender([&](Logger::Level lv, Logger&, const char* str)
			YB_NONNULL(4){
			const auto stream(lv <= Warning ? stderr : stdout);
			auto& term_ref(lv <= Warning ? te_err : te);
			const auto
				dcnt(duration_cast<ms>(steady_clock::now() - epoch).count());

			term_ref.RestoreAttributes();
			// XXX: Error from 'std::fprintf' is ignored.
			std::fprintf(stream, "[%04u.%03u][%zu:%#02X]",
				unsigned(dcnt / 1000U), unsigned(dcnt % 1000U),
				size_t(LastLogGroup), unsigned(lv));
			if(std::fflush(stream) == 0)
			{
				Terminal::Guard
					guard(term_ref, std::bind(UpdateForeColorByLevel, _1, lv));

				// XXX: Error is ignored.
				term_ref.WriteString(stream, &YSLib::Nonnull(str)[0]);
				// XXX: Error is ignored.
				std::fflush(stream);
			}
			// XXX: Errors are ignored.
			std::fputc('\n', stream);
		});

#if YCL_Win32
			yunused(argc), yunused(argv);

			auto xargv(platform_ex::ParseCommandArguments());
			const auto xargc(xargv.size());
#else
			const auto xargc(static_cast<size_t>(argc));
			const auto xargv(argv);
#endif

		if(xargc > 1)
		{
			PrintInfo(ystdex::sfmt("Finished loading %zu command argument(s).",
				xargc), Debug, LogGroup::General);

			vector<string> args;
			bool opt_trans(true);

			for(size_t i(1); i < xargc; ++i)
			{
				string arg(xargv[i]);

				if(opt_trans && arg == "--")
					opt_trans = {};
				else if(!opt_trans || (!arg.empty()
					&& std::none_of(begin(OptionsTable), end(OptionsTable),
					[&](const Option& opt){
						return opt(arg);
				})))
					args.emplace_back(std::move(arg));
			}
			if(!RequestedCommand.empty())
			{
				const auto sz(args.size());
				const auto check_n([sz](size_t n){
					if(sz != n)
						throw std::runtime_error(ystdex::sfmt("Wrong number %zu"
							" of arguments (should be %zu) found.", sz, n));
				});
				const auto check_n_ge([sz](size_t n){
					if(sz < n)
						throw std::runtime_error(ystdex::sfmt("Wrong number %zu"
							" of arguments (should at least be %zu) found.", sz,
							n));
				});

				try
				{
					using namespace NPL;

					PrintInfo(ystdex::sfmt("Found requested command '%s'.",
						RequestedCommand.c_str()), Debug, LogGroup::General);
					if(RequestedCommand == "EnsureDirectory")
					{
						check_n(1);
						IO::EnsureDirectory(args[0]);
					}
					else if(RequestedCommand == "InstallFile")
					{
						check_n(2);
						InstallFile(args[0], args[1]);
					}
					else if(RequestedCommand == "InstallDirectory")
					{
						check_n(2);
						InstallDirectory(args[0], args[1]);
					}
					else if(RequestedCommand == "InstallHardLink")
					{
						check_n(2);
						InstallHardLink(args[0], args[1]);
					}
					else if(RequestedCommand == "InstallSymbolicLink")
					{
						check_n(2);
						InstallSymbolicLink(args[0], args[1]);
					}
					else if(RequestedCommand == "InstallExecutable")
					{
						check_n(2);
						InstallExecutable(args[0], args[1]);
					}
					else if(RequestedCommand == "RunNPL"
						|| RequestedCommand == "RunNPLFile")
					{
						check_n_ge(1);

						const auto p_cmd_args(YSLib::LockCommandArguments());

						p_cmd_args->Arguments = std::move(args);

						const auto& arg0(p_cmd_args->Arguments.front());

						if(RequestedCommand == "RunNPL")
							RunNPLFromStream("*STDIN*",
								YSLib::istringstream(arg0));
						else
						{
							const auto p(A1::OpenFile(arg0.c_str()));

							RunNPLFromStream(arg0.c_str(), std::move(*p));
						}
					}
					else
						throw std::runtime_error(ystdex::sfmt("Specified"
							" command name '%s' is not supported.",
							RequestedCommand.c_str()));
				}
				CatchExpr(std::exception& e,
					std::throw_with_nested(std::move(e)))
			}
			else
			{
				PrintInfo(ystdex::sfmt("Ready to enter build mode for up to %zu"
					" parallel task(s).", MaxJobs), Informative,
					LogGroup::General);

				BuildContext bctx(MaxJobs, std::move(BuildVariables));

				for(const auto& env : DeEnvs)
					// NOTE: The environment variable does not override
					//	variables set by %BuildVariables.
					bctx.Vars.insert({env[0], env[1]});
				for(const auto& env : DeEnvs)
				{
					const string name(env[0]);

					YSLib::FetchEnvironmentVariable(bctx.Vars[name], env[0]);
					PrintInfo(name + " = " + bctx.GetEnv(name));
				}
				if(!OutputDir.empty())
					bctx.OutputDir = std::move(OutputDir);
				// NOTE: Remained command line arguments are moved as options
				//	saved in the build context options. SRCPATH is expected as
				//	the 1st build context option, following by the options in
				//	interface specification.
				yunseq(bctx.IgnoredDirs = std::move(IgnoredDirs),
					bctx.Options = std::move(args), bctx.Mode = Mode);
				if(!TargetName.empty())
					bctx.TargetName = std::move(TargetName);
				PrintInfo("Build OutputDir = " + bctx.OutputDir);
				bctx.Build();
			}
		}
		else if(xargc == 1)
		{
			using IO::StreamPut;
			using ystdex::sfmt;
			auto& os(std::cout);
			const string& prog(xargv[0]);

			StreamPut(os, sfmt("Usage: [ENV ...] \"%s\" [OPTIONS ...] SRCPATH"
				" [OPTIONS ... [-- [ARGS...]]]\n"
				"  or:  [ENV ...] \"%s\" [OPTIONS ... [-- ARGS...]]\n"
				"\tThis program is a tool to build the source tree, with some"
				" additional functionalities. If there are no command"
				" arguments, this help message is shown. Otherwise, the program"
				" will try working in a specific execution mode based on"
				" the specific command arguments.\n"
				"\tThere are two execution modes, the building mode and the"
				" command requesting mode, exclusively. In the former mode,"
				" building backends (commands for compiling) are called. The"
				" latter is only enabled when there are some options beginned"
				" with '-xcmd,', see below for details.\n"
				"\tThe execution of the program may entail nested instances of"
				" execution initiated by the command being executed. Such"
				" instances can be grouped by sessions. Each session shares the"
				" same initial time point of the execution named the session"
				" epoch, which determines the base time point used in the"
				" logging messages. The initial instance and any non-nested"
				" instances have their epochs independently to other instances."
				"\n"
				"The session epoch for each instance is currently configured by"
				" an environment variable (see below). If its value is"
				" '0', the instance is the initial one and it will maintain"
				" the value for their nested instance by replace the value"
				" before calling the commands for the nested instances. The"
				" concrete format of the value is otherwise unspecified (and it"
				" can depend on the version of this program), except that any"
				" maintained value is guraranteed not empty. So, set it to '0'"
				" or empty, or unset the variable before the execution of this"
				" program, to ensure it independent to other instances (i.e. in"
				" a different session).\n"
				"\n[ENV ...]\n\tThe environment variables settings in the"
				" shell. (Not all shells support this syntax, but the"
				" environment variables are still effective.)\n"
				"\tThere are no checks on the values. Any behaviors depending"
				" on the locale-specific values are unspecified.\n"
				"\tCurrently accepted settings are:\n\n",
				prog.c_str(), prog.c_str()).c_str());
			for(const auto& env : DeEnvs)
				StreamPut(os, sfmt("  %s\n\t%s Default value is %s.\n\n", env[0],
					env[2], env[1][0] == '\0' ? "empty"
					: Quote(string(env[1])).c_str()).c_str());
			ystdex::write_literal(os,
				"SRCPATH\n\tThe source path. It is handled if and only if this"
				" program runs in the building mode. In this case, SRCPATH is"
				" the 1st command line argument not recognized as an option"
				" (see below). Otherwise, the command line argument is treated"
				" as an option.\n\tSRCPATH shall specify a path to a directory"
				" to be recursively searched. A subdirectory thereof will be"
				" ignored implicitly in the search if its name begins with a"
				" dot ('.') character. A subdirectory whose name is same to"
				" one of the names specified by '-xid,' option (see below) will"
				" also be ignored.\n\n"
				"OPTIONS...\nOPTIONS... -- [ARGS...]\n\tThe options and"
				" arguments for the tool execution. After '--', if any, options"
				" parsing is turned off and every remained command line"
				" argument is interpreted as an argument.\n\tRecognized options"
				" are handled in this program, and the remained arguments will"
				" either be the arguments to the command specified in the"
				" options in the command requesting mode, or as options come"
				" after values of the environment variable SHBuild_CFLAGS or"
				" SHBuild_CXXFLAGS and a single space character when CC or CXX"
				" is called in the building mode, respectively. In the"
				" building mode, all command line arguments except SRCPATH and"
				" the recoginzed options as well as the (prefixed) values"
				" specified by the environment variables SHBuild_CFLAGS or"
				" SHBuild_CXXFLAGS will be sent to the building backends.\n\t"
				"The recognized options are:\n\n");
			for(const auto& opt : OptionsTable)
			{
				StreamPut(os,
					sfmt("  %s%s\n", opt.prefix, opt.option_arg).c_str());
				for(const auto& des : opt.option_details)
					StreamPut(os, sfmt("\t%s\n", des).c_str());
				os << '\n';
			}
		}
	}, {}, Err, [](const std::exception& e, RecordLevel lv){
		YSLib::ExtractException(
			[lv](const char* str, size_t level) YB_NONNULL(2){
			const auto print([=](const std::string& s){
				PrintInfo(std::string(level, ' ') + s, lv, LogGroup::General);
			});

			TryExpr(throw)
			CatchExpr(IntException& ex, print("IntException: "
				+ to_string(unsigned(ex)) + '.'))
			CatchExpr(std::system_error&, print(
				"ERROR: System error (possible file operation failure)."))
			CatchIgnore(std::exception&)
			CatchExpr(..., YAssert(false, "Invalid exception found."))
			print(std::string("ERROR: ") + str);
		}, e);
	}) ? EXIT_FAILURE : EXIT_SUCCESS;
}

