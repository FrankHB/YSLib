// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-18 13:44:34 + 08:00;
// UTime = 2010-11-06 13:24 + 08:00;
// Version = 0.3871;


#include "ycontrol.h"
#include "ywindow.h"
#include "ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

const ScreenPositionEventArgs
	ScreenPositionEventArgs::Empty = ScreenPositionEventArgs();


const InputEventArgs InputEventArgs::Empty = InputEventArgs();


const TouchEventArgs TouchEventArgs::Empty = TouchEventArgs();


const KeyEventArgs KeyEventArgs::Empty = KeyEventArgs();


void
OnKeyHeld(IVisualControl& c, const KeyEventArgs& e)
{
	using namespace InputStatus;

	if(RepeatHeld(KeyHeldState, 240, 120))
		c.GetKeyDown()(c, e);
}

void
OnTouchHeld(IVisualControl& c, const TouchEventArgs& e)
{
	using namespace InputStatus;
	bool b(RepeatHeld(TouchHeldState, 0, 0));

	if(DraggingOffset == Vec::FullScreen)
		DraggingOffset = c.GetLocation() - VisualControlLocationOffset;
	else if(b && c.GetLocation()
		!= VisualControlLocationOffset + DraggingOffset)
		c.GetTouchMove()(c, VisualControlLocationOffset);
}

void
OnTouchMove(IVisualControl& c, const TouchEventArgs& e)
{
	c.SetLocation(e + InputStatus::DraggingOffset);
	c.Refresh();
}


MVisualControl::MVisualControl()
	: Control(), AFocusRequester()
{}


AVisualControl::AVisualControl(HWND hWnd, const Rect& r, IUIBox* pCon)
	: Widget(hWnd, r, pCon), MVisualControl()
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
	IUIBox* p(GetContainerPtr());

	if(p && p->ResponseFocusRequest(*this))
		EventMap[EControl::GotFocus](*this, e);
}

void
AVisualControl::ReleaseFocus(const EventArgs& e)
{
	IUIBox* p(GetContainerPtr());

	if(p && p->ResponseFocusRelease(*this))
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

