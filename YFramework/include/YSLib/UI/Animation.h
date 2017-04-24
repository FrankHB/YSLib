/*
	© 2013-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Animation.h
\ingroup UI
\brief 样式无关的动画实现。
\version r449
\author FrankHB <frankhb1989@gmail.com>
\since build 448
\par 创建时间:
	2013-10-06 22:11:33 +0800
\par 修改时间:
	2017-04-24 22:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Animation
*/


#ifndef YSL_INC_UI_Animation_h_
#define YSL_INC_UI_Animation_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YWidget
#include YFM_YSLib_Core_Task
#include YFM_YSLib_Service_YTimer // for Timers::TimeSpan;

namespace YSLib
{

namespace UI
{

/*!
\brief 默认背景动画任务消息优先级。
\since build 450
*/
yconstexpr const Messaging::Priority AnimationPriority(0x20);


//! \since build 455
//@{
//! \brief 按更新条件和优先级通过消息队列部署动画任务。
template<typename _fUpdater>
void
AnimateTask(_fUpdater update, Messaging::Priority prior = UI::AnimationPriority)
{
	Messaging::Renew(update, prior);
}

//! \brief 按指定的连接对象更新动画任务。
template<typename _fUpdater>
void
AnimateConnection(const shared_ptr<_fUpdater>& conn,
	Messaging::Priority prior = UI::AnimationPriority)
{
	YAssertNonnull(conn);
	AnimateTask([=]{
		return (*conn)();
	}, prior);
}

/*!
\brief 按指定的可调用对象初始化为连接对象更新动画任务。
\since build 494
*/
template<typename _fUpdater>
void
Animate(_fUpdater f, Messaging::Priority prior = UI::AnimationPriority)
{
	AnimateConnection(share_copy(f), prior);
}


//! \brief 动画会话。
template<typename _fUpdater = std::function<bool()>>
class GAnimationSession final
{
public:
	using Connection = ystdex::decay_t<_fUpdater>;
	using ConnectionPtr = shared_ptr<Connection>;

private:
	ConnectionPtr conn;

public:
	DefDeCtor(GAnimationSession)
	/*!
	\brief 使用 ADL 调用 ResetState 函数复位连接对象状态以确保安全。
	\pre 调用的 ResetState 保证无异常抛出。
	*/
	~GAnimationSession()
	{
		if(conn)
			ResetState(*conn);
	}

	template<typename... _tParams>
	void
	Reset(_tParams&&... args)
	{
		conn.reset(new Connection(yforward(args)...));
	}

	//! \brief 使用 ADL 调用 AnimateConnection 开始动画。
	PDefH(void, Start, ) const
		ImplExpr(AnimateConnection(conn))

	DefGetter(const ynothrow, Connection&, ConnectionRef, Deref(conn))
	DefGetter(const ynothrow, const ConnectionPtr&, ConnectionPtr, conn)
};


/*!
\brief 无效状态更新器。
\warning 非虚析构。
*/
class YF_API InvalidationUpdater
{
public:
	//! \since build 462
	using Invalidator = std::function<bool(IWidget&)>;
	//! \since build 672
	observer_ptr<IWidget> WidgetPtr;
	//! \brief 准备和最后持续状态：更新函数的最后结果。
	mutable bool Ready;

	/*!
	\brief 指示对 WidgetPtr 用于判断是否继续动作的检查和无效化动作。
	\note 当值为空时表示不执行动作。
	\since build 457
	*/
	Invalidator Invalidate{DefaultInvalidate};

public:
	//! \since build 672
	InvalidationUpdater(observer_ptr<IWidget> p_wgt = {}, bool ready = {})
		: WidgetPtr(p_wgt), Ready(ready)
	{}

	//! \brief 更新函数：无效化后根据成员指定是否需要发送 SM_Task 消息。
	bool
	operator()() const;

	/*!
	\brief 默认无效化：对部件调用 InvalidateVisible 。
	\return <tt>true</tt> 。
	\since build 462
	*/
	static bool
	DefaultInvalidate(IWidget&);

	/*!
	\brief 默认无效化：对调用 IsEnabled 返回 true 的部件调用 InvalidateVisible 。
	\return <tt>true</tt> 。
	\since build 482
	*/
	static bool
	DefaultInvalidateControl(IWidget&);
};

/*!
\relates InvalidationUpdater
\since build 557
*/
inline PDefH(void, ResetState, InvalidationUpdater& updater) ynothrow
	ImplExpr(updater.WidgetPtr = {})
//@}


//! \since build 463
//@{
//! \brief 重新开始动画。
template<class _tAnimation, typename _fCallable>
inline void
Restart(_tAnimation& ani, IWidget& wgt, _fCallable f)
{
	ani.Reset(make_observer(&wgt), true);
	ani.GetConnectionRef().Invalidate = f;
	ani.Start();
}

//! \brief 安装动画效果。
//@{
template<class _tAnimation, typename _func>
void
Setup(_tAnimation& ani, IWidget& wgt, _func f)
{
	UI::Restart(ani, wgt, [f](IWidget&) ynoexcept_spec(f()){
		f();
		return true;
	});
}
template<class _tAnimation, typename _func, typename _fCond>
void
Setup(_tAnimation& ani, IWidget& wgt, _fCond cond, _func f)
{
	UI::Restart(ani, wgt,
		[cond, f](IWidget&) ynoexcept(noexcept(cond()) && noexcept(f())){
		if(cond())
			f();
		return true;
	});
}
//@}

//! \brief 安装以计时器控制的动画效果。
template<class _tAnimation, typename _func, typename _tTimer>
void
SetupByTimer(_tAnimation& ani, IWidget& wgt, _tTimer&& timer, _func f)
{
	UI::Setup(ani, wgt, [&]() ynoexcept_spec(timer.Refresh()){
		return timer.Refresh();
	}, f);
}
//@}


/*!
\brief 在一定时间后对部件进行操作。
\since build 584
*/
YF_API void
ActAfter(IWidget&, Timers::TimeSpan);

/*!
\brief 在一定时间内显示部件。
\since build 584
*/
inline PDefH(void, DisplayFor, IWidget& wgt, Timers::TimeSpan delay)
	ImplExpr(Show(wgt), ActAfter(wgt, delay))

} // namespace UI;

} // namespace YSLib;

#endif

