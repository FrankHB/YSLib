/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup YCLib YCommonLib
\brief YSLib 基础库。
*/

/*!	\file ycommon.h
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version r4234;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-12 22:14:28 +0800;
\par 修改时间:
	2012-05-30 23:03 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::YCommon;
*/


#ifndef YCL_INC_YCOMMON_H_
#define YCL_INC_YCOMMON_H_

//平台定义。
#include "Platform.h"

//平台无关部分。
#include <ydef.h>
#include <ystdex/cstdio.h>
#include <cstdlib>
#include <bitset>

//平台相关部分。
//#include <unistd.h>

//! \brief 默认平台命名空间。
namespace platform
{

/*!
\brief 主内存块设置。

满足条件时使用平台特定算法覆盖内存区域的每个字节，否则使用 std::memset 。
\note 参数和返回值语义同 std::memset 。
\warning 仅进行内存区域检查，不进行空指针或其它检查。
*/
void*
mmbset(void*, int, std::size_t);

/*!
\brief 主内存块复制。

满足条件时使用平台特定算法复制内存区域，否则使用 std::memcpy 。
\note 参数和返回值语义同 std::memcpy 。
\warning 仅进行内存区域检查，不进行空指针或其它检查。
*/
void*
mmbcpy(void*, const void*, std::size_t);


/*!
\brief 异常终止函数。
*/
void
terminate();


#ifdef YCL_DS
/*!
\brief 按键并行位宽。
\note 不少于实际表示的 KeyPad 按键数。
\since build 298 。
*/
yconstexpr std::size_t KeyBitsetWidth(32);
#elif defined(YCL_MINGW32)
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

#ifdef YCL_DS
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
#elif defined(YCL_MINGW32)
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


//断言。
#ifdef YCL_USE_YASSERT

#undef YAssert

/*!
\brief YCLib 默认断言函数。
\note 当定义 YCL_USE_YASSERT 宏时，宏 YAssert 操作由此函数实现。
*/
void
yassert(bool, const char*, const char*, int, const char*);

#define YAssert(exp, message) \
	platform::yassert(exp, #exp, message, __LINE__, __FILE__)

#else

#	include <cassert>
#	define YAssert(exp, message) assert(exp)

#endif


/*!
\brief 开始 tick 计时。
*/
void
StartTicks();

/*!
\brief 取 tick 数。
\note 单位为毫秒。
*/
std::uint32_t
GetTicks();

/*!
\brief 取高精度 tick 数。
\note 单位为纳秒。
\since build 291 。
*/
std::uint64_t
GetHighResolutionTicks();

} // namespace platform;

namespace platform_ex
{

#ifdef YCL_DS

/*!
\brief 设置允许设备进入睡眠的标识状态。
\return 旧状态。
\note 默认状态为 true 。
\since build 278 。
*/
bool
AllowSleep(bool);

#elif defined(YCL_MINGW32)
// TODO: add WinAPIs;
#else
#	error Unsupport platform found!
#endif

} // namespace platform_ex;

#endif

