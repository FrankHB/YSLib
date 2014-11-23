/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Timer.h
\ingroup YCLib
\brief 平台相关的计时器接口。
\version r100
\author FrankHB <frankhb1989@gmail.com>
\since build 313
\par 创建时间:
	2012-06-01 14:42:34 +0800
\par 修改时间:
	2014-11-21 12:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Timer
*/


#ifndef YCL_INC_Timer_h_
#define YCL_INC_Timer_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon

namespace platform
{

/*!
\brief 取 tick 数。
\note 单位为毫秒。
\warning 首次调用 StartTicks 前非线程安全。
\since build 555
*/
YF_API std::uint32_t
GetTicks() ynothrow;

/*!
\brief 取高精度 tick 数。
\note 单位为纳秒。
\warning 首次调用 StartTicks 前非线程安全。
\since build 555
*/
YF_API std::uint64_t
GetHighResolutionTicks() ynothrow;

} // namespace platform;

#endif

