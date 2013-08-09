/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Timer.cpp
\ingroup YCLib
\brief 平台相关的计时器接口。
\version r196
\author FrankHB <frankhb1989@gmail.com>
\since build 313
\par 创建时间:
	2012-06-01 14:44:52 +0800
\par 修改时间:
	2013-08-05 21:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Timer
*/


#include "YCLib/Timer.h"
#include "YCLib/NativeAPI.h"
#if YCL_MinGW32
#	include <mmsystem.h> // for multimedia timers;
#endif

namespace platform
{

namespace
{
	bool bUninitializedTimers(true);
#if YCL_DS
	volatile std::uint32_t system_tick;

	void
	timer_callback()
	{
		++system_tick;
	}
#elif YCL_MinGW32
	std::uint32_t(*p_tick_getter)(); //!< 计时器指针。
	std::uint64_t(*p_tick_getter_nano)(); //!< 高精度计时器指针。
	union
	{
		::DWORD start;
		struct
		{
			::LARGE_INTEGER start;
			::LARGE_INTEGER tps; //!< 每秒 tick 数。
		} hi; //!< 高精度计时器状态。
	} g_ticks; //!< 计时器状态。

	std::uint32_t
	get_ticks_hi_res()
	{
		::LARGE_INTEGER now;

		::QueryPerformanceCounter(&now);
		now.QuadPart -= g_ticks.hi.start.QuadPart;
		now.QuadPart *= 1000;
		return ::DWORD(now.QuadPart / g_ticks.hi.tps.QuadPart);
	}

	std::uint64_t
	get_ticks_hi_res_nano()
	{
		::LARGE_INTEGER now;

		::QueryPerformanceCounter(&now);
		now.QuadPart -= g_ticks.hi.start.QuadPart;
		now.QuadPart *= 1000000000;
		return now.QuadPart / g_ticks.hi.tps.QuadPart;
	}

	std::uint32_t
	get_ticks_mm()
	{
		::DWORD now(::timeGetTime());

		return now < g_ticks.start ? (~::DWORD(0) - g_ticks.start) + now
			: now - g_ticks.start;
	}

	std::uint64_t
	get_ticks_mm_nano()
	{
		return get_ticks_mm() * 1000000;
	}
#endif
}

void
StartTicks()
{
	if(bUninitializedTimers)
	{
#if YCL_DS
		// f = 33.513982MHz;
		// BUS_CLOCK = 33513982 = 2*311*53881;
#if 0
		::irqSet(IRQ_TIMER(2), timer_callback);
		::irqEnable(IRQ_TIMER(2));
		TIMER2_DATA = 0;
		TIMER2_CR = TIMER_ENABLE | ::ClockDivider_1;
#endif
		::timerStart(2, ::ClockDivider_1, u16(TIMER_FREQ(1000)),
			timer_callback);
#elif YCL_MinGW32
		if(::QueryPerformanceFrequency(&g_ticks.hi.tps))
		{
			yunseq(p_tick_getter = get_ticks_hi_res,
				p_tick_getter_nano = get_ticks_hi_res_nano),
			::QueryPerformanceCounter(&g_ticks.hi.start);
		}
		else
		{
			yunseq(p_tick_getter = get_ticks_mm,
				p_tick_getter_nano = get_ticks_mm_nano),
			::timeBeginPeriod(1); //精度 1 毫秒。
			// FIXME: ::timeEndPeriod shall be used at exit;
			g_ticks.start = ::timeGetTime();
		}
#else
#	error "Unsupported platform found."
#endif
		bUninitializedTimers = {};
	};
}

std::uint32_t
GetTicks()
{
	StartTicks();
#if YCL_DS
	return system_tick;
#elif YCL_MinGW32
	return p_tick_getter();
#endif
}

std::uint64_t
GetHighResolutionTicks()
{
	StartTicks();
#if YCL_DS
	return system_tick * 1000000ULL
		+ TIMER2_DATA * 1000000ULL / BUS_CLOCK;
#elif YCL_MinGW32
	return p_tick_getter_nano();
#endif
}

}

