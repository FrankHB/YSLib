/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file listbox.cpp
\ingroup UI
\brief 样式相关的图形用户界面列表框控件。
\version r3739;
\author FrankHB<frankhb1989@gmail.com>
\since build 194 。
\par 创建时间:
	2011-03-07 20:33:05 +0800;
\par 修改时间:
	2012-01-05 15:54 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::ListBox;
*/


#include "YSLib/UI/listbox.h"
#include "YSLib/UI/ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

namespace
{
	const SDst defMinScrollBarWidth(16); //!< 默认最小滚动条宽。
//	const SDst defMinScrollBarHeight(16); //!< 默认最小滚动条高。
}


ListBox::ListBox(const Rect& r, const shared_ptr<ListType>& h)
	: ScrollableContainer(r),
	TextListBox(Rect(Point::Zero, r), h)
{
	SetContainerPtrOf(TextListBox, this);
	VerticalScrollBar.GetTrack().GetScroll() += [this](ScrollEventArgs&& e){
		TextListBox.LocateViewPosition(SDst(round(e.GetValue())));
	};
	TextListBox.GetViewChanged() += [this](ViewArgs&& e){
		if(!e.Value && GetWidth() > defMinScrollBarWidth)
		{
			Size view_arena(TextListBox.GetFullViewSize());

			YAssert(view_arena.Height > 1, "Invalid size found"
				" @ ListBox::ListBox;");

			view_arena.Width = GetWidth() - defMinScrollBarWidth;
			SetSizeOf(TextListBox, FixLayout(view_arena));
			if(view_arena.Height > TextListBox.GetHeight())
			{
				VerticalScrollBar.SetSmallDelta(TextListBox.GetItemHeight());
				VerticalScrollBar.SetMaxValue(view_arena.Height
					- TextListBox.GetHeight());
				VerticalScrollBar.SetLargeDelta(TextListBox.GetHeight());
				VerticalScrollBar.SetValue(TextListBox.GetViewPosition());
			}
		}
	};
	//刷新文本状态，防止第一次绘制时无法正确决定是否需要滚动条。
	TextListBox.RefreshTextState();
}

IWidget*
ListBox::GetTopWidgetPtr(const Point& pt, bool(&f)(const IWidget&))
{
	auto pCtl(ScrollableContainer::GetTopWidgetPtr(pt, f));

	pCtl = pCtl ? pCtl : &TextListBox;
	return f(*pCtl) ? pCtl : nullptr;
}

Rect
ListBox::Refresh(const PaintContext& pc)
{
	auto r(ScrollableContainer::Refresh(pc));

	PaintChild(TextListBox, pc);
	return r;
}


FileBox::FileBox(const Rect& r)
	: FileList(), ListBox(r, GetListPtr())
{
	GetConfirmed() += [this](IndexEventArgs&& e){
		if(Contains(e) && bool(*this /= GetList()[e.Value]))
		{
			ListItems();
			ResetView();
		}
	};
	ListItems();
	UpdateView();
}

IO::Path
FileBox::GetPath() const
{
	if(IsSelected() && GetSelectedIndex() >= 0)
		return Directory / (GetList()[GetSelectedIndex()]);
	return Directory;
}

YSL_END_NAMESPACE(Components)

YSL_END

