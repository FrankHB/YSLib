/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yguicomp.cpp
\ingroup Shell
\brief 样式相关图形用户界面组件实现。
\version 0.2965;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-04 21:23:32 + 08:00;
\par 修改时间:
	2011-02-08 21:56 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YGUIComponent;
*/


#include "yguicomp.h"
#include "ywindow.h"
#include "ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

using namespace Runtime;

YSL_BEGIN_NAMESPACE(Widgets)



YSL_END_NAMESPACE(Widgets)

YSL_BEGIN_NAMESPACE(Controls)

namespace
{
	template<u8 r, u8 g, u8 b>
	inline void
	transPixelEx(BitmapPtr dst)
	{
		*dst ^= Color(r, g, b) & ~BITALPHA;
	}

	void
	WndDrawFocus(IWindow* pWnd, const Size&)
	{
		YAssert(pWnd, "Window pointer is null.");

		DrawWindowBounds(pWnd, ColorSpace::Fuchsia);

		IWidget* pWgt(pWnd->GetFocusingPtr());

		if(pWgt)
			DrawWidgetBounds(*pWgt, ColorSpace::Aqua);
	}

	void
	RectDrawArrow(const Graphics& g, const Point& p, SDST halfSize,
		Rotation rot = RDeg0, Color c = Drawing::ColorSpace::Black)
	{
		YAssert(g.IsValid(), "err: graphics context is invalid.");

		SDST x(p.X), y(p.Y);

		switch(rot)
		{
		case RDeg0:
			{
				SDST t(p.Y);

				for(SDST i(0); i < halfSize; ++i)
					DrawVLineSeg(g, x--, y--, t++, c);
			}
			break;

		case RDeg90:
			{
				SDST t(p.X);

				for(SDST i(0); i < halfSize; ++i)
					DrawHLineSeg(g, y++, x--, t++, c);
			}
			break;

		case RDeg180:
			{
				SDST t(p.Y);

				for(SDST i(0); i < halfSize; ++i)
					DrawVLineSeg(g, x++, y--, t++, c);
			}
			break;

		case RDeg270:
			{
				SDST t(p.X);

				for(SDST i(0); i < halfSize; ++i)
					DrawHLineSeg(g, y--, x--, t++, c);
			}
		default:
			break;
		}
	}

	void
	WndDrawArrow(const Graphics& g, const Rect& r, SDST halfSize,
		Rotation rot = RDeg0, Color c = Drawing::ColorSpace::Black)
	{
		SPOS x(r.X), y(r.Y);

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

	void
	RectDrawButton(const Graphics& g, const Point& p, const Size& s,
		bool bPressed = false, Color c = Color(48, 216, 255))
	{
		YAssert(g.IsValid(), "err: @g is invalid.");

		FillRect(g, p, s, c);
		if(s.Width > 4 && s.Height > 4)
		{
			const Size sz(s.Width - 4, (s.Height - 4) / 2);
			Point sp(p.X + 2, p.Y + 2);

			FillRect(g, sp, sz, Color(232, 240, 255));
			sp.Y += sz.Height;
			FillRect(g, sp, sz, Color(192, 224, 255));
		}
		if(bPressed)
			TransformRect(g, p, s, transPixelEx<56, 24, 32>);
	}
}


YThumb::YThumb(const Rect& r, IUIBox* pCon)
	: YVisualControl(r, pCon),
	MButton()
{
	FetchEvent<EControl::Enter>(*this) += &YThumb::OnEnter;
	FetchEvent<EControl::Leave>(*this) += &YThumb::OnLeave;
}

void
YThumb::DrawForeground()
{
	YWidgetAssert(this, Controls::YThumb, DrawForeground);

	ParentType::DrawForeground();

	IWindow* pWnd(FetchDirectWindowPtr(*this));

	RectDrawButton(pWnd->GetContext(), LocateForWindow(*this),
		GetSize(), bPressed);
	if(bFocused)
		WndDrawFocus(pWnd, GetSize());
}

void
YThumb::OnEnter(InputEventArgs&)
{
	bPressed = true;
	Refresh();
}

void
YThumb::OnLeave(InputEventArgs&)
{
	bPressed = false;
	Refresh();
}


YButton::YButton(const Rect& r, IUIBox* pCon, const Drawing::Font& f)
	: YThumb(r, pCon),
	MLabel(f)
{}

void
YButton::DrawForeground()
{
	YWidgetAssert(this, Controls::YButton, DrawForeground);

	ParentType::DrawForeground();
	PaintText(*this, FetchContext(*this), LocateForWindow(*this));
}


ATrack::ATrack(const Rect& r, IUIBox* pCon, SDST uMinThumbLength)
	: AVisualControl(Rect(r.GetPoint(),
		vmax<SDST>(16, r.Width), vmax<SDST>(16, r.Height)), pCon),
	MSimpleFocusResponser(),
	Thumb(Rect(0, 0, 16, 16), this), MinThumbLength(uMinThumbLength)
{
	FetchEvent<EControl::TouchMove>(*this) += OnTouchMove;
	FetchEvent<EControl::TouchDown>(*this) += &ATrack::OnTouchDown;
}

IVisualControl*
ATrack::GetTopVisualControlPtr(const Point& p)
{
	return Contains(Thumb, p) ? &Thumb : NULL;
}

void
ATrack::SetThumbLength(SDST l)
{
	RestrictInClosedInterval(l, MinThumbLength, GetTrackLength());

	Size s(Thumb.GetSize());

	UpdateTo(s, l, IsHorizontal());
	Thumb.SetSize(s);
	Refresh();
}
void
ATrack::SetThumbPosition(SDST l)
{
	const SDST tl(GetThumbLength());
	const SDST wl(GetTrackLength());

	if(l + tl > wl)
		l = wl - tl;

	Point p(Thumb.GetLocation());

	UpdateTo(p, l, IsHorizontal());
	Thumb.SetLocation(p);
	Refresh();
}

void
ATrack::DrawBackground()
{
	YWidgetAssert(this, Controls::ATrack, DrawBackground);

	const Graphics& g(FetchDirectWindowPtr(*this)->GetContext());
	const Point loc(LocateForWindow(*this));

	FillRect(g, loc, GetSize(), Color(237, 237, 237));

	const SPOS xr(loc.X + GetWidth() - 1);
	const SPOS yr(loc.Y + GetHeight() - 1);
	const Color c(227, 227, 227);

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

void
ATrack::DrawForeground()
{
	YWidgetAssert(this, Controls::ATrack, DrawForeground);

	ParentType::DrawForeground();
	Thumb.DrawForeground();
}

ATrack::Area
ATrack::CheckArea(SDST q) const
{
	const Area lst[] = {OnPrev, OnThumb, OnNext};
	const SDST a[] = {0, GetThumbPosition(),
		GetThumbPosition() + GetThumbLength()}; 
	std::size_t n(SwitchInterval(q, a, 3));

	YAssert(n < 3,
		"In function \"Components::Controls::ATrack::Area\n"
		"Components::Controls::ATrack::CheckArea(SPOS q) const\": \n"
		"Array index is out of bound.");

	return lst[n];
}

void
ATrack::ResponseTouchDown(SDST v)
{
	SPOS l(GetThumbPosition());

	switch(CheckArea(v))
	{
	case OnThumb:
		return;

	case OnPrev:
		l -= GetThumbLength();
		break;

	case OnNext:
		l += GetThumbLength();
		break;

	default:
		break;
	}
	if(l < 0)
		l = 0;
	SetThumbPosition(l);
}

void
ATrack::OnTouchDown(TouchEventArgs& e)
{
	if(Rect(Point::Zero, GetSize()).Contains(e))
		ResponseTouchDown(SelectFrom(e, IsHorizontal()));
}


YHorizontalTrack::YHorizontalTrack(const Rect& r, IUIBox* pCon,
	SDST uMinThumbLength)
	: YComponent(),
	ATrack(r, pCon, uMinThumbLength)
{
	YAssert(GetWidth() > GetHeight(),
		"In constructor Components::Controls::\n"
			"YHorizontalTrack::YHorizontalTrack"
		"(const Rect& r, IUIBox* pCon, SDST uMinThumbLength) const\": \n"
		"Width is not greater than height.");

	FetchEvent<EControl::TouchMove>(Thumb).Add(*this,
		&YHorizontalTrack::OnDrag_Thumb_Horizontal);
}

void
YHorizontalTrack::OnDrag_Thumb_Horizontal(TouchEventArgs&)
{
	using namespace InputStatus;

	SPOS x(LastVisualControlLocation.X + DraggingOffset.X);

	RestrictInClosedInterval(x, 0, GetWidth() - Thumb.GetWidth());
	Thumb.SetLocation(Point(x, Thumb.GetLocation().Y));
	Refresh();
}


YVerticalTrack::YVerticalTrack(const Rect& r, IUIBox* pCon,
	SDST uMinThumbLength)
	: YComponent(),
	ATrack(r, pCon, uMinThumbLength)
{
	YAssert(GetHeight() > GetWidth(),
		"In constructor Components::Controls::\n"
			"YHorizontalTrack::YHorizontalTrack"
		"(const Rect& r, IUIBox* pCon, SDST uMinThumbLength) const\": \n"
		"height is not greater than width.");

	FetchEvent<EControl::TouchMove>(Thumb).Add(*this,
		&YVerticalTrack::OnDrag_Thumb_Vertical);
}

void
YVerticalTrack::OnDrag_Thumb_Vertical(TouchEventArgs&)
{
	using namespace InputStatus;

	SPOS y(LastVisualControlLocation.Y + DraggingOffset.Y);

	RestrictInClosedInterval(y, 0, GetHeight() - Thumb.GetHeight());
	Thumb.SetLocation(Point(Thumb.GetLocation().X, y));
	Refresh();
}


AScrollBar::AScrollBar(const Rect& r, IUIBox* pCon, SDST uMinThumbSize,
	Orientation o)
try	: AVisualControl(r, pCon),
	MSimpleFocusResponser(),
	pTrack(o == Horizontal
		? static_cast<ATrack*>(new YHorizontalTrack(
			Rect(r.Height, 0, r.Width - r.Height * 2, r.Height), this,
			uMinThumbSize))
		: static_cast<ATrack*>(new YVerticalTrack(
			Rect(0, r.Width, r.Width, r.Height - r.Width * 2), this,
			uMinThumbSize))),
	PrevButton(Rect(), this), NextButton(Rect(), this)
{
	Size s(GetSize());
	const bool bHorizontal(o == Horizontal);
	const SDST l(SelectFrom(s, !bHorizontal));

	UpdateTo(s, l, bHorizontal);
	PrevButton.SetSize(s);
	NextButton.SetSize(s);
//	PrevButton.SetLocation(Point::Zero);
	Widgets::MoveToBottom(NextButton);
	Widgets::MoveToRight(NextButton);
}
catch(...)
{
	throw LoggedEvent("Error occured @@ constructor of AScrollBar;");
}

IVisualControl*
AScrollBar::GetTopVisualControlPtr(const Point& p)
{
	if(Contains(PrevButton, p))
		return &PrevButton;
	if(Contains(NextButton, p))
		return &NextButton;

	YAssert(pTrack.get(),
		"Null widget pointer found @@ AScrollBar::GetTopVisualControlPtr;");

	return pTrack.get();
}

void
AScrollBar::DrawBackground()
{
	YWidgetAssert(this, Controls::YHorizontalScrollBar, DrawBackground);
	YAssert(pTrack.get(),
		"Null widget pointer found @@ AScrollBar::DrawBackground;");

	pTrack->DrawBackground();
}

void
AScrollBar::DrawForeground()
{
	YWidgetAssert(this, Controls::YHorizontalScrollBar, DrawForeground);

	ParentType::DrawForeground();

	const Graphics& g(FetchDirectWindowPtr(*this)->GetContext());
	const Point b(LocateForWindow(*this));

	YAssert(pTrack.get(),
		"Null widget pointer found @@ AScrollBar::DrawForeground;");

	pTrack->DrawForeground();
	PrevButton.DrawForeground();
	NextButton.DrawForeground();
	WndDrawArrow(g, Rect(PrevButton.GetLocation() + GetLocation(),
		PrevButton.GetSize()), 4, pTrack->GetOrientation() == Horizontal
			? RDeg180 : RDeg90, ForeColor);
	WndDrawArrow(g, Rect(NextButton.GetLocation() + GetLocation(),
		NextButton.GetSize()), 4, pTrack->GetOrientation() == Horizontal
			? RDeg0 : RDeg270, ForeColor);
}


YHorizontalScrollBar::YHorizontalScrollBar(const Rect& r, IUIBox* pCon,
	SDST uMinThumbLength)
	: YComponent(),
	AScrollBar(r, pCon, uMinThumbLength, Horizontal)
{
	YAssert(GetWidth() > GetHeight() * 2,
		"In constructor Components::Controls::\n"
			"YHorizontalScrollBar::YHorizontalScrollBar"
		"(const Rect& r, IUIBox* pCon, SDST uMinThumbLength) const\": \n"
		"Width is not greater than twice of height.");
}


YVerticalScrollBar::YVerticalScrollBar(const Rect& r, IUIBox* pCon,
	SDST uMinThumbLength)
	: YComponent(),
	AScrollBar(r, pCon, uMinThumbLength, Vertical)
{
	YAssert(GetHeight() > GetWidth() * 2,
		"In constructor Components::Controls::\n"
			"YVerticalScrollBar::YVerticalScrollBar"
		"(const Rect& r, IUIBox* pCon, SDST uMinThumbLength) const\": \n"
		"height is not greater than twice of width.");
}


YSimpleListBox::YSimpleListBox(const Rect& r, IUIBox* pCon,
	GHWeak<ListType> wpList_)
	: YVisualControl(r, pCon),
	Font(), Margin(defMarginH, defMarginH, defMarginV, defMarginV),
	wpList(wpList_), Viewer(GetList()), TextState(Font)
{
	FetchEvent<EControl::KeyDown>(*this) += &YSimpleListBox::OnKeyDown;
	FetchEvent<EControl::KeyHeld>(*this) += OnKeyHeld;
	FetchEvent<EControl::TouchDown>(*this) += &YSimpleListBox::OnTouchDown;
	FetchEvent<EControl::TouchMove>(*this) += &YSimpleListBox::OnTouchMove;
	FetchEvent<EControl::Click>(*this) += &YSimpleListBox::OnClick;
	Selected += &YSimpleListBox::OnSelected;
	Confirmed += &YSimpleListBox::OnConfirmed;
}

Drawing::TextState&
YSimpleListBox::GetTextState() ythrow()
{
	TextState.Font.SetFont(Font);
	return TextState;
}
YSimpleListBox::ListType&
YSimpleListBox::GetList() const ythrow()
{
	YAssert(wpList,
		"In function \"Components::Controls::YSimpleListBox::ListType\n"
		"Components::Controls::YSimpleListBox::GetList()\":\n"
		"The list pointer is null.");

	return *wpList;
}
YSimpleListBox::ItemType*
YSimpleListBox::GetItemPtr(ViewerType::IndexType i)
{
	ListType& list(GetList());

	return IsInInterval<ViewerType::IndexType>(i, list.size())
		? &list[i] : NULL;
}
SDST
YSimpleListBox::GetItemHeight()
{
	return GetLnHeightExFrom(GetTextState()) + defMarginV * 2;
}

void
YSimpleListBox::SetSelected(YSimpleListBox::ViewerType::IndexType i)
{
	if(Viewer.Contains(i) && Viewer.SetSelected(i))
		CallSelected();
}
void
YSimpleListBox::SetSelected(SPOS x, SPOS y)
{
	SetSelected(CheckPoint(x, y));
}

void
YSimpleListBox::DrawForeground()
{
	YWidgetAssert(this, Controls::YSimpleListBox, DrawForeground);

	ParentType::DrawForeground();

	IWindow* pWnd(FetchDirectWindowPtr(*this));

	if(pWnd)
	{
		if(bFocused)
			WndDrawFocus(pWnd, GetSize());
		if(GetLnHeightFrom(GetTextState()) <= GetHeight())
		{
			const SDST lnWidth(GetWidth());
			const SDST lnHeight(GetItemHeight());

			Viewer.SetLength((GetHeight() + TextState.LineGap) / lnHeight);
			if(Viewer.GetIndex() >= 0)
			{
				const Padding m(defMarginH, defMarginH, defMarginV, defMarginV);
				const ViewerType::IndexType last(Viewer.GetIndex()
					+ Viewer.GetValid());
				const Graphics& g(pWnd->GetContext());
				Point pt(LocateForWindow(*this));
				ListType& list(GetList());

				for(ViewerType::IndexType i(Viewer.GetIndex()); i < last; ++i)
				{
					if(Viewer.IsSelected() && i == Viewer.GetSelected())
					{
						TextState.Color = Drawing::ColorSpace::White;
						FillRect<PixelType>(g.GetBufferPtr(), g.GetSize(),
							Rect(pt.X + 1, pt.Y + 1, lnWidth - 2, lnHeight - 1),
							ColorSpace::Aqua);
					}
					else
						TextState.Color = ForeColor;
					TextState.ResetForBounds(Rect(pt,lnWidth, lnHeight),
						g.GetSize(), m);
					DrawText(g, TextState, list[i]);
					pt.Y += lnHeight;
				}
			}
		}
	}
}

YSimpleListBox::ViewerType::IndexType
YSimpleListBox::CheckPoint(SPOS x, SPOS y)
{
	return Rect(Point::Zero, GetSize()).Contains(x, y)
		? y / GetItemHeight() + Viewer.GetIndex(): -1;
}

void
YSimpleListBox::CallSelected()
{
	IndexEventArgs e(*this, Viewer.GetSelected());

	Selected(*this, e);
}

void
YSimpleListBox::CallConfirmed(YSimpleListBox::ViewerType::IndexType i)
{
	if(Viewer.IsSelected() && Viewer.GetSelected() == i)
	{
		IndexEventArgs e(*this, i);

		Confirmed(*this, e);
	}
}

void
YSimpleListBox::OnKeyDown(KeyEventArgs& k)
{
	if(Viewer.IsSelected())
	{
		switch(k.GetKey())
		{
		case KeySpace::Enter:
			CallConfirmed(Viewer.GetSelected());
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
				const ViewerType::IndexType nOld(Viewer.GetSelected());

				switch(k.GetKey())
				{
				case KeySpace::Up:
					--Viewer;
					break;

				case KeySpace::Down:
					++Viewer;
					break;

				case KeySpace::PgUp:
					Viewer -= Viewer.GetLength();
					break;

				case KeySpace::PgDn:
					Viewer += Viewer.GetLength();
					break;
				}
				if(Viewer.GetSelected() != nOld)
					CallSelected();
			}
			break;

		default:
			return;
		}
		//	(*this)[es](*this, IndexEventArgs(*this, Viewer.GetSelected()));
	}
}

void
YSimpleListBox::OnTouchDown(TouchEventArgs& e)
{
	SetSelected(e);
}

void
YSimpleListBox::OnTouchMove(TouchEventArgs& e)
{
	SetSelected(e);
}

void
YSimpleListBox::OnClick(TouchEventArgs& e)
{
	CallConfirmed(CheckPoint(e));
}

void
YSimpleListBox::OnSelected(IndexEventArgs&)
{
	Refresh();
}

void
YSimpleListBox::OnConfirmed(IndexEventArgs& e)
{
	OnSelected(e);
}


YListBox::YListBox(const Rect& r, IUIBox* pCon, GHWeak<ListType> wpList_)
	: YVisualControl(r, pCon),
	MSimpleFocusResponser(),
	TextListBox(r, this, wpList_),
	HorizontalScrollBar(Rect(Point::Zero, r.Width, 16), this),
	VerticalScrollBar(Rect(Point::Zero, 16, r.Height), this)
{
	Selected = TextListBox.Selected;
	Confirmed = TextListBox.Confirmed;
	MoveToBottom(HorizontalScrollBar);
	MoveToRight(VerticalScrollBar);
}

IVisualControl*
YListBox::GetTopVisualControlPtr(const Point& p)
{
	if(ContainsVisible(HorizontalScrollBar, p))
		return &HorizontalScrollBar;
	if(ContainsVisible(VerticalScrollBar, p))
		return &VerticalScrollBar;
	return &TextListBox;
}

void
YListBox::DrawForeground()
{
	YWidgetAssert(this, Controls::YSimpleListBox, DrawForeground);

	FixLayout();
	ParentType::DrawForeground();
	if(HorizontalScrollBar.IsVisible())
		HorizontalScrollBar.DrawForeground();
	if(VerticalScrollBar.IsVisible())
		VerticalScrollBar.DrawForeground();
}

void
YListBox::FixLayout()
{
	HorizontalScrollBar.SetVisible(false);
	VerticalScrollBar.SetVisible(false);
}


YFileBox::YFileBox(const Rect& r, IUIBox* pCon)
	: FileList(), YSimpleListBox(r, pCon, GetListWeakPtr())
{
	Confirmed += &YFileBox::OnConfirmed;
}
YFileBox::~YFileBox() ythrow()
{}

IO::Path
YFileBox::GetPath() const
{
	if(Viewer.IsSelected() && Viewer.GetSelected() >= 0)
		return Directory / (GetList()[Viewer.GetSelected()]);
	return Directory;
}

void
YFileBox::DrawBackground()
{
	YWidgetAssert(this, Controls::YFileBox, DrawBackground);

	ParentType::DrawBackground();
}

void
YFileBox::DrawForeground()
{
	YWidgetAssert(this, Controls::YFileBox, DrawForeground);

	ListItems();
	ParentType::DrawForeground();
}

void
YFileBox::OnConfirmed(IndexEventArgs& e)
{
	if(Viewer.Contains(e) && static_cast<bool>(*this /= GetList()[e.Index]))
	{
		Viewer.MoveViewerToBegin();
		Viewer.SetSelected(0);
		Refresh();
	}
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

