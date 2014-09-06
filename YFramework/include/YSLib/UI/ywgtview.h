/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywgtview.h
\ingroup UI
\brief 样式无关的 GUI 部件。
\version r564
\author FrankHB <frankhb1989@gmail.com>
\since build 258
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2014-09-03 14:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidgetView
*/


#ifndef YSL_INC_UI_ywgtview_h_
#define YSL_INC_UI_ywgtview_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YComponent

namespace YSLib
{

namespace UI
{

/*!
\brief 方向模块。
\since build 170
*/
class YF_API MOriented
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
\note 成员使用非虚 \c public 实现。
\warning 非虚析构。
\since build 168
*/
class YF_API Visual
{
private:
	bool visible; //!< 可见性。
	Point location; //!< 左上角所在位置（相对容器的偏移坐标）。
	Size size; //!< 部件大小。

public:
	/*!
	\brief 构造：使用指定边界。
	\since build 337
	*/
	Visual(const Rect& = {});

	DefPred(const ynothrow, Visible, visible)

	DefGetter(const ynothrow, SPos, X, GetLocation().X)
	DefGetter(const ynothrow, SPos, Y, GetLocation().Y)
	DefGetter(const ynothrow, SDst, Width, GetSize().Width)
	DefGetter(const ynothrow, SDst, Height, GetSize().Height)
	DefGetter(const ynothrow, const Point&, Location, location)
	DefGetter(const ynothrow, const Size&, Size, size)
	//! \since build 307
	DefGetter(ynothrow, Point&, LocationRef, location)
	//! \since build 307
	DefGetter(ynothrow, Size&, SizeRef, size)

	DefSetter(bool, Visible, visible)
	/*!
	\brief 设置位置：横坐标。
	*/
	PDefH(void, SetX, SPos x)
		ImplBodyBase(Visual, SetLocation, Point(x, GetY()))
	/*!
	\brief 设置位置：纵坐标。
	*/
	PDefH(void, SetY, SPos y)
		ImplBodyBase(Visual, SetLocation, Point(GetX(), y))
	/*!
	\brief 设置大小：宽。
	*/
	PDefH(void, SetWidth, SDst w)
		ImplBodyBase(Visual, SetSize, Size(w, GetHeight()))
	/*!
	\brief 设置大小：高。
	*/
	PDefH(void, SetHeight, SDst h)
		ImplBodyBase(Visual, SetSize, Size(GetWidth(), h))
	/*!
	\brief 设置位置。
	*/
	DefSetter(const Point&, Location, location)
	/*!
	\brief 设置位置。
	*/
	PDefH(void, SetLocation, SPos x, SPos y)
		ImplBodyBase(Visual, SetLocation, Point(x, y))
	/*!
	\brief 设置大小。
	*/
	DefSetter(const Size&, Size, size)
	/*!
	\brief 设置大小。
	*/
	PDefH(void, SetSize, SDst w, SDst h)
		ImplBodyBase(Visual, SetSize, Size(w, h))
};


/*!
\brief 部件视图。
\since build 259
*/
class YF_API View
{
private:
	Visual visual; //!< 当前可视状态。

public:
	//! \since build 375
	//@{
	mutable IWidget* ContainerPtr; //!< 从属的部件容器的指针。
	/*!
	\brief 从属的部件指针。

	逻辑依赖的部件指针，用于提供边界、容器和焦点以外属性的默认值。
	*/
	mutable IWidget* DependencyPtr;
	mutable IWidget* FocusingPtr; //!< 焦点指针。
	//@}

	/*!
	\brief 构造：使用指定边界、前景色和背景色。
	\since build 337
	*/
	View(const Rect& r = {})
		: visual(r), ContainerPtr(), DependencyPtr(), FocusingPtr()
	{}
	View(const View& v)
		: visual(v.visual), ContainerPtr(), DependencyPtr(), FocusingPtr()
	{}
	View(View&& v)
		: visual(v.visual), ContainerPtr(v.ContainerPtr),
		DependencyPtr(v.DependencyPtr), FocusingPtr(v.FocusingPtr)
	{
		yunseq(v.ContainerPtr = {}, v.DependencyPtr = {}, v.FocusingPtr = {});
	}
	virtual DefDeDtor(View)

	/*!
	\brief 复制赋值：仅可视状态。
	\since build 295
	*/
	View&
	operator=(const View& v)
	{
		visual = v.visual;
		return *this;
	}
	/*!
	\brief 转移赋值：仅可视状态。
	\since build 295
	*/
	View&
	operator=(View&& v)
	{
		visual = std::move(v.visual);
		return *this;
	}

	bool
	IsVisible() const ynothrow;

	DefGetterMem(const ynothrow, SPos, X, visual)
	DefGetterMem(const ynothrow, SPos, Y, visual)
	DefGetterMem(const ynothrow, SDst, Width, visual)
	DefGetterMem(const ynothrow, SDst, Height, visual)
	DefGetterMem(const ynothrow, const Point&, Location, visual)
	DefGetterMem(const ynothrow, const Size&, Size, visual)
	//! \since build 307
	DefGetterMem(ynothrow, Point&, LocationRef, visual)
	//! \since build 307
	DefGetterMem(ynothrow, Size&, SizeRef, visual)

	void
	SetVisible(bool);
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

	//! \since build 409
	virtual DefClone(const, View)
};

} // namespace UI;

} // namespace YSLib;

#endif

