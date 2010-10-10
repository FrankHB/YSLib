// YSLib::Shell::YGUIComponent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-10-04 21:23:32 + 08:00;
// UTime = 2010-10-09 10:39 + 08:00;
// Version = 0.1082;


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

//按钮：V0.2680。
class YButton : public GMCounter<YButton>, public YVisualControl, public AButton
{
public:
	typedef YVisualControl ParentType;

/*
	YImage BackgroundImage; //背景图像。
	YImage Image; //前景图像。
*/

	//用字符串在窗口中以给定字号初始化标签。
	template<class _tChar>
	YButton(HWND, const _tChar*, const Rect& = Rect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(), IWidgetContainer* = NULL, GHResource<Drawing::TextRegion> = NULL);

protected:
	void
	_m_init();

public:
	virtual void
	DrawForeground();

	virtual void
	OnEnter(const Runtime::MInputEventArgs&);
	virtual void
	OnLeave(const Runtime::MInputEventArgs&);
	virtual void
	OnClick(const Runtime::MTouchEventArgs&);
	virtual void
	OnKeyDown(const Runtime::MKeyEventArgs&);
	virtual void
	OnConfirmed(const MIndexEventArgs&);
};

template<class _tChar>
YButton::YButton(HWND hWnd, const _tChar* l, const Rect& r,
	const Drawing::Font& f, IWidgetContainer* pCon, GHResource<Drawing::TextRegion> prTr_)
	: YVisualControl(hWnd, r, pCon), AButton(l, f, prTr_)
{
	_m_init();
}


//文本列表框：V0.1605E。
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
	ListType& List; //列表。

protected:
	GSequenceViewer<ListType> Viewer; //列表视图。

public:
	DefEvent(YIndexEventHandler, Selected) //项目选择状态改变事件。
	DefEvent(YIndexEventHandler, Confirmed) //项目选中确定事件。

	YListBox(HWND, const Rect& = Rect::Empty, IWidgetContainer* = NULL, GHResource<Drawing::TextRegion> = NULL);
	YListBox(HWND, const Rect& = Rect::Empty, IWidgetContainer* = NULL, GHResource<Drawing::TextRegion> = NULL, ListType& List_ = *GetGlobalResource<ListType>()); //外源列表。
	virtual
	~YListBox();

protected:
	void
	_m_init();

public:
	DefPredicateMember(Selected, Viewer)

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
	SetSelected(const Point&);

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

public:
	virtual void
	OnTouchDown(const Runtime::MTouchEventArgs&);
	virtual void
	OnClick(const Runtime::MTouchEventArgs&);
	virtual void
	OnKeyDown(const Runtime::MKeyEventArgs&);
	virtual void
	OnSelected(const MIndexEventArgs&);
	virtual void
	OnConfirmed(const MIndexEventArgs&);
};

inline void
YListBox::SetSelected(const Point& pt)
{
	SetSelected(pt.X, pt.Y);
}

inline void
YListBox::ClearSelected()
{
	Viewer.ClearSelected();
}


//文件列表框：V0.0854E。
class YFileBox : public GMCounter<YFileBox>, public YListBox, public IO::MFileList
{
public:
	typedef YListBox ParentType;
	typedef ParentType::ListType ListType;

	ListType& List;

	YFileBox(HWND, const Rect& = Rect::Empty, IWidgetContainer* = NULL, GHResource<Drawing::TextRegion> = NULL);
	virtual
	~YFileBox();

	IO::Path
	GetPath() const;

	virtual void
	DrawBackground();
	virtual void
	DrawForeground();

	virtual void
	OnTouchMove(const Runtime::MTouchEventArgs&);
	virtual void
	OnConfirmed(const MIndexEventArgs&);
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

