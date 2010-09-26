// YSLib::Service::YTimer by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-06-05 10:28:58 + 08:00;
// UTime = 2010-09-26 15:27 + 08:00;
// Version = 0.1468;


#include "ytimer.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Timers)

bool YTimer::NotInitialized(true);
vu32 YTimer::SystemTick(0);
YTimer::TMRs YTimer::Timers;

YTimer::YTimer(u32 i, bool a)
: nInterval(i), nBase(0)
{
	InitializeSystemTimer();
	if(a)
		Activate();
}

void
YTimer::InitializeSystemTimer()
{
	if(NotInitialized)
		platform::ResetRTC();
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
		if(i->second != NULL)
			t |= i->second->RefreshRaw();
	return t;
}

void
YTimer::Activate()
{
	if(nInterval)
	{
		Timers[GetID()] = this;
		Synchronize();
		nBase = SystemTick;
	}
}
void
YTimer::Deactivate()
{
	Timers[GetID()] = NULL;
}
void
YTimer::ResetAll()
{
	for(TMRs::iterator i(Timers.begin()); i != Timers.end(); ++i)
		if(i->second != NULL)
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

YSL_END_NAMESPACE(Timers)

YSL_END

