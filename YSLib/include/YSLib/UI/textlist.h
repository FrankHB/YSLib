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
\version 0.1345;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-19 22:59:02 +0800;
\par 修改时间:
	2011-07-18 06:53 +0800;
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

YSL_BEGIN_NAMESPACE(Controls)

//! \brief 文本列表。
class TextList : public Controls::Control, protected Widgets::MTextList
{
public:
	using MTextList::ItemType;
	using MTextList::ListType;
	using MTextList::IndexType;
	using MTextList::TextAlignmentStyle;
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
	SDst top_offset; //!< 列表视图首项目超出上边界的垂直偏移量。

	//! \brief 事件依赖项。
	class Dependencies
	{
	public:
		DeclDepEvent(HVisualEvent, ViewChanged) //!< 视图变更事件。
		DeclDepEvent(HIndexEvent, Selected) //!< 项目选择状态变更事件。
		DeclDepEvent(HIndexEvent, Confirmed) //!< 项目选中确定事件。

		Dependencies();
	} Events;

public:
	/*!
	\brief 构造：使用指定边界、文本列表和高亮背景色/文本色对。
	*/
	explicit
	TextList(const Rect& = Rect::Empty,
		const shared_ptr<ListType>& = shared_ptr<ListType>(),
		pair<Color, Color> = FetchGUIShell().Colors.GetPair(Styles::Highlight,
		Styles::HighlightText));

	DefPredicateMember(Selected, viewer)
	PDefH1(bool, Contains, ViewerType::SizeType i)
		ImplBodyMember1(viewer, Contains, i)

	DefMutableDepEventGetter(HVisualEvent, ViewChanged, Events.ViewChanged) \
		//!< 视图变更事件。
	DefMutableDepEventGetter(HIndexEvent, Selected, Events.Selected) \
		//!< 项目选择状态变更事件。
	DefMutableDepEventGetter(HIndexEvent, Confirmed, Events.Confirmed) \
		//!< 项目选中确定事件。

	using MTextList::GetList;
	using MTextList::GetItemPtr;
	using MTextList::GetItemHeight;
	DefGetterMember(ViewerType::SizeType, HeadIndex, viewer)
	DefGetterMember(ViewerType::SizeType, SelectedIndex, viewer)

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
	\brief 取视图顶端垂直位置。
	\note 依赖于 GetItemHeight 方法的结果。
	*/
	SDst
	GetViewPosition() const;

//	DefSetter(const ListType&, List, List)
	/*!
	\brief 按指定项目索引设置选中项目。
	*/
	void
	SetSelected(ViewerType::SizeType);
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

	/*!
	\brief 调整列表视图首项目超出上边界的垂直偏移量为零。
	\return 返回调整前的偏移量值。
	*/
	SDst
	AdjustTopOffset();
	/*!
	\brief 调整列表视图底项目超出上边界的垂直偏移量为零。
	\return 返回调整前的偏移量值。
	\note 若没有底项目则不调整，返回 0 。
	*/
	SDst
	AdjustBottomOffset();

	/*!
	\brief 检查点（相对于所在缓冲区的控件坐标）是否在选择范围内，
	\return 选择的项目索引。
	*/
	ViewerType::SizeType
	CheckPoint(SPos, SPos);
	/*!
	\brief 检查点（相对于所在缓冲区的控件坐标）是否在选择范围内，
	\return 选择的项目索引，若无效则为 static_cast<Viewer::IndexType>(-1) 。
	*/
	ViewerType::SizeType
	CheckPoint(const Point&);

	/*!
	\brief 绘制控件界面。
	*/
	virtual void
	DrawControl(const Graphics&, const Point&, const Rect&);

	PDefH0(void, ClearSelected)
		ImplBodyMember0(viewer, ClearSelected)

	/*!
	\brief 定位视图顶端至指定垂直位置。
	*/
	void
	LocateViewPosition(SDst);

protected:
	/*!
	\brief 绘制列表项。
	*/
	virtual void
	PaintItem(const Graphics&, const Rect&, ListType::size_type);

	/*!
	\brief 绘制列表。
	\note 参数意义同 Refresh 。
	*/
	virtual void
	PaintItems(const Graphics&, const Point&, const Rect&);

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
	CheckConfirmed(ViewerType::SizeType);

	/*!
	\brief 处理选中和确认事件。
	*/
	void
	OnSelected(IndexEventArgs&&);
};

inline void
TextList::SetSelected(const Point& pt)
{
	SetSelected(pt.X, pt.Y);
}

inline TextList::ViewerType::SizeType
TextList::CheckPoint(const Point& p)
{
	return CheckPoint(p.X, p.Y);
}


/*!
\brief 根据文本内容调整文本列表大小。
*/
void
ResizeForContent(TextList&);

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

