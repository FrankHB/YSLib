/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ypanel.h
\ingroup UI
\brief 样式无关的图形用户界面面板。
\version 0.1252;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-13 20:40:51 +0800;
\par 修改时间:
	2011-07-21 12:28 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YPanel;
*/


#ifndef YSL_INC_UI_YPANEL_H_
#define YSL_INC_UI_YPANEL_H_

#include "ycontrol.h"
#include "yuicont.h"
#include "../Service/yres.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

//! \brief 面板。
class Panel : public Control, protected Widgets::MUIContainer
{
public:
	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	Panel(const Rect& = Rect::Empty);

	/*!
	\brief 析构：空实现。
	*/
	virtual
	~Panel()
	{}

	virtual void
	operator+=(IWidget&);
	virtual void
	operator+=(IControl&);
	virtual PDefHOperator1(void, +=, GMFocusResponser<IControl>& rsp)
		ImplBodyBase1(MUIContainer, operator+=, rsp)
	template<class _type>
	inline void
	operator+=(_type& obj)
	{
		return operator+=(MoreConvertible<_type&, IControl&,
			IWidget&>::Cast(obj));
	}

	virtual bool
	operator-=(IWidget&);
	virtual bool
	operator-=(IControl&);
	virtual PDefHOperator1(bool, -=, GMFocusResponser<IControl>& rsp)
		ImplBodyBase1(MUIContainer, operator-=, rsp)
	template<class _type>
	inline bool
	operator-=(_type& obj)
	{
		return operator-=(MoreConvertible<_type&, IControl&,
			IWidget&>::Cast(obj));
	}

	using MUIContainer::Contains;

	virtual PDefH0(IControl*, GetFocusingPtr)
		ImplBodyBase0(GMFocusResponser<IControl>, GetFocusingPtr)
	virtual PDefH1(IWidget*, GetTopWidgetPtr, const Point& pt)
		ImplBodyBase1(MUIContainer, GetTopWidgetPtr, pt)
	virtual PDefH1(IControl*, GetTopControlPtr, const Point& pt)
		ImplBodyBase1(MUIContainer, GetTopControlPtr, pt)

	virtual void
	ClearFocusingPtr();

	virtual PDefH1(bool, ResponseFocusRequest, AFocusRequester& req)
		ImplBodyBase1(MUIContainer, ResponseFocusRequest, req)

	virtual PDefH1(bool, ResponseFocusRelease, AFocusRequester& req)
		ImplBodyBase1(MUIContainer, ResponseFocusRelease, req)

	virtual PDefH3(Rect, Refresh, const Graphics& g, const Point& pt,
		const Rect& r)
		ImplBodyBase3(Control, Refresh, g, pt, r)
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

