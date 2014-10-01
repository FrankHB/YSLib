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
\version r1340
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-06 14:33:55 +0800
\par 修改时间:
	2014-10-01 11:00 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Tools::SHBuild::Main

This is a command line tool to build project files simply.
See readme file for details.
*/


#include <ysbuild.h>
#include YFM_YSLib_Service_FileSystem
#include <iostream>
#include <iomanip> // for std::setw;
#include <sstream>
#include <Windows.h>
#include <ystdex/mixin.hpp>
#include YFM_MinGW32_YCLib_Consoles // for platform_ex::WConsole;
#include <ystdex/concurrency.h> // for ystdex::thread_pool;
#include <ystdex/exception.hpp> // for ystdex::raise_exception;

using std::for_each;
//! \since build 538
using std::string;
using std::wstring;
//! \since build 520
using std::ostringstream;
using namespace YSLib;
using namespace IO;
//! \since build 476
using platform_ex::MBCSToMBCS;

namespace
{

/*!
\brief 默认构建根目录路径。
\note 末尾的分隔符需要必须存在。
*/
yconstexpr auto build_path(u8".shbuild\\");

/*!
\brief 选项前缀：扫描时忽略的目录(ignore directory) 。
\since build 520
*/
#define OPT_pfx_xid u8"-xid,"

/*!
\brief 选项前缀：最大并行任务(jobs) 数。
\since build 520
*/
#define OPT_pfx_xj u8"-xj,"

/*!
\brief 选项前缀：过滤输出日志的等级(log filter level) 。
\since build 519
*/
#define OPT_pfx_xlogfl u8"-xlogfl,"


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
	ImplExpr(raise_exception(ret, u8"Failed calling command."));

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
		ostringstream oss;

		oss << string(level, ' ') << u8"ERROR: " << e.what();
		PrintInfo(oss.str(), Err);
		throw;
	}
	catch(IntException& e)
	{
		ostringstream oss;

		oss << "IntException: " << std::setw(8) << std::showbase
			<< std::uppercase << std::hex << int(e) << '.';
		PrintInfo(oss.str(), Err);
	}
	catch(FileOperationFailure& e)
	{
		PrintInfo(u8"ERROR: File operation failure.", Err);
	}
	catch(std::exception& e)
	{}
	catch(...)
	{
		PrintInfo(u8"ERROR: PrintException.", Err);
	}
	ystdex::handle_nested(e, [=](std::exception& e){
		PrintException(e, level + 1);
	});
}
//@}


//! \since build 520
//@{
template<typename _type, typename _fCallable>
bool
ParsePrefixedOption(const string& arg, const _type& prefix, _fCallable f)
{
	using namespace ystdex;

	YAssertNonnull(prefix);
	if(begins_with(arg, prefix))
	{
		auto&& val(arg.substr(string_length(prefix)));

		if(!val.empty())
			f(std::move(val));
		return true;
	}
	return {};
}

template<typename _fCallable>
void
TryParseOptionUL(const string& name, const string& val, _fCallable f)
{
	using namespace std;

	try
	{
		const auto uval(stoul(val));

		if(!f(uval))
			PrintInfo(u8"Warning: Value '" + val + u8"' of " + name
				+ u8" out of range.", Warning);
	}
	catch(std::invalid_argument&)
	{
		PrintInfo(u8"Warning: Value '" + val + u8"' of " + name
			+ u8" is invalid.", Warning);
	}
	catch(std::out_of_range&)
	{
		PrintInfo(u8"Warning: Value '" + val + u8"' of " + name
			+ u8" out of range.", Warning);
	}
}

template<typename _fCallable, typename _type>
bool
ParsePrefixedOptionUL(const string& arg, const _type& prefix,
	const string& name, unsigned long threshold, _fCallable f)
{
	return ParsePrefixedOption(arg, prefix, [&](string&& val){
		TryParseOptionUL(name, val, [=](unsigned long uval){
			return uval < threshold ? (void(f(uval)), true) : false;
		});
	});
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

	//! \since build 539
	//@{
	int
	Call(const wchar_t*, size_t n = 0) const;

	PDefH(void, CallWithException, const string& cmd, size_t n = 0) const
		ImplExpr(std::cout << cmd << std::endl,
			CheckResult(Call(platform_ex::MBCSToWCS(cmd).c_str(), n)))

	static PDefH(void, CheckResult, int ret)
		ImplExpr(ret == 0 ? void() : raise_exception(ret))
	//@}

	int
	RunTask(const wchar_t*) const;

	void
	Search(const string&, const string&) const;
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
	PrintInfo(ystdex::sfmt(u8"Ready to run, job max count: %u.",
		jobs.get_max_task_num()));

	if(Options.empty())
	{
		PrintInfo(u8"No options found. Stop.");
		return;
	}

	auto in(ystdex::rtrim(string(Options[0]), "/\\") + YCL_PATH_DELIMITER);
	Path ipath(in);

	if(ipath.empty())
		raise_exception(1, u8"Empty SRCPATH found.");
	if(IsRelative(ipath))
		ipath = Path(FetchCurrentWorkingDirectory()) / ipath;
	ipath.Normalize();

	YAssert(IsAbsolute(ipath), "Invalid path converted.");

	PrintInfo(u8"Absolute path recognized: " + to_string(ipath).GetMBCS());
	if(!VerifyDirectory(in))
		raise_exception(1, u8"SRCPATH is not existed.");
	try
	{
		EnsureDirectory(build_path);
	}
	catch(std::system_error&)
	{
		raise_exception(2, string(u8"Failed creating build directory.")
			+ '\'' + build_path + '\'');
	}
	for_each(next(Options.begin()), Options.end(), [&](const string& opt){
		flags += ' ' + opt;
	});

	string opath(build_path);

	if(!ipath.empty())
		opath += ipath.back().GetMBCS() + YCL_PATH_DELIMITER;
	Search(in, opath);
}

int
BuildContext::Call(const wchar_t* cmd, size_t n) const
{
	if(n == 0)
		n = jobs.get_max_task_num();
	YAssert(cmd, "Null pointer found.");
	return n <= 1 ? ::_wsystem(cmd) : RunTask(cmd);
}

int
BuildContext::RunTask(const wchar_t* cmd) const
{
	{
		std::lock_guard<std::mutex> lck(job_mtx);

		if(result != 0)
			return result;
	}
	YAssert(cmd, "Null pointer found.");

	wstring cmd_str(cmd);

	// TODO: Use ISO C++1y lambda initializers to simplify implementation.
	jobs.wait_for(std::chrono::milliseconds(80), [&, cmd_str]{
		const int res(::_wsystem(cmd_str.c_str()));
		{
			std::lock_guard<std::mutex> lck(job_mtx);

			if(result == 0)
				result = res;
		}
		return res;
	});
	return 0;
}

void
BuildContext::Search(const string& path, const string& opath) const
{
	YAssert(path.size() > 1 && path.back() == wchar_t(YCL_PATH_DELIMITER),
		"Invalid path found.");

	PrintInfo(u8"Purging path: " + opath + u8" ...");
	try
	{
		EnsureDirectory(opath);
	}
	catch(std::system_error& e)
	{
		raise_exception(std::runtime_error(u8"Failed creating directory '"
			+ opath + u8"'."));
	}
	TraverseChildren(opath, [&](NodeCategory c, const std::string& name){
		if(name[0] == '.')
			return;
		if(c == NodeCategory::Regular)
		{
			const auto ext(GetExtensionOf(String(name, CS_Path)));

			if(ext == u"a" || ext == u"o")
			{
				if(!uremove((opath + name).c_str()))
					raise_exception(std::runtime_error(
						u8"Failed deleting file '" + name + u8"'."));
				PrintInfo(u8"Deleted file '" + name + u8"'.");
			}
		}
	});
	PrintInfo(u8"Searching path: " + path + u8" ...");
	TraverseChildren(path, [&, this](NodeCategory c, const std::string& name){
		if(name[0] == '.')
			return;
		if(c == NodeCategory::Regular)
		{
			const auto ext(GetExtensionOf(String(name, CS_Path)));
			const auto& cmd(GetCommandName(ext));

			if(!cmd.empty())
				CallWithException(cmd + u8" -c" + flags + ' ' + path + name
					+ u8" -o " + opath + name + u8".o");
			else
				PrintInfo(u8"Ignored non source file '" + name + u8"'.");
		}
		else
		{
			const auto& fpath(path + name + YCL_PATH_DELIMITER);

			if(ystdex::exists(IgnoredDirs, name))
				PrintInfo(ystdex::sfmt(u8"Subdirectory %s is ignored.",
					fpath.c_str()));
			else
				Search(path + name + YCL_PATH_DELIMITER,
					opath + name + YCL_PATH_DELIMITER);
		}
	});

	size_t anum(0), onum(0);

	// TODO: Optimize for job dependency.
	if(jobs.get_max_task_num() > 1)
	{
		PrintInfo(u8"Wait for unfinished tasks before linking ...");
		jobs.reset();
	}
	CheckResult(GetLastResult());
	TraverseChildren(opath, [&](NodeCategory c, const std::string& name){
		if(name[0] == '.')
			return;
		if(c == NodeCategory::Regular)
		{
			const auto ext(GetExtensionOf(String(name, CS_Path)));

			if(ext == u"a")
				++anum;
			else if(ext == u"o")
				++onum;
		}
	});
	PrintInfo(ystdex::sfmt("Found %u .a file(s) and %u .o file(s).",
		unsigned(anum), unsigned(onum)));
	if(anum != 0 || onum != 0)
	{
		auto str(opath);
	
		if(str.back() == YCL_PATH_DELIMITER)
			str.pop_back();

		str = AR + u8" rcs \"" + str + u8".a\"";
		if(anum != 0)
			str += " \"" + opath + u8"*.a\"";
		if(onum != 0)
			str += " \"" + opath + u8"*.o\"";
		CallWithException(str, 1);
	}
}
//@}


void
PrintUsage(const char* prog)
{
	std::cout << u8"Usage: " + string(prog) + u8" SRCPATH [OPTIONS ...]\n\n"
		u8"SRCPATH\n"
		u8"\tThe source directory to be recursively searched.\n\n"
		u8"OPTIONS ...\n"
		u8"\tThe options. All other options would be sent to the backends,"
		u8" except for listed below:\n\n"
		u8"  " OPT_pfx_xid "DIR_NAME\n"
		u8"\tThe name of subdirectory which should be ignored when scanning.\n"
		u8"\tMultiple occurrence is allowed.\n\n"
		u8"  " OPT_pfx_xj "MAX_JOB_COUNT\n"
		u8"\tMax count of parallel jobs at the same time.\n"
		u8"\tThis number would be used to limit the number of tasks being"
		u8" spawning. If no valid value is explicitly specified, the implicit"
		u8" default value is 0. If this value is not more than 1, only one"
		u8" task would be load and run at each time.\n"
		u8"\tIf this option occurs more than once, only the last one is"
		u8" effective.\n\n"
		u8"  " OPT_pfx_xlogfl "LOG_LEVEL\n"
		u8"\tThe unsigned integer log level threshold of the logger.\n"
		u8"\tOnly log with level less than this value would be present in the"
		u8" out put stream.\n"
		u8"\tIf this option occurs more than once, only the last one is"
		u8" effective.\n\n"
		<< std::flush;
}


int
main(int argc, char* argv[])
{
	platform_ex::WConsole wcon, wcon_err(STD_ERROR_HANDLE);

	try
	{
		auto& logger(FetchCommonLogger());

		logger.FilterLevel = Logger::Level::Debug;
		logger.SetSender([&](Logger::Level lv, Logger&, const char* str){
			auto& out(lv <= Warning ? std::cerr : std::cout);
			using namespace std;

			if(lv <= Err)
				wcon_err.UpdateForeColor(platform::Consoles::Red);
			else if(lv <= Warning)
				wcon_err.UpdateForeColor(platform::Consoles::Yellow);
			else
				wcon_err.RestoreAttributes();
			YAssertNonnull(str);
			out << "[" << hex << showbase << setw(2) << uppercase
				<< unsigned(lv) << "]:" << MBCSToMBCS(str) << endl;
		});
		if(argc > 1)
		{
			vector<string> args;
			set<string> ignored_dirs;
			size_t max_jobs(0);

			for(int i(1); i < argc; ++i)
			{
				using namespace ystdex;
				auto&& arg(MBCSToMBCS(argv[i], CP_ACP, CP_UTF8));

				if(ParsePrefixedOption(arg, OPT_pfx_xid, [&](string&& val){
					PrintInfo(sfmt("Subdirectory '%s' should be ignored.",
						val.c_str()));
					ignored_dirs.emplace(std::move(val));
				}))
					;
				else if(ParsePrefixedOptionUL(arg, OPT_pfx_xj, "job max count",
					0x100, [&](unsigned long uval){
					PrintInfo(sfmt("Set job max count = %lu.", uval));
					max_jobs = size_t(uval);
				}))
					;
				else if(ParsePrefixedOptionUL(arg, OPT_pfx_xlogfl,
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
			PrintInfo(u8"CC = " + ctx.CC),
			PrintInfo(u8"CXX = " + ctx.CXX),
			PrintInfo(u8"AR = " + ctx.AR);
			yunseq(ctx.IgnoredDirs = std::move(ignored_dirs),
				ctx.Options = std::move(args));
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
		PrintInfo(u8"ERROR: Unknown failure.", Err);
		return EXIT_FAILURE;
	}
}

