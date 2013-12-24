/*
	© 2011-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Console.cpp
\ingroup UI
\brief 基于平台中立接口的控制台实现。
\version r90
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2011-04-20 09:28:39 +0800
\par 修改时间:
	2013-12-24 10:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Console
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Console
#include YFM_YCLib_Input // platform::WaitForInput;

namespace YSLib
{

namespace UI
{

void
Console::Pause()
{
	platform::WaitForInput();
}

void
Activate(Console&, Drawing::Color, Drawing::Color)
{
}

void
Deactivate(Console&)
{
}

} // namespace UI;

} // namespace YSLib;

