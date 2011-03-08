/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Shell Shell
\ingroup YSLib
\brief YSLib Shell 模块。
*/

/*!	\file ycomp.h
\ingroup Shell
\brief 平台无关的 Shell 组件实现。
\version 0.3011;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-19 20:05:08 +0800;
\par 修改时间:
	2011-03-06 21:59 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YComponent;
*/


#ifndef INCLUDED_YCOMPONENT_H_
#define INCLUDED_YCOMPONENT_H_

#include "../Core/ysdef.h"
#include "../Core/ycounter.hpp"
#include "../Core/yobject.h"
#include "../Helper/yglobal.h"
#include "../Core/yshell.h"
#include "../Core/yapp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)
PDeclInterface(IControl)
YSL_END_NAMESPACE(Controls)

YSL_BEGIN_NAMESPACE(Forms)
PDeclInterface(IWindow)
class YFrameWindow;
class YForm;
YSL_END_NAMESPACE(Forms)

YSL_BEGIN_NAMESPACE(Widgets)
PDeclInterface(IWidget)
PDeclInterface(IUIBox)
PDeclInterface(IUIContainer)
YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

using Components::Controls::IControl;
using Components::Forms::IWindow;
using Components::Widgets::IWidget;
using Components::Widgets::IUIBox;
using Components::Widgets::IUIContainer;

DeclareHandle(IWindow, HWND)

YSL_BEGIN_NAMESPACE(Components)

//! \brief 基本组件接口。
DeclInterface(IComponent)
EndDecl


//! \brief 基本组件。
class YComponent : public GMCounter<YComponent>, public YCountableObject,
	implements IComponent
{
public:
	typedef YCountableObject ParentType;
};


//! \brief 序列视图类模板。
template<class _tContainer, typename _tSize = typename _tContainer::size_type,
		typename _tIndex = std::ptrdiff_t>
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
		ImplRet(*this += 1)
	inline PDefHOperator0(GSequenceViewer&, --) //!< 选中项目下标自减。
		ImplRet(*this -= 1)
	inline PDefHOperator1(GSequenceViewer&, ++, int) \
		//!< 视图中首个项目下标自增。
		ImplRet(*this >> 1)
	inline PDefHOperator1(GSequenceViewer&, --, int) \
		//!< 视图中首个项目下标自减。
		ImplRet(*this >> -1)
	/*!
	\brief 视图中首个项目下标增加 d 。
	*/
	inline GSequenceViewer&
	operator>>(IndexType d)
	{
		SetHeadIndex(head + d);
		return *this;
	}
	inline PDefHOperator1(GSequenceViewer&, <<, IndexType d) \
		//!< 视图中首个项目下标减少 d 。
		ImplRet(*this >> -d)
	/*!
	\brief 选中项目下标增加 d 。
	*/
	inline GSequenceViewer&
	operator+=(IndexType d)
	{
		SetSelectedIndex(selected + d);
		return *this;
	}
	inline PDefHOperator1(GSequenceViewer&, -=, IndexType d) \
		//!< 选中项目下标减少 d 。
		ImplRet(*this += -d)

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
};


//! \brief 控制台。
class YConsole : public YComponent
{
public:
	typedef YComponent ParentType;

	YScreen& Screen;

	/*!
	\brief 构造：使用指定屏幕、有效性、前景色和背景色。
	*/
	explicit
	YConsole(YScreen& = theApp.GetPlatformResource().GetDefaultScreen(),
		bool = true,
		Drawing::Color = Drawing::ColorSpace::White,
		Drawing::Color = Drawing::ColorSpace::Black);
	/*!
	\brief 析构。
	\note 自动停用。
	*/
	virtual
	~YConsole();

	/*!
	\brief 激活：使用指定屏幕、有效性、前景色和背景色。
	*/
	friend void
	Activate(YConsole&, Drawing::Color = Drawing::ColorSpace::White,
		Drawing::Color = Drawing::ColorSpace::Black);

	/*!
	\brief 停用。
	*/
	friend void
	Deactivate(YConsole&);

	void
	Pause();
};

inline
YConsole::YConsole(YScreen& scr, bool a, Drawing::Color fc, Drawing::Color bc)
	: YComponent(),
	Screen(scr)
{
	if(a)
		Activate(*this, fc, bc);
}
inline
YConsole::~YConsole()
{
	Deactivate(*this);
}

inline void
YConsole::Pause()
{
	WaitForInput();
}

YSL_END_NAMESPACE(Components)

using Components::Forms::YForm;
using Components::Forms::YFrameWindow;

YSL_END

#endif

