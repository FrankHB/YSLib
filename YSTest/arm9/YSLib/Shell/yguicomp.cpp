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
\version 0.2839;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-04 21:23:32 + 08:00;
\par 修改时间:
	2011-01-02 10:20 + 08:00;
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
		Rotation rot = RDeg0, Color c = ColorSpace::Black)
	{
		YAssert(g.IsValid(), "err: @g is invalid.");

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
			break;
		}
	}

	void
	WndDrawArrow(const Graphics& g, const Rect& r, SDST halfSize,
		Rotation rot = RDeg0, Color c = ColorSpace::Black)
	{
		SPOS x(r.X), y(r.Y);

		switch(rot)
		{
		case RDeg0:
			x += r.Width - 11;
		case RDeg180:
			x += 5;
			y += (r.Height + 1) / 2;
			break;
		case RDeg90:
			y += r.Height - 11;
		case RDeg270:
			y += 5;
			x += (r.Width + 1) / 2;
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


YButton::YButton(const Rect& r, IUIBox* pCon, const Drawing::Font& f,
	GHWeak<Drawing::TextRegion> pTr_)
	: YThumb(r, pCon),
	MLabel(f, pTr_)
{}

void
YButton::DrawForeground()
{
	YWidgetAssert(this, Controls::YButton, DrawForeground);

	ParentType::DrawForeground();
	PaintText(*this, LocateForWindow(*this));
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
	YAssert(GetWidth() > GetHeight() * 2,
		"In constructor Components::Controls::\n"
			"YHorizontalTrack::YHorizontalTrack"
		"(HWND hWnd, const Rect& r, IUIBox* pCon) const\": \n"
		"Width is not greater than two times of height.");

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
	YAssert(GetHeight() > GetWidth() * 2,
		"In constructor Components::Controls::\n"
			"YHorizontalTrack::YHorizontalTrack"
		"(HWND hWnd, const Rect& r, IUIBox* pCon) const\": \n"
		"height is not greater than two times of width.");

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
			Rect(0, r.Width, r.Height - r.Width * 2, r.Width), this,
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
	throw LoggedEvent("Error occured @@ AScrollBar::AScrollBar();");
}

IVisualControl*
AScrollBar::GetTopVisualControlPtr(const Point& p)
{
	if(Contains(PrevButton, p))
		return &PrevButton;
	if(Contains(NextButton, p))
		return &NextButton;

	YAssert(pTrack.get(),
		"Invalid widget found @@ AScrollBar::GetTopVisualControlPtr;");

	return pTrack.get();
}

void
AScrollBar::DrawBackground()
{
	YWidgetAssert(this, Controls::YHorizontalScrollBar, DrawBackground);
	YAssert(pTrack.get(),
		"Invalid widget found @@ AScrollBar::DrawBackground;");

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
		"Invalid widget found @@ AScrollBar::DrawForeground;");

	pTrack->DrawForeground();
	PrevButton.DrawForeground();
	NextButton.DrawForeground();
	WndDrawArrow(g, Rect(PrevButton.GetLocation() + GetLocation(),
		PrevButton.GetSize()), 4, RDeg180, ForeColor);
	WndDrawArrow(g, Rect(NextButton.GetLocation() + GetLocation(),
		NextButton.GetSize()), 4, RDeg0, ForeColor);
}

/*SDST
AScrollBar::GetButtonLength(ValueType i, ValueType n) const
{*/
/*
> 可能的错误：
当所用 SDST 和 ValueType 的值需要用不小于 int 的整数类型才能容纳时，
乘法的中间结果时有可能溢出。但这里不考虑这点。
*/
/*	return i < n ? GetScrollAreaFixedSize() * i / n : 0;
}*/


YHorizontalScrollBar::YHorizontalScrollBar(const Rect& r, IUIBox* pCon,
	SDST uMinThumbLength)
	: YComponent(),
	AScrollBar(r, pCon, uMinThumbLength, Horizontal)
{}


YSimpleTextListBox::YSimpleTextListBox(const Rect& r, IUIBox* pCon,
	GHWeak<TextRegion> pTr_, GHWeak<ListType> wpList_)
	: YVisualControl(r, pCon),
	pTextRegion(pTr_ ? pTr_ : GetGlobalResource<TextRegion>()), Font(),
	Margin(pTextRegion->Margin), wpList(wpList_), Viewer(GetList())
{
	FetchEvent<EControl::KeyDown>(*this) += &YSimpleTextListBox::OnKeyDown;
	FetchEvent<EControl::KeyHeld>(*this) += OnKeyHeld;
	FetchEvent<EControl::TouchDown>(*this) += &YSimpleTextListBox::OnTouchDown;
	FetchEvent<EControl::TouchMove>(*this) += &YSimpleTextListBox::OnTouchMove;
	FetchEvent<EControl::Click>(*this) += &YSimpleTextListBox::OnClick;
	Selected += &YSimpleTextListBox::OnSelected;
	Confirmed += &YSimpleTextListBox::OnConfirmed;
}

Drawing::TextRegion&
YSimpleTextListBox::GetTextRegion() const ythrow()
{
	YAssert(pTextRegion,
		"In function \"SDST\n"
		"Components::Controls::YSimpleTextListBox::GetItemHeight()\": \n"
		"The text region pointer is null.");

	return *pTextRegion;
}
YSimpleTextListBox::ListType&
YSimpleTextListBox::GetList() const ythrow()
{
	YAssert(wpList,
		"In function \"Components::Controls::YSimpleTextListBox::ListType\n"
		"Components::Controls::YSimpleTextListBox::GetList()\":\n"
		"The list pointer is null.");

	return *wpList;
}
YSimpleTextListBox::ItemType*
YSimpleTextListBox::GetItemPtr(ViewerType::IndexType i)
{
	ListType& list(GetList());

	return IsInInterval<ViewerType::IndexType>(i, list.size())
		? &list[i] : NULL;
}
SDST
YSimpleTextListBox::GetItemHeight() const
{
	return GetLnHeightExFrom(GetTextRegion()) + defMarginV * 2;
}

void
YSimpleTextListBox::SetSelected(YSimpleTextListBox::ViewerType::IndexType i)
{
	if(Viewer.Contains(i) && Viewer.SetSelected(Viewer.GetIndex() + i))
		CallSelected();
}
void
YSimpleTextListBox::SetSelected(SPOS x, SPOS y)
{
	SetSelected(CheckPoint(x, y));
}

void
YSimpleTextListBox::DrawForeground()
{
	YWidgetAssert(this, Controls::YSimpleTextListBox, DrawForeground);

	ParentType::DrawForeground();

	IWindow* pWnd(FetchDirectWindowPtr(*this));

	if(pWnd)
	{
		if(bFocused)
			WndDrawFocus(pWnd, GetSize());
		if(pTextRegion && GetLnHeightFrom(*pTextRegion) <= GetHeight())
		{
			const SDST lnWidth(GetWidth());
			const SDST lnHeight(GetItemHeight());

			pTextRegion->Font = Font;
			pTextRegion->Font.Update();
			pTextRegion->ResetPen();
			pTextRegion->SetSize(lnWidth, lnHeight);
			SetMarginsTo(*pTextRegion, defMarginH, defMarginV);
			Viewer.SetLength((GetHeight() + pTextRegion->LineGap)
				/ lnHeight);

			const ViewerType::IndexType last(Viewer.GetIndex()
				+ Viewer.GetValid());
			Point pt(LocateForWindow(*this));
			const Graphics& g(pWnd->GetContext());

			if(Viewer.GetIndex() >= 0)
			{
				ListType& list(GetList());

				for(ViewerType::IndexType i(Viewer.GetIndex()); i < last; ++i)
				{
					if(Viewer.IsSelected() && i == Viewer.GetSelected())
					{
						pTextRegion->Color = ColorSpace::White;
						FillRect<PixelType>(g.GetBufferPtr(), g.GetSize(),
							Rect(pt.X + 1, pt.Y + 1,
							pTextRegion->GetWidth() - 2,
							pTextRegion->GetHeight() - 1),
							ColorSpace::Aqua);
					}
					else
						pTextRegion->Color = ForeColor;
					// TEST: begin: blit pollution;
					Drawing::ClearPixel(pTextRegion->GetBufferAlphaPtr(),
						Drawing::GetAreaFrom(pTextRegion->GetSize()));
					// TEST: end: blit pollution;
					pTextRegion->PutLine(list[i]);
					pTextRegion->ResetPen();
					pTextRegion->BlitToBuffer(g.GetBufferPtr(), RDeg0,
						g.GetSize(), Point::Zero, pt, pTextRegion->GetSize());
					pt.Y += lnHeight;
					pTextRegion->ClearImage();
				}
			}
			pTextRegion->SetSize(0, 0);
		}
	}
}

YSimpleTextListBox::ViewerType::IndexType
YSimpleTextListBox::CheckPoint(SPOS x, SPOS y)
{
	return Rect(Point::Zero, GetSize()).Contains(x, y)
		? y / GetItemHeight() : -1;
}

void
YSimpleTextListBox::CallSelected()
{
	IndexEventArgs e(*this, Viewer.GetSelected());

	Selected(*this, e);
}

void
YSimpleTextListBox::CallConfirmed(YSimpleTextListBox::ViewerType::IndexType i)
{
	if(Viewer.IsSelected() && Viewer.GetSelected() == i)
	{
		IndexEventArgs e(*this, i);

		Confirmed(*this, e);
	}
}

void
YSimpleTextListBox::OnKeyDown(KeyEventArgs& k)
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
YSimpleTextListBox::OnTouchDown(TouchEventArgs& e)
{
	SetSelected(e);
}

void
YSimpleTextListBox::OnTouchMove(TouchEventArgs& e)
{
	SetSelected(e);
}

void
YSimpleTextListBox::OnClick(TouchEventArgs& e)
{
	CallConfirmed(CheckPoint(e));
}

void
YSimpleTextListBox::OnSelected(IndexEventArgs&)
{
	Refresh();
}

void
YSimpleTextListBox::OnConfirmed(IndexEventArgs& e)
{
	OnSelected(e);
}


YListBox::YListBox(const Rect& r, IUIBox* pCon,
	GHWeak<TextRegion> pTr_, ListType* wpList_)
	: YVisualControl(r, pCon),
	MSimpleFocusResponser(),
	TextListBox(r, this, pTr_, wpList_), HorizontalScrollBar(r, this)
{
	HorizontalScrollBar.SetVisible(false);
}

IVisualControl*
YListBox::GetTopVisualControlPtr(const Point& p)
{
	if(ContainsVisible(HorizontalScrollBar, p))
		return &HorizontalScrollBar;
	return &TextListBox;
}

void
YListBox::DrawForeground()
{
	YWidgetAssert(this, Controls::YSimpleTextListBox, DrawForeground);

	ParentType::DrawForeground();
	HorizontalScrollBar.DrawForeground();
	if(HorizontalScrollBar.IsVisible())
		HorizontalScrollBar.DrawForeground();
}


YFileBox::YFileBox(const Rect& r, IUIBox* pCon,
	GHWeak<TextRegion> pTr_)
	: FileList(), YSimpleTextListBox(r, pCon, pTr_, GetListWeakPtr())
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

