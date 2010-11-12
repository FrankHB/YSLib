/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yguicomp.h
\ingroup Shell
\brief 样式相关图形用户界面组件实现。
\version 0.2042;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-04 21:23:32 + 08:00;
\par 修改时间:
	2010-11-12 18:54 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YGUIComponent;
*/


#ifndef INCLUDED_YGUICOMP_H_
#define INCLUDED_YGUICOMP_H_

#include "ycontrol.h"
//#include "ystyle.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Widgets)

YSL_BEGIN_NAMESPACE(Controls)

//! \brief 基本按钮/滑块。
class YThumb : public GMCounter<YThumb>, public YVisualControl,
	protected MButton
{
public:
	typedef YVisualControl ParentType;

	/*!
	\brief 构造：使用指定窗口句柄、边界和部件容器指针。
	*/
	explicit
	YThumb(HWND = NULL, const Rect& = Rect::FullScreen, IUIBox* = NULL);
	virtual DefEmptyDtor(YThumb)

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();

	/*!
	\brief 响应进入控件事件。
	*/
	void
	OnEnter(InputEventArgs&);

	/*!
	\brief 响应离开控件事件。
	*/
	void
	OnLeave(InputEventArgs&);
};


//! \brief 按钮。
class YButton : public GMCounter<YButton>, public YThumb, public Widgets::MLabel
{
public:
	typedef YThumb ParentType;

/*
	YImage BackgroundImage; //!< 背景图像。
	YImage Image; //!< 前景图像。
*/

	/*!
	\brief 构造：使用指定窗口句柄、边界、字体和部件容器指针。
	*/
	explicit
	YButton(HWND = NULL, const Rect& = Rect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(), IUIBox* = NULL,
		GHResource<Drawing::TextRegion> = NULL);
	virtual DefEmptyDtor(YButton)

protected:
	/*!
	\brief 逻辑初始化：添加事件响应器。
	\note 保护实现。
	*/
	void
	_m_init();

public:
	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();

	/*!
	\brief 响应按键接触开始事件。
	*/
	void
	OnKeyDown(KeyEventArgs&);

	/*!
	\brief 响应屏幕点击事件。
	*/
	void
	OnClick(TouchEventArgs&);
};


//! \brief 轨道。
class ATrack : public AVisualControl, public GMFocusResponser<IVisualControl>,
	implements IUIBox
{
public:
	typedef AVisualControl ParentType;

	//! \brief 轨道区域。
	typedef enum
		{
		None = 0,
		OnThumb = 1,
		OnPrev = 2,
		OnNext = 3
	} Area;

protected:
	SDST MinThumbLength;
	YThumb Thumb; //!< 滑块。
	IVisualControl* pFocusing;

public:
	/*!
	\brief 构造：使用指定窗口句柄、边界、部件容器指针和大小。
	*/
	explicit
	ATrack(HWND = NULL, const Rect& = Rect::FullScreen, IUIBox* = NULL,
		SDST = 8);
	virtual DefEmptyDtor(ATrack)

	DefPredicate(Horizontal, GetOrientation() == Widgets::Horizontal)
	DefPredicate(Vertical, GetOrientation() == Widgets::Vertical)

	ImplI(IUIBox) PDefH(IVisualControl*, GetFocusingPtr)
		ImplRet(pFocusing)
	/*!
	\brief 取顶端部件指针。
	\note 由顶端可视部件指针转换。
	*/
	ImplI(IUIBox) IWidget*
	GetTopWidgetPtr(const Point&);
	/*!
	\brief 取顶端可视部件指针。
	\note 仅滑块。
	*/
	ImplI(IUIBox) IVisualControl*
	GetTopVisualControlPtr(const Point&);
	DefGetter(SDST, MinThumbLength, MinThumbLength)
	DeclIEntry(Widgets::Orientation GetOrientation() const) //!< 取轨道方向。
	DefGetter(SDST, ThumbLength,
		SelectFrom(Thumb.GetSize(), IsHorizontal())) //!< 取轨道方向上的滑块长度。
	DefGetter(SDST, ThumbPosition,
		SelectFrom(Thumb.GetLocation(), IsHorizontal())) //!< 取滑块位置。
	virtual DefGetter(SDST, TrackLength,
		SelectFrom(GetSize(), IsHorizontal())) //!< 取轨道方向上的轨道长度。

	/*!
	\brief 设置轨道方向上的滑块长度。
	*/
	virtual void
	SetThumbLength(SDST);
	/*!
	\brief 设置滑块位置。
	*/
	virtual void
	SetThumbPosition(SDST);

	/*!
	\brief 清除焦点指针。
	*/
	ImplI(IUIBox) void
	ClearFocusingPtr();

	/*!
	\brief 响应焦点请求。
	*/
	ImplI(IUIBox) bool
	ResponseFocusRequest(AFocusRequester&);

	/*!
	\brief 响应焦点释放。
	*/
	ImplI(IUIBox) bool
	ResponseFocusRelease(AFocusRequester&);

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();

	/*!
	\brief 请求提升至容器顶端。
	\note 空实现。
	*/
	ImplI(IVisualControl) void
	RequestToTop()
	{}

protected:
	/*!
	\brief 检查轨道方向指定位置所在的区域。
	*/
	Area
	CheckArea(SDST) const;

	/*!
	\brief 响应轨道方向指定位置的点击事件。
	*/
	void
	ResponseTouchDown(SDST);

public:
	/*!
	\brief 处理水平滑块移动事件。
	*/
	void
	OnDrag_Thumb_Horizontal(TouchEventArgs&);

	/*!
	\brief 处理垂直滑块移动事件。
	*/
	void
	OnDrag_Thumb_Vertical(TouchEventArgs&);

	/*!
	\brief 处理水平区域点击事件。
	*/
	void
	OnTouchDown_Horizontal(TouchEventArgs&);

	/*!
	\brief 处理垂直区域点击事件。
	*/
	void
	OnTouchDown_Vertical(TouchEventArgs&);
};


//! \brief 水平轨道。
class YHorizontalTrack : public GMCounter<YHorizontalTrack>, public YComponent,
	public ATrack
{
public:
	/*!
	\brief 构造：使用指定窗口句柄、边界和部件容器指针。
	*/
	explicit
	YHorizontalTrack(HWND = NULL, const Rect& = Rect::FullScreen,
		IUIBox* = NULL);
	virtual DefEmptyDtor(YHorizontalTrack)

	ImplI(ATrack)
		DefGetter(Widgets::Orientation, Orientation, Widgets::Horizontal)
};


//! \brief 垂直轨道。
class YVerticalTrack : public GMCounter<YVerticalTrack>, public YComponent,
	public ATrack
{
public:
	/*!
	\brief 构造：使用指定窗口句柄、边界和部件容器指针。
	*/
	explicit
	YVerticalTrack(HWND = NULL, const Rect& = Rect::FullScreen,
		IUIBox* = NULL);
	virtual DefEmptyDtor(YVerticalTrack)

	ImplI(ATrack)
		DefGetter(Widgets::Orientation, Orientation, Widgets::Vertical)
};


//! \brief 滚动条。
class AScrollBar : public ATrack
{
public:
	typedef ATrack ParentType;

	//滚动条区域。覆盖了基类同名类型。
	typedef enum
	{
		None = 0,
		OnThumb = 1,
		OnPrevTrack = 2,
		OnNextTrack = 3,
		OnPrevButton = 4,
		OnNextButton = 5
	} Area;

protected:
	YThumb PrevButton, NextButton; //!< 滚动条按钮。

public:
	/*!
	\brief 构造：使用指定窗口句柄、边界、部件容器指针、大小和方向。
	*/
	explicit
	AScrollBar(HWND = NULL, const Rect& = Rect::FullScreen, IUIBox* = NULL,
		SDST = 8, Widgets::Orientation = Widgets::Horizontal);
	virtual DefEmptyDtor(AScrollBar)

	/*!
	\brief 取顶端可视部件指针。
	\note 仅滑块和滚动条按钮。
	*/
	virtual IVisualControl*
	GetTopVisualControlPtr(const Point&);
	virtual SDST
	GetTrackLength() const ythrow();

	virtual void
	SetThumbPosition(SDST);

	/*!
	\brief 绘制背景。
	*/
	virtual void
	DrawBackground();

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();
};


//! \brief 水平滚动条。
class YHorizontalScrollBar : public YComponent, public AScrollBar
{
public:
	typedef YComponent ParentType;

	explicit
	YHorizontalScrollBar(HWND = NULL, const Rect& = Rect::FullScreen,
		IUIBox* = NULL, SDST = 8);

	virtual DefEmptyDtor(YHorizontalScrollBar)

public:
	ImplI(ATrack)
		DefGetter(Widgets::Orientation, Orientation, Widgets::Vertical)
};


//! \brief 垂直滚动条。
class YVerticalScrollBar
{

};


//! \brief 文本列表框。
class YListBox : public GMCounter<YListBox>, public YVisualControl
{
public:
	typedef YVisualControl ParentType;
	typedef String ItemType; //!< 项目类型：字符串。
	typedef vector<ItemType> ListType; //!< 列表类型。
	typedef GSequenceViewer<ListType> ViewerType; //!< 视图类型。

protected:
	static const SDST defMarginH = 4, defMarginV = 2;

	GHResource<Drawing::TextRegion> prTextRegion; //!< 文本区域指针。
	const bool bDisposeList;

public:
	Drawing::Font Font; //!< 字体。
	Drawing::Padding Margin; //!< 文本和容器的间距。
	ListType& List; //!< 文本列表。

protected:
	GSequenceViewer<ListType> Viewer; //!< 列表视图。

public:
	DefEvent(IndexEventHandler, Selected) //!< 项目选择状态改变事件。
	DefEvent(IndexEventHandler, Confirmed) //!< 项目选中确定事件。

	/*!
	\brief 构造：使用指定窗口句柄、边界、部件容器指针和文本区域。
	*/
	explicit
	YListBox(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL,
		GHResource<Drawing::TextRegion> = NULL);
	/*!
	\brief 构造：使用指定窗口句柄、边界、部件容器指针、文本区域和文本列表。
	\note 使用外源列表。
	*/
	explicit
	YListBox(HWND = NULL, const Rect& = Rect::Empty, IUIBox* = NULL,
		GHResource<Drawing::TextRegion> = NULL,
		ListType& List_ = *GetGlobalResource<ListType>()); 
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual
	~YListBox() ythrow();

protected:
	/*!
	\brief 逻辑初始化：添加事件响应器。
	\note 保护实现。
	*/
	void
	_m_init();

public:
	DefPredicateMember(Selected, Viewer)

	DefGetter(ListType&, List, List)
	DefGetterMember(ViewerType::IndexType, Index, Viewer)
	DefGetterMember(ViewerType::IndexType, Selected, Viewer)
	/*!
	\brief 取指定项目索引的项目指针。
	*/
	ItemType*
	GetItemPtr(ViewerType::IndexType);
	/*!
	\brief 取项目行高。
	*/
	SDST
	GetItemHeight() const;

//	DefSetter(const ListType&, List, List)
	/*!
	\brief 按指定项目索引设置选中项目。
	*/
	void
	SetSelected(ViewerType::IndexType);
	/*!
	\brief 按接触点设置选中项目。
	*/
	void
	SetSelected(SPOS, SPOS);
	/*!
	\brief 按接触点设置选中项目。
	*/
	void
	SetSelected(const Point&);

	/*!
	\brief 绘制背景。
	*/
	virtual void
	DrawBackground();

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();

protected:
	/*!
	\brief 检查相对于所在缓冲区的控件坐标是否在选择范围内，
	//			返回选择的项目索引。
	*/
	ViewerType::IndexType
	CheckPoint(SPOS, SPOS);

public:
	PDefH(void, ClearSelected)
		ImplBodyMemberVoid(Viewer, ClearSelected)

private:
	/*!
	\brief 调用选中事件响应器。
	*/
	void
	CallSelected();

	/*!
	\brief 调用确认事件响应器。
	*/
	void
	CallConfirmed();

public:
	/*!
	\brief 响应按键接触开始事件。
	*/
	void
	OnKeyDown(KeyEventArgs&);

	/*!
	\brief 响应屏幕接触开始事件。
	*/
	void
	OnTouchDown(TouchEventArgs&);

	/*!
	\brief 响应屏幕接触移动事件。
	*/
	void
	OnTouchMove(TouchEventArgs&);

	/*!
	\brief 响应屏幕点击事件。
	*/
	void
	OnClick(TouchEventArgs&);

	/*!
	\brief 响应选中事件。
	*/
	void
	OnSelected(IndexEventArgs&);

	/*!
	\brief 响应确认事件。
	*/
	void
	OnConfirmed(IndexEventArgs&);
};

inline void
YListBox::SetSelected(const Point& pt)
{
	SetSelected(pt.X, pt.Y);
}


//! \brief 文件列表框。
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

	/*!
	\brief 取当前路径。
	*/
	IO::Path
	GetPath() const;

	/*!
	\brief 绘制背景。
	*/
	virtual void
	DrawBackground();

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();

	/*!
	\brief 响应确认事件。
	*/
	void
	OnConfirmed(IndexEventArgs&);
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

