/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Debug.cpp
\ingroup YCLib
\brief YCLib 调试设施。
\version r346
\author FrankHB <frankhb1989@gmail.com>
\since build 299
\par 创建时间:
	2012-04-07 14:22:09 +0800
\par 修改时间:
	2014-07-15 23:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Debug
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Debug
#include YFM_YCLib_Input
#include YFM_YCLib_Video // for platform::ColorSpace;
#include <cstdarg>
#include <ystdex/string.hpp>
#if YCL_Android
#	include <android/log.h>
#endif

namespace platform
{

namespace
{

#if YCL_Android

//! \since build 498
template<typename... _tParams>
inline int
PrintAndroidLog(Descriptions::RecordLevel lv, const char* tag, const char *fmt,
	_tParams&&... args)
{
	YAssertNonnull(tag), YAssertNonnull(fmt);
	return ::__android_log_print(platform_ex::MapAndroidLogLevel(lv), tag, fmt,
		yforward(args)...);
}
#endif

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
	YAssertNonnull(str);
	std::fprintf(stderr, "[%#X]: %s\n", unsigned(lv), str);
	std::fflush(stderr);
}

void
Logger::DoLog(Level level, const char* str)
{
	if(str)
	{
#if YF_Multithread == 1
		std::lock_guard<std::recursive_mutex> lck(record_mtx);
#endif

		DoLogRaw(level, str);
	}
}

void
Logger::DoLogRaw(Level level, const char* str)
{
	YAssertNonnull(str);

	sender(level, *this, str);
}

void
Logger::DoLogException(Level level, const std::exception& e) ynothrow
{
#if YF_Multithread == 1
		std::lock_guard<std::recursive_mutex> lck(record_mtx);
#endif

	try
	{
		// XXX: Log demangled type name.
		DoLogRaw(level, ystdex::sfmt("<%s>: %s.", typeid(e).name(), e.what()));
	}
	catch(std::exception& e)
	{
		try
		{
			DoLogRaw(Descriptions::Emergent,
				"Another exception thrown when handling exception.");
			DoLogRaw(Descriptions::Emergent, e.what());
		}
		catch(...)
		{
			ystdex::ytrace(stderr, Descriptions::Emergent, Descriptions::Notice,
				__FILE__, __LINE__, "Logging error: unhandled exception#1.");
		}
	}
	catch(...)
	{
		try
		{
			DoLogRaw(Descriptions::Emergent,
				"Another unknown exception thrown when handling exception.");
		}
		catch(...)
		{
			ystdex::ytrace(stderr, Descriptions::Emergent, Descriptions::Notice,
				__FILE__, __LINE__, "Logging error: unhandled exception#2.");
		}
	}
}

Logger::Sender
Logger::FetchDefaultSender(const std::string& tag)
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
#if YF_Multithread
	static std::mutex mtx;
	std::lock_guard<std::mutex> lck(mtx);
#endif
	static Logger logger;

	return logger;
}


std::string
LogWithSource(const char* file, int line, const char* fmt, ...)
{
	YAssertNonnull(file), YAssertNonnull(fmt);

	std::va_list args;

	va_start(args, fmt);

	std::string str(ystdex::vsfmt(fmt, args));

	va_end(args);
	return ystdex::sfmt("\"%s\":%i:\n", file, line) + std::move(str);
}

} // namespace platform;

using namespace platform;

namespace platform_ex
{

#if YCL_Android
void
LogAssert(bool expr, const char* expr_str, const char* file, int line,
	const char* msg)
{
	if(YB_UNLIKELY(!expr))
		::__android_log_assert(expr_str, "YFramework",
			"Assertion failed @ \"%s\":%i:\n %s .\nMessage: \n%s\n", file, line,
			expr_str, msg);
}


int
MapAndroidLogLevel(Descriptions::RecordLevel lv)
{
	return lv < Descriptions::Critical ? ANDROID_LOG_FATAL
		: ANDROID_LOG_FATAL - (std::uint8_t(lv) - 0x40) / 0x20;
}


AndroidLogSender::AndroidLogSender(const std::string& t)
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

