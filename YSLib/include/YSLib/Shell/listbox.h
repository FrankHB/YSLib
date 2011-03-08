/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file listbox.h
\ingroup Shell
\brief 样式相关的图形用户界面列表框控件实现。
\version 0.2957;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-03-07 20:30:40 +0800;
\par 修改时间:
	2011-03-07 21:40 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::ListBox;
*/


#ifndef INCLUDED_LISTBOX_H_
#define INCLUDED_LISTBOX_H_

#include "../Core/ysdef.h"
#include "../Core/ycounter.hpp"
#include "ycontrol.h"
#include "../Core/ystring.h"
#include "ycomp.h"
#include "ytext.h"
#include "scroll.h"
//#include "../Core/yobject.h"
//#include "yuicont.h"
//#include "yuicontx.h"
//#include "../Core/yres.h"
//#include "../Adaptor/yfont.h"
//#include "../Core/yres.h"
//#include "ylabel.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Widgets)

YSL_BEGIN_NAMESPACE(Controls)

//! \brief 文本列表框。
class YSimpleListBox : public GMCounter<YSimpleListBox>,
	public YControl
{
public:
	typedef YControl ParentType;
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
	DeclEvent(HVisualEvent, ViewChanged) //!< 视图变更事件。
	DeclEvent(HIndexEvent, Selected) //!< 项目选择状态变更事件。
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
	\brief 取完整视图高。
	*/
	SDST
	GetFullViewHeight() const;
	/*!
	\brief 取完整视图大小。
	*/
	Size
	GetFullViewSize() const;
	/*!
	\brief 取视图顶端垂直位置。
	*/
	SDST
	GetViewPosition() const;

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
	\brief 定位视图顶端至指定垂直位置。
	*/
	void
	LocateViewPosition(SDST);

	/*!
	\brief 复位视图。
	\note 若项目列表非空则选择首个项目。
	*/
	void
	ResetView();

	/*!
	\brief 更新视图。
	\note 调用视图变更事件。
	*/
	void
	UpdateView();

private:
	/*!
	\brief 调用选中事件处理器。
	*/
	void
	CallSelected();

	/*!
	\brief 检查和调用确认事件处理器。
	*/
	void
	CheckConfirmed(ViewerType::IndexType);

	/*!
	\brief 处理键接触开始事件。
	*/
	void
	OnKeyDown(KeyEventArgs&);

	/*!
	\brief 处理屏幕接触开始事件。
	*/
	void
	OnTouchDown(TouchEventArgs&);

	/*!
	\brief 处理屏幕接触移动事件。
	*/
	void
	OnTouchMove(TouchEventArgs&);

	/*!
	\brief 处理屏幕点击事件。
	*/
	void
	OnClick(TouchEventArgs&);

	/*!
	\brief 处理选中事件。
	*/
	void
	OnSelected(IndexEventArgs&);

	/*!
	\brief 处理确认事件。
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
	YSimpleListBox TextListBox; //文本列表框。

public:
	explicit
	YListBox(const Rect& = Rect::Empty, IUIBox* = NULL,
		GHWeak<ListType> = NULL);
	virtual DefEmptyDtor(YListBox)

	DefPredicateMember(Selected, TextListBox)
	PDefH1(bool, Contains, ViewerType::IndexType i)
		ImplBodyMember1(TextListBox, Contains, i)

	/*!
	\brief 取顶端控件指针。
	*/
	virtual IControl*
	GetTopControlPtr(const Point&);
	DefGetterMember(ViewerType::IndexType, HeadIndex, TextListBox)
	DefGetterMember(ViewerType::IndexType, SelectedIndex, TextListBox)
	DefGetterMember(ListType&, List, TextListBox)
	DefMutableEventGetter(HVisualEvent, ViewChanged, TextListBox.ViewChanged)
		//!< 视图变更事件。
	DefMutableEventGetter(HIndexEvent, Selected, TextListBox.Selected) \
		//!< 项目选择状态变更事件。
	DefMutableEventGetter(HIndexEvent, Confirmed, TextListBox.Confirmed) \
		//!< 项目选中确定事件。

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();

	PDefH0(void, ResetView)
		ImplBodyMember0(TextListBox, ResetView)
	PDefH0(void, UpdateView)
		ImplBodyMember0(TextListBox, UpdateView)

private:
	/*!
	\brief 处理垂直滚动条滚动事件。
	*/
	void
	OnScroll_VerticalScrollBar(ScrollEventArgs&);

	/*!
	\brief 处理文本列表框视图变更事件。
	*/
	void
	OnViewChanged_TextListBox(EventArgs&);
};


//! \brief 文件列表框。
class YFileBox : public GMCounter<YFileBox>, public IO::FileList,
	public YListBox
{
public:
	typedef YListBox ParentType;

	explicit
	YFileBox(const Rect& = Rect::Empty, IUIBox* = NULL);
	virtual DefEmptyDtor(YFileBox)

	/*!
	\brief 取当前路径。
	*/
	IO::Path
	GetPath() const;

private:
	/*!
	\brief 处理确认事件。
	*/
	void
	OnConfirmed(IndexEventArgs&);
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

