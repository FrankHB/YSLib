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
\version 0.5726;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 + 08:00;
\par 修改时间:
	2010-01-01 18:35 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YWidget;
*/


#ifndef INCLUDED_YWIDGET_H_
#define INCLUDED_YWIDGET_H_

#include "ycomp.h"
#include "yfocus.h"
#include "../Core/yres.h"
#include "ytext.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

// GUI 断言。

#undef YWidgetAssert

#ifdef YCL_USE_YASSERT

/*!
\brief 断言：判断所给表达式，如果为假给出指定错误信息。
*/
void
yassert(bool, const char*, int, const char*, const char*, const char*);

#	define YWidgetAssert(ptr, comp, func) \
	Components::yassert((ptr) && FetchDirectWindowPtr(*(ptr)), \
		"The direct window handle is null.", __LINE__, __FILE__, #comp, #func)

#else

#	define YWidgetAssert(ptr, comp, func) \
	assert((ptr) && FetchDirectWindowPtr(*(ptr)))

#endif

//名称引用。
using Drawing::PixelType;
using Drawing::BitmapPtr;
using Drawing::ConstBitmapPtr;
using Drawing::ScreenBufferType;
using Drawing::Color;
namespace ColorSpace = Drawing::ColorSpace;

using Drawing::Point;
using Drawing::Vec;
using Drawing::Size;
using Drawing::Rect;

using Drawing::YImage;


YSL_BEGIN_NAMESPACE(Widgets)

//前向声明。
class Widget;

// GUI 接口和部件定义。

//! \brief 部件接口。
DeclInterface(IWidget)
	DeclIEntry(bool IsVisible() const) //!< 判断是否可见。
	DeclIEntry(bool IsTransparent() const) //!< 判断是否透明。
	DeclIEntry(bool IsBgRedrawed() const) //!< 判断是否需要重绘。

	DeclIEntry(const Point& GetLocation() const)
	DeclIEntry(const Size& GetSize() const)
	DeclIEntry(IUIBox* GetContainerPtr() const)

	DeclIEntry(void SetVisible(bool)) //!< 设置可见。
	DeclIEntry(void SetTransparent(bool)) //!< 设置透明。
	DeclIEntry(void SetBgRedrawed(bool)) //!< 设置重绘状态。
	DeclIEntry(void SetLocation(const Point&)) \
		//!< 设置左上角所在位置（相对于容器的偏移坐标）。
	DeclIEntry(void SetSize(const Size&)) \
		//!< 设置大小。

	/*!
	\breif 绘制背景。
	\warning 可能不检查缓冲区指针是否为空。
	*/
	DeclIEntry(void DrawBackground())

	/*!
	\breif 绘制前景。
	\warning 可能不检查缓冲区指针是否为空。
	*/
	DeclIEntry(void DrawForeground())

	//! \brief 刷新至窗口缓冲区。
	DeclIEntry(void Refresh())

	//! \brief 请求提升至容器顶端。
	DeclIEntry(void RequestToTop())
EndDecl


//! \brief 固定部件容器接口。
DeclBasedInterface(IUIBox, virtual IWidget)
	DeclIEntry(IVisualControl* GetFocusingPtr()) //!< 取焦点对象指针。
	DeclIEntry(IWidget* GetTopWidgetPtr(const Point&)) \
		//!< 取指定的点（屏幕坐标）所处的部件的指针。
	DeclIEntry(IVisualControl* GetTopVisualControlPtr(const Point&)) \
		//!< 取指定的点（屏幕坐标）所处的可视控件的指针。

	//! \brief 清除焦点指针。
	DeclIEntry(void ClearFocusingPtr())

	//! \brief 响应焦点请求。
	DeclIEntry(bool ResponseFocusRequest(AFocusRequester&))

	//! \brief 响应焦点释放。
	DeclIEntry(bool ResponseFocusRelease(AFocusRequester&))
EndDecl


//部件容器接口。
DeclBasedInterface(IUIContainer, virtual IUIBox)
	DeclIEntry(void operator+=(IWidget&)) //!< 向部件组添加部件。
	DeclIEntry(bool operator-=(IWidget&)) //!< 从部件组移除部件。
	DeclIEntry(void operator+=(IVisualControl&)) //!< 向焦点对象组添加可视控件。
	DeclIEntry(bool operator-=(IVisualControl&)) //!< 从焦点对象组移除可视控件。
	DeclIEntry(void operator+=(GMFocusResponser<IVisualControl>&)) \
		//!< 向焦点对象组添加子焦点对象容器。
	DeclIEntry(bool operator-=(GMFocusResponser<IVisualControl>&)) \
		//!< 从焦点对象组移除子焦点对象容器。
EndDecl


/*!
\brief 判断点是否在部件的可视区域内。
*/
bool
Contains(const IWidget& w, SPOS x, SPOS y);
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
ContainsVisible(const IWidget& w, SPOS x, SPOS y);
/*!
\brief 判断点是否在可见部件的可视区域内。
*/
inline bool
ContainsVisible(const IWidget& w, const Point& p)
{
	return ContainsVisible(w, p.X, p.Y);
}


/*!
\brief 取部件边界。
*/
Rect
GetBoundsOf(IWidget&);

/*!
\brief 设置部件边界。
*/
void
SetBoundsOf(IWidget&, const Rect& r);


/*!
\brief 取指针指定的部件的直接窗口句柄。
*/
template<class _tWidget>
IWindow*
FetchWidgetDirectWindowPtr(_tWidget* pWgt)
{
	IWindow* pWnd(NULL);

	while(pWgt && !(pWnd = dynamic_cast<IWindow*>(pWgt)))
		pWgt = pWgt->GetContainerPtr();
	return pWnd;
}

/*!
\brief 取指针指定的部件的直接桌面句柄。
*/
YDesktop*
FetchWidgetDirectDesktopPtr(IWidget*);

/*!
\brief 取指定部件的窗口指针。
*/
template<class _tWidget>
IWindow*
FetchWindowPtr(const _tWidget& w)
{
	return FetchWidgetDirectWindowPtr(w.GetContainerPtr());
}
/*!
\brief 取指定部件的窗口指针。
*/
inline IWindow*
FetchWindowPtr(const IWidget& w)
{
	return FetchWindowPtr<IWidget>(w);
}

/*!
\brief 取指定部件的直接容器指针。
*/
template<class _tWidget>
IUIBox*
FetchDirectContainerPtr(_tWidget& w)
{
	IUIBox* const pCon(dynamic_cast<IUIBox*>(&w));

	return pCon ? pCon : w.GetContainerPtr();
}
/*!
\brief 取指定部件的直接容器指针。
*/
inline IUIBox*
FetchDirectContainerPtr(IWidget& w)
{
	return FetchDirectContainerPtr<IWidget>(w);
}

/*!
\brief 取指定部件的直接窗口句柄。
\note 加入容器指针判断。
*/
template<class _tWidget>
IWindow*
FetchDirectWindowPtr(_tWidget& w)
{
	return FetchWidgetDirectWindowPtr(FetchDirectContainerPtr(w));
}
/*!
\brief 取指定部件的直接窗口句柄。
\note 加入容器指针判断。
*/
inline IWindow*
FetchDirectWindowPtr(IWidget& w)
{
	return FetchDirectWindowPtr<IWidget>(w);
}

/*!
\brief 取指定部件的直接桌面句柄。
\note 加入容器指针判断。
*/
inline YDesktop*
FetchDirectDesktopPtr(IWidget& w)
{
	return FetchWidgetDirectDesktopPtr(FetchDirectContainerPtr(w));
}


/*!
\brief 取指定的点（相对此部件的坐标）相对于指定指针指向的父窗口的偏移坐标。
*/
Point
LocateOffset(const IWidget*, Point, const IWindow*);
/*!
\brief 取指定的点（相对此部件的坐标）相对于指定指针指向的父窗口的偏移坐标。
*/
template<class _tWidget>
Point
LocateOffset(const _tWidget* pCon, const Point& p, const IWindow* pWnd)
{
	const IWidget* pConI(dynamic_cast<const IWidget*>(pCon));

	return pConI ? LocateOffset(pConI, p, pWnd) : p;
}

/*!
\brief 取指定的点 p （相对部件 w 的坐标）相对于 w 的容器的偏移坐标。
*/
inline Point
LocateContainerOffset(const IWidget& w, const Point& p)
{
	return p + w.GetLocation();
}

/*!
\brief 取指定的点 p （相对部件 w 的坐标）相对于 w 的窗口的偏移坐标。
*/
inline Point
LocateWindowOffset(const IWidget& w, const Point& p)
{
	return LocateOffset(&w, p, GetPointer(FetchWindowPtr(w)));
}

/*!
\brief 取指定部件 a 相对于部件 b 的偏移坐标。
*/
Point
LocateForWidget(IWidget& a, IWidget& b);

/*!
\brief 取指定部件相对于直接窗口的偏移坐标。
\note 若自身是窗口则返回 Zero ；若无容器或窗口则返回 FullScreen 。
*/
Point
LocateForWindow(IWidget&);

/*!
\brief 取指定部件相对于桌面的偏移坐标。
\note 若自身是桌面则返回 Zero ；若无窗口或窗口不在桌面上则返回 FullScreen 。
*/
Point
LocateForDesktop(IWidget&);

/*!
\brief 取指定部件相对于容器的父容器的偏移坐标。
\note 若无容器则返回 FullScreen 。
*/
Point
LocateForParentContainer(const IWidget&);

/*!
\brief 取指定部件相对于窗口的父窗口的偏移坐标。
\note 若无容器则返回 FullScreen 。
*/
Point
LocateForParentWindow(const IWidget&);


/*!
\brief 移动部件至容器左端。
\pre 断言：w.GetContainerPtr()。
*/
void
MoveToLeft(IWidget&);

/*!
\brief 移动部件至容器右端。
\pre 断言：w.GetContainerPtr()。
*/
void
MoveToRight(IWidget&);

/*!
\brief 移动部件至容器上端。
\pre 断言：w.GetContainerPtr()。
*/
void
MoveToTop(IWidget&);

/*!
\brief 移动部件至容器下端。
\pre 断言：w.GetContainerPtr()。
*/
void
MoveToBottom(IWidget&);


/*!
\brief 以纯色填充部件所在窗口的对应显示缓冲区。
*/
void
Fill(IWidget&, Color);
/*!
\brief 以纯色填充部件所在窗口的对应显示缓冲区。
*/
void
Fill(Widget&, Color);


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


//! \brief 可视样式模块。
class Visual : public NonCopyable
{
private:
	bool visible; //!< 可见性。
	bool transparent; //!< 透明性。
	mutable bool background_redrawed; //!< 背景重绘状态。
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
		Color = ColorSpace::White, Color = ColorSpace::Black);
	virtual DefEmptyDtor(Visual)

	DefPredicate(Visible, visible)
	DefPredicate(Transparent, transparent)
	DefPredicate(BgRedrawed, background_redrawed)

	DefGetter(SPOS, X, GetLocation().X)
	DefGetter(SPOS, Y, GetLocation().Y)
	DefGetter(SDST, Width, GetSize().Width)
	DefGetter(SDST, Height, GetSize().Height)
	DefGetter(const Point&, Location, location)
	DefGetter(const Size&, Size, size)

	DefSetter(bool, Visible, visible)
	DefSetter(bool, Transparent, transparent)
	DefSetter(bool, BgRedrawed, background_redrawed)
	/*!
	\brief 设置位置。
	\note 虚公有实现。
	*/
	virtual DefSetter(const Point&, Location, location)
	/*!
	\brief 设置位置。
	\note 非虚公有实现。
	*/
	PDefH(void, SetLocation, SPOS x, SPOS y)
		ImplBodyBaseVoid(Visual, SetLocation, Point(x, y))
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
	PDefH(void, SetSize, SDST w, SDST h)
		ImplBodyBaseVoid(Visual, SetSize, Size(w, h))
};


//! \brief 部件模块。
class Widget : public Visual
{
private:
	IUIBox* pContainer; //!< 从属的部件容器的指针。

public:
	explicit
	Widget(const Rect& = Rect::Empty, IUIBox* = NULL,
		Color = ColorSpace::White, Color = ColorSpace::Black);
	virtual DefEmptyDtor(Widget)

	/*!
	\brief 判断是否属于指定部件容器指针指定的部件容器。
	*/
	bool
	BelongsTo(IUIBox*) const;

	DefGetter(IUIBox*, ContainerPtr, pContainer)

	/*!
	\brief 绘制背景。
	*/
	virtual void
	DrawBackground();

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();

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
	\brief 构造：使用指定边界和部件容器指针。
	*/
	explicit
	YWidget(const Rect& = Rect::Empty, IUIBox* = NULL);
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual
	~YWidget() ythrow();

	ImplI(IWidget) DefPredicateBase(Visible, Visual)
	ImplI(IWidget) DefPredicateBase(Transparent, Visual)
	ImplI(IWidget) DefPredicateBase(BgRedrawed, Visual)

	ImplI(IWidget) DefGetterBase(const Point&, Location, Visual)
	ImplI(IWidget) DefGetterBase(const Size&, Size, Visual)
	ImplI(IWidget) DefGetterBase(IUIBox*, ContainerPtr, Widget)

	ImplI(IWidget) DefSetterBase(bool, Visible, Visual)
	ImplI(IWidget) DefSetterBase(bool, Transparent, Visual)
	ImplI(IWidget) DefSetterBase(bool, BgRedrawed, Visual)
	ImplI(IWidget) DefSetterBase(const Point&, Location, Visual)
	ImplI(IWidget) DefSetterBase(const Size&, Size, Visual)

	ImplI(IWidget) PDefH(void, DrawBackground)
		ImplBodyBaseVoid(Widget, DrawBackground)

	ImplI(IWidget) PDefH(void, DrawForeground)
		ImplBodyBaseVoid(Widget, DrawForeground)

	ImplI(IWidget) PDefH(void, Refresh)
		ImplBodyBaseVoid(Widget, Refresh)

	/*!
	\brief 请求提升至容器顶端。
	\note 空实现。
	*/
	ImplI(IWidget) void
	RequestToTop()
	{}
};


//! \brief 部件容器模块。
class MUIContainer : protected GMFocusResponser<IVisualControl>
{
public:
	typedef GContainer<IWidget> WidgetSet;
	typedef WidgetSet::ContainerType WGTs; //!< 部件组类型。
	typedef GContainer<GMFocusResponser<IVisualControl> > FOCs; \
		//!< 子焦点对象容器组类型。

protected:
	WidgetSet sWgtSet; //!< 部件对象组模块。
	FOCs sFOCSet; //!< 子焦点对象容器组。

public:
	MUIContainer();
	virtual DefEmptyDtor(MUIContainer)

protected:
	virtual PDefHOperator(void, +=,
		IVisualControl& r) //!< 向焦点对象组添加焦点对象。
		ImplBodyBaseVoid(GMFocusResponser<IVisualControl>, operator+=, r)
	virtual PDefHOperator(bool, -=,
		IVisualControl& r) //!< 从焦点对象组移除焦点对象。
		ImplBodyBase(GMFocusResponser<IVisualControl>, operator-=, r)
	PDefHOperator(void, +=, GMFocusResponser<IVisualControl>& c) \
		//!< 向子焦点对象容器组添加子焦点对象容器。
		ImplBodyMemberVoid(sFOCSet, insert, &c)
	PDefHOperator(bool, -=, GMFocusResponser<IVisualControl>& c) \
		//!< 从子焦点对象容器组移除子焦点对象容器。
		ImplBodyMember(sFOCSet, erase, &c)

public:
	/*!
	\brief 取焦点控件指针。
	*/
	IVisualControl*
	GetFocusingPtr() const;
	/*!
	\brief 取顶端部件指针。
	*/
	IWidget*
	GetTopWidgetPtr(const Point&);
	/*!
	\brief 取顶端可视部件指针。
	*/
	IVisualControl*
	GetTopVisualControlPtr(const Point&);

	/*!
	\brief 响应焦点请求。
	*/
	bool
	ResponseFocusRequest(AFocusRequester&);

	/*!
	\brief 响应焦点释放。
	*/
	bool
	ResponseFocusRelease(AFocusRequester&);
};


//部件容器。
class YUIContainer : public GMCounter<YUIContainer>, public YComponent,
	public Widget, protected MUIContainer,
	implements IUIContainer
{
public:
	typedef YComponent ParentType;

	/*!
	\brief 构造：使用指定边界和部件容器指针。
	*/
	explicit
	YUIContainer(const Rect& = Rect::Empty, IUIBox* = NULL);
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual
	~YUIContainer() ythrow();

	ImplI(IUIContainer) PDefHOperator(void, +=, IWidget& w)
		ImplExpr(sWgtSet += w)
	ImplI(IUIContainer) PDefHOperator(bool, -=, IWidget& w)
		ImplRet(sWgtSet -= w)
	ImplI(IUIContainer) PDefHOperator(void, +=, IVisualControl& c)
		ImplBodyBaseVoid(MUIContainer, operator+=, c)
	ImplI(IUIContainer) PDefHOperator(bool, -=, IVisualControl& c)
		ImplBodyBase(MUIContainer, operator-=, c)
	ImplI(IUIContainer) PDefHOperator(void, +=,
		GMFocusResponser<IVisualControl>& c)
		ImplBodyBaseVoid(MUIContainer, operator+=, c)
	ImplI(IUIContainer) PDefHOperator(bool, -=,
		GMFocusResponser<IVisualControl>& c)
		ImplBodyBase(MUIContainer, operator-=, c)

	ImplI(IUIContainer) DefPredicateBase(Visible, Visual)
	ImplI(IUIContainer) DefPredicateBase(Transparent, Visual)
	ImplI(IUIContainer) DefPredicateBase(BgRedrawed, Visual)

	ImplI(IUIContainer) DefGetterBase(const Point&, Location, Visual)
	ImplI(IUIContainer) DefGetterBase(const Size&, Size, Visual)
	ImplI(IUIContainer) DefGetterBase(IUIBox*, ContainerPtr,
		Widget)
	ImplI(IUIContainer) PDefH(IVisualControl*, GetFocusingPtr)
		ImplBodyBase(GMFocusResponser<IVisualControl>, GetFocusingPtr)
	ImplI(IUIContainer) PDefH(IWidget*, GetTopWidgetPtr, const Point& p)
		ImplBodyBase(MUIContainer, GetTopWidgetPtr, p)
	ImplI(IUIContainer) PDefH(IVisualControl*, GetTopVisualControlPtr,
		const Point& p)
		ImplBodyBase(MUIContainer, GetTopVisualControlPtr, p)

	ImplI(IUIContainer) DefSetterBase(bool, Visible, Visual)
	ImplI(IUIContainer) DefSetterBase(bool, Transparent, Visual)
	ImplI(IUIContainer) DefSetterBase(bool, BgRedrawed, Visual)
	ImplI(IUIContainer) DefSetterBase(const Point&, Location, Visual)
	ImplI(IUIContainer) DefSetterBase(const Size&, Size, Visual)

	ImplI(IUIContainer) PDefH(void, ClearFocusingPtr)
		ImplBodyBaseVoid(MUIContainer, ClearFocusingPtr)

	ImplI(IUIContainer) PDefH(bool, ResponseFocusRequest, AFocusRequester& w)
		ImplBodyBase(MUIContainer, ResponseFocusRequest, w)

	ImplI(IUIContainer) PDefH(bool, ResponseFocusRelease, AFocusRequester& w)
		ImplBodyBase(MUIContainer, ResponseFocusRelease, w)

	ImplI(IUIContainer) PDefH(void, DrawBackground)
		ImplBodyBaseVoid(Widget, DrawBackground)

	ImplI(IUIContainer) PDefH(void, DrawForeground)
		ImplBodyBaseVoid(Widget, DrawForeground)

	ImplI(IUIContainer) PDefH(void, Refresh)
		ImplBodyBaseVoid(Widget, Refresh)

	/*!
	\brief 请求提升至容器顶端。
	\note 空实现。
	*/
	ImplI(IUIContainer) void
	RequestToTop()
	{}
};


//标签模块。
class MLabel : public NonCopyable
{
protected:
	GHWeak<Drawing::TextRegion> wpTextRegion; //!< 文本区域指针。

public:
	Drawing::Font Font; //!< 字体。
	Drawing::Padding Margin; //!< 文本和容器的间距。
	bool AutoSize; //!< 启用根据字号自动调整大小。
	bool AutoEllipsis; //!< 启用对超出标签宽度的文本调整大小。
	String Text; //!< 标签文本。

protected:
	/*!
	\brief 构造：使用指定字体和文本区域指针。
	*/
	explicit
	MLabel(const Drawing::Font& = Drawing::Font::GetDefault(),
		GHWeak<Drawing::TextRegion> = NULL);

	/*!
	\brief 绘制文本。
	*/
	void
	PaintText(Widget&, const Point&);
};


//标签。
class YLabel : public GMCounter<YLabel>, public YWidget,
	public Widgets::MLabel
{
public:
	typedef YWidget ParentType;

/*
	YImage BackgroundImage; //!< 背景图像。
	YImage Image; //!< 前景图像。
*/

	//用字符串在窗口中以指定字号初始化标签。
	/*!
	\brief 构造：使用指定边界、部件容器指针、字体和文本区域。
	*/
	explicit
	YLabel(const Rect& = Rect::FullScreen, IUIBox* = NULL,
		const Drawing::Font& = Drawing::Font::GetDefault(),
		GHWeak<Drawing::TextRegion> = NULL);

	virtual DefEmptyDtor(YLabel)

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();
};

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

