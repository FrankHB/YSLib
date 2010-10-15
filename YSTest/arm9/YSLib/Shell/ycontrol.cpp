// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-18 13:44:34 + 08:00;
// UTime = 2010-10-15 17:01 + 08:00;
// Version = 0.3588;


#include "ycontrol.h"
#include "ywindow.h"
#include "ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

using namespace Runtime;

MVisualControl::MVisualControl()
	: MControl(), AFocusRequester()
{
	EventMap[EControl::GotFocus] += &MVisualControl::OnGotFocus;
	EventMap[EControl::LostFocus] += &MVisualControl::OnLostFocus;
	TouchDown += &MVisualControl::OnTouchDown;
	TouchHeld += &MVisualControl::OnTouchHeld;
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
MVisualControl::OnKeyHeld(const Runtime::MKeyEventArgs& e)
{
	using namespace InputStatus;

	if(RepeatHeld(KeyHeldState, e, 240, 120))
	{
		try
		{
			KeyDown(dynamic_cast<IVisualControl&>(*this), e);
		}
		catch(std::bad_cast&)
		{}
	}
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
		using namespace InputStatus;
		IWidget& w(dynamic_cast<IWidget&>(*this));
		bool b(RepeatHeld(TouchHeldState, e, 0, 0));

		if(!IsOnDragging())
			SetDragOffset(w.GetLocation() - e);
		else if(b && w.GetLocation() != e + GetDragOffset())
			TouchMove(dynamic_cast<IVisualControl&>(*this), e);
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

		w.SetLocation(e + InputStatus::GetDragOffset());
		w.Refresh();
	}
	catch(std::bad_cast&)
	{}
}


AVisualControl::AVisualControl(HWND hWnd, const Rect& r, IWidgetContainer* pCon)
	: MWidget(hWnd, r, pCon), MVisualControl()
{
	if(pContainer != NULL)
	{
		*pContainer += static_cast<IWidget&>(*this);
		*pContainer += static_cast<IVisualControl&>(*this);
	}
}
AVisualControl::~AVisualControl()
{
	if(pContainer != NULL)
	{
		*pContainer -= static_cast<IVisualControl&>(*this);
		*pContainer -= static_cast<IWidget&>(*this);
	}
}

void
AVisualControl::RequestFocus(const MEventArgs& e)
{
	GMFocusResponser<IVisualControl>* const p(CheckFocusContainer(pContainer));

	if(p && AFocusRequester::RequestFocus(*p))
		EventMap[EControl::GotFocus](*this, e);
}
void
AVisualControl::ReleaseFocus(const MEventArgs& e)
{
	GMFocusResponser<IVisualControl>* const p(CheckFocusContainer(pContainer));

	if(p && AFocusRequester::ReleaseFocus(*p))
		EventMap[EControl::LostFocus](*this, e);
}


YVisualControl::YVisualControl(HWND hWnd, const Rect& r, IWidgetContainer* pCon)
	: YComponent(), AVisualControl(hWnd, r, pCon)
{}
YVisualControl::~YVisualControl()
{}


MScrollBar::MScrollBar(SDST blMaxThumbSize, SDST blPrev, SDST blNext)
	: MaxThumbSize(blMaxThumbSize), PrevButtonSize(blPrev), NextButtonSize(blNext),
	bPrevButtonPressed(false), bNextButtonPressed(false)
{}


AScrollBar::AScrollBar(SDST blMaxThumbSize, SDST blPrev, SDST blNext)
	: MScrollBar(blMaxThumbSize, blPrev, blNext)
{}

void
AScrollBar::DrawForeground()
{
	DrawPrevButton();
	DrawNextButton();
	DrawScrollArea();
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

