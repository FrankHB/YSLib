// YSLib::Shell::YWindow by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-28 16:46:40 + 08:00;
// UTime = 2010-10-04 21:37 + 08:00;
// Version = 0.3310;


#ifndef INCLUDED_YWINDOW_H_
#define INCLUDED_YWINDOW_H_

// YWindow ：平台无关的图形用户界面窗口实现。

#include "ywidget.h"
#include "yguicomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

//窗口接口。
DeclBasedInterface(IWindow, IWidgetContainer, IVisualControl)
	DeclIEntry(operator GraphicInterfaceContext() const) //生成图形接口上下文。

	DeclIEntry(bool IsRefreshRequired() const)
	DeclIEntry(bool IsUpdateRequired() const)

	DeclIEntry(YDesktop* GetDesktopPtr() const)
	DeclIEntry(IVisualControl* GetFocusingPtr() const)
	DeclIEntry(const Drawing::MBitmapBuffer& GetBuffer() const) //取显示缓冲区。
	DeclIEntry(BitmapPtr GetBufferPtr() const) //取缓冲区指针。

	DeclIEntry(void SetRefresh(bool = true))
	DeclIEntry(void SetUpdate(bool = true))

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
	DefTrivialDtor(MDesktopObject)

public:
	//判断从属关系。
	bool
	BelongsTo(YDesktop*) const;

	YDesktop* GetDesktopPtr() const;
};

inline
MDesktopObject::MDesktopObject(YDesktop* pDsk)
: pDesktop(pDsk)
{}

inline bool
MDesktopObject::BelongsTo(YDesktop* pDsk) const
{
	return pDesktop == pDsk;
}

inline YDesktop*
MDesktopObject::GetDesktopPtr() const
{
	return pDesktop;
}


//窗口模块。
class MWindow : public MDesktopObject, public Controls::MVisualControl
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


//抽象窗口模块基类。
class AWindow : public Widgets::MWidget, public MWindow,
	implements IWindow
{
public:
	explicit
	AWindow(const SRect& = SRect::Empty, const GHResource<YImage> = new YImage(),
		YDesktop* = ::YSLib::pDefaultDesktop, HSHL = ::YSLib::theApp.GetShellHandle(), HWND = NULL);
	virtual
	~AWindow();

	virtual PDefOpHead(EventMapType::Event&, [], const EventMapType::ID& id)
		ImplBodyBase(MVisualControl, operator[], id)

	virtual DefConverterMember(GraphicInterfaceContext, Buffer)

	virtual DefPredicateBase(Visible, MVisual)
	virtual DefPredicateBase(Transparent, MVisual)
	virtual DefPredicateBase(BgRedrawed, MVisual)
	virtual DefPredicateBase(Enabled, MVisualControl)
	virtual DefPredicateBase(Focused, AFocusRequester)
	virtual PDefHead(bool, IsFocusOfContainer, Runtime::GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(AFocusRequester, IsFocusOfContainer, c)
	virtual DefPredicateBase(RefreshRequired, MWindow)
	virtual DefPredicateBase(UpdateRequired, MWindow)

	//判断从属关系。
	PDefHead(bool, BelongsTo, HSHL h) const
		ImplRet(hShell == h)
	PDefHead(bool, BelongsTo, YDesktop* pDsk) const
		ImplBodyBase(MDesktopObject, BelongsTo, pDsk)

	//判断包含关系。
	virtual PDefHead(bool, Contains, const SPoint& p) const
		ImplBodyBase(MVisual, Contains, p)

	virtual PDefHead(bool, CheckRemoval, Runtime::GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(MVisualControl, CheckRemoval, c)

	virtual DefGetterBase(const SPoint&, Location, MVisual)
	virtual DefGetterBase(const SSize&, Size, MVisual)
	virtual DefGetterBase(IWidgetContainer*, ContainerPtr, MWidget)
	virtual DefGetterBase(HWND, WindowHandle, MWidget)
	virtual DefGetterBase(YDesktop*, DesktopPtr, MDesktopObject)
	virtual DefGetter(const Drawing::MBitmapBuffer&, Buffer, Buffer)
	virtual DefGetterMember(BitmapPtr, BufferPtr, Buffer)
	DefGetter(HWND, Handle, HWND(const_cast<AWindow*>(this)))
	DefGetterBase(HSHL, ShellHandle, MWindow)
	DefGetterBase(GHResource<YImage>, Background, MWindow)
	BitmapPtr
	GetBackgroundPtr() const;

	virtual DefSetterBaseDe(bool, Visible, MVisual, true)
	virtual DefSetterBaseDe(bool, Transparent, MVisual, true)
	virtual DefSetterBaseDe(bool, BgRedrawed, MVisual, true)
	virtual DefSetterBase(const SPoint&, Location, MVisual)
	virtual DefSetterDe(bool, Refresh, bRefresh, true)
	virtual DefSetterDe(bool, Update, bUpdate, true)
	virtual void
	SetSize(SDST, SDST);
	virtual void
	SetBounds(const SRect&);
	virtual DefSetterBaseDe(bool, Enabled, MVisualControl, true)
	virtual DefSetterBaseDe(GHResource<YImage>, Background, MWindow, NULL)

	PDefHead(void, ClearBackground) const //清除背景。
		ImplExpr(Buffer.ClearImage())

public:
	virtual void
	Fill(PixelType); //以纯色填充显示缓冲区。
	bool
	DrawBackgroundImage();
	virtual void
	DrawBackground();
	virtual void
	DrawForeground();
	virtual bool
	DrawWidgets() = 0;
	virtual void
	Draw();

	virtual void
	Refresh();
	virtual void
	Update();

private:
	virtual PDefHead(bool, ReleaseFocus, Runtime::GMFocusResponser<IVisualControl>& c)
		ImplBodyBase(MVisualControl, ReleaseFocus, c)

public:
	virtual void
	RequestFocus(const MEventArgs& = GetZeroElement<MEventArgs>()); //向部件容器申请获得焦点。
	virtual void
	ReleaseFocus(const MEventArgs& = GetZeroElement<MEventArgs>()); //释放焦点。
	virtual void
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
	OnGotFocus(IControl&, const MEventArgs& = GetZeroElement<MEventArgs>());
	static void
	OnLostFocus(IControl&, const MEventArgs& = GetZeroElement<MEventArgs>());
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
	YFrameWindow(const SRect& = SRect::Empty, const GHResource<YImage> = new YImage(),
		YDesktop* = ::YSLib::pDefaultDesktop, HSHL = ::YSLib::theApp.GetShellHandle(), HWND = NULL);
	virtual
	~YFrameWindow();

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

	virtual PDefHead(IWidget*, GetTopWidgetPtr, const SPoint& p) const
		ImplBodyBase(MWidgetContainer, GetTopWidgetPtr, p)
	virtual PDefHead(IVisualControl*, GetTopVisualControlPtr, const SPoint& p) const
		ImplBodyBase(MWidgetContainer, GetTopVisualControlPtr, p)
	virtual SPoint
	GetContainerLocationOffset(const SPoint& = SPoint::Zero) const;
	virtual SPoint
	GetWindowLocationOffset(const SPoint& = SPoint::Zero) const;
	virtual DefGetterBase(IVisualControl*, FocusingPtr, Runtime::GMFocusResponser<IVisualControl>)

	virtual PDefHead(void, ClearFocusingPtr)
		ImplBodyBaseVoid(MWidgetContainer, ClearFocusingPtr)

	virtual bool
	DrawWidgets();
};

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

