/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yguicomp.cpp
\ingroup Shell
\brief 样式相关图形用户界面组件实现。
\version 0.2620;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-04 21:23:32 + 08:00;
\par 修改时间:
	2010-11-24 11:14 + 08:00;
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
	WndDrawFocus(HWND hWnd, const Size& s)
	{
		YAssert(hWnd, "err: @hWnd is null.");

		DrawWindowBounds(hWnd, ColorSpace::Fuchsia);

		IWidget* pWgt(hWnd->GetFocusingPtr());

		if(pWgt)
			DrawWidgetBounds(*pWgt, ColorSpace::Aqua);
	}

	void
	RectDrawArrow(const Graphics& g, const Point& p, SDST halfSize,
		ROT rot = RDeg0, Color c = ColorSpace::Black)
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
		ROT rot = RDeg0, Color c = ColorSpace::Black)
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
	Enter += &YThumb::OnEnter;
	Leave += &YThumb::OnLeave;
}

void
YThumb::DrawForeground()
{
	YWidgetAssert(this, Controls::YThumb, DrawForeground);

	ParentType::DrawForeground();

	HWND hWnd(FetchDirectWindowHandle(*this));

	RectDrawButton(Graphics(*hWnd), LocateForWindow(*this),
		 GetSize(), bPressed);
	if(bFocused)
		WndDrawFocus(hWnd, GetSize());
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
	GHResource<Drawing::TextRegion> prTr_)
	: YThumb(r, pCon),
	MLabel(f, prTr_)
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
	Thumb(Rect(0, 0, 16, 16), this),
	MinThumbLength(uMinThumbLength)
{
	TouchMove += OnTouchMove;
	TouchDown += &ATrack::OnTouchDown;
}

IWidget*
ATrack::GetTopWidgetPtr(const Point& p)
{
	return GetTopVisualControlPtr(p);
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

	const Graphics g(*FetchDirectWindowHandle(*this));
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

	Thumb.TouchMove.Add(*this, &YHorizontalTrack::OnDrag_Thumb_Horizontal);
}

void
YHorizontalTrack::OnDrag_Thumb_Horizontal(TouchEventArgs& e)
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

	Thumb.TouchMove.Add(*this, &YVerticalTrack::OnDrag_Thumb_Vertical);
}

void
YVerticalTrack::OnDrag_Thumb_Vertical(TouchEventArgs& e)
{
	using namespace InputStatus;

	SPOS y(LastVisualControlLocation.Y + DraggingOffset.Y);

	RestrictInClosedInterval(y, 0, GetHeight() - Thumb.GetHeight());
	Thumb.SetLocation(Point(Thumb.GetLocation().X, y));
	Refresh();
}


AScrollBar::AScrollBar(const Rect& r, IUIBox* pCon, SDST uMinThumbSize,
	Widgets::Orientation o)
try	: AVisualControl(r, pCon),
	MSimpleFocusResponser(),
	pTrack(o == Widgets::Horizontal
		? static_cast<ATrack*>(new YHorizontalTrack(
			Rect(r.Height, 0, r.Width - r.Height * 2, r.Height), this,
			uMinThumbSize))
		: static_cast<ATrack*>(new YVerticalTrack(
			Rect(0, r.Width, r.Height - r.Width * 2, r.Width), this,
			uMinThumbSize))),
	PrevButton(Rect(), this), NextButton(Rect(), this)
{
	Size s(GetSize());
	const bool bHorizontal(o == Widgets::Horizontal);
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

IWidget*
AScrollBar::GetTopWidgetPtr(const Point& p)
{
	return GetTopVisualControlPtr(p);
}
IVisualControl*
AScrollBar::GetTopVisualControlPtr(const Point& p)
{
	if(Contains(PrevButton, p))
		return &PrevButton;
	if(Contains(NextButton, p))
		return &NextButton;
	return pTrack.get();
}

void
AScrollBar::DrawBackground()
{
	YWidgetAssert(this, Controls::YHorizontalScrollBar, DrawBackground);

	pTrack->DrawBackground();
}

void
AScrollBar::DrawForeground()
{
	YWidgetAssert(this, Controls::YHorizontalScrollBar, DrawForeground);

	ParentType::DrawForeground();

	const Graphics g(*FetchDirectWindowHandle(*this));
	const Point b(LocateForWindow(*this));

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
	AScrollBar(r, pCon, uMinThumbLength, Widgets::Horizontal)
{}


YListBox::YListBox(const Rect& r, IUIBox* pCon,
	GHResource<TextRegion> prTr_)
	: YVisualControl(r, pCon),
//	prTextRegion(pCon ? prTr_ : GetGlobalResource<TextRegion>()),
	prTextRegion(GetGlobalResource<TextRegion>()),
	bDisposeList(true),
	Font(), Margin(prTextRegion->Margin),
	List(*new ListType()), Viewer(List)
{
	_m_init();
}
YListBox::YListBox(const Rect& r, IUIBox* pCon,
	GHResource<TextRegion> prTr_, YListBox::ListType& List_)
	: YVisualControl(r, pCon),
//	prTextRegion(pCon ? prTr_ : GetGlobalResource<TextRegion>()),
	prTextRegion(GetGlobalResource<TextRegion>()),
	bDisposeList(false),
	Font(), Margin(prTextRegion->Margin),
	List(List_), Viewer(List)
{
	_m_init();
}
YListBox::~YListBox() ythrow()
{
	if(bDisposeList)
		delete &List;
}

void
YListBox::_m_init()
{
	KeyDown += &YListBox::OnKeyDown;
	KeyHeld += OnKeyHeld;
	TouchDown += &YListBox::OnTouchDown;
	TouchMove += &YListBox::OnTouchMove;
	Click += &YListBox::OnClick;
	Selected += &YListBox::OnSelected;
	Confirmed += &YListBox::OnConfirmed;
}

YListBox::ItemType*
YListBox::GetItemPtr(ViewerType::IndexType i)
{
	return IsInInterval<ViewerType::IndexType>(i, List.size())
		? &List[i] : NULL;
}
SDST
YListBox::GetItemHeight() const
{
	YAssert(prTextRegion,
		"In function \"SDST\n"
		"Components::Controls::YListBox::GetItemHeight()\": \n"
		"The text region pointer is null.");
	return GetLnHeightExFrom(*prTextRegion) + defMarginV * 2;
}

void
YListBox::SetSelected(YListBox::ViewerType::IndexType i)
{
	if(Viewer.Contains(i) && Viewer.SetSelected(Viewer.GetIndex() + i))
		CallSelected();
}
void
YListBox::SetSelected(SPOS x, SPOS y)
{
	SetSelected(CheckPoint(x, y));
}

void
YListBox::DrawBackground()
{
	YWidgetAssert(this, Controls::YListBox, DrawBackground);

	ParentType::DrawBackground();
}

void
YListBox::DrawForeground()
{
	YWidgetAssert(this, Controls::YListBox, DrawForeground);

	ParentType::DrawForeground();

	HWND hWnd(FetchDirectWindowHandle(*this));

	if(hWnd)
	{
		if(bFocused)
			WndDrawFocus(hWnd, GetSize());
		if(prTextRegion && GetLnHeightFrom(*prTextRegion) <= GetHeight())
		{
			const SDST lnWidth(GetWidth());
			const SDST lnHeight(GetItemHeight());

			prTextRegion->Font = Font;
			prTextRegion->Font.Update();
			prTextRegion->ResetPen();
			prTextRegion->SetSize(lnWidth, lnHeight);
			SetMarginsTo(*prTextRegion, defMarginH, defMarginV);
			Viewer.SetLength((GetHeight() + prTextRegion->LineGap)
				/ lnHeight);

			const ViewerType::IndexType last(Viewer.GetIndex()
				+ Viewer.GetValid());
			Point pt(LocateForWindow(*this));
			const Graphics g(*hWnd);

			if(Viewer.GetIndex() >= 0)
				for(ViewerType::IndexType i(Viewer.GetIndex()); i < last; ++i)
				{
					if(Viewer.IsSelected() && i == Viewer.GetSelected())
					{
						prTextRegion->Color = ColorSpace::White;
						FillRect<PixelType>(g.GetBufferPtr(), g.GetSize(),
							Rect(pt.X + 1, pt.Y + 1, prTextRegion->Width - 2,
							prTextRegion->Height - 1),
							ColorSpace::Aqua);
					}
					else
						prTextRegion->Color = ForeColor;
					prTextRegion->PutLine(List[i]);
					prTextRegion->ResetPen();
					prTextRegion->BlitToBuffer(g.GetBufferPtr(), RDeg0,
						g.GetSize(), Point::Zero, pt, *prTextRegion);
					pt.Y += lnHeight;
					prTextRegion->ClearImage();
				}
			prTextRegion->SetSize(0, 0);
		}
	}
}

YListBox::ViewerType::IndexType
YListBox::CheckPoint(SPOS x, SPOS y)
{
	YAssert(prTextRegion,
		"In function \"Components::Controls::YListBox::ViewerType::IndexType\n"
		"Components::Controls::YListBox::CheckClick(const Point& pt)\":\n"
		"The text region pointer is null.");

	return Rect(Point::Zero, GetSize()).Contains(x, y)
		? y / GetItemHeight() : -1;
}

void
YListBox::CallSelected()
{
	IndexEventArgs e(*this, Viewer.GetSelected());

	Selected(*this, e);
}

void
YListBox::CallConfirmed(YListBox::ViewerType::IndexType i)
{
	if(Viewer.IsSelected() && Viewer.GetSelected() + Viewer.GetIndex() == i)
	{
		IndexEventArgs e(*this, i);

		Confirmed(*this, e);
	}
}

void
YListBox::OnKeyDown(KeyEventArgs& k)
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
YListBox::OnTouchDown(TouchEventArgs& e)
{
	SetSelected(e);
}

void
YListBox::OnTouchMove(TouchEventArgs& e)
{
	SetSelected(e);
}

void
YListBox::OnClick(TouchEventArgs& e)
{
	CallConfirmed(CheckPoint(e));
}

void
YListBox::OnSelected(IndexEventArgs& e)
{
	Refresh();
}

void
YListBox::OnConfirmed(IndexEventArgs& e)
{
	OnSelected(e);
}


YFileBox::YFileBox(const Rect& r, IUIBox* pCon,
	GHResource<TextRegion> prTr_)
	: YListBox(r, pCon, prTr_, MFileList::List), MFileList(),
	List(ParentType::List)
{
	Confirmed += &YFileBox::OnConfirmed;
}
YFileBox::~YFileBox() ythrow()
{}

IO::Path
YFileBox::GetPath() const
{
	if(Viewer.IsSelected() && Viewer.GetSelected() >= 0)
		return Directory / (List[Viewer.GetSelected()]);
	return Directory;
}

void
YFileBox::DrawBackground()
{
	YWidgetAssert(this, Controls::YFileBox, DrawBackground);

	YListBox::DrawBackground();
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
	if(Viewer.Contains(e) && static_cast<bool>(*this /= List[e.Index]))
	{
		Viewer.MoveViewerToBegin();
		Viewer.SetSelected(0);
		Refresh();
	}
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

