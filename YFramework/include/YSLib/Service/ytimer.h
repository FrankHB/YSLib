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
\version r1834;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-06-05 10:28:58 +0800;
\par 修改时间:
	2012-04-13 19:25 +0800;
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
#include <chrono>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Timers)

/*!
\brief 高精度时钟。
\since build 291 。
*/
class HighResolutionClock
{
public:
	typedef std::chrono::nanoseconds duration;
	typedef duration::rep rep;
	typedef duration::period period;
	typedef std::chrono::time_point<HighResolutionClock, duration> time_point;

	static yconstexpr bool is_steady = false;

	static time_point
	now() ynothrow;
};

inline HighResolutionClock::time_point
HighResolutionClock::now() ynothrow
{
	return time_point(std::chrono::nanoseconds(GetHighResolutionTicks()));
}


/*!
\brief 高精度时间间隔。
\note 单位为纳秒。
\since build 291 。
*/
typedef HighResolutionClock::duration Duration;

/*!
\brief 时刻。
\since build 291 。
*/
typedef HighResolutionClock::time_point TimePoint;

/*!
\brief 低精度时间间隔。
\note 单位为毫秒。
\since build 291 。
*/
typedef std::chrono::milliseconds TimeSpan;


/*!
\brief 延时。
\since build 291 。
*/
void
Delay(const TimeSpan&);


/*!
\brief 计时器。
\warning 非虚析构。
\since build 243 。
*/
class Timer : private noncopyable, protected GMCounter<Timer>
{
public:
	typedef map<u32, Timer*> TimerMap; //!< 计时器组。

protected:
	static TimerMap mTimers;

	TimePoint nBase;
	Duration nInterval;

public:
	/*!
	\brief 构造：使用时间间隔和激活状态。
	\since build 293 。
	*/
	explicit
	Timer(const Duration& = Duration(), bool = false);
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

	DefGetter(const ynothrow, TimePoint, BaseTick, nBase)
	DefGetter(const ynothrow, Duration, Interval, nInterval)

	/*!
	\brief 设置时间间隔。
	\since build 300 。
	*/
	void
	SetInterval(const TimeSpan& i)
	{
		SetInterval(static_cast<const Duration&>(i));
	}
	/*!
	\brief 设置时间间隔。
	\since build 300 。
	*/
	void
	SetInterval(const Duration&);

	/*!
	\brief 刷新。
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
	Reset()
	{
		nBase = TimePoint();
	}

	/*!
	\brief 复位计时器组中的所有计时器。
	*/
	static void
	ResetAll();

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

YSL_END_NAMESPACE(Timers)

YSL_END

#endif

