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
\version r1583;
\author FrankHB<frankhb1989@gmail.com>
\since build 214 。
\par 创建时间:
	2011-04-20 09:28:38 +0800;
\par 修改时间:
	2011-11-30 20:59 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::TextList;
*/


#include "YSLib/UI/textlist.h"
#include "YSLib/UI/ywindow.h"

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
								if(viewer.GetSelectedIndex() == 0)
								{
									if(CyclicTraverse)
									{
										SelectLast();
										break;
									}
								}
								else
									--viewer;
								if(viewer.GetOffset() == 0)
									AdjustTopOffset();
								break;
							case KeySpace::Down:
								if(viewer.GetSelectedIndex()
									== ViewerType::DifferenceType(
									GetList().size() - 1))
								{
									if(CyclicTraverse)
									{
										SelectFirst();
										break;
									}
								}
								else
									++viewer;
								if(viewer.GetOffset() == 
									ViewerType::DifferenceType(
									viewer.GetLength() - 1))
									AdjustBottomOffset();
								break;
							case KeySpace::PgUp:
								viewer.DecreaseSelected(
									GetHeight() / GetItemHeight());
								AdjustTopOffset();
								break;
							case KeySpace::PgDn:
								viewer.IncreaseSelected(
									GetHeight() / GetItemHeight());
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
			if(&e.GetSender() == this)
			{
				SetSelected(e);
				UpdateView();
			}
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
TextList::SetSelected(ListType::size_type i)
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
TextList::AdjustBottomOffset()
{
	if(GetFullViewHeight() > GetHeight())
	{
		viewer.RestrictSelected();

		const SDst item_height(GetItemHeight());
		const auto d((GetHeight() + top_offset) % item_height);

		if(d != 0)
		{
			const auto tmp(top_offset + item_height - d);

			top_offset = tmp % item_height;
			AdjustViewLength();
			viewer.IncreaseHead(tmp / item_height);
		}
		return d;
	}
	return 0;
}

SDst
TextList::AdjustTopOffset()
{
	if(GetFullViewHeight() > GetHeight())
	{
		viewer.RestrictSelected();

		const auto d(top_offset);

		top_offset = 0;
		AdjustViewLength();
		return d;
	}
	return 0;
}

void
TextList::AdjustViewLength()
{
	const auto h(GetHeight());
	const auto ln_h(GetItemHeight());

	viewer.SetLength(h / ln_h + (top_offset != 0 || h % ln_h != 0));
}

bool
TextList::CheckConfirmed(TextList::ListType::size_type idx) const
{
	return viewer.IsSelected() && viewer.GetSelectedIndex() == idx;
}

TextList::ListType::size_type
TextList::CheckPoint(SPos x, SPos y)
{
	return Rect(Point::Zero, GetSizeOf(*this)).Contains(x, y)
		? (y + top_offset) / GetItemHeight() + viewer.GetHeadIndex()
		: ListType::size_type(-1);
}

Rect
TextList::Refresh(const PaintContext& pc)
{
//	Widget::Refresh(pc);

	const auto& pt(pc.Location);

	PaintItems(pc);
	DrawRect(pc.Target, pt, GetSizeOf(*this), IsFocused(*this) ? ColorSpace::Aqua
		: FetchGUIShell().Colors[Styles::ActiveBorder]);
	return Rect(pt, GetSizeOf(*this));
}

void
TextList::LocateViewPosition(SDst h)
{
	RestrictInInterval(h, 0, GetFullViewHeight() - GetHeight());

	if(GetViewPosition() != h)
	{
		const SDst item_height(GetItemHeight());

		//先保证避免部分显示的项目使视图超长，再设置视图位置。
		AdjustViewLength();
		viewer.SetHeadIndex(h / item_height);
		top_offset = h % item_height;
		//更新视图长度。
		AdjustViewLength();
	}
}

void
TextList::PaintItem(const Graphics& g, const Rect& mask, const Rect&,
	ListType::size_type i)
{
	DrawClippedText(g, mask, GetTextState(), GetList()[i]);
}

void
TextList::PaintItems(const PaintContext& pc)
{
	const auto& g(pc.Target);
	const auto& pt(pc.Location);
	const auto h(GetHeight());

	if(h != 0)
	{
		RefreshTextState();

		Widget::Refresh(pc);

		if(viewer.GetTotal() != 0)
		{
			const auto ln_w(GetWidth());
			const auto ln_h(GetItemHeight());

			//视图长度可能因为内容变化等原因改变，必须重新计算。
			AdjustViewLength();

			const auto last(viewer.GetHeadIndex() + viewer.GetValid());
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
						Intersect(Rect(pt.X + 1, top + 1, ln_w - 2, tmp - 1),
						pc.ClipArea), HilightBackColor);
				}
				else
					GetTextState().Color = ForeColor;

				const Rect unit_bounds(pt.X, top, ln_w, tmp);

				GetTextState().ResetForBounds(unit_bounds, g.GetSize(),
					Margin);
				if(y < 0)
					GetTextState().PenY -= top_offset;
				PaintItem(g, pc.ClipArea, unit_bounds, i);
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
	AdjustViewLength();
	Invalidate(*this);
}

void
TextList::CallSelected()
{
	GetSelected()(IndexEventArgs(*this, viewer.GetSelectedIndex()));
}

void
TextList::InvokeConfirmed(ListType::size_type idx)
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
	SetSizeOf(tl, Size(max_width + GetHorizontalOf(tl.Margin),
		tl.GetItemHeight() * tl.GetList().size()));
}

YSL_END_NAMESPACE(Components)

YSL_END

