/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yexcept.cpp
\ingroup Core
\brief 异常处理模块。
\version r280
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-06-15 20:30:14 +0800
\par 修改时间:
	2014-10-13 21:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YException
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YException

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


FatalError::FatalError(const char* t, const char* c)
	: GeneralEvent(t), content(c)
{}

} // namespace YSLib;

