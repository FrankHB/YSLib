/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ytimer.cpp
\ingroup Service
\brief 计时器服务。
\version r1702;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-06-05 10:28:58 +0800;
\par 修改时间:
	2012-03-07 21:36 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Service::YTimer;
*/


#include "YSLib/Service/ytimer.h"
#include <ystdex/iterator.hpp>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Timers)

namespace
{
	bool NotInitialized(true);

	void
	Init(Timer& tmr, bool b)
	{
		if(NotInitialized)
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
}

void
Delay(const TimeSpan& ms)
{
	const auto end(GetTickPoint() + ms);

	while(GetTickPoint() < end)
		;
}


Timer::TimerMap Timer::mTimers;

Timer::Timer(u32 i, bool b)
	: nBase(), nInterval(i * 1000000ULL)
{
	Init(*this, b);
}
Timer::Timer(const Duration& i, bool b)
	: nBase(), nInterval(i)
{
	Init(*this, b);
}


bool
Timer::IsActive() const
{
	try
	{
		mTimers.at(GetObjectID());
		return true;
	}
	catch(std::out_of_range&)
	{}
	return false;
}

void
Timer::SetInterval(TimeSpan i)
{
	nInterval = i;
	if(nInterval == Duration::zero())
		Deactivate(*this);
}

bool
Timer::Refresh()
{
	const auto tick(HighResolutionClock::now());

	if(tick < nBase + nInterval)
		return false;
	nBase = tick - (tick - nBase) % nInterval;
	return true;
}

bool
Timer::RefreshAll()
{
	using ystdex::get_value;

	bool t(false);

	std::for_each(mTimers.begin() | get_value, mTimers.end() | get_value,
		[&](Timer* const& pTmr){
		if(pTmr)
			t |= pTmr->Refresh();
	});
	return t;
}

void
Timer::ResetAll()
{
	using ystdex::get_value;

	std::for_each(mTimers.begin() | get_value, mTimers.end() | get_value,
		[](Timer* const& pTmr){
		if(pTmr)
			pTmr->Reset();
	});
}

void
Activate(Timer& tmr)
{
	if(tmr.nInterval != Duration::zero())
	{
		Timer::mTimers.insert(make_pair(tmr.GetObjectID(), &tmr));
		tmr.nBase = HighResolutionClock::now();
	}
}

void
Deactivate(Timer& tmr)
{
	Timer::mTimers.erase(tmr.GetObjectID());
}

YSL_END_NAMESPACE(Timers)

YSL_END

