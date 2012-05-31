/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycommon.cpp
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version r3492;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-12 22:14:42 +0800;
\par 修改时间:
	2012-05-30 23:10 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::YCommon;
*/


#include "YCLib/Debug.h"
#include <cstring>
#include <cerrno>
#include "YCLib/NativeAPI.h"
#ifdef YCL_MINGW32
#include <mmsystem.h> // for multimedia timers;
#endif

namespace platform
{

void*
mmbset(void* d, int v, std::size_t t)
{
#ifdef YCL_DS
	// NOTE: DMA fill to main RAM is maybe slower;
//	return safe_dma_fill(d, v, t) != 0 ? std::memset(d, v, t) : d;
#endif
	return std::memset(d, v, t);
}

void*
mmbcpy(void* d, const void* s, std::size_t t)
{
#ifdef YCL_DS
	// NOTE: DMA copy to main RAM is even slower;
	// TODO: use ASM optimization, like using LDMIA instructions;
//	return safe_dma_copy(d, s, t) != 0 ? std::memcpy(d, s, t) : d;
#endif
	return std::memcpy(d, s, t);
}


void
terminate()
{
#ifdef YCL_DS
	for(;;)
		::swiWaitForVBlank();
#else
	std::terminate();
#endif
}


#ifdef YCL_USE_YASSERT

void
yassert(bool exp, const char* expstr, const char* message,
	int line, const char* file)
{
	if(YCL_UNLIKELY(!exp))
	{
		yprintf("Assertion failed: \n"
			"%s\nMessage: \n%s\nAt line %i in file \"%s\".\n",
			expstr, message, line, file);
		std::abort();
	}
}

#endif


namespace
{
	bool bUninitializedTimers(true);
#ifdef YCL_DS
	volatile std::uint32_t system_tick;

	void
	timer_callback()
	{
		++system_tick;
	}
#elif defined(YCL_MINGW32)
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
#ifdef YCL_DS
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
#elif defined(YCL_MINGW32)
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
#	error Unsupported platform found!
#endif
		bUninitializedTimers = false;
	};
}

std::uint32_t
GetTicks()
{
	StartTicks();
#ifdef YCL_DS
	return system_tick;
#elif defined(YCL_MINGW32)
	return p_tick_getter();
#else
#	error Unsupported platform found!
#endif
}

std::uint64_t
GetHighResolutionTicks()
{
	StartTicks();
#ifdef YCL_DS
	return system_tick * 1000000ULL
		+ TIMER2_DATA * 1000000ULL / BUS_CLOCK;
#elif defined(YCL_MINGW32)
	return p_tick_getter_nano();
#else
#	error Unsupported platform found!
#endif
}

}

namespace platform_ex
{

#ifdef YCL_DS
bool
AllowSleep(bool b)
{
	static bool bSleepEnabled(true); //与 libnds 默认的 ARM7 电源管理同步。
	const bool b_old(bSleepEnabled);

	if(b != bSleepEnabled)
	{
		bSleepEnabled = b;
		::fifoSendValue32(FIFO_PM,
			b ? PM_REQ_SLEEP_ENABLE : PM_REQ_SLEEP_DISABLE);
	}
	return b_old;
}
#endif

}

