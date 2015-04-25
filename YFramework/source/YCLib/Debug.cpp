/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Debug.cpp
\ingroup YCLib
\brief YCLib 调试设施。
\version r532
\author FrankHB <frankhb1989@gmail.com>
\since build 299
\par 创建时间:
	2012-04-07 14:22:09 +0800
\par 修改时间:
	2015-04-24 04:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Debug
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Debug
#include YFM_YCLib_Input
#include YFM_YCLib_Video // for platform::ColorSpace, platform::YConsoleInit;
#include <cstdarg>
#include <ystdex/string.hpp>
#if YF_Multithread
#	include <ystdex/concurrency.h>
#endif
#if YCL_DS
#	include YFM_DS_YCLib_DSVideo // for platform_ex::DSInitConsole;
#elif YCL_Win32
#	include <csignal>
#	include <Windows.h>
#elif YCL_Android
#	include <android/log.h>
#endif

namespace platform
{

namespace
{

//! \since build 564
inline const char*
chk_null(const char* s)
{
	return s && *s != '\0'? s : "<unknown>";
}

#if YCL_Android

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
Logger::DefaultSendLog(Level lv, Logger&, const char* str) ynothrowv
{
#if YCL_DS
	if(lv <= Descriptions::Alert)
		platform_ex::DSConsoleInit({}, ColorSpace::White, ColorSpace::Blue);
#endif
#if YF_Multithread
	std::string t_id;

	TryExpr(t_id = ystdex::to_string(std::this_thread::get_id()))
	CatchIgnore(...)
	if(!t_id.empty())
		std::fprintf(stderr, "[%s:%#X]: %s\n",
			ystdex::to_string(std::this_thread::get_id()).c_str(),
			unsigned(lv), Nonnull(str));
	else
#endif
		std::fprintf(stderr, "[%#X]: %s\n", unsigned(lv), Nonnull(str));
	std::fflush(stderr);
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
	const auto do_log_excetpion_raw([this](const char* msg){
		try
		{
			DoLogRaw(Descriptions::Emergent,
				"Another exception thrown when handling exception.");
			if(msg)
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
Logger::FetchDefaultSender(const string& tag)
{
#if YCL_Android
	return platform_ex::AndroidLogSender(tag);
#else
	yunused(tag);

	return DefaultSendLog;
#endif
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

#if YB_Use_YAssert && YF_Multithread == 1
void
LogAssert(bool expr, const char* expr_str, const char* file, int line,
	const char* msg) ynothrow
{
	if(YB_UNLIKELY(!expr))
#	if YCL_Android
		::__android_log_assert(expr_str, "YFramework",
			"Assertion failed @ \"%s\":%i:\n %s .\nMessage: \n%s\n", file, line,
			expr_str, msg);
#	else
	{
#		if YCL_Win32
		try
		{
			char prog[MAX_PATH]{"<unknown>"};

			::GetModuleFileNameA({}, prog, MAX_PATH);

			const auto& errstr(sfmt("Assertion failed @ program %s: "
				"\"%s\":%i:\n %s .\nMessage: \n%s\n", prog, chk_null(file),
				line, chk_null(expr_str), chk_null(msg)));

			::OutputDebugStringA(errstr.c_str());
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
		{}
#		endif
		try
		{
			FetchCommonLogger().AccessRecord([=]{
				ystdex::yassert({}, expr_str, file, line, msg);
			});
		}
		catch(...)
		{
			std::fprintf(stderr, "Fetch logger failed.");
			std::fflush(stderr);
			ystdex::yassert({}, expr_str, file, line, msg);
		}
	}
#	endif
}
#endif


#if YCL_Android
int
MapAndroidLogLevel(Descriptions::RecordLevel lv)
{
	return lv < Descriptions::Critical ? ANDROID_LOG_FATAL
		: ANDROID_LOG_FATAL - (std::uint8_t(lv) - 0x40) / 0x20;
}


AndroidLogSender::AndroidLogSender(const string& t)
	: tag(t)
{}
AndroidLogSender::~AndroidLogSender()
{}

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

