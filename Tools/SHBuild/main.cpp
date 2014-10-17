/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file main.cpp
\ingroup MaintenanceTools
\brief 递归查找源文件并编译和静态链接。
\version r2249
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-06 14:33:55 +0800
\par 修改时间:
	2014-10-17 16:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Tools::SHBuild::Main

This is a command line tool to build project files simply.
See readme file for details.
*/


#include <ysbuild.h>
#include YFM_YSLib_Service_FileSystem
#include <ystdex/mixin.hpp>
#include YFM_MinGW32_YCLib_Consoles // for platform_ex::WConsole;
#include <ystdex/concurrency.h> // for ystdex::task_pool;
#include <ystdex/exception.hpp> // for ystdex::raise_exception;
#include YFM_NPL_SContext

using namespace YSLib;
using namespace IO;

namespace
{

/*!
\brief 默认构建根目录路径。
\since build 540
*/
#define OPT_build_path ".shbuild"

/*!
\brief 选项前缀：输出路径。
\since build 540
*/
#define OPT_pfx_xd "-xd,"

/*!
\brief 选项前缀：扫描时忽略的目录(ignore directory) 。
\since build 520
*/
#define OPT_pfx_xid "-xid,"

/*!
\brief 选项前缀：最大并行任务(jobs) 数。
\since build 520
*/
#define OPT_pfx_xj "-xj,"

/*!
\brief 选项前缀：过滤输出日志的等级(log filter level) 。
\since build 519
*/
#define OPT_pfx_xlogfl "-xlogfl,"

/*!
\brief 选项前缀：动作模式(mode) 。
\note 默认值为 1 ，表示调用 AR 。值 2 表示调用 \c LD 。
\since build 546
*/
#define OPT_pfx_xmode "-xmode,"


//! \since build 477
//@{
using IntException = ystdex::wrap_mixin_t<std::runtime_error, int>;
using ystdex::raise_exception;
//! \since build 539
template<typename... _tParams>
YB_NORETURN inline PDefH(void, raise_exception, int ret, _tParams&&... args)
	ImplExpr(raise_exception<IntException>({
		std::runtime_error(yforward(args)...), ret}))
//! \since build 522
YB_NORETURN inline PDefH(void, raise_exception, int ret)
	ImplExpr(raise_exception(ret, "Failed calling command."))

//! \since build 538
void
PrintInfo(const string& line, RecordLevel lv = Notice)
{
	FetchCommonLogger().Log(lv, [&]{
		return line;
	});
}


void
PrintException(const std::exception& e, size_t level = 0)
{
	try
	{
		PrintInfo(string(level, ' ') + "ERROR: " + e.what(), Err);
		throw;
	}
	catch(IntException& e)
	{
		PrintInfo("IntException: " + to_string(unsigned(e)) + ".", Err);
	}
	catch(FileOperationFailure& e)
	{
		PrintInfo("ERROR: File operation failure.", Err);
	}
	catch(std::exception& e)
	{}
	catch(...)
	{
		PrintInfo("ERROR: PrintException.", Err);
	}
	ystdex::handle_nested(e, [=](std::exception& e){
		PrintException(e, level + 1);
	});
}
//@}


//! \since build 542
//@{
template<typename _type, typename _func>
void
TryParseOption(const string& name, const string& val, _func f)
{
	try
	{
		const auto uval(ystdex::ston<_type>(val));

		if(!f(uval))
			PrintInfo("Warning: Value '" + val + "' of " + name
				+ " out of range.", Warning);
	}
	catch(std::invalid_argument&)
	{
		PrintInfo("Warning: Value '" + val + "' of " + name + " is invalid.",
			Warning);
	}
	catch(std::out_of_range&)
	{
		PrintInfo("Warning: Value '" + val + "' of " + name + " out of range.",
			Warning);
	}
}

template<typename _type, typename _fCallable, typename _tPrefix>
bool
ParsePrefixedOption(const string& str, const _tPrefix& prefix,
	const string& name, _type threshold, _fCallable f)
{
	return ystdex::filter_prefix(str, prefix, [&](string&& val){
		TryParseOption<_type>(name, val, [=](_type uval){
			return uval < threshold ? (void(f(uval)), true) : false;
		});
	});
}
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
	catch(std::system_error& e)
	{
		raise_exception(2, "Failed creating directory '" + opath + "'.");
	}
}

//! \since build 545
//@{
std::chrono::nanoseconds
CheckModification(const string& path)
{
	PrintInfo("Checking path '" + path + "' ...", RecordLevel(Debug + 8));

	const auto& file_time(platform::GetFileModificationTimeOf(path));

	PrintInfo("Modification time: " + to_string(file_time.count()) + " .",
		RecordLevel(Debug + 8));
	return file_time;
}

PDefH(bool, CompareModification, const string& ipath, const string& opath)
	ImplRet(CheckModification(opath) >= CheckModification(ipath))

template<typename _func>
bool
CheckBuild(_func f, const string& opath)
{
	try
	{
		if(f())
		{
			PrintInfo("Output '"+ opath + "' is up-to-date, skipped.",
				Informative);
			return {};
		}
	}
	catch(FileOperationFailure& e)
	{
		PrintInfo(e.what(), Debug);
	}
	return true;
}
bool
CheckBuild(const vector<string>& ipaths, const string& opath)
{
	return CheckBuild([&]{
		return std::all_of(ipaths.cbegin(), ipaths.cend(),
			std::bind(CompareModification, std::placeholders::_1, opath));
	}, opath);
}
//@}

/*!
\brief 分解 \c .d 文件的依赖项。
\since build 546
*/
vector<string>
GetDependencies(const string& path)
{
	using namespace ystdex;
	using namespace NPL;
	TextFile tf(path);
	set<size_t> spaces;
	Session sess(tf, [&](LexicalAnalyzer& lexer, char c){
		lexer.ParseQuoted(c, [&](string& buf, const UnescapeContext& uctx,
			char)->bool{
			const auto& escs(uctx.GetSequence());

			// NOTE: See comments in %munge function of 'mkdeps.c' from libcpp
			//	of GCC.
			if(escs.length() == 1)
			{
				if(uctx.Prefix == "\\")
					switch(escs[0])
					{
					case ' ':
						spaces.insert(buf.size());
					case '\\':
					case '#':
						buf += escs[0];
						return true;
					default:
						;
					}
				if(uctx.Prefix == "$" && escs[0] == '$')
				{
					buf += '$';
					return true;
				}
			}
			return {};
		}, [](char c, string& pfx)->bool{
			if(c == '\\')
				pfx = "\\";
			else if(c == '$')
				pfx = "$";
			else
				return {};
			return true;
		});
	});
	const auto& buf(sess.GetBuffer());
	vector<string> lst;

	ystdex::split_if_iter(buf.begin(), buf.end(), [](char c){
		return std::isspace(c);
	}, [&](string::const_iterator b, string::const_iterator e){
		lst.push_back(string(b, e));
	}, [&](string::const_iterator i){
		return !ystdex::exists(spaces, i - buf.cbegin());
	});

	const auto i_c(std::find_if(lst.cbegin(), lst.cend(), [](const string& dep){
		return !dep.empty() && dep.back() == ':';
	}));

	if(i_c == lst.cend())
		PrintInfo("Wrong dependencies format found.", Warning);
	lst.erase(lst.cbegin(), i_c + 1);
	if(lst.empty())
		PrintInfo("Wrong dependencies format found.", Warning);
	PrintInfo(to_string(lst.size()) + " dependenc"
		+ (lst.size() == 1 ? "y" : "ies") + " found.", Debug);
	return lst;
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

public:
	set<string> IgnoredDirs{};
	//! \since build 540
	string OutputDir{OPT_build_path};
	vector<string> Options{};
	//! \since build 538
	string CC = "gcc";
	//! \since build 538
	string CXX = "g++";
	//! \since build 539
	string AR = "ar";
	//! \since build 546
	//@{
	string ARFLAGS = "rcs";
	string LD = "ld";
	string LDFLAGS = "";
	string LIBS = "";
	size_t Mode = 1;
	//@}

	BuildContext(size_t n)
		: jobs(n)
	{}

	//! \since build 545
	DefGetter(const ynothrow, const string&, Flags, flags)

	//! \since build 538
	int
	GetLastResult() const;

	void
	Build();

	//! \since build 540
	int
	Call(const string&, size_t n = 0) const;

	//! \since build 539
	//@{
	PDefH(void, CallWithException, const string& cmd, size_t n = 0) const
		ImplExpr(PrintInfo(cmd, Debug), CheckResult(Call(cmd, n)))

	static PDefH(void, CheckResult, int ret)
		ImplExpr(ret == 0 ? void() : raise_exception(ret))
	//@}

	//! \since build 540
	int
	RunTask(const string&) const;

	//! \since build 545
	void
	Wait() const;
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
};

string
Rule::GetCommand(const String& ext) const
{
	if(ext == u"c")
		return Context.CC;
	if(ext == u"cc" || ext == u"cpp" || ext == u"cxx")
		return Context.CXX;
	return {};
}


//! \since build 546
namespace
{

Value
BuildFile(const Rule& rule)
{
	const auto& bctx(rule.Context);
	const auto& ipth(rule.Source.first);
	const auto& cmd(rule.GetCommand(GetExtensionOf(ipth)));
	const auto& fullname(ipth.GetMBCS());

	if(!cmd.empty())
	{
		const auto& ofullname(rule.Source.second.GetMBCS());
		bool build{true};

		try
		{
			auto dfullname(ofullname);

			YAssert(!dfullname.empty(), "Invalid output name found.");
			dfullname.back() = 'd';
			if(!CheckBuild(GetDependencies(dfullname), ofullname))
				build = {};
		}
		catch(std::exception&)
		{}
		if(build)
		{
			PrintInfo("Compile file: '" + ipth.back().GetMBCS() + "'.",
				Informative);
			bctx.CallWithException(cmd + " -MMD" + " -c" + bctx.GetFlags() + ' '
				+ fullname + " -o \"" + ofullname + '"');
		}
		return {ofullname};
	}
	else
		PrintInfo("No rule found for '" + fullname + "'.", Warning);
	return {};
}

Value
SearchDirectory(const Rule& rule, const ActionContext& actx)
{
	const auto& bctx(rule.Context);
	const auto& ipth(rule.Source.first);
	const auto& opth(rule.Source.second);
	const auto& path(ipth.GetMBCS());
	vector<string> subdirs, afiles, ofiles;
	vector<pair<string, string>> src_files;

	PrintInfo("Searching path: " + path + " ...");
	TraverseChildren(path, [&](NodeCategory c, const std::string& name){
		if(name[0] != '.')
		{
			if(c == NodeCategory::Directory)
			{
				if(ystdex::exists(bctx.IgnoredDirs, name))
					PrintInfo("Subdirectory " + path + name
						+ YCL_PATH_DELIMITER + " is ignored.", Informative);
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
					PrintInfo("Ignored non source file '" + name + "'.",
						Informative);
			}
		}
	});
	for(const auto& name : subdirs)
		ystdex::vector_concat(ofiles,
			actx(make_pair(ipth / name, opth / name)));

	const auto snum(src_files.size());

	PrintInfo(to_string(snum) + " file(s) found to be built in path: " + path
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
	OutputDir = NormalizeDirecoryPathTail(OutputDir);
	PrintInfo("Normalized output directory: '" + OutputDir + "'.");
	if(Options.empty())
	{
		PrintInfo("No options found. Stop.");
		return;
	}

	const auto in(NormalizeDirecoryPathTail(Options[0]));
	const auto ipath(MakeNormalizedAbsolute(in));

	PrintInfo("Absolute path recognized: " + to_string(ipath).GetMBCS() + " .");
	if(!VerifyDirectory(in))
		raise_exception(1, "SRCPATH is not existed.");
	EnsureOutputDirectory(OutputDir);
	std::for_each(next(Options.begin()), Options.end(), [&](const string& opt){
		flags += ' ' + opt;
	});

	const auto opth(Path(OutputDir) / ipath.back());
	const auto& ofiles(ActionContext([this](const Key& name){
		const shared_ptr<Rule> p_rule(new Rule{*this, name});

		return VerifyDirectory(name.first)
			? BuildAction([=](const ActionContext& actx){
			return SearchDirectory(*p_rule, actx);
		}) : BuildAction([=](const ActionContext&){
			return BuildFile(*p_rule);
		});
	})({in, opth}));

	Wait();

	const auto onum(ofiles.size());

	PrintInfo("Found " + to_string(onum) + " .o file(s).", Informative);
	if(onum != 0)
	{
		Path pth(opth);

		YAssert(!pth.empty(), "Invalid path found.");
		pth.pop_back();
		EnsureOutputDirectory(pth);

		auto target(to_string(opth).GetMBCS(CS_Path));
		const auto& cmd(Mode == 1 ? AR + ' ' + ARFLAGS
			: LD + ' ' + LDFLAGS + " -o");

		if(Mode == 1)
			target += ".a";
		// FIXME: Find extension properly.
		else
			// FIXME: Parse %LDFLAGS.
			target += ystdex::exists_substr(LDFLAGS, "-Bdynamic")
				|| ystdex::exists_substr(LDFLAGS, "-shared") ? ".dll" : ".exe";
		if(CheckBuild(ofiles, target))
		{
			auto str(cmd + " \"" + target + '"');

			// FIXME: Prevent path too long.
			for(const auto& ofile : ofiles)
				str += " \"" + ofile + "\"";		
			if(Mode == 2)
				str += ' ' + LIBS;
			PrintInfo("Link file: '" + target + "'.", Informative);
			CallWithException(str, 1);
		}
	}
	else
		PrintInfo("No files to be built.", Warning);
}

int
BuildContext::Call(const string& cmd, size_t n) const
{
	if(n == 0)
		n = jobs.get_max_task_num();
	return n <= 1 ? platform::usystem(cmd.c_str()) : RunTask(cmd);
}

int
BuildContext::RunTask(const string& cmd) const
{
	{
		std::lock_guard<std::mutex> lck(job_mtx);

		if(result != 0)
			return result;
	}
	// TODO: Use ISO C++1y lambda initializers to simplify implementation.
	jobs.wait_for(std::chrono::milliseconds(80), [&, cmd]{
		const int res(platform::usystem(cmd.c_str()));
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

void
BuildContext::Wait() const
{
	// TODO: Optimize for job dependency.
	if(jobs.get_max_task_num() > 1)
	{
		PrintInfo("Wait for unfinished tasks before linking ...");
		jobs.reset();
	}
	CheckResult(GetLastResult());
}


//! \since build 546
void
PrintUsage(const char*);

void
PrintUsage(const char* pr)
{
	YAssertNonnull(pr);
	std::printf("%s%s%s", "Usage: [ENV ...] ", pr, " SRCPATH [OPTIONS ...]\n\n"
		"[ENV ...]\n"
		"\tThe environment variables settings."
		" Currently accepted settings are listed below:\n\n"
		"  CC\n\tThe C compiler executable. Default value is 'gcc'.\n\n"
		"  CXX\n\tThe C++ compiler executable. Default value is 'g++'.\n\n"
		"  AR\n\tThe archiver executable. Default value is 'ar'.\n\n"
		"  ARFLAGS\n\tThe archiver flags. Default value is 'rcs'.\n\n"
		"  LD\n\tThe linker executable. Default value is 'ld'.\n\n"
		"  LDFLAGS\n\tThe linker flags. Default value is empty.\n\n"
		"  LIBS\n\tExtra options for at end. Default value is empty.\n\n"
		"SRCPATH\n"
		"\tThe source directory to be recursively searched.\n\n"
		"OPTIONS ...\n"
		"\tThe options. All other options would be sent to the backends,"
		" except for listed below:\n\n"
		"  " OPT_pfx_xd "DIR_NAME\n"
		"\tThe name of output directory."
		" Default value is '" OPT_build_path "'.\n"
		"\tMultiple occurrence is allowed.\n\n"
		"  " OPT_pfx_xid "DIR_NAME\n"
		"\tThe name of subdirectory which should be ignored when scanning.\n"
		"\tMultiple occurrence is allowed.\n\n"
		"  " OPT_pfx_xj "MAX_JOB_COUNT\n"
		"\tMax count of parallel jobs at the same time.\n"
		"\tThis number would be used to limit the number of tasks being"
		" spawning. If no valid value is explicitly specified, the implicit"
		" default value is 0. If this value is not more than 1, only one"
		" task would be load and run at each time.\n"
		"\tIf this option occurs more than once, only the last one is"
		" effective.\n\n"
		"  " OPT_pfx_xlogfl "LOG_LEVEL\n"
		"\tThe unsigned integer log level threshold of the logger.\n"
		"\tOnly log with level less than this value would be present in the"
		" out put stream.\n"
		"\tIf this option occurs more than once, only the last one is"
		" effective.\n\n"
		"  " OPT_pfx_xmode "MODE\n"
		"\tThe target action mode.\n"
		"\tValue '1' represents call of AR for the final target, and '2' is LD."
		" Other value is reserved and to be ignored."
		" Default value is '1'.\n"
		"\tIf this option occurs more than once, only the last one is"
		" effective.\n\n");
}


int
main(int argc, char* argv[])
{
	using namespace platform_ex;
	using namespace std::chrono;
	auto p_wcon(MakeWConsole()), p_wcon_err(MakeWConsole(STD_ERROR_HANDLE));

	try
	{
		auto& logger(FetchCommonLogger());
		const auto start_time(steady_clock::now());

		logger.FilterLevel = Logger::Level::Debug;
		logger.SetSender([&](Logger::Level lv, Logger&, const char* str){
			const auto stream(lv <= Warning ? stderr : stdout);
			const auto& p_con(lv <= Warning ? p_wcon_err : p_wcon);
			const auto dcnt(duration_cast<milliseconds>(steady_clock::now()
				- start_time).count());

			if(p_con)
				p_con->RestoreAttributes();
			std::fprintf(stream, "[%04u.%03u][%#02X]", unsigned(dcnt / 1000U),
				unsigned(dcnt % 1000U), unsigned(lv));
			YAssertNonnull(str);
			if(p_con)
			{
				using namespace platform::Consoles;
				static const Logger::Level
					lvs[]{Err, Warning, Notice, Informative, Debug};
				static const Color
					colors[]{Red, Yellow, Cyan, Magenta, DarkGreen};
				const auto i(
					std::lower_bound(&lvs[0], &lvs[arrlen(colors)], lv));

				if(i == &lvs[arrlen(colors)])
					p_con->RestoreAttributes();
				else
					p_con->UpdateForeColor(colors[i - lvs]);
			}
			std::fprintf(stream, "%s\n", MBCSToMBCS(str).c_str());
			if(p_con)
				p_con->RestoreAttributes();
		});
		if(argc > 1)
		{
			vector<string> args;
			set<string> ignored_dirs;
			string output_dir;
			size_t max_jobs(0);
			size_t mode(1);

			for(int i(1); i < argc; ++i)
			{
				using namespace ystdex;
				auto&& arg(MBCSToMBCS(argv[i], CP_ACP, CP_UTF8));

				if(ystdex::filter_prefix(arg, OPT_pfx_xd, [&](string&& val){
					PrintInfo("Output directory is switched to '" + val + "'.");
					output_dir = std::move(val);
				}))
					;
				else if(ystdex::filter_prefix(arg, OPT_pfx_xid,
					[&](string&& val){
					PrintInfo("Subdirectory '" + val + "' should be ignored.");
					ignored_dirs.emplace(std::move(val));
				}))
					;
				else if(ParsePrefixedOption<unsigned long>(arg, OPT_pfx_xj,
					"job max count", 0x100, [&](unsigned long uval){
					PrintInfo("Set job max count = " + to_string(uval) + ".");
					max_jobs = size_t(uval);
				}))
					;
				else if(ParsePrefixedOption<unsigned long>(arg, OPT_pfx_xlogfl,
					"log filter level", 0x100, [&](unsigned long uval){
					logger.FilterLevel = Logger::Level(uval);
				}))
					;
				else if(ParsePrefixedOption<unsigned long>(arg, OPT_pfx_xmode,
					"mode", 0x3, [&](unsigned long uval){
					if(uval == 1 || uval == 2)
						mode = uval;
					else
						PrintInfo("Ignored unsupported mode '" + to_string(uval)
							+ "'.", Warning);
				}))
					;
				else
					args.emplace_back(std::move(arg));
			}

			BuildContext ctx(max_jobs);
			using opts = pair<string&, const char*>;
				
			ystdex::seq_apply([](opts pr){
					FetchEnvironmentVariable(pr.first, pr.second);
					PrintInfo(pr.second + (" = " + pr.first));
				}, opts{ctx.CC, "CC"}, opts{ctx.CXX, "CXX"}, opts{ctx.AR, "AR"},
				opts{ctx.ARFLAGS, "ARFLAGS"}, opts{ctx.LD, "LD"},
				opts{ctx.LDFLAGS, "LDFLAGS"}, opts{ctx.LIBS, "LIBS"});
			if(!output_dir.empty())
				ctx.OutputDir = std::move(output_dir);
			yunseq(ctx.IgnoredDirs = std::move(ignored_dirs),
				ctx.Options = std::move(args), ctx.Mode = mode);
			PrintInfo("OutputDir = " + ctx.OutputDir);
			ctx.Build();
		}
		else if(argc == 1)
			PrintUsage(*argv);
	}
	catch(std::exception& e)
	{
		PrintException(e);
		return EXIT_FAILURE;
	}
	catch(...)
	{
		PrintInfo("ERROR: Unknown failure.", Err);
		return EXIT_FAILURE;
	}
}

