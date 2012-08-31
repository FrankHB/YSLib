/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Selector.h
\ingroup UI
\brief 样式相关的图形用户界面选择控件。
\version r286;
\author FrankHB<frankhb1989@gmail.com>
\since build 282 。
\par 创建时间:
	2011-03-22 07:17:17 +0800;
\par 修改时间:
	2012-08-30 20:16 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Selector;
*/


#ifndef YSL_INC_UI_SELECTOR_H_
#define YSL_INC_UI_SELECTOR_H_ 1

#include "button.h"
#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 复选框。
\since build 205 。
*/
class CheckBox : public Thumb
{
public:
	/*!
	\brief 复选框选中状态参数类型。
	\note bool 参数表示选中后的状态。
	\since build 292 。
	*/
	typedef GValueEventArgs<bool> TickedArgs;
	/*!
	\brief 复选框选中事件委托类型。
	\since build 292 。
	*/
	DeclDelegate(HTickedEvent, TickedArgs)

protected:
	bool bTicked; //选中状态。

private:
	/*!
	\brief 复选框选中事件。
	\since build 292 。
	*/
	DeclEvent(HTickedEvent, Ticked)

public:
	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	CheckBox(const Rect& = Rect::Empty);
	inline DefDeMoveCtor(CheckBox)

	DefPred(const ynothrow, Ticked, bTicked)

	/*!
	\brief 复选框选中事件。
	\since build 292 。
	*/
	DefEventGetter(ynothrow, HTickedEvent, Ticked, Ticked)

	/*!
	\brief 设置选中状态并检查复选框选中事件。
	\note 若选中状态发生改变则引起复选框选中事件。
	\since build 292 。
	*/
	void
	SetTicked(bool);

	/*!
	\brief 设置选中状态并触发复选框选中事件。
	\note 不检查状态改变。
	\since build 307 。
	*/
	void
	Tick(bool);

protected:
	/*!
	\brief 绘制复选框。
	\since build 311 。
	*/
	void
	PaintBox(const Graphics&, const Rect&);

public:
	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 311 。
	*/
	void
	Refresh(PaintEventArgs&&) override;
};


/*!
\brief 复选按钮。
\since build 311 。
*/
class CheckButton : public CheckBox, protected MLabel
{
public:
	using MLabel::Font;
	using MLabel::Text;

	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	CheckButton(const Rect& = Rect::Empty);

	void
	Refresh(PaintEventArgs&&) override;
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

