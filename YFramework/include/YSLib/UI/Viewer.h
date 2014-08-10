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
\version r555
\author FrankHB <frankhb1989@gmail.com>
\since build 203
\par 创建时间:
	2011-04-19 23:00:28 +0800
\par 修改时间:
	2014-08-08 19:22 +0800
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
\since build 525
*/
class YF_API SequenceViewer
{
private:
	size_t head = 0; //!< 视图中首个项目的索引，大于等于序列大小时无效。
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
		//!< 取选中的项目相对于视图中首个项目的的索引偏移（未选中时为 -1 ）。
	//! \brief 取当前视图中有效项目个数。
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

} // namespace UI;

} // namespace YSLib;

#endif

