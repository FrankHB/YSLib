/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Keys.h
\ingroup YCLib
\brief 平台相关的基本按键输入定义。
\version r144;
\author FrankHB<frankhb1989@gmail.com>
\since build 313 。
\par 创建时间:
	2009-06-01 14:29:56 +0800;
\par 修改时间:
	2012-08-30 20:06 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::Keys;
*/


#ifndef YCL_INC_KEYS_H_
#define YCL_INC_KEYS_H_ 1

#include "ycommon.h"
#include <bitset>

namespace platform
{

#if YCL_DS
/*!
\brief 按键并行位宽。
\note 不少于实际表示的 KeyPad 按键数。
\since build 298 。
*/
yconstexpr std::size_t KeyBitsetWidth(32);
#elif YCL_MINGW32
/*!
\brief 按键并行位宽。
\note 不少于实际表示的键盘按键数。
\since build 298 。
*/
yconstexpr std::size_t KeyBitsetWidth(256);
#else
#	error Unsupport platform found!
#endif

/*!
\brief 本机输入类型。
\since build 298 。

支持并行按键的缓冲区数据类型。
*/
typedef std::bitset<KeyBitsetWidth> KeyInput;


/*!
\brief 本机按键编码。
\note MinGW32 下的 KeyCodes 中仅含每个实现默认需要支持的基本集合。
\since build 298 。
*/
namespace KeyCodes
{

#if YCL_DS
typedef enum
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
} NativeSet;

//按键别名。
const NativeSet
	Enter = A,
	Esc = B,
	PgUp = L,
	PgDn = R;
#elif YCL_MINGW32
#define VK_RETURN 0x0D
#define VK_ESCAPE 0x1B
#define VK_PRIOR 0x21
#define VK_NEXT 0x22
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
/*!
\brief 基本公用按键集合。
\since build 297 。
*/
//@{
typedef enum
{
	Empty	= 0,
	Enter	= VK_RETURN,
	Esc		= VK_ESCAPE,
	PgUp	= VK_PRIOR,
	PgDn	= VK_NEXT,
	Left	= VK_LEFT,
	Up		= VK_UP,
	Right	= VK_RIGHT,
	Down	= VK_DOWN
} NativeSet;
//@}
#undef VK_RETURN
#undef VK_ESCAPE
#undef VK_PRIOR
#undef VK_NEXT
#undef VK_LEFT
#undef VK_UP
#undef VK_RIGHT
#undef VK_DOWN
#else
#	error Unsupport platform found!
#endif

} // namespace KeyCodes;

} // namespace platform_ex;

#endif

