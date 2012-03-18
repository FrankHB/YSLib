/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file uicontx.h
\ingroup UI
\brief 样式无关的图形用户界面附加容器。
\version r1256;
\author FrankHB<frankhb1989@gmail.com>
\since build 192 。
\par 创建时间:
	2011-02-21 08:59:34 +0800;
\par 修改时间:
	2012-03-18 13:11 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::UIContainerEx;
*/


#ifndef YSL_INC_UI_UICONTX_H_
#define YSL_INC_UI_UICONTX_H_

#include "ypanel.h"
#include "YBrush.h"
#include "button.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 对话框。
\since build 276 。
*/
class DialogBox : public Control
{
public:
	/*!
	\brief 边框样式指针。
	\since build 284 。
	*/
	shared_ptr<BorderStyle> BorderPtr;

protected:
	CloseButton btnClose;

public:
	explicit
	DialogBox(const Rect& = Rect::Empty);

	virtual IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294 。
	*/
	virtual void
	Refresh(PaintEventArgs&&);
};


/*!
\brief 对话面板。
\since build 276 。
*/
class DialogPanel : public Panel
{
public:
	/*!
	\brief 边框样式指针。
	\since build 284 。
	*/
	shared_ptr<BorderStyle> BorderPtr;

	CloseButton btnClose;
	Button btnOK;

	explicit
	DialogPanel(const Rect& = Rect::Empty);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

