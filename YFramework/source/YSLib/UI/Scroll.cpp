/*
	© 2011-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Scroll.cpp
\ingroup UI
\brief 样式相关的图形用户界面滚动控件。
\version r3791
\author FrankHB <frankhb1989@gmail.com>
\since build 194
\par 创建时间:
	2011-03-07 20:12:02 +0800
\par 修改时间:
	2017-06-05 02:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Scroll
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Scroll
#include YFM_YSLib_UI_YGUI
#include YFM_YSLib_Core_YStorage

using namespace ystdex;

namespace YSLib
{

namespace UI
{

ImplDeDtor(ScrollEventArgs)

namespace
{

pair<bool, bool>
FixScrollBarLayout(Size& d, const Size& s, SDst min_width, SDst min_height)
{
	bool need_h(d.Width < s.Width), need_v(d.Height < s.Height);

	if(need_h)
	{
		if(d.Height < min_height)
			throw GeneralEvent("Scroll bar need more height.");
		d.Height -= min_height;
	}
	if(need_v)
	{
		if(d.Width < min_width)
			throw GeneralEvent("Scroll bar need more width.");
		d.Width -= min_width;
	}
	if(need_h != need_v)
	{
		if(!need_h && d.Width < s.Width)
		{
			need_h = true;
			if(d.Height < min_height)
				throw GeneralEvent("Scroll bar need more height.");
			d.Height -= min_height;
		}
		if(!need_v && d.Height < s.Height)
		{
			need_v = true;
			if(d.Width < min_width)
				throw GeneralEvent("Scroll bar need more width.");
			d.Width -= min_width;
		}
	}
	return pair<bool, bool>(need_h, need_v);
}

const SDst defMinScrollBarWidth(16); //!< 默认最小滚动条宽。
const SDst defMinScrollBarHeight(16); //!< 默认最小滚动条高。

} //unnamed namespace;


ATrack::ATrack(const Rect& r, SDst uMinThumbLength)
	: Control({r.GetPoint(), max<SDst>(defMinScrollBarWidth, r.Width),
	max<SDst>(defMinScrollBarHeight, r.Height)}),
	GMRange<ValueType>(0xFF, 0),
	tmbScroll(Size(defMinScrollBarWidth, defMinScrollBarHeight)),
	min_thumb_length(uMinThumbLength), large_delta(min_thumb_length)
{
	SetContainerPtrOf(tmbScroll, make_observer(this));
	yunseq(
	Background = ystdex::bind1(DrawTrackBackground, std::ref(*this)),
	ThumbDrag += [this]{
		LocateThumb(0, ScrollCategory::ThumbTrack);
	},
	FetchEvent<TouchHeld>(*this) += OnTouchHeld,
	FetchEvent<TouchDown>(*this) += [this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct && &e.GetSender() == this
			&& Rect(GetSizeOf(*this)).Contains(e))
		{
			ScrollCategory t;

			switch(CheckArea(e.Position.GetRef(IsHorizontal())))
			{
			case OnPrev:
				t = ScrollCategory::LargeDecrement;
				break;
			case OnNext:
				t = ScrollCategory::LargeIncrement;
				break;
			case None:
				return;
			default:
				t = ScrollCategory::EndScroll;
			}
			LocateThumb(0, t);
		}
	}
	);
}

void
ATrack::SetThumbLength(SDst l)
{
	RestrictInInterval(l, min_thumb_length, GetTrackLength());

	Size s(GetSizeOf(tmbScroll));
	const bool is_h(IsHorizontal());

	if(l != s.GetRef(is_h))
	{
		Invalidate(tmbScroll);
		s.GetRef(is_h) = l;
		SetSizeOf(tmbScroll, s);
	}
}
void
ATrack::SetThumbPosition(SPos pos)
{
	// XXX: Conversion to 'SPos' might be implementation-defined.
	RestrictInClosedInterval<SPos>(pos, 0, SPos(GetScrollableLength()));

	Point pt(GetLocationOf(tmbScroll));
	const bool is_h(IsHorizontal());

	if(pos != pt.GetRef(is_h))
	{
		Invalidate(tmbScroll);
		pt.GetRef(is_h) = pos;
		SetLocationOf(tmbScroll, pt);
	}
}
void
ATrack::SetMaxValue(ValueType m)
{
	if(m > 0)
	{
		if(large_delta > m)
			large_delta = m;
		max_value = m;
	}
}
void
ATrack::SetValue(ValueType val)
{
	value = val;
	SetThumbPosition(SPos(round(val * GetScrollableLength() / max_value)));
}
void
ATrack::SetLargeDelta(ValueType val)
{
	large_delta = val;
	SetThumbLength(SDst(round(val * GetTrackLength() / (val + max_value))));
}

ATrack::Area
ATrack::CheckArea(SPos q) const
{
	if(q >= 0)
	{
		yconstexpr const Area lst[]{OnPrev, OnThumb, OnNext};
		const SPos a[]{SPos(), SPos(GetThumbPosition()),
			SPos(GetThumbPosition() + SPos(GetThumbLength()))};
		const auto n(SwitchInterval(q, a, 3));

		if(n < 3)
			return lst[n];
	}
	return None;
}

void
ATrack::LocateThumb(ValueType val, ScrollCategory t)
{
	ValueType old_value(value);

	if(t == ScrollCategory::ThumbTrack)
		// XXX: Conversion to 'SPos' might be implementation-defined.
		value = GetThumbPosition() == SPos(GetScrollableLength()) ? max_value
			: max_value * GetThumbPosition()
			/ (GetTrackLength() - GetThumbLength());
	else
	{
		if(t == ScrollCategory::LargeDecrement
			|| t == ScrollCategory::LargeIncrement)
			val = GetLargeDelta();
		switch(t)
		{
		case ScrollCategory::SmallDecrement:
		case ScrollCategory::LargeDecrement:
			if(value > val)
			{
				SetValue(value - val);
				break;
			}
			YB_ATTR_fallthrough;
		case ScrollCategory::First:
			value = 0;
			SetThumbPosition(0);
			break;
		case ScrollCategory::SmallIncrement:
		case ScrollCategory::LargeIncrement:
			if(value + val < max_value)
			{
				SetValue(value + val);
				break;
			}
			YB_ATTR_fallthrough;
		case ScrollCategory::Last:
			value = max_value;
			// XXX: Conversion to 'SPos' might be implementation-defined.
			SetThumbPosition(SPos(GetScrollableLength()));
		default:
			;
		}
	}
	Scroll(ScrollEventArgs(*this, t, value, old_value));
}


void
DrawTrackBackground(PaintEventArgs&& e, ATrack& trk)
{
	const auto& g(e.Target);
	const auto& pt(e.Location);
	const Rect& bounds(Rect(pt, GetSizeOf(trk)) & e.ClipArea);
	auto& pal(FetchGUIConfiguration().Colors);

	FillRect(g, bounds, pal[Styles::Track]);

#define YSL_UI_Impl_ATrack_Partial_Invalidation 1
	// NOTE: Partial invalidation made no efficiency improved here.
	const auto c(pal[Styles::Light]);
#if YSL_UI_Impl_ATrack_Partial_Invalidation
	SPos x(pt.X);
	SPos y(pt.Y);
	// XXX: Conversion to 'SPos' might be implementation-defined.
	SPos xr(x + SPos(trk.GetWidth()));
	SPos yr(y + SPos(trk.GetHeight()));
#else
	const SPos xr(pt.X + trk.GetWidth());
	const SPos yr(pt.Y + trk.GetHeight());
#endif

	if(trk.IsHorizontal())
	{
#if YSL_UI_Impl_ATrack_Partial_Invalidation
		RestrictInInterval(y, bounds.Y, bounds.GetBottom()),
		RestrictInInterval(yr, bounds.Y, bounds.GetBottom());
#endif
		DrawHLineSeg(g, bounds, pt.Y, pt.X, xr, c),
		DrawHLineSeg(g, bounds, yr, pt.X, xr, c);
	}
	else
	{
#if YSL_UI_Impl_ATrack_Partial_Invalidation
		RestrictInInterval(x, bounds.X, bounds.GetRight()),
		RestrictInInterval(xr, bounds.X, bounds.GetRight());
#endif
		DrawVLineSeg(g, bounds, pt.X, pt.Y, yr, c),
		DrawVLineSeg(g, bounds, xr, pt.Y, yr, c);
	}
}


HorizontalTrack::HorizontalTrack(const Rect& r, SDst uMinThumbLength)
	: ATrack(r, uMinThumbLength)
{
	YAssert(GetWidth() > GetHeight(), "Width is not greater than height.");
	FetchEvent<TouchHeld>(tmbScroll) += [this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			auto& st(FetchGUIState());

			if(st.CheckDraggingOffset())
			{
				SPos x(st.CursorLocation.X + st.DraggingOffset.X);

				// XXX: Conversion to 'SPos' might be implementation-defined.
				RestrictInClosedInterval<SPos>(x, 0,
					SPos(GetWidth()) - SPos(tmbScroll.GetWidth()));
				Invalidate(tmbScroll);
				SetLocationOf(tmbScroll, Point(x, GetLocationOf(tmbScroll).Y));
				ThumbDrag(UIEventArgs(*this));
			}
		}
	};
}
ImplDeDtor(HorizontalTrack)


VerticalTrack::VerticalTrack(const Rect& r, SDst uMinThumbLength)
	: ATrack(r, uMinThumbLength)
{
	YAssert(GetHeight() > GetWidth(), "height is not greater than width.");
	FetchEvent<TouchHeld>(tmbScroll) += [this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			auto& st(FetchGUIState());

			if(st.CheckDraggingOffset())
			{
				SPos y(st.CursorLocation.Y + st.DraggingOffset.Y);

				// XXX: Conversion to 'SPos' might be implementation-defined.
				RestrictInClosedInterval<SPos>(y, 0,
					SPos(GetHeight()) - SPos(tmbScroll.GetHeight()));
				Invalidate(tmbScroll);
				SetLocationOf(tmbScroll, Point(GetLocationOf(tmbScroll).X, y));
				ThumbDrag(UIEventArgs(*this));
			}
		}
	};
}
ImplDeDtor(VerticalTrack)


ScrollBar::ScrollBar(const Rect& r, Orientation o, SDst min_thumb)
	: Control(r),
	// XXX: Conversion to 'SPos' might be implementation-defined.
	pTrack(o == Horizontal ? static_cast<ATrack*>(new
	HorizontalTrack(Rect(SPos(r.Height), 0, r.Width - r.Height * 2, r.Height),
	min_thumb)) : static_cast<ATrack*>(new VerticalTrack(Rect(0, SPos(r.Width),
	r.Width, r.Height - r.Width * 2), min_thumb))),
	btnPrev(Rect()), btnNext(Rect()), small_delta(2)
{
	SetContainerPtrOf(*pTrack, make_observer(this)),
	SetContainerPtrOf(btnPrev, make_observer(this));
	SetContainerPtrOf(btnNext, make_observer(this));
	yunseq(
	FetchEvent<Resize>(*this) += [this](UIEventArgs&& e){
		auto& track(GetTrackRef());
		const bool is_h(track.IsHorizontal());
		const SDst prev_metric(GetSizeOf(btnPrev).GetRef(is_h));
		const SDst sum(prev_metric + GetSizeOf(btnNext).GetRef(is_h));

		YAssert(GetSizeOf(e.GetSender()).GetRef(is_h) - sum > 0,
			"No enough space for track.");

		const SDst tl(GetSizeOf(e.GetSender()).GetRef(is_h) - sum);
		auto pt(GetLocationOf(btnNext));
		Size s(GetSizeOf(track));

		// XXX: Conversion to 'SPos' might be implementation-defined.
		yunseq(pt.GetRef(is_h) = SPos(tl + prev_metric), s.GetRef(is_h) = tl);
		btnNext.GetView().SetLocation(pt), track.GetView().SetSize(s);
		// NOTE: No event %(Resize, Move) is raised.
	},
	FetchEvent<KeyUp>(*this) += OnKey_Bound_TouchUp,
	FetchEvent<KeyDown>(*this) += OnKey_Bound_TouchDown,
	FetchEvent<KeyHeld>(*this) += OnKeyHeld,
	FetchEvent<CursorWheel>(*this) += [this](CursorWheelEventArgs&& e){
		if(e.GetDelta() > 0)
			LocateThumb(small_delta, ScrollCategory::SmallDecrement);
		else if(e.GetDelta() < 0)
			LocateThumb(small_delta, ScrollCategory::SmallIncrement);
	},
	FetchEvent<TouchHeld>(btnPrev) += OnTouchHeld,
	FetchEvent<TouchDown>(btnPrev) += [this]{
		LocateThumb(small_delta, ScrollCategory::SmallDecrement);
	},
	FetchEvent<TouchHeld>(btnNext) += OnTouchHeld,
	FetchEvent<TouchDown>(btnNext) += [this]{
		LocateThumb(small_delta, ScrollCategory::SmallIncrement);
	}
	);

	Size s(GetSizeOf(*this));
	const bool bHorizontal(o == Horizontal);
	const SDst l(s.GetRef(!bHorizontal));

	s.GetRef(bHorizontal) = l;
	SetSizeOf(btnPrev, s);
	SetSizeOf(btnNext, s);
//	Button.SetLocationOf(btnPrev, Point());
	MoveToBottom(btnNext);
	MoveToRight(btnNext);
	if(o == Horizontal)
		InitializeArrowPainters(RDeg180, RDeg0);
	else
		InitializeArrowPainters(RDeg90, RDeg270);
}

void
ScrollBar::InitializeArrowPainters(Rotation prev, Rotation next)
{
	using namespace std;
	using namespace placeholders;

	yunseq(
	FetchEvent<Paint>(btnPrev) += [this, prev](PaintEventArgs&& e){
		DrawArrow(e.Target, e.ClipArea, {e.Location, GetSizeOf(btnPrev)}, 4,
			prev, ForeColor);
	},
	FetchEvent<Paint>(btnNext) += [this, next](PaintEventArgs&& e){
		DrawArrow(e.Target, e.ClipArea, {e.Location, GetSizeOf(btnNext)}, 4,
			next, ForeColor);
	}
	);
}

observer_ptr<IWidget>
ScrollBar::GetBoundControlPtr(const KeyInput& k)
{
	if(k.count() == 1)
	{
		if(GetOrientation() == Horizontal)
		{
			if(k[KeyCodes::Left])
				return make_observer(&btnPrev);
			if(k[KeyCodes::Right])
				return make_observer(&btnNext);
		}
		else
		{
			if(k[KeyCodes::Up])
				return make_observer(&btnPrev);
			if(k[KeyCodes::Down])
				return make_observer(&btnNext);
		}
	}
	return {};
}


ScrollableContainer::ScrollableContainer(const Rect& r)
	: Control(r, MakeBlankBrush()),
	hsbHorizontal(Size(r.Width, defMinScrollBarHeight), Horizontal),
	vsbVertical(Size(defMinScrollBarWidth, r.Height), Vertical)
{
	// TODO: Allow user to choose whether background is drawn.
	SetContainerPtrOf(hsbHorizontal, make_observer(this)),
	SetContainerPtrOf(vsbVertical, make_observer(this)),
	MoveToBottom(hsbHorizontal),
	MoveToRight(vsbVertical),
	FetchEvent<CursorWheel>(*this) += [this](CursorWheelEventArgs&& e){
		if(IsVisible(vsbVertical))
			CallEvent<CursorWheel>(vsbVertical, std::move(e));
	};
}
ImplDeDtor(ScrollableContainer)

Size
ScrollableContainer::FixLayout(const Size& s)
{
	Size arena(GetSizeOf(*this));

	try
	{
		const pair<bool, bool> p(FixScrollBarLayout(arena, s,
			defMinScrollBarWidth, defMinScrollBarHeight));

		if(p.first && p.second && GetWidth() > defMinScrollBarWidth
			&& GetHeight() > defMinScrollBarHeight)
		{
			hsbHorizontal.SetWidth(GetWidth() - defMinScrollBarWidth);
			vsbVertical.SetHeight(GetHeight() - defMinScrollBarHeight);
		}
		else if(p.first)
		{
			SetSizeOf(hsbHorizontal, Size(GetWidth(),
				hsbHorizontal.GetHeight()));
			MoveToBottom(hsbHorizontal);
		}
		else if(p.second)
		{
			SetSizeOf(vsbVertical, Size(vsbVertical.GetWidth(),
				GetHeight()));
			MoveToRight(vsbVertical);
		}
		SetVisibleOf(hsbHorizontal, p.first);
		SetVisibleOf(vsbVertical, p.second);
	}
	CatchIgnore(GeneralEvent&)
	return arena;
}

} // namespace UI;

} // namespace YSLib;

