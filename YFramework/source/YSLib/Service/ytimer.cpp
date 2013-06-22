/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ytimer.cpp
\ingroup Service
\brief 计时器服务。
\version r798
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-06-05 10:28:58 +0800
\par 修改时间:
	2013-06-20 20:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YTimer
*/


#include "YSLib/Service/ytimer.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Timers)

namespace
{

bool NotInitialized(true);

void
Init(Timer& tmr, bool b)
{
	if(YB_UNLIKELY(NotInitialized))
	{
		StartTicks();
		NotInitialized = false;
	}
	if(b)
		Activate(tmr);
}


inline TimePoint
GetTickPoint()
{
	return TimePoint(TimeSpan(GetTicks()));
}

} // unnamed namespace;

void
Delay(const TimeSpan& ms)
{
	const auto end(GetTickPoint() + ms);

	while(GetTickPoint() < end)
		;
}


Timer::Timer(const Duration& i, bool b)
	: nBase(), Interval(i)
{
	Init(*this, b);
}

bool
Timer::Refresh()
{
	const auto tick(HighResolutionClock::now());

	if(YB_LIKELY(tick < nBase + Interval))
		return false;
	nBase = tick - (tick - nBase) % Interval;
	return true;
}

void
Activate(Timer& tmr)
{
	if(tmr.Interval != Duration::zero())
		tmr.nBase = HighResolutionClock::now();
}

YSL_END_NAMESPACE(Timers)

YSL_END

