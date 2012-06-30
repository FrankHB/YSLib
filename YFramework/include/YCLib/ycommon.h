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
\version r4342;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-12 22:14:28 +0800;
\par 修改时间:
	2012-06-28 11:18 +0800;
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
\note 参数和返回值语义同 std::memset 。
\warning 仅进行内存区域检查，不进行空指针或其它检查。
\since build 319 。

满足条件时使用平台特定算法覆盖内存区域的每个字节，否则使用 std::memset 。
*/
void*
mmbset(void*, int, std::size_t) ynothrow;

/*!
\brief 主内存块复制。
\note 参数和返回值语义同 std::memcpy 。
\warning 仅进行内存区域检查，不进行空指针或其它检查。
\since build 319 。

满足条件时使用平台特定算法复制内存区域，否则使用 std::memcpy 。
*/
void*
mmbcpy(void*, const void*, std::size_t) ynothrow;


/*!
\brief 异常终止函数。
\since build 319 。
*/
void
terminate() ynothrow;


//断言。
#ifdef YB_USE_YASSERT

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

#include <cassert>
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

#if YCL_DS

/*!
\brief 设置允许设备进入睡眠的标识状态。
\return 旧状态。
\note 默认状态为 true 。
\warning 非线程安全。
\since build 278 。
*/
bool
AllowSleep(bool);

#elif YCL_MINGW32
// TODO: Add Windows specified APIs.
#else
#	error Unsupport platform found!
#endif

} // namespace platform_ex;

#endif

