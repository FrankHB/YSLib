// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-18 13:44:34 + 08:00;
// UTime = 2010-11-01 14:08 + 08:00;
// Version = 0.3729;


#include "ycontrol.h"
#include "ywindow.h"
#include "ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

using namespace Runtime;

void
OnKeyHeld(IVisualControl& c, const Runtime::KeyEventArgs& e)
{
	using namespace InputStatus;

	if(RepeatHeld(KeyHeldState, e, 240, 120))
		c.GetKeyDown()(c, e);
}

void
OnTouchHeld(IVisualControl& c, const Runtime::TouchEventArgs& e)
{
	using namespace InputStatus;
	bool b(RepeatHeld(TouchHeldState, e.GetKey(), 0, 0));

	if(!IsOnDragging())
		SetDragOffset(c.GetLocation() - e);
	else if(b && c.GetLocation() != e + GetDragOffset())
		c.GetTouchMove()(c, e);
}

void
OnTouchMove(IVisualControl& c, const Runtime::TouchEventArgs& e)
{
	c.SetLocation(e + InputStatus::GetDragOffset());
	c.Refresh();
}


MVisualControl::MVisualControl()
	: MControl(), AFocusRequester()
{}

GMFocusResponser<IVisualControl>*
MVisualControl::CheckFocusContainer(IUIBox* pCon)
{
	return pCon ? dynamic_cast<GMFocusResponser<IVisualControl>*>(pCon) : NULL;
}


AVisualControl::AVisualControl(HWND hWnd, const Rect& r, IUIBox* pCon)
	: MWidget(hWnd, r, pCon), MVisualControl()
{
	EventMap[EControl::GotFocus] += &AVisualControl::OnGotFocus;
	EventMap[EControl::LostFocus] += &AVisualControl::OnLostFocus;
	TouchDown += &AVisualControl::OnTouchDown;
	TouchHeld += OnTouchHeld;

	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
	{
		*p += static_cast<IWidget&>(*this);
		*p += static_cast<IVisualControl&>(*this);
	}
}
AVisualControl::~AVisualControl() ythrow()
{
	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
	{
		*p -= static_cast<IWidget&>(*this);
		*p -= static_cast<IVisualControl&>(*this);
	}
}

void
AVisualControl::RequestFocus(const EventArgs& e)
{
	GMFocusResponser<IVisualControl>* const p(CheckFocusContainer(
		GetContainerPtr()));

	if(p && AFocusRequester::RequestFocus(*p))
		EventMap[EControl::GotFocus](*this, e);
}

void
AVisualControl::ReleaseFocus(const EventArgs& e)
{
	GMFocusResponser<IVisualControl>* const p(CheckFocusContainer(
		GetContainerPtr()));

	if(p && AFocusRequester::ReleaseFocus(*p))
		EventMap[EControl::LostFocus](*this, e);
}

void
AVisualControl::OnGotFocus(const EventArgs&)
{
	Refresh();
}

void
AVisualControl::OnTouchDown(const TouchEventArgs& e)
{
	RequestFocus(e);
}


YVisualControl::YVisualControl(HWND hWnd, const Rect& r, IUIBox* pCon)
	: YComponent(),
	AVisualControl(hWnd, r, pCon)
{}
YVisualControl::~YVisualControl() ythrow()
{}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

