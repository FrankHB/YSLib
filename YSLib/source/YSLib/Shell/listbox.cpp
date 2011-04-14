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
\version 0.3526;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-03-07 20:33:05 +0800;
\par 修改时间:
	2011-04-13 08:03 +0800;
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
	const SDst defMarginH(2); //!< 默认水平边距。
	const SDst defMarginV(1); //!< 默认垂直边距。
	const SDst defMinScrollBarWidth(16); //!< 默认最小滚动条宽。
//	const SDst defMinScrollBarHeight(16); //!< 默认最小滚动条高。
}


YSimpleListBox::Dependencies::Dependencies()
{
	Selected.GetRef() += &YSimpleListBox::OnSelected;
	Confirmed.GetRef() += &YSimpleListBox::OnConfirmed;
}

YSimpleListBox::YSimpleListBox(const Rect& r, GHWeak<ListType> wpList_)
	: YControl(r),
	Font(), Margin(defMarginH, defMarginH, defMarginV, defMarginV),
	wpList(wpList_), viewer(GetList()), top_offset(0), text_state(Font),
	Events(GetStaticRef<Dependencies>())
{
	FetchEvent<KeyDown>(*this) += &YSimpleListBox::OnKeyDown;
	FetchEvent<KeyHeld>(*this) += OnKeyHeld;
	FetchEvent<TouchDown>(*this) += &YSimpleListBox::OnTouchDown;
	FetchEvent<TouchMove>(*this) += &YSimpleListBox::OnTouchMove;
	FetchEvent<Click>(*this) += &YSimpleListBox::OnClick;
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
SDst
YSimpleListBox::GetItemHeight() const
{
	return GetLnHeightExFrom(text_state);
}
SDst
YSimpleListBox::GetFullViewHeight() const
{
	return GetItemHeight() * viewer.GetTotal();
}
Size
YSimpleListBox::GetFullViewSize() const
{
	return Size(GetWidth(), GetFullViewHeight());
}
SDst
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
YSimpleListBox::SetSelected(SPos x, SPos y)
{
	SetSelected(CheckPoint(x, y));
}

void
YSimpleListBox::Draw()
{
	YWidgetAssert(this, Controls::YSimpleListBox, Draw);

	ParentType::Draw();

	IWindow* pWnd(FetchDirectWindowPtr(*this));

	if(pWnd)
	{
		if(IsFocused())
			WndDrawFocus(pWnd, GetSize());

		const SDst h(GetHeight());

		if(h != 0)
		{
			GetTextState();

			const SDst ln_w(GetWidth());
			const SDst ln_h(GetItemHeight());

			viewer.SetLength((GetHeight() + top_offset + ln_h - 1) / ln_h);
			if(viewer.GetHeadIndex() >= 0)
			{
				const ViewerType::IndexType last(viewer.GetHeadIndex()
					+ viewer.GetValid());
				const ListType& list(GetList());
				const Graphics& g(pWnd->GetContext());
				const Point pt(LocateForWindow(*this));
				SPos y(-top_offset);

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

SDst
YSimpleListBox::AdjustTopOffset()
{
	viewer.RestrictSelected();

	SDst d(top_offset);

	top_offset = 0;
	return d;
}

SDst
YSimpleListBox::AdjustBottomOffset()
{
	if(GetFullViewHeight() <= GetHeight())
		return 0;
	viewer.RestrictSelected();

	const SDst item_height(GetItemHeight()),
		down_offset(GetHeight() % item_height);

	top_offset = item_height - down_offset;
	return down_offset;
}

YSimpleListBox::ViewerType::IndexType
YSimpleListBox::CheckPoint(SPos x, SPos y)
{
	return Rect(Point::Zero, GetSize()).Contains(x, y)
		? (y + top_offset) / GetItemHeight() + viewer.GetHeadIndex() : -1;
}

void
YSimpleListBox::LocateViewPosition(SDst h)
{
	RestrictInInterval(h, 0, GetFullViewHeight());

	if(GetViewPosition() != h)
	{
		const SDst item_height(GetItemHeight());

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
	GetViewChanged()(*this, GetStaticRef<EventArgs>());
	Refresh();
}

void
YSimpleListBox::CallSelected()
{
	IndexEventArgs e(*this, viewer.GetSelectedIndex());

	GetSelected()(*this, e);
}

void
YSimpleListBox::CheckConfirmed(YSimpleListBox::ViewerType::IndexType i)
{
	if(viewer.IsSelected() && viewer.GetSelectedIndex() == i)
	{
		IndexEventArgs e(*this, i);

		GetConfirmed()(*this, e);
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
					viewer.DecreaseSelected(viewer.GetLength());
					AdjustTopOffset();
					break;
				case KeySpace::PgDn:
					viewer.IncreaseSelected(viewer.GetLength());
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
	TextListBox.GetTextState();
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
YListBox::Draw()
{
	YWidgetAssert(this, Controls::YListBox, Draw);

	ScrollableContainer::Draw();
	TextListBox.Draw();
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

