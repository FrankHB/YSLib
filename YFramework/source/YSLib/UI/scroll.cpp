/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file scroll.cpp
\ingroup UI
\brief 样式相关的图形用户界面滚动控件。
\version r4155;
\author FrankHB<frankhb1989@gmail.com>
\since build 194 。
\par 创建时间:
	2011-03-07 20:12:02 +0800;
\par 修改时间:
	2012-03-18 16:04 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::Scroll;
*/


#include "YSLib/UI/scroll.h"
#include "YSLib/UI/ygui.h"
#include "YSLib/Core/ystorage.hpp"
#include <ystdex/algorithm.hpp>

using namespace ystdex;

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

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
		if(need_h ^ need_v)
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
}


ATrack::ATrack(const Rect& r, SDst uMinThumbLength)
	: Control(Rect(r.GetPoint(),
		max<SDst>(defMinScrollBarWidth, r.Width),
		max<SDst>(defMinScrollBarHeight, r.Height))),
	GMRange<ValueType>(0xFF, 0),
	Thumb(Rect(0, 0, defMinScrollBarWidth, defMinScrollBarHeight)),
	min_thumb_length(uMinThumbLength), large_delta(min_thumb_length)
{
	SetContainerPtrOf(Thumb, this);
	yunseq(
		GetThumbDrag() += [this](UIEventArgs&&){
			LocateThumb(0, ScrollCategory::ThumbTrack);
		},
		FetchEvent<TouchMove>(*this) += OnTouchMove,
		FetchEvent<TouchDown>(*this) += [this](TouchEventArgs&& e){
			if(e.Strategy == RoutedEventArgs::Direct
				&& &e.GetSender() == this
				&& Rect(Point::Zero, GetSizeOf(*this)).Contains(e))
			{
				ScrollCategory t;

				switch(CheckArea(e.GetRef(IsHorizontal())))
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

IWidget*
ATrack::GetTopWidgetPtr(const Point& pt, bool(&f)(const IWidget&))
{
	return CheckWidget(Thumb, pt, f);
}

void
ATrack::SetThumbLength(SDst l)
{
	RestrictInInterval(l, min_thumb_length, GetTrackLength());

	Size s(GetSizeOf(Thumb));
	const bool is_h(IsHorizontal());

	if(l != s.GetRef(is_h))
	{
		Invalidate(Thumb);
		s.GetRef(is_h) = l;
		SetSizeOf(Thumb, s);
	}
}
void
ATrack::SetThumbPosition(SPos pos)
{
	RestrictInClosedInterval(pos, 0, GetScrollableLength());

	Point pt(GetLocationOf(Thumb));
	const bool is_h(IsHorizontal());

	if(pos != pt.GetRef(is_h))
	{
		Invalidate(Thumb);
		pt.GetRef(is_h) = pos;
		SetLocationOf(Thumb, pt);
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

void
ATrack::Refresh(PaintEventArgs&& e)
{
	if(!IsTransparent())
	{
		const auto& g(e.Target);
		const auto& pt(e.Location);
		auto& pal(FetchGUIState().Colors);

		FillRect(g, pt, GetSizeOf(*this), pal[Styles::Track]);
	//	FillRect(g, r, pal[Styles::Track]);

		const SPos xr(pt.X + GetWidth());
		const SPos yr(pt.Y + GetHeight());
		const Color& c(pal[Styles::Light]);

		if(IsHorizontal())
		{
			DrawHLineSeg(g, pt.Y, pt.X, xr, c),
			DrawHLineSeg(g, yr, pt.X, xr, c);
		}
		else
		{
			DrawVLineSeg(g, pt.X, pt.Y, yr, c),
			DrawVLineSeg(g, xr, pt.Y, yr, c);
		}
	}
	PaintChild(Thumb, e);
	e.ClipArea = Rect(e.Location, GetSizeOf(*this));
	// NOTE: partial invalidation made no efficiency improved here;
	/*
		const auto& g(e.Target);
		const auto& pt(e.Location);
		auto& st(FetchGUIState().Colors);

		FillRect(g, r, pal[Styles::Track]);

		const Color& c(pal[Styles::Light]);
		SPos x(pt.X);
		SPos y(pt.Y);
		SPos xr(x + GetWidth());
		SPos yr(y + GetHeight());

		if(IsHorizontal())
		{
			RestrictInInterval(y, r.Y, r.Y + r.Height),
			RestrictInInterval(yr, r.Y, r.Y + r.Height);
			DrawHLineSeg(g, pt.Y, pt.X, xr, c),
			DrawHLineSeg(g, yr, pt.X, xr, c);
		}
		else
		{
			RestrictInInterval(x, r.X, r.X + r.Width),
			RestrictInInterval(xr, r.X, r.X + r.Width);
			DrawVLineSeg(g, pt.X, pt.Y, yr, c),
			DrawVLineSeg(g, xr, pt.Y, yr, c);
		}
	}
	PaintChild(Thumb, e);
	return r;
	*/
}

ATrack::Area
ATrack::CheckArea(SPos q) const
{
	if(q >= 0)
	{
		yconstexpr Area lst[] = {OnPrev, OnThumb, OnNext};
		const SPos a[] = {0, GetThumbPosition(),
			SPos(GetThumbPosition() + GetThumbLength())};
		const size_t n(SwitchInterval(q, a, 3));

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
		value = GetThumbPosition() == GetScrollableLength() ? max_value
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
		case ScrollCategory::Last:
			value = max_value;
			SetThumbPosition(GetScrollableLength());
		default:
			;
		}
	}
	GetScroll()(ScrollEventArgs(*this, t, value, old_value));
}


HorizontalTrack::HorizontalTrack(const Rect& r, SDst uMinThumbLength)
	: ATrack(r, uMinThumbLength)
{
	YAssert(GetWidth() > GetHeight(),
		"In constructor Components::\n"
			"HorizontalTrack::HorizontalTrack"
		"(const Rect& r, SDst uMinThumbLength) const\": \n"
		"Width is not greater than height.");

	FetchEvent<TouchMove>(Thumb) +=[this](TouchEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			auto& st(FetchGUIState());
			SPos x(st.LastControlLocation.X + st.DraggingOffset.X);

			RestrictInClosedInterval(x, 0, GetWidth() - Thumb.GetWidth());
			Invalidate(Thumb);
			SetLocationOf(Thumb, Point(x, GetLocationOf(Thumb).Y));
			GetThumbDrag()(UIEventArgs(*this));
		}
	};
}


VerticalTrack::VerticalTrack(const Rect& r, SDst uMinThumbLength)
	: ATrack(r, uMinThumbLength)
{
	YAssert(GetHeight() > GetWidth(),
		"In constructor Components::\n"
			"HorizontalTrack::HorizontalTrack"
		"(const Rect& r, SDst uMinThumbLength) const\": \n"
		"height is not greater than width.");

	FetchEvent<TouchMove>(Thumb) += [this](TouchEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			auto& st(FetchGUIState());
			SPos y(st.LastControlLocation.Y + st.DraggingOffset.Y);

			RestrictInClosedInterval(y, 0, GetHeight() - Thumb.GetHeight());
			Invalidate(Thumb);
			SetLocationOf(Thumb, Point(GetLocationOf(Thumb).X, y));
			GetThumbDrag()(UIEventArgs(*this));
		}
	};
}


AScrollBar::AScrollBar(const Rect& r, SDst uMinThumbSize, Orientation o)
try	: Control(r),
	pTrack(o == Horizontal
		? static_cast<ATrack*>(new HorizontalTrack(
			Rect(r.Height, 0, r.Width - r.Height * 2, r.Height), uMinThumbSize))
		: static_cast<ATrack*>(new VerticalTrack(
			Rect(0, r.Width, r.Width, r.Height - r.Width * 2), uMinThumbSize))),
	btnPrev(Rect()), btnNext(Rect()), small_delta(2)
{
	SetContainerPtrOf(*pTrack, this),
	SetContainerPtrOf(btnPrev, this);
	SetContainerPtrOf(btnNext, this);
	yunseq(
		FetchEvent<KeyHeld>(*this) += OnKeyHeld,
		FetchEvent<TouchMove>(btnPrev) += OnTouchMove,
		FetchEvent<TouchDown>(btnPrev) += [this](TouchEventArgs&&){
			LocateThumb(small_delta, ScrollCategory::SmallDecrement);
		},
		FetchEvent<TouchMove>(btnNext) += OnTouchMove,
		FetchEvent<TouchDown>(btnNext) += [this](TouchEventArgs&&){
			LocateThumb(small_delta, ScrollCategory::SmallIncrement);
		},
		FetchEvent<KeyUp>(*this) += OnKey_Bound_TouchUpAndLeave,
		FetchEvent<KeyDown>(*this) += OnKey_Bound_EnterAndTouchDown
	);

	Size s(GetSizeOf(*this));
	const bool bHorizontal(o == Horizontal);
	const SDst l(s.GetRef(!bHorizontal));

	s.GetRef(bHorizontal) = l;
	SetSizeOf(btnPrev, s);
	SetSizeOf(btnNext, s);
//	Button.SetLocationOf(btnPrev, Point::Zero);
	MoveToBottom(btnNext);
	MoveToRight(btnNext);
}
catch(...)
{
	throw LoggedEvent("Error occured @ constructor of AScrollBar;");
}

IWidget*
AScrollBar::GetTopWidgetPtr(const Point& pt, bool(&f)(const IWidget&))
{
	if(auto p = CheckWidget(btnPrev, pt, f))
		return p;
	if(auto p = CheckWidget(btnNext, pt, f))
		return p;

	YAssert(bool(pTrack),
		"Null widget pointer found @ AScrollBar::GetTopWidgetPtr;");

	return f(*pTrack.get()) ? pTrack.get() : nullptr;
}

void
AScrollBar::Refresh(PaintEventArgs&& e)
{
	YAssert(bool(pTrack), "Null widget pointer found @ AScrollBar::Draw;");

	PaintChild(*pTrack, e),
	PaintChild(btnPrev, e),
	PaintChild(btnNext, e);
	WndDrawArrow(e.Target, Rect(e.Location + GetLocationOf(btnPrev),
		GetSizeOf(btnPrev)), 4, pTrack->GetOrientation() == Horizontal
		? RDeg180 : RDeg90, ForeColor),
	WndDrawArrow(e.Target, Rect(e.Location + GetLocationOf(btnNext),
		GetSizeOf(btnNext)), 4, pTrack->GetOrientation() == Horizontal
		? RDeg0 : RDeg270, ForeColor);
	e.ClipArea = Rect(e.Location, GetSizeOf(*this));
}


HorizontalScrollBar::HorizontalScrollBar(const Rect& r, SDst uMinThumbLength)
	: AScrollBar(r, uMinThumbLength, Horizontal)
{}

IWidget*
HorizontalScrollBar::GetBoundControlPtr(const KeyCode& k)
{
	if(k == KeySpace::Left)
		return &btnPrev;
	if(k == KeySpace::Right)
		return &btnNext;
	return nullptr;
}


VerticalScrollBar::VerticalScrollBar(const Rect& r, SDst uMinThumbLength)
	: AScrollBar(r, uMinThumbLength, Vertical)
{}

IWidget*
VerticalScrollBar::GetBoundControlPtr(const KeyCode& k)
{
	if(k == KeySpace::Up)
		return &btnPrev;
	if(k == KeySpace::Down)
		return &btnNext;
	return nullptr;
}


ScrollableContainer::ScrollableContainer(const Rect& r)
	: Control(r),
	HorizontalScrollBar(Rect(Point::Zero, r.Width, defMinScrollBarHeight)),
	VerticalScrollBar(Rect(Point::Zero, defMinScrollBarWidth, r.Height))
{
	SetTransparent(true);
	SetContainerPtrOf(HorizontalScrollBar, this),
	SetContainerPtrOf(VerticalScrollBar, this);
	MoveToBottom(HorizontalScrollBar);
	MoveToRight(VerticalScrollBar);
}

IWidget*
ScrollableContainer::GetTopWidgetPtr(const Point& pt, bool(&f)(const IWidget&))
{
	if(auto p = CheckWidget(HorizontalScrollBar, pt, f))
		return p;
	if(auto p = CheckWidget(VerticalScrollBar, pt, f))
		return p;
	return nullptr;
}

void
ScrollableContainer::Refresh(PaintEventArgs&& e)
{
	if(IsVisible(HorizontalScrollBar))
		e.ClipArea = Unite(e.ClipArea, PaintChild(HorizontalScrollBar, e));
	if(IsVisible(VerticalScrollBar))
		e.ClipArea = Unite(e.ClipArea, PaintChild(VerticalScrollBar, e));
}

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
			HorizontalScrollBar.SetWidth(GetWidth() - defMinScrollBarWidth);
			VerticalScrollBar.SetHeight(GetHeight() - defMinScrollBarHeight);
		}
		else if(p.first)
		{
			HorizontalScrollBar.SetWidth(GetWidth());
			MoveToBottom(HorizontalScrollBar);
		}
		else if(p.second)
		{
			VerticalScrollBar.SetHeight(GetHeight());
			MoveToRight(VerticalScrollBar);
		}
		SetVisibleOf(HorizontalScrollBar, p.first);
		SetVisibleOf(VerticalScrollBar, p.second);
	}
	catch(GeneralEvent&)
	{}
	return arena;
}

YSL_END_NAMESPACE(Components)

YSL_END

