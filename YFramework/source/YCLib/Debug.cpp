/*
	© 2011-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Debug.cpp
\ingroup YCLib
\brief YCLib 调试设施。
\version r843
\author FrankHB <frankhb1989@gmail.com>
\since build 299
\par 创建时间:
	2012-04-07 14:22:09 +0800
\par 修改时间:
	2017-09-23 23:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Debug
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Debug // for wstring, string_view, std::puts, std::fflush;
#if YCL_Win32
#	include YFM_Win32_YCLib_Consoles // for platform_ex::WConsole,
//	STD_OUTPUT_HANDLE, STD_ERROR_HANDLE, platform_ex::Win32Exception;
#	include YFM_Win32_YCLib_NLS // for platform_ex::UTF8ToWCS;
#	include YFM_YCLib_NativeAPI // for ::OutputDebugStringA, ::MessageBoxA;
#	include <csignal> // for std::raise, SIGABRT;
#elif YCL_Android
#	include <android/log.h>
#endif
#include YFM_YCLib_Host // for platfrom_ex::EncodeArg;
#if YF_Multithread == 1
#	include <ystdex/concurrency.h>
#endif
#if !YCL_DS
#	include <iostream> // for std::cerr;
#endif
#include <cstdarg> // for std::va_list, va_start, va_end;

namespace platform
{

namespace
{

//! \since build 564
inline PDefH(const char*, chk_null, const char* s)
	ImplRet(s && *s != char()? s : "<unknown>")

#if YF_Multithread == 1
//! \since build 626
std::string
FetchCurrentThreadID() ynothrow
{
	TryRet(ystdex::get_this_thread_id())
	// XXX: Nothing more can be done.
	CatchIgnore(...)
	return {};
}
#endif

#if YCL_Win32
//! \since build 742
YB_NONNULL(3) size_t
WConsoleOutput(wstring& wstr, unsigned long h, const char* str)
{
	using namespace platform_ex;

	wstr = UTF8ToWCS(str) + L'\n';
	return WConsole(h).WriteString(wstr);
}
#elif YCL_Android
//! \since build 498
template<typename... _tParams>
inline int
PrintAndroidLog(Descriptions::RecordLevel lv, const char* tag, const char *fmt,
	_tParams&&... args)
{
	return ::__android_log_print(platform_ex::MapAndroidLogLevel(lv),
		Nonnull(tag), Nonnull(fmt), yforward(args)...);
}
#endif

//! \since build 551
using namespace Concurrency;

} // unnamed namespace;

bool
Echo(string_view sv) ynoexcept(YF_Platform == YF_Platform_DS)
{
#if YCL_DS
	return std::puts(Nonnull(sv.data())) >= 0 && std::fflush(stdout) == 0;
#elif YCL_Win32
	wstring wstr;
	size_t n(0);

	TryExpr(n = WConsoleOutput(wstr, STD_OUTPUT_HANDLE, sv.data()))
	CatchIgnore(platform_ex::Win32Exception&)
	if(n < wstr.length())
		std::cout << &wstr[n];
	return bool(std::cout.flush());
#elif YF_Hosted
	return bool(std::cout << platform_ex::EncodeArg(sv) << std::endl);
#else
	return bool(std::cout << sv << std::endl);
#endif
}


Logger::Logger(const Logger& logger)
	: FilterLevel(logger.FilterLevel), filter(logger.filter),
	sender(logger.sender)
{}
Logger::Logger(Logger&& logger) ynothrow
	: Logger()
{
	swap(logger, *this);
}

void
Logger::SetFilter(Filter f)
{
	if(f)
		filter = std::move(f);
}
void
Logger::SetSender(Sender s)
{
	if(s)
		sender = std::move(s);
}

bool
Logger::DefaultFilter(Level lv, Logger& logger) ynothrow
{
	return lv <= logger.FilterLevel;
}

void
Logger::DefaultSendLog(Level lv, Logger& logger, const char* str) ynothrowv
{
#if YCL_DS
	yunused(lv), yunused(logger), yunused(str);
#else
	SendLog(std::cerr, lv, logger, str);
#endif
}

void
Logger::DefaultSendLogToFile(Level lv, Logger& logger, const char* str)
	ynothrowv
{
	SendLogToFile(stderr, lv, logger, str);
}

void
Logger::DoLog(Level level, const char* str)
{
	if(str)
	{
		lock_guard<recursive_mutex> lck(record_mutex);

		DoLogRaw(level, str);
	}
}

void
Logger::DoLogRaw(Level level, const char* str)
{
	sender(level, *this, Nonnull(str));
}

void
Logger::DoLogException(Level lv, const std::exception& e) ynothrow
{
	const auto do_log_excetpion_raw([this](const char* msg) YB_NONNULL(1)
		ynothrow{
		try
		{
			DoLogRaw(Descriptions::Emergent,
				"Another exception thrown when handling exception.");
			DoLogRaw(Descriptions::Emergent, msg);
		}
		CatchExpr(...,
			ystdex::ytrace(stderr, Descriptions::Emergent, Descriptions::Notice,
			__FILE__, __LINE__, "Logging error: unhandled exception."))
	});
	const auto& msg(e.what());
	lock_guard<recursive_mutex> lck(record_mutex);

	try
	{
		// XXX: Provide no throw guarantee and put it out of the critical
		//	section?
		// XXX: Log demangled type name.
		DoLogRaw(lv, sfmt("<%s>: %s", typeid(e).name(), msg));
	}
	CatchExpr(std::exception& ex, do_log_excetpion_raw(ex.what()))
	CatchExpr(..., do_log_excetpion_raw({}))
}

Logger::Sender
Logger::FetchDefaultSender(string_view tag)
{
	YAssertNonnull(tag.data());
#if YCL_Win32
	return [](Level lv, Logger& logger, const char* str){
		// TODO: Avoid throwing of %WriteString here for better performance?
		// FIXME: Output may be partially updated?
		try
		{
			wstring wstr;

			WConsoleOutput(wstr, STD_ERROR_HANDLE, str);
		}
		CatchExpr(platform_ex::Win32Exception&, DefaultSendLog(lv, logger, str))
	};
#elif YCL_Android
	return platform_ex::AndroidLogSender(tag);
#else
	return DefaultSendLog;
#endif
}

void
Logger::SendLog(std::ostream& os, Level lv, Logger&, const char* str)
	ynothrowv
{
	try
	{
#if YF_Multithread == 1
		const auto& t_id(FetchCurrentThreadID());

		if(!t_id.empty())
			os << ystdex::sfmt("[%s:%#X]: %s\n", t_id.c_str(), unsigned(lv),
				Nonnull(str));
		else
#endif
			os << ystdex::sfmt("[%#X]: %s\n", unsigned(lv), Nonnull(str));
		os.flush();
	}
	CatchIgnore(...)
}

void
Logger::SendLogToFile(std::FILE* stream, Level lv, Logger&, const char* str)
	ynothrowv
{
	YAssertNonnull(stream);
#if YF_Multithread == 1
	const auto& t_id(FetchCurrentThreadID());

	if(!t_id.empty())
		std::fprintf(stream, "[%s:%#X]: %s\n", t_id.c_str(), unsigned(lv),
			Nonnull(str));
	else
#endif
		std::fprintf(stream, "[%#X]: %s\n", unsigned(lv), Nonnull(str));
	std::fflush(stream);
}

void
swap(Logger& x, Logger& y) ynothrow
{
	// TODO: Wait for C++17.
	// XXX: See discussion in LWG 2062.
#if !__GLIBCXX__
	ynoexcept_assert("Unsupported luanguage implementation found.",
		x.filter.swap(y.filter));
	ynoexcept_assert("Unsupported luanguage implementation found.",
		x.sender.swap(y.sender));
#endif

	std::swap(x.FilterLevel, y.FilterLevel);
	x.filter.swap(y.filter);
	x.sender.swap(y.sender);
}


Logger&
FetchCommonLogger()
{
	static Logger logger;

	return logger;
}


string
LogWithSource(const char* file, int line, const char* fmt, ...) ynothrow
{
	try
	{
		std::va_list args;

		va_start(args, fmt);

		string str(vsfmt(fmt, args));

		va_end(args);
		return sfmt("\"%s\":%i:\n", chk_null(file), line) + std::move(str);
	}
	CatchExpr(...,
		ystdex::ytrace(stderr, Descriptions::Emergent, Descriptions::Notice,
		chk_null(file), line, "LogWithSource error: unhandled exception."))
	return {};
}

} // namespace platform;

using namespace platform;

namespace platform_ex
{

#if YF_Multithread == 1
void
LogAssert(const char* expr_str, const char* file, int line,
	const char* msg) ynothrow
{
#	if YCL_Android
		::__android_log_assert(expr_str, "YFramework",
			"Assertion failed @ \"%s\":%i:\n %s .\nMessage: \n%s\n", file, line,
			expr_str, msg);
#	else
#		if YCL_Win32
	try
	{
		char prog[MAX_PATH]{"<unknown>"};

		::GetModuleFileNameA({}, prog, MAX_PATH);

		const auto& errstr(sfmt("Assertion failed @ program %s: "
			"\"%s\":%i:\n %s .\nMessage: \n%s\n", prog, chk_null(file),
			line, chk_null(expr_str), chk_null(msg)));

		::OutputDebugStringA(errstr.c_str());
		// XXX: Not safe in windows procedure, but not used in YFramework.
		// TODO: Use custom windows creation?
		switch(::MessageBoxA({}, errstr.c_str(),
			"YCLib Runtime Assertion", MB_ABORTRETRYIGNORE | MB_ICONHAND
			| MB_SETFOREGROUND | MB_TASKMODAL))
		{
		case IDIGNORE:
			return;
		case IDABORT:
			std::raise(SIGABRT);
		default:
			break;
		}
		std::terminate();
	}
	catch(...)
	{
		YF_TraceRaw(Descriptions::Emergent, "Unknown exception found.");
		ystdex::yassert(expr_str, file, line, msg);
	}
#		endif
	TryExpr(FetchCommonLogger().AccessRecord([=]{
		ystdex::yassert(expr_str, file, line, msg);
	}))
	catch(...)
	{
		std::fprintf(stderr, "Fetch logger failed.");
		std::fflush(stderr);
	}
	ystdex::yassert(expr_str, file, line, msg);
#	endif
}
#endif


#if YCL_Win32
void
SendDebugString(Logger::Level lv, Logger&, const char* str) ynothrowv
{
	try
	{
		const auto& t_id(FetchCurrentThreadID());

		// TODO: Use %::WaitForDebugEventEx if possible. See https://msdn.microsoft.com/en-us/library/windows/desktop/mt171594(v=vs.85).aspx.
		::OutputDebugStringA((!t_id.empty() ? ystdex::sfmt("[%s:%#X]: %s",
			t_id.c_str(), unsigned(lv), Nonnull(str)) : ystdex::sfmt(
			"[%#X]: %s", unsigned(lv), Nonnull(str))).c_str());
	}
	CatchIgnore(...)
}
#elif YCL_Android
int
MapAndroidLogLevel(Descriptions::RecordLevel lv)
{
	return lv < Descriptions::Critical ? ANDROID_LOG_FATAL
		: ANDROID_LOG_FATAL - (std::uint8_t(lv) - 0x40) / 0x20;
}


AndroidLogSender::AndroidLogSender(string_view sv)
	: tag((Nonnull(sv.data()), sv))
{}
ImplDeDtor(AndroidLogSender)

void
AndroidLogSender::operator()(Level lv, Logger& logger, const char* str) const
{
	Logger::DefaultSendLog(lv, logger, str);
	if(lv < Descriptions::Critical)
		::__android_log_assert("", "YFramework", "[%#X]: %s\n",
			unsigned(lv), str);
	else
		PrintAndroidLog(lv, tag.c_str(), "[%#X]: %s\n", unsigned(lv), str);
}

#endif

} // namespace platform_ex;

