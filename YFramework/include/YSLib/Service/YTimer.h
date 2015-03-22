/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YTimer.h
\ingroup Service
\brief 计时器服务。
\version r1123
\author FrankHB <frankhb1989@gmail.com>
\since build 572
\par 创建时间:
	2010-06-05 10:28:58 +0800
\par 修改时间:
	2015-03-21 14:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YTimer
*/


#ifndef YSL_INC_Service_YTimer_h_
#define YSL_INC_Service_YTimer_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YClock

namespace YSLib
{

namespace Timers
{

/*!
\brief 取公共历元。
\since build 547
*/
template<class _tClock = HighResolutionClock>
inline typename _tClock::time_point
FetchEpoch()
{
	static auto start_time(_tClock::now());

	return start_time;
}

/*!
\brief 取自公共历元以来经历的时间。
\since build 547
*/
template<class _tClock = HighResolutionClock>
inline typename _tClock::duration
FetchElapsed()
{
	return _tClock::now() - Timers::FetchEpoch<_tClock>();
}


/*!
\brief 阻塞延时。
\since build 584
*/
YF_API void
Delay(TimeSpan);


/*!
\brief 计时器。
\since build 243
*/
class YF_API Timer
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
	\since build 572
	*/
	explicit
	Timer(Duration = {}, bool = true) ynothrow;
	//! \since build 555
	DefDeCopyCtor(Timer)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 416
	*/
	virtual ~Timer();

	//! \since build 555
	DefDeCopyAssignment(Timer)

	DefGetter(const ynothrow, TimePoint, BaseTick, nBase)

	/*!
	\brief 激活：设置时间基点为当前时间。
	\since build 555
	*/
	friend PDefH(void, Activate, Timer& tmr) ynothrow
		ImplExpr(tmr.nBase = HighResolutionClock::now())

	/*!
	\brief 延时。
	\note 非阻塞，立即返回。
	\since build 572
	*/
	PDefH(void, Delay, Duration d)
		ImplExpr(nBase += d)

	/*!
	\brief 刷新：对非零时间间隔判断有效性并都更新时间基点。
	\pre <tt>Interval != Duration::zero()</tt> 。
	\return 是否有效：当前时刻已达到基点后的时间间隔。
	*/
	bool
	Refresh();

	/*!
	\brief 刷新：对非零时间间隔判断有效性并都更新时间基点。
	\pre <tt>Interval != Duration::zero()</tt> 。
	\return 当前时刻达到基点前的时间间隔。
	\since build 482
	*/
	Duration
	RefreshDelta();

	/*!
	\brief 刷新：对非零时间间隔判断有效性并都更新时间基点。
	\pre <tt>Interval != Duration::zero()</tt> 。
	\return 当前时刻达到基点前的时间间隔被最小时间间隔除的余数。
	\since build 482
	*/
	Duration
	RefreshRemainder();
};

/*!
\brief 检查超时：当前时刻到达计时器的时间基点后时间间隔指定的预定时刻。
\relates Timer
\since build 457
*/
inline PDefH(bool, CheckTimeout, Timer& tmr) ynothrow
	ImplRet(tmr.Interval == Duration::zero() || tmr.Refresh())

/*!
\brief 测试是否未超时。
\relates Timer
\since build 416
*/
inline PDefH(bool, Test, const Timer& tmr) ynothrow
	ImplRet(HighResolutionClock::now() < tmr.GetBaseTick() + tmr.Interval)

} // namespace Timers;

} // namespace YSLib;

#endif

