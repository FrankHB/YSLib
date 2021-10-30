/*
	© 2010-2016, 2018-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YException.cpp
\ingroup Core
\brief 异常处理模块。
\version r448
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-06-15 20:30:14 +0800
\par 修改时间:
	2021-10-23 00:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YException
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YException // for Nonnull, Logger, to_std_string,
//	ystdex::bind1, std::placeholders::_2;
#include <ystdex/exception.h> // for ystdex::handle_nested;
#include <cstdio> // for std::fprintf, stderr;

namespace YSLib
{

LoggedEvent::LoggedEvent(const char* str, RecordLevel lv)
	: GeneralEvent(Nonnull(str)),
	level(lv)
{}
LoggedEvent::LoggedEvent(const std::string& str, RecordLevel lv)
	: GeneralEvent(str),
	level(lv)
{}
LoggedEvent::LoggedEvent(string_view sv, RecordLevel lv)
	: GeneralEvent(to_std_string(sv)),
	level(lv)
{}
LoggedEvent::LoggedEvent(const GeneralEvent& e, RecordLevel lv)
	: GeneralEvent(e),
	level(lv)
{}
ImplDeDtor(LoggedEvent)


FatalError::FatalError(const char* t, string_view c)
	: GeneralEvent(Nonnull(t)),
	content((yunused(Nonnull(c.data())), make_shared<string>(string(c))))
{}
ImplDeDtor(FatalError)


void
PrintCriticalFor(const ExtractedLevelPrinter& print, const char* str,
	RecordLevel lv, size_t level) ynothrow
{
	TryExpr(print(Nonnull(str), level))
	CatchExpr(..., YF_TraceRaw(lv < Critical ? lv : Critical,
		"Failure @ PrintCriticalFor."))
}

void
PrintMessage(const char* str, RecordLevel lv, size_t level) ynothrow
{
	// XXX: Format '%*c' may not work in some implementations of %ystdex::sfmt
	//	used as the default by %YF_TraceRaw.
	TryExpr(YF_TraceRaw(lv, "%*s%s", int(level), "", Nonnull(str)))
	CatchExpr(..., YF_TraceRaw(lv < Critical ? lv : Critical,
		"Failure @ PrintMessage."))
}

void
TraceExceptionType(const std::exception& e, RecordLevel lv) ynothrow
{
	YF_TraceRaw(lv, "Caught std::exception[%s].", typeid(e).name());
}

void
ExtractAndTrace(const std::exception& e, RecordLevel lv) ynothrow
{
	TraceExceptionType(e, lv);
	ExtractException(ystdex::bind1(PrintMessage, lv, std::placeholders::_2), e);
}

void
ExtractException(const ExtractedLevelPrinter& print, const std::exception& e,
	size_t level) ynothrow
{
	TryExpr(print(e.what(), level))
	CatchExpr(..., PrintCriticalFor(print, "Exception occurred when printing"
		" @ ExtractException.", Critical, level))
	TryExpr(ystdex::handle_nested(e, [&, level](std::exception& ex) ynothrow{
		ExtractException(print, ex, level + 1);
	}))
	CatchExpr(..., PrintCriticalFor(print, "Unknown nested exception found"
		" nested on calling ystdex::handle_nested @ ExtractException.",
		Critical, level))
}

} // namespace YSLib;

