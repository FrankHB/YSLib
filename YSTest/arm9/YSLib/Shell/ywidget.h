// YSLib::Shell::YWidget by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-11-07 18:49 + 08:00;
// Version = 0.5301;


#ifndef INCLUDED_YWIDGET_H_
#define INCLUDED_YWIDGET_H_

// YWidget ：平台无关的图形用户界面部件实现。

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

// GUI 部件定义。

//部件接口。
DeclInterface(IWidget)
	DeclIEntry(bool IsVisible() const) //判断是否可见。
	DeclIEntry(bool IsTransparent() const) //判断是否透明。
	DeclIEntry(bool IsBgRedrawed() const) //判断是否需要重绘。

	DeclIEntry(const Point& GetLocation() const)
	DeclIEntry(const Size& GetSize() const)
	DeclIEntry(IUIBox* GetContainerPtr() const)
	DeclIEntry(HWND GetWindowHandle() const)

	DeclIEntry(void SetVisible(bool)) //设置可见。
	DeclIEntry(void SetTransparent(bool)) //设置透明。
	DeclIEntry(void SetBgRedrawed(bool)) //设置重绘状态。
	DeclIEntry(void SetLocation(const Point&)) \
		//设置左上角所在位置（相对于容器的偏移坐标）。

	/*
	注意： void DrawBackground() 和 void DrawForeground()
	可能不检查缓冲区指针是否为空。
	*/

	//绘制背景。
	DeclIEntry(void DrawBackground())

	//绘制前景。
	DeclIEntry(void DrawForeground())

	//刷新至窗口缓冲区。
	DeclIEntry(void Refresh())

	//请求提升至容器顶端。
	DeclIEntry(void RequestToTop())
EndDecl


//固定部件容器接口
DeclBasedInterface(IUIBox, virtual IWidget)
	DeclIEntry(IVisualControl* GetFocusingPtr()) //取焦点对象指针。
	DeclIEntry(IWidget* GetTopWidgetPtr(const Point&)) \
		//取指定的点（屏幕坐标）所处的部件的指针。
	DeclIEntry(IVisualControl* GetTopVisualControlPtr(const Point&)) \
		//取指定的点（屏幕坐标）所处的可视控件的指针。

	//清除焦点指针。
	DeclIEntry(void ClearFocusingPtr())

	//响应焦点请求。
	DeclIEntry(bool ResponseFocusRequest(AFocusRequester&))

	//响应焦点释放。
	DeclIEntry(bool ResponseFocusRelease(AFocusRequester&))
EndDecl


//部件容器接口。
DeclBasedInterface(IUIContainer, virtual IUIBox)
	DeclIEntry(void operator+=(IWidget&)) //向部件组添加部件。
	DeclIEntry(bool operator-=(IWidget&)) //从部件组移除部件。
	DeclIEntry(void operator+=(IVisualControl&)) //向焦点对象组添加可视控件。
	DeclIEntry(bool operator-=(IVisualControl&)) //从焦点对象组移除可视控件。
	DeclIEntry(void operator+=(GMFocusResponser<IVisualControl>&)) \
		//向焦点对象组添加子焦点对象容器。
	DeclIEntry(bool operator-=(GMFocusResponser<IVisualControl>&)) \
		//从焦点对象组移除子焦点对象容器。
EndDecl


#undef YWidgetAssert

#undef YWindowAssert

#ifdef YC_USE_YASSERT

//********************************
//名称:		yassert
//全名:		platform::yassert
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	bool exp
//形式参数:	const char * msg
//形式参数:	int line
//形式参数:	const char * file
//形式参数:	const char * comp
//形式参数:	const char * func
//功能概要:	断言：判断所给表达式，如果为假给出指定错误信息。
//备注:		
//********************************
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


//********************************
//名称:		Contains
//全名:		YSLib::Components::Widgets::Contains
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const IWidget & w
//形式参数:	SPOS
//形式参数:	SPOS
//功能概要:	判断点是否在可视区域内。
//备注:		
//********************************
bool
Contains(const IWidget& w, SPOS x, SPOS y);
//********************************
//名称:		Contains
//全名:		YSLib::Components::Widgets::Contains
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const IWidget & w
//形式参数:	const Point & p
//功能概要:	判断点是否在可视区域内。
//备注:		
//********************************
inline bool
Contains(const IWidget& w, const Point& p)
{
	return Contains(w, p.X, p.Y);
}


//********************************
//名称:		LocateOffset
//全名:		YSLib::Components::Widgets::LocateOffset
//可访问性:	public 
//返回类型:	YSLib::Components::Point
//修饰符:	
//形式参数:	const IWidget *
//形式参数:	Point
//形式参数:	IWindow *
//功能概要:	取指定的点（相对此部件的坐标）相对于指定指针指向的父窗口的偏移坐标。
//备注:		
//********************************
Point
LocateOffset(const IWidget*, Point, IWindow*);

//********************************
//名称:		LocateContainerOffset
//全名:		YSLib::Components::Widgets::LocateContainerOffset
//可访问性:	public 
//返回类型:	YSLib::Components::Point
//修饰符:	
//形式参数:	const IWidget & w
//形式参数:	const Point & p
//功能概要:	取指定的点 p （相对部件 w 的坐标）相对于 w 的容器的偏移坐标。
//备注:		
//********************************
inline Point
LocateContainerOffset(const IWidget& w, const Point& p)
{
	return p + w.GetLocation();
}

//********************************
//名称:		LocateWindowOffset
//全名:		YSLib::Components::Widgets::LocateWindowOffset
//可访问性:	public 
//返回类型:	YSLib::Components::Point
//修饰符:	
//形式参数:	const IWidget & w
//形式参数:	const Point & p
//功能概要:	取指定的点 p （相对部件 w 的坐标）相对于 w 的窗口的偏移坐标。
//备注:		
//********************************
inline Point
LocateWindowOffset(const IWidget& w, const Point& p)
{
	return LocateOffset(&w, p, w.GetWindowHandle());
}

//********************************
//名称:		LocateForWidget
//全名:		YSLib::Components::Widgets::LocateForWidget
//可访问性:	public 
//返回类型:	YSLib::Components::Point
//修饰符:	
//形式参数:	const IWidget & a
//形式参数:	const IWidget & b
//功能概要:	取指定部件 a 相对于部件 b 的偏移坐标。
//备注:		
//********************************
Point
LocateForWidget(IWidget& a, IWidget& b);

//********************************
//名称:		LocateForWindow
//全名:		YSLib::Components::Widgets::LocateForWindow
//可访问性:	public 
//返回类型:	YSLib::Components::Point
//修饰符:	
//形式参数:	const IWidget &
//功能概要:	取指定部件相对于最直接的窗口的偏移坐标。
//备注:		若无窗口则返回 FullScreen 。
//********************************
Point
LocateForWindow(const IWidget&);

//********************************
//名称:		LocateForDesktop
//全名:		YSLib::Components::Widgets::LocateForDesktop
//可访问性:	public 
//返回类型:	YSLib::Components::Point
//修饰符:	
//形式参数:	const IWidget &
//功能概要:	取指定部件相对于桌面的偏移坐标。
//备注:		若无窗口或窗口不在桌面上则返回 FullScreen 。
//********************************
Point
LocateForDesktop(const IWidget&);

//********************************
//名称:		LocateForParentContainer
//全名:		YSLib::Components::Widgets::LocateForParentContainer
//可访问性:	public 
//返回类型:	YSLib::Components::Point
//修饰符:	
//形式参数:	const IWidget &
//功能概要:	取指定部件相对于容器的父容器的偏移坐标。
//备注:		若无容器则返回 FullScreen 。
//********************************
Point
LocateForParentContainer(const IWidget&);

//********************************
//名称:		LocateForParentWindow
//全名:		YSLib::Components::Widgets::LocateForParentWindow
//可访问性:	public 
//返回类型:	YSLib::Components::Point
//修饰符:	
//形式参数:	const IWidget &
//功能概要:	取指定部件相对于窗口的父窗口的偏移坐标。
//备注:		若无容器则返回 FullScreen 。
//********************************
Point
LocateForParentWindow(const IWidget&);


//可视样式模块。
class Visual
{
protected:
	bool Visible; //可见性。
	bool Transparent; //透明性。

private:
	mutable bool bBgRedrawed; //背景重绘状态。

protected:
	Point Location; //左上角所在位置（相对于容器的偏移坐标）。

private:
	Drawing::Size Size; //部件大小。

public:
	Color BackColor; //默认背景色。
	Color ForeColor; //默认前景色。

	//********************************
	//名称:		Visual
	//全名:		YSLib::Components::Widgets::Visual::Visual
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Rect &
	//形式参数:	Color
	//形式参数:	Color
	//功能概要:	构造：使用指定边界、前景色和背景色。
	//备注:		
	//********************************
	explicit
	Visual(const Rect& = Rect::Empty,
		Color = ColorSpace::White, Color = ColorSpace::Black);
	virtual DefEmptyDtor(Visual)

private:
	//********************************
	//名称:		_m_SetSize
	//全名:		YSLib::Components::Widgets::Visual::_m_SetSize
	//可访问性:	private 
	//返回类型:	void
	//修饰符:	
	//形式参数:	SDST
	//形式参数:	SDST
	//功能概要:	设置大小。
	//备注:		非虚私有实现。
	//********************************
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
	//********************************
	//名称:		SetBounds
	//全名:		YSLib::Components::Widgets::Visual::SetBounds
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Rect & r
	//功能概要:	设置边界。
	//备注:		虚公有实现。
	//********************************
	virtual void
	SetBounds(const Rect& r);
};


//部件模块。
class Widget : public Visual
{
private:
	HWND hWindow; //从属的窗口的句柄。
	IUIBox* pContainer; //从属的部件容器的指针。

public:
	explicit
	Widget(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL,
		Color = ColorSpace::White, Color = ColorSpace::Black);
	virtual DefEmptyDtor(Widget)

	//判断从属关系。
	//********************************
	//名称:		BelongsTo
	//全名:		YSLib::Components::Widgets::Widget::BelongsTo
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	HWND
	//功能概要:	判断是否属于窗口句柄指定的窗口。
	//备注:		
	//********************************
	bool
	BelongsTo(HWND) const;
	//********************************
	//名称:		BelongsTo
	//全名:		YSLib::Components::Widgets::Widget::BelongsTo
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	IUIBox *
	//功能概要:	判断是否属于指定部件容器指针指定的部件容器。
	//备注:		
	//********************************
	bool
	BelongsTo(IUIBox*) const;

	DefGetter(IUIBox*, ContainerPtr, pContainer)
	DefGetter(HWND, WindowHandle, hWindow)

	//********************************
	//名称:		BeFilledWith
	//全名:		YSLib::Components::Widgets::Widget::BeFilledWith
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	以背景色填充显示缓冲区。
	//备注:		
	//********************************
	virtual void
	BeFilledWith();
	//********************************
	//名称:		BeFilledWith
	//全名:		YSLib::Components::Widgets::Widget::BeFilledWith
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	Color
	//功能概要:	以纯色填充显示缓冲区。
	//备注:		
	//********************************
	virtual void
	BeFilledWith(Color);

	//********************************
	//名称:		DrawBackground
	//全名:		YSLib::Components::Widgets::Widget::DrawBackground
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制背景。
	//备注:		
	//********************************
	virtual void
	DrawBackground();

	//********************************
	//名称:		DrawForeground
	//全名:		YSLib::Components::Widgets::Widget::DrawForeground
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制前景。
	//备注:		
	//********************************
	virtual void
	DrawForeground();

	//********************************
	//名称:		Refresh
	//全名:		YSLib::Components::Widgets::Widget::Refresh
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	刷新至窗口缓冲区。
	//备注:		
	//********************************
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


//部件。
class YWidget : public GMCounter<YWidget>, public YComponent, public Widget,
	implements IWidget
{
public:
	typedef YComponent ParentType;

	//********************************
	//名称:		YWidget
	//全名:		YSLib::Components::Widgets::YWidget::YWidget
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	HWND
	//形式参数:	const Rect &
	//形式参数:	IUIBox *
	//功能概要:	构造：使用指定窗口句柄、边界和部件容器指针。
	//备注:		
	//********************************
	explicit
	YWidget(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL);
	//********************************
	//名称:		~YWidget
	//全名:		YSLib::Components::Widgets::YWidget::~YWidget
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//********************************
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

	//********************************
	//名称:		RequestToTop
	//全名:		YSLib::Components::Widgets::YWidget::RequestToTop
	//可访问性:	ImplI(IWidget) public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	请求提升至容器顶端。
	//备注:		空实现。
	//********************************
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
	typedef WidgetSet::ContainerType WGTs; //部件组类型。
	typedef GContainer<GMFocusResponser<IVisualControl> > FOCs; \
		//子焦点对象容器组类型。

protected:
	WidgetSet sWgtSet; //部件对象组模块。
	FOCs sFOCSet; //子焦点对象容器组。

public:
	//********************************
	//名称:		MUIContainer
	//全名:		YSLib::Components::Widgets::MUIContainer::MUIContainer
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		
	//********************************
	MUIContainer();
	virtual DefEmptyDtor(MUIContainer)

protected:
	ImplI(GIContainer<IVisualControl>) PDefHOperator(void, +=,
		IVisualControl& r) //向焦点对象组添加焦点对象。
		ImplBodyBaseVoid(GMFocusResponser<IVisualControl>, operator+=, r)
	ImplI(GIContainer<IVisualControl>) PDefHOperator(bool, -=,
		IVisualControl& r) //从焦点对象组移除焦点对象。
		ImplBodyBase(GMFocusResponser<IVisualControl>, operator-=, r)
	PDefHOperator(void, +=, GMFocusResponser<IVisualControl>& c) \
		//向子焦点对象容器组添加子焦点对象容器。
		ImplBodyMemberVoid(sFOCSet, insert, &c)
	PDefHOperator(bool, -=, GMFocusResponser<IVisualControl>& c) \
		//从子焦点对象容器组移除子焦点对象容器。
		ImplBodyMember(sFOCSet, erase, &c)

public:
	//********************************
	//名称:		IVisualControl
	//全名:		YSLib::Components::Widgets::MUIContainer::GetFocusingPtr
	//可访问性:	public 
	//返回类型:	IVisualControl*
	//修饰符:	const
	//功能概要:	取焦点控件指针。
	//备注:		
	//********************************
	IVisualControl*
	GetFocusingPtr() const;
	//********************************
	//名称:		GetTopWidgetPtr
	//全名:		YSLib::Components::Widgets::MUIContainer::GetTopWidgetPtr
	//可访问性:	public 
	//返回类型:	IWidget*
	//修饰符:	
	//形式参数:	const Point &
	//功能概要:	取顶端部件指针。
	//备注:		
	//********************************
	IWidget*
	GetTopWidgetPtr(const Point&);
	//********************************
	//名称:		GetTopVisualControlPtr
	//全名:		YSLib::Components::Widgets::MUIContainer
	//				::GetTopVisualControlPtr
	//可访问性:	public 
	//返回类型:	IVisualControl*
	//修饰符:	
	//形式参数:	const Point &
	//功能概要:	取顶端可视部件指针。
	//备注:		
	//********************************
	IVisualControl*
	GetTopVisualControlPtr(const Point&);

	//********************************
	//名称:		ResponseFocusRequest
	//全名:		YSLib::Components::Widgets::MUIContainer
	//				::ResponseFocusRequest
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	AFocusRequester &
	//功能概要:	响应焦点请求。
	//备注:		
	//********************************
	bool
	ResponseFocusRequest(AFocusRequester&);

	//********************************
	//名称:		ResponseFocusRequest
	//全名:		YSLib::Components::Widgets::MUIContainer
	//				::ResponseFocusRelease
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	AFocusRequester &
	//功能概要:	响应焦点释放。
	//备注:		
	//********************************
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

	//********************************
	//名称:		YUIContainer
	//全名:		YSLib::Components::Widgets::YUIContainer::YUIContainer
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	HWND
	//形式参数:	const Rect &
	//形式参数:	IUIBox *
	//功能概要:	构造：使用指定窗口句柄、边界和部件容器指针。
	//备注:		
	//********************************
	explicit
	YUIContainer(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL);
	//********************************
	//名称:		~YUIContainer
	//全名:		YSLib::Components::Widgets::YUIContainer::~YUIContainer
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//********************************
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

	//********************************
	//名称:		RequestToTop
	//全名:		YSLib::Components::Widgets::YUIContainer::RequestToTop
	//可访问性:	ImplI(IUIContainer) public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	请求提升至容器顶端。
	//备注:		空实现。
	//********************************
	ImplI(IUIContainer) void
	RequestToTop()
	{}
};


//标签模块。
class MLabel
{
protected:
	GHResource<Drawing::TextRegion> prTextRegion; //文本区域指针。

public:
	Drawing::Font Font; //字体。
	Drawing::Padding Margin; //文本和容器的间距。
	bool AutoSize; //启用根据字号自动调整大小。
	bool AutoEllipsis; //启用对超出标签宽度的文本调整大小。
	String Text; //标签文本。

	//********************************
	//名称:		MLabel
	//全名:		YSLib::Components::Widgets::MLabel::MLabel<_tChar>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const _tChar *
	//形式参数:	const Drawing::Font &
	//形式参数:	GHResource<Drawing::TextRegion>
	//功能概要:	构造：使用指定字符串、字体和文本区域指针。
	//备注:		
	//********************************
	template<class _tChar>
	MLabel(const _tChar*, const Drawing::Font& = Drawing::Font::GetDefault(),
		GHResource<Drawing::TextRegion> = NULL);

protected:
	DefEmptyDtor(MLabel)

	//********************************
	//名称:		PaintText
	//全名:		YSLib::Components::Widgets::MLabel::PaintText
	//可访问性:	protected 
	//返回类型:	void
	//修饰符:	
	//形式参数:	Widget &
	//形式参数:	const Point &
	//功能概要:	绘制文本。
	//备注:		
	//********************************
	void
	PaintText(Widget&, const Point&);
};

template<class _tChar>
MLabel::MLabel(const _tChar* l, const Drawing::Font& f,
	GHResource<Drawing::TextRegion> prTr_)
	: prTextRegion(prTr_ ? prTr_ : GetGlobalResource<Drawing::TextRegion>()),
	Font(f),
	Margin(prTextRegion->Margin), AutoSize(true), AutoEllipsis(false), Text(l)
{}


//标签。
class YLabel : public GMCounter<YLabel>, public YWidget, public Widgets::MLabel
{
public:
	typedef YWidget ParentType;

/*
	YImage BackgroundImage; //背景图像。
	YImage Image; //前景图像。
*/

	//用字符串在窗口中以指定字号初始化标签。
	//********************************
	//名称:		YLabel
	//全名:		YSLib::Components::Widgets::YLabel<_tChar>::YLabel
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	HWND
	//形式参数:	const _tChar *
	//形式参数:	const Rect &
	//形式参数:	const Drawing::Font &
	//形式参数:	IUIBox *
	//形式参数:	GHResource<Drawing::TextRegion>
	//功能概要:	构造：
	//			使用指定窗口句柄、字符串、边界、字体、部件容器指针和文本区域。
	//备注:		
	//********************************
	template<class _tChar>
	YLabel(HWND, const _tChar*, const Rect& = Rect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(),
		IUIBox* = NULL, GHResource<Drawing::TextRegion> = NULL);

	virtual DefEmptyDtor(YLabel)

	//********************************
	//名称:		DrawForeground
	//全名:		YSLib::Components::Widgets::YLabel::DrawForeground
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制前景。
	//备注:		
	//********************************
	virtual void
	DrawForeground();
};

template<class _tChar>
YLabel::YLabel(HWND hWnd, const _tChar* l, const Rect& r,
	const Drawing::Font& f, IUIBox* pCon,
	GHResource<Drawing::TextRegion> prTr_)
	: YWidget(hWnd, r, pCon), MLabel(l, f, pCon
	? prTr_ : GetGlobalResource<Drawing::TextRegion>())
{}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

