// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-18 13:44:34 + 08:00;
// UTime = 2010-10-22 13:31 + 08:00;
// Version = 0.3631;


#include "ycontrol.h"
#include "ywindow.h"
#include "ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

using namespace Runtime;

MVisualControl::MVisualControl()
	: MControl(), AFocusRequester()
{}

GMFocusResponser<IVisualControl>*
MVisualControl::CheckFocusContainer(IWidgetContainer* pCon)
{
	return pCon ? dynamic_cast<GMFocusResponser<IVisualControl>*>(pCon) : NULL;
}


AVisualControl::AVisualControl(HWND hWnd, const Rect& r, IWidgetContainer* pCon)
	: MWidget(hWnd, r, pCon), MVisualControl()
{
	EventMap[EControl::GotFocus] += &AVisualControl::OnGotFocus;
	EventMap[EControl::LostFocus] += &AVisualControl::OnLostFocus;
	TouchDown += &AVisualControl::OnTouchDown;
	TouchHeld += &AVisualControl::OnTouchHeld;
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

void
AVisualControl::OnGotFocus(const MEventArgs&)
{
	Refresh();
}

void
AVisualControl::OnLostFocus(const MEventArgs& e)
{
	Refresh();
}

void
AVisualControl::OnKeyHeld(const Runtime::MKeyEventArgs& e)
{
	using namespace InputStatus;

	if(RepeatHeld(KeyHeldState, e, 240, 120))
		KeyDown(*this, e);
}

void
AVisualControl::OnTouchDown(const MTouchEventArgs& e)
{
	RequestFocus(e);
}

void
AVisualControl::OnTouchHeld(const Runtime::MTouchEventArgs& e)
{
	using namespace InputStatus;
	bool b(RepeatHeld(TouchHeldState, e.GetKey(), 0, 0));

	if(!IsOnDragging())
		SetDragOffset(GetLocation() - e);
	else if(b && GetLocation() != e + GetDragOffset())
		TouchMove(*this, e);
}

void
AVisualControl::OnTouchMove(const Runtime::MTouchEventArgs& e)
{
	SetLocation(e + InputStatus::GetDragOffset());
	Refresh();
}


YVisualControl::YVisualControl(HWND hWnd, const Rect& r, IWidgetContainer* pCon)
	: YComponent(), AVisualControl(hWnd, r, pCon)
{}
YVisualControl::~YVisualControl()
{}


MScrollBar::MScrollBar(SDST blMaxThumbSize, SDST blPrev, SDST blNext)
	: MinThumbSize(blMaxThumbSize), PrevButtonSize(blPrev), NextButtonSize(blNext),
	bPrevButtonPressed(false), bNextButtonPressed(false)
{}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

