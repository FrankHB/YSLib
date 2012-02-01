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
\version r1580;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-06-05 10:28:58 +0800;
\par 修改时间:
	2012-01-31 05:55 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Service::YTimer;
*/


#include "YSLib/Service/ytimer.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Timers)

bool Timer::NotInitialized(true);
vu32 Timer::SystemTick(0);
Timer::TimerMap Timer::mTimers;

Timer::Timer(TimeSpan i, bool a)
	: nInterval(i), nBase(0)
{
	InitializeSystemTimer();
	if(a)
		Activate(*this);
}

void
Timer::SetInterval(TimeSpan i)
{
	nInterval = i;
	if(!nInterval)
		Deactivate(*this);
}

void
Timer::InitializeSystemTimer()
{
	if(NotInitialized)
		ResetRTC();
}

void
Timer::ResetSystemTimer()
{
	NotInitialized = true;
	InitializeSystemTimer();
}

bool
Timer::RefreshRaw()
{
	if(SystemTick < nBase + nInterval)
		return false;
	nBase = SystemTick - (SystemTick - nBase) % nInterval;
	return true;
}

bool
Timer::Refresh()
{
	Synchronize();
	return RefreshRaw();
}

bool
Timer::RefreshAll()
{
	bool t(false);

	Synchronize();
	std::for_each(mTimers.begin(), mTimers.end(),
		[&](decltype(*mTimers.begin())& pr){
		if(pr.second)
			t |= pr.second->RefreshRaw();
	});
	return t;
}

void
Timer::ResetAll()
{
	std::for_each(mTimers.begin(), mTimers.end(),
		[](decltype(*mTimers.begin())& pr){
		if(pr.second)
			pr.second->Reset();
	});
}

void
Timer::ResetYTimer()
{
	std::for_each(mTimers.begin(), mTimers.end(),
		[](decltype(*mTimers.begin())& pr){
		const auto p(pr.second);

		if(p)
		{
			p->SetInterval(0);
			p->Reset();
		}
	});
	mTimers.clear();
	ResetSystemTimer();
}

void
Activate(Timer& t)
{
	if(t.nInterval != 0)
	{
		Timer::mTimers[t.GetObjectID()] = &t;
		t.Synchronize();
		t.nBase = Timer::SystemTick;
	}
}

void
Deactivate(Timer& t)
{
	Timer::mTimers[t.GetObjectID()] = nullptr;
}

YSL_END_NAMESPACE(Timers)

YSL_END

