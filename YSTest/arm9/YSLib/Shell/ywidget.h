// YSLib::Shell::YWidget by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-09 10:56 + 08:00;
// Version = 0.4734;


#ifndef INCLUDED_YWIDGET_H_
#define INCLUDED_YWIDGET_H_

// YWidget ：平台无关的图形用户界面部件实现。

#include "ycomp.h"
#include "../Core/ymodule.h"
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

	DeclIEntry(void SetVisible(bool = true)) //设置可见。
	DeclIEntry(void SetTransparent(bool = true)) //设置透明。
	DeclIEntry(void SetBgRedrawed(bool = true)) //设置重绘状态。
	DeclIEntry(void SetLocation(const Point&)) //设置左上角所在位置（相对于容器的偏移坐标）。

	DeclIEntry(void DrawBackground()) //绘制背景。
	DeclIEntry(void DrawForeground()) //绘制前景。

	DeclIEntry(void Refresh()) //刷新至窗口缓冲区。

	DeclIEntry(void RequestToTop()) //请求提升至容器顶端。
EndDecl


//部件容器接口。
DeclBasedInterface(IWidgetContainer, IWidget)
	DeclIEntry(void operator+=(IWidget&)) //向部件组添加部件。
	DeclIEntry(bool operator-=(IWidget&)) //从部件组移除部件。
	DeclIEntry(void operator+=(IVisualControl&)) //向焦点对象组添加可视控件。
	DeclIEntry(bool operator-=(IVisualControl&)) //从焦点对象组移除可视控件。
	DeclIEntry(void operator+=(Runtime::GMFocusResponser<IVisualControl>&)) //向焦点对象组添加子焦点对象容器。
	DeclIEntry(bool operator-=(Runtime::GMFocusResponser<IVisualControl>&)) //从焦点对象组移除子焦点对象容器。

	DeclIEntry(IWidget* GetTopWidgetPtr(const Point&) const) //取指定的点（屏幕坐标）所处的部件的指针。
	DeclIEntry(IVisualControl* GetTopVisualControlPtr(const Point&) const) //取指定的点（屏幕坐标）所处的焦点对象的指针。
	DeclIEntry(Point GetContainerLocationOffset(const Point& = Point::Zero) const) //取指定的点（相对此容器的坐标）相对于此容器的父容器的偏移坐标。
	DeclIEntry(Point GetWindowLocationOffset(const Point& = Point::Zero) const) //取指定的点（相对此容器的坐标）相对于此容器的父窗口的偏移坐标。

	DeclIEntry(void ClearFocusingPtr()) //清除焦点指针。
EndDecl


Point
GetLocationOffset(IWidget*, const Point&, const HWND&);


//可视样式模块。
class MVisual
{
public:
	bool Visible; //可见性。
	bool Transparent; //透明性。

private:
	mutable bool bBgRedrawed; //背景重绘状态。

protected:
	Point Location; //左上角所在位置（相对于容器的偏移坐标）。
	Drawing::Size Size; //部件大小。

public:
	Color BackColor; //默认背景色。
	Color ForeColor; //默认前景色。

	explicit
	MVisual(const Rect& = Rect::Empty,
		Color = Color::White, Color = Color::Black);
	virtual DefTrivialDtor(MVisual)

	DefPredicate(Visible, Visible)
	DefPredicate(Transparent, Transparent)
	DefPredicate(BgRedrawed, bBgRedrawed)

	//判断包含关系。
	PDefHead(bool, Contains, const Point& p) const
		ImplBodyMember(GetBounds(), IsInBoundsRegular, p)
	PDefHead(bool, Contains, const int& x, const int& y) const //判断点(x, y)是否在边界内或边界上。
		ImplBodyMember(GetBounds(), IsInBoundsRegular, x, y)

	DefGetter(SPOS, X, Location.X)
	DefGetter(SPOS, Y, Location.Y)
	DefGetter(SDST, Width, Size.Width)
	DefGetter(SDST, Height, Size.Height)
	virtual DefGetter(const Point&, Location, Location)
	virtual DefGetter(const Drawing::Size&, Size, Size)
	virtual DefGetter(Rect, Bounds, Rect(Location, Size.Width, Size.Height))

	DefSetterDe(bool, Visible, Visible, true)
	DefSetterDe(bool, Transparent, Transparent, true)
	DefSetterDe(bool, BgRedrawed, bBgRedrawed, true)
	virtual DefSetter(const Point&, Location, Location)
	virtual PDefHead(void, SetLocation, SPOS x, SPOS y)
		ImplBodyBaseVoid(MVisual, SetLocation, Point(x, y))
	virtual void
	SetSize(SDST, SDST);
	virtual void
	SetBounds(const Rect& r);
};


//部件模块。
class MWidget : public MVisual
{
protected:
	HWND hWindow; //从属的窗口的句柄。

public:
	IWidgetContainer* const pContainer; //从属的部件容器的指针。

	explicit
	MWidget(HWND = NULL, const Rect& = Rect::Empty, IWidgetContainer* = NULL,
		Color = Color::White, Color = Color::Black);

	//判断从属关系。
	bool
	BelongsTo(HWND) const;
	bool
	BelongsTo(IWidgetContainer*) const;

	virtual DefGetter(IWidgetContainer*, ContainerPtr, pContainer)
	virtual DefGetter(HWND, WindowHandle, hWindow)
	Point
	GetLocationForWindow() const; //取部件相对于最直接的窗口的位置（若无窗口则返回 FullScreen ）。
	Point
	GetLocationForParentContainer() const; //取部件相对于容器的父容器的位置（若无容器则返回 FullScreen ）。
	Point
	GetLocationForParentWindow() const; //取部件相对于容器的父窗口的位置（若无容器则返回 FullScreen ）。

	virtual void
	Fill(); //以背景色填充显示缓冲区。
	virtual void
	Fill(Color); //以纯色填充显示缓冲区。
	virtual void
	DrawBackground();
	virtual void
	DrawForeground();

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

	explicit
	YWidget(HWND = NULL, const Rect& = Rect::Empty, IWidgetContainer* = NULL);
	virtual
	~YWidget();

	virtual DefPredicateBase(Visible, MVisual)
	virtual DefPredicateBase(Transparent, MVisual)
	virtual DefPredicateBase(BgRedrawed, MVisual)

	//判断包含关系。
	virtual PDefHead(bool, Contains, const Point& p) const
		ImplBodyBase(MVisual, Contains, p)

	virtual DefGetterBase(const Point&, Location, MVisual)
	virtual DefGetterBase(const Drawing::Size&, Size, MVisual)
	virtual DefGetterBase(IWidgetContainer*, ContainerPtr, MWidget)
	virtual DefGetterBase(HWND, WindowHandle, MWidget)

	virtual DefSetterBaseDe(bool, Visible, MVisual, true)
	virtual DefSetterBaseDe(bool, Transparent, MVisual, true)
	virtual DefSetterBaseDe(bool, BgRedrawed, MVisual, true)
	virtual DefSetterBase(const Point&, Location, MVisual)

	virtual PDefHead(void, DrawBackground)
		ImplBodyBaseVoid(MWidget, DrawBackground)
	virtual PDefHead(void, DrawForeground)
		ImplBodyBaseVoid(MWidget, DrawForeground)

	virtual PDefHead(void, Refresh)
		ImplBodyBaseVoid(MWidget, Refresh)

	virtual void
	RequestToTop()
	{}
};


//部件容器模块。
class MWidgetContainer : public Runtime::GMFocusResponser<IVisualControl>,
	implements GIContainer<IVisualControl>
{
public:
	typedef Components::GMContainer<IWidget> WidgetSet;
	typedef WidgetSet::ContainerType WGTs; //部件组类型。
	typedef Components::GMContainer<Runtime::GMFocusResponser<IVisualControl> > FOCs; //子焦点对象容器组类型。

protected:
	WidgetSet sWgtSet; //部件对象组模块。
	FOCs sFOCSet; //子焦点对象容器组。

public:
	MWidgetContainer();

protected:
	PDefOpHead(void, +=, IVisualControl& r) //向焦点对象组添加焦点对象。
		ImplBodyBaseVoid(Runtime::GMFocusResponser<IVisualControl>, operator+=, r)
	PDefOpHead(bool, -=, IVisualControl& r) //从焦点对象组移除焦点对象。
		ImplBodyBase(Runtime::GMFocusResponser<IVisualControl>, operator-=, r)
	PDefOpHead(void, +=, Runtime::GMFocusResponser<IVisualControl>& c) //向子焦点对象容器组添加子焦点对象容器。
		ImplBodyMemberVoid(sFOCSet, insert, &c)
	PDefOpHead(bool, -=, Runtime::GMFocusResponser<IVisualControl>& c) //从子焦点对象容器组移除子焦点对象容器。
		ImplBodyMember(sFOCSet, erase, &c)

public:
	virtual IVisualControl*
	GetFocusingPtr() const;
	virtual IWidget*
	GetTopWidgetPtr(const Point&) const;
	virtual IVisualControl*
	GetTopVisualControlPtr(const Point&) const;
};


//部件容器。
class YWidgetContainer : public GMCounter<YWidgetContainer>, public YComponent, public MWidget, public MWidgetContainer,
	implements IWidgetContainer
{
public:
	typedef YComponent ParentType;

	explicit
	YWidgetContainer(HWND, const Rect& = Rect::Empty, IWidgetContainer* = NULL);
	virtual
	~YWidgetContainer();

	virtual PDefOpHead(void, +=, IWidget& w)
		ImplExpr(sWgtSet += w)
	virtual PDefOpHead(bool, -=, IWidget& w)
		ImplRet(sWgtSet -= w)
	virtual PDefOpHead(void, +=, IVisualControl& c)
		ImplBodyBaseVoid(MWidgetContainer, operator+=, c)
	virtual PDefOpHead(bool, -=, IVisualControl& c)
		ImplBodyBase(MWidgetContainer, operator-=, c)
	virtual PDefOpHead(void, +=, Runtime::GMFocusResponser<IVisualControl>& c)
		ImplBodyBaseVoid(MWidgetContainer, operator+=, c)
	virtual PDefOpHead(bool, -=, Runtime::GMFocusResponser<IVisualControl>& c)
		ImplBodyBase(MWidgetContainer, operator-=, c)

	virtual DefPredicateBase(Visible, MVisual)
	virtual DefPredicateBase(Transparent, MVisual)
	virtual DefPredicateBase(BgRedrawed, MVisual)

	//判断包含关系。
	virtual PDefHead(bool, Contains, const Point& p) const
		ImplBodyBase(MVisual, Contains, p)

	virtual DefGetterBase(const Point&, Location, MVisual)
	virtual DefGetterBase(const Drawing::Size&, Size, MVisual)
	virtual DefGetterBase(IWidgetContainer*, ContainerPtr, MWidget)
	virtual DefGetterBase(HWND, WindowHandle, MWidget)
	virtual PDefHead(IWidget*, GetTopWidgetPtr, const Point& p) const
		ImplBodyBase(MWidgetContainer, GetTopWidgetPtr, p)
	virtual PDefHead(IVisualControl*, GetTopVisualControlPtr, const Point& p) const
		ImplBodyBase(MWidgetContainer, GetTopVisualControlPtr, p)
	virtual Point
	GetContainerLocationOffset(const Point& = Point::Zero) const;
	virtual Point
	GetWindowLocationOffset(const Point& = Point::Zero) const;

	virtual DefSetterBaseDe(bool, Visible, MVisual, true)
	virtual DefSetterBaseDe(bool, Transparent, MVisual, true)
	virtual DefSetterBaseDe(bool, BgRedrawed, MVisual, true)
	virtual DefSetterBase(const Point&, Location, MVisual)

	virtual PDefHead(void, ClearFocusingPtr)
		ImplBodyBaseVoid(MWidgetContainer, ClearFocusingPtr)

	virtual PDefHead(void, DrawBackground)
		ImplBodyBaseVoid(MWidget, DrawBackground)
	virtual PDefHead(void, DrawForeground)
		ImplBodyBaseVoid(MWidget, DrawForeground)

	virtual PDefHead(void, Refresh)
		ImplBodyBaseVoid(MWidget, Refresh)

	virtual void
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

	template<class _tChar>
	MLabel(const _tChar*, const Drawing::Font& = Drawing::Font::GetDefault(), GHResource<Drawing::TextRegion> = NULL);

protected:
	DefTrivialDtor(MLabel)

	void
	PaintText(MWidget&);
};

template<class _tChar>
MLabel::MLabel(const _tChar* l, const Drawing::Font& f, GHResource<Drawing::TextRegion> prTr_)
	: prTextRegion(prTr_ ? prTr_ : GetGlobalResource<Drawing::TextRegion>()), Font(f),
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

	//用字符串在窗口中以给定字号初始化标签。
	template<class _tChar>
	YLabel(HWND, const _tChar*, const Rect& = Rect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(), IWidgetContainer* = NULL, GHResource<Drawing::TextRegion> = NULL);

	virtual void
	DrawForeground();
};

template<class _tChar>
YLabel::YLabel(HWND hWnd, const _tChar* l, const Rect& r,
	const Drawing::Font& f, IWidgetContainer* pCon, GHResource<Drawing::TextRegion> prTr_)
	: YWidget(hWnd, r, pCon), MLabel(l, f, pCon ? prTr_ : GetGlobalResource<Drawing::TextRegion>())
{}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

