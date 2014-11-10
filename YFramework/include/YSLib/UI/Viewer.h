/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Viewer.h
\ingroup UI
\brief 样式无关的视图。
\version r684
\author FrankHB <frankhb1989@gmail.com>
\since build 203
\par 创建时间:
	2011-04-19 23:00:28 +0800
\par 修改时间:
	2014-11-09 13:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Viewer
*/


#ifndef YSL_INC_UI_viewer_hpp_
#define YSL_INC_UI_viewer_hpp_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YComponent
#include <ystdex/algorithm.hpp>

namespace YSLib
{

namespace UI
{

/*!
\brief 序列视图。
\warning 非虚析构。
\since build 525
*/
class YF_API SequenceViewer
{
private:
	/*!
	\brief 视图中首个项目的索引，大于序列大小时无效。
	\note 空序列时应等于 0 。
	*/
	size_t head = 0;
	size_t selected = 0; //!< 选中项目的索引，大于等于序列大小时无效。
	//! \since build 147
	bool is_selected = {}; //!< 选中状态。

public:
	size_t Length = 1; //!< 视图长度：最大可视项目数。

	/*!
	\brief 构造：使用指定容器。
	\post <tt>GetHeadIndex() == 0 && GetSelectedIndex() == 0 && Length == 1
		&& !IsSelected()</tt> 。
	*/
	SequenceViewer() = default;

	/*!
	\brief 判断是否为选中状态。
	\since build 147
	*/
	DefPred(const ynothrow, Selected, is_selected)

	/*!
	\brief 判断是否在有效范围内包含指定项目索引。
	\since build 147
	*/
	bool
	Contains(size_t) const;

	DefGetter(const ynothrow, size_t, HeadIndex, head)
	DefGetter(const ynothrow, size_t, SelectedIndex, selected)
	DefGetter(const ynothrow, ptrdiff_t, Offset, IsSelected()
		? GetSelectedIndex() - GetHeadIndex() : -1) \
		//!< 取选中的项目相对视图中首个项目的的索引偏移（未选中时为 -1 ）。
	/*!
	\brief 取当前视图中有效项目个数。
	\param total 外部列表长度。
	\pre 断言：外部列表长度不小于首个项目的索引。
	*/
	size_t
	GetValid(size_t total) const ynothrow;

	/*!
	\brief 设置视图中首个项目的索引。
	\param t 设置的索引。
	\param total 外部列表长度。
	*/
	bool
	SetHeadIndex(size_t t, size_t total);
	/*!
	\brief 设置选中项目的索引。
	\param t 设置的索引。
	\param total 外部列表长度。
	*/
	bool
	SetSelectedIndex(size_t t, size_t total);

	/*!
	\brief 按序列内容大小依次调整选中和首个项目的索引。
	\post <tt>(total == 0 && !IsSelected()) || (GetSelectedIndex() < total
		&& GetHeadIndex() < total)</tt> 。
	\param total 外部列表长度。
	\return 是否改变了视图中首个项目的索引。
	*/
	bool
	AdjustForContent(size_t total);

	//! \brief 检查列表中的指定项是否被选中。
	PDefH(bool, CheckSelected, size_t idx) const
		ImplRet(IsSelected() && GetSelectedIndex() == idx)

	/*!
	\brief 取消选中状态。
	\since build 147
	*/
	PDefH(void, ClearSelected, )
		ImplExpr(is_selected = {})

	/*!
	\brief 按偏移设置视图中首个项目的索引。
	\param d 增加的索引偏移值。
	*/
	SequenceViewer&
	IncreaseHead(ptrdiff_t d, size_t total);

	/*!
	\brief 按偏移设置选中项目的索引。
	\param d 增加的索引偏移值。
	\param total 外部列表长度。
	*/
	SequenceViewer&
	IncreaseSelected(ptrdiff_t d, size_t total);

	/*!
	\brief 移动视图至序列起始。
	\since build 147
	*/
	bool
	MoveViewerToBegin();

	/*!
	\brief 移动视图至序列结尾。
	\param total 外部列表长度。
	*/
	bool
	MoveViewerToEnd(size_t total);

	/*!
	\brief 复位视图。
	\since build 298
	*/
	PDefH(void, Reset, )
		ImplUnseq(head = 0, selected = 0, Length = 1, is_selected = {})

	/*!
	\brief 约束被选中的元素在视图内。
	\post <tt>(!(GetSelectedIndex() < GetHeadIndex())
		&& GetSelectedIndex() < GetHeadIndex() + Length)</tt> 。
	\return 是否改变了视图中选中项目的索引。
	\since build 147
	*/
	bool
	RestrictSelected();

	/*!
	\brief 约束视图包含被选中的元素。
	\post <tt>(!(GetSelectedIndex() < GetHeadIndex())
		&& GetSelectedIndex() < GetHeadIndex() + Length)</tt> 。
	\return 是否改变了视图中首个项目的索引。
	\since build 147
	*/
	bool
	RestrictView();
};


/*!
\ingroup UIModels
\brief 单元部件列表模块。
\since build 527
*/
class YF_API AMUnitList
{
protected:
	//! \brief 列表视图。
	SequenceViewer vwList{};
	/*!
	\brief 列表视图首项目超出上边界的竖直偏移量。
	\since build 523
	*/
	SDst uTopOffset = 0;
	//! \since build 523
	size_t idxShared = size_t(-1);

public:
	virtual DefDeDtor(AMUnitList)

	/*!
	\brief 循环选择遍历。
	\since build 222
	*/
	bool CyclicTraverse = {};

	/*!
	\brief 取完整视图高。
	\note 依赖 GetItemHeight 的结果。
	\since build 203
	*/
	SDst
	GetFullViewHeight() const;
	/*!
	\brief 取项目行高。
	\note 默认实现为取单元部件的高。
	\since build 203
	*/
	virtual SDst
	GetItemHeight() const;
	//! \since build 523
	size_t
	GetLastLabelIndexClipped(SPos, SDst) const;
	//! \since build 528
	DefGetter(const ynothrow, size_t, SharedIndex, idxShared)
	//! \since build 528
	DefGetter(const ynothrow, SDst, TopOffset, uTopOffset)
	//! \brief 取单元总数。
	DeclIEntry(SDst GetTotal() const)
	//! \brief 取单元部件引用。
	DeclIEntry(IWidget& GetUnitRef() const)
	/*!
	\brief 取单元部件的位置。
	\note 参数为项目索引。
	\since build 523
	*/
	Point
	GetUnitLocation(size_t) const;
	/*!
	\brief 取视图顶端竖直位置。
	\note 依赖 GetItemHeight 的结果。
	\since build 203
	*/
	SDst
	GetViewPosition() const;

	/*!
	\brief 调整列表视图底项目（可能不完全）超出下边界以上的竖直偏移量为零。
	\post 若调整则 <tt>uTopOffset == 0</tt> 。
	\return 返回调整前的偏移量值（取值区间 [0, <tt>GetItemHeight()</tt>) ）。
	\note 不限定选择项。

	第一参数指定单元部件的高。
	第二参数指定视图可视区域的高。
	*/
	SDst
	AdjustBottomForHeight(SDst, SDst);

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
	\brief 根据指定的单元部件的高和视图可视区域的高调整视图长度。
	\note 视图长为当项目数足够时所有在视图中显示的（可能不完全）项目总数。
	\note 当视图可视区域的高为 0 时忽略。
	\warning 设置大小或列表内容后不调用此方法可能导致显示错误。
	*/
	void
	AdjustViewLengthForHeight(SDst, SDst);

	/*!
	\brief 检查点（相对左上角）是否在选择范围内，
	\return 选择的项目索引，若无效则为 static_cast<size_t>(-1) 。
	\since build 528
	*/
	size_t
	CheckPoint(const Size&, const Point&);

	/*!
	\brief 复位视图。
	\note 若项目列表非空则选择首个项目。
	*/
	void
	ResetView();
	//@}
};

} // namespace UI;

} // namespace YSLib;

#endif

