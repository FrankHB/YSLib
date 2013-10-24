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
\version r182
\author FrankHB <frankhb1989@gmail.com>
\since build 448
\par 创建时间:
	2013-10-06 22:11:33 +0800
\par 修改时间:
	2013-10-23 19:08 +0800
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

通过根据更新操作发送指定优先级的 SM_Task 消息无效化部件区域实现动画。
*/
class YF_API AnimationTask
	: public Messaging::GAutoTask<std::function<bool()>>
{
public:
	using BaseType = Messaging::GAutoTask<std::function<bool()>>;
	/*!
	\brief 状态更新器。
	\since build 454
	*/
	struct YF_API StateUpdater
	{
		//! \since build 454
		IWidget* WidgetPtr = {};
		bool Ready = {};

		//! \since build 454
		StateUpdater(IWidget* p_wgt = {}, bool ready = {})
			: WidgetPtr(p_wgt), Ready(ready)
		{}

		/*!
		\brief 更新函数：无效化后根据成员指定是否需要发送 SM_Task 消息。
		\since build 454
		*/
		bool
		operator()() const;
	};

	//! \since build 451
	//@{
	AnimationTask(Messaging::Priority prior = AnimationPriority)
		: BaseType(StateUpdater(), prior)
	{}
	//! \since build 454
	AnimationTask(IWidget& wgt, Messaging::Priority prior = AnimationPriority)
		: BaseType(StateUpdater(&wgt), prior)
	{}
	template<typename _fUpdater>
	AnimationTask(_fUpdater f, Messaging::Priority prior = AnimationPriority)
		: BaseType(f, prior)
	{}
	DefDeCopyCtor(AnimationTask)
	DefDeMoveCtor(AnimationTask)
	//@}

	/*!
	\warning 发送的 SM_Task 消息依赖参数指定部件的生存期。
	\since build 454

	调用更新操作，当结果为 true 时发送基类成员指定优先级的 SM_Task 消息。
	*/
	void
	Renew();
};

} // namespace UI;

} // namespace YSLib;

#endif

