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
\version r1786
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-06 14:33:55 +0800
\par 修改时间:
	2014-10-11 19:52 +0800
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


//! \since build 477
//@{
using IntException = ystdex::wrap_mixin_t<std::runtime_error, int>;
using ystdex::raise_exception;
//! \since build 539
template<typename... _tParams>
YB_NORETURN inline PDefH(void, raise_exception, int ret, _tParams&&... args)
	ImplExpr(raise_exception<IntException>({
		std::runtime_error(yforward(args)...), ret}));
//! \since build 522
YB_NORETURN inline PDefH(void, raise_exception, int ret)
	ImplExpr(raise_exception(ret, "Failed calling command."));

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

	BuildContext(size_t n)
		: jobs(n)
	{}

	//! \since build 538
	string
	GetCommandName(const String&) const;

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

	//! \since build 541
	string
	Search(const Path&, const Path&) const;
};

string
BuildContext::GetCommandName(const String& ext) const
{
	if(ext == u"c")
		return CC;
	if(ext == u"cc" || ext == u"cpp" || ext == u"cxx")
		return CXX;
	return "";
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
	Search(in, Path(OutputDir) / ipath.back());
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

string
BuildContext::Search(const Path& ipth, const Path& opth) const
{
	const auto& path(ipth.GetMBCS());
	vector<string> subdirs, afiles;
	vector<pair<string, string>> files;
	const auto get_file_time([](const string& path){
		PrintInfo("Checking path '" + path + "' ...", Debug);

		const auto& file_time(platform::GetFileModificationTimeOf(path));

		PrintInfo("Modification time: " + to_string(file_time.count()) + " .",
			Debug);
		return file_time;
	});
	const auto need_build([=](const string& ipath, const string& opath)->bool{
		try
		{
			if(get_file_time(opath) >= get_file_time(ipath))
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
	});

	PrintInfo("Searching path: " + path + " ...");
	TraverseChildren(path, [&, this](NodeCategory c, const std::string& name){
		if(name[0] != '.')
		{
			if(c == NodeCategory::Directory)
			{
				if(ystdex::exists(IgnoredDirs, name))
					PrintInfo("Subdirectory " + path + name + YCL_PATH_DELIMITER
						+ " is ignored.", Informative);
				else
					subdirs.push_back(name);
			}
			else
			{
				const auto ext(GetExtensionOf(String(name, CS_Path)));
				auto cmd(GetCommandName(ext));

				if(!cmd.empty())
					files.emplace_back(std::move(cmd), name);
				else
					PrintInfo("Ignored non source file '" + name + "'.",
						Informative);
			}
		}
	});
	for(const auto& name : subdirs)
	{
		auto afile(Search(ipth / name, opth / name));

		if(!afile.empty())
			afiles.push_back(std::move(afile));
	}

	const auto onum(files.size());

	PrintInfo(to_string(onum) + " file(s) found to be built in path: "
		+ path + " .", Informative);
	if(onum != 0)
	{
		const auto& opath(opth.GetMBCS());

		EnsureOutputDirectory(opth);
		for(const auto& pr : files)
		{
			// TODO: Check dependencies timestamps.
			const auto& name(pr.second);
			const auto& fullname(path + name);
			const auto& ofullname(opath + name + ".o");

			if(need_build(fullname, ofullname))
			{
				PrintInfo("Compile file: '" + name + "'.", Informative);
				CallWithException(pr.first + " -MMD" + " -c" + flags + ' '
					+ fullname + " -o \"" + ofullname + '"');
			}
		}
	}
	// TODO: Optimize for job dependency.
	if(jobs.get_max_task_num() > 1)
	{
		PrintInfo("Wait for unfinished tasks before linking ...");
		jobs.reset();
	}
	CheckResult(GetLastResult());

	const auto anum(afiles.size());

	PrintInfo("Found " + to_string(anum) + " .a file(s) and "
		+ to_string(onum) + " .o file(s).", Informative);
	if(anum != 0 || onum != 0)
	{
		Path pth(opth);

		YAssert(!pth.empty(), "Invalid path found.");
		pth.pop_back();
		EnsureOutputDirectory(pth);

		auto target(to_string(opth).GetMBCS(CS_Path) + ".a");
		auto str(AR + " rcs \"" + target + '"');

		// FIXME: Prevent path too long.
		if(anum != 0)
			for(const auto& afile : afiles)
				str += " \"" + afile + "\"";		
		if(onum != 0)
		{
			const auto opath(opth.GetString().GetMBCS(CS_Path));

			for(const auto& pr : files)
				str += " \"" + opath + pr.second + ".o\"";
		}
		PrintInfo("Link file: '" + target + "'.", Informative);
		CallWithException(str, 1);
		return std::move(target);
	}
	return "";
}
//@}


//! \since build 542
void
PrintUsage(const string& prog)
{
	std::printf("%s", ("Usage: " + prog + " SRCPATH [OPTIONS ...]\n\n"
		"SRCPATH\n"
		"\tThe source directory to be recursively searched.\n\n"
		"OPTIONS ...\n"
		"\tThe options. All other options would be sent to the backends,"
		" except for listed below:\n\n"
		"  " OPT_pfx_xd "DIR_NAME\n"
		"\tThe name of output directory. "
		"Default value is '" OPT_build_path "'.\n"
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
		" effective.\n\n").c_str());
}


int
main(int argc, char* argv[])
{
	using namespace platform_ex;
	auto p_wcon(MakeWConsole()), p_wcon_err(MakeWConsole(STD_ERROR_HANDLE));

	try
	{
		auto& logger(FetchCommonLogger());

		logger.FilterLevel = Logger::Level::Debug;
		logger.SetSender([&](Logger::Level lv, Logger&, const char* str){
			const auto stream(lv <= Warning ? stderr : stdout);
			const auto& p_con(lv <= Warning ? p_wcon_err : p_wcon);

			if(p_con)
				p_con->RestoreAttributes();
			std::fprintf(stream, "[%#02X]", unsigned(lv));
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
				else
					args.emplace_back(std::move(arg));
			}

			BuildContext ctx(max_jobs);
				
			WriteEnvironmentVariable(ctx.CC, "CC"),
			WriteEnvironmentVariable(ctx.CXX, "CXX"),
			WriteEnvironmentVariable(ctx.AR, "AR");
			PrintInfo("CC = " + ctx.CC);
			PrintInfo("CXX = " + ctx.CXX);
			PrintInfo("AR = " + ctx.AR);
			if(!output_dir.empty())
				ctx.OutputDir = std::move(output_dir);
			yunseq(ctx.IgnoredDirs = std::move(ignored_dirs),
				ctx.Options = std::move(args));
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

