/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file listbox.cpp
\ingroup Shell
\brief 样式相关的图形用户界面列表框控件。
\version 0.3601;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-03-07 20:33:05 +0800;
\par 修改时间:
	2011-04-22 12:42 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::ListBox;
*/


#include "listbox.h"
#include "ywindow.h"
#include "ystyle.h"
#include "ytext.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

namespace
{
	const SDst defMinScrollBarWidth(16); //!< 默认最小滚动条宽。
//	const SDst defMinScrollBarHeight(16); //!< 默认最小滚动条高。
}


YListBox::YListBox(const Rect& r, GHWeak<ListType> wpList_)
	: YComponent(),
	ScrollableContainer(r),
	TextListBox(Rect(Point::Zero, r), wpList_)
{
	TextListBox.GetContainerPtr() = this;
	VerticalScrollBar.GetTrack().GetScroll().Add(*this,
		&YListBox::OnScroll_VerticalScrollBar);
	TextListBox.GetViewChanged().Add(*this,
		&YListBox::OnViewChanged_TextListBox);
	//刷新文本状态，防止第一次绘制时无法正确决定是否需要滚动条。
	TextListBox.RefreshTextState();
}

IControl*
YListBox::GetTopControlPtr(const Point& p)
{
	IControl* pCon(ScrollableContainer::GetTopControlPtr(p));

	if(pCon == this)
		return &TextListBox;
	return pCon;
}

void
YListBox::Paint()
{
	YWidgetAssert(this, Controls::YListBox, Draw);

	ScrollableContainer::Paint();
	TextListBox.Paint();
}

void
YListBox::OnScroll_VerticalScrollBar(ScrollEventArgs& e)
{
	TextListBox.LocateViewPosition(e.Value);
	Refresh();
}

void
YListBox::OnViewChanged_TextListBox(EventArgs&)
{
	if(GetWidth() > defMinScrollBarWidth)
	{
		Size view_arena(TextListBox.GetFullViewSize());

		view_arena.Width = GetWidth() - defMinScrollBarWidth;
		TextListBox.SetSize(FixLayout(view_arena));
		VerticalScrollBar.SetSmallDelta(TextListBox.GetItemHeight());
		VerticalScrollBar.SetMaxValue(view_arena.Height);
		VerticalScrollBar.SetLargeDelta(TextListBox.GetHeight());
		VerticalScrollBar.SetValue(TextListBox.GetViewPosition());
	}
}


YFileBox::YFileBox(const Rect& r)
	: FileList(), YListBox(r, GetListWeakPtr())
{
	GetConfirmed().Add(*this, &YFileBox::OnConfirmed);
	ListItems();
	UpdateView();
}

IO::Path
YFileBox::GetPath() const
{
	if(IsSelected() && GetSelectedIndex() >= 0)
		return Directory / (GetList()[GetSelectedIndex()]);
	return Directory;
}

void
YFileBox::OnConfirmed(IndexEventArgs& e)
{
	if(Contains(e) && static_cast<bool>(*this /= GetList()[e.Index]))
	{
		ListItems();
		ResetView();
	}
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

