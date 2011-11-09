/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywgtview.h
\ingroup UI
\brief 样式无关的图形用户界面部件。
\version r1307;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-11-09 15:18 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWidgetView;
*/


#ifndef YSL_INC_UI_YWGTVIEW_H_
#define YSL_INC_UI_YWGTVIEW_H_

#include "ycomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 方向模块。
class MOriented
{
protected:
	Drawing::Orientation Orientation;

	explicit
	MOriented(Drawing::Orientation);

	DefGetter(Drawing::Orientation, Orientation, Orientation)
};

inline
MOriented::MOriented(Drawing::Orientation o)
	: Orientation(o)
{}


/*!
\brief 可视状态。
*/
class Visual
{
private:
	bool visible; //!< 可见性。
	bool transparent; //!< 透明性。
	Point location; //!< 左上角所在位置（相对于容器的偏移坐标）。
	Size size; //!< 部件大小。

public:
	/*!
	\brief 构造：使用指定边界。
	*/
	Visual(const Rect& = Rect::Empty);

	DefPredicate(Visible, visible)
	DefPredicate(Transparent, transparent) //!< 判断是否透明。

	DefGetter(SPos, X, GetLocation().X)
	DefGetter(SPos, Y, GetLocation().Y)
	DefGetter(SDst, Width, GetSize().Width)
	DefGetter(SDst, Height, GetSize().Height)
	DefGetter(const Point&, Location, location)
	DefGetter(const Size&, Size, size)

	DefSetter(bool, Visible, visible)
	DefSetter(bool, Transparent, transparent) //!< 设置透明性。
	/*!
	\brief 设置位置：横坐标。
	\note 非虚 \c public 实现。
	*/
	PDefH(void, SetX, SPos x)
		ImplBodyBase(Visual, SetLocation, Point(x, GetY()))
	/*!
	\brief 设置位置：纵坐标。
	\note 非虚 \c public 实现。
	*/
	PDefH(void, SetY, SPos y)
		ImplBodyBase(Visual, SetLocation, Point(GetX(), y))
	/*!
	\brief 设置大小：宽。
	\note 非虚 \c public 实现。
	*/
	PDefH(void, SetWidth, SDst w)
		ImplBodyBase(Visual, SetSize, Size(w, GetHeight()))
	/*!
	\brief 设置大小：高。
	\note 非虚 \c public 实现。
	*/
	PDefH(void, SetHeight, SDst h)
		ImplBodyBase(Visual, SetSize, Size(GetWidth(), h))
	/*!
	\brief 设置位置。
	*/
	DefSetter(const Point&, Location, location)
	/*!
	\brief 设置位置。
	\note 非虚 \c public 实现。
	*/
	PDefH(void, SetLocation, SPos x, SPos y)
		ImplBodyBase(Visual, SetLocation, Point(x, y))
	/*!
	\brief 设置大小。
	\note 虚 \c public 实现。
	*/
	DefSetter(const Size&, Size, size)
	/*!
	\brief 设置大小。
	\note 非虚 \c public 实现。
	*/
	PDefH(void, SetSize, SDst w, SDst h)
		ImplBodyBase(Visual, SetSize, Size(w, h))
};


/*!
\brief 部件视图。
*/
class WidgetView
{
private:
	Visual visual; //!< 当前可视状态。
	mutable IWidget* pContainer; //!< 从属的部件容器的指针。

public:
	mutable IWidget* pFocusing; //!< 焦点指针。

	/*!
	\brief 构造：使用指定边界、前景色和背景色。
	*/
	WidgetView(const Rect& = Rect::Empty);
	WidgetView(const WidgetView&);
	WidgetView(WidgetView&&);
	virtual DefEmptyDtor(WidgetView)

	DefPredicateMember(Visible, visual)
	DefPredicateMember(Transparent, visual)

	DefGetterMember(SPos, X, visual)
	DefGetterMember(SPos, Y, visual)
	DefGetterMember(SDst, Width, visual)
	DefGetterMember(SDst, Height, visual)
	DefGetterMember(const Point&, Location, visual)
	DefGetterMember(const Size&, Size, visual)
	DefGetter(IWidget*&, ContainerPtrRef, pContainer)

	DefSetterMember(bool, Visible, visual)
	DefSetterMember(bool, Transparent, visual)
	DefSetterMember(SPos, X, visual)
	DefSetterMember(SPos, Y, visual)
	DefSetterMember(SDst, Width, visual)
	DefSetterMember(SDst, Height, visual)
	virtual DefSetterMember(const Point&, Location, visual)
	virtual DefSetterMember(const Size&, Size, visual)
	PDefH(void, SetLocation, SPos x, SPos y)
		ImplBodyMember(visual, SetLocation, Point(x, y))
	PDefH(void, SetSize, SDst w, SDst h)
		ImplBodyMember(visual, SetSize, Size(w, h))
};

inline
WidgetView::WidgetView(const Rect& r)
	: visual(r), pContainer(), pFocusing()
{}
inline
WidgetView::WidgetView(const WidgetView& wv)
	: visual(wv.visual), pContainer(), pFocusing()
{}
inline
WidgetView::WidgetView(WidgetView&& wv)
	: visual(wv.visual), pContainer(wv.pContainer), pFocusing(wv.pFocusing)
{
	yunsequenced(wv.pContainer = nullptr, wv.pFocusing = nullptr);
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

