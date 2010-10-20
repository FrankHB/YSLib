// YSLib::Shell::YGUI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-20 09:21 + 08:00;
// Version = 0.2842;


#include "ygui.h"
//#include <stack>

YSL_BEGIN

using namespace Drawing;
using namespace Components;
using namespace Components::Controls;
using namespace Components::Widgets;

YSL_BEGIN_NAMESPACE(Runtime)

IWidget*
GetCursorWidgetPtr(HSHL hShl, YDesktop& d, const Point& pt)
{
	HWND hWnd(hShl->GetTopWindowHandle(d, pt));

	return hWnd ? hWnd->GetTopWidgetPtr(pt) : NULL;
}

IVisualControl*
GetFocusedObject(YDesktop& d)
{
	IVisualControl* p(d.GetFocusingPtr());
	GMFocusResponser<IVisualControl>* q;

	while(p && (q = dynamic_cast<GMFocusResponser<IVisualControl>*>(p)) && q->GetFocusingPtr())
		p = q->GetFocusingPtr();
	return p;
}

void
RequestFocusCascade(IVisualControl& c)
{
	IVisualControl* p(&c);

	do
	{
		p->RequestFocus(GetZeroElement<MEventArgs>());
	}while((p = dynamic_cast<IVisualControl*>(p->GetContainerPtr())));
}

void
ReleaseFocusCascade(IVisualControl& c)
{
	IVisualControl* p(&c);

	do
	{
		p->ReleaseFocus(GetZeroElement<MEventArgs>());
	}while((p = dynamic_cast<IVisualControl*>(p->GetContainerPtr())));
}


//记录输入保持状态。

YSL_BEGIN_NAMESPACE(InputStatus)

HeldStateType KeyHeldState(Free);
HeldStateType TouchHeldState(Free);
Vec DragOffset(Vec::FullScreen);
Timers::YTimer HeldTimer(1000, false);

bool
IsOnDragging()
{
	return DragOffset != Vec::FullScreen;
}

const Vec&
GetDragOffset()
{
	return DragOffset;
}

void
SetDragOffset(const Vec& v)
{
	DragOffset = v;
}

bool
RepeatHeld(HeldStateType& s, const MKeyEventArgs& e, Timers::TimeSpan InitialDelay, Timers::TimeSpan RepeatedDelay)
{
	//三状态自动机。
	switch(s)
	{
	case Free:
		//必须立即转移状态，否则 HeldTimer.Activate() 会使 HeldTimer.Refresh() 始终为 false ，导致状态无法转移。
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

	//即时输入（按下）状态所在控件指针。
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
	TryEnter(IVisualControl& c, const MTouchEventArgs& e)
	{
		if(!bEntered && p_TouchDown == &c)
		{
			c.GetEnter()(c, e);
			bEntered = true;
		}
	}
	void
	TryLeave(IVisualControl& c, const MTouchEventArgs& e)
	{
		if(bEntered && p_TouchDown == &c)
		{
			c.GetLeave()(c, e);
			bEntered = false;
		}
	}

	IVisualControl*
	GetTouchedVisualControl(IWidgetContainer& con, Point& pt)
	{
		using namespace ExOp;

	/*	if(bEntered && p_TouchDown != NULL && ExtraOperation != TouchUp && ExtraOperation != TouchHeld)
		{
			pt = p_TouchDown->GetLocation();
			return p_TouchDown;
		}*/

		IWidgetContainer* pCon(&con);
		IVisualControl* p;

		while((p = pCon->GetTopVisualControlPtr(pt)) != NULL)
		{
			if((pCon = dynamic_cast<IWidgetContainer*>(p)) == NULL)
			{
				pt -= p->GetLocation();
				break;
			}
			pt -= pCon->GetLocation();
			if(ExtraOperation == TouchDown)
			{
				pCon->RequestToTop();
				pCon->ClearFocusingPtr();
			}
		}
		if(ExtraOperation == TouchHeld)
		{
			if(p_TouchDown != p)
			{
				if(bEntered)
					TryLeave(*p_TouchDown, pt);
				return NULL;
			}
			else if(!bEntered)
				TryEnter(*p, pt);
			return p_TouchDown;
		}
		return p != NULL ? p : dynamic_cast<IVisualControl*>(pCon);
	}

	IVisualControl*
	GetFocusedEnabledVisualControlPtr(IVisualControl* p)
	{
		return p != NULL && p->IsEnabled() ? p : NULL;
	}

	inline IVisualControl*
	GetFocusedEnabledVisualControlPtr(YDesktop& d)
	{
		return GetFocusedEnabledVisualControlPtr(GetFocusedObject(d));
	}

	bool
	ResponseKeyUpBase(IVisualControl& c, const MKeyEventArgs& e)
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
	ResponseKeyDownBase(IVisualControl& c, const MKeyEventArgs& e)
	{
		p_KeyDown = &c;
		c.GetEnter()(c, e);
		c.GetKeyDown()(c, e);
		return true;
	}
	bool
	ResponseKeyHeldBase(IVisualControl& c, const MKeyEventArgs& e)
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
	ResponseTouchUpBase(IVisualControl& c, const MTouchEventArgs& e)
	{
		ResetHeldState(TouchHeldState);
		SetDragOffset();
		if(p_TouchDown == &c && TouchHeldState == Free)
			c.GetClick()(c, e);
		c.GetTouchUp()(c, e);
		TryLeave(c, e);
		return true;
	}
	bool
	ResponseTouchDownBase(IVisualControl& c, const MTouchEventArgs& e)
	{
		p_TouchDown = &c;
		TryEnter(c, e);
		c.GetTouchDown()(c, e);
		return true;
	}
	bool
	ResponseTouchHeldBase(IVisualControl& c, const MTouchEventArgs& e)
	{
		if(p_TouchDown != &c)
		{
			ResetHeldState(TouchHeldState);
			SetDragOffset();
			return false;
		}
		c.GetTouchHeld()(c, e);
		return true;
	}

	bool
	ResponseKeyBase(YDesktop& d, HKeyCallback f)
	{
		IVisualControl* const p(GetFocusedEnabledVisualControlPtr(d));

		return f(p != NULL ? *p : d);
	}

	bool
	ResponseTouchBase(IWidgetContainer& con, HTouchCallback f)
	{
		Point pt(f);
		IVisualControl* pVC(GetTouchedVisualControl(con, pt));

		return pVC != NULL ? f(*pVC, f) : false;
	}
}

bool
ResponseKeyUp(YDesktop& d, const MKeyEventArgs& e)
{
	return ResponseKeyBase(d, HKeyCallback(e, ResponseKeyUpBase));
}
bool
ResponseKeyDown(YDesktop& d, const MKeyEventArgs& e)
{
	return ResponseKeyBase(d, HKeyCallback(e, ResponseKeyDownBase));
}
bool
ResponseKeyHeld(YDesktop& d, const MKeyEventArgs& e)
{
	return ResponseKeyBase(d, HKeyCallback(e, ResponseKeyHeldBase));
}

bool
ResponseTouchUp(IWidgetContainer& con, const MTouchEventArgs& e)
{
	ExtraOperation = ExOp::TouchUp;
	return ResponseTouchBase(con, HTouchCallback(e, ResponseTouchUpBase));
}
bool
ResponseTouchDown(IWidgetContainer& con, const MTouchEventArgs& e)
{
	ExtraOperation = ExOp::TouchDown;
	return ResponseTouchBase(con, HTouchCallback(e, ResponseTouchDownBase));
}
bool
ResponseTouchHeld(IWidgetContainer& con, const MTouchEventArgs& e)
{
	ExtraOperation = ExOp::TouchHeld;
	return ResponseTouchBase(con, HTouchCallback(e, ResponseTouchHeldBase));
}

YSL_END_NAMESPACE(Runtime)

YSL_BEGIN_NAMESPACE(Drawing)

void
DrawBounds(const Graphics& g, const Point& location, const Size& size, Color c)
{
	if(size.GetWidth() > 1 && size.GetHeight() > 1)
		DrawRect(g, location, Size(size - Vec(1, 1)), c);
}

void
DrawWindowBounds(HWND hWnd, Color c)
{
	Graphics g(GetGraphicInterfaceContext(hWnd));

	DrawBounds(g, Point::Zero, hWnd->GetSize(), c);
}

void
DrawWidgetBounds(IWidget& w, Color c)
{
	if(w.GetWindowHandle())
	{
		Graphics g(GetGraphicInterfaceContext(w.GetWindowHandle()));

		DrawBounds(g, w.GetLocation(), w.GetSize(), c);
	}
}

YSL_END_NAMESPACE(Drawing)

YSL_END

