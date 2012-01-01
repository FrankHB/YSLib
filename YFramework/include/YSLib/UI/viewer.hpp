/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file viewer.hpp
\ingroup UI
\brief 样式无关的视图。
\version r1173;
\author FrankHB<frankhb1989@gmail.com>
\since build 203 。
\par 创建时间:
	2011-04-19 23:00:28 +0800;
\par 修改时间:
	2011-12-30 12:12 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Viewer
*/


#ifndef YSL_INC_UI_VIEWER_HPP_
#define YSL_INC_UI_VIEWER_HPP_

#include "ycomp.h"
#include <ystdex/algorithm.hpp>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 序列视图类模板。
\since build 147 。
*/
template<class _tContainer>
class GSequenceViewer
{
public:
	typedef typename _tContainer::size_type SizeType; //!< 项目下标类型。
	typedef typename _tContainer::difference_type DifferenceType; \
		//!< 项目索引差值类型。

private:
	_tContainer& c; //!< 序列容器引用。
	SizeType head; //!< 视图中首个项目下标，大于等于 GetTotal() 时无效。
	SizeType selected; //!< 选中项目下标，大于等于 GetTotal() 时无效。
	SizeType length; //!< 视图长度：最大可视项目数。
	bool is_selected; //!< 选中状态。

public:
	/*!
	\brief 构造：使用指定容器。
	*/
	explicit
	GSequenceViewer(_tContainer& c_)
		: c(c_), head(0), selected(0), length(1), is_selected(false)
	{}

	inline PDefHOp(GSequenceViewer&, ++) //!< 选中项目下标自增。
		ImplRet(IncreaseSelected(1))
	inline PDefHOp(GSequenceViewer&, --) //!< 选中项目下标自减。
		ImplRet(IncreaseSelected(-1))
	inline PDefHOp(GSequenceViewer&, ++, int) \
		//!< 视图中首个项目下标自增。
		ImplRet(IncreaseHead(1))
	inline PDefHOp(GSequenceViewer&, --, int) \
		//!< 视图中首个项目下标自减。
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

	DefGetter(const ynothrow, SizeType, Total, c.size()) //!< 取容器中项目个数。
	DefGetter(const ynothrow, SizeType, Length, length)
	DefGetter(const ynothrow, SizeType, HeadIndex, head)
	DefGetter(const ynothrow, SizeType, SelectedIndex, selected)
	DefGetter(const ynothrow, DifferenceType, Offset, IsSelected()
		? GetSelectedIndex() - GetHeadIndex() : -1) \
		//!< 取选中的项目相对于视图中首个项目的下标偏移（未选中时为 -1 ）。
	DefGetter(const ynothrow, SizeType, Valid, min(GetTotal() - GetHeadIndex(),
		GetLength())) //!< 取当前视图中有效项目个数。

	/*!
	\brief 设置项目索引。
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
		return false;
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
		return false;
	}
	/*!
	\brief 设置选中项目下标。
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
		return false;
	}

	/*!
	\brief 取消选中状态。
	*/
	inline void
	ClearSelected()
	{
		is_selected = false;
	}

	inline PDefH(GSequenceViewer&, DecreaseHead, DifferenceType d) \
		//!< 视图中首个项目下标减少 d 。
		ImplRet(IncreaseHead(-d))

	inline PDefH(GSequenceViewer&, DecreaseSelected, DifferenceType d) \
		//!< 选中项目下标减少 d 。
		ImplRet(IncreaseSelected(-d))

	/*!
	\brief 视图中首个项目下标增加 d 。
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
	\brief 选中项目下标增加 d 。
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
		return false;
	}

	/*!
	\brief 移动视图至序列结尾。
	*/
	bool
	MoveViewerToEnd()
	{
		if(GetTotal() >= length)
		{
			head = GetTotal() - length;
			return true;
		}
		return false;
	}

	/*!
	\brief 约束被选中的元素在视图内。
	*/
	bool
	RestrictSelected()
	{
		if(GetTotal() == 0)
			return false;
		if(selected < head)
			selected = head;
		else if(selected >= head + length)
			selected = head + length - 1;
		else
			return false;
		return true;
	}

	/*!
	\brief 约束视图包含被选中的元素。
	*/
	bool
	RestrictView()
	{
		if(GetTotal() == 0)
			return false;
		if(selected < head)
			head = selected;
		else if(selected >= head + length)
			head = selected + 1 - length;
		else
			return false;
		return true;
	}
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

