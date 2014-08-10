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
\version r1596
\author FrankHB <frankhb1989@gmail.com>
\since build 214
\par 创建时间:
	2011-04-20 09:28:38 +0800
\par 修改时间:
	2014-08-08 19:31 +0800
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

} // unnamed namespace;


MTextList::MTextList(const shared_ptr<ListType>& h, const Drawing::Font& fnt,
	const pair<Color, Color>& hilight_pair)
	: MHilightText(hilight_pair),
	Font(fnt), hList(h ? h : make_shared<ListType>())
{}

WidgetRange
MTextList::GetChildrenByIndices(size_t idx1, size_t idx2)
{
	return {MakeIterator(idx1), MakeIterator(idx2)};
}
SDst
MTextList::GetFullViewHeight() const
{
	return GetItemHeight() * GetList().size();
}
size_t
MTextList::GetLastLabelIndexClipped(SPos v_off, SDst height) const
{
	// XXX: Conversion to 'SPos' might be implementation-defined.
	return vwText.GetHeadIndex() + min<size_t>((SPos(height + uTopOffset)
		- v_off - 1) / SPos(GetItemHeight()) + 1,
		vwText.GetValid(GetList().size()));
}
MTextList::ItemType*
MTextList::GetItemPtr(const IndexType& idx)
{
	auto& lst(GetListRef());

	return IsInInterval<IndexType>(idx, lst.size()) ? &lst[idx] : nullptr;
}
const MTextList::ItemType*
MTextList::GetItemPtr(const IndexType& idx) const
{
	const auto& lst(GetList());

	return IsInInterval<IndexType>(idx, lst.size()) ? &lst[idx] : nullptr;
}
SDst
MTextList::GetMaxTextWidth() const
{
	return FetchMaxTextWidth(Font, GetList().cbegin(), GetList().cend());
}
Point
MTextList::GetUnitLocation(size_t idx) const
{
	// XXX: Conversion to 'SPos' might be implementation-defined.
	return Point(0, -SPos(uTopOffset) + SPos(GetItemHeight()) * (SPos(idx)
		- SPos(vwText.GetHeadIndex())));
}
SDst
MTextList::GetViewPosition() const
{
	return GetItemHeight() * vwText.GetHeadIndex() + uTopOffset;
}

void
MTextList::SetList(const shared_ptr<ListType>& h)
{
	if(YB_LIKELY(h))
		hList = h;
}

SDst
MTextList::AdjustOffsetForHeight(SDst h, bool is_top)
{
	if(GetFullViewHeight() > h)
	{
		vwText.RestrictSelected();

		if(is_top)
		{
			const auto d(uTopOffset);

			uTopOffset = 0;
			AdjustViewLengthForHeight(h);
			return d;
		}
		else
		{
			const SDst ln_h(GetItemHeight());
			const auto d((h + uTopOffset) % ln_h);

			if(d != 0)
			{
				const auto tmp(uTopOffset + ln_h - d);

				uTopOffset = tmp % ln_h;
				AdjustViewLengthForHeight(h);
				vwText.IncreaseHead(tmp / ln_h, GetList().size());
			}
			return d;
		}
	}
	return 0;
}

void
MTextList::AdjustViewForContent(SDst h)
{
	if(vwText.AdjustForContent(GetList().size()) && vwText.IsSelected())
	{
		AdjustOffsetForHeight(h,
			vwText.GetSelectedIndex() == vwText.GetHeadIndex());
		return;
	}
	if(GetFullViewHeight() < GetViewPosition() + h)
		uTopOffset = 0;
	AdjustViewLengthForHeight(h);
}

void
MTextList::AdjustViewLengthForHeight(SDst h)
{
	if(YB_LIKELY(h != 0))
	{
		const auto ln_h(GetItemHeight());

		vwText.Length = h / ln_h + (uTopOffset != 0 || h % ln_h != 0);
	}
}

MTextList::IndexType
MTextList::Find(const ItemType& text) const
{
	const auto& lst(GetList());
	const auto i(std::find(lst.begin(), lst.end(), text));

	return i != lst.end() ? i - lst.begin() : IndexType(-1);
}

WidgetIterator
MTextList::MakeIterator(size_t item_idx)
{
	return ystdex::make_prototyped_iterator(lblShared, item_idx,
		[this](IWidget& wgt, size_t idx){
		if(idxShared != idx && idx < GetList().size())
			SetLocationOf(wgt, GetUnitLocation(idx)),
			yunseq(
			lblShared.Margin = Margin,
			lblShared.Text = GetList()[idx], idxShared = idx
			);
	});
}

void
MTextList::ResetView()
{
	bool b(vwText.IsSelected());

	vwText.Reset();
	if(b)
		vwText.SetSelectedIndex(0, GetList().size());
	uTopOffset = 0;
}


TextList::TextList(const Rect& r, const shared_ptr<ListType>& h,
	const pair<Color, Color>& hilight_pair)
	: Control(r, MakeBlankBrush()), MTextList(h, {}, hilight_pair)
{
	const auto invalidator([this]{
		Invalidate(*this);
	});

	Margin = Padding(defMarginH, defMarginH, defMarginV, defMarginV);
	SetContainerPtrOf(lblShared, this);
	yunseq(
	FetchEvent<KeyDown>(*this) += [this](KeyEventArgs&& e){
		if(!GetList().empty())
		{
			using namespace KeyCodes;
			const auto& k(e.GetKeys());

			if(k.count() != 1)
				return;
			if(k[Up] || k[Down] || k[PgUp] || k[PgDn])
			{
				const auto old_sel(vwText.GetSelectedIndex());
				const auto old_off(vwText.GetOffset());
				const auto old_hid(vwText.GetHeadIndex());
				const auto old_top(uTopOffset);

				{
					const bool up(k[Up] || k[PgUp]);

					if(vwText.IsSelected())
					{
						vwText.IncreaseSelected((up ? -1 : 1) * (k[Up]
							|| k[Down] ? 1 : GetHeight() / GetItemHeight()),
							GetList().size());
						if(old_sel == vwText.GetSelectedIndex()
							&& CyclicTraverse)
							goto bound_select;
						if(vwText.GetOffset()
							== (up ? 0 : ptrdiff_t(vwText.Length - 1)))
							AdjustOffsetForHeight(GetHeight(), up);
					}
					else
bound_select:
						up ? SelectLast() : SelectFirst();
				}

				const auto new_off(vwText.GetOffset());

				if(vwText.GetSelectedIndex() != old_sel)
					CallSelected();
				if(old_top != uTopOffset || vwText.GetHeadIndex() != old_hid)
					UpdateView(*this);
				else if(old_off != new_off)
					InvalidateSelected2(old_off, new_off);
			}
			else if(vwText.IsSelected())
			{
				// NOTE: Do not confuse with %UI::Enter.
				if(k[KeyCodes::Enter])
					InvokeConfirmed(vwText.GetSelectedIndex());
				else if(k[Esc])
				{
					InvalidateSelected(vwText.GetOffset());
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
	FetchEvent<Paint>(*this).Add(BorderBrush(), BoundaryPriority),
	FetchEvent<GotFocus>(*this) += invalidator,
	FetchEvent<LostFocus>(*this) += invalidator,
	lblShared.Background = [this](PaintEventArgs&& e){
		if(vwText.CheckSelected(idxShared))
			lblShared.ForeColor = HilightTextColor,
			FillRect(e.Target, e.ClipArea, Rect(e.Location.X + 1, e.Location.Y,
				lblShared.GetWidth() - 2, lblShared.GetHeight()),
				HilightBackColor);
		else
			lblShared.ForeColor = ForeColor;
	}
	);
	AdjustViewLength();
}

void
TextList::SetList(const shared_ptr<ListType>& h)
{
	if(h)
	{
		MTextList::SetList(h);
		AdjustViewLength();
	}
}

size_t
TextList::GetLastLabelIndex() const
{
	return GetLastLabelIndexClipped(0, GetHeight());
}

void
TextList::SetSelected(ListType::size_type i)
{
	if(vwText.Contains(i))
	{
		const auto old_off(vwText.GetOffset());

		if(vwText.SetSelectedIndex(i, GetList().size()))
		{
			CallSelected();
			InvalidateSelected2(old_off, vwText.GetOffset());
		}
	}
}
void
TextList::SetSelected(SPos x, SPos y)
{
	SetSelected(CheckPoint(x, y));
}

void
TextList::CallSelected()
{
	Selected(IndexEventArgs(*this, vwText.GetSelectedIndex()));
}

TextList::ListType::size_type
TextList::CheckPoint(SPos x, SPos y)
{
	return Rect(GetSizeOf(*this)).Contains(x, y) ? (y + uTopOffset)
		/ GetItemHeight() + vwText.GetHeadIndex() : ListType::size_type(-1);
}

void
TextList::InvalidateSelected(ListType::difference_type offset,
	ListType::size_type n)
{
	if(offset >= 0 && n != 0)
	{
		const auto ln_h(GetItemHeight());
		Rect r(0, ln_h * offset - uTopOffset, GetWidth(), ln_h * n);

		if(r.Y < 0 || SDst(r.Y) < GetHeight())
		{
			r.Y = max<SPos>(0, r.Y);
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
TextList::InvokeConfirmed(ListType::size_type idx)
{
	if(CheckConfirmed(idx))
		Confirmed(IndexEventArgs(*this, idx));
}

void
TextList::LocateViewPosition(SDst h)
{
	SDst fvh(GetFullViewHeight());
	const SDst height(GetHeight());

	if(fvh <= height)
		return;
	RestrictUnsignedStrict(h, fvh - height - 1);
	if(GetViewPosition() != h)
	{
		const SDst item_height(GetItemHeight());

		//先保证避免部分显示的项目使视图超长，再设置视图位置。
		AdjustViewLength();
		vwText.SetHeadIndex(h / item_height, GetList().size());
		uTopOffset = h % item_height;
		//更新视图。
		UpdateView(*this, true);
	}
}

void
TextList::Refresh(PaintEventArgs&& e)
{
	const auto h(GetHeight());

	if(h != 0)
	{
		const Rect& r(e.ClipArea);

		if(!r.IsUnstrictlyEmpty())
		{
			if(!GetList().empty() && bool(r))
			{
				// NOTE: View length could be already changed by contents.
				AdjustViewLength();
				SetSizeOf(lblShared, {GetWidth(), GetItemHeight()});
				for(auto pr(GetChildrenByIndices(GetHeadIndex(),
					GetLastLabelIndexClipped(e.Location.Y - r.Y, r.Height)));
					pr.first != pr.second; ++pr.first)
					PaintVisibleChildAndCommit(*pr.first, e);
			}
		}
	}
}

void
TextList::ResetView()
{
	MTextList::ResetView();
	UpdateView(*this);
}

void
TextList::SelectFirst()
{
	vwText.SetSelectedIndex(0, GetList().size());
	AdjustOffsetForHeight(GetHeight(), true);
}

void
TextList::SelectLast()
{
	const auto s(GetList().size());

	vwText.SetSelectedIndex(s - 1, s);
	AdjustOffsetForHeight(GetHeight(), {});
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

