/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywgtview.h
\ingroup UI
\brief 样式无关的图形用户界面部件。
\version r1487;
\author FrankHB<frankhb1989@gmail.com>
\since build 258 。
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2012-05-09 11:21 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWidgetView;
*/


#ifndef YSL_INC_UI_YWGTVIEW_H_
#define YSL_INC_UI_YWGTVIEW_H_

#include "ycomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 方向模块。
\since build 170 。
*/
class MOriented
{
protected:
	Drawing::Orientation Orientation;

	explicit
	MOriented(Drawing::Orientation o)
		: Orientation(o)
	{}

	DefGetter(const ynothrow, Drawing::Orientation, Orientation, Orientation)
};


/*!
\brief 可视状态。
\warning 非虚析构。
\since build 168 。
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

	DefPred(const ynothrow, Visible, visible)
	DefPred(const ynothrow, Transparent, transparent) //!< 判断是否透明。

	DefGetter(const ynothrow, SPos, X, GetLocation().X)
	DefGetter(const ynothrow, SPos, Y, GetLocation().Y)
	DefGetter(const ynothrow, SDst, Width, GetSize().Width)
	DefGetter(const ynothrow, SDst, Height, GetSize().Height)
	DefGetter(const ynothrow, const Point&, Location, location)
	DefGetter(const ynothrow, const Size&, Size, size)
	//! \since build 307 。
	DefGetter(ynothrow, Point&, LocationRef, location)
	//! \since build 307 。
	DefGetter(ynothrow, Size&, SizeRef, size)

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
\since build 259 。
*/
class View
{
private:
	Visual visual; //!< 当前可视状态。

public:
	mutable IWidget* pContainer; //!< 从属的部件容器的指针。
	/*!
	\brief 从属的部件指针。

	逻辑依赖的部件指针，用于提供边界、容器和焦点以外属性的默认值。
	\since build 283 。
	*/
	mutable IWidget* pDependency;
	mutable IWidget* pFocusing; //!< 焦点指针。

	/*!
	\brief 构造：使用指定边界、前景色和背景色。
	*/
	View(const Rect& r = Rect::Empty)
		: visual(r), pContainer(), pDependency(), pFocusing()
	{}
	View(const View& v)
		: visual(v.visual), pContainer(), pDependency(), pFocusing()
	{}
	View(View&& v)
		: visual(v.visual), pContainer(v.pContainer), pDependency(v.pDependency),
		pFocusing(v.pFocusing)
	{
		yunseq(v.pContainer = nullptr, v.pDependency = nullptr,
			v.pFocusing = nullptr);
	}
	virtual DefClone(View, Clone)
	virtual DefEmptyDtor(View)

	/*!
	\brief 复制赋值：仅可视状态。
	\since build 295 。
	*/
	View&
	operator=(const View& v)
	{
		visual = v.visual;
		return *this;
	}
	/*!
	\brief 转移赋值：仅可视状态。
	\since build 295 。
	*/
	View&
	operator=(View&& v)
	{
		visual = std::move(v.visual);
		return *this;
	}

	bool
	IsVisible() const ynothrow;
	bool
	IsTransparent() const ynothrow;

	DefGetterMem(const ynothrow, SPos, X, visual)
	DefGetterMem(const ynothrow, SPos, Y, visual)
	DefGetterMem(const ynothrow, SDst, Width, visual)
	DefGetterMem(const ynothrow, SDst, Height, visual)
	DefGetterMem(const ynothrow, const Point&, Location, visual)
	DefGetterMem(const ynothrow, const Size&, Size, visual)
	//! \since build 307 。
	DefGetterMem(ynothrow, Point&, LocationRef, visual)
	//! \since build 307 。
	DefGetterMem(ynothrow, Size&, SizeRef, visual)

	void
	SetVisible(bool);
	void
	SetTransparent(bool);
	DefSetterMem(SPos, X, visual)
	DefSetterMem(SPos, Y, visual)
	DefSetterMem(SDst, Width, visual)
	DefSetterMem(SDst, Height, visual)
	virtual DefSetterMem(const Point&, Location, visual)
	virtual DefSetterMem(const Size&, Size, visual)
	PDefH(void, SetLocation, SPos x, SPos y)
		ImplBodyMem(visual, SetLocation, Point(x, y))
	PDefH(void, SetSize, SDst w, SDst h)
		ImplBodyMem(visual, SetSize, Size(w, h))
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

