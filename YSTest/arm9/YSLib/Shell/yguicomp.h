// YSLib::Shell::YGUIComponent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-10-04 21:23:32 + 08:00;
// UTime = 2010-11-08 20:15 + 08:00;
// Version = 0.1748;


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

//基本按钮/滑块。
class YThumb : public GMCounter<YThumb>, public YVisualControl,
	protected MButton
{
public:
	typedef YVisualControl ParentType;

	//********************************
	//名称:		YThumb
	//全名:		YSLib::Components::Controls::YThumb::YThumb
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
	YThumb(HWND = NULL, const Rect& = Rect::FullScreen, IUIBox* = NULL);
	virtual DefEmptyDtor(YThumb)

	//********************************
	//名称:		DrawForeground
	//全名:		YSLib::Components::Controls::YThumb::DrawForeground
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制前景。
	//备注:		
	//********************************
	virtual void
	DrawForeground();

	//********************************
	//名称:		OnEnter
	//全名:		YSLib::Components::Controls::YThumb::OnEnter
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	InputEventArgs &
	//功能概要:	响应进入控件事件。
	//备注:		
	//********************************
	void
	OnEnter(InputEventArgs&);

	//********************************
	//名称:		OnLeave
	//全名:		YSLib::Components::Controls::YThumb::OnLeave
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	InputEventArgs &
	//功能概要:	响应离开控件事件。
	//备注:		
	//********************************
	void
	OnLeave(InputEventArgs&);
};


//按钮。
class YButton : public GMCounter<YButton>, public YThumb, public Widgets::MLabel
{
public:
	typedef YThumb ParentType;

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
	//形式参数:	IUIBox *
	//形式参数:	GHResource<Drawing::TextRegion>
	//功能概要:	构造：使用指定窗口句柄、字符串、边界、字体和部件容器指针。
	//备注:		
	//********************************
	template<class _tChar>
	YButton(HWND, const _tChar*, const Rect& = Rect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(), IUIBox* = NULL,
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

public:
	//********************************
	//名称:		DrawForeground
	//全名:		YSLib::Components::Controls::YButton::DrawForeground
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制前景。
	//备注:		
	//********************************
	virtual void
	DrawForeground();

	//********************************
	//名称:		OnKeyDown
	//全名:		YSLib::Components::Controls::YButton::OnKeyDown
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	KeyEventArgs &
	//功能概要:	响应按键接触开始事件。
	//备注:		
	//********************************
	void
	OnKeyDown(KeyEventArgs&);

	//********************************
	//名称:		OnClick
	//全名:		YSLib::Components::Controls::YButton::OnClick
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	TouchEventArgs &
	//功能概要:	响应屏幕点击事件。
	//备注:		
	//********************************
	void
	OnClick(TouchEventArgs&);
};

template<class _tChar>
YButton::YButton(HWND hWnd, const _tChar* l, const Rect& r,
	const Drawing::Font& f, IUIBox* pCon,
	GHResource<Drawing::TextRegion> prTr_)
	: YThumb(hWnd, r, pCon),
	MLabel(l, f, prTr_)
{
	_m_init();
}


//轨道。
class ATrack : public AVisualControl, public GMFocusResponser<IVisualControl>,
	implements IUIBox
{
public:
	typedef AVisualControl ParentType;

	struct EArea
	{
	public:
		typedef enum
		{
			None = 0,
			OnThumb = 1,
			OnPrev = 2,
			OnNext = 3
		} Area;

	private:
		Area value;

	public:
		template<typename _type>
		inline
		EArea(_type v)
			: value(v)
		{}

		DefConverter(int, static_cast<int>(value))
	};

protected:
	SDST MinThumbLength;
	YThumb Thumb; //滑块。
	IVisualControl* pFocusing;

public:
	explicit
	ATrack(HWND = NULL, const Rect& = Rect::FullScreen, IUIBox* = NULL,
		SDST = 8);
	virtual DefEmptyDtor(ATrack)

	ImplI(IUIBox) PDefH(IVisualControl*, GetFocusingPtr)
		ImplRet(pFocusing)
	ImplI(IUIBox) IWidget*
	GetTopWidgetPtr(const Point&);
	ImplI(IUIBox) IVisualControl*
	GetTopVisualControlPtr(const Point&);
	DefGetter(SDST, MinThumbLength, MinThumbLength)

	DeclIEntry(SDST GetThumbLength() const) //取滑块长度。
	DeclIEntry(SDST GetThumbPosition() const) //取滑块位置。

	DeclIEntry(void SetThumbLength(SDST)) //设置滑块长度。
	DeclIEntry(void SetThumbPosition(SDST)) //设置滑块位置。

	ImplI(IUIBox) void
	ClearFocusingPtr();

	ImplI(IUIBox) bool
	ResponseFocusRequest(AFocusRequester&);

	ImplI(IUIBox) bool
	ResponseFocusRelease(AFocusRequester&);

	//********************************
	//名称:		DrawForeground
	//全名:		YSLib::Components::Controls::ATrack::DrawForeground
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制前景。
	//备注:		
	//********************************
	virtual void
	DrawForeground();

	ImplI(IVisualControl) void
	RequestToTop()
	{}

protected:
	EArea
	CheckArea(SDST) const;

	void
	ResponseTouchDown(SDST);
};


//水平轨道。
class YHorizontalTrack : public GMCounter<YHorizontalTrack>, public YComponent,
	public ATrack
{
public:
	explicit
	YHorizontalTrack(HWND = NULL, const Rect& = Rect::FullScreen,
		IUIBox* = NULL);
	virtual DefEmptyDtor(YHorizontalTrack)

	ImplI(ATrack) DefGetter(SDST, ThumbLength, Thumb.GetWidth())
	ImplI(ATrack) DefGetter(SDST, ThumbPosition, Thumb.GetLocation().X)

	ImplI(ATrack) void
	SetThumbLength(SDST);
	ImplI(ATrack) void
	SetThumbPosition(SDST);

	void
	OnTouchDown(TouchEventArgs&);

	void
	OnDrag_Thumb(TouchEventArgs&);
};


//垂直轨道。
class YVerticalTrack : public GMCounter<YVerticalTrack>, public YComponent,
	public ATrack
{
public:
	explicit
	YVerticalTrack(HWND = NULL, const Rect& = Rect::FullScreen,
		IUIBox* = NULL);
	virtual DefEmptyDtor(YVerticalTrack)

	ImplI(ATrack) DefGetter(SDST, ThumbLength, Thumb.GetHeight())
	ImplI(ATrack) DefGetter(SDST, ThumbPosition, Thumb.GetLocation().Y)

	ImplI(ATrack) void
	SetThumbLength(SDST);
	ImplI(ATrack) void
	SetThumbPosition(SDST);

	void
	OnTouchDown(TouchEventArgs&);

	void
	OnDrag_Thumb(TouchEventArgs&);
};


//滚动条模块。
class MScrollBar
{
public:
	typedef enum
	{
		None = 0,
		PrevButton = 1,
		NextButton = 2,
		PrevTrack = 3,
		NextTrack = 4,
		Thumb = 5
	} CompIndex;

protected:
	YThumb Prev, Next;
	CompIndex Pressed; //按键状态：是否处于按下状态。

	//********************************
	//名称:		MScrollBar
	//全名:		YSLib::Components::Controls::MScrollBar::MScrollBar
	//可访问性:	protected 
	//返回类型:	
	//修饰符:	
	//形式参数:	SDST
	//形式参数:	const Rect &
	//形式参数:	const Rect &
	//功能概要:	构造：使用指定滑块长度、前按钮边界和后按钮边界。
	//备注:		
	//********************************
	MScrollBar(HWND, IUIBox*, const Rect&, const Rect&);

public:
	DefGetter(CompIndex, PressedState, Pressed)
};


//滚动条。
class AScrollBar : public ATrack, protected MScrollBar
{
public:
	typedef ATrack ParentType;

public:
	explicit
	AScrollBar(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL,
		SDST = 8, const Rect& = Rect::Empty, const Rect& = Rect::Empty);
	DefEmptyDtor(AScrollBar)

	DeclIEntry(SDST& GetWidgetLengthRef()) \
		//取滚动条方向的长度引用。
	DeclIEntry(SDST& GetThumbLengthRef()) //取滑块长度引用。
	DeclIEntry(SDST GetWidgetLength() const)
	DeclIEntry(SDST GetPrevButtonLength() const)
	DeclIEntry(SDST GetNextButtonLength() const)
	DefGetter(SDST, ScrollAreaLength,
		GetWidgetLength() - GetPrevButtonLength() - GetNextButtonLength())
	DefGetter(SDST, ScrollAreaFixedLength,
		GetScrollAreaLength() - GetMinThumbLength())

	ImplA(IVisualControl) // with an inline default implementation;
	DeclIEntry(void DrawForeground())

	ImplA(ATrack)
	DeclIEntry(void SetThumbPosition())

	//********************************
	//名称:		SetThumbSize
	//全名:		YSLib::Components::Controls::MScrollBar::SetThumbLength
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	SDST
	//功能概要:	设置滑块在滚动条方向的长度。
	//备注:		
	//********************************
	void
	SetThumbLength(SDST);

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
	typedef u16 ValueType;

	explicit
	YHorizontalScrollBar(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL,
		SDST = 8, SDST = 16, SDST = 16);

	virtual DefEmptyDtor(YHorizontalScrollBar)

public:
	ImplI(AScrollBar) DefGetter(SDST, WidgetLength, GetSize().Width);

	//********************************
	//名称:		DrawBackground
	//全名:		YSLib::Components::Controls::YHorizontalScrollBar
	//				::DrawBackground
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
	//全名:		YSLib::Components::Controls::YHorizontalScrollBar
	//				::DrawForeground
	//可访问性:	ImplI(AScrollBar) public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制前景。
	//备注:		
	//********************************
	ImplI(AScrollBar) void
	DrawForeground();
};


//垂直滚动条。
class YVerticalScrollBar
{

};


//文本列表框。
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
	//形式参数:	IUIBox *
	//形式参数:	GHResource<Drawing::TextRegion>
	//功能概要:	构造：使用指定窗口句柄、边界、部件容器指针和文本区域。
	//备注:		
	//********************************
	explicit
	YListBox(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL,
		GHResource<Drawing::TextRegion> = NULL);
	//********************************
	//名称:		YListBox
	//全名:		YSLib::Components::Controls::YListBox::YListBox
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	HWND
	//形式参数:	const Rect &
	//形式参数:	IUIBox *
	//形式参数:	GHResource<Drawing::TextRegion>
	//形式参数:	ListType & List_
	//功能概要:	构造：使用指定窗口句柄、边界、部件容器指针、文本区域和文本列表。
	//备注:		使用外源列表。
	//********************************
	explicit
	YListBox(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL,
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

	//********************************
	//名称:		DrawBackground
	//全名:		YSLib::Components::Controls::YListBox::DrawBackground
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
	//全名:		YSLib::Components::Controls::YListBox::DrawForeground
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制前景。
	//备注:		
	//********************************
	virtual void
	DrawForeground();

protected:
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
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	KeyEventArgs &
	//功能概要:	响应按键接触开始事件。
	//备注:		
	//********************************
	void
	OnKeyDown(KeyEventArgs&);

	//********************************
	//名称:		OnTouchDown
	//全名:		YSLib::Components::Controls::YListBox::OnTouchDown
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	TouchEventArgs &
	//功能概要:	响应屏幕接触开始事件。
	//备注:		
	//********************************
	void
	OnTouchDown(TouchEventArgs&);

	//********************************
	//名称:		OnTouchMove
	//全名:		YSLib::Components::Controls::YListBox::OnTouchMove
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	TouchEventArgs &
	//功能概要:	响应屏幕接触移动事件。
	//备注:		
	//********************************
	void
	OnTouchMove(TouchEventArgs&);

	//********************************
	//名称:		OnClick
	//全名:		YSLib::Components::Controls::YListBox::OnClick
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	TouchEventArgs &
	//功能概要:	响应屏幕点击事件。
	//备注:		
	//********************************
	void
	OnClick(TouchEventArgs&);

	//********************************
	//名称:		OnSelected
	//全名:		YSLib::Components::Controls::YListBox::OnSelected
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	IndexEventArgs &
	//功能概要:	响应选中事件。
	//备注:		
	//********************************
	void
	OnSelected(IndexEventArgs&);

	//********************************
	//名称:		OnConfirmed
	//全名:		YSLib::Components::Controls::YListBox::OnConfirmed
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	IndexEventArgs &
	//功能概要:	响应确认事件。
	//备注:		
	//********************************
	void
	OnConfirmed(IndexEventArgs&);
};

inline void
YListBox::SetSelected(const Point& pt)
{
	SetSelected(pt.X, pt.Y);
}


//文件列表框。
class YFileBox : public GMCounter<YFileBox>, public YListBox,
	public IO::MFileList
{
public:
	typedef YListBox ParentType;
	typedef ParentType::ListType ListType;

	ListType& List;

	explicit
	YFileBox(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL,
		GHResource<Drawing::TextRegion> = NULL);
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

	//********************************
	//名称:		DrawBackground
	//全名:		YSLib::Components::Controls::YFileBox::DrawBackground
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
	//全名:		YSLib::Components::Controls::YFileBox::DrawForeground
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制前景。
	//备注:		
	//********************************
	virtual void
	DrawForeground();

	//********************************
	//名称:		OnConfirmed
	//全名:		YSLib::Components::Controls::YFileBox::OnConfirmed
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	IndexEventArgs &
	//功能概要:	响应确认事件。
	//备注:		
	//********************************
	void
	OnConfirmed(IndexEventArgs&);
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

