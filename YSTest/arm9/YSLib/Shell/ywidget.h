// YSLib::Shell::YWidget by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-25 12:12 + 08:00;
// Version = 0.5060;


#ifndef INCLUDED_YWIDGET_H_
#define INCLUDED_YWIDGET_H_

// YWidget ：平台无关的图形用户界面部件实现。

#include "ycomp.h"
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

	DeclIEntry(bool Contains(const Point&) const) //判断点是否在边界内或边界上。

	DeclIEntry(const Point& GetLocation() const)
	DeclIEntry(const Size& GetSize() const)
	DeclIEntry(IWidgetContainer* GetContainerPtr() const)
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


//部件容器接口。
DeclBasedInterface(IWidgetContainer, virtual IWidget)
	DeclIEntry(void operator+=(IWidget&)) //向部件组添加部件。
	DeclIEntry(bool operator-=(IWidget&)) //从部件组移除部件。
	DeclIEntry(void operator+=(IVisualControl&)) //向焦点对象组添加可视控件。
	DeclIEntry(bool operator-=(IVisualControl&)) //从焦点对象组移除可视控件。
	DeclIEntry(void operator+=(GMFocusResponser<IVisualControl>&)) \
		//向焦点对象组添加子焦点对象容器。
	DeclIEntry(bool operator-=(GMFocusResponser<IVisualControl>&)) \
		//从焦点对象组移除子焦点对象容器。

	DeclIEntry(IWidget* GetTopWidgetPtr(const Point&) const) \
		//取指定的点（屏幕坐标）所处的部件的指针。
	DeclIEntry(IVisualControl* GetTopVisualControlPtr(const Point&) const) \
		//取指定的点（屏幕坐标）所处的焦点对象的指针。

	//清除焦点指针。
	DeclIEntry(void ClearFocusingPtr())
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
	Components::Widgets::yassert(ptr->GetWindowHandle() != NULL, \
		"The window handle is null.", __LINE__, __FILE__, #comp, #func)

#	define YWindowAssert(ptr, comp, func) \
	Components::Widgets::yassert( \
	static_cast<Drawing::Graphics>(*ptr).IsValid(), \
	"The graphics context is invalid.", __LINE__, __FILE__, #comp, #func)

#else

#	define YWidgetAssert(ptr, comp, func) \
	assert((ptr)->GetWindowHandle() != NULL)

#	define YWindowAssert(ptr, comp, func) \
	assert(static_cast<Drawing::Graphics>(*ptr).IsValid())

#endif


//********************************
//名称:		GetLocationOffset
//全名:		YSLib::Components::Widgets::GetLocationOffset
//可访问性:	public 
//返回类型:	YSLib::Components::Point
//修饰符:	
//形式参数:	const IWidget *
//形式参数:	const Point &
//形式参数:	const HWND &
//功能概要:	取指定的点（相对此部件的坐标）相对于指定父窗口的偏移坐标。
//备注:		
//********************************
Point
GetLocationOffset(const IWidget*, const Point&, const HWND&);

//********************************
//名称:		GetContainerLocationOffset
//全名:		YSLib::Components::Widgets::GetContainerLocationOffset
//可访问性:	public 
//返回类型:	YSLib::Components::Point
//修饰符:	
//形式参数:	const IWidget & w
//形式参数:	const Point & p
//功能概要:	取指定的点（相对此部件的坐标）相对于此部件的父容器的偏移坐标。
//备注:		
//********************************
inline Point
GetContainerLocationOffset(const IWidget& w, const Point& p)
{
	return p + w.GetLocation();
}

//********************************
//名称:		GetWindowLocationOffset
//全名:		YSLib::Components::Widgets::GetWindowLocationOffset
//可访问性:	public 
//返回类型:	YSLib::Components::Point
//修饰符:	
//形式参数:	const IWidget & w
//形式参数:	const Point & p
//功能概要:	取指定的点（相对此部件的坐标）相对于此部件的父窗口的偏移坐标。
//备注:		
//********************************
inline Point
GetWindowLocationOffset(const IWidget& w, const Point& p)
{
	return Widgets::GetLocationOffset(&w, p, w.GetWindowHandle());
}


//可视样式模块。
class MVisual
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
	//名称:		MVisual
	//全名:		YSLib::Components::Widgets::MVisual::MVisual
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
	MVisual(const Rect& = Rect::Empty,
		Color = ColorSpace::White, Color = ColorSpace::Black);
	virtual DefEmptyDtor(MVisual)

private:
	//********************************
	//名称:		_m_SetSize
	//全名:		YSLib::Components::Widgets::MVisual::_m_SetSize
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

	//判断包含关系。
	virtual PDefH(bool, Contains, const Point& p) const \
		//判断点 P 是否在边界内或边界上。
		ImplBodyMember(GetBounds(), IsInBoundsRegular, p.X, p.Y)
	PDefH(bool, Contains, SPOS x, SPOS y) const \
		//判断点 (x, y) 是否在边界内或边界上。
		ImplBodyBase(MVisual, Contains, Point(x, y))

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
		ImplBodyBaseVoid(MVisual, SetLocation, Point(x, y))
	virtual PDefH(void, SetSize, const Drawing::Size& s)
		ImplExpr(_m_SetSize(s.Width, s.Height))
	PDefH(void, SetSize, SDST w, SDST h)
		ImplExpr(_m_SetSize(w, h))
	//********************************
	//名称:		SetBounds
	//全名:		YSLib::Components::Widgets::MVisual::SetBounds
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
class MWidget : public MVisual
{
private:
	HWND hWindow; //从属的窗口的句柄。

public:
	IWidgetContainer* const pContainer; //从属的部件容器的指针。

	explicit
	MWidget(HWND = NULL, const Rect& = Rect::Empty, IWidgetContainer* = NULL,
		Color = ColorSpace::White, Color = ColorSpace::Black);
	virtual DefEmptyDtor(MWidget)

	//判断从属关系。
	//********************************
	//名称:		BelongsTo
	//全名:		YSLib::Components::Widgets::MWidget::BelongsTo
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
	//全名:		YSLib::Components::Widgets::MWidget::BelongsTo
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	IWidgetContainer *
	//功能概要:	判断是否属于指定部件容器指针指定的部件容器。
	//备注:		
	//********************************
	bool
	BelongsTo(IWidgetContainer*) const;

	DefGetter(IWidgetContainer*, ContainerPtr, pContainer)
	DefGetter(HWND, WindowHandle, hWindow)
	//********************************
	//名称:		GetLocationForWindow
	//全名:		YSLib::Components::Widgets::MWidget::GetLocationForWindow
	//可访问性:	public 
	//返回类型:	YSLib::Components::Point
	//修饰符:	const
	//功能概要:	取部件相对于最直接的窗口的位置。
	//备注:		若无窗口则返回 FullScreen 。
	//********************************
	Point
	GetLocationForWindow() const;
	// 。
	//********************************
	//名称:		GetLocationForParentContainer
	//全名:		YSLib::Components::Widgets::MWidget
	//				::GetLocationForParentContainer
	//可访问性:	public 
	//返回类型:	YSLib::Components::Point
	//修饰符:	const
	//功能概要:	取部件相对于容器的父容器的位置。
	//备注:		若无容器则返回 FullScreen 。
	//********************************
	Point
	GetLocationForParentContainer() const;
	//********************************
	//名称:		GetLocationForParentWindow
	//全名:		YSLib::Components::Widgets::MWidget::GetLocationForParentWindow
	//可访问性:	public 
	//返回类型:	YSLib::Components::Point
	//修饰符:	const
	//功能概要:	取部件相对于容器的父窗口的位置。
	//备注:		若无容器则返回 FullScreen 。
	//********************************
	Point
	GetLocationForParentWindow() const;

	//********************************
	//名称:		Fill
	//全名:		YSLib::Components::Widgets::MWidget::Fill
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	以背景色填充显示缓冲区。
	//备注:		
	//********************************
	virtual void
	Fill();
	//********************************
	//名称:		Fill
	//全名:		YSLib::Components::Widgets::MWidget::Fill
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	Color
	//功能概要:	以纯色填充显示缓冲区。
	//备注:		
	//********************************
	virtual void
	Fill(Color);

protected:
	//********************************
	//名称:		DrawBackground
	//全名:		YSLib::Components::Widgets::MWidget::DrawBackground
	//可访问性:	virtual protected 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制背景。
	//备注:		
	//********************************
	virtual void
	DrawBackground();

	//********************************
	//名称:		DrawForeground
	//全名:		YSLib::Components::Widgets::MWidget::DrawForeground
	//可访问性:	virtual protected 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制前景。
	//备注:		
	//********************************
	virtual void
	DrawForeground();

public:
	//********************************
	//名称:		Refresh
	//全名:		YSLib::Components::Widgets::MWidget::Refresh
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
MWidget::BelongsTo(HWND hWnd) const
{
	return hWindow == hWnd;
}
inline bool
MWidget::BelongsTo(IWidgetContainer* pCon) const
{
	return pContainer == pCon;
}

inline void
MWidget::Fill()
{
	Fill(BackColor);
}


//部件。
class YWidget : public GMCounter<YWidget>, public YComponent, public MWidget,
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
	//形式参数:	IWidgetContainer *
	//功能概要:	构造：使用指定窗口句柄、边界和部件容器指针。
	//备注:		
	//********************************
	explicit
	YWidget(HWND = NULL, const Rect& = Rect::Empty, IWidgetContainer* = NULL);
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

	ImplI(IWidget) DefPredicateBase(Visible, MVisual)
	ImplI(IWidget) DefPredicateBase(Transparent, MVisual)
	ImplI(IWidget) DefPredicateBase(BgRedrawed, MVisual)

	//判断包含关系。
	ImplI(IWidget) PDefH(bool, Contains, const Point& p) const
		ImplBodyBase(MVisual, Contains, p)

	ImplI(IWidget) DefGetterBase(const Point&, Location, MVisual)
	ImplI(IWidget) DefGetterBase(const Drawing::Size&, Size, MVisual)
	ImplI(IWidget) DefGetterBase(IWidgetContainer*, ContainerPtr, MWidget)
	ImplI(IWidget) DefGetterBase(HWND, WindowHandle, MWidget)

	ImplI(IWidget) DefSetterBase(bool, Visible, MVisual)
	ImplI(IWidget) DefSetterBase(bool, Transparent, MVisual)
	ImplI(IWidget) DefSetterBase(bool, BgRedrawed, MVisual)
	ImplI(IWidget) DefSetterBase(const Point&, Location, MVisual)

protected:
	ImplI(IWidget) PDefH(void, DrawBackground)
		ImplBodyBaseVoid(MWidget, DrawBackground)

	ImplI(IWidget) PDefH(void, DrawForeground)
		ImplBodyBaseVoid(MWidget, DrawForeground)

public:
	ImplI(IWidget) PDefH(void, Refresh)
		ImplBodyBaseVoid(MWidget, Refresh)

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
class MWidgetContainer : public GMFocusResponser<IVisualControl>,
	implements GIContainer<IVisualControl>
{
public:
	typedef Components::GMContainer<IWidget> WidgetSet;
	typedef WidgetSet::ContainerType WGTs; //部件组类型。
	typedef Components::GMContainer<GMFocusResponser<IVisualControl> > FOCs; \
		//子焦点对象容器组类型。

protected:
	WidgetSet sWgtSet; //部件对象组模块。
	FOCs sFOCSet; //子焦点对象容器组。

public:
	//********************************
	//名称:		MWidgetContainer
	//全名:		YSLib::Components::Widgets::MWidgetContainer::MWidgetContainer
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		
	//********************************
	MWidgetContainer();
	virtual DefEmptyDtor(MWidgetContainer);

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
	//全名:		YSLib::IVisualControl
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
	//全名:		YSLib::Components::Widgets::MWidgetContainer::GetTopWidgetPtr
	//可访问性:	public 
	//返回类型:	IWidget*
	//修饰符:	const
	//形式参数:	const Point &
	//功能概要:	取顶端部件指针。
	//备注:		
	//********************************
	IWidget*
	GetTopWidgetPtr(const Point&) const;
	//********************************
	//名称:		GetTopVisualControlPtr
	//全名:		YSLib::Components::Widgets::MWidgetContainer
	//				::GetTopVisualControlPtr
	//可访问性:	public 
	//返回类型:	IVisualControl*
	//修饰符:	const
	//形式参数:	const Point &
	//功能概要:	取顶端可视部件指针。
	//备注:		
	//********************************
	IVisualControl*
	GetTopVisualControlPtr(const Point&) const;
};


//部件容器。
class YWidgetContainer : public GMCounter<YWidgetContainer>, public YComponent,
	public MWidget, public MWidgetContainer,
	implements IWidgetContainer
{
public:
	typedef YComponent ParentType;

	//********************************
	//名称:		YWidgetContainer
	//全名:		YSLib::Components::Widgets::YWidgetContainer::YWidgetContainer
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	HWND
	//形式参数:	const Rect &
	//形式参数:	IWidgetContainer *
	//功能概要:	构造：使用指定窗口句柄、边界和部件容器指针。
	//备注:		
	//********************************
	explicit
	YWidgetContainer(HWND, const Rect& = Rect::Empty, IWidgetContainer* = NULL);
	//********************************
	//名称:		~YWidgetContainer
	//全名:		YSLib::Components::Widgets::YWidgetContainer::~YWidgetContainer
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//********************************
	virtual
	~YWidgetContainer() ythrow();

	ImplI(IWidgetContainer) PDefHOperator(void, +=, IWidget& w)
		ImplExpr(sWgtSet += w)
	ImplI(IWidgetContainer) PDefHOperator(bool, -=, IWidget& w)
		ImplRet(sWgtSet -= w)
	ImplI(IWidgetContainer) PDefHOperator(void, +=, IVisualControl& c)
		ImplBodyBaseVoid(MWidgetContainer, operator+=, c)
	ImplI(IWidgetContainer) PDefHOperator(bool, -=, IVisualControl& c)
		ImplBodyBase(MWidgetContainer, operator-=, c)
	ImplI(IWidgetContainer) PDefHOperator(void, +=,
		GMFocusResponser<IVisualControl>& c)
		ImplBodyBaseVoid(MWidgetContainer, operator+=, c)
	ImplI(IWidgetContainer) PDefHOperator(bool, -=,
		GMFocusResponser<IVisualControl>& c)
		ImplBodyBase(MWidgetContainer, operator-=, c)

	ImplI(IWidgetContainer) DefPredicateBase(Visible, MVisual)
	ImplI(IWidgetContainer) DefPredicateBase(Transparent, MVisual)
	ImplI(IWidgetContainer) DefPredicateBase(BgRedrawed, MVisual)

	//判断包含关系。
	ImplI(IWidgetContainer) PDefH(bool, Contains, const Point& p) const
		ImplBodyBase(MVisual, Contains, p)

	ImplI(IWidgetContainer) DefGetterBase(const Point&, Location, MVisual)
	ImplI(IWidgetContainer) DefGetterBase(const Drawing::Size&, Size, MVisual)
	ImplI(IWidgetContainer) DefGetterBase(IWidgetContainer*, ContainerPtr,
		MWidget)
	ImplI(IWidgetContainer) DefGetterBase(HWND, WindowHandle, MWidget)
	ImplI(IWidgetContainer) PDefH(IWidget*, GetTopWidgetPtr,
		const Point& p) const
		ImplBodyBase(MWidgetContainer, GetTopWidgetPtr, p)
	ImplI(IWidgetContainer) PDefH(IVisualControl*, GetTopVisualControlPtr,
		const Point& p) const
		ImplBodyBase(MWidgetContainer, GetTopVisualControlPtr, p)

	ImplI(IWidgetContainer) DefSetterBase(bool, Visible, MVisual)
	ImplI(IWidgetContainer) DefSetterBase(bool, Transparent, MVisual)
	ImplI(IWidgetContainer) DefSetterBase(bool, BgRedrawed, MVisual)
	ImplI(IWidgetContainer) DefSetterBase(const Point&, Location, MVisual)

	ImplI(IWidgetContainer) PDefH(void, ClearFocusingPtr)
		ImplBodyBaseVoid(MWidgetContainer, ClearFocusingPtr)

protected:
	ImplI(IWidgetContainer) PDefH(void, DrawBackground)
		ImplBodyBaseVoid(MWidget, DrawBackground)

	ImplI(IWidgetContainer) PDefH(void, DrawForeground)
		ImplBodyBaseVoid(MWidget, DrawForeground)

public:
	ImplI(IWidgetContainer) PDefH(void, Refresh)
		ImplBodyBaseVoid(MWidget, Refresh)

	//********************************
	//名称:		RequestToTop
	//全名:		YSLib::Components::Widgets::YWidgetContainer::RequestToTop
	//可访问性:	ImplI(IWidgetContainer) public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	请求提升至容器顶端。
	//备注:		空实现。
	//********************************
	ImplI(IWidgetContainer) void
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
	//形式参数:	MWidget &
	//功能概要:	绘制文本。
	//备注:		
	//********************************
	void
	PaintText(MWidget&);
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
	//形式参数:	IWidgetContainer *
	//形式参数:	GHResource<Drawing::TextRegion>
	//功能概要:	构造：
	//			使用指定窗口句柄、字符串、边界、字体、部件容器指针和文本区域。
	//备注:		
	//********************************
	template<class _tChar>
	YLabel(HWND, const _tChar*, const Rect& = Rect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(),
		IWidgetContainer* = NULL, GHResource<Drawing::TextRegion> = NULL);

	virtual DefEmptyDtor(YLabel)

protected:
	//********************************
	//名称:		DrawForeground
	//全名:		YSLib::Components::Widgets::YLabel::DrawForeground
	//可访问性:	virtual protected 
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
	const Drawing::Font& f, IWidgetContainer* pCon,
	GHResource<Drawing::TextRegion> prTr_)
	: YWidget(hWnd, r, pCon), MLabel(l, f, pCon
	? prTr_ : GetGlobalResource<Drawing::TextRegion>())
{}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

