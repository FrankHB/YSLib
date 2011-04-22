/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ymenu.cpp
\ingroup Shell
\brief 样式相关的菜单。
\version 0.1138;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-20 09:28:38 +0800;
\par 修改时间:
	2011-04-22 12:54 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YMenu
*/


#include "ymenu.h"
#include "ywindow.h"
#include "ystyle.h"

YSL_BEGIN

using namespace Drawing;
using namespace Runtime;

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

namespace
{
	const SDst defMarginH(2); //!< 默认水平边距。
	const SDst defMarginV(1); //!< 默认垂直边距。
	const SDst defMinScrollBarWidth(16); //!< 默认最小滚动条宽。
//	const SDst defMinScrollBarHeight(16); //!< 默认最小滚动条高。
}


YMenu::Dependencies::Dependencies()
{
	Selected.GetRef() += &YMenu::OnSelected;
	Confirmed.GetRef() += &YMenu::OnConfirmed;
}

YMenu::YMenu(const Rect& r, GHWeak<ListType> wp)
	: YControl(r),
	MTextList(wp),
	viewer(GetList()), top_offset(0), Events(GetStaticRef<Dependencies>())
{
	SetAllTo(Margin, defMarginH, defMarginV);
	FetchEvent<KeyDown>(*this) += &YMenu::OnKeyDown;
	FetchEvent<KeyHeld>(*this) += OnKeyHeld;
	FetchEvent<TouchDown>(*this) += &YMenu::OnTouchDown;
	FetchEvent<TouchMove>(*this) += &YMenu::OnTouchMove;
	FetchEvent<Click>(*this) += &YMenu::OnClick;
}

SDst
YMenu::GetFullViewHeight() const
{
	return GetItemHeight() * viewer.GetTotal();
}
Size
YMenu::GetFullViewSize() const
{
	return Size(GetWidth(), GetFullViewHeight());
}
SDst
YMenu::GetViewPosition() const
{
	return GetItemHeight() * viewer.GetHeadIndex() + top_offset;
}

void
YMenu::SetSelected(YMenu::ViewerType::IndexType i)
{
	if(viewer.Contains(i) && viewer.SetSelectedIndex(i))
		CallSelected();
}
void
YMenu::SetSelected(SPos x, SPos y)
{
	SetSelected(CheckPoint(x, y));
}

void
YMenu::Paint()
{
	YWidgetAssert(this, Controls::YMenu, Paint);

	ParentType::Paint();

	IWindow* pWnd(FetchDirectWindowPtr(*this));

	if(pWnd)
	{
		if(IsFocused())
			WndDrawFocus(pWnd, GetSize());

		const SDst h(GetHeight());

		if(h != 0)
		{
			RefreshTextState();

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
						GetTextState().Color = Drawing::ColorSpace::White;
						FillRect<PixelType>(g.GetBufferPtr(), g.GetSize(),
							Rect(pt.X + 1, top + 1, ln_w - 2, tmp - 1),
							ColorSpace::Aqua);
					}
					else
						GetTextState().Color = ForeColor;
					GetTextState().ResetForBounds(Rect(pt.X, top, ln_w, tmp),
						g.GetSize(), Margin);
					if(y < 0)
						GetTextState().PenY -= top_offset;
					DrawText(g, GetTextState(), list[i]);
					y += ln_h;
				}
			}
		}
	}
}

SDst
YMenu::AdjustTopOffset()
{
	viewer.RestrictSelected();

	SDst d(top_offset);

	top_offset = 0;
	return d;
}

SDst
YMenu::AdjustBottomOffset()
{
	if(GetFullViewHeight() <= GetHeight())
		return 0;
	viewer.RestrictSelected();

	const SDst item_height(GetItemHeight()),
		down_offset(GetHeight() % item_height);

	top_offset = item_height - down_offset;
	return down_offset;
}

YMenu::ViewerType::IndexType
YMenu::CheckPoint(SPos x, SPos y)
{
	return Rect(Point::Zero, GetSize()).Contains(x, y)
		? (y + top_offset) / GetItemHeight() + viewer.GetHeadIndex() : -1;
}

void
YMenu::LocateViewPosition(SDst h)
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
YMenu::ResetView()
{
	viewer.MoveViewerToBegin();
	if(viewer.IsSelected())
		viewer.SetSelectedIndex(0);
	top_offset = 0;
	UpdateView();
}

void
YMenu::UpdateView()
{
	GetViewChanged()(*this, GetStaticRef<EventArgs>());
	Refresh();
}

void
YMenu::CallSelected()
{
	IndexEventArgs e(*this, viewer.GetSelectedIndex());

	GetSelected()(*this, e);
}

void
YMenu::CheckConfirmed(YMenu::ViewerType::IndexType i)
{
	if(viewer.IsSelected() && viewer.GetSelectedIndex() == i)
	{
		IndexEventArgs e(*this, i);

		GetConfirmed()(*this, e);
	}
}

void
YMenu::OnKeyDown(KeyEventArgs& k)
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
YMenu::OnTouchDown(TouchEventArgs& e)
{
	SetSelected(e);
	UpdateView();
}

void
YMenu::OnTouchMove(TouchEventArgs& e)
{
	SetSelected(e);
	UpdateView();
}

void
YMenu::OnClick(TouchEventArgs& e)
{
	CheckConfirmed(CheckPoint(e));
}

void
YMenu::OnSelected(IndexEventArgs&)
{
	Refresh();
}

void
YMenu::OnConfirmed(IndexEventArgs& e)
{
	OnSelected(e);
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

