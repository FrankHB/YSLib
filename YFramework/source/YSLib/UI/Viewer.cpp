/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Viewer.cpp
\ingroup UI
\brief 样式无关的视图。
\version r297
\author FrankHB <frankhb1989@gmail.com>
\since build 525
\par 创建时间:
	2014-08-08 14:39:59 +0800
\par 修改时间:
	2015-03-29 12:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Viewer
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Viewer
#include YFM_YSLib_UI_YWidget // for UI::GetSizeOf;

namespace YSLib
{

namespace UI
{

bool
SequenceViewer::Contains(size_t i) const
{
	return Length != 0 && IsInInterval<size_t>(i - GetHeadIndex(), Length);
}

size_t
SequenceViewer::GetValid(size_t total) const ynothrow
{
	YAssert(total >= head, "Total number of items is less than head index");

	return min<size_t>(total - head, Length);
}

bool
SequenceViewer::SetHeadIndex(size_t t, size_t total)
{
	if(t < total && t != head)
	{
		if(t == 0)
			MoveViewerToBegin();
		else if(total < Length + t)
			MoveViewerToEnd(total);
		else
			head = t;
		return true;
	}
	return {};
}
bool
SequenceViewer::SetSelectedIndex(size_t t, size_t total)
{
	if(t < total && !(t == selected && is_selected))
	{
		selected = t;
		RestrictView();
		is_selected = true;
		return true;
	}
	return {};
}

bool
SequenceViewer::AdjustForContent(size_t total)
{
	if(total != 0)
	{
		if(!(selected < total))
			selected = total - 1;
		return RestrictView();
	}
	else
		Reset();
	return true;
}

SequenceViewer&
SequenceViewer::IncreaseHead(ptrdiff_t d, size_t total)
{
	// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
	auto t(ptrdiff_t(head) + d);

	RestrictInInterval(t, ptrdiff_t(0), ptrdiff_t(total));
	SetHeadIndex(size_t(t), total);
	return *this;
}

SequenceViewer&
SequenceViewer::IncreaseSelected(ptrdiff_t d, size_t total)
{
	// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
	auto t(ptrdiff_t(selected) + d);

	RestrictInInterval(t, ptrdiff_t(0), ptrdiff_t(total));
	SetSelectedIndex(size_t(t), total);
	return *this;
}

bool
SequenceViewer::MoveViewerToBegin()
{
	if(head)
	{
		head = 0;
		return true;
	}
	return {};
}

bool
SequenceViewer::MoveViewerToEnd(size_t total)
{
	if(total < Length)
		return {};
	head = total - Length;
	return true;
}

bool
SequenceViewer::RestrictSelected()
{
	if(selected < head)
		selected = head;
	else if(selected < head + Length)
		return {};
	else
		selected = head + Length - 1;
	return true;
}

bool
SequenceViewer::RestrictView()
{
	if(selected < head)
		head = selected;
	else if(selected < head + Length)
		return {};
	else
		head = selected + 1 - Length;
	return true;
}


SDst
AMUnitList::GetFullViewHeight() const
{
	return SDst(GetItemHeight() * GetTotal());
}
SDst
AMUnitList::GetItemHeight() const
{
	const auto item_h(GetItemHeightCore());

	if(item_h != 0)
		return item_h;
	throw LoggedEvent("Zero item height found.");
}
SDst
AMUnitList::GetItemHeightCore() const
{
	return GetSizeOf(GetUnitRef()).Height;
}
size_t
AMUnitList::GetLastLabelIndexClipped(SPos v_off, SDst height) const
{
	// XXX: Conversion to 'SPos' might be implementation-defined.
	return vwList.GetHeadIndex() + min(size_t((SPos(height + uTopOffset)
		- v_off - 1) / SPos(GetItemHeight()) + 1), vwList.GetValid(GetTotal()));
}
Point
AMUnitList::GetUnitLocation(size_t idx) const
{
	// XXX: Conversion to 'SPos' might be implementation-defined.
	return Point(0, -SPos(uTopOffset) + SPos(GetItemHeight()) * (SPos(idx)
		- SPos(vwList.GetHeadIndex())));
}
SDst
AMUnitList::GetViewPosition() const
{
	return SDst(GetItemHeight() * vwList.GetHeadIndex() + uTopOffset);
}

SDst
AMUnitList::AdjustBottomForHeight(SDst item_h, SDst h)
{
	const auto d(uTopOffset);

	uTopOffset = 0;
	AdjustViewLengthForHeight(item_h, h);
	return d;
}

SDst
AMUnitList::AdjustOffsetForHeight(SDst h, bool is_top)
{
	if(GetFullViewHeight() > h)
	{
		const SDst item_h(GetItemHeight());

		if(YB_UNLIKELY(item_h == 0))
			return 0;
		vwList.RestrictSelected();
		if(is_top)
			return AdjustBottomForHeight(item_h, h);
		else
		{
			const auto d((h + uTopOffset) % item_h);

			if(d != 0)
			{
				const auto tmp(uTopOffset + item_h - d);

				uTopOffset = tmp % item_h;
				AdjustViewLengthForHeight(item_h, h);
			// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
				vwList.IncreaseHead(ptrdiff_t(tmp / item_h), GetTotal());
			}
			return d;
		}
	}
	return 0;
}

void
AMUnitList::AdjustViewForContent(SDst h)
{
	if(vwList.AdjustForContent(GetTotal()) && vwList.IsSelected())
	{
		AdjustOffsetForHeight(h,
			vwList.GetSelectedIndex() == vwList.GetHeadIndex());
		return;
	}
	if(GetFullViewHeight() < GetViewPosition() + h)
		uTopOffset = 0;
	AdjustViewLengthForHeight(GetItemHeight(), h);
}

void
AMUnitList::AdjustViewLengthForHeight(SDst item_h, SDst h)
{
	if(YB_LIKELY(item_h != 0))
		vwList.Length = size_t((h + uTopOffset + item_h - 1) / item_h);
}

size_t
AMUnitList::CheckPoint(const Size& s, const Point& pt)
{
	return Rect(s).Contains(pt) ? size_t((pt.Y + SPos(uTopOffset))
		/ SPos(GetItemHeight())) + vwList.GetHeadIndex() : size_t(-1);
}

void
AMUnitList::ResetView()
{
	bool b(vwList.IsSelected());

	vwList.Reset();
	if(b)
		vwList.SetSelectedIndex(0, GetTotal());
	yunseq(uTopOffset = 0, idxShared = size_t(-1));
}

} // namespace UI;

} // namespace YSLib;

