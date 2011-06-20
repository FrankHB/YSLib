/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yviewer.hpp
\ingroup UI
\brief 样式无关的视图。
\version 0.1090;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-19 23:00:28 +0800;
\par 修改时间:
	2011-06-16 19:51 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YViewer
*/


#ifndef YSL_INC_UI_YVIEWER_HPP_
#define YSL_INC_UI_YVIEWER_HPP_

#include "ycomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 序列视图类模板。
template<class _tContainer, typename _tSize = typename _tContainer::size_type,
		typename _tIndex = ssize_t>
class GSequenceViewer
{
public:
	typedef _tSize SizeType; //!< 项目下标类型。
	typedef _tIndex IndexType; //!< 项目索引类型。

private:
	_tContainer& c; //!< 序列容器引用。
	IndexType head, //!< 视图中首个项目下标，若不存在则为 -1 。
		selected; //!< 选中项目下标，大于等于 GetTotal() 时无效。
	SizeType length; //!< 视图长度：最大可视项目数。
	bool is_selected; //!< 选中状态。

public:
	/*!
	\brief 构造：使用指定容器。
	*/
	explicit
	GSequenceViewer(_tContainer& c_)
		: c(c_), head(0), selected(0), length(0), is_selected(false)
	{}

	inline PDefHOperator0(GSequenceViewer&, ++) //!< 选中项目下标自增。
		ImplRet(IncreaseSelected(1))
	inline PDefHOperator0(GSequenceViewer&, --) //!< 选中项目下标自减。
		ImplRet(IncreaseSelected(-1))
	inline PDefHOperator1(GSequenceViewer&, ++, int) \
		//!< 视图中首个项目下标自增。
		ImplRet(IncreaseHead(1))
	inline PDefHOperator1(GSequenceViewer&, --, int) \
		//!< 视图中首个项目下标自减。
		ImplRet(IncreaseHead(-1))

	/*!
	\brief 判断是否为选中状态。
	*/
	DefPredicate(Selected, is_selected)

	/*!
	\brief 判断是否在有效范围内包含指定项目索引。
	*/
	bool
	Contains(IndexType i) const
	{
		return i < head ? false
			: IsInInterval<IndexType>(i - GetHeadIndex(), GetLength());
	}

	DefGetter(SizeType, Total, c.size()) //!< 取容器中项目个数。
	DefGetter(SizeType, Length, length)
	DefGetter(IndexType, HeadIndex, head)
	DefGetter(IndexType, SelectedIndex, selected)
	DefGetter(IndexType, RelativeIndex, IsSelected()
		? GetSelectedIndex() - GetHeadIndex() : -1) \
		//!< 取选中的项目相对于视图中首个项目的下标偏移。
	DefGetter(SizeType, Valid, ystdex::vmin(GetTotal() - GetHeadIndex(),
		GetLength())) //!< 取当前视图中有效项目个数。

	/*!
	\brief 设置项目索引。
	*/
	bool
	SetHeadIndex(IndexType t)
	{
		if(GetTotal() && IsInInterval<IndexType>(t, GetTotal())
			&& t != head)
		{
			if(!t)
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
	SetSelectedIndex(IndexType t)
	{
		if(GetTotal() && IsInInterval<IndexType>(t, GetTotal())
			&& !(t == selected && is_selected))
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
	bool
	ClearSelected()
	{
		if(IsSelected())
		{
			is_selected = false;
			return true;
		}
		return false;
	}

	inline PDefH1(GSequenceViewer&, DecreaseHead, IndexType d) \
		//!< 视图中首个项目下标减少 d 。
		ImplRet(IncreaseHead(-d))

	inline PDefH1(GSequenceViewer&, DecreaseSelected, IndexType d) \
		//!< 选中项目下标减少 d 。
		ImplRet(IncreaseSelected(-d))

	/*!
	\brief 视图中首个项目下标增加 d 。
	*/
	inline GSequenceViewer&
	IncreaseHead(IndexType d)
	{
		SetHeadIndex(head + d);
		return *this;
	}

	/*!
	\brief 选中项目下标增加 d 。
	*/
	inline GSequenceViewer&
	IncreaseSelected(IndexType d)
	{
		SetSelectedIndex(selected + d);
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
		if(head < 0)
			return false;
		if(selected < head)
			selected = head;
		else if(selected >= head + static_cast<IndexType>(length))
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
		if(head < 0)
			return false;
		if(selected < head)
			head = selected;
		else if(selected >= head + static_cast<IndexType>(length))
			head = selected + 1 - length;
		else
			return false;
		return true;
	}
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

