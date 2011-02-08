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
\version 0.2938;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-19 20:05:08 + 08:00;
\par 修改时间:
	2011-01-21 14:34 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YComponent;
*/


#ifndef INCLUDED_YCOMPONENT_H_
#define INCLUDED_YCOMPONENT_H_

#include "../Core/yapp.h"
#include "../Core/yevt.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)
PDeclInterface(IControl)
PDeclInterface(IVisualControl)
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
using Components::Controls::IVisualControl;
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
template<class _tContainer>
class GSequenceViewer
{
public:
	typedef typename _tContainer::size_type SizeType; //!< 项目下标类型。
	typedef std::ptrdiff_t IndexType; //!< 项目索引类型。

private:
	_tContainer& c; //!< 序列容器引用。
	IndexType nIndex, //!< 项目索引：视图中首个项目下标，若不存在则为 -1 。
		nSelected; //!< 选中项目下标，大于等于 GetTotal() 时无效。
	SizeType nLength; //!< 视图长度：最大可视项目数。
	bool bSelected; //!< 选中状态。

public:
	/*!
	\brief 构造：使用指定容器。
	*/
	explicit
	GSequenceViewer(_tContainer& c_)
	: c(c_), nIndex(0), nSelected(0), nLength(0), bSelected(false)
	{}

	inline PDefHOperator0(GSequenceViewer&, ++) //!< 选中项目下标自增。
		ImplRet(*this += 1)
	inline PDefHOperator0(GSequenceViewer&, --) //!< 选中项目下标自减。
		ImplRet(*this -= 1)
	inline PDefHOperator1(GSequenceViewer&, ++, int) //!< 视图中首个项目下标自增。
		ImplRet(*this >> 1)
	inline PDefHOperator1(GSequenceViewer&, --, int) //!< 视图中首个项目下标自减。
		ImplRet(*this >> -1)
	/*!
	\brief 视图中首个项目下标增加 d 。
	*/
	inline GSequenceViewer&
	operator>>(IndexType d)
	{
		SetIndex(nIndex + d);
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
		SetSelected(nSelected + d);
		return *this;
	}
	inline PDefHOperator1(GSequenceViewer&, -=, IndexType d) \
		//!< 选中项目下标减少 d 。
		ImplRet(*this += -d)

	/*!
	\brief 判断是否为选中状态。
	*/
	DefPredicate(Selected, bSelected)

	/*!
	\brief 判断是否在有效范围内包含指定项目索引。
	*/
	bool
	Contains(IndexType i)
	{
		return i < nIndex ? false
			: IsInInterval<IndexType>(i - GetIndex(), GetLength());
	}

	DefGetter(SizeType, Total, c.size()) //!< 取容器中项目个数。
	DefGetter(SizeType, Length, nLength)
	DefGetter(IndexType, Index, nIndex)
	DefGetter(IndexType, Selected, nSelected)
	DefGetter(SizeType, Valid, vmin(GetTotal() - GetIndex(), GetLength())) \
		//!< 取当前视图中有效项目个数。

	/*!
	\brief 设置项目索引。
	*/
	bool
	SetIndex(IndexType t)
	{
		if(GetTotal() && IsInInterval<IndexType>(t, GetTotal())
			&& t != nIndex)
		{
			if(!t)
				MoveViewerToBegin();
			else if(nLength + t > GetTotal())
				MoveViewerToEnd();
			else
				nIndex = t;
			RestrictSelected();
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
		if(l != nLength)
		{
			if(l < nLength && nSelected >= static_cast<IndexType>(l))
				nSelected = l - 1;
			else
				nLength = l;
			RestrictSelected();
			return true;
		}
		return false;
	}
	/*!
	\brief 设置选中项目下标。
	*/
	bool
	SetSelected(IndexType t)
	{
		if(GetTotal() && IsInInterval<IndexType>(t, GetTotal())
			&& !(t == nSelected && bSelected))
		{
			nSelected = t;
			RestrictViewer();
			bSelected = true;
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
			bSelected = false;
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
		if(nIndex < 0)
			return false;
		if(nSelected < nIndex)
			nSelected = nIndex;
		else if(nSelected >= nIndex + static_cast<IndexType>(nLength))
			nSelected = nIndex + nLength - 1;
		else
			return false;
		return true;
	}

	/*!
	\brief 约束视图包含被选中的元素。
	*/
	bool
	RestrictViewer()
	{
		if(nIndex < 0)
			return false;
		if(nSelected < nIndex)
			nIndex = nSelected;
		else if(nSelected >= nIndex + static_cast<IndexType>(nLength))
			nIndex = nSelected + 1 - nLength;
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
		if(nIndex)
		{
			nIndex = 0;
			return true;
		}
		return false;
	}

	/*!
	\brief 移图至序列结动视尾。
	*/
	bool
	MoveViewerToEnd()
	{
		if(GetTotal() >= nLength)
		{
			nIndex = GetTotal() - nLength;
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
	YConsole(YScreen& = *theApp.hDefaultScreen, bool = true,
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

YSL_BEGIN_NAMESPACE(Shells)

//! \brief 默认图形用户界面 Shell 。
class YGUIShell : public YShell
{
public:
	typedef YShell ParentType;
	typedef list<HWND> WNDs;

private:
	WNDs sWnds; //!< 窗口组。

public:
	/*!
	\brief 无参数构造。
	\note 向应用程序对象添加自身。
	*/
	YGUIShell();
	/*!
	\brief 析构。
	\note 无异常抛出。
			空实现。
	*/
	virtual
	~YGUIShell() ythrow();

	/*!
	\brief 向窗口组添加指定窗口句柄。
	\note 仅当句柄非空时添加。
	*/
	virtual void
	operator+=(HWND);
	/*!
	\brief 从窗口组中移除指定窗口句柄。
	*/
	virtual bool
	operator-=(HWND);
	/*!
	\brief 从窗口组中移除所有指定窗口句柄，返回移除的对象数。
	\note 使用 set 实现，因此返回值应为 0 或 1 。
	*/
	WNDs::size_type
	RemoveAll(HWND);
	/*!
	\brief 移除窗口队列中首个窗口对象。
	*/
	void
	RemoveWindow();

	/*!
	\brief 取得窗口组中首个窗口对象的句柄。
	*/
	HWND
	GetFirstWindowHandle() const;
	/*!
	\brief 取得窗口组中顶端窗口对象的句柄。
	*/
	HWND
	GetTopWindowHandle() const;
	/*!
	\brief 取得窗口组中指定屏幕的指定的点所处的最顶层窗口对象的句柄。
	*/
	HWND
	GetTopWindowHandle(YDesktop&, const Point&) const;

	/*!
	\brief 向屏幕发送指定窗口对象。
	*/
	bool
	SendWindow(IWindow&);

	/*!
	\brief 向屏幕分发窗口对象。
	*/
	void
	DispatchWindows();

	/*!
	\brief 清除指定屏幕中属于窗口组的窗口对象。
	*/
	void
	ClearScreenWindows(YDesktop&);

	/*!
	\brief Shell 处理函数。
	*/
	virtual int
	ShlProc(const Message&);
};

YSL_END_NAMESPACE(Shells)

using Components::Forms::YForm;
using Components::Forms::YFrameWindow;
using Shells::YGUIShell;

YSL_END

#endif

