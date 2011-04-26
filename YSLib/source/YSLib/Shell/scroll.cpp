/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file scroll.cpp
\ingroup Shell
\brief 样式相关的图形用户界面滚动控件。
\version 0.3568;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-03-07 20:12:02 +0800;
\par 修改时间:
	2011-04-25 12:49 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::Scroll;
*/


#include "scroll.h"
#include "ywindow.h"
#include "ygdi.h"
#include "ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

using ystdex::vmax;

namespace
{
	void
	RectDrawArrow(const Graphics& g, const Point& p, SDst halfSize,
		Rotation rot = RDeg0, Color c = Drawing::ColorSpace::Black)
	{
		YAssert(g.IsValid(), "err: graphics context is invalid.");

		SDst x(p.X), y(p.Y);

		switch(rot)
		{
		case RDeg0:
			{
				SDst t(p.Y);

				for(SDst i(0); i < halfSize; ++i)
					DrawVLineSeg(g, x--, y--, t++, c);
			}
			break;
		case RDeg90:
			{
				SDst t(p.X);

				for(SDst i(0); i < halfSize; ++i)
					DrawHLineSeg(g, y++, x--, t++, c);
			}
			break;
		case RDeg180:
			{
				SDst t(p.Y);

				for(SDst i(0); i < halfSize; ++i)
					DrawVLineSeg(g, x++, y--, t++, c);
			}
			break;
		case RDeg270:
			{
				SDst t(p.X);

				for(SDst i(0); i < halfSize; ++i)
					DrawHLineSeg(g, y--, x--, t++, c);
			}
		default:
			break;
		}
	}

	void
	WndDrawArrow(const Graphics& g, const Rect& r, SDst halfSize,
		Rotation rot = RDeg0, Color c = Drawing::ColorSpace::Black)
	{
		SPos x(r.X), y(r.Y);

		switch(rot)
		{
		case RDeg0:
		case RDeg180:
			x += (rot == RDeg180
				? (r.Width - halfSize) : (r.Width + halfSize)) / 2;
			y += (r.Height + 1) / 2;
			break;
		case RDeg90:
		case RDeg270:
			y += (rot == RDeg90
				? (r.Height - halfSize) : (r.Height + halfSize)) / 2;
			x += (r.Width + 1) / 2;
		default:
			break;
		}
		RectDrawArrow(g, Point(x, y), halfSize, rot, c);
	}

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


ATrack::Dependencies::Dependencies()
{
	ThumbDrag.GetRef() += &ATrack::OnThumbDrag;
}

ATrack::ATrack(const Rect& r, SDst uMinThumbLength)
	: AUIBoxControl(Rect(r.GetPoint(),
		vmax<SDst>(defMinScrollBarWidth, r.Width),
		vmax<SDst>(defMinScrollBarHeight, r.Height))),
	GMRange<u16>(0xFF, 0),
	Thumb(Rect(0, 0, defMinScrollBarWidth, defMinScrollBarHeight)),
	min_thumb_length(uMinThumbLength), large_delta(min_thumb_length),
	Events(GetStaticRef<Dependencies>())
{
	Thumb.GetContainerPtr() = this;
	FetchEvent<TouchMove>(*this) += OnTouchMove;
	FetchEvent<TouchDown>(*this) += &ATrack::OnTouchDown;
}

IControl*
ATrack::GetTopControlPtr(const Point& p)
{
	return Contains(Thumb, p) ? &Thumb : nullptr;
}

void
ATrack::SetThumbLength(SDst l)
{
	RestrictInClosedInterval(l, min_thumb_length, GetTrackLength());

	Size s(Thumb.GetSize());

	UpdateTo(s, l, IsHorizontal());
	Thumb.SetSize(s);
	Refresh();
}
void
ATrack::SetThumbPosition(SPos pos)
{
	RestrictInClosedInterval(pos, 0, GetTrackLength() - GetThumbLength());

	Point p(Thumb.GetLocation());

	UpdateTo(p, pos, IsHorizontal());
	Thumb.SetLocation(p);
	Refresh();
}
void
ATrack::SetMaxValue(ValueType m)
{
	if(m > 1)
	{
		if(large_delta >= m)
			large_delta = m - 1;
		max_value = m;
	}
}
void
ATrack::SetValue(ATrack::ValueType v)
{
	value = v;
	// TODO: check ValueType incompatibility(perhaps overflow);
	SetThumbPosition(v * GetTrackLength() / max_value);
}
void
ATrack::SetLargeDelta(ATrack::ValueType ld)
{
	large_delta = ld;
	// TODO: check ValueType incompatibility(perhaps overflow);
	SetThumbLength(ld * GetTrackLength() / max_value);
}

void
ATrack::Paint()
{
	YWidgetAssert(this, Controls::ATrack, Paint);

	Control::Paint();
	if(!IsTransparent())
	{
		const Graphics& g(FetchDirectWindowPtr(*this)->GetContext());
		const Point loc(LocateForWindow(*this));
		Styles::Palette& pal(FetchGUIShell().Colors);

		FillRect(g, loc, GetSize(), pal[Styles::Track]);

		const SPos xr(loc.X + GetWidth() - 1);
		const SPos yr(loc.Y + GetHeight() - 1);
		const Color& c(pal[Styles::Light]);

		if(IsHorizontal())
		{
			DrawHLineSeg(g, loc.Y, loc.X, xr, c);
			DrawHLineSeg(g, yr, loc.X, xr, c);
		}
		else
		{
			DrawVLineSeg(g, loc.X, loc.Y, yr, c);
			DrawVLineSeg(g, xr, loc.Y, yr, c);
		}
	}
	Thumb.Paint();
}

ATrack::Area
ATrack::CheckArea(SDst q) const
{
	const Area lst[] = {OnPrev, OnThumb, OnNext};
	const SDst a[] = {0, GetThumbPosition(),
		GetThumbPosition() + GetThumbLength()}; 
	size_t n(SwitchInterval(q, a, 3));

	YAssert(n < 3,
		"In function \"Components::Controls::ATrack::Area\n"
		"Components::Controls::ATrack::CheckArea(SPos q) const\": \n"
		"Array index is out of bound.");

	return lst[n];
}

void
ATrack::CheckScroll(ScrollEventSpace::ScrollEventType t, ValueType old_value)
{
	ScrollEventArgs e(t, value, old_value);

	GetScroll()(*this, e);
}

void
ATrack::LocateThumb(ScrollEventSpace::ScrollEventType t, ValueType v)
{
	switch(t)
	{
	case ScrollEventSpace::First:
		v = 0;
		break;
	case ScrollEventSpace::Last:
		v = max_value - large_delta;
		break;
	default:
		break;
	}

	ValueType old_value(value);

	SetValue(v);
	CheckScroll(t, old_value);
}

void
ATrack::LocateThumbForIncrement(ScrollEventSpace::ScrollEventType t,
	ValueType abs_delta)
{
	ValueType v(value);

	const ValueType m(max_value - large_delta);

	if(v + abs_delta > m)
		v = m;
	else
		v += abs_delta;
	LocateThumb(t, v);
}

void
ATrack::LocateThumbForDecrement(ScrollEventSpace::ScrollEventType t,
	ValueType abs_delta)
{
	ValueType v(value);

	if(v < abs_delta)
		v = 0;
	else
		v -= abs_delta;
	LocateThumb(t, v);
}

void
ATrack::UpdateValue()
{
	// TODO: check ValueType incompatibility(perhaps overflow);
	value = GetThumbPosition() * max_value / GetTrackLength();
}

void
ATrack::OnTouchDown(TouchEventArgs& e)
{
	if(e.Strategy == RoutedEventArgs::Direct
		&& Rect(Point::Zero, GetSize()).Contains(e))
	{
		using namespace ScrollEventSpace;

		switch(CheckArea(SelectFrom(e, IsHorizontal())))
		{
		case OnPrev:
			LocateThumbForLargeDecrement();
			break;
		case OnNext:
			LocateThumbForLargeIncrement();
			break;
		case OnThumb:
		default:
			LocateThumb(EndScroll, value);
			break;
		}
	}
}

void
ATrack::OnThumbDrag(EventArgs&)
{
	ValueType old_value(value);
	// TODO: get correct old value;
	UpdateValue();
	CheckScroll(ScrollEventSpace::ThumbTrack, old_value);
	Refresh();
}


YHorizontalTrack::YHorizontalTrack(const Rect& r, SDst uMinThumbLength)
	: YComponent(),
	ATrack(r, uMinThumbLength)
{
	YAssert(GetWidth() > GetHeight(),
		"In constructor Components::Controls::\n"
			"YHorizontalTrack::YHorizontalTrack"
		"(const Rect& r, SDst uMinThumbLength) const\": \n"
		"Width is not greater than height.");

	FetchEvent<TouchMove>(Thumb).Add(*this,
		&YHorizontalTrack::OnTouchMove_Thumb_Horizontal);
}

void
YHorizontalTrack::OnTouchMove_Thumb_Horizontal(TouchEventArgs& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		YGUIShell& shl(FetchGUIShell());
		SPos x(shl.LastControlLocation.X + shl.DraggingOffset.X);

		RestrictInClosedInterval(x, 0, GetWidth() - Thumb.GetWidth());
		Thumb.SetLocation(Point(x, Thumb.GetLocation().Y));
		GetThumbDrag()(*this, GetStaticRef<EventArgs>());
	}
}


YVerticalTrack::YVerticalTrack(const Rect& r, SDst uMinThumbLength)
	: YComponent(),
	ATrack(r, uMinThumbLength)
{
	YAssert(GetHeight() > GetWidth(),
		"In constructor Components::Controls::\n"
			"YHorizontalTrack::YHorizontalTrack"
		"(const Rect& r, SDst uMinThumbLength) const\": \n"
		"height is not greater than width.");

	FetchEvent<TouchMove>(Thumb).Add(*this,
		&YVerticalTrack::OnTouchMove_Thumb_Vertical);
}

void
YVerticalTrack::OnTouchMove_Thumb_Vertical(TouchEventArgs& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		YGUIShell& shl(FetchGUIShell());
		SPos y(shl.LastControlLocation.Y + shl.DraggingOffset.Y);

		RestrictInClosedInterval(y, 0, GetHeight() - Thumb.GetHeight());
		Thumb.SetLocation(Point(Thumb.GetLocation().X, y));
		GetThumbDrag()(*this, GetStaticRef<EventArgs>());
	}
}


AScrollBar::AScrollBar(const Rect& r, SDst uMinThumbSize, Orientation o)
try	: AUIBoxControl(r),
	pTrack(o == Horizontal
		? static_cast<ATrack*>(new YHorizontalTrack(
			Rect(r.Height, 0, r.Width - r.Height * 2, r.Height), uMinThumbSize))
		: static_cast<ATrack*>(new YVerticalTrack(
			Rect(0, r.Width, r.Width, r.Height - r.Width * 2), uMinThumbSize))),
	PrevButton(Rect()), NextButton(Rect()), small_delta(2)
{
	pTrack->GetContainerPtr() = this;
	PrevButton.GetContainerPtr() = this;
	NextButton.GetContainerPtr() = this;
	FetchEvent<TouchMove>(PrevButton) += OnTouchMove;
	FetchEvent<TouchDown>(PrevButton).Add(*this,
		&AScrollBar::OnTouchDown_PrevButton);
	FetchEvent<TouchMove>(NextButton) += OnTouchMove;
	FetchEvent<TouchDown>(NextButton).Add(*this,
		&AScrollBar::OnTouchDown_NextButton);

	Size s(GetSize());
	const bool bHorizontal(o == Horizontal);
	const SDst l(SelectFrom(s, !bHorizontal));

	UpdateTo(s, l, bHorizontal);
	PrevButton.SetSize(s);
	NextButton.SetSize(s);
//	PrevButton.SetLocation(Point::Zero);
	Widgets::MoveToBottom(NextButton);
	Widgets::MoveToRight(NextButton);
}
catch(...)
{
	throw LoggedEvent("Error occured @ constructor of AScrollBar;");
}

IControl*
AScrollBar::GetTopControlPtr(const Point& p)
{
	if(Contains(PrevButton, p))
		return &PrevButton;
	if(Contains(NextButton, p))
		return &NextButton;

	YAssert(pTrack.get(),
		"Null widget pointer found @ AScrollBar::GetTopControlPtr;");

	return pTrack.get();
}

void
AScrollBar::Paint()
{
	YAssert(pTrack.get(),
		"Null widget pointer found @ AScrollBar::Draw;");

	YWidgetAssert(this, Controls::YHorizontalScrollBar, Draw);

	Control::Paint();

	const Graphics& g(FetchDirectWindowPtr(*this)->GetContext());
	const Point b(LocateForWindow(*this));

	pTrack->Paint();
	PrevButton.Paint();
	NextButton.Paint();
	WndDrawArrow(g, Rect(LocateForWindow(PrevButton),
		PrevButton.GetSize()), 4, pTrack->GetOrientation() == Horizontal
			? RDeg180 : RDeg90, ForeColor);
	WndDrawArrow(g, Rect(LocateForWindow(NextButton),
		NextButton.GetSize()), 4, pTrack->GetOrientation() == Horizontal
			? RDeg0 : RDeg270, ForeColor);
}

void
AScrollBar::OnTouchDown_PrevButton(TouchEventArgs& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
		GetTrack().LocateThumbForSmallDecrement(small_delta);
}

void
AScrollBar::OnTouchDown_NextButton(TouchEventArgs& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
		GetTrack().LocateThumbForSmallIncrement(small_delta);
}


YHorizontalScrollBar::YHorizontalScrollBar(const Rect& r, SDst uMinThumbLength)
	: YComponent(),
	AScrollBar(r, uMinThumbLength, Horizontal)
{
	YAssert(GetWidth() > GetHeight() * 2,
		"In constructor Components::Controls::\n"
			"YHorizontalScrollBar::YHorizontalScrollBar"
		"(const Rect& r, SDst uMinThumbLength) const\": \n"
		"Width is not greater than twice of height.");
}


YVerticalScrollBar::YVerticalScrollBar(const Rect& r, SDst uMinThumbLength)
	: YComponent(),
	AScrollBar(r, uMinThumbLength, Vertical)
{
	YAssert(GetHeight() > GetWidth() * 2,
		"In constructor Components::Controls::\n"
			"YVerticalScrollBar::YVerticalScrollBar"
		"(const Rect& r, SDst uMinThumbLength) const\": \n"
		"height is not greater than twice of width.");
}


ScrollableContainer::ScrollableContainer(const Rect& r)
	: AUIBoxControl(r),
	HorizontalScrollBar(Rect(Point::Zero, r.Width, defMinScrollBarHeight)),
	VerticalScrollBar(Rect(Point::Zero, defMinScrollBarWidth, r.Height))
{
	HorizontalScrollBar.GetContainerPtr() = this;
	VerticalScrollBar.GetContainerPtr() = this;
	MoveToBottom(HorizontalScrollBar);
	MoveToRight(VerticalScrollBar);
}

IControl*
ScrollableContainer::GetTopControlPtr(const Point& p)
{
	if(ContainsVisible(HorizontalScrollBar, p))
		return &HorizontalScrollBar;
	if(ContainsVisible(VerticalScrollBar, p))
		return &VerticalScrollBar;
	return this;
}

void
ScrollableContainer::Paint()
{
	YWidgetAssert(this, Controls::ScrollableContainer, Draw);

	AUIBoxControl::Paint();
	if(HorizontalScrollBar.IsVisible())
		HorizontalScrollBar.Paint();
	if(VerticalScrollBar.IsVisible())
		VerticalScrollBar.Paint();
}

Size
ScrollableContainer::FixLayout(const Size& s)
{
	Size arena(GetSize());

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
		HorizontalScrollBar.SetVisible(p.first);
		VerticalScrollBar.SetVisible(p.second);
	}
	catch(GeneralEvent&)
	{}
	return arena;
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

