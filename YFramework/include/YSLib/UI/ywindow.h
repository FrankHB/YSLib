/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywindow.h
\ingroup UI
\brief 样式无关的图形用户界面窗口。
\version r4747;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-28 16:46:40 +0800;
\par 修改时间:
	2011-11-30 08:41 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWindow;
*/


#ifndef YSL_INC_UI_YWINDOW_H_
#define YSL_INC_UI_YWINDOW_H_

#include "ypanel.h"
#include "../Service/yblit.h"

YSL_BEGIN

#undef YWindowAssert

YSL_BEGIN_NAMESPACE(Components)

#ifdef YCL_USE_YASSERT

#	define YWindowAssert(ptr, comp, func) \
	Components::yassert((ptr) && FetchContext(*(ptr)).IsValid(), \
	"The graphics context is invalid.", __LINE__, __FILE__, #comp, #func)

#else

#	define YWindowAssert(ptr, comp, func) \
	assert((ptr) && FetchContext(*(ptr)).IsValid())

#endif


/*!
\brief 窗口。
\since build 264 。
*/
class Window : public Panel, protected MBackground
{
public:
	/*!
	\brief 构造：使用指定边界和背景图像。
	*/
	explicit
	Window(const Rect& = Rect::Empty,
		const shared_ptr<Drawing::Image>& = share_raw(new Drawing::Image()));
	inline DefDeMoveCtor(Window)

	virtual void
	operator+=(IWidget&);
	virtual void
	operator+=(Window&);
	template<class _type>
	inline void
	operator+=(_type& p)
	{
		return operator+=(static_cast<typename std::conditional<
			std::is_convertible<_type&, Window&>::value,
			Window&, IWidget&>::type>(p));
	}

	virtual bool
	operator-=(IWidget&);
	virtual bool
	operator-=(Window&);
	template<class _type>
	inline bool
	operator-=(_type& p)
	{
		return operator-=(static_cast<typename std::conditional<
			std::is_convertible<_type&, Window&>::value,
			Window&, IWidget&>::type>(p));
	}

	using MUIContainer::Contains;

	using MBackground::GetBackgroundImagePtr;
	using MBackground::GetBackgroundPtr;
	virtual PDefH(IWidget*, GetTopWidgetPtr, const Point& pt,
		bool(&f)(const IWidget&))
		ImplBodyBase(MUIContainer, GetTopWidgetPtr, pt, f)

	virtual void
	Add(IWidget&, ZOrderType = DefaultZOrder);

	/*!
	\brief 提升部件至顶端。

	子部件组中查找指定部件并重新插入至顶端。
	*/
	bool
	MoveToTop(IWidget&);

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const PaintContext&);

	/*!
	\brief 按需更新。
	\note 以父窗口、屏幕优先顺序。
	*/
	virtual void
	Update();
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

