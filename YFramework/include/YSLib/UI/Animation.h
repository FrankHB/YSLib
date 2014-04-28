/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Animation.h
\ingroup UI
\brief 样式无关的动画实现。
\version r386
\author FrankHB <frankhb1989@gmail.com>
\since build 448
\par 创建时间:
	2013-10-06 22:11:33 +0800
\par 修改时间:
	2014-04-25 09:15 +0800
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

namespace YSLib
{

namespace UI
{

/*!
\brief 默认背景动画任务消息优先级。
\since build 450
*/
yconstexpr Messaging::Priority AnimationPriority(0x20);


//! \since build 455
//@{
//! \brief 按更新条件和优先级通过消息队列部署动画任务。
template<typename _fCallable>
void
AnimateTask(_fCallable update,
	Messaging::Priority prior = UI::AnimationPriority)
{
	Messaging::Renew(update, prior);
}

//! \brief 按指定的连接对象更新动画任务。
template<typename _fCallable>
void
AnimateConnection(const shared_ptr<_fCallable>& conn,
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
template<typename _fCallable>
void
Animate(_fCallable f, Messaging::Priority prior = UI::AnimationPriority)
{
	AnimateConnection(ystdex::make_shared<_fCallable>(f), prior);
}


//! \brief 动画会话。
template<typename _tCallable = std::function<bool()>>
class GAnimationSession final
{
public:
	using Connection = ystdex::decay_t<_tCallable>;
	using ConnectionPtr = shared_ptr<Connection>;

private:
	ConnectionPtr conn;

public:
	DefDeCtor(GAnimationSession)
	//! \brief 使用 ADL 调用 ResetState 函数复位连接对象状态以确保安全。
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

	DefGetter(const ynothrow, Connection&, ConnectionRef,
		(YAssertNonnull(conn), *conn))
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
	IWidget* WidgetPtr;
	//! \brief 准备和最后持续状态：更新函数的最后结果。
	mutable bool Ready;

	/*!
	\build 指示对于 WidgetPtr 用于判断是否继续动作的检查和无效化动作。
	\note 当值为空时表示不执行动作。
	\since build 457
	*/
	Invalidator Invalidate{DefaultInvalidate};

public:
	InvalidationUpdater(IWidget* p_wgt = {}, bool ready = {})
		: WidgetPtr(p_wgt), Ready(ready)
	{}

	//! \brief 更新函数：无效化后根据成员指定是否需要发送 SM_Task 消息。
	bool
	operator()() const;

	/*!
	\build 默认无效化：对部件调用 InvalidateVisible 。
	\return <tt>true</tt> 。
	\since build 462
	*/
	static bool
	DefaultInvalidate(IWidget&);

	/*!
	\build 默认无效化：对调用 IsEnabled 返回 true 的部件调用 InvalidateVisible 。
	\return <tt>true</tt> 。
	\since build 482
	*/
	static bool
	DefaultInvalidateControl(IWidget&);
};

//! \relates InvalidationUpdater
inline PDefH(void, ResetState, InvalidationUpdater& updater)
	ImplExpr(updater.WidgetPtr = {})
//@}


//! \since build 463
//@{
//! \brief 重新开始动画。
template<class _tAnimation, typename _fCallable>
inline void
Restart(_tAnimation& ani, IWidget& wgt, _fCallable f)
{
	ani.Reset(&wgt, true);
	ani.GetConnectionRef().Invalidate = f;
	ani.Start();
}

//! \brief 安装动画效果。
//@{
template<class _tAnimation, typename _fCallable>
void
Setup(_tAnimation& ani, IWidget& wgt, _fCallable f)
{
	UI::Restart(ani, wgt, [f](IWidget&){
		f();
		return true;
	});
}
template<class _tAnimation, typename _fCallable, typename _fCond>
void
Setup(_tAnimation& ani, IWidget& wgt, _fCond cond, _fCallable f)
{
	UI::Restart(ani, wgt, [cond, f](IWidget&){
		if(cond())
			f();
		return true;
	});
}
//@}

//! \brief 安装以计时器控制的动画效果。
template<class _tAnimation, typename _fCallable, typename _tTimer>
void
SetupByTimer(_tAnimation& ani, IWidget& wgt, _tTimer&& timer,
	_fCallable f)
{
	UI::Setup(ani, wgt, [&]{
		return timer.Refresh();
	}, f);
}
//@}

} // namespace UI;

} // namespace YSLib;

#endif

