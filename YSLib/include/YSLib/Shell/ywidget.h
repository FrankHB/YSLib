/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywidget.h
\ingroup Shell
\brief 平台无关的图形用户界面部件实现。
\version 0.5912;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-04-16 20:51 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YWidget;
*/


#ifndef INCLUDED_YWIDGET_H_
#define INCLUDED_YWIDGET_H_

#include "ycomp.h"
#include "ygdi.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//名称引用。
using Drawing::PixelType;
using Drawing::BitmapPtr;
using Drawing::ConstBitmapPtr;
using Drawing::ScreenBufferType;
using Drawing::Color;

using Drawing::Point;
using Drawing::Vec;
using Drawing::Size;
using Drawing::Rect;

using Drawing::Graphics;


YSL_BEGIN_NAMESPACE(Widgets)

//前向声明。
PDeclInterface(IUIBox)
PDeclInterface(IUIContainer)
class Widget;

// GUI 接口和部件定义。

//! \brief 部件接口。
DeclInterface(IWidget)
	DeclIEntry(bool IsVisible() const) //!< 判断是否可见。
	DeclIEntry(bool IsTransparent() const) //!< 判断是否透明。

	DeclIEntry(const Point& GetLocation() const)
	DeclIEntry(const Size& GetSize() const)
	DeclIEntry(IUIBox*& GetContainerPtr() const)

	DeclIEntry(void SetVisible(bool)) //!< 设置可见。
	DeclIEntry(void SetTransparent(bool)) //!< 设置透明。
	DeclIEntry(void SetLocation(const Point&)) \
		//!< 设置左上角所在位置（相对于容器的偏移坐标）。
	DeclIEntry(void SetSize(const Size&)) \
		//!< 设置大小。

	/*!
	\brief 绘制界面。
	\warning 可能不检查缓冲区指针是否为空。
	*/
	DeclIEntry(void Paint())

	//! \brief 刷新至窗口缓冲区。
	DeclIEntry(void Refresh())
EndDecl


/*!
\brief 判断点是否在部件的可视区域内。
*/
bool
Contains(const IWidget&, SPos, SPos);
/*!
\brief 判断点是否在部件的可视区域内。
*/
inline bool
Contains(const IWidget& w, const Point& p)
{
	return Contains(w, p.X, p.Y);
}

/*!
\brief 判断点是否在可见部件的可视区域内。
*/
bool
ContainsVisible(const IWidget& w, SPos x, SPos y);
/*!
\brief 判断点是否在可见部件的可视区域内。
*/
inline bool
ContainsVisible(const IWidget& w, const Point& p)
{
	return ContainsVisible(w, p.X, p.Y);
}


/*!
\brief 请求提升至容器顶端。

\todo 完全实现提升 IWidget 至容器顶端（目前仅实现 IControl 且
	父容器为 YDesktop 的情形）。
*/
void
RequestToTop(IWidget&);


/*!
\brief 取部件边界。
*/
template<class _tWidget>
inline Rect
GetBoundsOf(const _tWidget& w)
{
	return Rect(w.GetLocation(), w.GetSize());
}

/*!
\brief 设置部件边界。
*/
void
SetBoundsOf(IWidget&, const Rect& r);


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


//! \brief 窗口对象模块。
class MWindowObject
{
private:
	HWND hWindow; //!< 从属的窗口的句柄。

protected:
	MWindowObject(HWND);

public:
	/*!
	\brief 判断是否属于窗口句柄指定的窗口。
	*/
	bool
	BelongsTo(HWND) const;

	DefGetter(HWND, WindowHandle, hWindow)
};

inline
MWindowObject::MWindowObject(HWND hWnd)
	: hWindow(hWnd)
{}

inline bool
MWindowObject::BelongsTo(HWND hWnd) const
{
	return hWindow == hWnd;
}


//! \brief 可视样式基实现类。
class Visual : public NonCopyable
{
private:
	bool visible; //!< 可见性。
	bool transparent; //!< 透明性。
	Point location; //!< 左上角所在位置（相对于容器的偏移坐标）。
	Size size; //!< 部件大小。

public:
	Color BackColor; //!< 默认背景色。
	Color ForeColor; //!< 默认前景色。

	/*!
	\brief 构造：使用指定边界、前景色和背景色。
	*/
	explicit
	Visual(const Rect& = Rect::Empty,
		Color = Drawing::ColorSpace::White, Color = Drawing::ColorSpace::Black);
	virtual DefEmptyDtor(Visual)

	DefPredicate(Visible, visible)
	DefPredicate(Transparent, transparent)

	DefGetter(SPos, X, GetLocation().X)
	DefGetter(SPos, Y, GetLocation().Y)
	DefGetter(SDst, Width, GetSize().Width)
	DefGetter(SDst, Height, GetSize().Height)
	DefGetter(const Point&, Location, location)
	DefGetter(const Size&, Size, size)

	DefSetter(bool, Visible, visible)
	DefSetter(bool, Transparent, transparent)
	/*!
	\brief 设置位置：横坐标。
	\note 非虚公有实现。
	*/
	PDefH1(void, SetX, SPos x)
		ImplBodyBase1(Visual, SetLocation, Point(x, GetY()))
	/*!
	\brief 设置位置：纵坐标。
	\note 非虚公有实现。
	*/
	PDefH1(void, SetY, SPos y)
		ImplBodyBase1(Visual, SetLocation, Point(GetX(), y))
	/*!
	\brief 设置大小：宽。
	\note 非虚公有实现。
	*/
	PDefH1(void, SetWidth, SDst w)
		ImplBodyBase1(Visual, SetSize, Size(w, GetHeight()))
	/*!
	\brief 设置大小：高。
	\note 非虚公有实现。
	*/
	PDefH1(void, SetHeight, SDst h)
		ImplBodyBase1(Visual, SetSize, Size(GetWidth(), h))
	/*!
	\brief 设置位置。
	\note 虚公有实现。
	*/
	virtual DefSetter(const Point&, Location, location)
	/*!
	\brief 设置位置。
	\note 非虚公有实现。
	*/
	PDefH2(void, SetLocation, SPos x, SPos y)
		ImplBodyBase1(Visual, SetLocation, Point(x, y))
	/*!
	\brief 设置大小。
	\note 虚公有实现。
	*/
	virtual void
	SetSize(const Size& s);
	/*!
	\brief 设置大小。
	\note 非虚公有实现。
	*/
	PDefH2(void, SetSize, SDst w, SDst h)
		ImplBodyBase1(Visual, SetSize, Size(w, h))
};


//! \brief 部件基实现类。
class Widget : public Visual
{
private:
	mutable IUIBox* pContainer; //!< 从属的部件容器的指针。

public:
	explicit
	Widget(const Rect& = Rect::Empty,
		Color = Drawing::ColorSpace::White, Color = Drawing::ColorSpace::Black);

	/*!
	\brief 判断是否属于指定部件容器指针指定的部件容器。
	*/
	bool
	BelongsTo(IUIBox*) const;

	DefGetter(IUIBox*&, ContainerPtr, pContainer)

	/*!
	\brief 绘制界面。
	*/
	virtual void
	Paint();

	/*!
	\brief 刷新至窗口缓冲区。
	*/
	virtual void
	Refresh();
};

inline bool
Widget::BelongsTo(IUIBox* pCon) const
{
	return pContainer == pCon;
}


//! \brief 部件。
class YWidget : public GMCounter<YWidget>, public YComponent,
	public Widget,
	implements IWidget
{
public:
	typedef YComponent ParentType;

	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	YWidget(const Rect& = Rect::Empty);

	ImplI1(IWidget) DefPredicateBase(Visible, Visual)
	ImplI1(IWidget) DefPredicateBase(Transparent, Visual)

	ImplI1(IWidget) DefGetterBase(const Point&, Location, Visual)
	ImplI1(IWidget) DefGetterBase(const Size&, Size, Visual)
	ImplI1(IWidget) DefGetterBase(IUIBox*&, ContainerPtr, Widget)

	ImplI1(IWidget) DefSetterBase(bool, Visible, Visual)
	ImplI1(IWidget) DefSetterBase(bool, Transparent, Visual)
	ImplI1(IWidget) DefSetterBase(const Point&, Location, Visual)
	ImplI1(IWidget) DefSetterBase(const Size&, Size, Visual)

	ImplI1(IWidget) PDefH0(void, Paint)
		ImplBodyBase0(Widget, Paint)

	ImplI1(IWidget) PDefH0(void, Refresh)
		ImplBodyBase0(Widget, Refresh)
};

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

