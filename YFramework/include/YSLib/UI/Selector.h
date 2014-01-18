/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Selector.h
\ingroup UI
\brief 样式相关的图形用户界面选择控件。
\version r332
\author FrankHB <frankhb1989@gmail.com>
\since build 282
\par 创建时间:
	2011-03-22 07:17:17 +0800
\par 修改时间:
	2014-01-11 11:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Selector
*/


#ifndef YSL_INC_UI_Selector_h_
#define YSL_INC_UI_Selector_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_Button
#include YFM_YSLib_UI_YStyle

namespace YSLib
{

namespace UI
{

/*!
\brief 复选框。
\since build 205
*/
class YF_API CheckBox : public Thumb
{
public:
	/*!
	\brief 复选框选中状态参数类型。
	\note bool 参数表示选中后的状态。
	\since build 292
	*/
	using TickedArgs = GValueEventArgs<bool>;
	/*!
	\brief 复选框选中事件委托类型。
	\since build 292
	*/
	DeclDelegate(HTickedEvent, TickedArgs)

protected:
	bool bTicked; //选中状态。

private:
	/*!
	\brief 复选框选中事件。
	\since build 292
	*/
	DeclEvent(HTickedEvent, Ticked)

public:
	/*!
	\brief 构造：使用指定边界。
	\since build 337
	*/
	explicit
	CheckBox(const Rect& = {});
	DefDeMoveCtor(CheckBox)

	DefPred(const ynothrow, Ticked, bTicked)

	/*!
	\brief 复选框选中事件。
	\since build 292
	*/
	DefEventGetter(ynothrow, HTickedEvent, Ticked, Ticked)

	/*!
	\brief 设置选中状态并检查复选框选中事件。
	\note 若选中状态发生改变则引起复选框选中事件。
	\since build 292
	*/
	void
	SetTicked(bool);

	/*!
	\brief 设置选中状态并触发复选框选中事件。
	\note 不检查状态改变。
	\since build 307
	*/
	void
	Tick(bool);

protected:
	/*!
	\brief 绘制复选框。
	\since build 453
	*/
	void
	PaintBox(const PaintContext&, const Size&);

public:
	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 311
	*/
	void
	Refresh(PaintEventArgs&&) override;
};


/*!
\brief 复选按钮。
\since build 311
*/
class YF_API CheckButton : public CheckBox, protected MLabel
{
public:
	using MLabel::Font;
	using MLabel::Text;

	/*!
	\brief 构造：使用指定边界。
	\since build 337
	*/
	explicit
	CheckButton(const Rect& = {});

	void
	Refresh(PaintEventArgs&&) override;
};

} // namespace UI;

} // namespace YSLib;

#endif

