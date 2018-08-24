/*
	© 2012, 2015, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YConsole.h
\ingroup Core
\brief 通用控制台接口。
\version r91
\author FrankHB <frankhb1989@gmail.com>
\since build 585
\par 创建时间:
	2015-03-17 17:44:34 +0800
\par 修改时间:
	2018-08-19 11:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YConsole
*/


#ifndef YSL_INC_Core_YConsole_h_
#define YSL_INC_Core_YConsole_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YGDIBase // for Drawing::ColorSpace;

namespace YSLib
{

/*!
\brief 控制台接口。
\since build 328
*/
namespace Consoles
{

//! \since build 585
namespace ColorSpace = Drawing::ColorSpace;

/*!
\brief 控制台颜色枚举。
\since build 416
*/
enum Color
{
	Black = 0,
	DarkBlue,
	DarkGreen,
	DarkCyan,
	DarkRed,
	DarkMagenta,
	DarkYellow,
	Gray,
	DarkGray,
	Blue,
	Green,
	Cyan,
	Red,
	Magenta,
	Yellow,
	White
};

/*!
\brief 控制台颜色。
\note 顺序和 Consoles::Color 对应。
\since build 328
*/
yconstexpr const ColorSpace::ColorSet ConsoleColors[]{ColorSpace::Black,
	ColorSpace::Navy, ColorSpace::Green, ColorSpace::Teal, ColorSpace::Maroon,
	ColorSpace::Purple, ColorSpace::Olive, ColorSpace::Silver, ColorSpace::Gray,
	ColorSpace::Blue, ColorSpace::Lime, ColorSpace::Aqua, ColorSpace::Red,
	ColorSpace::Yellow, ColorSpace::Fuchsia, ColorSpace::White};

} // namespace Consoles;

} // namespace YSLib;

#endif

