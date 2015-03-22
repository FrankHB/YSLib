/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ListControl.h
\ingroup UI
\brief 列表控件。
\version r1634
\author FrankHB <frankhb1989@gmail.com>
\since build 528
\par 创建时间:
	2011-04-19 22:59:02 +0800
\par 修改时间:
	2015-03-22 15:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::ListControl
*/


#ifndef YSL_INC_UI_ListControl_h_
#define YSL_INC_UI_ListControl_h_ 1

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
\brief 索引事件。
\since build 268
*/
using IndexEventArgs = GValueEventArgs<size_t>;


/*!
\ingroup UIModels
\brief 控件列表模块。
\since build 528
*/
class YF_API AMUnitControlList : protected AMUnitList
{
public:
	/*!
	\brief 视图参数类型。
	\param 表示主动定位视图。
	\since build 268
	*/
	using ViewArgs = GValueEventArgs<bool>;

	/*!
	\brief 视图变更事件。
	\since build 478

	当视图长度或位置发生改变时触发的事件。
	*/
	GEvent<void(ViewArgs)> ViewChanged;
	/*!
	\brief 项目选择状态变更事件。
	\since build 478

	当项目被选中或取消选中时触发的事件。
	*/
	GEvent<void(IndexEventArgs)> Selected;
	/*!
	\brief 项目选中确认事件。
	\since build 478

	当前选中项目被确定时触发的事件。
	*/
	GEvent<void(IndexEventArgs)> Confirmed;
	/*!
	\brief 迭代器迭代事件。
	\since build 529
	*/
	GEvent<void(size_t)> Iterated;

private:
	unique_ptr<IWidget> p_unit;

public:
	/*!
	\brief 构造：使用部件指针。
	\note 当文本列表指针为空时新建。
	\since build 527
	*/
	explicit
	AMUnitControlList(unique_ptr<IWidget>&&);
	DefDeMoveCtor(AMUnitControlList)

	//! \since build 529
	using AMUnitList::GetFullViewHeight;
	//! \since build 581
	using AMUnitList::GetItemHeight;
	//! \since build 527
	IWidget&
	GetUnitRef() const ImplI(AMUnitList);
	//! \since build 529
	using AMUnitList::GetViewPosition;

	/*!
	\brief 设置单元部件。
	\throw std::invalid_argument 参数为空。
	\since build 580
	*/
	void
	SetUnit(unique_ptr<IWidget>);

	/*!
	\brief 构造共享子部件迭代器。
	\note 非强异常安全：状态和最后一次调用此方法相关。
	\since build 523
	*/
	WidgetIterator
	MakeIterator(size_t);
};


/*!
\ingroup UIModels
\brief 文本列表模块。
\since build 188
*/
class YF_API MTextList : public AMUnitControlList
{
public:
	using ItemType = String; //!< 项目类型：字符串。
	using ListType = vector<ItemType>; //!< 列表类型。
	using IndexType = typename ListType::size_type; //!< 索引类型。

protected:
	mutable shared_ptr<ListType> hList; //!< 文本列表句柄。

public:
	//! \since build 529
	MLabel LabelBrush{};

	/*!
	\brief 构造：使用文本列表句柄。
	\note 当文本列表指针为空时新建。
	\since build 527
	*/
	explicit
	MTextList(const shared_ptr<ListType>& = {});
	DefDeMoveCtor(MTextList)

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

protected:
	/*!
	\brief 取项目行高：由字体和行距决定。
	\note 实现：断言：结果非空。
	\since build 581
	*/
	SDst
	GetItemHeightCore() const override;

public:
	//! \since build 586
	size_t
	GetTotal() const ImplI(AMUnitList);

	/*!
	\brief 设置文本列表。
	\note 若参数为空则忽略。
	\since build 292
	*/
	void
	SetList(const shared_ptr<ListType>&);
};


/*!
\brief 文本列表。
\note LabelBrush 非强异常安全：不抛出异常时保持每次绘制最后项目的状态。
\since build 212
*/
class YF_API TextList : public Control, public MTextList, protected MHilightText
{
public:
	//! \since build 581
	//@{
	//! \brief 选择操作选项。
	enum SelectionOption : size_t
	{
		//! \brief 确认时自动清空选择。
		ClearSelectionOnConfirm,
		//! \brief 离开时自动清空选择。
		ClearSelectionOnLeave,
		//! \brief 悬停时自动选择。
		SelectOnHover,
		SelectionOptionMax
	};

	//! \brief 选择操作选项。
	std::bitset<SelectionOptionMax> SelectionOptions{};
	//! \brief 判断是否为 Leave 目标时不取消选择的相关部件。
	std::function<bool(IWidget&)> IsRelated{};
	//@}
	/*!
	\brief 默认前景色。
	\since build 525
	*/
	Color ForeColor{Drawing::ColorSpace::Black};
	//! \since build 486
	using MHilightText::HilightBackColor;
	//! \since build 486
	using MHilightText::HilightTextColor;

	/*!
	\brief 构造：使用指定边界、文本列表和高亮背景色/文本色对。
	\since build 486
	*/
	explicit
	TextList(const Rect& = {}, const shared_ptr<ListType>& = {},
		const pair<Color, Color>& = FetchGUIConfiguration().Colors.GetPair(
		Styles::Highlight, Styles::HighlightText));
	DefDeMoveCtor(TextList)

	DefPredMem(const ynothrow, Selected, vwList)
	PDefH(bool, Contains, ListType::size_type i)
		ImplRet(vwList.Contains(i))

	//! \since build 530
	DefGetter(override, WidgetRange, Children, WidgetRange(begin(), end()))
	DefGetterMem(const ynothrow, ListType::size_type, HeadIndex, vwList)
	//! \since build 523
	size_t
	GetLastLabelIndex() const;
	DefGetterMem(const ynothrow, ListType::size_type, SelectedIndex, vwList)
	//! \since build 522
	PDefH(Rect, GetUnitBounds, size_t idx)
		ImplRet(Rect(GetUnitLocation(idx), GetWidth(), GetItemHeight()))

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
		ImplExpr(AdjustViewLengthForHeight(GetItemHeight(), GetHeight()))

private:
	/*!
	\brief 调用选中事件处理器。
	*/
	void
	CallSelected();

public:
	PDefH(void, ClearSelected, )
		ImplExpr(vwList.ClearSelected())

private:
	/*!
	\brief 当 ClearSelectionOnLeave 被设置时在 Leave 事件中清除已选择项。
	\note 若 IsRelated 非空，使用 IsRelated 判端是否忽略。
	\sa IsRelated
	\since build 581
	*/
	void
	ClearSelectedOnLeave(CursorEventArgs&&);

protected:
	/*!
	\brief 无效化偏移量对应的列表项区域。
	\param offset 起始偏移量。
	\param diff 终止偏移量相对起始偏移量的距离偏移量。
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

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\note 非强异常安全：不抛出异常时保持每次绘制最后项目的状态。
	\sa AMUnitControlList::MakeIterator
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;

	/*!
	\brief 复位视图：调用 MUnitList::ResetView 和 UpdateView 。
	\sa MUnitList::ResetView
	\sa UpdateView
	*/
	void
	ResetView();

	/*!
	\brief 选择第一个项目。
	\note 仅操作 vwList 并调整视图偏移量，不更新视图。
	*/
	void
	SelectFirst();

	/*!
	\brief 选择最后一个项目。
	\note 仅操作 vwList 并调整视图偏移量，不更新视图。
	*/
	void
	SelectLast();

	//! \since build 530
	//@{
	WidgetIterator
	begin();

	WidgetIterator
	end();
	//@}
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

