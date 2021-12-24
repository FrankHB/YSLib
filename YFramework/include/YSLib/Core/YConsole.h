/*
	© 2012, 2015, 2018, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YConsole.h
\ingroup Core
\brief 通用控制台接口。
\version r104
\author FrankHB <frankhb1989@gmail.com>
\since build 585
\par 创建时间:
	2015-03-17 17:44:34 +0800
\par 修改时间:
	2021-12-21 21:22 +0800
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
\see ISO/IEC 6429:1992 8.3.118 。
\see https://invisible-island.net/ncurses/ 。

枚举值的数值为颜色代码，同 SVr4 curses 实现。
Microsoft Windows 控制台和 FreeBSD syscons 也使用相同的颜色代码。
和 ANSI 转义序列使用的 SGR 代码的顺序存在差异，参见下列 ncurses 的实现文件：
ncurses/base/lib_color.c
ncurses/tinfo/tinfo_driver.c
ncurses/win32con/windriver.c
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
\sa ColorSpace::ColorSet
\see https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit 。

基准控制台颜色。
指派 Microsoft Windows XP 控制台使用的相同的饱和 8 色颜色值。
*/
yconstexpr const ColorSpace::ColorSet ConsoleColors[]{ColorSpace::Black,
	ColorSpace::Navy, ColorSpace::Green, ColorSpace::Teal, ColorSpace::Maroon,
	ColorSpace::Purple, ColorSpace::Olive, ColorSpace::Silver, ColorSpace::Gray,
	ColorSpace::Blue, ColorSpace::Lime, ColorSpace::Aqua, ColorSpace::Red,
	ColorSpace::Yellow, ColorSpace::Fuchsia, ColorSpace::White};

} // namespace Consoles;

} // namespace YSLib;

#endif

