/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textlist.h
\ingroup UI
\brief 样式相关的文本列表。
\version r1015
\author FrankHB <frankhb1989@gmail.com>
\since build 214
\par 创建时间:
	2011-04-19 22:59:02 +0800
\par 修改时间:
	2014-08-04 09:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TextList
*/


#ifndef YSL_INC_UI_textlist_h_
#define YSL_INC_UI_textlist_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YControl
#include YFM_YSLib_UI_YGUI
#include YFM_YSLib_UI_Label
#include YFM_YSLib_UI_Viewer

namespace YSLib
{

namespace UI
{

/*!
\ingroup UIModels
\brief 文本列表模块。
\warning 非虚析构。
\since build 188
*/
class YF_API MTextList : protected MHilightText
{
public:
	using ItemType = String; //!< 项目类型：字符串。
	using ListType = vector<ItemType>; //!< 列表类型。
	using IndexType = typename ListType::size_type; //!< 索引类型。
	/*!
	\brief 视图类型。
	\since build 523
	*/
	using ViewerType = GSequenceViewer<ListType>;

	/*!
	\brief 字体。
	\since build 524
	*/
	Drawing::Font Font;
	/*!
	\brief 文本和容器的间距
	\since build 524
	*/
	Drawing::Padding Margin{Drawing::DefaultMargin};

protected:
	mutable shared_ptr<ListType> hList; //!< 文本列表句柄。

public:
	/*!
	\brief 列表文本行距。
	\since build 524
	*/
	u8 LineGap = 0;

	/*!
	\brief 循环选择遍历。
	\since build 523
	*/
	bool CyclicTraverse = {};

protected:
	/*!
	\brief 列表视图。
	\since build 523
	*/
	ViewerType vwText;
	/*!
	\brief 列表视图首项目超出上边界的竖直偏移量。
	\since build 523
	*/
	SDst uTopOffset = 0;
	//! \since build 523
	Label lblShared{};
	//! \since build 523
	size_t idxShared = size_t(-1);

public:
	/*!
	\brief 构造：使用文本列表句柄、字体引用和高亮背景色/文本色对。
	\note 当文本列表指针为空时新建。
	\since build 523
	*/
	explicit
	MTextList(const shared_ptr<ListType>& = {}, const Drawing::Font& = {},
		const pair<Color, Color>& = FetchGUIState().Colors.GetPair(
		Styles::Highlight, Styles::HighlightText));
	DefDeMoveCtor(MTextList)

	//! \since build 523
	//@{
	WidgetRange
	GetChildrenByIndices(size_t, size_t);
	/*!
	\brief 取完整视图高。
	\note 依赖于 GetItemHeight 方法的结果。
	*/
	SDst
	GetFullViewHeight() const;
	size_t
	GetLastLabelIndexClipped(SPos, SDst) const;
	//@}

	/*!
	\brief 取文本列表。
	\since build 392
	*/
	DefGetter(const ynothrow, const ListType&, List, *hList)
	/*!
	\brief 取文本列表引用。
	\since build 392
	*/
	DefGetter(ynothrow, ListType&, ListRef, *hList)
	/*!
	\brief 取指定项目索引的项目指针。
	\since build 392
	*/
	ItemType*
	GetItemPtr(const IndexType&);
	/*!
	\brief 取指定项目索引的项目 const 指针。
	\since build 392
	*/
	const ItemType*
	GetItemPtr(const IndexType&) const;
	/*!
	\brief 取项目行高。
	\since build 301
	*/
	DefGetter(const ynothrow, SDst, ItemHeight, Font.GetHeight() + LineGap)
	/*!
	\brief 取文本列表中的最大文本宽度。
	\since build 282
	*/
	SDst
	GetMaxTextWidth() const;
	//! \since build 523
	Point
	GetUnitLocation(size_t) const;
	/*!
	\brief 取视图顶端竖直位置。
	\note 依赖于 GetItemHeight 方法的结果。
	\since build 523
	*/
	SDst
	GetViewPosition() const;

	/*!
	\brief 设置文本列表。
	\note 若参数为空则忽略。
	\since build 292
	*/
	void
	SetList(const shared_ptr<ListType>&);

	//! \since build 523
	//@{
	/*!
	\brief 调整列表视图底项目的竖直偏移量为零。
	\post 若调整则 <tt>uTopOffset == 0</tt> 。
	\return 返回调整前的偏移量值（取值区间 [0, <tt>GetItemHeight()</tt>) ）。
	\note 若没有底项目则不调整，返回 0 。

	第一参数指定视图可视区域的高。
	第二参数为 <tt>true</tt> 时，调整列表视图底项目（可能不完全）超出下边界以上的竖直
	偏移量为零；否则，调整列表视图首项目（可能不完全）超出上边界以上的竖直偏移量为零。
	*/
	SDst
	AdjustOffsetForHeight(SDst, bool);

	/*!
	\brief 按内容大小依次调整视图中选中和首个项目的索引，然后按需调整竖直偏移量。
	\note 参数指定视图可视区域的高。
	\warning 若视图大小变化后不调用此方法调整视图，可能导致选择项越界而行为未定义。
	*/
	void
	AdjustViewForContent(SDst);

	/*!
	\brief 根据指定的视图可视区域的高调整视图长度。
	\note 视图长为当项目数足够时所有在视图中显示的（可能不完全）项目总数。
	\note 当高为 0 时忽略。
	\warning 设置大小或列表内容后不调用此方法可能导致显示错误。
	*/
	void
	AdjustViewLengthForHeight(SDst);

	//! \brief 检查列表中的指定项是否被选中。
	PDefH(bool, CheckSelected, ListType::size_type idx) const
		ImplRet(vwText.IsSelected() && vwText.GetSelectedIndex() == idx)
	//@}

	/*!
	\brief 查找项。
	\return 若找到则返回对应索引，否则返回 <tt>IndexType(-1)</tt> 。
	\since build 316
	*/
	IndexType
	Find(const ItemType&) const;

	//! \since build 523
	WidgetIterator
	MakeIterator(size_t);

	//! \brief 刷新文本状态。
	void
	RefreshTextState();

	/*!
	\brief 复位视图。
	\note 若项目列表非空则选择首个项目。
	\since build 523
	*/
	void
	ResetView();
};


/*!
\brief 索引事件。
\since build 268
*/
using IndexEventArgs = GValueEventArgs<MTextList::IndexType>;
DeclDelegate(HIndexEvent, IndexEventArgs)

/*!
\brief 文本列表。
\since build 212
*/
class YF_API TextList : public Control, protected MTextList
{
public:
	using MTextList::ItemType;
	using MTextList::ListType;
	using MTextList::IndexType;
	//! \since build 523
	using MTextList::ViewerType;
	/*!
	\brief 视图参数类型。
	\param 表示主动定位视图。
	\since build 268
	*/
	using ViewArgs = GValueEventArgs<bool>;
	/*!
	\brief 视图事件委托类型。
	\since build 268
	*/
	DeclDelegate(HViewEvent, ViewArgs)

	using MTextList::Font;
	using MTextList::Margin;
	//! \since build 486
	using MHilightText::HilightBackColor;
	//! \since build 486
	using MHilightText::HilightTextColor;
	//! \since build 523
	using MTextList::CyclicTraverse;

	/*!
	\brief 视图变更事件。
	\since build 478

	当视图长度或位置发生改变时触发的事件。
	*/
	DeclEvent(HViewEvent, ViewChanged)
	/*!
	\brief 项目选择状态变更事件。
	\since build 478

	当项目被选中或取消选中时触发的事件。
	*/
	DeclEvent(HIndexEvent, Selected)
	/*!
	\brief 项目选中确认事件。
	\since build 478

	当前选中项目被确定时触发的事件。
	*/
	DeclEvent(HIndexEvent, Confirmed)

	/*!
	\brief 构造：使用指定边界、文本列表和高亮背景色/文本色对。
	\since build 486
	*/
	explicit
	TextList(const Rect& = {}, const shared_ptr<ListType>& = {},
		const pair<Color, Color>& = FetchGUIState().Colors.GetPair(
		Styles::Highlight, Styles::HighlightText));
	DefDeMoveCtor(TextList)

	DefPredMem(const ynothrow, Selected, vwText)
	PDefH(bool, Contains, ListType::size_type i)
		ImplBodyMem(vwText, Contains, i)

	DefGetterMem(const ynothrow, ListType::size_type, HeadIndex, vwText)
	using MTextList::GetItemHeight;
	using MTextList::GetItemPtr;
	//! \since build 523
	size_t
	GetLastLabelIndex() const;
	using MTextList::GetList;
	//! \since build 392
	using MTextList::GetListRef;
	using MTextList::GetMaxTextWidth;
	DefGetterMem(const ynothrow, ListType::size_type, SelectedIndex, vwText)
	//! \since build 523
	using MTextList::GetFullViewHeight;
	//! \since build 522
	PDefH(Rect, GetUnitBounds, size_t idx)
		ImplRet(Rect(GetUnitLocation(idx), GetWidth(), GetItemHeight()))
	//! \since build 523
	using MTextList::GetUnitLocation;
	//! \since build 523
	using MTextList::GetViewPosition;

	/*!
	\brief 设置文本列表。
	\note 若参数为空则忽略，否则设置列表并重置视图。
	\since build 292
	*/
	void
	SetList(const shared_ptr<ListType>&);
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
	PDefH(void, SetSelected, const Point& pt)
		ImplExpr(SetSelected(pt.X, pt.Y))

	/*!
	\brief 按内容大小依次调整视图中选中和首个项目的索引，然后按需调整竖直偏移量。
	\warning 若视图大小变化后不调用此方法调整视图，可能导致选择项越界而行为未定义。
	\since build 392
	*/
	PDefH(void, AdjustViewForContent, )
		ImplExpr(MTextList::AdjustViewForContent(GetHeight()))

	/*!
	\brief 调整视图长度。
	\note 视图长为当项目数足够时所有在视图中显示的（可能不完全）项目总数。
	\note 当部件高为 0 时忽略。
	\warning 设置大小或列表内容后不调用此方法可能导致显示错误。
	\since build 285
	*/
	PDefH(void, AdjustViewLength, )
		ImplExpr(AdjustViewLengthForHeight(GetHeight()))

private:
	/*!
	\brief 调用选中事件处理器。
	*/
	void
	CallSelected();

public:
	/*!
	\brief 检查列表中的指定项是否有效。
	\note 当且仅当有效时响应 Confirmed 事件。
	*/
	virtual PDefH(bool, CheckConfirmed, ListType::size_type idx) const
		ImplRet(MTextList::CheckSelected(idx))

	/*!
	\brief 检查点（相对于所在缓冲区的控件坐标）是否在选择范围内，
	\return 选择的项目索引。
	*/
	ListType::size_type
	CheckPoint(SPos, SPos);
	/*!
	\brief 检查点（相对于所在缓冲区的控件坐标）是否在选择范围内，
	\return 选择的项目索引，若无效则为 static_cast<ViewerType::IndexType>(-1) 。
	*/
	PDefH(ListType::size_type, CheckPoint, const Point& pt)
		ImplRet(CheckPoint(pt.X, pt.Y))

	PDefH(void, ClearSelected, )
		ImplBodyMem(vwText, ClearSelected, )

protected:
	/*!
	\brief 描画列表项背景。
	\param r 列表项有效区域边界。
	\since build 346
	*/
	virtual void
	DrawItemBackground(const PaintContext&, const Rect& r);

public:
	//! \since build 316
	using MTextList::Find;

protected:
	/*!
	\brief 无效化偏移量对应的列表项区域。
	\param offset 起始偏移量。
	\param diff 终止偏移量相对于起始偏移量的距离偏移量。
	\since build 318
	*/
	void
	InvalidateSelected(ListType::difference_type offset,
		ListType::size_type diff = 1);

	/*!
	\brief 无效化偏移量对应的列表项区域。
	\note 若第二参数小于第一参数则交换它们的值。
	\note 区域由参数组成的闭区间指定，若区间左端小于 0 则视为 0 。
	\since build 318
	*/
	void
	InvalidateSelected2(ListType::difference_type, ListType::difference_type);

private:
	/*!
	\brief 检查和调用确认事件处理器。
	*/
	void
	InvokeConfirmed(ListType::size_type);

public:
	/*!
	\brief 定位视图顶端至指定竖直位置。
	*/
	void
	LocateViewPosition(SDst);

	//! \since build 523
	using MTextList::MakeIterator;

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\note 绘制列表。
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;

	using MTextList::RefreshTextState;

	/*!
	\brief 复位视图：调用 MTextList::ResetView 和 UpdateView 。
	\sa MTextList::ResetView
	\sa UpdateView
	*/
	void
	ResetView();

	/*!
	\brief 选择第一个项目。
	\note 仅操作 vwText 并调整视图偏移量，不更新视图。
	*/
	void
	SelectFirst();

	/*!
	\brief 选择最后一个项目。
	\note 仅操作 vwText 并调整视图偏移量，不更新视图。
	*/
	void
	SelectLast();
};

//! \relates TextList
//@{
/*!
\brief 根据文本内容调整文本列表大小。
\note 调整大小后自动调整视图长度。
*/
YF_API void
ResizeForContent(TextList&);

/*!
\brief 更新文本列表视图。
\param is_active 是否确定仅变更视图位置。
\since build 392

调用视图变更事件、调整视图长度并无效化。
*/
YF_API void
UpdateView(TextList&, bool is_active = {});
//@}

} // namespace UI;

} // namespace YSLib;

#endif

