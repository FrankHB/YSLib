// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-18 13:44:34 + 08:00;
// UTime = 2010-09-26 20:08 + 08:00;
// Version = 0.3274;


#include "ycontrol.h"
#include "ywindow.h"
#include "ygui.h"
#include "../Core/yexcept.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)
YSL_BEGIN_NAMESPACE(Controls)

using namespace Drawing;
using namespace Runtime;
using namespace Widgets;

inline static void
transPixelEx(BitmapPtr dst)
{
	*dst ^= RGB15(7, 3, 4);
}

static void
RectDrawFocusDefault(const SPoint& l, const SSize& s, HWND hWnd)
{
	YAssert(hWnd, "err: @hWnd is null.");

	DrawWindowBounds(hWnd, ARGB16(1, 1, 1, 31));

	transRect()(hWnd->GetBufferPtr(), hWnd->GetSize(), SRect(l, s), transPixelEx, transSeq());

	IWidget* pWgt(dynamic_cast<IWidget*>(hWnd->GetFocusingPtr()));

	if(pWgt)
		DrawWidgetBounds(*pWgt, ARGB16(1, 31, 1, 31));

//	GraphicInterfaceContext g(hWnd->GetBufferPtr(), hWnd->GetBounds());
}

inline static void
RectOnGotFocus(const SPoint& l, const SSize& s, HWND hWnd)
{
//	RectDrawFocusX(l, s, hWnd);
	RectDrawFocusDefault(l, s, hWnd);
}


MVisualControl::MVisualControl(Drawing::PixelType b, Drawing::PixelType f)
: MControl(), AFocusRequester(),
BackColor(b), ForeColor(f)
{
	EventMap[EControl::GotFocus] += &MVisualControl::OnGotFocus;
	EventMap[EControl::LostFocus] += &MVisualControl::OnLostFocus;
	TouchDown += &MVisualControl::OnTouchDown;
}

GMFocusResponser<IVisualControl>*
MVisualControl::CheckFocusContainer(IWidgetContainer* pCon)
{
	return pCon ? dynamic_cast<GMFocusResponser<IVisualControl>*>(pCon) : NULL;
}

void
MVisualControl::OnGotFocus(const MEventArgs&)
{
	try
	{
		dynamic_cast<IWidget&>(*this).Refresh();
	}
	catch(std::bad_cast&)
	{
	//	throw;
	}
}
void
MVisualControl::OnLostFocus(const MEventArgs& e)
{
	OnGotFocus(e);
}
void
MVisualControl::OnTouchDown(const MTouchEventArgs& e)
{
	try
	{
		dynamic_cast<IVisualControl&>(*this).RequestFocus(e);
	}
	catch(std::bad_cast&)
	{}
}
void
MVisualControl::OnTouchHeld(const Runtime::MTouchEventArgs& e)
{
	try
	{
		IWidget& w(dynamic_cast<IWidget&>(*this));

		if(!SInputStatus::IsOnDragging())
			SInputStatus::SetDragOffset(w.GetLocation() - e);
		else if(w.GetLocation() != e + SInputStatus::GetDragOffset())
			OnTouchMove(e);
	}
	catch(std::bad_cast&)
	{}
}
void
MVisualControl::OnTouchMove(const Runtime::MTouchEventArgs& e)
{
	try
	{
		IWidget& w(dynamic_cast<IWidget&>(*this));

		w.SetLocation(e + SInputStatus::GetDragOffset());
		w.Refresh();
	}
	catch(std::bad_cast&)
	{}
}
void
MVisualControl::OnKeyHeld(const Runtime::MKeyEventArgs& e)
{
	SInputStatus::RepeatKeyHeld(*this, e);
}


YVisualControl::YVisualControl(HWND hWnd, const SRect& r, IWidgetContainer* pCon)
: YComponent(), MWidget(hWnd, r, pCon), MVisualControl(Color::White, Color::Black)
{
	if(pContainer != NULL)
	{
		*pContainer += static_cast<IWidget&>(*this);
		*pContainer += static_cast<IVisualControl&>(*this);
	}
}
YVisualControl::~YVisualControl()
{
	if(pContainer != NULL)
	{
		*pContainer -= static_cast<IVisualControl&>(*this);
		*pContainer -= static_cast<IWidget&>(*this);
	}
}

void
YVisualControl::DrawBackground()
{
	if(!Transparent)
		FillRect(hWindow->GetBufferPtr(), hWindow->GetSize(), GetBounds(), BackColor);
}
void
YVisualControl::DrawForeground()
{
	if(!Transparent)
		SetBgRedrawed(false);
}

void
YVisualControl::RequestFocus(const MEventArgs& e)
{
	GMFocusResponser<IVisualControl>* const p(CheckFocusContainer(pContainer));

	if(p && AFocusRequester::RequestFocus(*p))
		EventMap[EControl::GotFocus](*this, e);
}
void
YVisualControl::ReleaseFocus(const MEventArgs& e)
{
	GMFocusResponser<IVisualControl>* const p(CheckFocusContainer(pContainer));

	if(p && AFocusRequester::ReleaseFocus(*p))
		EventMap[EControl::LostFocus](*this, e);
}


YLabel::~YLabel()
{
}

void
YLabel::DrawForeground()
{
	YVisualControl::DrawForeground();
	if(bFocused)
		RectOnGotFocus(Location, Size, hWindow);
	if(prTextRegion)
	{
		prTextRegion->Font = Font;
		prTextRegion->Font.Update();
		prTextRegion->SetPen();
		prTextRegion->Color = ForeColor;
		prTextRegion->SetSize(GetWidth(), GetHeight());
		prTextRegion->SetMargins(2, 2, 2, 2);
		prTextRegion->PutLine(Text);

		SPoint pt(GetLocationForWindow());

		prTextRegion->BlitToBuffer(hWindow->GetBufferPtr(), RDeg0,
			hWindow->GetSize(), SPoint::Zero, pt, GetSize());
		prTextRegion->SetSize(0, 0);
	}
}


YListBox::YListBox(HWND hWnd, const SRect& r, IWidgetContainer* pCon, GHResource<TextRegion> prTr_)
: YVisualControl(hWnd, r, pCon),
prTextRegion(pCon ? prTr_ : GetGlobalResource<TextRegion>()), bDisposeList(true),
Font(), Margin(prTextRegion->Margin),
List(*new ListType()), Viewer(List)
{
	_m_init();
}
YListBox::YListBox(HWND hWnd, const SRect& r, IWidgetContainer* pCon, GHResource<TextRegion> prTr_, YListBox::ListType& List_)
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
	YVisualControl::DrawBackground();
}
void
YListBox::DrawForeground()
{
	ParentType::DrawForeground();
	if(bFocused)
		RectOnGotFocus(Location, Size, hWindow);
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
		SPoint pt(GetLocationForWindow());

		for(ViewerType::IndexType i(Viewer.GetIndex() >= 0 ? Viewer.GetIndex() : last); i < last; ++i)
		{
			if(Viewer.IsSelected() && i == Viewer.GetSelected())
			{
				prTextRegion->Color = Color::White;
				FillRect(hWindow->GetBufferPtr(), hWindow->GetSize(),
					SRect(pt.X + 1, pt.Y + 1, prTextRegion->GetWidth() - 2, prTextRegion->GetHeight() - 2),
					PixelType(ARGB16(1, 6, 27, 31)));
			}
			else
				prTextRegion->Color = ForeColor;
			prTextRegion->PutLine(List[i]);
			prTextRegion->SetPen();
			prTextRegion->BlitToBuffer(hWindow->GetBufferPtr(), RDeg0,
				hWindow->GetSize(), SPoint::Zero, pt, *prTextRegion);
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
		"Components::Controls::YListBox::CheckClick(const SPoint& pt)\":\n"
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


YFileBox::YFileBox(HWND hWnd, const SRect& r, IWidgetContainer* pCon, GHResource<TextRegion> prTr_)
: YListBox(hWnd, r, pCon, prTr_, MFileList::List), MFileList(),
List(ParentType::List)
{
	TouchHeld += &MVisualControl::OnTouchHeld;
	KeyHeld += &MVisualControl::OnKeyHeld;
}
YFileBox::~YFileBox()
{
}

IO::Path
YFileBox::GetPath() const
{
	if(Viewer.IsSelected() && Viewer.GetSelected() >= 0)
		return Directory / Text::StringToMBCS(List[Viewer.GetSelected()]);
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

