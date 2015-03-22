/*
	© 2011-2013, 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file UIContainerEx.h
\ingroup UI
\brief 样式无关的 GUI 附加容器。
\version r355
\author FrankHB <frankhb1989@gmail.com>
\since build 586
\par 创建时间:
	2011-02-21 08:59:34 +0800
\par 修改时间:
	2015-03-21 15:34 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::UIContainerEx
*/


#ifndef YSL_INC_UI_UIContainerEx_h_
#define YSL_INC_UI_UIContainerEx_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YPanel
#include YFM_YSLib_UI_Button
#include YFM_YSLib_UI_WidgetIteration

namespace YSLib
{

namespace UI
{

/*!
\brief 对话框。
\since build 276
*/
class YF_API DialogBox : public Control
{
public:
	//! \since build 460
	using iterator = ystdex::subscriptive_iterator<DialogBox, IWidget>;

protected:
	/*!
	\brief 关闭按钮。
	\since build 302
	*/
	Thumb btnClose;

public:
	//! \since build 337
	explicit
	DialogBox(const Rect& = {});
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~DialogBox() override;

	//! \since build 357
	DefWidgetMemberIteration(btnClose)
};


/*!
\brief 对话面板。
\since build 276
*/
class YF_API DialogPanel : public Panel
{
protected:
	/*!
	\brief 关闭按钮。
	\since build 394
	*/
	Thumb btnClose;
	/*!
	\brief 确定按钮。
	\since build 394
	*/
	Thumb btnOK;

public:
	//! \since build 337
	explicit
	DialogPanel(const Rect& = {});
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~DialogPanel() override;
};

} // namespace UI;

} // namespace YSLib;

#endif

