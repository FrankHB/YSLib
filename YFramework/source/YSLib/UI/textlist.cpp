/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textlist.cpp
\ingroup UI
\brief 样式相关的文本列表。
\version r1207
\author FrankHB <frankhb1989@gmail.com>
\since build 214
\par 创建时间:
	2011-04-20 09:28:38 +0800
\par 修改时间:
	2014-03-15 22:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TextList
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_TextList
#include YFM_YSLib_UI_YWindow
#include YFM_YSLib_UI_Border
#include YFM_YSLib_Service_YBlit
#include YFM_YSLib_Service_TextLayout

namespace YSLib
{

using namespace Drawing;

namespace UI
{

namespace
{
	const SDst defMarginH(2); //!< 默认水平边距。
	const SDst defMarginV(1); //!< 默认竖直边距。
	const SDst defMinScrollBarWidth(16); //!< 默认最小滚动条宽。
//	const SDst defMinScrollBarHeight(16); //!< 默认最小滚动条高。
}


TextList::TextList(const Rect& r, const shared_ptr<ListType>& h,
	const pair<Color, Color>& hilight_pair)
	: Control(r, MakeBlankBrush()), MTextList(h), MHilightText(hilight_pair),
	CyclicTraverse(false), viewer(GetListRef()), top_offset(0)
{
	Margin = Padding(defMarginH, defMarginH, defMarginV, defMarginV);
	yunseq(
	FetchEvent<KeyDown>(*this) += [this](KeyEventArgs&& e){
		if(viewer.GetTotal() != 0)
		{
			using namespace KeyCodes;
			const auto& k(e.GetKeys());

			if(k.count() != 1)
				return;
			if(k[Up] || k[Down] || k[PgUp] || k[PgDn])
			{
				const auto old_sel(viewer.GetSelectedIndex());
				const auto old_off(viewer.GetOffset());
				const auto old_hid(viewer.GetHeadIndex());
				const auto old_top(top_offset);

				{
					const bool up(k[Up] || k[PgUp]);

					if(viewer.IsSelected())
					{
						viewer.IncreaseSelected((up ? -1 : 1) * (k[Up]
							|| k[Down] ? 1 : GetHeight() / GetItemHeight()));
						if(old_sel == viewer.GetSelectedIndex()
							&& CyclicTraverse)
							goto bound_select;
						if(viewer.GetOffset() == (up ? 0 : ViewerType
							::DifferenceType(viewer.GetLength() - 1)))
							AdjustOffset(up);
					}
					else
bound_select:
						up ? SelectLast() : SelectFirst();
				}

				const auto new_off(viewer.GetOffset());

				if(viewer.GetSelectedIndex() != old_sel)
					CallSelected();
				if(old_top != top_offset || viewer.GetHeadIndex() != old_hid)
					UpdateView(*this);
				else if(old_off != new_off)
					InvalidateSelected2(old_off, new_off);
			}
			else if(viewer.IsSelected())
			{
				// NOTE: Do not confuse with %UI::Enter.
				if(k[KeyCodes::Enter])
					InvokeConfirmed(viewer.GetSelectedIndex());
				else if(k[Esc])
				{
					InvalidateSelected(viewer.GetOffset());
					ClearSelected();
				// TODO: Create new event for canceling selection.
					CallSelected();
				}
			}
		}
	},
	FetchEvent<KeyHeld>(*this) += OnKeyHeld,
	FetchEvent<TouchDown>(*this) += [this](CursorEventArgs&& e){
		SetSelected(e);
		UpdateView(*this);
	},
	FetchEvent<TouchHeld>(*this) += [this](CursorEventArgs&& e){
		if(&e.GetSender() == this)
		{
			SetSelected(e);
			UpdateView(*this);
		}
	},
	FetchEvent<Click>(*this) += [this](CursorEventArgs&& e){
		InvokeConfirmed(CheckPoint(e));
	},
	FetchEvent<Paint>(*this).Add(BorderBrush(), BoundaryPriority)
	);
	AdjustViewLength();
}

SDst
TextList::GetFullViewHeight() const
{
	return GetItemHeight() * viewer.GetTotal();
}
SDst
TextList::GetViewPosition() const
{
	return GetItemHeight() * viewer.GetHeadIndex() + top_offset;
}

void
TextList::SetList(const shared_ptr<ListType>& h)
{
	if(h)
	{
		MTextList::SetList(h);
		viewer.SetContainer(*h);
		AdjustViewLength();
	}
}

void
TextList::SetSelected(ListType::size_type i)
{
	if(viewer.Contains(i))
	{
		const auto old_off(viewer.GetOffset());

		if(viewer.SetSelectedIndex(i))
		{
			CallSelected();
			InvalidateSelected2(old_off, viewer.GetOffset());
		}
	}
}
void
TextList::SetSelected(SPos x, SPos y)
{
	SetSelected(CheckPoint(x, y));
}

SDst
TextList::AdjustOffset(bool is_top)
{
	if(GetFullViewHeight() > GetHeight())
	{
		viewer.RestrictSelected();

		if(is_top)
		{
			const auto d(top_offset);

			top_offset = 0;
			AdjustViewLength();
			return d;
		}
		else
		{
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
	}
	return 0;
}

void
TextList::AdjustViewForContent()
{
	const bool b(viewer.AdjustForContent());

	if(viewer.IsSelected() && b)
	{
		AdjustOffset(viewer.GetSelectedIndex() == viewer.GetHeadIndex());
		return;
	}
	if(GetFullViewHeight() < GetViewPosition() + GetHeight())
		top_offset = 0;
	AdjustViewLength();
}

void
TextList::AdjustViewLength()
{
	const auto h(GetHeight());

	if(h != 0)
	{
		const auto ln_h(GetItemHeight());

		viewer.SetLength(h / ln_h + (top_offset != 0 || h % ln_h != 0));
	}
}

bool
TextList::CheckConfirmed(ListType::size_type idx) const
{
	return viewer.IsSelected() && viewer.GetSelectedIndex() == idx;
}

TextList::ListType::size_type
TextList::CheckPoint(SPos x, SPos y)
{
	return Rect(GetSizeOf(*this)).Contains(x, y) ? (y + top_offset)
		/ GetItemHeight() + viewer.GetHeadIndex() : ListType::size_type(-1);
}

void
TextList::InvalidateSelected(ListType::difference_type offset,
	ListType::size_type n)
{
	if(offset >= 0 && n != 0)
	{
		const auto ln_h(GetItemHeight());
		Rect r(0, ln_h * offset - top_offset, GetWidth(), ln_h * n);

		if(r.Y < GetHeight())
		{
			r.Y = max<int>(0, r.Y);
			RestrictUnsignedStrict(r.Height, GetHeight() - r.Y);
			Invalidate(*this, r);
		}
	}
}

void
TextList::InvalidateSelected2(ListType::difference_type x,
	ListType::difference_type y)
{
	if(y < x)
		std::swap(x, y);
	InvalidateSelected(x < 0 ? 0 : x, y - x + 1);
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
		//更新视图。
		UpdateView(*this, true);
	}
}

void
TextList::DrawItem(const Graphics& g, const Rect& mask, const Rect& unit,
	ListType::size_type i)
{
	Drawing::DrawClippedText(g, mask & (unit + Margin), tsList, GetList()[i],
		false);
}

void
TextList::DrawItemBackground(const PaintContext& pc, const Rect& r)
{
	FillRectRaw<PixelType>(pc.Target.GetBufferPtr(), pc.Target.GetSize(),
		pc.ClipArea & Rect(r.X + 1, r.Y, r.Width - 2, r.Height),
		HilightBackColor);
}

void
TextList::DrawItems(const PaintContext& pc)
{
	const auto h(GetHeight());

	if(h != 0)
	{
		RefreshTextState();

		const Rect& r(pc.ClipArea);

		if(viewer.GetTotal() != 0 && bool(r))
		{
			const auto& g(pc.Target);
			const auto& pt(pc.Location);
			const auto ln_w(GetWidth());
			const auto ln_h(GetItemHeight());

			//视图长度可能因为内容变化等原因改变，必须重新计算。
			AdjustViewLength();

			const SPos lbound(r.Y - pt.Y);
			const auto last(viewer.GetHeadIndex()
				+ min<ViewerType::SizeType>((lbound + r.Height + top_offset
				- 1) / ln_h + 1, viewer.GetValid()));
			SPos y(ln_h * ((min<SPos>(0, lbound) + top_offset - 1) / ln_h)
				- top_offset);

			for(auto i(viewer.GetHeadIndex()); i < last; yunseq(y += ln_h, ++i))
			{
				SPos top(y), tmp(y + ln_h);

				RestrictInInterval<SPos>(top, 0, h);
				RestrictInInterval<SPos>(tmp, 1, h + 1);
				tmp -= top;

				const Rect unit(pt.X, top + pt.Y, ln_w, tmp);

				if(viewer.IsSelected() && i == viewer.GetSelectedIndex())
				{
					tsList.Color = HilightTextColor;
					DrawItemBackground(pc, unit);
				}
				else
					tsList.Color = ForeColor;
				AdjustEndOfLine(tsList, unit + Margin, g.GetWidth()),
				tsList.ResetPen(unit.GetPoint(), Margin);
				if(y < 0)
					tsList.Pen.Y -= top_offset;
				DrawItem(g, pc.ClipArea, unit, i);
			}
		}
	}
}

void
TextList::Refresh(PaintEventArgs&& e)
{
	DrawItems(e);
}

void
TextList::ResetView()
{
	bool b(viewer.IsSelected());

	viewer.Reset();
	if(b)
		viewer.SetSelectedIndex(0);
	top_offset = 0;
	UpdateView(*this);
}

void
TextList::SelectFirst()
{
	viewer.SetSelectedIndex(0);
	AdjustOffset(true);
}

void
TextList::SelectLast()
{
	viewer.SetSelectedIndex(GetList().size() - 1);
	AdjustOffset(false);
}

void
TextList::CallSelected()
{
	Selected(IndexEventArgs(*this, viewer.GetSelectedIndex()));
}

void
TextList::InvokeConfirmed(ListType::size_type idx)
{
	if(CheckConfirmed(idx))
		Confirmed(IndexEventArgs(*this, idx));
}


void
ResizeForContent(TextList& tl)
{
	SetSizeOf(tl, Size(tl.GetMaxTextWidth() + GetHorizontalOf(tl.Margin),
		tl.GetFullViewHeight()));
	tl.AdjustViewLength();
}

void
UpdateView(TextList& tl, bool is_active)
{
	tl.ViewChanged(TextList::ViewArgs(tl, is_active));
	tl.AdjustViewLength();
	Invalidate(tl);
}

} // namespace UI;

} // namespace YSLib;

