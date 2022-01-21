/*
	© 2012-2015, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YClock.h
\ingroup Core
\brief 时钟接口。
\version r91
\author FrankHB <frankhb1989@gmail.com>
\since build 584
\par 创建时间:
	2015-03-17 02:09:02 +0800
\par 修改时间:
	2021-12-29 01:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YClock
*/


#ifndef YSL_INC_Core_YClock_h_
#define YSL_INC_Core_YClock_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YShellDefinition
#include <chrono> // for mandated header;

namespace YSLib
{

namespace Timers
{

/*!
\brief 高精度时钟。
\since build 291
*/
class YF_API HighResolutionClock
{
public:
	using duration = std::chrono::duration<
		ystdex::make_signed_t<std::chrono::nanoseconds::rep>, std::nano>;
	using rep = duration::rep;
	using period = duration::period;
	using time_point = std::chrono::time_point<HighResolutionClock,
		std::chrono::nanoseconds>;

	static yconstexpr const bool is_steady = {};

	//! \warning 首次调用前非线程安全。
	static PDefH(time_point, now, ) ynothrow
		ImplRet(time_point(std::chrono::nanoseconds(GetHighResolutionTicks())))
};


/*!
\brief 高精度时间间隔。
\note 单位为纳秒。
\since build 291
*/
using Duration = HighResolutionClock::duration;

/*!
\brief 时刻。
\since build 291
*/
using TimePoint = HighResolutionClock::time_point;

/*!
\brief 低精度时间间隔。
\note 单位为毫秒。
\since build 291
*/
using TimeSpan = std::chrono::duration<Duration::rep, std::milli>;

} // namespace Timers;

} // namespace YSLib;

#endif

