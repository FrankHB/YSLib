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
\version 0.2206;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-04 21:23:32 + 08:00;
\par 修改时间:
	2010-11-12 15:19 + 08:00;
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
		YAssert(g.IsValid(), "err: @g is not valid.");

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
	WndDrawArrow(HWND hWnd, const Rect& r, SDST halfSize,
		ROT rot = RDeg0, Color c = ColorSpace::Black)
	{
		YAssert(hWnd, "err: @hWnd is null.");

		SPOS x(r.X), y(r.Y);

		switch(rot)
		{
		case RDeg0:
			x += r.Width - 11;
		case RDeg180:
			x += 5;
			y += (r.Height + 1) >> 1;
			break;
		case RDeg90:
			y += r.Height - 11;
		case RDeg270:
			y += 5;
			x += (r.Width + 1) >> 1;
			break;
		}
		RectDrawArrow(Graphics(*hWnd), Point(x, y), halfSize, rot, c);
	}

	void
	RectDrawButton(const Graphics& g, const Point& p, const Size& s,
		bool bPressed = false, Color c = Color(48, 216, 255))
	{
		YAssert(g.IsValid(), "err: @g is not valid.");

		FillRect(g, p, s, c);
		if(s.Width > 4 && s.Height > 4)
		{
			const Size sz(s.Width - 4, (s.Height - 4) >> 1);
			Point sp(p.X + 2, p.Y + 2);

			FillRect(g, sp, sz, Color(232, 240, 255));
			sp.Y += sz.Height;
			FillRect(g, sp, sz, Color(192, 224, 255));
		}
		if(bPressed)
			TransformRect(g, p, s, transPixelEx<56, 24, 32>);
	}
}


YThumb::YThumb(HWND hWnd, const Rect& r, IUIBox* pCon)
	: YVisualControl(hWnd, r, pCon),
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

	HWND hWnd(GetWindowHandle());
	const Point pt(LocateForWindow(*this));

	RectDrawButton(Graphics(*hWnd), pt, GetSize(), bPressed);
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


YButton::YButton(HWND hWnd, const Rect& r, const Drawing::Font& f, IUIBox* pCon,
	GHResource<Drawing::TextRegion> prTr_)
	: YThumb(hWnd, r, pCon),
	MLabel(f, prTr_)
{
	_m_init();
}

void
YButton::_m_init()
{
	Click += &YButton::OnClick;
	KeyDown += &YButton::OnKeyDown;
}

void
YButton::DrawForeground()
{
	YWidgetAssert(this, Controls::YButton, DrawForeground);

	ParentType::DrawForeground();
	PaintText(*this, LocateForWindow(*this));
}

void
YButton::OnKeyDown(KeyEventArgs&)
{}

void
YButton::OnClick(TouchEventArgs&)
{}


ATrack::ATrack(HWND hWnd, const Rect& r, IUIBox* pCon, SDST uMinThumbLength)
	: AVisualControl(hWnd, Rect(r.GetPoint(),
		vmax<SDST>(16, r.Width), vmax<SDST>(16, r.Height)), pCon),
	Thumb(hWnd, Rect(0, 0, 16, 16), this), pFocusing(NULL)
{
	// TODO: 添加断言。
	// Make sure the long side is greater than two times of the short side;

	TouchMove += OnTouchMove;
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

	Drawing::Size s(Thumb.GetSize());

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
ATrack::ClearFocusingPtr()
{
	pFocusing = NULL;
}

bool
ATrack::ResponseFocusRequest(AFocusRequester& w)
{
//	if(&w == &Thumb)
	pFocusing = dynamic_cast<IVisualControl*>(&w);
	return pFocusing;
}

bool
ATrack::ResponseFocusRelease(AFocusRequester& w)
{
	if(pFocusing == dynamic_cast<IVisualControl*>(&w))
	{
		pFocusing = NULL;
	//	w.ReleaseFocusRaw();
		return true;
	}
	return false;
}

void
ATrack::DrawForeground()
{
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
		"In function \"ATrack::Area\n"
		"ATrack::CheckArea(SPOS q) const\": \n"
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
ATrack::OnDrag_Thumb_Horizontal(TouchEventArgs& e)
{
	using namespace InputStatus;

	SPOS x(LastVisualControlLocation.X + DraggingOffset.X);

	RestrictInClosedInterval(x, 0, GetWidth() - Thumb.GetWidth());
	Thumb.SetLocation(Point(x, Thumb.GetLocation().Y));
	Refresh();
}

void
ATrack::OnDrag_Thumb_Vertical(TouchEventArgs& e)
{
	using namespace InputStatus;

	SPOS y(LastVisualControlLocation.Y + DraggingOffset.Y);

	RestrictInClosedInterval(y, 0, GetHeight() - Thumb.GetHeight());
	Thumb.SetLocation(Point(Thumb.GetLocation().X, y));
	Refresh();
}

void
ATrack::OnTouchDown_Horizontal(TouchEventArgs& e)
{
	if(Rect(Point::Zero, GetSize()).Contains(e))
		ResponseTouchDown(e.X);
}

void
ATrack::OnTouchDown_Vertical(TouchEventArgs& e)
{
	if(Rect(Point::Zero, GetSize()).Contains(e))
		ResponseTouchDown(e.Y);
}


YHorizontalTrack::YHorizontalTrack(HWND hWnd, const Rect& r, IUIBox* pCon)
	: YComponent(),
	ATrack(hWnd, r, pCon)
{
	TouchDown += &ATrack::OnTouchDown_Horizontal;
	Thumb.TouchMove.Add(static_cast<ATrack&>(*this),
		&ATrack::OnDrag_Thumb_Horizontal);
}


YVerticalTrack::YVerticalTrack(HWND hWnd, const Rect& r, IUIBox* pCon)
	: YComponent(),
	ATrack(hWnd, r, pCon)
{
	TouchDown += &ATrack::OnTouchDown_Vertical;
	Thumb.TouchMove.Add(static_cast<ATrack&>(*this),
		&ATrack::OnDrag_Thumb_Vertical);
}


AScrollBar::AScrollBar(HWND hWnd, const Rect& r, IUIBox* pCon,
	SDST uMinThumbSize, Widgets::Orientation o)
	: ATrack(hWnd, r, pCon, uMinThumbSize),
	PrevButton(hWnd, Rect(), pCon), NextButton(hWnd, Rect(), pCon)
{
//	PrevButton.SetLocation(Point::Zero);

	Drawing::Size s(GetSize());

	const bool isHorizontal(o == Widgets::Horizontal);
	const SDST l(SelectFrom(s, !isHorizontal));

	UpdateTo(s, isHorizontal, l);
	PrevButton.SetSize(s);
	NextButton.SetSize(s);

	Point p(GetLocation());

	UpdateTo(p, SelectFrom(GetSize(), isHorizontal) - l, isHorizontal);
	NextButton.SetLocation(p);
}

IVisualControl*
AScrollBar::GetTopVisualControlPtr(const Point& p)
{
	if(Contains(PrevButton, p))
		return &PrevButton;
	if(Contains(NextButton, p))
		return &NextButton;

	Point pt(p);

	if(IsHorizontal())
		pt.X -= PrevButton.GetWidth();
	else
		pt.Y -= PrevButton.GetHeight();
	return ParentType::GetTopVisualControlPtr(pt);
}
SDST
AScrollBar::GetTrackLength() const ythrow()
{
	return ParentType::GetTrackLength()
		- SelectFrom(PrevButton.GetSize(), IsHorizontal())
		- SelectFrom(NextButton.GetSize(), IsHorizontal());
}

void
AScrollBar::SetThumbPosition(SDST l)
{
	return ParentType::SetThumbPosition(l
		- SelectFrom(PrevButton.GetSize(), IsHorizontal()));
}

void
AScrollBar::DrawBackground()
{
	YWidgetAssert(this, Controls::YHorizontalScrollBar, DrawBackground);

	if(GetWindowHandle())
	{
		const Graphics g(*GetWindowHandle());

		FillRect(g, GetBounds(), Color(237, 237, 237));

		SPOS xr(GetX() + GetWidth() - 1);

		DrawHLineSeg(g, GetY(), GetX(), xr, Color(227, 227, 227));
		DrawHLineSeg(g, GetY() + GetHeight() - 1, GetX(), xr,
			Color(227, 227, 227));
	}
}

void
AScrollBar::DrawForeground()
{
	YWidgetAssert(this, Controls::YHorizontalScrollBar, DrawForeground);
	const Graphics g(*GetWindowHandle());
	const Point b(LocateForWindow(*this));

	ParentType::DrawForeground();
	PrevButton.DrawForeground();
	NextButton.DrawForeground();
/*	RectDrawButton(g, b,
		Drawing::Size(PrevButtonSize, GetHeight()),	Pressed == PrevButton);
	WndDrawArrow(GetWindowHandle(), GetBounds(), 4, RDeg180, ForeColor);
	RectDrawButton(g, Point(b.X + GetWidth() - NextButtonSize, b.Y),
		Drawing::Size(NextButtonSize, GetHeight()), Pressed == NextButton);
	WndDrawArrow(GetWindowHandle(), GetBounds(), 4, RDeg0, ForeColor);
	RectDrawButton(g, Point(b.X + PrevButtonSize + GetThumbPosition(), b.Y),
		Drawing::Size(GetThumbSize(), GetHeight()), Pressed == Thumb,
		ColorSpace::Gray);*/
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


YHorizontalScrollBar::YHorizontalScrollBar(HWND hWnd, const Rect& r,
	IUIBox* pCon, SDST uMinThumbLength)
	: YComponent(),
	AScrollBar(hWnd, r, pCon, uMinThumbLength, Widgets::Horizontal)
{}


YListBox::YListBox(HWND hWnd, const Rect& r, IUIBox* pCon,
	GHResource<TextRegion> prTr_)
	: YVisualControl(hWnd, r, pCon),
	prTextRegion(pCon ? prTr_ : GetGlobalResource<TextRegion>()),
	bDisposeList(true),
	Font(), Margin(prTextRegion->Margin),
	List(*new ListType()), Viewer(List)
{
	_m_init();
}
YListBox::YListBox(HWND hWnd, const Rect& r, IUIBox* pCon,
	GHResource<TextRegion> prTr_, YListBox::ListType& List_)
	: YVisualControl(hWnd, r, pCon),
	prTextRegion(pCon ? prTr_ : GetGlobalResource<TextRegion>()),
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
		"In function \"SDST\nYListBox::GetItemHeight()\": \n"
		"The text region pointer is null.");
	return GetLnHeightExFrom(*prTextRegion) + (defMarginV << 1);
}

void
YListBox::SetSelected(YListBox::ViewerType::IndexType i)
{
	if(IsInInterval<ViewerType::IndexType>(i, Viewer.GetLength()))
	{
		const ViewerType::IndexType nOld(Viewer.GetSelected());

		Viewer.SetSelected(Viewer.GetIndex() + i);
		if(Viewer.GetSelected() != nOld)
			CallSelected();
	}
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

	HWND hWnd(GetWindowHandle());

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
			SetPensTo(*prTextRegion);
			prTextRegion->SetSize(lnWidth, lnHeight);
			SetMarginsTo(*prTextRegion, defMarginH, defMarginV);
			Viewer.SetLength((GetHeight() + prTextRegion->LineGap)
				/ lnHeight);

			const ViewerType::IndexType last(Viewer.GetIndex()
				+ Viewer.GetValid());
			Point pt(LocateForWindow(*this));
			const Graphics g(*GetWindowHandle());

			for(ViewerType::IndexType i(Viewer.GetIndex() >= 0
				? Viewer.GetIndex() : last); i < last; ++i)
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
				SetPensTo(*prTextRegion);
				prTextRegion->BlitToBuffer(g.GetBufferPtr(), RDeg0, g.GetSize(),
					Point::Zero, pt, *prTextRegion);
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

	return GetBounds().Contains(x, y)
		? (y - GetY()) / (GetLnHeightExFrom(*prTextRegion) + (defMarginV << 1))
		: -1;
}

void
YListBox::CallSelected()
{
	IndexEventArgs e(*this, Viewer.GetSelected());

	Selected(*this, e);
}

void
YListBox::CallConfirmed()
{
	if(Viewer.IsSelected())
	{
		IndexEventArgs e(*this, Viewer.GetSelected());

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
			CallConfirmed();
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
YListBox::OnClick(TouchEventArgs&)
{
	CallConfirmed();
}

void
YListBox::OnSelected(IndexEventArgs&)
{
	Refresh();
}

void
YListBox::OnConfirmed(IndexEventArgs& e)
{
	OnSelected(e);
}


YFileBox::YFileBox(HWND hWnd, const Rect& r, IUIBox* pCon,
	GHResource<TextRegion> prTr_)
	: YListBox(hWnd, r, pCon, prTr_, MFileList::List), MFileList(),
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
	YListBox::DrawForeground();
}

void
YFileBox::OnConfirmed(IndexEventArgs& e)
{
	if(*this /= List[e.Index])
	{
		Viewer.MoveViewerToBegin();
		Viewer.SetSelected(0);
		Refresh();
	}
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

