// YSLib::Shell::YGUIComponent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-10-04 21:23:32 + 08:00;
// UTime = 2010-10-08 00:37 + 08:00;
// Version = 0.1148;


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
	inline void
	transPixelEx(BitmapPtr dst)
	{
		*dst ^= Color(56, 24, 32) & ~BITALPHA;
	}

	void
	RectDrawFocus(const Point& l, const Size& s, HWND hWnd)
	{
		YAssert(hWnd, "err: @hWnd is null.");

		DrawWindowBounds(hWnd, Color::Fuchsia);

		IWidget* pWgt(dynamic_cast<IWidget*>(hWnd->GetFocusingPtr()));

		if(pWgt)
			DrawWidgetBounds(*pWgt, Color::Aqua);

	//	GraphicInterfaceContext g(hWnd->GetBufferPtr(), hWnd->GetBounds());
	}

	void
	RectDrawPressed(const Point& l, const Size& s, HWND hWnd)
	{
		YAssert(hWnd, "err: @hWnd is null.");

		transRect()(hWnd->GetBufferPtr(), hWnd->GetSize(), Rect(l, s), transPixelEx, transSeq());
	}

	void
	RectDrawButtonSurface(const Point& l, const Size& s, HWND hWnd)
	{
		YAssert(hWnd, "err: @hWnd is null.");

		BitmapPtr const dst(hWnd->GetBufferPtr());
		const SDST dw(hWnd->GetSize().Width),
			dh(hWnd->GetSize().Height);
		SDST sw(s.Width),
			sh(s.Height);
		SPOS sx(l.X),
			sy(l.Y);

		FillRect<PixelType>(dst, dw, dh, sx, sy, sw, sh, Color(48, 216, 255));
		if(sw < 5 || sh < 5)
			return;
		sw -= 4;
		sh = (sh - 4) >> 1;
		sx += 2;
		sy += 2;
		FillRect<PixelType>(dst, dw, dh, sx, sy, sw, sh, Color(232, 240, 255));
		sy += sh;
		FillRect<PixelType>(dst, dw, dh, sx, sy, sw, sh, Color(192, 224, 255));
	}
}


void
YButton::_m_init()
{
	Enter += &YButton::OnEnter;
	Leave += &YButton::OnLeave;
	Click += &YButton::OnClick;
	KeyDown += &YButton::OnKeyDown;
	Confirmed += &YListBox::OnConfirmed;
}

void
YButton::DrawForeground()
{
	ParentType::DrawForeground();
	RectDrawButtonSurface(GetLocation(), GetSize(), GetWindowHandle());
	if(bPressed)
		RectDrawPressed(Location, Size, hWindow);
	if(bFocused)
		RectDrawFocus(Location, Size, hWindow);
	PaintText(*this);
}

void
YButton::OnEnter(const Runtime::MInputEventArgs&)
{
	bPressed = true;
	Refresh();
}
void
YButton::OnLeave(const Runtime::MInputEventArgs&)
{
	bPressed = false;
	Refresh();
}
void
YButton::OnClick(const Runtime::MTouchEventArgs&)
{
}
void
YButton::OnKeyDown(const Runtime::MKeyEventArgs&)
{
}
void
YButton::OnConfirmed(const MIndexEventArgs&)
{
}


YListBox::YListBox(HWND hWnd, const Rect& r, IWidgetContainer* pCon, GHResource<TextRegion> prTr_)
	: YVisualControl(hWnd, r, pCon),
	prTextRegion(pCon ? prTr_ : GetGlobalResource<TextRegion>()), bDisposeList(true),
	Font(), Margin(prTextRegion->Margin),
	List(*new ListType()), Viewer(List)
{
	_m_init();
}
YListBox::YListBox(HWND hWnd, const Rect& r, IWidgetContainer* pCon, GHResource<TextRegion> prTr_, YListBox::ListType& List_)
	: YVisualControl(hWnd, r, pCon),
	prTextRegion(pCon ? prTr_ : GetGlobalResource<TextRegion>()), bDisposeList(false),
	Font(), Margin(prTextRegion->Margin),
	List(List_), Viewer(List)
{
	_m_init();
}
YListBox::~YListBox()
{
	if(bDisposeList)
		delete &List;
}

void
YListBox::_m_init()
{
	Click += &YListBox::OnClick;
	KeyDown += &YListBox::OnKeyDown;
	Selected += &YListBox::OnSelected;
	Confirmed += &YListBox::OnConfirmed;
}

YListBox::ItemType*
YListBox::GetItemPtr(ViewerType::IndexType i)
{
	return isInIntervalRegular<ViewerType::IndexType>(i, List.size()) ? &List[i] : NULL;
}
SDST
YListBox::GetItemHeight() const
{
	return prTextRegion->GetLnHeightEx() + (defMarginV << 1);
}

void
	YListBox::SetSelected(YListBox::ViewerType::IndexType i)
{
	if(isInIntervalRegular<ViewerType::IndexType>(i, Viewer.GetLength()))
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
	ParentType::DrawBackground();
}
void
YListBox::DrawForeground()
{
	ParentType::DrawForeground();
	if(bFocused)
		RectDrawFocus(Location, Size, hWindow);
	if(prTextRegion && prTextRegion->GetLnHeight() <= Size.Height)
	{
		const SDST lnWidth(GetWidth());
		const SDST lnHeight(GetItemHeight());

		prTextRegion->Font = Font;
		prTextRegion->Font.Update();
		prTextRegion->SetPen();
		prTextRegion->SetSize(lnWidth, lnHeight);
		prTextRegion->SetMargins(defMarginH, defMarginV);
		Viewer.SetLength((Size.Height + prTextRegion->GetLineGap()) / lnHeight);

		const ViewerType::IndexType last(Viewer.GetIndex() + Viewer.GetValid());
		Point pt(GetLocationForWindow());

		for(ViewerType::IndexType i(Viewer.GetIndex() >= 0 ? Viewer.GetIndex() : last); i < last; ++i)
		{
			if(Viewer.IsSelected() && i == Viewer.GetSelected())
			{
				prTextRegion->Color = Color::White;
				FillRect<PixelType>(hWindow->GetBufferPtr(), hWindow->GetSize(),
					Rect(pt.X + 1, pt.Y + 1, prTextRegion->GetWidth() - 2, prTextRegion->GetHeight() - 2),
					Color::Aqua);
			}
			else
				prTextRegion->Color = ForeColor;
			prTextRegion->PutLine(List[i]);
			prTextRegion->SetPen();
			prTextRegion->BlitToBuffer(hWindow->GetBufferPtr(), RDeg0,
				hWindow->GetSize(), Point::Zero, pt, *prTextRegion);
			pt.Y += lnHeight;
			prTextRegion->ClearImage();
		}
		prTextRegion->SetSize(0, 0);
	}
}

YListBox::ViewerType::IndexType
YListBox::CheckPoint(SPOS x, SPOS y)
{
	YAssert(prTextRegion != NULL,
		"In function \"Components::Controls::YListBox::ViewerType::IndexType\n"
		"Components::Controls::YListBox::CheckClick(const Point& pt)\":\n"
		"The text region pointer is null.");

	return GetBounds().IsInBoundsRegular(x, y)
		? (y - GetY()) / (prTextRegion->GetLnHeightEx() + (defMarginV << 1))
		: -1;
}

void
YListBox::CallSelected()
{
	Selected(*this, MIndexEventArgs(*this, Viewer.GetSelected()));
}
void
YListBox::CallConfirmed()
{
	if(Viewer.IsSelected())
		Confirmed(*this, MIndexEventArgs(*this, Viewer.GetSelected()));
}

void
YListBox::OnTouchDown(const MTouchEventArgs& e)
{
	ParentType::OnTouchDown(e);
	SetSelected(e);
}
void
YListBox::OnClick(const MTouchEventArgs&)
{
	CallConfirmed();
}
void
YListBox::OnKeyDown(const MKeyEventArgs& k)
{
	if(Viewer.IsSelected())
	{
		switch(k)
		{
		case Key::Enter:
			CallConfirmed();
			break;

		case Key::ESC:
			ClearSelected();
			CallSelected();
			break;

		case Key::Up:
		case Key::Down:
		case Key::PgUp:
		case Key::PgDn:
			{
				const ViewerType::IndexType nOld(Viewer.GetSelected());

				switch(k)
				{
				case Key::Up:
					--Viewer;
					break;

				case Key::Down:
					++Viewer;
					break;

				case Key::PgUp:
					Viewer -= Viewer.GetLength();
					break;

				case Key::PgDn:
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
		//	(*this)[es](*this, MIndexEventArgs(*this, Viewer.GetSelected()));
	}
}
void
YListBox::OnSelected(const MIndexEventArgs&)
{
	Refresh();
}
void
YListBox::OnConfirmed(const MIndexEventArgs& e)
{
	OnSelected(e);
}


YFileBox::YFileBox(HWND hWnd, const Rect& r, IWidgetContainer* pCon, GHResource<TextRegion> prTr_)
	: YListBox(hWnd, r, pCon, prTr_, MFileList::List), MFileList(),
	List(ParentType::List)
{
	TouchMove += &YFileBox::OnTouchMove;
	KeyHeld += &MVisualControl::OnKeyHeld;
}
YFileBox::~YFileBox()
{
}

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
	YListBox::DrawBackground();
}
void
YFileBox::DrawForeground()
{
	ListItems();
	YListBox::DrawForeground();
}

void
YFileBox::OnTouchMove(const Runtime::MTouchEventArgs& e)
{
	SetSelected(e);
}
void
YFileBox::OnConfirmed(const MIndexEventArgs& e)
{
	ParentType::OnConfirmed(e);
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

