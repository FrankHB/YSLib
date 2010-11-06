// YSLib::Shell::YGUI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-11-06 13:24 + 08:00;
// Version = 0.3012;


#include "ygui.h"
//#include <stack>

YSL_BEGIN

using namespace Drawing;
using namespace Components;
using namespace Components::Controls;
using namespace Components::Widgets;

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

IWidget*
GetCursorWidgetPtr(HSHL hShl, YDesktop& d, const Point& pt)
{
	HWND hWnd(hShl->GetTopWindowHandle(d, pt));

	return hWnd ? hWnd->GetTopWidgetPtr(pt) : NULL;
}

IVisualControl*
GetFocusedObjectPtr(YDesktop& d)
{
	IVisualControl* p(d.GetFocusingPtr()), *q(NULL);
	IUIBox* pCon;

	while(p && (pCon = dynamic_cast<IUIBox*>(p))
		&& (q = pCon->GetFocusingPtr()))
		p = q;
	return p;
}

void
RequestFocusCascade(IVisualControl& c)
{
	IVisualControl* p(&c);

	do
	{
		p->RequestFocus(GetZeroElement<EventArgs>());
	}while((p = dynamic_cast<IVisualControl*>(p->GetContainerPtr())));
}

void
ReleaseFocusCascade(IVisualControl& c)
{
	IVisualControl* p(&c);

	do
	{
		p->ReleaseFocus(GetZeroElement<EventArgs>());
	}while((p = dynamic_cast<IVisualControl*>(p->GetContainerPtr())));
}


//记录输入保持状态。

YSL_BEGIN_NAMESPACE(InputStatus)

HeldStateType KeyHeldState(Free);
HeldStateType TouchHeldState(Free);
Vec DraggingOffset(Vec::FullScreen);
Timers::YTimer HeldTimer(1000, false);
Point VisualControlLocationOffset(Point::Zero);

bool
RepeatHeld(HeldStateType& s,
	Timers::TimeSpan InitialDelay, Timers::TimeSpan RepeatedDelay)
{
	//三状态自动机。
	switch(s)
	{
	case Free:
		/*
		必须立即转移状态，否则 HeldTimer.Activate() 会使 HeldTimer.Refresh()
		始终为 false ，导致状态无法转移。
		*/
		s = Pressed;
		HeldTimer.SetInterval(InitialDelay); //初始按键延迟。
		HeldTimer.Activate();
		break;

	case Pressed:		
	case Held:
		if(HeldTimer.Refresh())
		{
			if(s == Pressed)
			{
				s = Held;
				HeldTimer.SetInterval(RepeatedDelay); //重复按键延迟。
			}
			return true;
		}
		break;
	}
	return false;
}

void
ResetHeldState(HeldStateType& s)
{
	HeldTimer.Deactivate();
	s = Free;
}

YSL_END_NAMESPACE(InputStatus)

namespace
{
	using namespace InputStatus;

	//独立焦点指针：即时输入（按下）状态所在控件指针。
	IVisualControl* p_TouchDown(NULL);
	IVisualControl* p_KeyDown(NULL);

	bool bEntered(false); //记录指针是否在控件内部。

	namespace ExOp
	{
		typedef enum
		{
			NoOp = 0,
			TouchUp = 1,
			TouchDown = 2,
			TouchHeld = 3
		} ExOpType;
	};
	ExOp::ExOpType ExtraOperation(ExOp::NoOp);

	void
	TryEnter(IVisualControl& c, const TouchEventArgs& e)
	{
		if(!bEntered && p_TouchDown == &c)
		{
			c.GetEnter()(c, e);
			bEntered = true;
		}
	}
	void
	TryLeave(IVisualControl& c, const TouchEventArgs& e)
	{
		if(bEntered && p_TouchDown == &c)
		{
			c.GetLeave()(c, e);
			bEntered = false;
		}
	}

	void ResetTouchHeldState()
	{
		ResetHeldState(TouchHeldState);
		DraggingOffset = Vec::FullScreen;
	}

	IVisualControl*
	GetTouchedVisualControlPtr(IUIBox& con, Point& pt)
	{
		using namespace ExOp;

		IUIBox* pCon(&con);
		IVisualControl* p;

		while((p = pCon->GetTopVisualControlPtr(pt)))
		{
			pt -= p->GetLocation();
			if(!(pCon = dynamic_cast<IUIBox*>(p)))
				break;
			if(ExtraOperation == TouchDown)
			{
				pCon->RequestToTop();
				pCon->ClearFocusingPtr();
			}
		}
		if(!p)
			p = dynamic_cast<IVisualControl*>(pCon);
		if(ExtraOperation == TouchHeld)
		{
			if(p_TouchDown != p)
			{
				if(bEntered)
					TryLeave(*p_TouchDown, pt);
			}
			else if(!bEntered)
				TryEnter(*p, pt);
			return p_TouchDown;
		}
		return p;
	}

	IVisualControl*
	GetFocusedEnabledVisualControlPtr(IVisualControl* p)
	{
		return p && p->IsEnabled() ? p : NULL;
	}

	inline IVisualControl*
	GetFocusedEnabledVisualControlPtr(YDesktop& d)
	{
		return GetFocusedEnabledVisualControlPtr(GetFocusedObjectPtr(d));
	}

	bool
	ResponseKeyUpBase(IVisualControl& c, const KeyEventArgs& e)
	{
		ResetHeldState(KeyHeldState);
		if(p_KeyDown == &c && KeyHeldState == Free)
			c.GetKeyPress()(c, e);
		c.GetKeyUp()(c, e);
		c.GetLeave()(c, e);
		p_KeyDown = NULL;
		return true;
	}
	bool
	ResponseKeyDownBase(IVisualControl& c, const KeyEventArgs& e)
	{
		p_KeyDown = &c;
		c.GetEnter()(c, e);
		c.GetKeyDown()(c, e);
		return true;
	}
	bool
	ResponseKeyHeldBase(IVisualControl& c, const KeyEventArgs& e)
	{
		if(p_KeyDown != &c)
		{
			ResetHeldState(KeyHeldState);
			return false;
		}
		c.GetKeyHeld()(c, e);
		return true;
	}

	bool
	ResponseTouchUpBase(IVisualControl& c, const TouchEventArgs& e)
	{
		ResetTouchHeldState();
		if(p_TouchDown == &c && TouchHeldState == Free)
			c.GetClick()(c, e);
		c.GetTouchUp()(c, e);
		TryLeave(c, e);
		p_TouchDown = NULL;
		return true;
	}
	bool
	ResponseTouchDownBase(IVisualControl& c, const TouchEventArgs& e)
	{
		p_TouchDown = &c;
		TryEnter(c, e);
		c.GetTouchDown()(c, e);
		return true;
	}
	bool
	ResponseTouchHeldBase(IVisualControl& c, const TouchEventArgs& e)
	{
		if(p_TouchDown != &c)
		{
			ResetTouchHeldState();
			return false;
		}
		c.GetTouchHeld()(c, e);
		return true;
	}

	bool
	ResponseKeyBase(YDesktop& d, HKeyCallback f)
	{
		IVisualControl* const p(GetFocusedEnabledVisualControlPtr(d));

		return f(p ? *p : d);
	}

	bool
	ResponseTouchBase(IUIBox& con, HTouchCallback f)
	{
		VisualControlLocationOffset = f;
		IVisualControl* const pVC(GetTouchedVisualControlPtr(con, f));

		return pVC ? f(*pVC, f) : false;
	}
}

bool
ResponseKeyUp(YDesktop& d, const KeyEventArgs& e)
{
	return ResponseKeyBase(d, HKeyCallback(e, ResponseKeyUpBase));
}
bool
ResponseKeyDown(YDesktop& d, const KeyEventArgs& e)
{
	return ResponseKeyBase(d, HKeyCallback(e, ResponseKeyDownBase));
}
bool
ResponseKeyHeld(YDesktop& d, const KeyEventArgs& e)
{
	return ResponseKeyBase(d, HKeyCallback(e, ResponseKeyHeldBase));
}

bool
ResponseTouchUp(IUIBox& con, const TouchEventArgs& e)
{
	ExtraOperation = ExOp::TouchUp;
	return ResponseTouchBase(con, HTouchCallback(e, ResponseTouchUpBase));
}
bool
ResponseTouchDown(IUIBox& con, const TouchEventArgs& e)
{
	ExtraOperation = ExOp::TouchDown;
	return ResponseTouchBase(con, HTouchCallback(e, ResponseTouchDownBase));
}
bool
ResponseTouchHeld(IUIBox& con, const TouchEventArgs& e)
{
	ExtraOperation = ExOp::TouchHeld;
	return ResponseTouchBase(con, HTouchCallback(e, ResponseTouchHeldBase));
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Drawing)

void
DrawWindowBounds(HWND hWnd, Color c)
{
	Graphics g(*hWnd);

	DrawRect(g, Point::Zero, Size(hWnd->GetSize() - Vec(1, 1)), c);
}

void
DrawWidgetBounds(IWidget& w, Color c)
{
	if(w.GetWindowHandle())
	{
		Graphics g(*w.GetWindowHandle());

		DrawRect(g, LocateForWindow(w), Size(w.GetSize() - Vec(1, 1)), c);
	}
}

YSL_END_NAMESPACE(Drawing)

YSL_END

