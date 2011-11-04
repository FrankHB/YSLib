/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textlist.cpp
\ingroup UI
\brief 样式相关的文本列表。
\version r1475;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-20 09:28:38 +0800;
\par 修改时间:
	2011-10-31 18:53 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::TextList;
*/


#include "textlist.h"
#include "ywindow.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

namespace
{
	const SDst defMarginH(2); //!< 默认水平边距。
	const SDst defMarginV(1); //!< 默认竖直边距。
	const SDst defMinScrollBarWidth(16); //!< 默认最小滚动条宽。
//	const SDst defMinScrollBarHeight(16); //!< 默认最小滚动条高。
}


TextList::TextList(const Rect& r, const shared_ptr<ListType>& h,
	pair<Color, Color> hilight_pair)
	: Control(r), MTextList(h),
	HilightBackColor(hilight_pair.first), HilightTextColor(hilight_pair.second),
	CyclicTraverse(false),
	viewer(GetList()), top_offset(0)
{
	SetAllOf(Margin, defMarginH, defMarginV);

	static auto on_selected = [this](IndexEventArgs&&){
		Invalidate(*this);
	};
	yunsequenced(
		GetSelected() += on_selected,
		GetConfirmed() += on_selected,
		FetchEvent<KeyDown>(*this) += [this](KeyEventArgs&& e){
			if(viewer.GetTotal() != 0)
			{
				if(viewer.IsSelected())
				{
					switch(e.GetKeyCode())
					{
					case KeySpace::Enter:
						InvokeConfirmed(viewer.GetSelectedIndex());
						break;
					case KeySpace::Esc:
						ClearSelected();
						CallSelected();
						break;
					case KeySpace::Up:
					case KeySpace::Down:
					case KeySpace::PgUp:
					case KeySpace::PgDn:
						{
							const auto nOld(viewer.GetSelectedIndex());

							switch(e.GetKeyCode())
							{
							case KeySpace::Up:
								if(GetSelectedIndex() == 0)
								{
									if(CyclicTraverse)
										SelectLast();
								}
								else
								{
									--viewer;
									if(viewer.GetOffset() == 0)
										AdjustTopOffset();
								}
								break;
							case KeySpace::Down:
								if(GetSelectedIndex() == GetList().size() - 1)
								{
									if(CyclicTraverse)
										SelectFirst();
								}
								else
								{
									++viewer;
									if(viewer.GetOffset() == static_cast<
										ViewerType::DifferenceType>(
										viewer.GetLength() - 1))
										AdjustBottomOffset();
								}
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
				}
				else
					switch(e.GetKeyCode())
					{
					case KeySpace::Up:
					case KeySpace::PgUp:
						SelectLast();
						break;
					case KeySpace::Down:
					case KeySpace::PgDn:
						SelectFirst();
						break;
					default:
						return;
					}
			}
			UpdateView();
		},
		FetchEvent<KeyHeld>(*this) += OnKeyHeld,
		FetchEvent<TouchDown>(*this) += [this](TouchEventArgs&& e){
			SetSelected(e);
			UpdateView();
		},
		FetchEvent<TouchMove>(*this) += [this](TouchEventArgs&& e){
			SetSelected(e);
			UpdateView();
		},
		FetchEvent<Click>(*this) += [this](TouchEventArgs&& e){
			InvokeConfirmed(CheckPoint(e));
		}
	);
}

SDst
TextList::GetFullViewHeight() const
{
	return GetItemHeight() * viewer.GetTotal();
}
Size
TextList::GetFullViewSize() const
{
	return Size(GetWidth(), GetFullViewHeight());
}
SDst
TextList::GetViewPosition() const
{
	return GetItemHeight() * viewer.GetHeadIndex() + top_offset;
}

void
TextList::SetSelected(ViewerType::SizeType i)
{
	if(viewer.Contains(i) && viewer.SetSelectedIndex(i))
		CallSelected();
}
void
TextList::SetSelected(SPos x, SPos y)
{
	SetSelected(CheckPoint(x, y));
}

SDst
TextList::AdjustTopOffset()
{
	viewer.RestrictSelected();

	SDst d(top_offset);

	top_offset = 0;
	return d;
}

SDst
TextList::AdjustBottomOffset()
{
	if(GetFullViewHeight() <= GetHeight())
		return 0;
	viewer.RestrictSelected();

	const SDst item_height(GetItemHeight()),
		down_offset(GetHeight() % item_height);

	top_offset = item_height - down_offset;
	return down_offset;
}

bool
TextList::CheckConfirmed(TextList::ViewerType::SizeType idx) const
{
	return viewer.IsSelected() && viewer.GetSelectedIndex() == idx;
}

TextList::ViewerType::SizeType
TextList::CheckPoint(SPos x, SPos y)
{
	return Rect(Point::Zero, GetSize()).Contains(x, y)
		? (y + top_offset) / GetItemHeight() + viewer.GetHeadIndex()
		: static_cast<ViewerType::SizeType>(-1);
}

Rect
TextList::Refresh(const PaintContext& e)
{
//	Widget::Refresh(e);

	const auto& pt(e.Location);

	PaintItems(e);
	DrawRect(e.Target, pt, GetSize(), IsFocused(*this) ? ColorSpace::Aqua
		: FetchGUIShell().Colors[Styles::ActiveBorder]);
	return Rect(pt, GetSize());
}

void
TextList::LocateViewPosition(SDst h)
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
TextList::PaintItem(const Graphics& g, const Rect&, ListType::size_type i)
{
	DrawText(g, GetTextState(), GetList()[i]);
}

void
TextList::PaintItems(const PaintContext& e)
{
	const auto& g(e.Target);
	const auto& pt(e.Location);
	const auto h(GetHeight());

	if(h != 0)
	{
		RefreshTextState();

		// TODO: refresh for 'rect' properly;
		Widget::Refresh(PaintContext(g, pt, Rect(pt, GetSize())));

		const SDst ln_w(GetWidth());
		const SDst ln_h(GetItemHeight());

		viewer.SetLength((GetHeight() + top_offset + ln_h - 1) / ln_h);
		if(viewer.GetTotal() != 0)
		{
			const auto last(viewer.GetHeadIndex()
				+ viewer.GetValid());
			SPos y(-top_offset);

			for(auto i(viewer.GetHeadIndex()); i < last; ++i)
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

				const Rect unit_bounds(pt.X, top, ln_w, tmp);

				GetTextState().ResetForBounds(unit_bounds, g.GetSize(),
					Margin);
				if(y < 0)
					GetTextState().PenY -= top_offset;
				PaintItem(g, unit_bounds, i);
				y += ln_h;
			}
		}
	}
}

void
TextList::ResetView()
{
	viewer.MoveViewerToBegin();
	if(viewer.IsSelected())
		viewer.SetSelectedIndex(0);
	top_offset = 0;
	UpdateView();
}

void
TextList::SelectFirst()
{
	viewer.SetSelectedIndex(0);
	AdjustTopOffset();
}

void
TextList::SelectLast()
{
	viewer.SetSelectedIndex(GetList().size() - 1);
	AdjustBottomOffset();
}

void
TextList::UpdateView()
{
	GetViewChanged()(UIEventArgs(*this));
	Invalidate(*this);
}

void
TextList::CallSelected()
{
	GetSelected()(IndexEventArgs(*this, viewer.GetSelectedIndex()));
}

void
TextList::InvokeConfirmed(ViewerType::SizeType idx)
{
	if(CheckConfirmed(idx))
		GetConfirmed()(IndexEventArgs(*this, idx));
}


void
ResizeForContent(TextList& tl)
{
	SDst max_width(0);

	std::for_each(tl.GetList().cbegin(), tl.GetList().cend(),
		[&](const TextList::ListType::value_type& str){
			SDst ln_width(FetchStringWidth(tl.Font, str));

			if(ln_width > max_width)
				max_width = ln_width;
	});
	tl.SetSize(Size(max_width + GetHorizontalOf(tl.Margin),
		tl.GetItemHeight() * tl.GetList().size()));
}

YSL_END_NAMESPACE(Components)

YSL_END

