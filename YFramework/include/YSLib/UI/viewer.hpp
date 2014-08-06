/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file viewer.hpp
\ingroup UI
\brief 样式无关的视图。
\version r348
\author FrankHB <frankhb1989@gmail.com>
\since build 203
\par 创建时间:
	2011-04-19 23:00:28 +0800
\par 修改时间:
	2014-08-01 12:04 +0800
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
\brief 序列视图类模板。
\since build 147
*/
template<class _tCon>
class GSequenceViewer
{
public:
	/*!
	\brief 容器类型。
	\since build 292
	*/
	using ContainerType = _tCon;
	using SizeType = typename _tCon::size_type; //!< 项目索引类型。
	using DifferenceType = typename _tCon::difference_type; \
		//!< 项目索引差值类型。

	//! \since build 392
	static_assert(std::is_unsigned<SizeType>::value,
		"Invalid size type found!");
	//! \since build 392
	static_assert(std::is_signed<DifferenceType>::value,
		"Invalid difference type found!");

private:
	/*!
	\brief 序列容器指针。
	\note 非空。
	\since build 433
	*/
	_tCon* p_con;
	SizeType head; //!< 视图中首个项目的索引，大于等于 GetTotal() 时无效。
	SizeType selected; //!< 选中项目的索引，大于等于 GetTotal() 时无效。
	SizeType length; //!< 视图长度：最大可视项目数。
	bool is_selected; //!< 选中状态。

public:
	/*!
	\brief 构造：使用指定容器。
	\post <tt>GetHeadIndex() == 0 && GetSelectedIndex() == 0 && GetLength() == 1
		&& !IsSelected()</tt> 。
	*/
	explicit
	GSequenceViewer(ContainerType& con)
		: p_con(&con), head(0), selected(0), length(1), is_selected(false)
	{}

	PDefHOp(GSequenceViewer&, ++, ) //!< 选中项目的索引自增。
		ImplRet(IncreaseSelected(1))
	PDefHOp(GSequenceViewer&, --, ) //!< 选中项目的索引自减。
		ImplRet(IncreaseSelected(-1))
	PDefHOp(GSequenceViewer&, ++, int) \
		//!< 视图中首个项目的索引自增。
		ImplRet(IncreaseHead(1))
	PDefHOp(GSequenceViewer&, --, int) \
		//!< 视图中首个项目的索引自减。
		ImplRet(IncreaseHead(-1))

	/*!
	\brief 判断是否为选中状态。
	*/
	DefPred(const ynothrow, Selected, is_selected)

	/*!
	\brief 判断是否在有效范围内包含指定项目索引。
	*/
	bool
	Contains(SizeType i) const
	{
		return GetTotal() != 0 && GetLength() != 0
			&& IsInInterval<SizeType>(i - GetHeadIndex(), GetLength());
	}

	//! \brief 取容器中项目个数。
	DefGetter(const ynothrow, SizeType, Total, p_con ? p_con->size() : 0)
	DefGetter(const ynothrow, SizeType, Length, length)
	DefGetter(const ynothrow, SizeType, HeadIndex, head)
	DefGetter(const ynothrow, SizeType, SelectedIndex, selected)
	DefGetter(const ynothrow, DifferenceType, Offset, IsSelected()
		? GetSelectedIndex() - GetHeadIndex() : -1) \
		//!< 取选中的项目相对于视图中首个项目的的索引偏移（未选中时为 -1 ）。
	DefGetter(const ynothrow, SizeType, Valid, min(GetTotal() - GetHeadIndex(),
		GetLength())) //!< 取当前视图中有效项目个数。

	/*!
	\brief 设置容器。
	\note 若容器和之前不同则重置视图。
	\since build 292
	*/
	void
	SetContainer(ContainerType& con)
	{
		if(YB_LIKELY(p_con != &con))
			yunseq(p_con = &con, selected = 0, head = 0, length = 1);
	}
	/*!
	\brief 设置视图中首个项目的索引。
	*/
	bool
	SetHeadIndex(SizeType t)
	{
		if(t < GetTotal() && t != head)
		{
			if(t == 0)
				MoveViewerToBegin();
			else if(length + t > GetTotal())
				MoveViewerToEnd();
			else
				head = t;
			return true;
		}
		return {};
	}
	/*!
	\brief 设置长度。
	*/
	bool
	SetLength(SizeType l)
	{
		if(l != length)
		{
			length = l;
			return true;
		}
		return {};
	}
	/*!
	\brief 设置选中项目的索引。
	*/
	bool
	SetSelectedIndex(SizeType t)
	{
		if(t < GetTotal() && !(t == selected && is_selected))
		{
			selected = t;
			RestrictView();
			is_selected = true;
			return true;
		}
		return {};
	}

	/*!
	\brief 按序列内容大小依次调整选中和首个项目的索引。
	\post <tt>(GetTotal() == 0 && !IsSelected()) || (GetSelectedIndex()
		< GetTotal() && GetHeadIndex() < GetTotal())</tt> 。
	\return 是否改变了视图中首个项目的索引。
	\since build 393
	*/
	bool
	AdjustForContent()
	{
		const auto total(GetTotal());

		if(total != 0)
		{
			if(!(selected < total))
				selected = total - 1;
			return RestrictView();
		}
		else
			Reset();
		return true;
	}

	/*!
	\brief 取消选中状态。
	*/
	PDefH(void, ClearSelected, )
		ImplExpr(is_selected = {})

	inline PDefH(GSequenceViewer&, DecreaseHead, DifferenceType d) \
		//!< 视图中首个项目的索引减少 d 。
		ImplRet(IncreaseHead(-d))

	inline PDefH(GSequenceViewer&, DecreaseSelected, DifferenceType d) \
		//!< 选中项目的索引减少 d 。
		ImplRet(IncreaseSelected(-d))

	/*!
	\brief 按偏移设置视图中首个项目的索引。
	\param d 增加的索引偏移值。
	*/
	GSequenceViewer&
	IncreaseHead(DifferenceType d)
	{
		int t(head + d);

		RestrictInInterval(t, 0, int(GetTotal()));
		SetHeadIndex(t);
		return *this;
	}

	/*!
	\brief 按偏移设置选中项目的索引。
	\param d 增加的索引偏移值。
	*/
	GSequenceViewer&
	IncreaseSelected(DifferenceType d)
	{
		int t(selected + d);

		RestrictInInterval(t, 0, int(GetTotal()));
		SetSelectedIndex(t);
		return *this;
	}

	/*!
	\brief 移动视图至序列起始。
	*/
	bool
	MoveViewerToBegin()
	{
		if(head)
		{
			head = 0;
			return true;
		}
		return {};
	}

	/*!
	\brief 移动视图至序列结尾。
	*/
	bool
	MoveViewerToEnd()
	{
		if(GetTotal() < length)
			return {};
		head = GetTotal() - length;
		return true;
	}

	/*!
	\brief 复位视图。
	\since build 298
	*/
	PDefH(void, Reset, )
		ImplUnseq(head = 0, selected = 0, length = 1, is_selected = {})

	/*!
	\brief 约束被选中的元素在视图内。
	\post <tt>GetTotal() == 0 || (!(GetSelectedIndex() < GetHeadIndex())
		&& GetSelectedIndex() < GetHeadIndex() + GetLength())</tt> 。
	\return 是否改变了视图中选中项目的索引。
	*/
	bool
	RestrictSelected()
	{
		if(GetTotal() == 0)
			return {};
		if(selected < head)
			selected = head;
		else if(selected < head + length)
			return {};
		else
			selected = head + length - 1;
		return true;
	}

	/*!
	\brief 约束视图包含被选中的元素。
	\post <tt>GetTotal() == 0 || (!(GetSelectedIndex() < GetHeadIndex())
		&& GetSelectedIndex() < GetHeadIndex() + GetLength())</tt> 。
	\return 是否改变了视图中首个项目的索引。
	*/
	bool
	RestrictView()
	{
		if(GetTotal() == 0)
			return {};
		if(selected < head)
			head = selected;
		else if(selected < head + length)
			return {};
		else
			head = selected + 1 - length;
		return true;
	}
};

} // namespace UI;

} // namespace YSLib;

#endif

