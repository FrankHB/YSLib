/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textlist.h
\ingroup UI
\brief 样式相关的文本列表。
\version r1419;
\author FrankHB<frankhb1989@gmail.com>
\since build 214 。
\par 创建时间:
	2011-04-19 22:59:02 +0800;
\par 修改时间:
	2011-11-30 19:44 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::TextList;
*/


#ifndef YSL_INC_UI_TEXTLIST_H_
#define YSL_INC_UI_TEXTLIST_H_

#include "ygui.h"
#include "label.h"
#include "viewer.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 文本列表。
\since build 212 。
*/
class TextList : public Control, protected MTextList
{
public:
	using MTextList::ItemType;
	using MTextList::ListType;
	using MTextList::IndexType;
	typedef GSequenceViewer<ListType> ViewerType; //!< 视图类型。

	using MTextList::Font;
	using MTextList::Margin;
	using MTextList::HorizontalAlignment;
	using MTextList::VerticalAlignment;
	using MTextList::Text;
	Color HilightBackColor; //!< 高亮背景色。
	Color HilightTextColor; //!< 高亮文本色。

	bool CyclicTraverse; //!< 循环选择遍历。

private:
	ViewerType viewer; //!< 列表视图。
	SDst top_offset; //!< 列表视图首项目超出上边界的竖直偏移量。
	DeclEvent(HUIEvent, ViewChanged) //!< 视图变更事件。
	DeclEvent(HIndexEvent, Selected) //!< 项目选择状态变更事件。
	DeclEvent(HIndexEvent, Confirmed) //!< 项目选中确定事件。

public:
	/*!
	\brief 构造：使用指定边界、文本列表和高亮背景色/文本色对。
	*/
	explicit
	TextList(const Rect& = Rect::Empty,
		const shared_ptr<ListType>& = shared_ptr<ListType>(),
		pair<Color, Color> = FetchGUIShell().Colors.GetPair(Styles::Highlight,
		Styles::HighlightText));
	inline DefDeMoveCtor(TextList)

	DefPredicateMember(Selected, viewer)
	PDefH(bool, Contains, ListType::size_type i)
		ImplBodyMember(viewer, Contains, i)

	DefMutableEventGetter(HUIEvent, ViewChanged, ViewChanged) //!< 视图变更事件。
	DefMutableEventGetter(HIndexEvent, Selected, Selected) \
		//!< 项目选择状态变更事件。
	DefMutableEventGetter(HIndexEvent, Confirmed, Confirmed) \
		//!< 项目选中确定事件。

	using MTextList::GetList;
	using MTextList::GetItemPtr;
	using MTextList::GetItemHeight;
	DefGetterMember(ListType::size_type, HeadIndex, viewer)
	DefGetterMember(ListType::size_type, SelectedIndex, viewer)

	/*!
	\brief 取完整视图高。
	\note 依赖于 GetItemHeight 方法的结果。
	*/
	SDst
	GetFullViewHeight() const;
	/*!
	\brief 取完整视图大小。
	*/
	Size
	GetFullViewSize() const;
	/*!
	\brief 取视图顶端竖直位置。
	\note 依赖于 GetItemHeight 方法的结果。
	*/
	SDst
	GetViewPosition() const;

//	DefSetter(const ListType&, List, List)
	/*!
	\brief 按指定项目索引设置选中项目。
	*/
	void
	SetSelected(ListType::size_type);
	/*!
	\brief 按接触点设置选中项目。
	*/
	void
	SetSelected(SPos, SPos);
	/*!
	\brief 按接触点设置选中项目。
	*/
	void
	SetSelected(const Point&);

private:
	/*!
	\brief 调整列表视图底项目（可能不完全）超出下边界以上的竖直偏移量为零。
	\return 返回调整前的偏移量值（取值区间 [0, <tt>GetItemHeight()</tt>) ）。
	\note 若没有底项目则不调整，返回 0 。
	*/
	SDst
	AdjustBottomOffset();

	/*!
	\brief 调整列表视图首项目（可能不完全）超出上边界以上的竖直偏移量为零。
	\return 返回调整前的偏移量值（取值区间 [0, <tt>GetItemHeight()</tt>) ）。
	\post 若调整则 <tt>top_offset == 0</tt> 。
	*/
	SDst
	AdjustTopOffset();

	/*!
	\brief 调整视图长度。
	\note 视图长为当项目数足够时所有在视图中显示的（可能不完全）项目总数。
	*/
	void
	AdjustViewLength();

public:
	/*!
	\brief 检查列表中的指定项是否有效。
	\note 当且仅当有效时响应 Confirmed 事件。
	*/
	virtual bool
	CheckConfirmed(ListType::size_type) const;

	/*!
	\brief 检查点（相对于所在缓冲区的控件坐标）是否在选择范围内，
	\return 选择的项目索引。
	*/
	ListType::size_type
	CheckPoint(SPos, SPos);
	/*!
	\brief 检查点（相对于所在缓冲区的控件坐标）是否在选择范围内，
	\return 选择的项目索引，若无效则为 static_cast<Viewer::IndexType>(-1) 。
	*/
	ListType::size_type
	CheckPoint(const Point&);

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const PaintContext&);

	PDefH(void, ClearSelected)
		ImplBodyMember(viewer, ClearSelected)

	/*!
	\brief 定位视图顶端至指定竖直位置。
	*/
	void
	LocateViewPosition(SDst);

protected:
	/*!
	\brief 绘制列表项。
	\since build 265 。
	*/
	virtual void
	PaintItem(const Graphics&, const Rect& mask, const Rect&,
		ListType::size_type);

	/*!
	\brief 绘制列表。
	*/
	virtual void
	PaintItems(const PaintContext&);

public:
	using MTextList::RefreshTextState;

	/*!
	\brief 复位视图。
	\note 若项目列表非空则选择首个项目。
	*/
	void
	ResetView();

	/*!
	\brief 选择第一个项目。
	\note 仅操作 viewer ，不更新视图。
	*/
	void
	SelectFirst();

	/*!
	\brief 选择最后一个项目。
	\note 仅操作 viewer ，不更新视图。
	*/
	void
	SelectLast();

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
	InvokeConfirmed(ListType::size_type);
};

inline void
TextList::SetSelected(const Point& pt)
{
	SetSelected(pt.X, pt.Y);
}

inline TextList::ListType::size_type
TextList::CheckPoint(const Point& pt)
{
	return CheckPoint(pt.X, pt.Y);
}


/*!
\brief 根据文本内容调整文本列表大小。
*/
void
ResizeForContent(TextList&);

YSL_END_NAMESPACE(Components)

YSL_END

#endif

