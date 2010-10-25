// YSLib::Shell::YGUIComponent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-10-04 21:23:32 + 08:00;
// UTime = 2010-10-25 13:10 + 08:00;
// Version = 0.1394;


#ifndef INCLUDED_YGUICOMP_H_
#define INCLUDED_YGUICOMP_H_

// YGUIComponent ：样式相关图形用户界面组件实现。

#include "ycontrol.h"
//#include "ystyle.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Widgets)

YSL_BEGIN_NAMESPACE(Controls)

//按钮： V0.2740 。
class YButton : public GMCounter<YButton>, public YVisualControl, public MButton, public Widgets::MLabel
{
public:
	typedef YVisualControl ParentType;

/*
	YImage BackgroundImage; //背景图像。
	YImage Image; //前景图像。
*/

	//********************************
	//名称:		YButton
	//全名:		YSLib::Components::Controls::YButton<_tChar>::YButton
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	HWND
	//形式参数:	const _tChar *
	//形式参数:	const Rect &
	//形式参数:	const Drawing::Font &
	//形式参数:	IWidgetContainer *
	//形式参数:	GHResource<Drawing::TextRegion>
	//功能概要:	构造：使用指定窗口句柄、字符串、边界、字体和部件容器指针。
	//备注:		
	//********************************
	template<class _tChar>
	YButton(HWND, const _tChar*, const Rect& = Rect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(), IWidgetContainer* = NULL,
		GHResource<Drawing::TextRegion> = NULL);

	virtual DefEmptyDtor(YButton)

protected:
	//********************************
	//名称:		_m_init
	//全名:		YSLib::Components::Controls::YButton::_m_init
	//可访问性:	protected 
	//返回类型:	void
	//修饰符:	
	//功能概要:	逻辑初始化：添加事件响应器。
	//备注:		保护实现。
	//********************************
	void
	_m_init();

	//********************************
	//名称:		DrawForeground
	//全名:		YSLib::Components::Controls::YButton::DrawForeground
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
	//名称:		OnEnter
	//全名:		YSLib::Components::Controls::YButton::OnEnter
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Runtime::MInputEventArgs &
	//功能概要:	响应进入控件事件。
	//备注:		
	//********************************
	virtual void
	OnEnter(const Runtime::MInputEventArgs&);

	//********************************
	//名称:		OnLeave
	//全名:		YSLib::Components::Controls::YButton::OnLeave
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Runtime::MInputEventArgs &
	//功能概要:	响应离开控件事件。
	//备注:		
	//********************************
	virtual void
	OnLeave(const Runtime::MInputEventArgs&);

	//********************************
	//名称:		OnKeyDown
	//全名:		YSLib::Components::Controls::YButton::OnKeyDown
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Runtime::MKeyEventArgs &
	//功能概要:	响应按键接触开始事件。
	//备注:		
	//********************************
	virtual void
	OnKeyDown(const Runtime::MKeyEventArgs&);

	//********************************
	//名称:		OnClick
	//全名:		YSLib::Components::Controls::YButton::OnClick
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Runtime::MTouchEventArgs &
	//功能概要:	响应屏幕点击事件。
	//备注:		
	//********************************
	virtual void
	OnClick(const Runtime::MTouchEventArgs&);
};

template<class _tChar>
YButton::YButton(HWND hWnd, const _tChar* l, const Rect& r,
	const Drawing::Font& f, IWidgetContainer* pCon,
	GHResource<Drawing::TextRegion> prTr_)
	: YVisualControl(hWnd, r, pCon), MButton(), MLabel(l, f, prTr_)
{
	_m_init();
}


//滚动条： V0.2125 。
class AScrollBar : public AVisualControl, public MScrollBar
{
public:
	typedef AVisualControl ParentType;

	explicit
	AScrollBar(HWND = NULL, const Rect& = Rect::Empty, IWidgetContainer* = NULL,
		SDST = 8, SDST = 16, SDST = 16);

	DefGetter(SDST, ScrollAreaSize, GetWidth() - GetPrevButtonSize() - GetNextButtonSize())
	DefGetter(SDST, ScrollAreaFixedSize, GetScrollAreaSize() - GetMinThumbSize())

protected:
	ImplA(IVisualControl) // with an inline default implementation;
	DeclIEntry(void DrawForeground())

public:
	//********************************
	//名称:		RequestToTop
	//全名:		YSLib::Components::Controls::AScrollBar::RequestToTop
	//可访问性:	ImplI(IVisualControl) public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	请求提升至容器顶端。
	//备注:		空实现。
	//********************************
	ImplI(IVisualControl) void
	RequestToTop()
	{}
/*
	DeclIEntry(void DrawPrevButton())

	DeclIEntry(void DrawNextButton())

	DeclIEntry(void DrawScrollArea())
*/
};

inline void
AScrollBar::DrawForeground()
{
	AVisualControl::DrawForeground();
}


//水平滚动条。
class YHorizontalScrollBar : public YComponent, public AScrollBar
{
public:
	typedef YComponent ParentType;

	explicit
	YHorizontalScrollBar(HWND, const Rect& = Rect::Empty, IWidgetContainer* = NULL,
		SDST = 8, SDST = 16, SDST = 16);

	virtual DefEmptyDtor(YHorizontalScrollBar)

protected:
	//********************************
	//名称:		DrawBackground
	//全名:		YSLib::Components::Controls::YHorizontalScrollBar::DrawBackground
	//可访问性:	protected 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制背景。
	//备注:		
	//********************************
	void
	DrawBackground();

	//********************************
	//名称:		DrawForeground
	//全名:		YSLib::Components::Controls::YHorizontalScrollBar::DrawForeground
	//可访问性:	protected 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制前景。
	//备注:		
	//********************************
	void
	DrawForeground();
};


//垂直滚动条。
class YVerticalScrollBar
{

};


//文本列表框： V0.1605 。
class YListBox : public GMCounter<YListBox>, public YVisualControl
{
public:
	typedef YVisualControl ParentType;
	typedef String ItemType; //项目类型：字符串。
	typedef vector<ItemType> ListType; //列表类型。
	typedef GSequenceViewer<ListType> ViewerType; //视图类型。

protected:
	static const SDST defMarginH = 4, defMarginV = 2;

	GHResource<Drawing::TextRegion> prTextRegion; //文本区域指针。
	const bool bDisposeList;

public:
	Drawing::Font Font; //字体。
	Drawing::Padding Margin; //文本和容器的间距。
	ListType& List; //文本列表。

protected:
	GSequenceViewer<ListType> Viewer; //列表视图。

public:
	DefEvent(IndexEventHandler, Selected) //项目选择状态改变事件。
	DefEvent(IndexEventHandler, Confirmed) //项目选中确定事件。

	//********************************
	//名称:		YListBox
	//全名:		YSLib::Components::Controls::YListBox::YListBox
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	HWND
	//形式参数:	const Rect &
	//形式参数:	IWidgetContainer *
	//形式参数:	GHResource<Drawing::TextRegion>
	//功能概要:	构造：使用指定窗口句柄、边界、部件容器指针和文本区域。
	//备注:		
	//********************************
	YListBox(HWND, const Rect& = Rect::Empty, IWidgetContainer* = NULL,
		GHResource<Drawing::TextRegion> = NULL);
	//********************************
	//名称:		YListBox
	//全名:		YSLib::Components::Controls::YListBox::YListBox
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	HWND
	//形式参数:	const Rect &
	//形式参数:	IWidgetContainer *
	//形式参数:	GHResource<Drawing::TextRegion>
	//形式参数:	ListType & List_
	//功能概要:	构造：使用指定窗口句柄、边界、部件容器指针、文本区域和文本列表。
	//备注:		使用外源列表。
	//********************************
	YListBox(HWND, const Rect& = Rect::Empty, IWidgetContainer* = NULL,
		GHResource<Drawing::TextRegion> = NULL,
		ListType& List_ = *GetGlobalResource<ListType>()); 
	//********************************
	//名称:		~YListBox
	//全名:		YSLib::Components::Controls::YListBox::~YListBox
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//********************************
	virtual
	~YListBox() ythrow();

protected:
	//********************************
	//名称:		_m_init
	//全名:		YSLib::Components::Controls::YListBox::_m_init
	//可访问性:	protected 
	//返回类型:	void
	//修饰符:	
	//功能概要:	逻辑初始化：添加事件响应器。
	//备注:		保护实现。
	//********************************
	void
	_m_init();

public:
	DefPredicateMember(Selected, Viewer)

	DefGetter(ListType&, List, List)
	DefGetterMember(ViewerType::IndexType, Index, Viewer)
	DefGetterMember(ViewerType::IndexType, Selected, Viewer)
	//********************************
	//名称:		GetItemPtr
	//全名:		YSLib::Components::Controls::YListBox::GetItemPtr
	//可访问性:	public 
	//返回类型:	ItemType*
	//修饰符:	
	//形式参数:	ViewerType::IndexType
	//功能概要:	取指定项目索引的项目指针。
	//备注:		
	//********************************
	ItemType*
	GetItemPtr(ViewerType::IndexType);
	//********************************
	//名称:		GetItemHeight
	//全名:		YSLib::Components::Controls::YListBox::GetItemHeight
	//可访问性:	public 
	//返回类型:	YSLib::SDST
	//修饰符:	const
	//功能概要:	取项目行高。
	//备注:		
	//********************************
	SDST
	GetItemHeight() const;

//	DefSetter(const ListType&, List, List)
	//********************************
	//名称:		SetSelected
	//全名:		YSLib::Components::Controls::YListBox::SetSelected
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	ViewerType::IndexType
	//功能概要:	按指定项目索引设置选中项目。
	//备注:		
	//********************************
	void
	SetSelected(ViewerType::IndexType);
	//********************************
	//名称:		SetSelected
	//全名:		YSLib::Components::Controls::YListBox::SetSelected
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	SPOS
	//形式参数:	SPOS
	//功能概要:	按接触点设置选中项目。
	//备注:		
	//********************************
	void
	SetSelected(SPOS, SPOS);
	//********************************
	//名称:		SetSelected
	//全名:		YSLib::Components::Controls::YListBox::SetSelected
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Point &
	//功能概要:	按接触点设置选中项目。
	//备注:		
	//********************************
	void
	SetSelected(const Point&);

protected:
	//********************************
	//名称:		DrawBackground
	//全名:		YSLib::Components::Controls::YListBox::DrawBackground
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
	//全名:		YSLib::Components::Controls::YListBox::DrawForeground
	//可访问性:	virtual protected 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制前景。
	//备注:		
	//********************************
	virtual void
	DrawForeground();

	//********************************
	//名称:		CheckPoint
	//全名:		YSLib::Components::Controls::YListBox::CheckPoint
	//可访问性:	protected 
	//返回类型:	ViewerType::IndexType
	//修饰符:	
	//形式参数:	SPOS
	//形式参数:	SPOS
	//功能概要:	检查相对于所在缓冲区的控件坐标是否在选择范围内，
	//			返回选择的项目索引。
	//备注:		
	//********************************
	ViewerType::IndexType
	CheckPoint(SPOS, SPOS);

public:
	PDefH(void, ClearSelected)
		ImplBodyMemberVoid(Viewer, ClearSelected)

private:
	//********************************
	//名称:		CallSelected
	//全名:		YSLib::Components::Controls::YListBox::CallSelected
	//可访问性:	private 
	//返回类型:	void
	//修饰符:	
	//功能概要:	调用选中事件响应器。
	//备注:		
	//********************************
	void
	CallSelected();

	//********************************
	//名称:		CallConfirmed
	//全名:		YSLib::Components::Controls::YListBox::CallConfirmed
	//可访问性:	private 
	//返回类型:	void
	//修饰符:	
	//功能概要:	调用确认事件响应器。
	//备注:		
	//********************************
	void
	CallConfirmed();

public:
	//********************************
	//名称:		OnKeyDown
	//全名:		YSLib::Components::Controls::YListBox::OnKeyDown
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Runtime::MKeyEventArgs &
	//功能概要:	响应按键接触开始事件。
	//备注:		
	//********************************
	virtual void
	OnKeyDown(const Runtime::MKeyEventArgs&);

	//********************************
	//名称:		OnTouchDown
	//全名:		YSLib::Components::Controls::YListBox::OnTouchDown
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Runtime::MTouchEventArgs &
	//功能概要:	响应屏幕接触开始事件。
	//备注:		
	//********************************
	virtual void
	OnTouchDown(const Runtime::MTouchEventArgs&);

	//********************************
	//名称:		OnClick
	//全名:		YSLib::Components::Controls::YListBox::OnClick
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Runtime::MTouchEventArgs &
	//功能概要:	响应屏幕点击事件。
	//备注:		
	//********************************
	virtual void
	OnClick(const Runtime::MTouchEventArgs&);

	//********************************
	//名称:		OnSelected
	//全名:		YSLib::Components::Controls::YListBox::OnSelected
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const MIndexEventArgs &
	//功能概要:	响应选中事件。
	//备注:		
	//********************************
	virtual void
	OnSelected(const MIndexEventArgs&);

	//********************************
	//名称:		OnConfirmed
	//全名:		YSLib::Components::Controls::YListBox::OnConfirmed
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const MIndexEventArgs &
	//功能概要:	响应确认事件。
	//备注:		
	//********************************
	virtual void
	OnConfirmed(const MIndexEventArgs&);
};

inline void
YListBox::SetSelected(const Point& pt)
{
	SetSelected(pt.X, pt.Y);
}


//文件列表框： V0.0854 。
class YFileBox : public GMCounter<YFileBox>, public YListBox, public IO::MFileList
{
public:
	typedef YListBox ParentType;
	typedef ParentType::ListType ListType;

	ListType& List;

	YFileBox(HWND, const Rect& = Rect::Empty, IWidgetContainer* = NULL, GHResource<Drawing::TextRegion> = NULL);
	virtual
	~YFileBox() ythrow();

	//********************************
	//名称:		GetPath
	//全名:		YSLib::Components::Controls::YFileBox::GetPath
	//可访问性:	public 
	//返回类型:	IO::Path
	//修饰符:	const
	//功能概要:	取当前路径。
	//备注:		
	//********************************
	IO::Path
	GetPath() const;

protected:
	//********************************
	//名称:		DrawBackground
	//全名:		YSLib::Components::Controls::YFileBox::DrawBackground
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
	//全名:		YSLib::Components::Controls::YFileBox::DrawForeground
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
	//名称:		OnTouchMove
	//全名:		YSLib::Components::Controls::YFileBox::OnTouchMove
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Runtime::MTouchEventArgs &
	//功能概要:	响应屏幕接触移动事件。
	//备注:		
	//********************************
	virtual void
	OnTouchMove(const Runtime::MTouchEventArgs&);

	//********************************
	//名称:		OnConfirmed
	//全名:		YSLib::Components::Controls::YFileBox::OnConfirmed
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const MIndexEventArgs &
	//功能概要:	响应确认事件。
	//备注:		
	//********************************
	virtual void
	OnConfirmed(const MIndexEventArgs&);
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

