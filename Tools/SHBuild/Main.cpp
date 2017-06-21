/*
	© 2014-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Main.cpp
\ingroup MaintenanceTools
\brief 宿主构建工具：递归查找源文件并编译和静态链接。
\version r3572
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-06 14:33:55 +0800
\par 修改时间:
	2017-06-19 19:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Tools::SHBuild::Main

This is a command line tool to build project files simply.
See readme file for details.
*/


#include <YSBuild.h>
#include YFM_YSLib_Core_YStorage // for YSLib::FetchStaticRef,
//	ystdex::raise_exception;
#include YFM_YSLib_Service_YTimer // for YSLib::Timers::FetchElapsed;
#include YFM_YSLib_Service_FileSystem
#include <ystdex/mixin.hpp> // for ystdex::wrap_mixin_t;
#include YFM_NPL_Dependency // for NPL::DepsEventType,
//	NPL::DecomposeMakefileDepList, NPL::FilterMakefileDependencies,
//	NPL::Install*;
#include YFM_YSLib_Core_YConsole // for YSLib::Consoles;
#include <ystdex/concurrency.h> // for ystdex::task_pool;
#include YFM_YCLib_Host // for platform_ex::EncodeArg, platform_ex::DecodeArg,
//	platform_ex::Terminal, platform_ex::SetEnvironmentVariable;

using namespace YSLib;
using namespace IO;
//! \since build 547
//@{
using namespace platform_ex;
using namespace std::chrono;
using namespace std::placeholders;
//@}

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

//! \since build 648
using ystdex::quote;
//! \since build 648
template<class _tString>
auto
Quote(_tString&& str) -> decltype(quote(yforward(str), '\''))
{
	return quote(yforward(str), '\'');
}

void
PrintInfo(const string& line, RecordLevel lv = Notice,
	LogGroup grp = LogGroup::General)
{
	std::lock_guard<std::mutex> lck(LastLogGroupMutex);

	LastLogGroup = grp;
	FetchStaticRef<Logger>().Log(lv, [&]{
		return line;
	});
}


#define OPT_des_mul "Multiple occurrence is allowed."
#define OPT_des_last \
	"If this option occurs more than once, only the last one is effective."
string RequestedCommand;
set<string> IgnoredDirs;
string OutputDir;
size_t MaxJobs(0);
//! \since build 592
BuildMode Mode(BuildMode::AR);
//! \since build 556
string TargetName;
const struct Option
{
	const char *prefix, *name = {}, *option_arg;
	std::initializer_list<const char*> option_details;
	std::function<bool(const string&)> filter;

	Option(const char* pfx, const char* n, const char* opt_arg,
		std::function<void(string&&)> parse,
		std::initializer_list<const char*> il)
		: prefix(pfx), name(n), option_arg(opt_arg), option_details(il),
		filter(std::bind(ystdex::filter_prefix<string, string, decltype(parse)>,
		_1, string(prefix), parse))
	{}
	template<typename _type, typename _func>
	Option(const char* pfx, const char* n, const char* opt_arg, _func f,
		_type threshold, std::initializer_list<const char*> il)
		: Option(pfx, n, opt_arg, [=](string&& val){
			const auto
				print(std::bind(PrintInfo, _1, Warning, LogGroup::General));

			try
			{
				const auto uval(ystdex::ston<_type>(val));

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
	{"-xd,", "output directory", "DIR_NAME", [](string&& val){
		PrintInfo("Output directory is switched to " + Quote(val) + '.');
		OutputDir = std::move(val);
	}, {"The name of output directory. Default value is '" OPT_build_path "'.",
		OPT_des_mul}},
	{"-xid,", "ignored directories", "DIR_NAME", [](string&& val){
		PrintInfo("Subdirectory " + Quote(val) + " should be ignored.");
		IgnoredDirs.emplace(std::move(val));
	}, {"The name of subdirectory which should be ignored when scanning.",
		OPT_des_mul}},
	{"-xcmd,", "command", "COMMAND", [](string&& val) ynothrow{
		RequestedCommand = std::move(val);
	}, {"Specified name of a command to run.", "If this option is set, all"
		" other parameters not recognized as options are treated as parameters"
		" of the command. Currently the following COMMAND name and parameters"
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
		" complier is called."}}
};
//@}

//! \since build 541
void
EnsureOutputDirectory(const string& opath)
{
	try
	{
		PrintInfo("Checking output directory: " + Quote(opath) + " ...");
		EnsureDirectory(opath);
	}
	CatchExpr(std::system_error&,
		raise_exception(2, "Failed creating directory " + Quote(opath) + '.'))
}

//! \since build 545
//@{
nanoseconds
CheckModification(const string& path)
{
	const auto print(std::bind(PrintInfo, _1, Debug, LogGroup::DepsCheck));

	print("Checking path " + Quote(path) + " ...");

	const auto& file_time(GetFileModificationTimeOf(path.c_str()));

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
CheckedLoad(const char* name, std::istream& is, NPL::A1::REPLContext& context)
{
	TryExpr(context.LoadFrom(is))
	CatchExpr(..., std::throw_with_nested(NPL::NPLException(
		ystdex::sfmt("Failed loading external unit '%s'.", name))));
}

//! \since build 797
ArgumentsVector CommandArguments;

//! \since build 796
YB_NONNULL(1) void
RunNPLFromStream(const char* name, std::istream&& is)
{
	using namespace NPL;
	using namespace A1;
	using namespace Forms;
	REPLContext context;
	auto& root(context.Root);

	LoadNPLContextForSHBuild(context);
	RegisterStrictBinary<const string>(root, "env-set",
		[&](const string& var, const string& val){
		SetEnvironmentVariable(var.c_str(), val.c_str());
	});
	RegisterStrict(root, "cmd-get-args", [](TermNode& term){
		RetainN(term, 0);
		term.Clear();
		for(const auto& s : CommandArguments.Arguments)
			term.AddValue(MakeIndex(term), s);
		return ReductionStatus::Retained;
	});
	RegisterStrict(root, "system-get", [](TermNode& term){
		CallUnaryAs<const string>([&](const string& cmd){
			auto res(FetchCommandOutput(cmd.c_str()));

			term.Clear();
			term.AddValue(MakeIndex(0), ystdex::trim(std::move(res.first)));
			term.AddValue(MakeIndex(1), res.second);
		}, term);
		return ReductionStatus::Retained;
	});
	RegisterStrictUnary<const string>(root, "load", [&](const string& src){
		platform::ifstream ifs(src, std::ios_base::in);

		CheckedLoad(src.c_str(), ifs, context);
	});
	// XXX: Overriding.
	root.GetRecordRef().Define("SHBuild_BaseTerminalHook_",
		ValueObject(std::function<void(const string&, const string&)>(
		[](const string& n, const string& val){
			// XXX: Errors from 'std::printf' and 'std::puts' are ignored.
			using namespace Consoles;

			Terminal te;
			{
				const auto gd(te.LockForeColor(DarkCyan));

				std::printf("%s", n.c_str());
			}
			std::printf(" = \"");
			{
				const auto gd(te.LockForeColor(DarkRed));

				std::printf("%s", val.c_str());
			}
			std::puts("\"");
	})), true);
	CheckedLoad(name, is, context);
}

} // unnamed namespace;


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
	//! \since build 547
	map<string, string> Envs;
	//! \since build 592
	BuildMode Mode = BuildMode::AR;
	//! \since build 556
	string TargetName{};

	BuildContext(size_t n)
		: jobs(n)
	{
		for(const auto& env : DeEnvs)
			Envs.insert({env[0], env[1]});
	}

	//! \since build 545
	DefGetter(const ynothrow, const string&, Flags, flags)
	//! \since build 596
	string
	GetFlags(const string&) const;

	//! \since build 538
	int
	GetLastResult() const;
	//! \since build 547
	PDefH(const string&, GetEnv, const string& name) const
		ImplRet(Envs.at(name))

	void
	Build();

	//! \since build 540
	int
	Call(const string&, size_t n = 0) const;

	//! \since build 539
	//@{
	PDefH(void, CallWithException, const string& cmd, size_t n = 0) const
		ImplExpr(CheckResult(Call(cmd, n)))

	static PDefH(void, CheckResult, int ret)
		ImplExpr(ret == 0 ? void() : raise_exception(ret))
	//@}

	//! \since build 540
	int
	RunTask(const string&) const;
};


//! \since build 545
//@{
using Key = pair<Path, Path>;
using Value = vector<string>;
using ActionContext = GRecursiveCallContext<Key, Value>;
using BuildAction = ActionContext::CallerType;

class Rule
{
public:
	BuildContext& Context;
	Key Source;

	//! \since build 648
	//@{
	PDefH(string, GetCommand, string_view ext) const
		ImplRet(LookupCommand(GetCommandType(ext)))
	static string
	GetCommandType(string_view);

	string
	LookupCommand(string_view) const;
	//@}
};

string
Rule::GetCommandType(string_view ext)
{
	if(ext == "c")
		return "CC";
	if(ext == "cc" || ext == "cpp" || ext == "cxx")
		return "CXX";
	return {};
}
string
Rule::LookupCommand(string_view name) const
{
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
	const auto& cmd_type(rule.GetCommandType(GetExtensionOf(fullname)));
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
			if(ifstream tf{dfullname, std::ios_base::in})
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
	const auto& ipth(rule.Source.first);
	const auto& opth(rule.Source.second);
	const auto& path(ipth.VerifyAsMBCS());
	vector<string> subdirs, ofiles;
	vector<pair<string, string>> src_files;
	const auto print(std::bind(PrintInfo, _1, _2, LogGroup::Search));

	print("Searching path: " + Quote(path) + " ...", Notice);
	TraverseChildren(path, [&](NodeCategory c, NativePathView npv){
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
				auto cmd(rule.GetCommand(GetExtensionOf(name)));

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

	print(to_string(snum) + " file(s) found to be built in path " + Quote(path)
		+ '.', Informative);
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
BuildContext::GetFlags(const string& cmd_type) const
{
	string res;

	if(cmd_type == "CC")
		res = GetEnv("SHBuild_CFLAGS");
	else if(cmd_type == "CXX")
		res = GetEnv("SHBuild_CXXFLAGS");
	return res + ' ' + flags;
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
	OutputDir = NormalizeDirectoryPathTail(OutputDir);
	PrintInfo("Normalized output directory: " + Quote(OutputDir) + '.');
	if(Options.empty())
	{
		PrintInfo("No options found. Stop.");
		return;
	}

	const auto in(NormalizeDirectoryPathTail(Options[0]));
	const auto ipath(MakeNormalizedAbsolute(Path(in)));

	PrintInfo("Absolute path " + Quote(to_string(ipath).GetMBCS()) + " recognized.");
	if(!VerifyDirectory(in))
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
			print("Wait for unfinished tasks before linking ...", Notice);
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
			// FIXME: Find extension properly.
			else
				// FIXME: Parse %LDFLAGS.
// FIXME: Support cross compiling.
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
					if(uremove(target.c_str()) != 0)
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

			print("Wating unfinished tasks ...", Warning);
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
BuildContext::Call(const string& cmd, size_t n) const
{
	if(n == 0)
		n = jobs.get_max_task_num();
	PrintInfo(n <= 1 ? cmd : "Task enqueued.", Debug, LogGroup::Command);
	return n <= 1 ? usystem(cmd.c_str()) : RunTask(cmd);
}

int
BuildContext::RunTask(const string& cmd) const
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
		PrintInfo(cmd, Debug, LogGroup::Command);
		const int res(usystem(cmd.c_str()));
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


int
main(int argc, char* argv[])
{
	Terminal te, te_err(stderr);

	return FilterExceptions([&]{
		auto& logger(FetchStaticRef<Logger>());

		LogDisabled.set(size_t(LogGroup::DepsCheck));
		yunseq(FetchCommonLogger().FilterLevel = Logger::Level::Informative,
			logger.FilterLevel = Logger::Level::Debug);
		logger.SetFilter([](Logger::Level lv, Logger& l){
			return !ystdex::qualify(LogDisabled)[size_t(LastLogGroup)]
				&& Logger::DefaultFilter(lv, l);
		});
		logger.SetSender([&](Logger::Level lv, Logger&, const char* str)
			YB_NONNULL(4){
			const auto stream(lv <= Warning ? stderr : stdout);
			auto& term_ref(lv <= Warning ? te_err : te);
			const auto dcnt(duration_cast<milliseconds>(
				Timers::FetchElapsed<steady_clock>()).count());

			term_ref.RestoreAttributes();
			std::fprintf(stream, "[%04u.%03u][%zu:%#02X]",
				unsigned(dcnt / 1000U), unsigned(dcnt % 1000U),
				size_t(LastLogGroup), unsigned(lv));
			{
				Terminal::Guard
					guard(term_ref, std::bind(UpdateForeColorByLevel, _1, lv));

				std::fprintf(stream, "%s", &EncodeArg(Nonnull(str))[0]);
			}
			std::fputc('\n', stream);
		});
		if(argc > 1)
		{
			vector<string> args;
			bool opt_trans(true);

			for(int i(1); i < argc; ++i)
			{
				string arg(DecodeArg(argv[i]));

				if(opt_trans && string(argv[i]) == "--")
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
						throw std::runtime_error(sfmt("Wrong number %zu of"
							" arguments (should be %zu) found.", sz, n));
				});
				const auto check_n_ge([sz](size_t n){
					if(sz < n)
						throw std::runtime_error(sfmt("Wrong number %zu of"
							" arguments (should at least be %zu) found.", sz,
							n));
				});

				try
				{
					using namespace NPL;

					if(RequestedCommand == "EnsureDirectory")
					{
						check_n(1);
						EnsureDirectory(args[0]);
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
					else if(RequestedCommand == "RunNPL")
					{
						check_n(1);
						RunNPLFromStream("<stdin>",
							std::istringstream(args[0]));
					}
					else if(RequestedCommand == "RunNPLFile")
					{
						check_n_ge(1);

						const auto name(args[0]);

						CommandArguments.Arguments = std::move(args);
						RunNPLFromStream(name.c_str(),
							ifstream{name, std::ios_base::in});
					}
					else
						throw std::runtime_error(sfmt("Specified command name"
							" '%s' not supported.", RequestedCommand.c_str()));
				}
				CatchExpr(std::exception& e,
					std::throw_with_nested(std::move(e)))
			}
			else
			{
				BuildContext ctx(MaxJobs);

				for(const auto& env : DeEnvs)
				{
					const string name(env[0]);

					FetchEnvironmentVariable(ctx.Envs[name], env[0]);
					PrintInfo(name + " = " + ctx.GetEnv(name));
				}
				if(!OutputDir.empty())
					ctx.OutputDir = std::move(OutputDir);
				yunseq(ctx.IgnoredDirs = std::move(IgnoredDirs),
					ctx.Options = std::move(args), ctx.Mode = Mode);
				if(!TargetName.empty())
					ctx.TargetName = std::move(TargetName);
				PrintInfo("OutputDir = " + ctx.OutputDir);
				ctx.Build();
			}
		}
		else if(argc == 1)
		{
			std::printf("%s%s%s", "Usage: [ENV ...] ",
				quote(string(*argv)).c_str(),
				" SRCPATH [OPTIONS ... [-- ARGS...]]\n"
				"\tThis program is a tool to build the source tree, with some"
				" additional functionalities. It has two execution mode,"
				" building mode and command requesting mode, exclusively. In"
				" the former mode, tools for compiling (called building"
				" backends) are called. The latter is only enabled when"
				" provided some command introduced by specific options, see"
				" below for details.\n"
				"\n[ENV ...]\n\tThe environment variables settings in shell."
				" (Note not all shells support this syntax, but the"
				" environment variables are still effective.)"
				" Currently accepted settings are listed below:\n\n");
			for(const auto& env : DeEnvs)
				std::printf("  %s\n\t%s Default value is %s.\n\n", env[0],
					env[2], env[1][0] == '\0' ? "empty"
					: Quote(string(env[1])).c_str());
			std::puts("SRCPATH\n\tThe source directory to be recursively"
				" searched.\n\nOPTIONS... [-- ARGS...]\n"
				"\tThe options and arguments. After '--', if any, options"
				" parsing is turned off and every remained token is interpreted"
				" as an argument. Recognized options are handled by this"
				" program, and the remained arguments would either be the"
				" argument of the options when the commands are requested, or"
				" as options come after values of environment variable"
				" SHBuild_CFLAGS or SHBuild_CXXFLAGS and a single space"
				" character when CC or CXX is called in the building mode,"
				" respectively. In the building mode, all options including"
				" these prefixed values of SHBuild_CFLAGS or SHBuild_CXXFLAGS"
				" would be sent to the building backends, except for listed"
				" below (handled by this program):\n");
			for(const auto& opt : OptionsTable)
			{
				std::printf("  %s%s\n", opt.prefix, opt.option_arg);
				for(const auto& des : opt.option_details)
					std::printf("\t%s\n", des);
				std::putchar('\n');
			}
		}
	}, {}, Err, [](const std::exception& e, RecordLevel l){
		ExtractException([](const char* str, RecordLevel lv,
			size_t level) YB_NONNULL(1){
			const auto print([=](const string& s){
				PrintInfo(string(level, ' ') + s, lv, LogGroup::General);
			});

			TryExpr(throw)
			CatchExpr(IntException& ex, print("IntException: "
				+ to_string(unsigned(ex)) + '.'))
			CatchExpr(std::system_error&, print(
				"ERROR: System error (possible file operation failure)."))
			CatchIgnore(std::exception&)
			CatchExpr(..., YAssert(false, "Invalid exception found."))
			print(string("ERROR: ") + str);
		}, e, l, 0);
	}) ? EXIT_FAILURE : EXIT_SUCCESS;
}

