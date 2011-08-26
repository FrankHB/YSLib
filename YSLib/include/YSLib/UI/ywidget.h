/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywidget.h
\ingroup UI
\brief 样式无关的图形用户界面部件。
\version r5918;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-08-25 13:32 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Widget;
*/


#ifndef YSL_INC_UI_YWIDGET_H_
#define YSL_INC_UI_YWIDGET_H_

#include "ycomp.h"
#include "yfocus.h"
#include "../Service/ydraw.h"
#include "../Service/ygdi.h"

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


/*!
\brief 部件渲染器。
*/
class WidgetRenderer : public noncopyable
{
protected:
	static const Graphics InvalidGraphics; //!< 无效图形接口上下文。

public:
	virtual DefEmptyDtor(WidgetRenderer)

	/*!
	\brief 判断是否需要刷新。
	\note 总是需要刷新。
	*/
	virtual PDefH0(bool, RequiresRefresh) const
		ImplRet(true)

	/*!
	\brief 取无效区域。
	\note 空实现。
	*/
	virtual void
	GetInvalidatedArea(Rect&) const
	{}
	/*!
	\brief 取图形接口上下文。
	\note 返回无效图形接口上下文。
	*/
	virtual DefGetter(const Graphics&, Context, InvalidGraphics)

	/*!
	\brief 设置缓冲区大小。
	\note 空实现。
	*/
	virtual void
	SetSize(const Size&)
	{}

	/*!
	\brief 清除无效区域。
	\note 空实现。
	*/
	virtual void
	ClearInvalidation()
	{}

	/*!
	\brief 提交无效区域。
	\note 空实现。
	*/
	virtual void
	CommitInvalidation(const Rect&)
	{}

	/*
	\brief 以纯色填充无效区域。
	\note 空实现。
	*/
	virtual void
	FillInvalidation(Color)
	{}

	/*!
	\brief 更新至指定图形设备上下文的指定点。
	\param 意义同 IWidget::Refresh 。
	\note 空实现。
	*/
	virtual void
	UpdateTo(const Graphics&, const Point&, const Rect&) const
	{}
};


/*!
\brief 带缓冲的部件渲染器。
*/
class BufferedWidgetRenderer : public WidgetRenderer
{
protected:
	mutable Rect rInvalidated; \
		//!< 无效区域：包含所有新绘制请求的区域（不一定是最小的）。

public:
	Drawing::BitmapBuffer Buffer; //!< 显示缓冲区。

	/*!
	\brief 判断是否需要刷新。
	\note 若无效区域长宽都不为零，则需要刷新。
	*/
	virtual bool
	RequiresRefresh() const;

	/*!
	\brief 取无效区域。
	*/
	DefGetter(const Rect&, InvalidatedArea, rInvalidated)
	/*!
	\brief 取无效区域。
	\note 通过参数返回。
	*/
	virtual void
	GetInvalidatedArea(Rect&) const;
	/*!
	\brief 取图形接口上下文。
	\return 缓冲区图形接口上下文。
	*/
	virtual DefGetter(const Graphics&, Context, Buffer)

	/*!
	\brief 设置缓冲区大小。
	*/
	virtual void
	SetSize(const Size&);

	/*!
	\brief 清除无效区域。
	\note 通过设置大小为 Size::Zero 使无效区域被忽略。
	*/
	virtual void
	ClearInvalidation();

	/*!
	\brief 提交无效区域。
	\note 合并至现有无效区域中。
	\note 由于无效区域的形状限制，结果可能会包含部分有效区域。
	*/
	virtual void
	CommitInvalidation(const Rect&);

	/*
	\brief 以纯色填充无效区域。
	*/
	virtual void
	FillInvalidation(Color);

	/*!
	\brief 更新至指定图形设备上下文的指定点。
	\param 意义同 IWidget::Refresh 。
	*/
	virtual void
	UpdateTo(const Graphics&, const Point&, const Rect&) const;
};

YSL_BEGIN_NAMESPACE(Widgets)

//! \brief 部件接口。
DeclInterface(IWidget)
	DeclIEntry(bool IsVisible() const) //!< 判断是否可见。
	DeclIEntry(bool IsTransparent() const) //!< 判断是否透明。

	DeclIEntry(const Point& GetLocation() const)
	DeclIEntry(const Size& GetSize() const)
	/*!
	\brief 取容器指针的引用。
	\warning 注意修改容器指针时，应保持和容器包含部件的状态同步。
	*/
	DeclIEntry(IWidget*& GetContainerPtrRef() const)
	/*!
	\brief 取渲染器。
	*/
	DeclIEntry(WidgetRenderer& GetRenderer() const)
	/*!
	\brief 取焦点对象指针。
	\return 若为容器且存在子部件中的焦点对象则返回指针，否则返回 \c nullptr 。
	*/
	DeclIEntry(IControl* GetFocusingPtr()) \
	/*!
	\brief 取指定的点所在的可见部件的指针。
	\return 若为容器且存在子部件中的可见部件则返回指针，否则返回 \c nullptr 。
	\note 使用部件坐标。
	*/
	DeclIEntry(IWidget* GetTopWidgetPtr(const Point&)) \
	/*!
	\brief 取指定的点所在的可见控件的指针。
	\return 若为容器且存在子部件中的可见控则返回指针，否则返回 \c nullptr 。
	\note 使用部件坐标。
	*/
	DeclIEntry(IControl* GetTopControlPtr(const Point&)) \

	DeclIEntry(void SetVisible(bool)) //!< 设置可见。
	DeclIEntry(void SetTransparent(bool)) //!< 设置透明。
	DeclIEntry(void SetLocation(const Point&)) \
		//!< 设置左上角所在位置（相对于父容器的偏移坐标）。
	DeclIEntry(void SetSize(const Size&)) \
		//!< 设置大小。

	/*!
	\brief 清除焦点指针。
	\note 若此部件非容器则无效。
	*/
	DeclIEntry(void ClearFocusingPtr())

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	\param g 图形接口上下文。
	\param pt 相对图形接口上下文的偏移坐标，指定部件左上角的位置。
	\param r 相对于图形接口上下文的正则矩形，指定需要保证被刷新的边界区域。
	\return 实际被绘制的界面区域。
	\note 边界仅为暗示，允许实现忽略，但可以保证边界内的区域保持最新显示状态。
	\note 若部件的内部状态能够保证显示状态最新，则返回的区域可能比参数 r 更小。
	*/
	DeclIEntry(Rect Refresh(const Graphics& g, const Point& pt, const Rect& r))

	/*!
	\brief 响应焦点请求。
	\note 若此部件非容器则无效。
	*/
	DeclIEntry(bool ResponseFocusRequest(AFocusRequester&))

	/*!
	\brief 响应焦点释放。
	\note 若此部件非容器则无效。
	*/
	DeclIEntry(bool ResponseFocusRelease(AFocusRequester&))
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
\ingroup HelperFunction
\brief 取部件的容器指针。
\note 使用此函数确保返回值传递的值语义。
*/
inline IWidget*
FetchContainerPtr(const IWidget& wgt)
{
	return wgt.GetContainerPtrRef();
}

/*
\ingroup HelperFunction
\brief 取指定部件的图形接口上下文。
*/
inline const Graphics&
FetchContext(const IWidget& wgt)
{
	return wgt.GetRenderer().GetContext();
}

/*!
\brief 取部件边界。
*/
inline Rect
GetBoundsOf(const IWidget& w)
{
	return Rect(w.GetLocation(), w.GetSize());
}

/*!
\brief 设置部件边界。
*/
void
SetBoundsOf(IWidget&, const Rect& r);

/*!
\brief 设置部件的无效区域。
*/
void
SetInvalidationOf(IWidget&);

/*!
\brief 在容器设置部件的无效区域。
\note 若容器不存在则忽略。
*/
void
SetInvalidationToParent(IWidget&);

/*!
\brief 无效化：使部件区域在窗口缓冲区中无效。
*/
void
Invalidate(IWidget&);

/*!
\brief 级联无效化：使相对于部件的指定区域在直接和简洁的窗口缓冲区中无效。
*/
void
InvalidateCascade(IWidget&, const Rect&);

/*
\brief 渲染：若缓冲存储不可用则刷新指定部件。
*/
Rect
Render(IWidget&, const Graphics&, const Point&, const Rect&);

/*
\brief 渲染子部件。
*/
void
RenderChild(IWidget&, const Graphics&, const Point&, const Rect&);

/*!
\brief 请求提升至容器顶端。

\todo 完全实现提升 IWidget 至容器顶端（目前仅实现 IControl 且
	父容器为 Desktop 的情形）。
*/
void
RequestToTop(IWidget&);

/*!
\brief 更新部件至指定图形设备上下文的指定点。
\note 后三个参数意义同 IWidget::Refresh 。
*/
void
Update(const IWidget&, const Graphics&, const Point&, const Rect&);

/*!
\brief 验证：若部件的缓冲区存在，绘制缓冲区使之有效。
\return 实际绘制的区域，意义同 IWidget::Refresh 。
*/
Rect
Validate(IWidget&);


/*!
\brief 显示部件。
\note 设置可见性和容器（若存在）背景重绘状态并设置自身刷新状态。
*/
void
Show(IWidget&);

/*!
\brief 隐藏部件。
\note 设置不可见性和容器（若存在）背景重绘状态并取消自身刷新状态。
*/
void
Hide(IWidget&);


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


//! \brief 可视样式。
class Visual : public noncopyable
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
	\note 非虚 \c public 实现。
	*/
	PDefH1(void, SetX, SPos x)
		ImplBodyBase1(Visual, SetLocation, Point(x, GetY()))
	/*!
	\brief 设置位置：纵坐标。
	\note 非虚 \c public 实现。
	*/
	PDefH1(void, SetY, SPos y)
		ImplBodyBase1(Visual, SetLocation, Point(GetX(), y))
	/*!
	\brief 设置大小：宽。
	\note 非虚 \c public 实现。
	*/
	PDefH1(void, SetWidth, SDst w)
		ImplBodyBase1(Visual, SetSize, Size(w, GetHeight()))
	/*!
	\brief 设置大小：高。
	\note 非虚 \c public 实现。
	*/
	PDefH1(void, SetHeight, SDst h)
		ImplBodyBase1(Visual, SetSize, Size(GetWidth(), h))
	/*!
	\brief 设置位置。
	\note 虚 \c public 实现。
	*/
	virtual DefSetter(const Point&, Location, location)
	/*!
	\brief 设置位置。
	\note 非虚 \c public 实现。
	*/
	PDefH2(void, SetLocation, SPos x, SPos y)
		ImplBodyBase1(Visual, SetLocation, Point(x, y))
	/*!
	\brief 设置大小。
	\note 虚 \c public 实现。
	*/
	virtual void
	SetSize(const Size& s);
	/*!
	\brief 设置大小。
	\note 非虚 \c public 实现。
	*/
	PDefH2(void, SetSize, SDst w, SDst h)
		ImplBodyBase1(Visual, SetSize, Size(w, h))
};


//! \brief 部件。
class Widget : public Visual,
	virtual implements IWidget
{
private:
	mutable IWidget* pContainer; //!< 从属的部件容器的指针。
	unique_ptr<WidgetRenderer> pRenderer; //!< 渲染器指针。

public:
	explicit
	Widget(const Rect& = Rect::Empty,
		Color = Drawing::ColorSpace::White, Color = Drawing::ColorSpace::Black);

	ImplI1(IWidget) DefPredicateBase(Visible, Visual)
	ImplI1(IWidget) DefPredicateBase(Transparent, Visual)

	ImplI1(IWidget) DefGetterBase(const Point&, Location, Visual)
	ImplI1(IWidget) DefGetterBase(const Size&, Size, Visual)
	ImplI1(IWidget) DefGetter(IWidget*&, ContainerPtrRef, pContainer)
	ImplI1(IWidget) DefGetter(WidgetRenderer&, Renderer, *pRenderer)
	ImplI1(IWidget) PDefH0(IControl*, GetFocusingPtr)
		ImplRet(nullptr)
	ImplI1(IWidget) PDefH1(IWidget*, GetTopWidgetPtr, const Point&)
		ImplRet(nullptr)
	ImplI1(IWidget) PDefH1(IControl*, GetTopControlPtr, const Point&)
		ImplRet(nullptr)

	ImplI1(IWidget) DefSetterBase(bool, Visible, Visual)
	ImplI1(IWidget) DefSetterBase(bool, Transparent, Visual)
	ImplI1(IWidget) DefSetterBase(const Point&, Location, Visual)
	ImplI1(IWidget) DefSetterBase(const Size&, Size, Visual)
	/*!
	\brief 设置渲染器为指定指针指向的对象。
	\note 若指针为空，则使用新建的 WidgetRenderer 对象。
	\note 取得指定对象的所有权。
	*/
	void
	SetRenderer(unique_ptr<WidgetRenderer>&&);

	ImplI1(IWidget) PDefH0(void, ClearFocusingPtr)
		ImplRet(static_cast<void>(this))

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	ImplI1(IWidget) Rect
	Refresh(const Graphics&, const Point&, const Rect&);

	ImplI1(IWidget) PDefH1(bool, ResponseFocusRequest, AFocusRequester&)
		ImplRet(false)

	ImplI1(IWidget) PDefH1(bool, ResponseFocusRelease, AFocusRequester&)
		ImplRet(false)
};

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

