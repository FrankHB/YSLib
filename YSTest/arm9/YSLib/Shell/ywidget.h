// YSLib::Shell::YWidget by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-22 13:52 + 08:00;
// Version = 0.4900;


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
	DeclIEntry(void SetLocation(const Point&)) //设置左上角所在位置（相对于容器的偏移坐标）。

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
	DeclIEntry(void operator+=(GMFocusResponser<IVisualControl>&)) //向焦点对象组添加子焦点对象容器。
	DeclIEntry(bool operator-=(GMFocusResponser<IVisualControl>&)) //从焦点对象组移除子焦点对象容器。

	DeclIEntry(IWidget* GetTopWidgetPtr(const Point&) const) //取指定的点（屏幕坐标）所处的部件的指针。
	DeclIEntry(IVisualControl* GetTopVisualControlPtr(const Point&) const) //取指定的点（屏幕坐标）所处的焦点对象的指针。

	//清除焦点指针。
	DeclIEntry(void ClearFocusingPtr())
EndDecl


//取指定的点（相对此部件的坐标）相对于指定父窗口的偏移坐标。
Point
GetLocationOffset(const IWidget*, const Point&, const HWND&);

//取指定的点（相对此部件的坐标）相对于此部件的父容器的偏移坐标。
inline Point
GetContainerLocationOffset(const IWidget& w, const Point& p)
{
	return p + w.GetLocation();
}

//取指定的点（相对此部件的坐标）相对于此部件的父窗口的偏移坐标。
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

	explicit
	MVisual(const Rect& = Rect::Empty,
		Color = ColorSpace::White, Color = ColorSpace::Black);
	virtual DefEmptyDtor(MVisual)

private:
	void
	_m_SetSize(SDST, SDST);

public:
	DefPredicate(Visible, Visible)
	DefPredicate(Transparent, Transparent)
	DefPredicate(BgRedrawed, bBgRedrawed)

	//判断包含关系。
	virtual PDefH(bool, Contains, const Point& p) const //判断点 P 是否在边界内或边界上。
		ImplBodyMember(GetBounds(), IsInBoundsRegular, p.X, p.Y)
	PDefH(bool, Contains, SPOS x, SPOS y) const //判断点 (x, y) 是否在边界内或边界上。
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

	//判断从属关系。
	bool
	BelongsTo(HWND) const;
	bool
	BelongsTo(IWidgetContainer*) const;

	DefGetter(IWidgetContainer*, ContainerPtr, pContainer)
	DefGetter(HWND, WindowHandle, hWindow)
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

	ImplI(IWidget) PDefH(void, DrawBackground)
		ImplBodyBaseVoid(MWidget, DrawBackground)

		ImplI(IWidget) PDefH(void, DrawForeground)
		ImplBodyBaseVoid(MWidget, DrawForeground)

	ImplI(IWidget) PDefH(void, Refresh)
		ImplBodyBaseVoid(MWidget, Refresh)

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
	typedef Components::GMContainer<GMFocusResponser<IVisualControl> > FOCs; //子焦点对象容器组类型。

protected:
	WidgetSet sWgtSet; //部件对象组模块。
	FOCs sFOCSet; //子焦点对象容器组。

public:
	MWidgetContainer();
	virtual DefEmptyDtor(MWidgetContainer);

protected:
	ImplI(GIContainer<IVisualControl>) PDefHOperator(void, +=, IVisualControl& r) //向焦点对象组添加焦点对象。
		ImplBodyBaseVoid(GMFocusResponser<IVisualControl>, operator+=, r)
	ImplI(GIContainer<IVisualControl>) PDefHOperator(bool, -=, IVisualControl& r) //从焦点对象组移除焦点对象。
		ImplBodyBase(GMFocusResponser<IVisualControl>, operator-=, r)
	PDefHOperator(void, +=, GMFocusResponser<IVisualControl>& c) //向子焦点对象容器组添加子焦点对象容器。
		ImplBodyMemberVoid(sFOCSet, insert, &c)
	PDefHOperator(bool, -=, GMFocusResponser<IVisualControl>& c) //从子焦点对象容器组移除子焦点对象容器。
		ImplBodyMember(sFOCSet, erase, &c)

public:
	IVisualControl*
	GetFocusingPtr() const;
	IWidget*
	GetTopWidgetPtr(const Point&) const;
	IVisualControl*
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

	ImplI(IWidgetContainer) PDefHOperator(void, +=, IWidget& w)
		ImplExpr(sWgtSet += w)
	ImplI(IWidgetContainer) PDefHOperator(bool, -=, IWidget& w)
		ImplRet(sWgtSet -= w)
	ImplI(IWidgetContainer) PDefHOperator(void, +=, IVisualControl& c)
		ImplBodyBaseVoid(MWidgetContainer, operator+=, c)
	ImplI(IWidgetContainer) PDefHOperator(bool, -=, IVisualControl& c)
		ImplBodyBase(MWidgetContainer, operator-=, c)
	ImplI(IWidgetContainer) PDefHOperator(void, +=, GMFocusResponser<IVisualControl>& c)
		ImplBodyBaseVoid(MWidgetContainer, operator+=, c)
	ImplI(IWidgetContainer) PDefHOperator(bool, -=, GMFocusResponser<IVisualControl>& c)
		ImplBodyBase(MWidgetContainer, operator-=, c)

	ImplI(IWidgetContainer) DefPredicateBase(Visible, MVisual)
	ImplI(IWidgetContainer) DefPredicateBase(Transparent, MVisual)
	ImplI(IWidgetContainer) DefPredicateBase(BgRedrawed, MVisual)

	//判断包含关系。
	ImplI(IWidgetContainer) PDefH(bool, Contains, const Point& p) const
		ImplBodyBase(MVisual, Contains, p)

	ImplI(IWidgetContainer) DefGetterBase(const Point&, Location, MVisual)
	ImplI(IWidgetContainer) DefGetterBase(const Drawing::Size&, Size, MVisual)
	ImplI(IWidgetContainer) DefGetterBase(IWidgetContainer*, ContainerPtr, MWidget)
	ImplI(IWidgetContainer) DefGetterBase(HWND, WindowHandle, MWidget)
	ImplI(IWidgetContainer) PDefH(IWidget*, GetTopWidgetPtr, const Point& p) const
		ImplBodyBase(MWidgetContainer, GetTopWidgetPtr, p)
	ImplI(IWidgetContainer) PDefH(IVisualControl*, GetTopVisualControlPtr, const Point& p) const
		ImplBodyBase(MWidgetContainer, GetTopVisualControlPtr, p)

	ImplI(IWidgetContainer) DefSetterBase(bool, Visible, MVisual)
	ImplI(IWidgetContainer) DefSetterBase(bool, Transparent, MVisual)
	ImplI(IWidgetContainer) DefSetterBase(bool, BgRedrawed, MVisual)
	ImplI(IWidgetContainer) DefSetterBase(const Point&, Location, MVisual)

	ImplI(IWidgetContainer) PDefH(void, ClearFocusingPtr)
		ImplBodyBaseVoid(MWidgetContainer, ClearFocusingPtr)

	ImplI(IWidgetContainer) PDefH(void, DrawBackground)
		ImplBodyBaseVoid(MWidget, DrawBackground)

	ImplI(IWidgetContainer) PDefH(void, DrawForeground)
		ImplBodyBaseVoid(MWidget, DrawForeground)

	ImplI(IWidgetContainer) PDefH(void, Refresh)
		ImplBodyBaseVoid(MWidget, Refresh)

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

	template<class _tChar>
	MLabel(const _tChar*, const Drawing::Font& = Drawing::Font::GetDefault(), GHResource<Drawing::TextRegion> = NULL);

protected:
	DefEmptyDtor(MLabel)

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

