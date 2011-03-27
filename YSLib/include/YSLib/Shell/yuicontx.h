/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yuicontx.h
\ingroup Shell
\brief 平台无关的图形用户界面控件实现。
\version 0.1098;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-02-21 08:59:34 +0800;
\par 修改时间:
	2011-03-08 21:43 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YUIContainerEx;
*/


#ifndef INCLUDED_YUICONTX_H_
#define INCLUDED_YUICONTX_H_

#include "ycontrol.h"
#include "yfocus.h"
#include "yuicont.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

//! \brief 固定容器抽象实现类。
class AUIBoxControl : public Control, protected MSimpleFocusResponser,
	implements IUIBox
{
public:
	AUIBoxControl(const Rect& = Rect::Empty, IUIBox* = NULL);
	virtual DefEmptyDtor(AUIBoxControl)

	/*!
	\brief 取焦点指针。
	*/
	ImplI1(IUIBox) DefMutableGetterBase(IControl*, FocusingPtr,
		MSimpleFocusResponser)
	/*!
	\brief 取顶端部件指针。
	\note 由顶端控件指针转换。
	*/
	ImplI1(IUIBox) PDefH1(IWidget*, GetTopWidgetPtr, const Point& p)
		ImplRet(GetTopControlPtr(p))
	/*!
	\brief 取顶端控件指针。
	*/
	ImplA1(IUIBox)
	DeclIEntry(IControl* GetTopControlPtr(const Point&));

	/*!
	\brief 清除焦点指针。
	*/
	ImplI1(IUIBox) void
	ClearFocusingPtr();

	/*!
	\brief 响应焦点请求。
	*/
	ImplI1(IUIBox) PDefH1(bool, ResponseFocusRequest, AFocusRequester& w)
		ImplBodyBase1(MSimpleFocusResponser, ResponseFocusRequest, w)

	/*!
	\brief 响应焦点释放。
	*/
	ImplI1(IUIBox) PDefH1(bool, ResponseFocusRelease, AFocusRequester& w)
		ImplBodyBase1(MSimpleFocusResponser, ResponseFocusRelease, w)
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

