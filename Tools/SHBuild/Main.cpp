/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Main.cpp
\ingroup MaintenanceTools
\brief 递归查找源文件并编译和静态链接。
\version r2980
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-06 14:33:55 +0800
\par 修改时间:
	2015-08-16 00:12 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Tools::SHBuild::Main

This is a command line tool to build project files simply.
See readme file for details.
*/


#include <ysbuild.h>
#include YFM_YSLib_Core_YStorage // for YSLib::FetchStaticRef
#include YFM_YSLib_Service_YTimer // for YSLib::Timers::FetchElapsed;
#include YFM_YSLib_Service_FileSystem
#include <ystdex/mixin.hpp>
#include YFM_YCLib_Host // for platform_ex::EncodeArg, platform_ex::DecodeArg,
//	platform_ex::Terminal;
#include <ystdex/concurrency.h> // for ystdex::task_pool;
#include <ystdex/exception.h> // for ystdex::raise_exception;
#include YFM_NPL_Dependency // for NPL::DecomposeMakefileDepList,
//	NPL::FilterMakefileDependencies;
#include YFM_YSLib_Core_YConsole

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
//@{
using IntException = ystdex::wrap_mixin_t<std::runtime_error, int>;
using ystdex::raise_exception;
//! \since build 539
template<typename... _tParams>
YB_NORETURN inline PDefH(void, raise_exception, int ret, _tParams&&... args)
	ImplExpr(raise_exception<IntException>({
		std::runtime_error(yforward(args)...), ret}))
//@}
//! \since build 522
YB_NORETURN inline PDefH(void, raise_exception, int ret)
	ImplExpr(raise_exception(ret, "Failed calling command."))

//! \since build 547
//@{
enum class LogGroup : yimpl(size_t)
{
	General,
	Search,
	Build,
	Command,
	DepsCheck,
	Max
};

//! \since build 592
enum class BuildMode : yimpl(size_t)
{
	AR = 1,
	LD = 2
};

using opt_uint = unsigned long;
LogGroup LastLogGroup(LogGroup::General);
std::mutex LastLogGroupMutex;
std::bitset<size_t(LogGroup::Max)> LogDisabled;

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
//@}

//! \since build 547
//@{
#define OPT_des_mul "Multiple occurrence is allowed."
#define OPT_des_last "If this option occurs more than once, only the last one" \
	" is effective."
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
					print("Warning: Value '" + val + "' of " + name
						+ " out of range.");
			}
			CatchExpr(std::invalid_argument&,
				print("Warning: Value '" + val + "' of " + name
					+ " is invalid."))
			CatchExpr(std::out_of_range&,
				print("Warning: Value '" + val + "' of " + name
					+ " out of range."))
		}, il)
	{}

	PDefHOp(bool, (), const string& arg) const
		ImplRet(filter(arg))
} OptionsTable[]{
	{"-xd,", "output directory", "DIR_NAME", [](string&& val){
		PrintInfo("Output directory is switched to '" + val
			+ "'.");
		OutputDir = std::move(val);
	}, {"The name of output directory. Default value is '" OPT_build_path "'.",
		OPT_des_mul}},
	{"-xid,", "ignored directories", "DIR_NAME", [](string&& val){
		PrintInfo("Subdirectory '" + val + "' should be ignored.");
		IgnoredDirs.emplace(std::move(val));
	}, {"The name of subdirectory which should be ignored when scanning.",
		OPT_des_mul}},
	{"-xj,", "job max count", "MAX_JOB_COUNT", [](opt_uint uval){
		PrintInfo("Set job max count = " + to_string(uval) + ".");
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
		PrintInfo("Log group disabled: " + to_string(uval) + ".");
	}, opt_uint(LogGroup::Max), {"The log group should be disabled.",
		"Currently these groups are supported:", "  0: General;",
		"  1: Search;", "  2: Build;", "  3: Command;", "  4: DepsCheck.",
		"Except for group 4, other groups are enabled by default.",
		OPT_des_last}},
	{"-xlogge,", "enabled log group", "LOG_GROUP", [](opt_uint uval){
		LogDisabled.set(uval, {});
		PrintInfo("Log group enabled: " + to_string(uval) + ".");
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
			PrintInfo("Ignored unsupported mode '" + to_string(uval) + "'.",
				Warning);
		}
	}, 0x3UL, {"The target action mode.",
		"Value '1' represents call of AR for the final target, and '2' is LD."
		" Other value is reserved and to be ignored."
		" Default value is '1'.",
		"If this option occurs more than once, only the last one is"
		" effective."}},
	{"-xn,", "Target name", "OBJ_NAME", [](string&& val){
		PrintInfo("Target name is switched to '" + val
			+ "'.");
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
		PrintInfo("Checking output directory: '" + opath + "' ...");
		EnsureDirectory(opath);
	}
	CatchExpr(std::system_error&,
		raise_exception(2, "Failed creating directory '" + opath + "'."))
}

//! \since build 545
//@{
nanoseconds
CheckModification(const string& path)
{
	const auto print(std::bind(PrintInfo, _1, Debug, LogGroup::DepsCheck));

	print("Checking path '" + path + "' ...");

	const auto& file_time(GetFileModificationTimeOf(path));

	print("Modification time: " + to_string(file_time.count()) + " .");
	return file_time;
}

//! \since build 560
PDefH(bool, CompareModification, const string& ipath, const nanoseconds& omod)
	ImplRet(omod >= CheckModification(ipath))

template<typename _func>
bool
CheckBuild(_func f, const string& opath)
{
	const auto print(std::bind(PrintInfo, _1, _2, LogGroup::Build));

	try
	{
		if(f())
		{
			print("Output '"+ opath + "' is up-to-date, skipped.", Informative);
			return {};
		}
	}
	CatchExpr(FileOperationFailure& e, print(e.what(), Debug))
	return true;
}
bool
CheckBuild(const vector<string>& ipaths, const string& opath)
{
	return CheckBuild([&]{
		return ipaths.empty() ? false
			: std::all_of(ipaths.cbegin(), ipaths.cend(),
			std::bind(CompareModification, _1, CheckModification(opath)));
	}, opath);
}
//@}

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
		ImplExpr(PrintInfo(cmd, Debug, LogGroup::Command),
			CheckResult(Call(cmd, n)))

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

	string
	GetCommand(const String&) const;
	//! \since build 596
	static string
	GetCommandType(const String&);

	//! \since build 596
	string
	LookupCommand(const string&) const;
};

string
Rule::GetCommand(const String& ext) const
{
	return LookupCommand(GetCommandType(ext));
}
string
Rule::GetCommandType(const String& ext)
{
	if(ext == u"c")
		return "CC";
	if(ext == u"cc" || ext == u"cpp" || ext == u"cxx")
		return "CXX";
	return {};
}
string
Rule::LookupCommand(const string& name) const
{
	return name.empty() ? string() : Context.GetEnv(name);
}


//! \since build 546
namespace
{

Value
BuildFile(const Rule& rule)
{
	const auto& bctx(rule.Context);
	const auto& ipth(rule.Source.first);
	const auto& cmd_type(rule.GetCommandType(GetExtensionOf(ipth)));
	const auto& cmd(rule.LookupCommand(cmd_type));
	const auto& fullname(ipth.GetMBCS());
	const auto print(std::bind(PrintInfo, _1, _2, LogGroup::Build));

	if(!cmd.empty())
	{
		const auto& ofullname(rule.Source.second.GetMBCS());
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
			print("Compile file: '" + ipth.back().GetMBCS() + "'.",
				Informative);
			bctx.CallWithException(cmd + " -MMD" + " -c " + bctx.GetFlags(
				cmd_type) + " \"" + fullname + "\" -o \"" + ofullname + '"');
		}
		return {ofullname};
	}
	else
		print("No rule found for '" + fullname + "'.", Warning);
	return {};
}

Value
SearchDirectory(const Rule& rule, const ActionContext& actx)
{
	const auto& ipth(rule.Source.first);
	const auto& opth(rule.Source.second);
	const auto& path(ipth.GetMBCS());
	vector<string> subdirs, ofiles;
	vector<pair<string, string>> src_files;
	const auto print(std::bind(PrintInfo, _1, _2, LogGroup::Search));

	print("Searching path: " + path + " ...", Notice);
	TraverseChildren(path, [&](NodeCategory c, const std::string& name){
		if(name[0] != '.')
		{
			if(bool(c & NodeCategory::Directory))
			{
				if(ystdex::exists(rule.Context.IgnoredDirs, name))
					print("Subdirectory " + path + name + YCL_PATH_DELIMITER
						+ " is ignored.", Informative);
				else
					subdirs.push_back(name);
			}
			else
			{
				auto cmd(rule.GetCommand(
					GetExtensionOf(String(name, CS_Path))));

				if(!cmd.empty())
					src_files.emplace_back(std::move(cmd), name);
				else
					print("Ignored non source file '" + name + "'.",
						Informative);
			}
		}
	});
	for(const auto& name : subdirs)
		ystdex::vector_concat(ofiles,
			actx(make_pair(ipth / name, opth / name)));

	const auto snum(src_files.size());

	print(to_string(snum) + " file(s) found to be built in path: " + path
		+ " .", Informative);
	if(snum != 0)
	{
		EnsureOutputDirectory(opth);
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
		+ to_string(jobs.get_max_task_num()) + ".");
	OutputDir = NormalizeDirectoryPathTail(OutputDir);
	PrintInfo("Normalized output directory: '" + OutputDir + "'.");
	if(Options.empty())
	{
		PrintInfo("No options found. Stop.");
		return;
	}

	const auto in(NormalizeDirectoryPathTail(Options[0]));
	const auto ipath(MakeNormalizedAbsolute(Path(in)));

	PrintInfo("Absolute path recognized: " + to_string(ipath).GetMBCS() + " .");
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
		PrintInfo("Wait for unfinished tasks before linking ...",
			Notice, LogGroup::Build);
		jobs.reset();
	}
	CheckResult(GetLastResult());

	const auto onum(ofiles.size());
	const auto print(std::bind(PrintInfo, _1, _2, LogGroup::Build));

	print("Found " + to_string(onum) + " .o file(s).", Informative);
	if(onum != 0)
	{
		Path pth(opth);

		YAssert(!pth.empty(), "Invalid path found.");
		pth.pop_back();
		EnsureOutputDirectory(pth);

		auto target(to_string(opth).GetMBCS(CS_Path));
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
			auto str(cmd + " \"" + target + '"');

			// FIXME: Prevent path too long.
			for(const auto& ofile : ofiles)
				str += " \"" + ofile + "\"";
			if(Mode == BuildMode::LD)
				str += ' ' + GetEnv("LIBS");
			print("Link file: '" + target + "'.", Informative);
			if(Mode == BuildMode::AR)
			{
				// NOTE: Since the return value might be implementation-defined
				//	and the next operations might be still meaningful, it is
				//	not intended to throw an exception.
				if(uremove(target.c_str()) != 0)
					PrintInfo(u8"Failed deleting file '" + target + u8"'.",
						Warning);
				PrintInfo(u8"Deleted file '" + target + u8"'.", Debug);
			}
			CallWithException(str, 1);
		}
	}
	else
		print("No files to be built.", Warning);
}

int
BuildContext::Call(const string& cmd, size_t n) const
{
	if(n == 0)
		n = jobs.get_max_task_num();
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
	// FIXME: Correct handling of result value.
	jobs.wait([&, cmd]{
		const int res(usystem(cmd.c_str()));
		{
			std::lock_guard<std::mutex> lck(job_mtx);

			if(result == 0)
				result = res;
		}
		return res;
	});
	return 0;
}
//@}


int
main(int argc, char* argv[])
{
	Terminal term, term_err(stderr);

	return FilterExceptions([&]{
		auto& logger(FetchStaticRef<Logger>());

		LogDisabled.set(size_t(LogGroup::DepsCheck));
		yunseq(FetchCommonLogger().FilterLevel = Logger::Level::Informative,
			logger.FilterLevel = Logger::Level::Debug);
		logger.SetFilter([](Logger::Level lv, Logger& l){
			return !ystdex::qualify(LogDisabled)[size_t(LastLogGroup)]
				&& Logger::DefaultFilter(lv, l);
		});
		logger.SetSender([&](Logger::Level lv, Logger&, const char* str){
			const auto stream(lv <= Warning ? stderr : stdout);
			auto& term_ref(lv <= Warning ? term_err : term);
			const auto dcnt(duration_cast<milliseconds>(
				Timers::FetchElapsed<steady_clock>()).count());

			term_ref.RestoreAttributes();
			std::fprintf(stream, "[%04u.%03u][%zu:%#02X]",
				unsigned(dcnt / 1000U), unsigned(dcnt % 1000U),
				size_t(LastLogGroup), unsigned(lv));
			if(term_ref)
			{
				using namespace Consoles;
				static const Logger::Level
					lvs[]{Err, Warning, Notice, Informative, Debug};
				static const Color
					colors[]{Red, Yellow, Cyan, Magenta, DarkGreen};
				const auto i(
					std::lower_bound(&lvs[0], &lvs[arrlen(colors)], lv));

				if(i == &lvs[arrlen(colors)])
					term_ref.RestoreAttributes();
				else
					term_ref.UpdateForeColor(colors[i - lvs]);
			}
			std::fprintf(stream, "%s", &EncodeArg(Nonnull(str))[0]);
			term_ref.RestoreAttributes();
			std::fputc('\n', stream);
		});
		if(argc > 1)
		{
			vector<string> args;

			for(int i(1); i < argc; ++i)
			{
				string arg(DecodeArg(argv[i]));

				if(!arg.empty() && std::none_of(begin(OptionsTable),
					end(OptionsTable), [&](const Option& opt){
						return opt(arg);
				}))
					args.emplace_back(std::move(arg));
			}

			BuildContext ctx(MaxJobs);

			for(const auto& env : DeEnvs)
			{
				const string name(env[0]);

				FetchEnvironmentVariable(ctx.Envs[name], name);
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
		else if(argc == 1)
		{
			std::printf("%s%s%s", "Usage: [ENV ...] \"", *argv,
				"\" SRCPATH [OPTIONS ...]\n"
				"\n[ENV ...]\n\tThe environment variables settings."
				" Currently accepted settings are listed below:\n\n");
			for(const auto& env : DeEnvs)
				std::printf("  %s\n\t%s Default value is %s.\n\n", env[0],
					env[2], env[1][0] == '\0' ? "empty"
					: ('\'' + string(env[1]) + '\'').c_str());
			std::puts("SRCPATH\n\tThe source directory to be recursively"
				" searched.\n\n"
				"OPTIONS ...\n"
				"\tThe options. This comes after values of environment"
				" variables SHBuild_CFLAGS or SHBuild_FLAGS and one single"
				" space character when CC or CXX is called, respectively. All"
				" options including these prefixed values of SHBuild_CFLAGS or"
				" SHBuild_FLAGS would be sent to the backends, except for"
				" listed below:\n");
			for(const auto& opt : OptionsTable)
			{
				std::printf("  %s%s\n", opt.prefix, opt.option_arg);
				for(const auto& des : opt.option_details)
					std::printf("\t%s\n", des);
				std::putchar('\n');
			}
		}
	}, {}, Err, [](const std::exception& e, LoggedEvent::LevelType l){
		return ExtractException([](const char* str, LoggedEvent::LevelType lv,
			size_t level){
			const auto print([=](const string& s){
				PrintInfo(string(level, ' ') + s, lv, LogGroup::General);
			});

			TryExpr(throw)
			CatchExpr(IntException& ex, print("IntException: "
				+ to_string(unsigned(ex)) + "."))
			CatchExpr(FileOperationFailure&, print(
				"ERROR: File operation failure."))
			CatchIgnore(std::exception&)
			CatchExpr(..., YAssert(false, "Invalid exception found."))
			print(string("ERROR: ") + str);
		}, e, l, 0);
	}) ? EXIT_FAILURE : EXIT_SUCCESS;
}

