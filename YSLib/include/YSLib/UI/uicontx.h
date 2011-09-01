/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file uicontx.h
\ingroup UI
\brief 样式无关的图形用户界面附加容器。
\version r1127;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-02-21 08:59:34 +0800;
\par 修改时间:
	2011-09-01 21:04 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::UIContainerEx;
*/


#ifndef YSL_INC_UI_UICONTX_H_
#define YSL_INC_UI_UICONTX_H_

#include "ycontrol.h"
#include "yfocus.h"
#include "yuicont.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 固定容器抽象实现类。
class AUIBoxControl : public Control, protected MSimpleFocusResponser
{
public:
	AUIBoxControl(const Rect& = Rect::Empty);

	/*!
	\brief 取焦点指针。
	*/
	virtual DefMutableGetterBase(IControl*, FocusingPtr,
		MSimpleFocusResponser)
	/*!
	\brief 取顶端部件指针。
	\note 由顶端控件指针转换。
	*/
	virtual PDefH1(IWidget*, GetTopWidgetPtr, const Point& p)
		ImplRet(GetTopControlPtr(p))
	/*!
	\brief 取顶端控件指针。
	*/
	ImplA1(IWidget)
	DeclIEntry(IControl* GetTopControlPtr(const Point&));

	/*!
	\brief 清除焦点指针。
	*/
	virtual void
	ClearFocusingPtr();

	/*!
	\brief 响应焦点请求。
	*/
	virtual PDefH1(bool, ResponseFocusRequest, IControl& ctl)
		ImplBodyBase1(MSimpleFocusResponser, ResponseFocusRequest, ctl)

	/*!
	\brief 响应焦点释放。
	*/
	virtual PDefH1(bool, ResponseFocusRelease, IControl& ctl)
		ImplBodyBase1(MSimpleFocusResponser, ResponseFocusRelease, ctl)
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

