/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yconsole.cpp
\ingroup Shell
\brief 平台无关的控制台。
\version 0.1050;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-20 09:28:39 +0800;
\par 修改时间:
	2011-05-14 20:39 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Console
*/


#include "yconsole.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

void
Activate(Console& c, Drawing::Color fc, Drawing::Color bc)
{
	InitConsole(c.Screen, fc, bc);
}

void
Deactivate(Console&)
{
	InitVideo();
}

YSL_END_NAMESPACE(Components)

YSL_END

