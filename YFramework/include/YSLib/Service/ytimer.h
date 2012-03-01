/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Service Service
\ingroup YSLib
\brief YSLib 服务模块。
*/

/*!	\file ytimer.h
\ingroup Service
\brief 计时器服务。
\version r1712;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-06-05 10:28:58 +0800;
\par 修改时间:
	2012-02-29 14:35 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Service::YTimer;
*/


#ifndef YSL_INC_SERVICE_YTIMER_H_
#define YSL_INC_SERVICE_YTIMER_H_

#include "../Core/yobject.h"
#include "../Adaptor/ycont.h"
#include "../Core/ycounter.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Timers)

typedef u32 TimeSpan;

/*!
\brief 计时器。
\warning 非虚析构。
\since build 243 。
*/
class Timer : public noncopyable, public GMCounter<Timer>
{
public:
	typedef map<u32, Timer*> TimerMap; //!< 计时器组。

protected:
	static bool NotInitialized;
	static vu32 SystemTick;
	static TimerMap mTimers;

	TimeSpan nInterval;
	TimeSpan nBase;

public:
	/*!
	\brief 构造：使用时间间隔和激活状态。
	*/
	explicit
	Timer(TimeSpan = 1000, bool = true);
	/*!
	\brief 析构：自动停用。
	\since build 289 。
	*/
	~Timer();

	/*!
	\brief 判断 Timer 是否处于激活状态。
	\since build 289 。
	*/
	bool
	IsActive() const;

	static DefGetter(ynothrow, TimeSpan, SystemTick, SystemTick)
	DefGetter(const ynothrow, TimeSpan, Interval, nInterval)
	DefGetter(const ynothrow, TimeSpan, BaseTick, nBase)

	/*!
	\brief 设置时间间隔。
	*/
	void
	SetInterval(TimeSpan);

private:
	/*!
	\brief 初始化系统计时器。
	*/
	static void
	InitializeSystemTimer();

	/*!
	\brief 复位系统计时器。
	*/
	static void
	ResetSystemTimer();

	/*!
	\brief 与系统计时器同步。
	*/
	static void
	Synchronize();

	/*!
	\brief 直接刷新。
	\note 不经过同步。
	*/
	bool
	RefreshRaw();

public:
	/*!
	\brief 刷新。
	\note 刷新前同步。
	*/
	bool
	Refresh();

	/*!
	\brief 刷新计时器组中的所有计时器。
	*/
	static bool
	RefreshAll();

	/*!
	\brief 复位。
	*/
	void
	Reset();

	/*!
	\brief 复位计时器组中的所有计时器。
	*/
	static void
	ResetAll();

	/*!
	\brief 复位计时器类所有状态。
	*/
	static void
	ResetYTimer();

	/*!
	\brief 激活。
	*/
	friend void
	Activate(Timer&);

	/*!
	\brief 停用。
	*/
	friend void
	Deactivate(Timer&);
};

inline
Timer::~Timer()
{
	Deactivate(*this);
}

inline void
Timer::Synchronize()
{
	SystemTick = GetRTC();
}

inline void
Timer::Reset()
{
	nBase = 0;
}

YSL_END_NAMESPACE(Timers)

YSL_END

#endif

