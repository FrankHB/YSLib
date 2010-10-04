// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-18 13:44:34 + 08:00;
// UTime = 2010-10-04 21:50 + 08:00;
// Version = 0.3426;


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
: YComponent(), MWidget(hWnd, r, pCon), MVisualControl()
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
		Fill();
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

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

