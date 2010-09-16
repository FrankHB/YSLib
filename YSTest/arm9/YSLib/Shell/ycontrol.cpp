// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-2-18 13:44:34;
// UTime = 2010-9-16 23:31;
// Version = 0.3088;


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


//static const u8 SingleColor_Threshold = 31;

/*
inline static PixelType
mpshl(PixelType c, u8 n, PixelType mask)
{
	return ((c & mask) << n) & mask;
}
inline static PixelType
mpshlR(PixelType c, u8 n)
{
	return mpshl(c, n, 0x7C00);
}
inline static PixelType
mpshlG(PixelType c, u8 n)
{
	return mpshl(c, n, 0x03E0);
}
inline static PixelType
mpshlB(PixelType c, u8 n)
{
	return mpshl(c, n, 0x001F);
}
inline static PixelType
mpshr(PixelType c, u8 n, PixelType mask)
{
	return ((c & mask) >> n) & mask;
}
inline static PixelType
mpshrR(PixelType c, u8 n)
{
	return mpshr(c, n, 0x7C00);
}
inline static PixelType
mpshrG(PixelType c, u8 n)
{
	return mpshr(c, n, 0x03E0);
}
inline static PixelType
mpshrB(PixelType c, u8 n)
{
	return mpshr(c, n, 0x001F);
}


//template<typename _pixelType>
struct transPixelShiftLeft
{
	u8 nShift;

	inline explicit
	transPixelShiftLeft(u8 n)
	: nShift(n)
	{}

	inline void
	operator()(BitmapPtr dst)
	{
		PixelType c = *dst;

		*dst = (c & BITALPHA)
			| mpshlR(c, nShift)
			| mpshlG(c, nShift)
			| mpshlB(c, nShift);
	}
};

struct transPixelShiftRight
{
	u8 nShift;

	inline explicit
	transPixelShiftRight(u8 n)
	: nShift(n)
	{}

	inline void
	operator()(BitmapPtr dst)
	{
		PixelType c = *dst;

		*dst = (c & BITALPHA)
			| mpshrR(c, nShift)
			| mpshrG(c, nShift)
			| mpshrB(c, nShift);
	}
};
*/

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
/*
static void
RectDrawFocusX(const SPoint& l, const SSize& s, HWND hWnd)
{
	YAssert(hWnd, "err: @hWnd is null.");

	BitmapPtr const dst(hWnd->GetBufferPtr());
	const SDST dw(hWnd->GetWidth()),
		dh(hWnd->GetHeight());
	SDST sw(s.Width),
		sh(s.Height);
	SPOS sx(l.X),
		sy(l.Y);

	FillRect(dst, dw, dh, sx, sy, sw, sh, PixelType(ARGB16(1, 6, 27, 31)));
	if(sw < 5 || sh < 5)
		return;
	sw -= 4;
	sh = (sh - 4) >> 1;
	sx += 2;
	sy += 2;
	FillRect(dst, dw, dh, sx, sy, sw, sh, PixelType(ARGB16(1, 29, 30, 31)));
	sy += sh;
	FillRect(dst, dw, dh, sx, sy, sw, sh, PixelType(ARGB16(1, 24, 28, 31)));
}
*/
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
	EventMap[EControl::GotFocus] += OnGotFocus;
	EventMap[EControl::LostFocus] += OnLostFocus;
	TouchDown += OnTouchDown;
	TouchMove += OnTouchMove;
}

GMFocusResponser<IVisualControl>*
MVisualControl::CheckFocusContainer(IWidgetContainer* pCon)
{
	return pCon ? dynamic_cast<GMFocusResponser<IVisualControl>*>(pCon) : NULL;
}

void
MVisualControl::_m_OnTouchHeld(const Runtime::MTouchEventArgs& e)
{
	try
	{
		IWidget& w(dynamic_cast<IWidget&>(*this));
		if(!STouchStatus::IsOnDragging())
			STouchStatus::SetDragOffset(w.GetLocation() - e);
		else if(w.GetLocation() != e + STouchStatus::GetDragOffset())
			OnTouchMove(dynamic_cast<IVisualControl&>(*this), e);
	}
	catch(std::bad_cast&)
	{}
}
void
MVisualControl::_m_OnTouchMove(const Runtime::MTouchEventArgs& e)
{
	try
	{
		IWidget& w(dynamic_cast<IWidget&>(*this));

		w.SetLocation(e + STouchStatus::GetDragOffset());
		w.Refresh();
	}
	catch(std::bad_cast&)
	{}
}

void
MVisualControl::OnGotFocus(IControl& c, const MEventArgs& e)
{
	try
	{
		dynamic_cast<IWidget&>(c).Refresh();
	}
//	catch(std::bad_cast)
//	{}
	catch(std::bad_cast&)
	{
	//	throw;
	}
}
void
MVisualControl::OnLostFocus(IControl& c, const MEventArgs& e)
{
	OnGotFocus(c, e);
/*	try
	{
		dynamic_cast<IWidget&>(c).Refresh();
	}
	catch(std::bad_cast&)
	{}*/
}
void
MVisualControl::OnTouchDown(IVisualControl& c, const MTouchEventArgs& e)
{
	c.RequestFocus(e);
}
void
MVisualControl::OnTouchHeld(IVisualControl& c, const MTouchEventArgs& e)
{
	try
	{
		dynamic_cast<MVisualControl&>(c)._m_OnTouchHeld(e);
	}
	catch(std::bad_cast&)
	{}
}
void
MVisualControl::OnTouchMove(IVisualControl& c, const MTouchEventArgs& e)
{
	try
	{
		dynamic_cast<MVisualControl&>(c)._m_OnTouchMove(e);
	}
	catch(std::bad_cast&)
	{}
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
	Init_();
}
YListBox::YListBox(HWND hWnd, const SRect& r, IWidgetContainer* pCon, GHResource<TextRegion> prTr_, YListBox::ListType& List_)
: YVisualControl(hWnd, r, pCon),
prTextRegion(pCon ? prTr_ : GetGlobalResource<TextRegion>()), bDisposeList(false),
Font(), Margin(prTextRegion->Margin),
List(List_), Viewer(List)
{
	Init_();
}
YListBox::~YListBox()
{
	if(bDisposeList)
		delete &List;
}

void
YListBox::Init_()
{
	Click += OnClick;
	KeyPress += OnKeyPress;
	Selected += OnSelected;
	Confirmed += OnConfirmed;
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
		Viewer.SetSelected(Viewer.GetIndex() + i);
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
YListBox::_m_OnClick(const MTouchEventArgs& pt)
{
	SetSelected(pt);
	Refresh();
	CallConfirmed();
}
void
YListBox::_m_OnKeyPress(const MKeyEventArgs& k)
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
				const ViewerType::IndexType i(Viewer.GetSelected());

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
				if(Viewer.GetSelected() != i)
					CallSelected();
			}
			break;

		default:
			return;
		}
	//	(*this)[es](*this, MIndexEventArgs(*this, Viewer.GetSelected()));
		Refresh();
	}
}

void
YListBox::OnClick(IVisualControl& c, const MTouchEventArgs& e)
{
	try
	{
		dynamic_cast<YListBox&>(c)._m_OnClick(e);
	}
	catch(std::bad_cast&)
	{}
}
void
YListBox::OnKeyPress(IVisualControl& c, const MKeyEventArgs& e)
{
	try
	{
		dynamic_cast<YListBox&>(c)._m_OnKeyPress(e);
	}
	catch(std::bad_cast&)
	{}
}
void
YListBox::OnSelected(IVisualControl& c, const MIndexEventArgs&)
{
	try
	{
		dynamic_cast<YListBox&>(c).Refresh();
	}
	catch(std::bad_cast&)
	{}
}
void
YListBox::OnConfirmed(IVisualControl& c, const MIndexEventArgs& e)
{
	OnSelected(c, e);
}


YFileBox::YFileBox(HWND hWnd, const SRect& r, IWidgetContainer* pCon, GHResource<TextRegion> prTr_)
: YListBox(hWnd, r, pCon, prTr_, MFileList::List), MFileList(),
List(ParentType::List)
{}
YFileBox::~YFileBox()
{
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
YFileBox::OnClick(IVisualControl& c, const MTouchEventArgs& e)
{
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

