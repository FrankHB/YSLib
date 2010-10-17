// YSLib::Shell::YGUI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-16 14:40 + 08:00;
// Version = 0.2745;


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
	IWidget* q;

	do
	{
		p->RequestFocus(GetZeroElement<MEventArgs>());
		q = dynamic_cast<IWidget*>(p);
	}while(q && (p = dynamic_cast<IVisualControl*>(q->GetContainerPtr())));
}

void
ReleaseFocusCascade(IVisualControl& c)
{
	IVisualControl* p(&c);
	IWidget* q;

	do
	{
		p->ReleaseFocus(GetZeroElement<MEventArgs>());
		q = dynamic_cast<IWidget*>(p);
	}while(q && (p = dynamic_cast<IVisualControl*>(q->GetContainerPtr())));
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
	IVisualControl* p_TouchDown_locked(NULL); //持续按键离开控件时记录的原控件指针。

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

	bool
	TryEnter(IVisualControl& c, const MTouchEventArgs& e)
	{
		if(p_TouchDown_locked == &c)
		{
			c.GetEnter()(c, e);
			p_TouchDown_locked = NULL;
			return true;
		}
		return false;
	}
	bool
	TryLeave(IVisualControl& c, const MTouchEventArgs& e)
	{
		if(p_TouchDown_locked == NULL)
		{
			c.GetLeave()(c, e);
			p_TouchDown_locked = &c;
			return true;
		}
		return false;
	}

	IVisualControl*
	GetTouchedVisualControl(IWidgetContainer& con, Point& pt)
	{
		using namespace ExOp;

		if(p_TouchDown_locked != NULL && ExtraOperation != TouchUp && ExtraOperation != TouchHeld)
		{
			IWidget* pWgt(dynamic_cast<IWidget*>(p_TouchDown_locked));

			pt = pWgt->GetLocation();
			return p_TouchDown_locked;
		}

		IWidgetContainer* pCon(&con);
		IVisualControl* p;

		while((p = pCon->GetTopVisualControlPtr(pt)) != NULL)
		{
			if((pCon = dynamic_cast<IWidgetContainer*>(p)) == NULL)
			{
				IWidget* pWgt(dynamic_cast<IWidget*>(p));
				
				if(pWgt != NULL)
					pt -= pWgt->GetLocation();
				break;
			}
			pt -= pCon->GetLocation();
			if(ExtraOperation == TouchDown)
			{
				pCon->RequestToTop();
				pCon->ClearFocusingPtr();
			}
		}
		switch(ExtraOperation)
		{
		case TouchUp:
			p_TouchDown_locked = NULL;
			break;
		case TouchHeld:
			if(p_TouchDown_locked != NULL && p_TouchDown_locked == dynamic_cast<IVisualControl*>(p))
				TryEnter(*p, pt);
			if(p_TouchDown_locked == NULL && p == NULL)
			{
				if(dynamic_cast<IVisualControl*>(p_TouchDown) != NULL)
					TryLeave(*dynamic_cast<IVisualControl*>(p_TouchDown), pt);
				p_TouchDown_locked = p_TouchDown;
			}
		default:
			break;
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
		if(p_KeyDown == &c)
		{
			if(KeyHeldState == Free)
				c.GetKeyPress()(c, e);
			p_KeyDown = NULL;
		}
		c.GetKeyUp()(c, e);
		c.GetLeave()(c, e);
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
		if(p_TouchDown == &c)
		{
			c.GetClick()(c, e);
			p_TouchDown = NULL;
		}
		c.GetTouchUp()(c, e);
		TryLeave(c, e);
		p_TouchDown_locked = NULL;
		return true;
	}
	bool
	ResponseTouchDownBase(IVisualControl& c, const MTouchEventArgs& e)
	{
		p_TouchDown = &c;
		p_TouchDown_locked = &c;
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
DrawBounds(GraphicInterfaceContext& g, const Point& location, const Size& size, PixelType c)
{
	DrawRect(g, location, Point(location + size - Vec(1, 1)), c);
}

void
DrawWindowBounds(HWND hWnd, PixelType c)
{
	GraphicInterfaceContext g(GetGraphicInterfaceContext(hWnd));

	DrawBounds(g, Point::Zero, hWnd->GetSize(), c);
}

void
DrawWidgetBounds(IWidget& w, PixelType c)
{
	if(w.GetWindowHandle())
	{
		GraphicInterfaceContext g(GetGraphicInterfaceContext(w.GetWindowHandle()));

		DrawBounds(g, w.GetLocation(), w.GetSize(), c);
	}
}

YSL_END_NAMESPACE(Drawing)

YSL_END

