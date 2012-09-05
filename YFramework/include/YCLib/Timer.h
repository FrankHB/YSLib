/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Timer.h
\ingroup YCLib
\brief 平台相关的计时器接口。
\version r75
\author FrankHB<frankhb1989@gmail.com>
\since build 313
\par 创建时间:
	2012-06-01 14:42:34 +0800
\par 修改时间:
	2012-09-04 12:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Timer
*/


#ifndef YCL_INC_TIMER_H_
#define YCL_INC_TIMER_H_ 1

#include "ycommon.h"

namespace platform
{

/*!
\brief 开始 tick 计时。
\warning 首次调用前非线程安全。
*/
void
StartTicks();

/*!
\brief 取 tick 数。
\note 单位为毫秒。
\note 调用 StartTicks 。
\warning 首次调用 StartTicks 前非线程安全。
*/
std::uint32_t
GetTicks();

/*!
\brief 取高精度 tick 数。
\note 单位为纳秒。
\note 调用 StartTicks 。
\warning 首次调用 StartTicks 前非线程安全。
\since build 291
*/
std::uint64_t
GetHighResolutionTicks();

} // namespace platform;

#endif

