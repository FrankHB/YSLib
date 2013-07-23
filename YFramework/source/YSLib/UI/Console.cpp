/*
	Copyright by FrankHB 2011 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Console.cpp
\ingroup UI
\brief 基于平台中立接口的控制台实现。
\version r82
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2011-04-20 09:28:39 +0800
\par 修改时间:
	2013-07-23 18:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Console
*/


#include "YSLib/UI/Console.h"
#include "YCLib/Input.h" // platform::WaitForInput;

YSL_BEGIN

YSL_BEGIN_NAMESPACE(UI)

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

YSL_END_NAMESPACE(UI)

YSL_END

