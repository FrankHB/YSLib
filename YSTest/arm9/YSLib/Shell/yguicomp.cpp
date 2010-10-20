// YSLib::Shell::YGUIComponent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-10-04 21:23:32 + 08:00;
// UTime = 2010-10-19 22:37 + 08:00;
// Version = 0.1342;


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
	RectDrawFocus(HWND hWnd, const Point& l, const Size& s)
	{
		YAssert(hWnd, "err: @hWnd is null.");

		DrawWindowBounds(hWnd, Color::Fuchsia);

		IWidget* pWgt(hWnd->GetFocusingPtr());

		if(pWgt != NULL)
			DrawWidgetBounds(*pWgt, Color::Aqua);

	//	const Graphics g(*hWnd);
	}

	void
	RectDrawPressed(const Graphics& g, const Point& l, const Size& s)
	{
		transRect()(g.GetBufferPtr(), g.GetSize(), Rect(l, s), transPixelEx, transSeq());
	}

	void
	RectDrawButtonSurface(const Graphics& g, const Point& l, const Size& s)
	{
		FillRect(g, l, s, Color(48, 216, 255));
		if(s.GetWidth() > 4 && s.GetHeight() > 4)
		{
			Size sz(s.GetWidth() - 4, (s.GetHeight() - 4) >> 1);
			Point sp(l.GetX() + 2, l.GetY() + 2);

			FillRect(g, sp, sz, Color(232, 240, 255));
			FillRect(g, Point(sp.GetX(), static_cast<SPOS>(sp.GetY() + sz.GetHeight())), sz, Color(192, 224, 255));
		}
	}
}


void
YButton::_m_init()
{
	Enter += &YButton::OnEnter;
	Leave += &YButton::OnLeave;
	Click += &YButton::OnClick;
	KeyDown += &YButton::OnKeyDown;
}

void
YButton::DrawForeground()
{
	ParentType::DrawForeground();

	HWND hWnd(GetWindowHandle());

	if(hWnd != NULL)
	{
		const Graphics g(*hWnd);

		RectDrawButtonSurface(g, GetLocationForWindow(), GetSize());
		if(bPressed)
			RectDrawPressed(g, GetLocation(), GetSize());
		if(bFocused)
			RectDrawFocus(hWnd, GetLocation(), GetSize());
		PaintText(*this);
	}
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
YButton::OnKeyDown(const Runtime::MKeyEventArgs&)
{}
void
YButton::OnClick(const Runtime::MTouchEventArgs&)
{}


YHorizontalScrollBar::YHorizontalScrollBar(HWND hWnd, const Rect& r, IWidgetContainer* pCon,
	SDST blMaxThumbSize, SDST blPrev, SDST blNext)
	: AScrollBar(hWnd, r, pCon, blMaxThumbSize, blPrev, blNext)
{

}

void
YHorizontalScrollBar::DrawPrevButton()
{
	Color c(Color::Red);
//	Rect r(GetLocation(), GetPrevButtonSize(), GetHeight());

//	DrawRect(const Graphics(*GetWindowHandle()), r, c);
}
void
YHorizontalScrollBar::DrawNextButton()
{
	Color c(Color::Green);
//	Rect r(GetLocation() + Vec(GetWidth() - GetNextButtonSize(), 0), GetNextButtonSize(), GetHeight());

//	DrawRect(const Graphics(*GetWindowHandle()), r, c);
}
void
YHorizontalScrollBar::DrawScrollArea()
{
	Color c(Color::Blue);
//	Rect r(GetLocation() + Vec(GetPrevButtonSize(), 0), GetScrollAreaSize(), GetHeight());

//	DrawRect(const Graphics(*GetWindowHandle()), r, c);
}


YListBox::YListBox(HWND hWnd, const Rect& r, IWidgetContainer* pCon,
	GHResource<TextRegion> prTr_)
	: YVisualControl(hWnd, r, pCon),
	prTextRegion(pCon ? prTr_ : GetGlobalResource<TextRegion>()), bDisposeList(true),
	Font(), Margin(prTextRegion->Margin),
	List(*new ListType()), Viewer(List)
{
	_m_init();
}
YListBox::YListBox(HWND hWnd, const Rect& r, IWidgetContainer* pCon,
	GHResource<TextRegion> prTr_, YListBox::ListType& List_)
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

	HWND hWnd(GetWindowHandle());

	if(hWnd != NULL)
	{
		if(bFocused)
			RectDrawFocus(hWnd, Location, GetSize());
		if(prTextRegion != NULL && prTextRegion->GetLnHeight() <= GetHeight())
		{
			const SDST lnWidth(GetWidth());
			const SDST lnHeight(GetItemHeight());

			prTextRegion->Font = Font;
			prTextRegion->Font.Update();
			prTextRegion->SetPen();
			prTextRegion->SetSize(lnWidth, lnHeight);
			prTextRegion->SetMargins(defMarginH, defMarginV);
			Viewer.SetLength((GetHeight() + prTextRegion->GetLineGap()) / lnHeight);

			const ViewerType::IndexType last(Viewer.GetIndex() + Viewer.GetValid());
			Point pt(GetLocationForWindow());
			const Graphics g(*GetWindowHandle());

			for(ViewerType::IndexType i(Viewer.GetIndex() >= 0 ? Viewer.GetIndex() : last); i < last; ++i)
			{
				if(Viewer.IsSelected() && i == Viewer.GetSelected())
				{
					prTextRegion->Color = Color::White;
					FillRect<PixelType>(g.GetBufferPtr(), g.GetSize(),
						Rect(pt.X + 1, pt.Y + 1, prTextRegion->GetWidth() - 2, prTextRegion->GetHeight() - 2),
						Color::Aqua);
				}
				else
					prTextRegion->Color = ForeColor;
				prTextRegion->PutLine(List[i]);
				prTextRegion->SetPen();
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
YListBox::OnSelected(const MIndexEventArgs&)
{
	Refresh();
}
void
YListBox::OnConfirmed(const MIndexEventArgs& e)
{
	OnSelected(e);
}


YFileBox::YFileBox(HWND hWnd, const Rect& r, IWidgetContainer* pCon,
	GHResource<TextRegion> prTr_)
	: YListBox(hWnd, r, pCon, prTr_, MFileList::List), MFileList(),
	List(ParentType::List)
{
	TouchMove += &YFileBox::OnTouchMove;
	KeyHeld += &AVisualControl::OnKeyHeld;
}
YFileBox::~YFileBox()
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

