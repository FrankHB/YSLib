/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Animation.h
\ingroup UI
\brief 样式无关的动画实现。
\version r136
\author FrankHB <frankhb1989@gmail.com>
\since build 448
\par 创建时间:
	2013-10-06 22:11:33 +0800
\par 修改时间:
	2013-10-15 19:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Animation
*/


#ifndef YSL_INC_UI_Animation_h_
#define YSL_INC_UI_Animation_h_ 1

#include "ywidget.h"
#include "../Core/Task.h"

namespace YSLib
{

namespace UI
{

/*!
\brief 默认背景动画任务消息优先级。
\since build 450
*/
yconstexpr Messaging::Priority AnimationPriority(0x20);


/*!
\brief 动画自动更新任务。
\since build 449

通过根据更新操作发送指定优先级的 SM_TASK 消息无效化部件区域实现动画。
*/
class YF_API AnimationTask : public
	Messaging::GAutoTask<std::function<bool(UI::IWidget&, const Rect&)>>
{
public:
	using BaseType
		= Messaging::GAutoTask<std::function<bool(UI::IWidget&, const Rect&)>>;
	/*!
	\brief 状态更新器。
	\since build 451
	*/
	struct StateUpdater
	{
		bool Ready = {};

		//! \brief 更新函数：无效化后根据成员指定是否需要发送 SM_TASK 消息。
		bool
		operator()(UI::IWidget&, const Rect&);
	};

	//! \since build 451
	//@{
	AnimationTask(Messaging::Priority prior = AnimationPriority)
		: BaseType(AlwaysUpdate, prior)
	{}
	template<typename _fUpdater>
	AnimationTask(_fUpdater f, Messaging::Priority prior = AnimationPriority)
		: BaseType(f, prior)
	{}
	DefDeCopyCtor(AnimationTask)
	DefDeMoveCtor(AnimationTask)
	//@}

	/*!
	\brief 更新操作：总是要求更新后发送 SM_TASK 消息。
	\note 应确保无效化部件不会引起调用 Renew 或其它引起发送消息的操作。
	\since build 451
	*/
	static bool
	AlwaysUpdate(IWidget&, const Rect&);

	/*!
	\brief 更新操作：不在更新后发送 SM_TASK 消息。
	\note 当无效化部件引起调用 Renew 或其它引起发送消息的操作时能持续发送消息。
	\since build 451
	*/
	static bool
	UpdateOnce(IWidget&, const Rect&);

	/*!
	\warning 发送的 SM_TASK 消息依赖参数指定部件的生存期。
	\since build 450

	调用更新操作，当结果为 true 时发送基类成员指定优先级的 SM_TASK 消息。
	*/
	//@{
	PDefH(void, Renew, IWidget& wgt)
		ImplExpr(Renew(wgt, GetSizeOf(wgt)))
	void
	Renew(IWidget&, const Rect&);
	//@}
};

} // namespace UI;

} // namespace YSLib;

#endif

