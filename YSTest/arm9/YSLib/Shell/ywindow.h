// YSLib::Shell::YWindow by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-28 16:46:40 + 08:00;
// UTime = 2010-11-03 19:56 + 08:00;
// Version = 0.3732;


#ifndef INCLUDED_YWINDOW_H_
#define INCLUDED_YWINDOW_H_

// YWindow ：平台无关的图形用户界面窗口实现。

#include "ywidget.h"
#include "yguicomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

//窗口接口。
DeclBasedInterface(IWindow, virtual IUIContainer, virtual IVisualControl)
	DeclIEntry(operator Graphics() const) //生成图形接口上下文。

	DeclIEntry(bool IsRefreshRequired() const)
	DeclIEntry(bool IsUpdateRequired() const)

	DeclIEntry(YDesktop* GetDesktopPtr() const)

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
	//********************************
	//名称:		MDesktopObject
	//全名:		YSLib::Components::Forms::MDesktopObject::MDesktopObject
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	YDesktop *
	//功能概要:	构造：使用指定桌面指针。
	//备注:		
	//********************************
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
class MWindow : protected MDesktopObject
{
protected:
	Drawing::BitmapBuffer Buffer; //显示缓冲区。
	//基类中的 hWindow 为父窗口对象句柄，若为空则说明无父窗口。
	HSHL hShell;
	GHResource<YImage> prBackImage; //背景图像指针。
	bool bRefresh; //刷新属性：表示有新的绘制请求。
	bool bUpdate; //更新属性：表示绘制结束，缓冲区准备完毕。

public:
	//********************************
	//名称:		MWindow
	//全名:		YSLib::Components::Forms::MWindow::MWindow
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const GHResource<YImage>
	//形式参数:	YDesktop *
	//形式参数:	HSHL
	//功能概要:	构造：使用指定背景图像、桌面指针和 Shell 。
	//备注:		
	//********************************
	explicit
	MWindow(const GHResource<YImage> = new YImage(),
		YDesktop* = ::YSLib::pDefaultDesktop,
		HSHL = ::YSLib::theApp.GetShellHandle());

	DefPredicate(RefreshRequired, bRefresh)
	DefPredicate(UpdateRequired, bUpdate)

	DefGetter(HSHL, ShellHandle, hShell)
	DefGetter(GHResource<YImage>, Background, prBackImage)

	DefSetterDe(GHResource<YImage>, Background, prBackImage, NULL)
};


//抽象窗口。
class AWindow : public Controls::AVisualControl, protected MWindow,
	virtual implements IWindow
{
public:
	typedef Controls::AVisualControl ParentType;

	//********************************
	//名称:		AWindow
	//全名:		YSLib::Components::Forms::AWindow::AWindow
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Rect &
	//形式参数:	const GHResource<YImage>
	//形式参数:	YDesktop *
	//形式参数:	HSHL
	//形式参数:	HWND
	//功能概要:	构造：使用指定边界、背景图像、桌面指针、 Shell 和 父窗口句柄。
	//备注:		
	//********************************
	explicit
	AWindow(const Rect& = Rect::Empty, const GHResource<YImage> = new YImage(),
		YDesktop* = ::YSLib::pDefaultDesktop,
		HSHL = ::YSLib::theApp.GetShellHandle(), HWND = NULL);
	virtual DefEmptyDtor(AWindow)

	ImplI(IWindow) DefConverterMember(Graphics, Buffer)

	ImplI(IWindow) DefPredicateBase(RefreshRequired, MWindow)
	ImplI(IWindow) DefPredicateBase(UpdateRequired, MWindow)

	//判断从属关系。
	PDefH(bool, BelongsTo, HSHL h) const
		ImplRet(hShell == h)
	PDefH(bool, BelongsTo, YDesktop* pDsk) const
		ImplBodyBase(MDesktopObject, BelongsTo, pDsk)

	ImplI(IWindow) DefGetterBase(YDesktop*, DesktopPtr, MDesktopObject)
	ImplI(IWindow) DefGetter(const Drawing::BitmapBuffer&, Buffer, Buffer) \
		//取显示缓冲区。
	ImplI(IWindow) DefGetterMember(BitmapPtr, BufferPtr, Buffer) \
		//取缓冲区指针。
	DefGetter(HWND, Handle, HWND(const_cast<AWindow*>(this)))
	DefGetterBase(HSHL, ShellHandle, MWindow)
	DefGetterBase(GHResource<YImage>, Background, MWindow)
	//********************************
	//名称:		GetBackgroundPtr
	//全名:		YSLib::Components::Forms::AWindow::GetBackgroundPtr
	//可访问性:	public 
	//返回类型:	YSLib::Components::BitmapPtr
	//修饰符:	const
	//功能概要:	取位图背景指针。
	//备注:		
	//********************************
	BitmapPtr
	GetBackgroundPtr() const;

	ImplI(IWindow) DefSetter(bool, Refresh, bRefresh)
	ImplI(IWindow) DefSetter(bool, Update, bUpdate)
	//********************************
	//名称:		SetSize
	//全名:		YSLib::Components::Forms::AWindow::SetSize
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Drawing::Size &
	//功能概要:	设置大小。
	//备注:		虚公共实现。
	//********************************
	virtual void
	SetSize(const Drawing::Size&);
	ImplI(IWindow) DefSetterBaseDe(GHResource<YImage>, Background, MWindow,
		NULL)

	PDefH(void, ClearBackground) const //清除背景。
		ImplExpr(Buffer.ClearImage())

	PDefH(void, BeFilledWith, PixelType c)
		ImplBodyMemberVoid(Buffer, BeFilledWith, c) //以纯色填充显示缓冲区。

protected:
	//********************************
	//名称:		DrawBackgroundImage
	//全名:		YSLib::Components::Forms::AWindow::DrawBackgroundImage
	//可访问性:	protected 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	绘制背景图像。
	//备注:		
	//********************************
	bool
	DrawBackgroundImage();

public:
	//********************************
	//名称:		DrawBackground
	//全名:		YSLib::Components::Forms::AWindow::DrawBackground
	//可访问性:	public 
	//返回类型:	ImplI(IWindow) void
	//修饰符:	
	//功能概要:	绘制背景。
	//备注:		
	//********************************
	ImplI(IWindow) void
	DrawBackground();

protected:
	ImplA(IWindow)
	DeclIEntry(bool DrawWidgets())

public:
	//********************************
	//名称:		Draw
	//全名:		YSLib::Components::Forms::AWindow::Draw
	//可访问性:	ImplI(IWindow) public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘图。
	//备注:		
	//********************************
	ImplI(IWindow) void
	Draw();

	//********************************
	//名称:		Refresh
	//全名:		YSLib::Components::Forms::AWindow::Refresh
	//可访问性:	public 
	//返回类型:	ImplI(IWindow) void
	//修饰符:	
	//功能概要:	刷新至窗口缓冲区。
	//备注:		
	//********************************
	ImplI(IWindow) void
	Refresh();

	//********************************
	//名称:		Update
	//全名:		YSLib::Components::Forms::AWindow::Update
	//可访问性:	ImplI(IWindow) public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	按需更新。
	//备注:		以父窗口、屏幕优先顺序。
	//********************************
	ImplI(IWindow) void
	Update();

	//********************************
	//名称:		RequestToTop
	//全名:		YSLib::Components::Forms::AWindow::RequestToTop
	//可访问性:	ImplI(IWindow) public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	请求提升至容器顶端。
	//备注:		
	//********************************
	ImplI(IWindow) void
	RequestToTop();

	//********************************
	//名称:		UpdateToScreen
	//全名:		YSLib::Components::Forms::AWindow::UpdateToScreen
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	const
	//功能概要:	更新至屏幕。
	//备注:		
	//********************************
	virtual void
	UpdateToScreen() const;

	//********************************
	//名称:		UpdateToWindow
	//全名:		YSLib::Components::Forms::AWindow::UpdateToWindow
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	const
	//功能概要:	更新至上层窗口缓冲区。
	//备注:		
	//********************************
	virtual void
	UpdateToWindow() const;

	//********************************
	//名称:		UpdateToScreen
	//全名:		YSLib::Components::Forms::AWindow::UpdateToScreen
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	const
	//形式参数:	YDesktop &
	//功能概要:	更新至指定桌面所在的屏幕。
	//备注:		
	//********************************
	void
	UpdateToScreen(YDesktop&) const;

	//********************************
	//名称:		UpdateToWindow
	//全名:		YSLib::Components::Forms::AWindow::UpdateToWindow
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	const
	//形式参数:	IWindow &
	//功能概要:	更新至指定窗口缓冲区。
	//备注:		
	//********************************
	void
	UpdateToWindow(IWindow&) const;

	//********************************
	//名称:		Show
	//全名:		YSLib::Components::Forms::AWindow::Show
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	显示窗口。
	//备注:		自动设置可见性和更新至屏幕。
	//********************************
	void
	Show();
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
class YFrameWindow : public GMCounter<YFrameWindow>, public YComponent,
	public AWindow, protected Widgets::MUIContainer
{
public:
	typedef YComponent ParentType;

	//********************************
	//名称:		YFrameWindow
	//全名:		YSLib::Components::Forms::YFrameWindow::YFrameWindow
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Rect &
	//形式参数:	const GHResource<YImage>
	//形式参数:	YDesktop *
	//形式参数:	HSHL
	//形式参数:	HWND
	//功能概要:	构造：使用指定边界、背景图像、桌面指针、 Shell 句柄和窗口句柄。
	//备注:		
	//********************************
	explicit
	YFrameWindow(const Rect& = Rect::Empty,
		const GHResource<YImage> = new YImage(),
		YDesktop* = ::YSLib::pDefaultDesktop,
		HSHL = ::YSLib::theApp.GetShellHandle(), HWND = NULL);
	//********************************
	//名称:		~YFrameWindow
	//全名:		YSLib::Components::Forms::YFrameWindow::~YFrameWindow
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	无异常抛出。
	//备注:		空实现。
	//********************************
	virtual
	~YFrameWindow() ythrow();

	virtual PDefHOperator(void, +=, IWidget& w)
		ImplExpr(sWgtSet += w)
	virtual PDefHOperator(bool, -=, IWidget& w)
		ImplRet(sWgtSet -= w)
	virtual PDefHOperator(void, +=, IVisualControl& c)
		ImplBodyBaseVoid(MUIContainer, operator+=, c)
	virtual PDefHOperator(bool, -=, IVisualControl& c)
		ImplBodyBase(MUIContainer, operator-=, c)
	virtual PDefHOperator(void, +=, GMFocusResponser<IVisualControl>& c)
		ImplBodyBaseVoid(MUIContainer, operator+=, c)
	virtual PDefHOperator(bool, -=, GMFocusResponser<IVisualControl>& c)
		ImplBodyBase(MUIContainer, operator-=, c)

	ImplI(IWindow) PDefH(IVisualControl*, GetFocusingPtr)
		ImplBodyBase(GMFocusResponser<IVisualControl>, GetFocusingPtr)
	ImplI(IWindow) PDefH(IWidget*, GetTopWidgetPtr, const Point& p)
		ImplBodyBase(MUIContainer, GetTopWidgetPtr, p)
	ImplI(IWindow) PDefH(IVisualControl*, GetTopVisualControlPtr, const Point& p)
		ImplBodyBase(MUIContainer, GetTopVisualControlPtr, p)

	ImplI(IWindow) PDefH(void, ClearFocusingPtr)
		ImplBodyBaseVoid(MUIContainer, ClearFocusingPtr)

	ImplI(IWindow) PDefH(bool, ResponseFocusRequest, AFocusRequester& w)
		ImplBodyBase(MUIContainer, ResponseFocusRequest, w)

	ImplI(IWindow) PDefH(bool, ResponseFocusRelease, AFocusRequester& w)
		ImplBodyBase(MUIContainer, ResponseFocusRelease, w)

protected:
	//********************************
	//名称:		DrawWidgets
	//全名:		YSLib::Components::Forms::YFrameWindow::DrawWidgets
	//可访问性:	virtual protected 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	绘制部件组。
	//备注:		
	//********************************
	virtual bool
	DrawWidgets();
};

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

