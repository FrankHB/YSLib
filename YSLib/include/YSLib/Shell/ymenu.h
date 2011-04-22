/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ymenu.h
\ingroup Shell
\brief 样式相关的菜单。
\version 0.1150;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-19 22:59:02 +0800;
\par 修改时间:
	2011-04-22 12:53 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YMenu
*/


#ifndef YSL_INC_SHELL_YMENU_H_
#define YSL_INC_SHELL_YMENU_H_

#include "ycontrol.h"
#include "yviewer.hpp"
#include "ylabel.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

//! \brief 文本菜单。
class YMenu : public GMCounter<YMenu>, public YControl,
	public Widgets::MTextList
{
public:
	typedef YControl ParentType;
	typedef GSequenceViewer<ListType> ViewerType; //!< 视图类型。

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
	\brief 构造：使用指定边界和文本列表。
	*/
	explicit
	YMenu(const Rect& = Rect::Empty, GHWeak<ListType> = NULL);

public:
	DefPredicateMember(Selected, viewer)
	PDefH1(bool, Contains, ViewerType::IndexType i)
		ImplBodyMember1(viewer, Contains, i)

	DefMutableDepEventGetter(HVisualEvent, ViewChanged, Events.ViewChanged) \
		//!< 视图变更事件。
	DefMutableDepEventGetter(HIndexEvent, Selected, Events.Selected) \
		//!< 项目选择状态变更事件。
	DefMutableDepEventGetter(HIndexEvent, Confirmed, Events.Confirmed) \
		//!< 项目选中确定事件。

	DefGetterMember(ViewerType::IndexType, HeadIndex, viewer)
	DefGetterMember(ViewerType::IndexType, SelectedIndex, viewer)

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
	SetSelected(ViewerType::IndexType);
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
	\brief 绘制界面。
	*/
	virtual void
	Paint();

protected:
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
	ViewerType::IndexType
	CheckPoint(SPos, SPos);

	/*!
	\brief 检查点（相对于所在缓冲区的控件坐标）是否在选择范围内，
	\return 选择的项目索引。
	*/
	ViewerType::IndexType
	CheckPoint(const Point&);

public:
	PDefH0(void, ClearSelected)
		ImplRet(static_cast<void>(viewer.ClearSelected()))

	/*!
	\brief 定位视图顶端至指定垂直位置。
	*/
	void
	LocateViewPosition(SDst);

	/*!
	\brief 复位视图。
	\note 若项目列表非空则选择首个项目。
	*/
	void
	ResetView();

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
	CheckConfirmed(ViewerType::IndexType);

	/*!
	\brief 处理键接触开始事件。
	*/
	void
	OnKeyDown(KeyEventArgs&);

	/*!
	\brief 处理屏幕接触开始事件。
	*/
	void
	OnTouchDown(TouchEventArgs&);

	/*!
	\brief 处理屏幕接触移动事件。
	*/
	void
	OnTouchMove(TouchEventArgs&);

	/*!
	\brief 处理屏幕点击事件。
	*/
	void
	OnClick(TouchEventArgs&);

	/*!
	\brief 处理选中事件。
	*/
	void
	OnSelected(IndexEventArgs&);

	/*!
	\brief 处理确认事件。
	*/
	void
	OnConfirmed(IndexEventArgs&);
};

inline void
YMenu::SetSelected(const Point& pt)
{
	SetSelected(pt.X, pt.Y);
}

inline YMenu::ViewerType::IndexType
YMenu::CheckPoint(const Point& p)
{
	return CheckPoint(p.X, p.Y);
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

