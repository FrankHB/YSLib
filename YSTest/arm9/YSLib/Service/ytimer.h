// YSLib::Service::YTimer by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-06-05 10:28:58 + 08:00;
// UTime = 2010-10-24 19:55 + 08:00;
// Version = 0.1604;


#ifndef INCLUDED_YTIMER_H_
#define INCLUDED_YTIMER_H_

// YTimer ：平台无关的计时器服务。

#include "../Core/yobject.h"
#include "../Adaptor/cont.h"
//#include <map>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Timers)

typedef u32 TimeSpan;

//计时器。
class YTimer : public YCountableObject
{
private:
	typedef map<u32, YTimer*> TMRs; //计时器组。

	static bool NotInitialized;
	static vu32 SystemTick;
	static TMRs Timers;

	TimeSpan nInterval;
	TimeSpan nBase;

public:
	//********************************
	//名称:		YTimer
	//全名:		YSLib::Timers::YTimer::YTimer
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	TimeSpan
	//形式参数:	bool
	//功能概要:	构造：使用时间间隔和有效性。
	//备注:		
	//********************************
	explicit
	YTimer(TimeSpan = 1000, bool = true);

	DefStaticGetter(TimeSpan, SystemTick, SystemTick)
	DefGetter(TimeSpan, Interval, nInterval)
	DefGetter(TimeSpan, BaseTick, nBase)

	//********************************
	//名称:		SetInterval
	//全名:		YSLib::Timers::YTimer::SetInterval
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	TimeSpan
	//功能概要:	设置时间间隔。
	//备注:		
	//********************************
	void
	SetInterval(TimeSpan);

private:
	//********************************
	//名称:		InitializeSystemTimer
	//全名:		YSLib::Timers::YTimer::InitializeSystemTimer
	//可访问性:	private static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	初始化系统计时器。
	//备注:		
	//********************************
	static void
	InitializeSystemTimer();

	//********************************
	//名称:		ResetSystemTimer
	//全名:		YSLib::Timers::YTimer::ResetSystemTimer
	//可访问性:	private static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	复位系统计时器。
	//备注:		
	//********************************
	static void
	ResetSystemTimer();

	//********************************
	//名称:		Synchronize
	//全名:		YSLib::Timers::YTimer::Synchronize
	//可访问性:	private static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	与系统计时器同步。
	//备注:		
	//********************************
	static void
	Synchronize();

	//********************************
	//名称:		RefreshRaw
	//全名:		YSLib::Timers::YTimer::RefreshRaw
	//可访问性:	private 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	直接刷新。
	//备注:		不经过同步。
	//********************************
	bool
	RefreshRaw();

public:
	//********************************
	//名称:		Refresh
	//全名:		YSLib::Timers::YTimer::Refresh
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	刷新。
	//备注:		刷新前同步。
	//********************************
	bool
	Refresh();

	//********************************
	//名称:		RefreshAll
	//全名:		YSLib::Timers::YTimer::RefreshAll
	//可访问性:	public static 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	刷新计时器组中的所有计时器。
	//备注:		
	//********************************
	static bool
	RefreshAll();

	//********************************
	//名称:		Activate
	//全名:		YSLib::Timers::YTimer::Activate
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	激活。
	//备注:		
	//********************************
	void
	Activate();

	//********************************
	//名称:		Deactivate
	//全名:		YSLib::Timers::YTimer::Deactivate
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	停用。
	//备注:		
	//********************************
	void
	Deactivate();

	//********************************
	//名称:		Reset
	//全名:		YSLib::Timers::YTimer::Reset
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	复位。
	//备注:		
	//********************************
	void
	Reset();

	//********************************
	//名称:		ResetAll
	//全名:		YSLib::Timers::YTimer::ResetAll
	//可访问性:	public static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	复位计时器组中的所有计时器。
	//备注:		
	//********************************
	static void
	ResetAll();

	//********************************
	//名称:		ResetYTimer
	//全名:		YSLib::Timers::YTimer::ResetYTimer
	//可访问性:	public static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	复位计时器类所有状态。
	//备注:		
	//********************************
	static void
	ResetYTimer();
};

inline void
YTimer::SetInterval(TimeSpan i)
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
	nBase = 0;
}

YSL_END_NAMESPACE(Timers)

YSL_END

#endif

