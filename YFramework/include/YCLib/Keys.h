/*
	© 2012-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Keys.h
\ingroup YCLib
\brief 平台相关的基本按键输入定义。
\version r195
\author FrankHB <frankhb1989@gmail.com>
\since build 313
\par 创建时间:
	2012-06-01 14:29:56 +0800
\par 修改时间:
	2013-11-30 23:01 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Keys
*/


#ifndef YCL_INC_Keys_h_
#define YCL_INC_Keys_h_ 1

#include "ycommon.h"
#include <bitset>

namespace platform
{

#if YCL_DS
/*!
\brief 按键并行位宽。
\note 不少于实际表示的 KeyPad 按键数。
\since build 298
*/
yconstexpr std::size_t KeyBitsetWidth(32);
#elif YCL_Win32
/*!
\brief 按键并行位宽。
\note 不少于实际表示的键盘按键数。
\since build 298
*/
yconstexpr std::size_t KeyBitsetWidth(256);
#else
#	error "Unsupported platform found."
#endif

/*!
\brief 本机输入类型。
\since build 298

支持并行按键的缓冲区数据类型。
*/
using KeyInput = std::bitset<KeyBitsetWidth>;


/*!
\brief 本机按键编码。
\note MinGW32 下的 KeyCodes 中仅含每个实现默认需要支持的基本集合。
\since build 298
*/
namespace KeyCodes
{

#if YCL_DS
//! \since build 416
enum NativeSet
{
	A		= 0,
	B		= 1,
	Select	= 2,
	Start	= 3,
	Right	= 4,
	Left	= 5,
	Up		= 6,
	Down	= 7,
	R		= 8,
	L		= 9,
	X		= 10,
	Y		= 11,
	Touch	= 12,
	Lid		= 13
};

//按键别名。
const NativeSet
	Enter = A,
	Esc = B,
	PgUp = L,
	PgDn = R;
#elif YCL_Win32
/*!
\brief 基本公用按键集合。
\note 值和 WinSDK 的 VK_* 宏替换结果对应。
\since build 416
*/
enum NativeSet
{
	Empty	= 0,
	Enter	= 0x0D, //!< 同 VK_RETURN 。
	Esc		= 0x1B, //!< 同 VK_ESCAPE 。
	PgUp	= 0x21, //!< 同 VK_PRIOR 。
	PgDn	= 0x22, //!< 同 VK_NEXT 。
	Left	= 0x25, //!< 同 VK_LEFT 。
	Up		= 0x26, //!< 同 VK_UP 。
	Right	= 0x27, //!< 同 VK_RIGHT 。
	Down	= 0x28 //!< 同 VK_DOWN 。
};
#undef VK_RETURN
#undef VK_ESCAPE
#undef VK_PRIOR
#undef VK_NEXT
#undef VK_LEFT
#undef VK_UP
#undef VK_RIGHT
#undef VK_DOWN
#endif

} // namespace KeyCodes;

} // namespace platform_ex;

#endif

