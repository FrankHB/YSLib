/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ListControl.cpp
\ingroup UI
\brief 列表控件。
\version r2080
\author FrankHB <frankhb1989@gmail.com>
\since build 214
\par 创建时间:
	2011-04-20 09:28:38 +0800
\par 修改时间:
	2014-09-11 12:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::ListControl
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_ListControl
#include YFM_YSLib_UI_YWindow
#include YFM_YSLib_UI_Border
#include YFM_YSLib_Service_YBlit
#include YFM_YSLib_Service_TextLayout // for FetchMaxTextWidth;
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;

namespace YSLib
{

using namespace Drawing;

namespace UI
{

AMUnitControlList::AMUnitControlList(unique_ptr<IWidget>&& p_wgt)
	: AMUnitList(),
	p_unit(std::move(p_wgt))
{}

IWidget&
AMUnitControlList::GetUnitRef() const
{
	YAssertNonnull(p_unit);
	return *p_unit;
}

void
AMUnitControlList::SetUnit(unique_ptr<IWidget>&& p)
{
	p_unit = std::move(p);
}

WidgetIterator
AMUnitControlList::MakeIterator(size_t item_idx)
{
	return ystdex::make_prototyped_iterator(GetUnitRef(), item_idx,
		[this](IWidget& wgt, size_t idx){
		if(idxShared != idx && idx < GetTotal())
		{
			Iterated(idx);
			yunseq(FetchGUIState().WidgetIdentity = idx, idxShared = idx),
			wgt.GetView().SetLocation(GetUnitLocation(idx));
		}
	});
}


MTextList::MTextList(const shared_ptr<ListType>& h)
	: AMUnitControlList(make_unique<Control>()),
	hList(h ? h : make_shared<ListType>())
{
	auto& unit(GetUnitRef());

	LabelBrush.Margin = Padding(2, 2, 1, 1),
	unit.GetView().GetSizeRef().Height = GetItemHeight(),
	Iterated += [this](size_t idx){
		YAssert(idx < GetTotal(), "Index is out of range.");
		LabelBrush.Text = GetList()[idx];
	},
	FetchEvent<Paint>(unit) = std::ref(LabelBrush);
}

SDst
MTextList::GetItemHeight() const
{
	const auto
		item_h(LabelBrush.Font.GetHeight() + GetVerticalOf(LabelBrush.Margin));

	YAssert(item_h != 0, "Invalid item height found.");
	return item_h;
}
SDst
MTextList::GetTotal() const
{
	return GetList().size();
}

void
MTextList::SetList(const shared_ptr<ListType>& h)
{
	if(YB_LIKELY(h))
		hList = h;
}


TextList::TextList(const Rect& r, const shared_ptr<ListType>& h,
	const pair<Color, Color>& hilight_pair)
	: Control(r, MakeBlankBrush()), MTextList(h), MHilightText(hilight_pair)
{
	const auto invalidator([this]{
		Invalidate(*this);
	});
	auto& unit(GetUnitRef());

	SetContainerPtrOf(unit, this),
	// TODO: Implement auto fit.
	SetSizeOf(unit, {GetWidth(), GetItemHeight()}),
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
				const auto old_sel(vwList.GetSelectedIndex());
				const auto old_off(vwList.GetOffset());
				const auto old_hid(vwList.GetHeadIndex());
				const auto old_top(uTopOffset);

				{
					const bool up(k[Up] || k[PgUp]);

					if(vwList.IsSelected())
					{
						vwList.IncreaseSelected((up ? -1 : 1) * (k[Up]
							|| k[Down] ? 1 : GetHeight() / GetItemHeight()),
							GetList().size());
						if(old_sel == vwList.GetSelectedIndex()
							&& CyclicTraverse)
							goto bound_select;
						if(vwList.GetOffset()
							== (up ? 0 : ptrdiff_t(vwList.Length - 1)))
							AdjustOffsetForHeight(GetHeight(), up);
					}
					else
bound_select:
						up ? SelectLast() : SelectFirst();
				}

				const auto new_off(vwList.GetOffset());

				if(vwList.GetSelectedIndex() != old_sel)
					CallSelected();
				if(old_top != uTopOffset || vwList.GetHeadIndex() != old_hid)
					UpdateView(*this);
				else if(old_off != new_off)
					InvalidateSelected2(old_off, new_off);
			}
			else if(vwList.IsSelected())
			{
				// NOTE: Do not confuse with %UI::Enter.
				if(k[KeyCodes::Enter])
					InvokeConfirmed(vwList.GetSelectedIndex());
				else if(k[Esc])
				{
					InvalidateSelected(vwList.GetOffset());
					ClearSelected();
				// TODO: Create new event for canceling selection.
					CallSelected();
				}
			}
		}
		e.Handled = true;
	},
	FetchEvent<KeyHeld>(*this) += OnKeyHeld,
	FetchEvent<Paint>(*this).Add(BorderBrush(), BoundaryPriority),
	FetchEvent<GotFocus>(*this) += invalidator,
	FetchEvent<LostFocus>(*this) += invalidator,
	FetchEvent<Click>(unit) += [this]{
		InvokeConfirmed(idxShared);
	},
	FetchEvent<TouchDown>(unit) += [this]{
		SetSelected(idxShared);
	},
	FetchEvent<TouchHeld>(unit) += [this](CursorEventArgs&& e){
		if(&e.GetSender() == &GetUnitRef())
			SetSelected(idxShared);
	},
	FetchEvent<Paint>(unit).Add([this](PaintEventArgs&& e){
		auto& unit(GetUnitRef());

		if(vwList.CheckSelected(idxShared))
			LabelBrush.ForeColor = HilightTextColor,
			FillRect(e.Target, e.ClipArea, Rect(e.Location.X + 1, e.Location.Y,
				GetSizeOf(unit).Width - 2, GetSizeOf(unit).Height),
				HilightBackColor);
		else
			LabelBrush.ForeColor = ForeColor;
	}, BackgroundPriority)
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
	if(vwList.Contains(i))
	{
		const auto old_off(vwList.GetOffset());

		if(vwList.SetSelectedIndex(i, GetList().size()))
		{
			CallSelected();
			InvalidateSelected2(old_off, vwList.GetOffset());
		}
	}
}

void
TextList::CallSelected()
{
	Selected(IndexEventArgs(*this, vwList.GetSelectedIndex()));
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
	if(vwList.CheckSelected(idx))
		Confirmed(IndexEventArgs(*this, idx));
}

void
TextList::LocateViewPosition(SDst h)
{
	const SDst fvh(GetFullViewHeight()), height(GetHeight());

	if(fvh <= height)
		return;
	RestrictUnsignedStrict(h, fvh - height - 1);
	if(GetViewPosition() != h)
	{
		const SDst item_height(GetItemHeight());

		//先保证避免部分显示的项目使视图超长，再设置视图位置。
		AdjustViewLength();
		vwList.SetHeadIndex(h / item_height, GetList().size());
		uTopOffset = h % item_height;
		//更新视图。
		UpdateView(*this, true);
	}
}

void
TextList::Refresh(PaintEventArgs&& e)
{
	if(GetHeight() != 0)
	{
		const Rect& bounds(e.ClipArea);

		if(!bounds.IsUnstrictlyEmpty() && !GetList().empty())
		{
			// NOTE: View length could be already changed by contents.
			AdjustViewLength();
			SetSizeOf(GetUnitRef(), {GetWidth(), GetItemHeight()});
			for(WidgetIterator first(begin()), last(MakeIterator(
				GetLastLabelIndexClipped(e.Location.Y - bounds.Y,
				bounds.Height))); first != last; ++first)
				PaintVisibleChildAndCommit(*first, e);
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
	vwList.SetSelectedIndex(0, GetList().size());
	AdjustOffsetForHeight(GetHeight(), true);
}

void
TextList::SelectLast()
{
	const auto s(GetList().size());

	vwList.SetSelectedIndex(s - 1, s);
	AdjustOffsetForHeight(GetHeight(), {});
}

WidgetIterator
TextList::begin()
{
	return MakeIterator(GetHeadIndex());
}

WidgetIterator
TextList::end()
{
	return MakeIterator(GetLastLabelIndexClipped(0, GetHeight()));
}


void
ResizeForContent(TextList& tl)
{
	const auto& lst(tl.GetList());

	SetSizeOf(tl, Size(FetchMaxTextWidth(tl.LabelBrush.Font, lst.cbegin(),
		lst.cend()) + GetHorizontalOf(tl.LabelBrush.Margin),
		tl.GetFullViewHeight()));
	tl.AdjustViewLength();
}

void
UpdateView(TextList& tl, bool is_active)
{
	tl.ViewChanged(TextList::ViewArgs(tl, is_active));
	tl.AdjustViewLength();
	SetInvalidationOf(tl.GetUnitRef()),
	Invalidate(tl);
}

} // namespace UI;

} // namespace YSLib;

