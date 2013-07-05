/*
	Copyright by FrankHB 2010 - 2013.

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
\version r963
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-06-05 10:28:58 +0800
\par 修改时间:
	2013-07-04 07:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YTimer
*/


#ifndef YSL_INC_Service_ytimer_h_
#define YSL_INC_Service_ytimer_h_ 1

#include "../Core/ysdef.h"
#include <chrono>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Timers)

/*!
\brief 高精度时钟。
\since build 291
*/
class YF_API HighResolutionClock
{
public:
	typedef std::chrono::nanoseconds duration;
	typedef duration::rep rep;
	typedef duration::period period;
	typedef std::chrono::time_point<HighResolutionClock, duration> time_point;

	static yconstexpr bool is_steady = {};

	//! \warning 首次调用前非线程安全。
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
\since build 291
*/
typedef HighResolutionClock::duration Duration;

/*!
\brief 时刻。
\since build 291
*/
typedef HighResolutionClock::time_point TimePoint;

/*!
\brief 低精度时间间隔。
\note 单位为毫秒。
\since build 291
*/
typedef std::chrono::milliseconds TimeSpan;


/*!
\brief 阻塞延时。
\since build 291
*/
YF_API void
Delay(const TimeSpan&);


/*!
\brief 计时器。
\since build 243
*/
class YF_API Timer : private noncopyable
{
protected:
	//! \brief 时间基点：计时的起点。
	TimePoint nBase;

public:
	/*!
	\brief 重复刷新有效的最小时间间隔。
	\since build 405
	*/
	Duration Interval;

	/*!
	\brief 构造：使用时间间隔和激活状态。
	\since build 405
	*/
	explicit
	Timer(const Duration& = {}, bool = true);
	//! \since build 416
	virtual DefDeDtor(Timer)

	DefGetter(const ynothrow, TimePoint, BaseTick, nBase)

	/*!
	\brief 延时。
	\note 非阻塞，立即返回。
	\since build 303
	*/
	PDefH(void, Delay, const Duration& d)
		ImplExpr(nBase += d)

	/*!
	\brief 刷新。
	\return 是否有效。
	*/
	bool
	Refresh();

	/*!
	\brief 激活：当时间间隔非零时同步时间基点。
	*/
	YF_API friend void
	Activate(Timer&);
};

/*!
\brief 测试是否未超时。
\since 416
*/
inline PDefH(bool, Test, const Timer& tmr) ynothrow
	ImplRet(HighResolutionClock::now() < tmr.GetBaseTick() + tmr.Interval)

YSL_END_NAMESPACE(Timers)

YSL_END

#endif

