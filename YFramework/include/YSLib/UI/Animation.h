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
\version r76
\author FrankHB <frankhb1989@gmail.com>
\since build 448
\par 创建时间:
	2013-10-06 22:11:33 +0800
\par 修改时间:
	2013-10-06 22:27 +0800
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
\brief 动画自动更新任务。
\since build 449
*/
class YF_API AnimationTask : public
	Messaging::GAutoTask<std::function<bool(UI::IWidget&, const Rect&)>>
{
public:
	using BaseType
		= Messaging::GAutoTask<std::function<bool(UI::IWidget&, const Rect&)>>;

	AnimationTask()
		: BaseType(DefaultUpdate)
	{}
	template<typename _fUpdater>
	AnimationTask(_fUpdater&& f)
		: BaseType(yforward(f))
	{}

	PDefH(void, Start, IWidget& wgt)
		ImplExpr(Start(wgt, GetSizeOf(wgt)))
	void
	Start(IWidget&, const Rect&);

	static bool
	DefaultUpdate(IWidget& wgt, const Rect& r)
	{
		Invalidate(wgt, r);
		return false;
	}
};

} // namespace UI;

} // namespace YSLib;

#endif

