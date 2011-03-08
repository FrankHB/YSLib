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
\brief 样式相关的图形用户界面列表框控件实现。
\version 0.3456;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-03-07 20:33:05 +0800;
\par 修改时间:
	2011-03-07 22:27 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::ListBox;
*/


#include "listbox.h"
#include "../Core/yobject.h"
#include "ycontrol.h"
#include "yuicont.h"
#include "ystyle.h"
#include "../Core/ycutil.h"
#include "ywindow.h"
#include "ytext.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

namespace
{
	const SDST defMarginH(2); //!< 默认水平边距。
	const SDST defMarginV(1); //!< 默认垂直边距。
	const SDST defMinScrollBarWidth(16); //!< 默认最小滚动条宽。
//	const SDST defMinScrollBarHeight(16); //!< 默认最小滚动条高。
}


YSimpleListBox::YSimpleListBox(const Rect& r, IUIBox* pCon,
	GHWeak<ListType> wpList_)
	: YControl(r, pCon),
	Font(), Margin(defMarginH, defMarginH, defMarginV, defMarginV),
	wpList(wpList_), viewer(GetList()), top_offset(0), text_state(Font)
{
	FetchEvent<KeyDown>(*this) += &YSimpleListBox::OnKeyDown;
	FetchEvent<KeyHeld>(*this) += OnKeyHeld;
	FetchEvent<TouchDown>(*this) += &YSimpleListBox::OnTouchDown;
	FetchEvent<TouchMove>(*this) += &YSimpleListBox::OnTouchMove;
	FetchEvent<Click>(*this) += &YSimpleListBox::OnClick;
	Selected += &YSimpleListBox::OnSelected;
	Confirmed += &YSimpleListBox::OnConfirmed;
}

Drawing::TextState&
YSimpleListBox::GetTextState() ythrow()
{
	text_state.LineGap = GetVerticalFrom(Margin);
	text_state.Font.SetFont(Font);
	return text_state;
}
YSimpleListBox::ListType&
YSimpleListBox::GetList() const ythrow()
{
	YAssert(wpList,
		"In function \"Components::Controls::YSimpleListBox::ListType\n"
		"Components::Controls::YSimpleListBox::GetList()\":\n"
		"The list pointer is null.");

	return *wpList;
}
YSimpleListBox::ItemType*
YSimpleListBox::GetItemPtr(ViewerType::IndexType i)
{
	ListType& list(GetList());

	return IsInInterval<ViewerType::IndexType>(i, list.size())
		? &list[i] : NULL;
}
SDST
YSimpleListBox::GetItemHeight() const
{
	return GetLnHeightExFrom(text_state);
}
SDST
YSimpleListBox::GetFullViewHeight() const
{
	return GetItemHeight() * viewer.GetTotal();
}
Size
YSimpleListBox::GetFullViewSize() const
{
	return Size(GetWidth(), GetFullViewHeight());
}
SDST
YSimpleListBox::GetViewPosition() const
{
	return GetItemHeight() * viewer.GetHeadIndex() + top_offset;
}

void
YSimpleListBox::SetSelected(YSimpleListBox::ViewerType::IndexType i)
{
	if(viewer.Contains(i) && viewer.SetSelectedIndex(i))
		CallSelected();
}
void
YSimpleListBox::SetSelected(SPOS x, SPOS y)
{
	SetSelected(CheckPoint(x, y));
}

void
YSimpleListBox::DrawForeground()
{
	YWidgetAssert(this, Controls::YSimpleListBox, DrawForeground);

	ParentType::DrawForeground();

	IWindow* pWnd(FetchDirectWindowPtr(*this));

	if(pWnd)
	{
		if(IsFocused())
			WndDrawFocus(pWnd, GetSize());

		const SDST h(GetHeight());

		if(h != 0)
		{
			GetTextState();

			const SDST ln_w(GetWidth());
			const SDST ln_h(GetItemHeight());

			viewer.SetLength((GetHeight() + top_offset + ln_h - 1) / ln_h);
			if(viewer.GetHeadIndex() >= 0)
			{
				const ViewerType::IndexType last(viewer.GetHeadIndex()
					+ viewer.GetValid());
				const ListType& list(GetList());
				const Graphics& g(pWnd->GetContext());
				const Point pt(LocateForWindow(*this));
				SPOS y(-top_offset);

				for(ViewerType::IndexType i(viewer.GetHeadIndex());
					i < last; ++i)
				{
					int top(y), tmp(y + ln_h);

					RestrictInInterval<int>(top, 0, h);
					RestrictInInterval<int>(tmp, 0, h);
					tmp -= top;
					top += pt.Y;
					if(viewer.IsSelected() && i == viewer.GetSelectedIndex())
					{
						text_state.Color = Drawing::ColorSpace::White;
						FillRect<PixelType>(g.GetBufferPtr(), g.GetSize(),
							Rect(pt.X + 1, top + 1, ln_w - 2, tmp - 1),
							ColorSpace::Aqua);
					}
					else
						text_state.Color = ForeColor;
					text_state.ResetForBounds(Rect(pt.X, top, ln_w, tmp),
						g.GetSize(), Margin);
					if(y < 0)
						text_state.PenY -= top_offset;
					DrawText(g, text_state, list[i]);
					y += ln_h;
				}
			}
		}
	}
}

SDST
YSimpleListBox::AdjustTopOffset()
{
	viewer.RestrictSelected();

	SDST d(top_offset);

	top_offset = 0;
	return d;
}

SDST
YSimpleListBox::AdjustBottomOffset()
{
	if(GetFullViewHeight() <= GetHeight())
		return 0;
	viewer.RestrictSelected();

	const SDST item_height(GetItemHeight()),
		down_offset(GetHeight() % item_height);

	top_offset = item_height - down_offset;
	return down_offset;
}

YSimpleListBox::ViewerType::IndexType
YSimpleListBox::CheckPoint(SPOS x, SPOS y)
{
	return Rect(Point::Zero, GetSize()).Contains(x, y)
		? (y + top_offset) / GetItemHeight() + viewer.GetHeadIndex() : -1;
}

void
YSimpleListBox::LocateViewPosition(SDST h)
{
	RestrictInInterval(h, 0, GetFullViewHeight());

	if(GetViewPosition() != h)
	{
		const SDST item_height(GetItemHeight());

		viewer.SetHeadIndex(h / item_height);
		top_offset = h % item_height;
	}
}

void
YSimpleListBox::ResetView()
{
	viewer.MoveViewerToBegin();
	if(viewer.IsSelected())
		viewer.SetSelectedIndex(0);
	top_offset = 0;
	UpdateView();
}

void
YSimpleListBox::UpdateView()
{
	ViewChanged(*this, GetStaticRef<EventArgs>());
	Refresh();
}

void
YSimpleListBox::CallSelected()
{
	IndexEventArgs e(*this, viewer.GetSelectedIndex());

	Selected(*this, e);
}

void
YSimpleListBox::CheckConfirmed(YSimpleListBox::ViewerType::IndexType i)
{
	if(viewer.IsSelected() && viewer.GetSelectedIndex() == i)
	{
		IndexEventArgs e(*this, i);

		Confirmed(*this, e);
	}
}

void
YSimpleListBox::OnKeyDown(KeyEventArgs& k)
{
	if(viewer.IsSelected())
	{
		using namespace Runtime;

		switch(k.GetKey())
		{
		case KeySpace::Enter:
			CheckConfirmed(viewer.GetSelectedIndex());
			break;

		case KeySpace::ESC:
			ClearSelected();
			CallSelected();
			break;

		case KeySpace::Up:
		case KeySpace::Down:
		case KeySpace::PgUp:
		case KeySpace::PgDn:
			{
				const ViewerType::IndexType nOld(viewer.GetSelectedIndex());

				switch(k.GetKey())
				{
				case KeySpace::Up:
					--viewer;
					if(viewer.GetRelativeIndex() == 0)
						AdjustTopOffset();
					break;

				case KeySpace::Down:
					++viewer;
					if(viewer.GetRelativeIndex()
						== static_cast<int>(viewer.GetLength() - 1))
						AdjustBottomOffset();
					break;

				case KeySpace::PgUp:
					viewer -= viewer.GetLength();
					AdjustTopOffset();
					break;

				case KeySpace::PgDn:
					viewer += viewer.GetLength();
					AdjustBottomOffset();
					break;
				}
				if(viewer.GetSelectedIndex() != nOld)
					CallSelected();
			}
			break;

		default:
			return;
		}
		UpdateView();
	}
}

void
YSimpleListBox::OnTouchDown(TouchEventArgs& e)
{
	SetSelected(e);
	UpdateView();
}

void
YSimpleListBox::OnTouchMove(TouchEventArgs& e)
{
	SetSelected(e);
	UpdateView();
}

void
YSimpleListBox::OnClick(TouchEventArgs& e)
{
	CheckConfirmed(CheckPoint(e));
}

void
YSimpleListBox::OnSelected(IndexEventArgs&)
{
	Refresh();
}

void
YSimpleListBox::OnConfirmed(IndexEventArgs& e)
{
	OnSelected(e);
}


YListBox::YListBox(const Rect& r, IUIBox* pCon, GHWeak<ListType> wpList_)
	: YComponent(),
	ScrollableContainer(r, pCon),
	TextListBox(Rect(Point::Zero, r), this, wpList_)
{
	VerticalScrollBar.GetTrack().Scroll.Add(*this,
		&YListBox::OnScroll_VerticalScrollBar);
	TextListBox.ViewChanged.Add(*this, &YListBox::OnViewChanged_TextListBox);
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
YListBox::DrawForeground()
{
	YWidgetAssert(this, Controls::YListBox, DrawForeground);

	if(GetWidth() > defMinScrollBarWidth)
	{
		Size view_arena(TextListBox.GetFullViewSize());

		view_arena.Width = GetWidth() - defMinScrollBarWidth;
		TextListBox.SetSize(FixLayout(view_arena));
		ScrollableContainer::DrawForeground();
	}
	TextListBox.DrawForeground();
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
	VerticalScrollBar.SetMaxValue(TextListBox.GetFullViewHeight());
	VerticalScrollBar.SetValue(TextListBox.GetViewPosition());
	VerticalScrollBar.SetLargeDelta(TextListBox.GetHeight());
	VerticalScrollBar.SetSmallDelta(TextListBox.GetItemHeight());
}


YFileBox::YFileBox(const Rect& r, IUIBox* pCon)
	: FileList(), YListBox(r, pCon, GetListWeakPtr())
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

