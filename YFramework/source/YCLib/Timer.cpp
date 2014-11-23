/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Timer.cpp
\ingroup YCLib
\brief 平台相关的计时器接口。
\version r324
\author FrankHB <frankhb1989@gmail.com>
\since build 313
\par 创建时间:
	2012-06-01 14:44:52 +0800
\par 修改时间:
	2014-11-21 12:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Timer
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Timer
#include YFM_YCLib_NativeAPI
#include <system_error>
#if YCL_Win32 || YCL_Android
#	include <chrono>
#endif

namespace platform
{

namespace
{

#if YCL_DS
bool bUninitializedTimers(true);
volatile std::uint32_t system_tick;

void
timer_callback()
{
	++system_tick;
}

//! \since build 539
void
start_ticks() ynothrow
{
	if(YB_UNLIKELY(bUninitializedTimers))
	{
		// f = 33.513982MHz, BUS_CLOCK = 33513982 = 2*311*53881;
#if 0
		::irqSet(IRQ_TIMER(2), timer_callback);
		::irqEnable(IRQ_TIMER(2));
		TIMER2_DATA = 0;
		TIMER2_CR = TIMER_ENABLE | ::ClockDivider_1;
#endif
		::timerStart(2, ::ClockDivider_1, u16(TIMER_FREQ(1000)),
			timer_callback);
	};
	bUninitializedTimers = {};
}

#endif

} // unnamed namespace;

std::uint32_t
GetTicks() ynothrow
{
#if YCL_DS
	start_ticks();
	return system_tick;
#elif YCL_Win32 || YCL_Android
	return std::uint32_t(std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now().time_since_epoch()).count());
#else
#	error "Unsupported platform found."
#endif
}

std::uint64_t
GetHighResolutionTicks() ynothrow
{
#if YCL_DS
	start_ticks();
	return system_tick * 1000000ULL + TIMER2_DATA * 1000000ULL / BUS_CLOCK;
#elif YCL_Win32 || YCL_Android
	return std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::steady_clock::now().time_since_epoch()).count());
#endif
}

}

