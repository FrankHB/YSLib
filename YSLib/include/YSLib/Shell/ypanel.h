/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ypanel.h
\ingroup Shell
\brief 平台无关的图形用户界面面板实现。
\version 0.1145;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-13 20:40:51 +0800;
\par 修改时间:
	2011-04-16 20:50 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YPanel;
*/


#ifndef INCLUDED_YPANEL_H_
#define INCLUDED_YPANEL_H_

#include "ycontrol.h"
#include "yuicont.h"
#include "../Core/yres.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

//! \brief 面板接口。
DeclBasedInterface(IPanel, IUIContainer, virtual IControl)
EndDecl


//! \brief 面板基实现类。
class Panel : public Control, protected Widgets::MUIContainer,
	implements IPanel
{
public:
	typedef Control ParentType;

	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	Panel(const Rect& = Rect::Empty);
	
	virtual
	~Panel();

	ImplI1(IPanel) void
	operator+=(IWidget*);
	ImplI1(IPanel) void
	operator+=(IControl*);
	ImplI1(IPanel) PDefHOperator1(void, +=, GMFocusResponser<IControl>* p)
		ImplBodyBase1(MUIContainer, operator+=, p)
	template<class _type>
	inline void
	operator+=(_type* p)
	{
		return operator+=(Design::MoreConvertible<_type*,
			IControl*, IWidget*>::Cast(p));
	}

	ImplI1(IPanel) bool
	operator-=(IWidget*);
	ImplI1(IPanel) bool
	operator-=(IControl*);
	ImplI1(IPanel) PDefHOperator1(bool, -=, GMFocusResponser<IControl>* p)
		ImplBodyBase1(MUIContainer, operator-=, p)
	template<class _type>
	inline bool
	operator-=(_type* p)
	{
		return operator-=(Design::MoreConvertible<_type*,
			IControl*, IWidget*>::Cast(p));
	}

	ImplI1(IPanel) PDefH0(IControl*, GetFocusingPtr)
		ImplBodyBase0(GMFocusResponser<IControl>, GetFocusingPtr)
	ImplI1(IPanel) PDefH1(IWidget*, GetTopWidgetPtr, const Point& pt)
		ImplBodyBase1(MUIContainer, GetTopWidgetPtr, pt)
	ImplI1(IPanel) PDefH1(IControl*, GetTopControlPtr, const Point& pt)
		ImplBodyBase1(MUIContainer, GetTopControlPtr, pt)

	ImplI1(IPanel) void
	ClearFocusingPtr();

	ImplI1(IPanel) PDefH1(bool, ResponseFocusRequest, AFocusRequester& w)
		ImplBodyBase1(MUIContainer, ResponseFocusRequest, w)

	ImplI1(IPanel) PDefH1(bool, ResponseFocusRelease, AFocusRequester& w)
		ImplBodyBase1(MUIContainer, ResponseFocusRelease, w)

	ImplI1(IPanel) PDefH0(void, Paint)
		ImplBodyBase0(Widget, Paint)

	ImplI1(IPanel) PDefH0(void, Refresh)
		ImplBodyBase0(Widget, Refresh)
};


//! \brief 控件基类。
class YPanel : public YComponent,
	public Panel
{
public:
	typedef YComponent ParentType;

	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	YPanel(const Rect& = Rect::Empty);
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

