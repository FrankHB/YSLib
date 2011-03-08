/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ytimer.h
\ingroup Service
\brief 计时器服务。
\version 0.1657;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-06-05 10:28:58 +0800;
\par 修改时间:
	2011-03-05 17:05 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::YTimer;
*/


#ifndef INCLUDED_YTIMER_H_
#define INCLUDED_YTIMER_H_

#include "../Core/ysdef.h"
#include "../Core/yobject.h"
#include "../Adaptor/cont.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Timers)

typedef u32 TimeSpan;

//! \brief 计时器。
class YTimer : public YCountableObject
{
private:
	typedef map<u32, YTimer*> TMRs; //!< 计时器组。

	static bool NotInitialized;
	static vu32 SystemTick;
	static TMRs Timers;

	TimeSpan nInterval;
	TimeSpan nBase;

public:
	/*!
	\brief 构造：使用时间间隔和有效性。
	*/
	explicit
	YTimer(TimeSpan = 1000, bool = true);

	static DefMutableGetter(TimeSpan, SystemTick, SystemTick)
	DefGetter(TimeSpan, Interval, nInterval)
	DefGetter(TimeSpan, BaseTick, nBase)

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
	Activate(YTimer&);

	/*!
	\brief 停用。
	*/
	friend void
	Deactivate(YTimer&);
};

inline void
YTimer::Synchronize()
{
	SystemTick = GetRTC();
}

inline void
YTimer::Reset()
{
	nBase = 0;
}

YSL_END_NAMESPACE(Timers)

YSL_END

#endif

