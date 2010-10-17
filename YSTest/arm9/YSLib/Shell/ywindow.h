// YSLib::Shell::YWindow by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-28 16:46:40 + 08:00;
// UTime = 2010-10-17 22:44 + 08:00;
// Version = 0.3488;


#ifndef INCLUDED_YWINDOW_H_
#define INCLUDED_YWINDOW_H_

// YWindow ：平台无关的图形用户界面窗口实现。

#include "ywidget.h"
#include "yguicomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

//窗口接口。
DeclBasedInterface(IWindow, virtual IWidgetContainer, virtual IVisualControl)
	DeclIEntry(operator GraphicInterfaceContext() const) //生成图形接口上下文。

	DeclIEntry(bool IsRefreshRequired() const)
	DeclIEntry(bool IsUpdateRequired() const)

	DeclIEntry(YDesktop* GetDesktopPtr() const)
	DeclIEntry(IVisualControl* GetFocusingPtr() const)
	DeclIEntry(const Drawing::MBitmapBuffer& GetBuffer() const) //取显示缓冲区。
	DeclIEntry(BitmapPtr GetBufferPtr() const) //取缓冲区指针。

	DeclIEntry(void SetRefresh(bool))
	DeclIEntry(void SetUpdate(bool))

	DeclIEntry(void Draw())

	DeclIEntry(void Update()) //按需更新（以父窗口、屏幕优先顺序）。
EndDecl


//桌面对象模块。
class MDesktopObject
{
protected:
	YDesktop* pDesktop; //桌面指针。

public:
	explicit
	MDesktopObject(YDesktop*);

protected:
	DefEmptyDtor(MDesktopObject)

public:
	//判断从属关系。
	PDefH(bool, BelongsTo, YDesktop* pDsk) const
		ImplRet(pDesktop == pDsk)

	DefGetter(YDesktop*, DesktopPtr, pDesktop)
};

inline
MDesktopObject::MDesktopObject(YDesktop* pDsk)
	: pDesktop(pDsk)
{}


//窗口模块。
class MWindow : public Controls::MVisualControl, public MDesktopObject
{
protected:
	Drawing::MBitmapBuffer Buffer; //显示缓冲区。
	//基类中的 hWindow 为父窗口对象句柄，若为空则说明无父窗口。
	HSHL hShell;
	GHResource<YImage> prBackImage; //背景图像指针。
	bool bRefresh; //刷新属性：表示有新的绘制请求。
	bool bUpdate; //更新属性：表示绘制结束，缓冲区准备完毕。

public:
	explicit
	MWindow(const GHResource<YImage> = new YImage(), YDesktop* = ::YSLib::pDefaultDesktop, HSHL = ::YSLib::theApp.GetShellHandle());

	DefPredicate(RefreshRequired, bRefresh)
	DefPredicate(UpdateRequired, bUpdate)

	DefGetter(HSHL, ShellHandle, hShell)
	DefGetter(GHResource<YImage>, Background, prBackImage)

	DefSetterDe(GHResource<YImage>, Background, prBackImage, NULL)
};


//抽象窗口。
class AWindow : public Widgets::MWidget, public MWindow,
	virtual implements IWindow
{
public:
	explicit
	AWindow(const Rect& = Rect::Empty, const GHResource<YImage> = new YImage(),
		YDesktop* = ::YSLib::pDefaultDesktop, HSHL = ::YSLib::theApp.GetShellHandle(), HWND = NULL);
	virtual DefEmptyDtor(AWindow)

	ImplI(IWindow) PDefHOperator(EventMapType::Event&, [], const EventMapType::ID& id)
		ImplBodyBase(MVisualControl, operator[], id)

	ImplI(IWindow) DefConverterMember(GraphicInterfaceContext, Buffer)

	ImplI(IWindow) DefPredicateBase(Visible, MVisual)
	ImplI(IWindow) DefPredicateBase(Transparent, MVisual)
	ImplI(IWindow) DefPredicateBase(BgRedrawed, MVisual)
	ImplI(IWindow) DefPredicateBase(Enabled, MVisualControl)
	ImplI(IWindow) DefPredicateBase(Focused, AFocusRequester)
	ImplI(IWindow) PDefH(bool, IsFocusOfContainer, GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(AFocusRequester, IsFocusOfContainer, c)
	ImplI(IWindow) DefPredicateBase(RefreshRequired, MWindow)
	ImplI(IWindow) DefPredicateBase(UpdateRequired, MWindow)

	//判断从属关系。
	PDefH(bool, BelongsTo, HSHL h) const
		ImplRet(hShell == h)
	PDefH(bool, BelongsTo, YDesktop* pDsk) const
		ImplBodyBase(MDesktopObject, BelongsTo, pDsk)

	//判断包含关系。
	ImplI(IWindow) PDefH(bool, Contains, const Point& p) const
		ImplBodyBase(MVisual, Contains, p)

	ImplI(IWindow) PDefH(bool, CheckRemoval, GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(MVisualControl, CheckRemoval, c)

	ImplI(IWindow) DefGetterBase(const Point&, Location, MVisual)
	ImplI(IWindow) DefGetterBase(const Drawing::Size&, Size, MVisual)
	ImplI(IWindow) DefGetterBase(IWidgetContainer*, ContainerPtr, MWidget)
	ImplI(IWindow) DefGetterBase(HWND, WindowHandle, MWidget)
	ImplI(IWindow) DefGetterBase(YDesktop*, DesktopPtr, MDesktopObject)
	ImplI(IWindow) DefGetter(const Drawing::MBitmapBuffer&, Buffer, Buffer)
	ImplI(IWindow) DefGetterMember(BitmapPtr, BufferPtr, Buffer)
	DefGetter(HWND, Handle, HWND(const_cast<AWindow*>(this)))
	DefGetterBase(HSHL, ShellHandle, MWindow)
	DefGetterBase(GHResource<YImage>, Background, MWindow)
	BitmapPtr
	GetBackgroundPtr() const;

	ImplI(IWindow) DefEventGetter(Controls::InputEventHandler, Enter)
	ImplI(IWindow) DefEventGetter(Controls::InputEventHandler, Leave)
	ImplI(IWindow) DefEventGetter(Controls::KeyEventHandler, KeyUp)
	ImplI(IWindow) DefEventGetter(Controls::KeyEventHandler, KeyDown)
	ImplI(IWindow) DefEventGetter(Controls::KeyEventHandler, KeyHeld)
	ImplI(IWindow) DefEventGetter(Controls::KeyEventHandler, KeyPress)
	ImplI(IWindow) DefEventGetter(Controls::TouchEventHandler, TouchUp)
	ImplI(IWindow) DefEventGetter(Controls::TouchEventHandler, TouchDown)
	ImplI(IWindow) DefEventGetter(Controls::TouchEventHandler, TouchHeld)
	ImplI(IWindow) DefEventGetter(Controls::TouchEventHandler, TouchMove)
	ImplI(IWindow) DefEventGetter(Controls::TouchEventHandler, Click)

	ImplI(IWindow) DefSetterBase(bool, Visible, MVisual)
	ImplI(IWindow) DefSetterBase(bool, Transparent, MVisual)
	ImplI(IWindow) DefSetterBase(bool, BgRedrawed, MVisual)
	ImplI(IWindow) DefSetterBase(const Point&, Location, MVisual)
	ImplI(IWindow) DefSetter(bool, Refresh, bRefresh)
	ImplI(IWindow) DefSetter(bool, Update, bUpdate)
	virtual void
	SetSize(const Drawing::Size&);
	ImplI(IWindow) DefSetterBaseDe(bool, Enabled, MVisualControl, true)
	ImplI(IWindow) DefSetterBaseDe(GHResource<YImage>, Background, MWindow, NULL)

	PDefH(void, ClearBackground) const //清除背景。
		ImplExpr(Buffer.ClearImage())

public:
	PDefH(void, Fill, PixelType c)
		ImplBodyMemberVoid(Buffer, Fill, c) //以纯色填充显示缓冲区。
	bool
	DrawBackgroundImage();
	ImplI(IWindow) void
	DrawBackground();
	ImplI(IWindow) void
	DrawForeground();
	//ImplI(IWindow)
	DeclIEntry(bool DrawWidgets())
	ImplI(IWindow) void
	Draw();

	ImplI(IWindow) void
	Refresh();
	ImplI(IWindow) void
	Update();

private:
	virtual PDefH(bool, ReleaseFocus, GMFocusResponser<IVisualControl>& c)
		ImplBodyBase(MVisualControl, ReleaseFocus, c)

public:
	ImplI(IWindow) void
	RequestFocus(const MEventArgs&); //向部件容器申请获得焦点。
	ImplI(IWindow) void
	ReleaseFocus(const MEventArgs&); //释放焦点。
	ImplI(IWindow) void
	RequestToTop();

	virtual void
	UpdateToScreen() const; //更新至屏幕。
	virtual void
	UpdateToWindow() const; //更新至上层窗口缓冲区。
	void
	UpdateToScreen(YDesktop&) const; //更新至指定桌面所在的屏幕。
	void
	UpdateToWindow(IWindow&) const; //更新至指定窗口缓冲区。
	void
	Show();

	static void
	OnGotFocus(IControl&, const MEventArgs&);
	static void
	OnLostFocus(IControl&, const MEventArgs&);
};

inline void
AWindow::UpdateToScreen() const
{
	if(GetDesktopPtr())
		UpdateToScreen(*GetDesktopPtr());
}
inline void
AWindow::UpdateToWindow() const
{
	if(GetWindowHandle())
		UpdateToWindow(*GetWindowHandle());
}


//框架窗口。
class YFrameWindow : public GMCounter<YFrameWindow>, public YComponent, public AWindow, public Widgets::MWidgetContainer
{
public:
	typedef YComponent ParentType;

	explicit
	YFrameWindow(const Rect& = Rect::Empty, const GHResource<YImage> = new YImage(),
		YDesktop* = ::YSLib::pDefaultDesktop, HSHL = ::YSLib::theApp.GetShellHandle(), HWND = NULL);
	virtual
	~YFrameWindow();

	virtual PDefHOperator(void, +=, IWidget& w)
		ImplExpr(sWgtSet += w)
	virtual PDefHOperator(bool, -=, IWidget& w)
		ImplRet(sWgtSet -= w)
	virtual PDefHOperator(void, +=, IVisualControl& c)
		ImplBodyBaseVoid(MWidgetContainer, operator+=, c)
	virtual PDefHOperator(bool, -=, IVisualControl& c)
		ImplBodyBase(MWidgetContainer, operator-=, c)
	virtual PDefHOperator(void, +=, GMFocusResponser<IVisualControl>& c)
		ImplBodyBaseVoid(MWidgetContainer, operator+=, c)
	virtual PDefHOperator(bool, -=, GMFocusResponser<IVisualControl>& c)
		ImplBodyBase(MWidgetContainer, operator-=, c)

	virtual PDefH(IWidget*, GetTopWidgetPtr, const Point& p) const
		ImplBodyBase(MWidgetContainer, GetTopWidgetPtr, p)
	virtual PDefH(IVisualControl*, GetTopVisualControlPtr, const Point& p) const
		ImplBodyBase(MWidgetContainer, GetTopVisualControlPtr, p)
	virtual DefGetterBase(IVisualControl*, FocusingPtr, GMFocusResponser<IVisualControl>)

	virtual PDefH(void, ClearFocusingPtr)
		ImplBodyBaseVoid(MWidgetContainer, ClearFocusingPtr)

	virtual bool
	DrawWidgets();
};

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

