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
\version 0.1214;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-20 09:28:38 +0800;
\par 修改时间:
	2011-05-17 02:42 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Menu
*/


#include "ymenu.h"
#include "ywindow.h"

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


Menu::Dependencies::Dependencies()
{
	Selected.GetRef() += &Menu::OnSelected;
	Confirmed.GetRef() += &Menu::OnConfirmed;
}

Menu::Menu(const Rect& r, const shared_ptr<ListType>& h,
	pair<Color, Color> hilight_pair)
	: Control(r), MTextList(h),
	HilightBackColor(hilight_pair.first), HilightTextColor(hilight_pair.second),
	viewer(GetList()), top_offset(0), Events(GetStaticRef<Dependencies>())
{
	SetAllTo(Margin, defMarginH, defMarginV);
	FetchEvent<KeyDown>(*this) += &Menu::OnKeyDown;
	FetchEvent<KeyHeld>(*this) += OnKeyHeld;
	FetchEvent<TouchDown>(*this) += &Menu::OnTouchDown;
	FetchEvent<TouchMove>(*this) += &Menu::OnTouchMove;
	FetchEvent<Click>(*this) += &Menu::OnClick;
}

SDst
Menu::GetFullViewHeight() const
{
	return GetItemHeight() * viewer.GetTotal();
}
Size
Menu::GetFullViewSize() const
{
	return Size(GetWidth(), GetFullViewHeight());
}
SDst
Menu::GetViewPosition() const
{
	return GetItemHeight() * viewer.GetHeadIndex() + top_offset;
}

void
Menu::SetSelected(Menu::ViewerType::IndexType i)
{
	if(viewer.Contains(i) && viewer.SetSelectedIndex(i))
		CallSelected();
}
void
Menu::SetSelected(SPos x, SPos y)
{
	SetSelected(CheckPoint(x, y));
}

SDst
Menu::AdjustTopOffset()
{
	viewer.RestrictSelected();

	SDst d(top_offset);

	top_offset = 0;
	return d;
}

SDst
Menu::AdjustBottomOffset()
{
	if(GetFullViewHeight() <= GetHeight())
		return 0;
	viewer.RestrictSelected();

	const SDst item_height(GetItemHeight()),
		down_offset(GetHeight() % item_height);

	top_offset = item_height - down_offset;
	return down_offset;
}

Menu::ViewerType::IndexType
Menu::CheckPoint(SPos x, SPos y)
{
	return Rect(Point::Zero, GetSize()).Contains(x, y)
		? (y + top_offset) / GetItemHeight() + viewer.GetHeadIndex() : -1;
}

void
Menu::PaintItems(const Graphics& g)
{
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
					GetTextState().Color = HilightTextColor;
					FillRect<PixelType>(g.GetBufferPtr(), g.GetSize(),
						Rect(pt.X + 1, top + 1, ln_w - 2, tmp - 1),
						HilightBackColor);
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

void
Menu::LocateViewPosition(SDst h)
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
Menu::Paint()
{
	YWidgetAssert(this, Controls::Menu, Paint);

	IWindow* pWnd(FetchDirectWindowPtr(*this));

	if(pWnd)
	{
		Control::Paint();
		if(IsFocused())
			WndDrawFocus(pWnd, GetSize());
		PaintItems(pWnd->GetContext());
	}
}

void
Menu::ResetView()
{
	viewer.MoveViewerToBegin();
	if(viewer.IsSelected())
		viewer.SetSelectedIndex(0);
	top_offset = 0;
	UpdateView();
}

void
Menu::UpdateView()
{
	GetViewChanged()(*this, EventArgs());
	Refresh();
}

void
Menu::CallSelected()
{
	GetSelected()(*this, IndexEventArgs(*this, viewer.GetSelectedIndex()));
}

void
Menu::CheckConfirmed(Menu::ViewerType::IndexType i)
{
	if(viewer.IsSelected() && viewer.GetSelectedIndex() == i)
		GetConfirmed()(*this, IndexEventArgs(*this, i));
}

void
Menu::OnKeyDown(KeyEventArgs&& e)
{
	if(viewer.IsSelected())
	{
		using namespace Runtime;

		switch(e.GetKey())
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

				switch(e.GetKey())
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
Menu::OnTouchDown(TouchEventArgs&& e)
{
	SetSelected(e);
	UpdateView();
}

void
Menu::OnTouchMove(TouchEventArgs&& e)
{
	SetSelected(e);
	UpdateView();
}

void
Menu::OnClick(TouchEventArgs&& e)
{
	CheckConfirmed(CheckPoint(e));
}

void
Menu::OnSelected(IndexEventArgs&& /*e*/)
{
	Refresh();
}

void
Menu::OnConfirmed(IndexEventArgs&& e)
{
	OnSelected(std::move(e));
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

