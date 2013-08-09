/*
	Copyright by FrankHB 2010 - 2011, 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YComponent.cpp
\ingroup UI
\brief 平台中立的 UI 组件。
\version r955
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-19 20:05:18 +0800
\par 修改时间:
	2013-08-05 21:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YComponent
*/


#include "YSLib/UI/YComponent.h"

namespace YSLib
{

namespace Drawing
{

string
to_string(const Size& s)
{
	using YSLib::to_string;

	return "(" + to_string(s.Width) + ", " + to_string(s.Height) + ')';
}

string
to_string(const Rect& r)
{
	using YSLib::to_string;

	return "(" + to_string(r.X) + ", " + to_string(r.Y) + ", "
		+ to_string(r.Width) + ", " + to_string(r.Height) + ')';
}

} // namespace Drawing;

} // namespace YSLib;

