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
\version r1563;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-06-05 10:28:58 +0800;
\par 修改时间:
	2011-09-16 04:05 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::YTimer;
*/


#include "ytimer.h"

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
	for(auto i(mTimers.begin()); i != mTimers.end(); ++i)
		if(i->second)
			t |= i->second->RefreshRaw();
	return t;
}

void
Timer::ResetAll()
{
	for(auto i(mTimers.begin()); i != mTimers.end(); ++i)
		if(i->second)
			i->second->Reset();
}

void
Timer::ResetYTimer()
{
	for(auto i(mTimers.begin()); i != mTimers.end(); ++i)
	{
		Timer* const p(i->second);
		if(p)
		{
			p->SetInterval(0);
			p->Reset();
		}
	}
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

