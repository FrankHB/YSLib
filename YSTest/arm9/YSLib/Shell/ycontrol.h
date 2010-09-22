// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-2-18 13:44:24;
// UTime = 2010-9-22 3:20;
// Version = 0.3452;


#ifndef INCLUDED_YCONTROL_H_
#define INCLUDED_YCONTROL_H_

// YControl ：平台无关的控件实现。

#include "ywidget.h"
#include "ytext.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

//控件事件参数类型。
struct MIndexEventArgs : public MEventArgs
{
	typedef std::ptrdiff_t IndexType;

	IVisualControl& Control;
	IndexType Index;

	MIndexEventArgs(IVisualControl& c, IndexType i)
	: MEventArgs(),
	Control(c), Index(i)
	{}
};


//控件事件类型。
typedef Runtime::GEvent<true, IControl, MEventArgs> YControlEvent;


//事件处理器类型。
DefDelegate(YTouchEventHandler, IVisualControl, Runtime::MTouchEventArgs)
DefDelegate(YKeyEventHandler, IVisualControl, Runtime::MKeyEventArgs)
DefDelegate(YIndexEventHandler, IVisualControl, MIndexEventArgs)


//可视控件事件空间。
struct EControl
{
	typedef enum EventSpace
	{
	//	AutoSizeChanged,
	//	BackColorChanged,
	//	ForeColorChanged,
	//	LocationChanged,
	//	MarginChanged,
	//	VisibleChanged,

	//	EnabledChanged,
	//	Resize,
	//	Move,

		GotFocus,
		LostFocus,

	//	TextChanged,
	//	FontChanged,
	//	FontColorChanged,
	//	FontSizeChanged,
	} EventID;
};


//控件接口。
DeclInterface(IControl)
	typedef Runtime::GEventMap<EControl, YControlEvent> EventMapType;

	DeclIEntry(EventMapType::Event& operator[](const EventMapType::ID&))

	DeclIEntry(bool IsEnabled() const) //判断是否有效。

	DeclIEntry(void SetEnabled(bool)) //设置有效性。
EndDecl


//可视控件接口。
DeclBasedInterface(IVisualControl, IControl, Runtime::GIFocusRequester<IVisualControl>)
	DeclIEntry(void RequestFocus(const MEventArgs& = GetZeroElement<MEventArgs>()))
	DeclIEntry(void ReleaseFocus(const MEventArgs& = GetZeroElement<MEventArgs>()))
EndDecl


//控件模块类。
class MControl// : implements IControl
{
protected:
	bool Enabled; //控件有效性。
	IControl::EventMapType EventMap; //事件映射表。

public:
	explicit
	MControl(bool = true);

	virtual
	~MControl()
	{}

	virtual PDefOpHead(IControl::EventMapType::Event&, [], const IControl::EventMapType::ID& id)
		ImplRet(EventMap[id])

	virtual DefBoolGetter(Enabled, Enabled)

	virtual DefSetterDe(bool, Enabled, Enabled, true)
};

inline
MControl::MControl(bool e)
: Enabled(e), EventMap()
{}


//可视控件模块类。
class MVisualControl : public MControl, public Runtime::AFocusRequester
{
public:
	Drawing::PixelType BackColor; //默认背景色。
	Drawing::PixelType ForeColor; //默认前景色。

	DefEvent(YTouchEventHandler, TouchUp)
	DefEvent(YTouchEventHandler, TouchDown)
	DefEvent(YTouchEventHandler, TouchHeld)
	DefEvent(YTouchEventHandler, TouchMove)
	DefEvent(YTouchEventHandler, Click)
	DefEvent(YKeyEventHandler, KeyUp)
	DefEvent(YKeyEventHandler, KeyDown)
	DefEvent(YKeyEventHandler, KeyHeld)
	DefEvent(YKeyEventHandler, KeyPress)

	explicit
	MVisualControl(Drawing::PixelType = Drawing::Color::Black, Drawing::PixelType = Drawing::Color::White);
	virtual
	~MVisualControl()
	{}

protected:
	Runtime::GMFocusResponser<IVisualControl>*
	CheckFocusContainer(IWidgetContainer*); //检查给定的容器指针是否有效且指向接受焦点的容器。

private:
	void
	_m_OnTouchHeld(const Runtime::MTouchEventArgs&);
	void
	_m_OnTouchMove(const Runtime::MTouchEventArgs&);

public:
	static void
	OnGotFocus(IControl&, const MEventArgs& = GetZeroElement<MEventArgs>());
	static void
	OnLostFocus(IControl&, const MEventArgs& = GetZeroElement<MEventArgs>());
	static void
	OnTouchDown(IVisualControl&, const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
	static void
	OnTouchHeld(IVisualControl&, const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
	static void
	OnTouchMove(IVisualControl&, const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
};


//控件基类。
class YControl : public YComponent, public MControl
{
public:
	typedef YComponent ParentType;
};


//可视控件基类。
class YVisualControl : public YComponent, public Widgets::MWidget, public MVisualControl,
	implements IWidget, implements IVisualControl
{
public:
	typedef YComponent ParentType;

	explicit
	YVisualControl(HWND = NULL, const SRect& = SRect::FullScreen, IWidgetContainer* = NULL);
	~YVisualControl();

	virtual PDefHead(EventMapType::Event&, operator[], const EventMapType::ID& id)
		ImplBodyBase(MVisualControl, operator[], id)

	virtual DefBoolGetterBase(Visible, MVisual)
	virtual DefBoolGetterBase(Transparent, MVisual)
	virtual DefBoolGetterBase(BgRedrawed, MVisual)
	virtual DefBoolGetterBase(Enabled, MControl)
	virtual DefBoolGetterBase(Focused, AFocusRequester)
	virtual PDefHead(bool, IsFocusOfContainer, Runtime::GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(AFocusRequester, IsFocusOfContainer, c)

	//判断包含关系。
	virtual PDefHead(bool, Contains, const SPoint& p) const
		ImplBodyBase(MVisual, Contains, p)

	virtual PDefHead(bool, CheckRemoval, Runtime::GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(MVisualControl, CheckRemoval, c)

	virtual DefGetterBase(const SPoint&, Location, MVisual)
	virtual DefGetterBase(const SSize&, Size, MVisual)
	virtual DefGetterBase(IWidgetContainer*, ContainerPtr, MWidget)
	virtual DefGetterBase(HWND, WindowHandle, MWidget)

	virtual DefSetterBaseDe(bool, Visible, MVisual, true)
	virtual DefSetterBaseDe(bool, Transparent, MVisual, true)
	virtual DefSetterBaseDe(bool, BgRedrawed, MVisual, true)
	virtual DefSetterBase(const SPoint&, Location, MVisual)
	virtual DefSetterBaseDe(bool, Enabled, MControl, true)

	virtual void
	DrawBackground();
	virtual void
	DrawForeground();

	virtual PDefHead(void, Refresh)
		ImplBodyBaseVoid(MWidget, Refresh)

	virtual void
	RequestFocus(const MEventArgs& = GetZeroElement<MEventArgs>()); //向部件容器申请获得焦点，若成功则引发 GotFocus 事件。
	virtual void
	ReleaseFocus(const MEventArgs& = GetZeroElement<MEventArgs>()); //释放焦点，并引发 LostFocus 事件。
	virtual void
	RequestToTop()
	{}
};


//V0.1600;
//标签。
class YLabel : public GMCounter<YLabel>, public YVisualControl
{
public:
	typedef YVisualControl ParentType;

protected:
	GHResource<Drawing::TextRegion> prTextRegion; //文本区域指针。

public:
	Drawing::Font Font; //字体。
	Drawing::Padding Margin; //文本和容器的间距。
	bool AutoSize; //启用根据字号自动调整大小。
	bool AutoEllipsis; //启用对超出标签宽度的文本调整大小。
	String Text; //标签文本。
/*
	YImage BackgroundImage; //背景图像。
	YImage Image; //前景图像。
*/

	//用字符串在窗口中以给定字号初始化标签。
	template<class _tChar>
	YLabel(HWND, const _tChar*, const SRect& = SRect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(), IWidgetContainer* = NULL, GHResource<Drawing::TextRegion> = NULL);
	virtual
	~YLabel();

	virtual void
	DrawForeground();
};

template<class _tChar>
YLabel::YLabel(HWND hWnd, const _tChar* l, const SRect& r,
			   const Drawing::Font& f, IWidgetContainer* pCon, GHResource<Drawing::TextRegion> prTr_)
: YVisualControl(hWnd, r, pCon),
prTextRegion(pCon ? prTr_ : GetGlobalResource<Drawing::TextRegion>()), Font(),
Margin(prTextRegion->Margin), AutoSize(true), AutoEllipsis(false), Text(l)
{}


//V0.1544E;
//文本列表框。
class YListBox : public GMCounter<YListBox>, public YVisualControl
{
public:
	typedef YVisualControl ParentType;
	typedef String ItemType; //项目类型：字符串。
	typedef std::vector<ItemType> ListType; //列表类型。
	typedef GSequenceViewer<ListType> ViewerType; //视图类型。

protected:
	static const SDST defMarginH = 4, defMarginV = 2;

	GHResource<Drawing::TextRegion> prTextRegion; //文本区域指针。
	const bool bDisposeList;

public:
	Drawing::Font Font; //字体。
	Drawing::Padding Margin; //文本和容器的间距。
	ListType& List; //列表。

protected:
	GSequenceViewer<ListType> Viewer; //列表视图。

public:
	DefEvent(YIndexEventHandler, Selected) //项目选择状态改变事件。
	DefEvent(YIndexEventHandler, Confirmed) //项目选中确定事件。

	YListBox(HWND, const SRect& = SRect::Empty, IWidgetContainer* = NULL, GHResource<Drawing::TextRegion> = NULL);
	YListBox(HWND, const SRect& = SRect::Empty, IWidgetContainer* = NULL, GHResource<Drawing::TextRegion> = NULL, ListType& List_ = *GetGlobalResource<ListType>()); //外源列表。
	virtual
	~YListBox();

protected:
	void
	Init_();

public:
	DefBoolGetterMember(Selected, Viewer)

	DefGetter(ListType&, List, List)
	DefGetterMember(ViewerType::IndexType, Index, Viewer)
	DefGetterMember(ViewerType::IndexType, Selected, Viewer)
	ItemType*
	GetItemPtr(ViewerType::IndexType);
	SDST
	GetItemHeight() const;

//	DefSetter(const ListType&, List, List)
	void
	SetSelected(ViewerType::IndexType);
	void
	SetSelected(SPOS, SPOS);
	void
	SetSelected(const SPoint&);

public:
	virtual void
	DrawBackground();
	virtual void
	DrawForeground();

protected:
	ViewerType::IndexType
	CheckPoint(SPOS, SPOS); //检查相对于所在缓冲区的控件坐标是否在选择范围内，返回选择的项目索引。

public:
	void
	ClearSelected();

private:
	void
	CallSelected();
	void
	CallConfirmed();
	void
	_m_OnClick(const Runtime::MTouchEventArgs&);
	void
	_m_OnKeyPress(const Runtime::MKeyEventArgs&);

public:
	static void
	OnClick(IVisualControl&, const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
	static void
	OnKeyPress(IVisualControl&, const Runtime::MKeyEventArgs& = Runtime::MKeyEventArgs::Empty);
	static void
	OnSelected(IVisualControl&, const MIndexEventArgs&);
	static void
	OnConfirmed(IVisualControl&, const MIndexEventArgs&);
};

inline void
YListBox::SetSelected(const SPoint& pt)
{
	SetSelected(pt.X, pt.Y);
}

inline void
YListBox::ClearSelected()
{
	Viewer.ClearSelected();
}


//V0.0664E;
//文件列表框。
class YFileBox : public GMCounter<YFileBox>, public YListBox, public IO::MFileList
{
public:
	typedef YListBox ParentType;
	typedef ParentType::ListType ListType;

	ListType& List;

	YFileBox(HWND, const SRect& = SRect::Empty, IWidgetContainer* = NULL, GHResource<Drawing::TextRegion> = NULL);
	virtual
	~YFileBox();

	virtual void
	DrawBackground();
	virtual void
	DrawForeground();

	static void
	OnConfirmed(IVisualControl&, const MIndexEventArgs&);
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

