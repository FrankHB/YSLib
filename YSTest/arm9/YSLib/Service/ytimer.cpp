/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ytimer.cpp
\ingroup Service
\brief 计时器服务。
\version 0.1527;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-06-05 10:28:58 + 08:00;
\par 修改时间:
	2010-12-19 13:42 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::YTimer;
*/


#include "ytimer.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Timers)

bool YTimer::NotInitialized(true);
vu32 YTimer::SystemTick(0);
YTimer::TMRs YTimer::Timers;

YTimer::YTimer(TimeSpan i, bool a)
	: YCountableObject(),
	nInterval(i), nBase(0)
{
	InitializeSystemTimer();
	if(a)
		Activate(*this);
}

void
YTimer::SetInterval(TimeSpan i)
{
	nInterval = i;
	if(!nInterval)
		Deactivate(*this);
}

void
YTimer::InitializeSystemTimer()
{
	if(NotInitialized)
		ResetRTC();
}

void
YTimer::ResetSystemTimer()
{
	NotInitialized = true;
	InitializeSystemTimer();
}

bool
YTimer::RefreshRaw()
{
	if(SystemTick < nBase + nInterval)
		return false;
	nBase = SystemTick - (SystemTick - nBase) % nInterval;
	return true;
}

bool
YTimer::Refresh()
{
	Synchronize();
	return RefreshRaw();
}

bool
YTimer::RefreshAll()
{
	bool t(false);
	Synchronize();
	for(TMRs::iterator i(Timers.begin()); i != Timers.end(); ++i)
		if(i->second)
			t |= i->second->RefreshRaw();
	return t;
}

void
YTimer::ResetAll()
{
	for(TMRs::iterator i(Timers.begin()); i != Timers.end(); ++i)
		if(i->second)
			i->second->Reset();
}

void
YTimer::ResetYTimer()
{
	for(TMRs::iterator i(Timers.begin()); i != Timers.end(); ++i)
	{
		YTimer* const p(i->second);
		if(p)
		{
			p->SetInterval(0);
			p->Reset();
		}
	}
	Timers.clear();
	ResetSystemTimer();
}

void
Activate(YTimer& t)
{
	if(t.nInterval != 0)
	{
		YTimer::Timers[t.GetObjectID()] = &t;
		t.Synchronize();
		t.nBase = YTimer::SystemTick;
	}
}

void
Deactivate(YTimer& t)
{
	YTimer::Timers[t.GetObjectID()] = NULL;
}

YSL_END_NAMESPACE(Timers)

YSL_END

