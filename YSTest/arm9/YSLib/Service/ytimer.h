// YSLib::Service::YTimer by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-5 10:28:58;
// UTime = 2010-7-9 9:18;
// Version = 0.1444;


#ifndef INCLUDED_YTIMER_H_
#define INCLUDED_YTIMER_H_

// YTimer ：平台无关的计时器服务。

#include "../Core/yobject.h"
#include <map>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Timers)

//计时器。
class YTimer : public YCountableObject
{
private:
	typedef std::map<u32, YTimer*> TMRs; //计时器组。

	static bool NotInitialized;
	static vu32 SystemTick;
	static TMRs Timers;

	u32 nInterval;
	u32 nStart;

public:
	explicit
	YTimer(u32 = 1000, bool = true);

	DefStaticGetter(u32, SystemTick, SystemTick)
	DefGetter(u32, Interval, nInterval)
	DefGetter(u32, StartTick, nStart)

	void
	SetInterval(u32);

private:
	static void
	InitializeSystemTimer();
	static void
	ResetSystemTimer();
	static void
	Synchronize();

	bool
	RefreshRaw();

public:
	bool
	Refresh();
	static bool
	RefreshAll();

	void
	Activate();
	void
	Deactivate();
	void
	Reset();
	static void
	ResetAll();
	static void
	ResetYTimer();
};

inline void
YTimer::SetInterval(u32 i)
{
	nInterval = i;
	if(!nInterval)
		Deactivate();
}

inline void
YTimer::Synchronize()
{
	SystemTick = platform::GetRTC();
}

inline void
YTimer::Reset()
{
	nStart = 0;
}

YSL_END_NAMESPACE(Timers)

YSL_END

#endif

