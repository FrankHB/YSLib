/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YException.cpp
\ingroup Core
\brief 异常处理模块。
\version r339
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-06-15 20:30:14 +0800
\par 修改时间:
	2015-05-18 21:44 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YException
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YException
#include <ystdex/exception.h> // for ystdex::handle_nested;

namespace YSLib
{

LoggedEvent::LoggedEvent(const std::string& s, LevelType lv)
	: GeneralEvent(s),
	level(lv)
{}
LoggedEvent::LoggedEvent(const GeneralEvent& e, LevelType lv)
	: GeneralEvent(e),
	level(lv)
{}
ImplDeDtor(LoggedEvent)


FatalError::FatalError(const std::string& t, const std::string& c)
	: GeneralEvent(t), content(c)
{}
ImplDeDtor(FatalError)


void
TraceException(const char* str, LoggedEvent::LevelType lv, size_t level)
	ynothrow
{
	TryExpr(
		YCL_TraceRaw(lv, "%s%s", std::string(level, ' ').c_str(), Nonnull(str)))
	CatchExpr(..., YCL_TraceRaw(Critical, "Failure @ TraceException."))
}

void
TraceExceptionType(std::exception& e, LoggedEvent::LevelType lv) ynothrow
{
	YCL_TraceRaw(lv, "Caught std::exception[%s]: %s", typeid(e).name(),
		e.what());
}


void
ExtractException(const ExtractedExceptionPrinter& print,
	const std::exception& e, LoggedEvent::LevelType lv, size_t level)
	ynothrow
{
	TryExpr(print(e.what(), lv, level))
	CatchExpr(..., print("Exception occurred when printing @ ExtractException.",
		Critical, level))
	// FIXME: Following code only tested OK for %YCL_Win32.
	TryExpr(ystdex::handle_nested(e,
		[&, lv, level](std::exception& ex) ynothrow{
		ExtractException(print, ex, lv, level + 1);
	}))
	CatchExpr(..., print("Unknown nested exception found nested on calling"
		" ystdex::handle_nested @ ExtractException.", Critical, level))
}

bool
TryExecute(std::function<void()> f, const char* desc,
	LoggedEvent::LevelType lv)
{
	const ExtractedExceptionPrinter paint(TraceException);

	try
	{
		TryExpr(f())
		catch(...)
		{
			if(desc)
				YCL_TraceRaw(Notice, "Exception filtered: %s.", desc);
			throw;
		}
		return {};
	}
	CatchExpr(std::exception& e, TraceExceptionType(e, lv),
		ExtractException(paint, e, lv))
	return true;
}

} // namespace YSLib;

