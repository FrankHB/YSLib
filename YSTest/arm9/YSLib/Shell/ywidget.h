/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywidget.h
\ingroup Shell
\brief 平台无关的图形用户界面部件实现。
\version 0.5336;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 + 08:00;
\par 修改时间:
	2010-11-12 18:30 + 08:00;
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
//#include <set>
#include "ytext.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//前向声明。

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

// GUI 断言。

#undef YWidgetAssert

#undef YWindowAssert

#ifdef YC_USE_YASSERT

/*!
\brief 断言：判断所给表达式，如果为假给出指定错误信息。
*/
inline void
yassert(bool exp, const char* msg, int line, const char* file,
	const char* comp, const char* func)
{
	if(!exp)
	{
		YConsole dbg;

		iprintf(
			"At line %i in file %s: \n"
			"An error occured in precedure %s of \n"
			"Components::%s:\n"
			"%s", line, file, func, comp, msg);
		dbg.Pause();
	}
}

#	define YWidgetAssert(ptr, comp, func) \
	Components::Widgets::yassert(ptr->GetWindowHandle(), \
		"The window handle is null.", __LINE__, __FILE__, #comp, #func)

#	define YWindowAssert(ptr, comp, func) \
	Components::Widgets::yassert( \
	static_cast<Drawing::Graphics>(*ptr).IsValid(), \
	"The graphics context is invalid.", __LINE__, __FILE__, #comp, #func)

#else

#	define YWidgetAssert(ptr, comp, func) \
	assert((ptr)->GetWindowHandle())

#	define YWindowAssert(ptr, comp, func) \
	assert(static_cast<Drawing::Graphics>(*ptr).IsValid())

#endif


// GUI 特性定义。

//方向。
typedef enum
{
	Horizontal = 0,
	Vertical = 1
} Orientation;


// GUI 接口和部件定义。

//部件接口。
DeclInterface(IWidget)
	DeclIEntry(bool IsVisible() const) //!< 判断是否可见。
	DeclIEntry(bool IsTransparent() const) //!< 判断是否透明。
	DeclIEntry(bool IsBgRedrawed() const) //!< 判断是否需要重绘。

	DeclIEntry(const Point& GetLocation() const)
	DeclIEntry(const Size& GetSize() const)
	DeclIEntry(IUIBox* GetContainerPtr() const)
	DeclIEntry(HWND GetWindowHandle() const)

	DeclIEntry(void SetVisible(bool)) //!< 设置可见。
	DeclIEntry(void SetTransparent(bool)) //!< 设置透明。
	DeclIEntry(void SetBgRedrawed(bool)) //!< 设置重绘状态。
	DeclIEntry(void SetLocation(const Point&)) \
		//!< 设置左上角所在位置（相对于容器的偏移坐标）。

	/*
	\breif 绘制背景。
	\warning 可能不检查缓冲区指针是否为空。
	*/
	DeclIEntry(void DrawBackground())

	/*
	\breif 绘制前景。
	\warning 可能不检查缓冲区指针是否为空。
	*/
	DeclIEntry(void DrawForeground())

	//! \brief 刷新至窗口缓冲区。
	DeclIEntry(void Refresh())

	//! \brief 请求提升至容器顶端。
	DeclIEntry(void RequestToTop())
EndDecl


//固定部件容器接口
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
\brief 判断点是否在可视区域内。
\note 
*/
bool
Contains(const IWidget& w, SPOS x, SPOS y);
/*!
\brief 判断点是否在可视区域内。
\note 
*/
inline bool
Contains(const IWidget& w, const Point& p)
{
	return Contains(w, p.X, p.Y);
}


/*!
\brief 取指定的点（相对此部件的坐标）相对于指定指针指向的父窗口的偏移坐标。
\note 
*/
Point
LocateOffset(const IWidget*, Point, IWindow*);

/*!
\brief 取指定的点 p （相对部件 w 的坐标）相对于 w 的容器的偏移坐标。
\note 
*/
inline Point
LocateContainerOffset(const IWidget& w, const Point& p)
{
	return p + w.GetLocation();
}

/*!
\brief 取指定的点 p （相对部件 w 的坐标）相对于 w 的窗口的偏移坐标。
\note 
*/
inline Point
LocateWindowOffset(const IWidget& w, const Point& p)
{
	return LocateOffset(&w, p, w.GetWindowHandle());
}

/*!
\brief 取指定部件 a 相对于部件 b 的偏移坐标。
\note 
*/
Point
LocateForWidget(IWidget& a, IWidget& b);

/*!
\brief 取指定部件相对于最直接的窗口的偏移坐标。
\note 若无窗口则返回 FullScreen 。
*/
Point
LocateForWindow(const IWidget&);

/*!
\brief 取指定部件相对于桌面的偏移坐标。
\note 若无窗口或窗口不在桌面上则返回 FullScreen 。
*/
Point
LocateForDesktop(const IWidget&);

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


//方向模块。
class MOriented
{
protected:
	Widgets::Orientation Orientation;

	explicit
	MOriented(Widgets::Orientation);

	DefGetter(Widgets::Orientation, Orientation, Orientation)
};

inline
MOriented::MOriented(Widgets::Orientation o)
	: Orientation(o)
{}


//可视样式模块。
class Visual
{
protected:
	bool Visible; //!< 可见性。
	bool Transparent; //!< 透明性。

private:
	mutable bool bBgRedrawed; //!< 背景重绘状态。

protected:
	Point Location; //!< 左上角所在位置（相对于容器的偏移坐标）。

private:
	Drawing::Size Size; //!< 部件大小。

public:
	Color BackColor; //!< 默认背景色。
	Color ForeColor; //!< 默认前景色。

	/*!
		Visual
	\brief 构造：使用指定边界、前景色和背景色。
	*/
	explicit
	Visual(const Rect& = Rect::Empty,
		Color = ColorSpace::White, Color = ColorSpace::Black);
	virtual DefEmptyDtor(Visual)

private:
	/*!
	\brief 设置大小。
	\note 非虚私有实现。
	*/
	void
	_m_SetSize(SDST, SDST);

public:
	DefPredicate(Visible, Visible)
	DefPredicate(Transparent, Transparent)
	DefPredicate(BgRedrawed, bBgRedrawed)

	DefGetter(SPOS, X, GetLocation().X)
	DefGetter(SPOS, Y, GetLocation().Y)
	DefGetter(SDST, Width, GetSize().Width)
	DefGetter(SDST, Height, GetSize().Height)
	DefGetter(const Point&, Location, Location)
	DefGetter(const Drawing::Size&, Size, Size)
	DefGetter(Rect, Bounds, Rect(GetLocation(), GetSize()))

	DefSetter(bool, Visible, Visible)
	DefSetter(bool, Transparent, Transparent)
	DefSetter(bool, BgRedrawed, bBgRedrawed)
	virtual DefSetter(const Point&, Location, Location)
	PDefH(void, SetLocation, SPOS x, SPOS y)
		ImplBodyBaseVoid(Visual, SetLocation, Point(x, y))
	virtual PDefH(void, SetSize, const Drawing::Size& s)
		ImplExpr(_m_SetSize(s.Width, s.Height))
	PDefH(void, SetSize, SDST w, SDST h)
		ImplExpr(_m_SetSize(w, h))
	/*!
	\brief 设置边界。
	\note 虚公有实现。
	*/
	virtual void
	SetBounds(const Rect& r);
};


//部件模块。
class Widget : public Visual
{
private:
	HWND hWindow; //!< 从属的窗口的句柄。
	IUIBox* pContainer; //!< 从属的部件容器的指针。

public:
	explicit
	Widget(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL,
		Color = ColorSpace::White, Color = ColorSpace::Black);
	virtual DefEmptyDtor(Widget)

	//判断从属关系。
	/*!
	\brief 判断是否属于窗口句柄指定的窗口。
	*/
	bool
	BelongsTo(HWND) const;
	/*!
	\brief 判断是否属于指定部件容器指针指定的部件容器。
	*/
	bool
	BelongsTo(IUIBox*) const;

	DefGetter(IUIBox*, ContainerPtr, pContainer)
	DefGetter(HWND, WindowHandle, hWindow)

	/*!
	\brief 以背景色填充显示缓冲区。
	*/
	virtual void
	BeFilledWith();
	/*!
	\brief 以纯色填充显示缓冲区。
	*/
	virtual void
	BeFilledWith(Color);

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
Widget::BelongsTo(HWND hWnd) const
{
	return hWindow == hWnd;
}
inline bool
Widget::BelongsTo(IUIBox* pCon) const
{
	return pContainer == pCon;
}

inline void
Widget::BeFilledWith()
{
	BeFilledWith(BackColor);
}


//! \brief 部件。
class YWidget : public GMCounter<YWidget>, public YComponent, public Widget,
	implements IWidget
{
public:
	typedef YComponent ParentType;

	/*!
	\brief 构造：使用指定窗口句柄、边界和部件容器指针。
	*/
	explicit
	YWidget(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL);
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
	ImplI(IWidget) DefGetterBase(const Drawing::Size&, Size, Visual)
	ImplI(IWidget) DefGetterBase(IUIBox*, ContainerPtr, Widget)
	ImplI(IWidget) DefGetterBase(HWND, WindowHandle, Widget)

	ImplI(IWidget) DefSetterBase(bool, Visible, Visual)
	ImplI(IWidget) DefSetterBase(bool, Transparent, Visual)
	ImplI(IWidget) DefSetterBase(bool, BgRedrawed, Visual)
	ImplI(IWidget) DefSetterBase(const Point&, Location, Visual)

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


//部件容器模块。
class MUIContainer : public GMFocusResponser<IVisualControl>,
	implements GIContainer<IVisualControl>
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
	/*!
	\brief 无参数构造。
	*/
	MUIContainer();
	virtual DefEmptyDtor(MUIContainer)

protected:
	ImplI(GIContainer<IVisualControl>) PDefHOperator(void, +=,
		IVisualControl& r) //!< 向焦点对象组添加焦点对象。
		ImplBodyBaseVoid(GMFocusResponser<IVisualControl>, operator+=, r)
	ImplI(GIContainer<IVisualControl>) PDefHOperator(bool, -=,
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
	\brief 构造：使用指定窗口句柄、边界和部件容器指针。
	*/
	explicit
	YUIContainer(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL);
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
	ImplI(IUIContainer) DefGetterBase(const Drawing::Size&, Size, Visual)
	ImplI(IUIContainer) DefGetterBase(IUIBox*, ContainerPtr,
		Widget)
	ImplI(IUIContainer) DefGetterBase(HWND, WindowHandle, Widget)
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
class MLabel
{
protected:
	GHResource<Drawing::TextRegion> prTextRegion; //!< 文本区域指针。

public:
	Drawing::Font Font; //!< 字体。
	Drawing::Padding Margin; //!< 文本和容器的间距。
	bool AutoSize; //!< 启用根据字号自动调整大小。
	bool AutoEllipsis; //!< 启用对超出标签宽度的文本调整大小。
	String Text; //!< 标签文本。

	/*!
	\brief 构造：使用指定字体和文本区域指针。
	*/
	explicit
	MLabel(const Drawing::Font& = Drawing::Font::GetDefault(),
		GHResource<Drawing::TextRegion> = NULL);

protected:
	DefEmptyDtor(MLabel)

	/*!
	\brief 绘制文本。
	*/
	void
	PaintText(Widget&, const Point&);
};


//标签。
class YLabel : public GMCounter<YLabel>, public YWidget, public Widgets::MLabel
{
public:
	typedef YWidget ParentType;

/*
	YImage BackgroundImage; //!< 背景图像。
	YImage Image; //!< 前景图像。
*/

	//用字符串在窗口中以指定字号初始化标签。
	/*!
	\brief 构造：使用指定窗口句柄、边界、字体、部件容器指针和文本区域。
	*/
	explicit
	YLabel(HWND = NULL, const Rect& = Rect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(),
		IUIBox* = NULL, GHResource<Drawing::TextRegion> = NULL);

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

