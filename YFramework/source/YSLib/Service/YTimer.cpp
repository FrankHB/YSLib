/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YTimer.cpp
\ingroup Service
\brief 计时器服务。
\version r858
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-06-05 10:28:58 +0800
\par 修改时间:
	2015-03-21 16:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YTimer
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_YTimer

namespace YSLib
{

namespace Timers
{

namespace
{

bool NotInitialized(true);

//! \since build 555
void
InitClock() ynothrow
{
	if(YB_UNLIKELY(NotInitialized))
	{
		// NOTE: To initialize.
		GetTicks();
		NotInitialized = {};
	}
}


inline TimePoint
GetTickPoint()
{
	return TimePoint(TimeSpan(GetTicks()));
}

} // unnamed namespace;

void
Delay(TimeSpan delay)
{
	const auto end(GetTickPoint() + delay);

	while(GetTickPoint() < end)
		;
}


Timer::Timer(Duration d, bool b) ynothrow
	: nBase(), Interval(d)
{
	InitClock();
	if(b)
		Activate(*this);
}
ImplDeDtor(Timer)

bool
Timer::Refresh()
{
	YAssert(Interval != Duration::zero(), "Zero interval found.");

	const auto tick(HighResolutionClock::now());

	if(YB_LIKELY(tick < nBase + Interval))
		return {};
	nBase = tick - (tick - nBase) % Interval;
	return true;
}

Duration
Timer::RefreshDelta()
{
	YAssert(Interval != Duration::zero(), "Zero interval found.");
	const auto tick(HighResolutionClock::now());
	const auto delta(tick - nBase);

	if(YB_UNLIKELY(!(tick < nBase + Interval)))
		nBase = tick - delta % Interval;
	return delta;
}

Duration
Timer::RefreshRemainder()
{
	YAssert(Interval != Duration::zero(), "Zero interval found.");
	const auto tick(HighResolutionClock::now());
	const auto remainder((tick - nBase) % Interval);

	if(YB_UNLIKELY(!(tick < nBase + Interval)))
		nBase = tick - remainder;
	return remainder;
}

} // namespace Timers;

} // namespace YSLib;

