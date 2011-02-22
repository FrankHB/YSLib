/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yguicomp.h
\ingroup Shell
\brief 样式相关图形用户界面组件实现。
\version 0.2634;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-04 21:23:32 + 08:00;
\par 修改时间:
	2011-02-22 21:16 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YGUIComponent;
*/


#ifndef INCLUDED_YGUICOMP_H_
#define INCLUDED_YGUICOMP_H_

#include "../Core/ysdef.h"
#include "../Core/ycounter.hpp"
#include "ycontrol.h"
#include "../Core/yobject.h"
#include "yuicont.h"
#include "yuicontx.h"
//#include "../Core/yres.h"
#include "../Adaptor/yfont.h"
#include "../Core/yres.h"
#include "ylabel.h"
//#include "ystyle.h"

YSL_BEGIN

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
	\brief 构造：使用指定边界和部件容器指针。
	*/
	explicit
	YThumb(const Rect& = Rect::Empty, IUIBox* = NULL);
	virtual DefEmptyDtor(YThumb)

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();

private:
	/*!
	\brief 响应控件进入事件。
	*/
	void
	OnEnter(InputEventArgs&);

	/*!
	\brief 响应控件离开事件。
	*/
	void
	OnLeave(InputEventArgs&);
};


//! \brief 按钮。
class YButton : public GMCounter<YButton>, public YThumb,
	public Widgets::MLabel
{
public:
	typedef YThumb ParentType;

/*
	YImage BackgroundImage; //!< 背景图像。
	YImage Image; //!< 前景图像。
*/

	/*!
	\brief 构造：使用指定边界、部件容器指针和字体。
	*/
	explicit
	YButton(const Rect& = Rect::Empty, IUIBox* = NULL,
		const Drawing::Font& = Drawing::Font::GetDefault());
	virtual DefEmptyDtor(YButton)

public:
	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();
};


//! \brief 轨道。
class ATrack : public AUIBoxControl
{
public:
	//! \brief 轨道区域。
	typedef enum
	{
		None = 0,
		OnThumb = 1,
		OnPrev = 2,
		OnNext = 3
	} Area;

protected:
	YThumb Thumb; //!< 滑块。

private:
	SDST MinThumbLength; //!< 最小滑块长度。

public:
	/*!
	\brief 构造：使用指定边界、部件容器指针和大小。
	*/
	explicit
	ATrack(const Rect& = Rect::Empty, IUIBox* = NULL, SDST = 8);
	virtual DefEmptyDtor(ATrack)

	DefPredicate(Horizontal, GetOrientation() == Horizontal)
	DefPredicate(Vertical, GetOrientation() == Vertical)

	/*!
	\brief 取焦点指针。
	*/
	ImplI1(IUIBox) DefMutableGetterBase(IVisualControl*, FocusingPtr,
		MSimpleFocusResponser)

	/*!
	\brief 取顶端可视部件指针。
	\note 仅滑块。
	*/
	ImplI1(AUIBoxControl) IVisualControl*
	GetTopVisualControlPtr(const Point&);
	DefGetter(SDST, MinThumbLength, MinThumbLength)
	DeclIEntry(Orientation GetOrientation() const) //!< 取轨道方向。
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
	\brief 检查轨道方向指定位置所在的区域。
	\note 断言检查。
	*/
	Area
	CheckArea(SDST) const;

	/*!
	\brief 响应轨道方向指定位置的点击事件。
	*/
	void
	ResponseTouchDown(SDST);

private:
	/*!
	\brief 处理区域点击事件。
	*/
	void
	OnTouchDown(TouchEventArgs&);
};


//! \brief 水平轨道。
class YHorizontalTrack : public GMCounter<YHorizontalTrack>, public YComponent,
	public ATrack
{
public:
	/*!
	\brief 构造：使用指定边界和部件容器指针和最小滑块长。
	\note 断言检查：宽大于长的 2 倍。
	*/
	explicit
	YHorizontalTrack(const Rect& = Rect::Empty, IUIBox* = NULL, SDST = 8);
	virtual DefEmptyDtor(YHorizontalTrack)

	ImplI1(ATrack)
	DefGetter(Orientation, Orientation, Horizontal)

private:
	/*!
	\brief 处理水平滑块移动事件。
	*/
	void
	OnDrag_Thumb_Horizontal(TouchEventArgs&);
};


//! \brief 垂直轨道。
class YVerticalTrack : public GMCounter<YVerticalTrack>, public YComponent,
	public ATrack
{
public:
	/*!
	\brief 构造：使用指定边界、部件容器指针和最小滑块长。
	\note 断言检查：长大于宽的 2 倍。
	*/
	explicit
	YVerticalTrack(const Rect& = Rect::Empty, IUIBox* = NULL, SDST = 8);
	virtual DefEmptyDtor(YVerticalTrack)

	ImplI1(ATrack)
		DefGetter(Orientation, Orientation, Vertical)

private:
	/*!
	\brief 处理垂直滑块移动事件。
	*/
	void
	OnDrag_Thumb_Vertical(TouchEventArgs&);
};


//! \brief 滚动条。
class AScrollBar : public AUIBoxControl
{
private:
	std::auto_ptr<ATrack> pTrack; //轨道。

protected:
	YThumb PrevButton, NextButton; //!< 滚动条按钮。

public:
	/*!
	\brief 构造：使用指定边界、部件容器指针、大小和方向。
	*/
	explicit
	AScrollBar(const Rect& = Rect::Empty, IUIBox* = NULL, SDST = 8,
		Orientation = Horizontal);
	virtual DefEmptyDtor(AScrollBar)

	/*!
	\brief 取顶端可视部件指针。
	\note 仅滑块和滚动条按钮。
	*/
	ImplI1(AUIBoxControl) IVisualControl*
	GetTopVisualControlPtr(const Point&);
	DefGetter(ATrack&, Track, *pTrack)

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
class YHorizontalScrollBar : public GMCounter<YHorizontalScrollBar>,
	public YComponent,
	public AScrollBar
{
public:
	typedef YComponent ParentType;

	explicit
	YHorizontalScrollBar(const Rect& = Rect::Empty, IUIBox* = NULL,
		SDST = 8);

	virtual DefEmptyDtor(YHorizontalScrollBar)

public:
	ImplI1(ATrack)
	DefGetter(Orientation, Orientation, Horizontal)
};


//! \brief 垂直滚动条。
class YVerticalScrollBar : public GMCounter<YVerticalScrollBar>,
	public YComponent,
	public AScrollBar
{
public:
	typedef YComponent ParentType;

	explicit
	YVerticalScrollBar(const Rect& = Rect::Empty, IUIBox* = NULL,
		SDST = 8);

	virtual DefEmptyDtor(YVerticalScrollBar)

public:
	ImplI1(ATrack)
	DefGetter(Orientation, Orientation, Vertical)
};


//! \brief 带滚动条的容器基实现类。
class ScrollableContainer : public AUIBoxControl
{
private:
	YHorizontalScrollBar HorizontalScrollBar;
	YVerticalScrollBar VerticalScrollBar;

public:
	explicit
	ScrollableContainer(const Rect& = Rect::Empty, IUIBox* = NULL);
	virtual DefEmptyDtor(ScrollableContainer)

	/*!
	\brief 取顶端可视部件指针。
	*/
	ImplI1(AUIBoxControl) IVisualControl*
	GetTopVisualControlPtr(const Point&);

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();

protected:
	/*!
	\brief 固定布局。
	\return 调整后的视图大小。
	*/
	Size
	FixLayout(const Size&);
};


//! \brief 文本列表框。
class YSimpleListBox : public GMCounter<YSimpleListBox>,
	public YVisualControl
{
public:
	typedef YVisualControl ParentType;
	typedef String ItemType; //!< 项目类型：字符串。
	typedef vector<ItemType> ListType; //!< 列表类型。
	typedef GSequenceViewer<ListType> ViewerType; //!< 视图类型。

public:
	Drawing::Font Font; //!< 字体。
	Drawing::Padding Margin; //!< 文本项的边距。

protected:
	GHWeak<ListType> wpList; //!< 文本列表指针。

private:
	ViewerType viewer; //!< 列表视图。
	SDST top_offset; //!< 列表视图首项目超出上边界的垂直偏移量。
	Drawing::TextState text_state; //!< 文本状态。

public:
	DeclEvent(HIndexEvent, Selected) //!< 项目选择状态改变事件。
	DeclEvent(HIndexEvent, Confirmed) //!< 项目选中确定事件。

	/*!
	\brief 构造：使用指定边界、部件容器指针、文本区域和文本列表。
	*/
	explicit
	YSimpleListBox(const Rect& = Rect::Empty, IUIBox* = NULL,
		GHWeak<ListType> = NULL); 
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual DefEmptyDtor(YSimpleListBox)

public:
	DefPredicateMember(Selected, viewer)
	PDefH1(bool, Contains, ViewerType::IndexType i)
		ImplBodyMember1(viewer, Contains, i)

	/*!
	\brief 取文本列表。
	\note 断言检查：wpList 。
	*/
	ListType&
	GetList() const ythrow();

protected:
	/*!
	\brief 取文本状态。
	*/
	Drawing::TextState&
	GetTextState() ythrow();

public:
	DefGetterMember(ViewerType::IndexType, HeadIndex, viewer)
	DefGetterMember(ViewerType::IndexType, SelectedIndex, viewer)
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
	/*!
	\brief 取完整视图大小。
	*/
	Size
	GetFullViewSize() const;

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
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();

protected:
	/*!
	\brief 调整列表视图首项目超出上边界的垂直偏移量为零。
	\return 返回调整前的偏移量值。
	*/
	SDST
	AdjustTopOffset();
	/*!
	\brief 调整列表视图底项目超出上边界的垂直偏移量为零。
	\return 返回调整前的偏移量值。
	\note 若没有底项目则不调整，返回 0 。
	*/
	SDST
	AdjustBottomOffset();
	/*!
	\brief 检查点（相对于所在缓冲区的控件坐标）是否在选择范围内，
	\return 选择的项目索引。
	*/
	ViewerType::IndexType
	CheckPoint(SPOS, SPOS);

	/*!
	\brief 检查点（相对于所在缓冲区的控件坐标）是否在选择范围内，
	\return 选择的项目索引。
	*/
	ViewerType::IndexType
	CheckPoint(const Point&);

public:
	PDefH0(void, ClearSelected)
		ImplRet(static_cast<void>(viewer.ClearSelected()))

	/*!
	\brief 复位视图。
	\note 若项目列表非空则选择首个项目。
	*/
	void
	ResetView();

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
	CallConfirmed(ViewerType::IndexType);

	/*!
	\brief 响应键接触开始事件。
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
YSimpleListBox::SetSelected(const Point& pt)
{
	SetSelected(pt.X, pt.Y);
}

inline YSimpleListBox::ViewerType::IndexType
YSimpleListBox::CheckPoint(const Point& p)
{
	return CheckPoint(p.X, p.Y);
}


/*!
\brief 列表框。

带滚动条的文本列表框。
*/
class YListBox : public GMCounter<YListBox>, public YComponent,
	public ScrollableContainer
{
public:
	typedef YComponent ParentType;
	typedef YSimpleListBox::ListType ListType;
	typedef YSimpleListBox::ViewerType ViewerType;

private:
	YSimpleListBox TextListBox;

public:
	explicit
	YListBox(const Rect& = Rect::Empty, IUIBox* = NULL,
		GHWeak<ListType> = NULL);
	virtual DefEmptyDtor(YListBox)

	DefPredicateMember(Selected, TextListBox)
	PDefH1(bool, Contains, ViewerType::IndexType i)
		ImplBodyMember1(TextListBox, Contains, i)

	/*!
	\brief 取顶端可视部件指针。
	*/
	virtual IVisualControl*
	GetTopVisualControlPtr(const Point&);
	DefGetterMember(ViewerType::IndexType, HeadIndex, TextListBox)
	DefGetterMember(ViewerType::IndexType, SelectedIndex, TextListBox)
	DefGetterMember(ListType&, List, TextListBox)
	DefMutableEventGetter(HIndexEvent, Selected, TextListBox.Selected) \
		//!< 项目选择状态改变事件。
	DefMutableEventGetter(HIndexEvent, Confirmed, TextListBox.Confirmed) \
		//!< 项目选中确定事件。

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();

	PDefH0(void, ResetView)
		ImplBodyMember0(TextListBox, ResetView)
};


//! \brief 文件列表框。
class YFileBox : public GMCounter<YFileBox>, public IO::FileList,
	public YListBox
{
public:
	typedef YListBox ParentType;

	explicit
	YFileBox(const Rect& = Rect::Empty, IUIBox* = NULL);
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

private:
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

