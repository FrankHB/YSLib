// YSLib::Shell::YGUI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-09 11:00 + 08:00;
// Version = 0.2586;


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
		p->RequestFocus();
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
		p->ReleaseFocus();
		q = dynamic_cast<IWidget*>(p);
	}while(q && (p = dynamic_cast<IVisualControl*>(q->GetContainerPtr())));
}


//记录输入保持状态。
Vec InputStatus::DragOffset(Vec::FullScreen);
InputStatus::KeyHeldStateType InputStatus::KeyHeldState(KeyFree);
Timers::YTimer InputStatus::KeyTimer(1000, false);

void
InputStatus::RepeatKeyHeld(MVisualControl& c, const MKeyEventArgs& e)
{
	//三状态自动机。
	switch(KeyHeldState)
	{
	case KeyFree:
		//必须立即转移状态，否则 KeyTimer.Activate() 会使 KeyTimer.Refresh() 始终为 false ，导致状态无法转移。
		KeyHeldState = KeyPressed;
		KeyTimer.SetInterval(240); //初始按键延迟。
		KeyTimer.Activate();
		break;

	case KeyPressed:		
	case KeyHeld:
		if(KeyTimer.Refresh())
		{
			if(KeyHeldState == KeyPressed)
			{
				KeyHeldState = KeyHeld;
				KeyTimer.SetInterval(120); //重复按键延迟。
			}
			try
			{
				c.KeyDown(dynamic_cast<IVisualControl&>(c), e);
			}
			catch(std::bad_cast&)
			{}
		}
		break;
	}
}

void
InputStatus::ResetKeyHeldState()
{
	KeyTimer.Deactivate();
	KeyHeldState = KeyFree;
}


namespace
{
	//即时输入（按下）状态所在控件指针。
	MVisualControl* p_TouchDown(NULL);
	MVisualControl* p_KeyDown(NULL);
	MVisualControl* p_TouchDown_locked(NULL); //持续按键离开控件时记录的原控件指针。

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
	TryEnter(IVisualControl& con, const MTouchEventArgs& e)
	{
		try
		{
			MVisualControl& c(dynamic_cast<MVisualControl&>(con));

			if(p_TouchDown_locked == &c)
			{
				c.Enter(con, e);
				p_TouchDown_locked = NULL;
				return true;
			}
		}
		catch (std::bad_cast&)
		{}
		return false;
	}
	bool
	TryLeave(IVisualControl& con, const MTouchEventArgs& e)
	{
		try
		{
			MVisualControl& c(dynamic_cast<MVisualControl&>(con));

			if(p_TouchDown_locked == NULL)
			{
				c.Leave(con, e);
				p_TouchDown_locked = &c;
				return true;
			}
		}
		catch (std::bad_cast&)
		{}
		return false;
	}

	MVisualControl*
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
			if(p_TouchDown_locked != NULL && p_TouchDown_locked == dynamic_cast<MVisualControl*>(p))
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
		return p != NULL ? dynamic_cast<MVisualControl*>(p) : dynamic_cast<MVisualControl*>(pCon);
	}

	MVisualControl*
	GetFocusedEnabledVisualControlPtr(IVisualControl* pFocused)
	{
		MVisualControl* p(dynamic_cast<MVisualControl*>(pFocused));

		return p && p->IsEnabled() ? p : NULL;
	}

	inline MVisualControl*
	GetFocusedEnabledVisualControlPtr(YDesktop& d)
	{
		return GetFocusedEnabledVisualControlPtr(GetFocusedObject(d));
	}

	bool
	ResponseTouchUpBase(MVisualControl& c, const MTouchEventArgs& e)
	{
		try
		{
			IVisualControl& con(dynamic_cast<IVisualControl&>(c));

			if(p_TouchDown == &c)
			{
				c.Click(con, e);
				p_TouchDown = NULL;
			}
			c.TouchUp(con, e);
			TryLeave(con, e);
			p_TouchDown_locked = NULL;
		}
		catch(std::bad_cast&)
		{
			return false;
		}
		return true;
	}
	bool
	ResponseTouchDownBase(MVisualControl& c, const MTouchEventArgs& e)
	{
		p_TouchDown = &c;
		try
		{
			IVisualControl& con(dynamic_cast<IVisualControl&>(c));

			p_TouchDown_locked = &c;
			TryEnter(con, e);
			c.TouchDown(con, e);
		}
		catch(std::bad_cast&)
		{
			return false;
		}
		InputStatus::SetDragOffset();
		return true;
	}
	bool
	ResponseTouchHeldBase(MVisualControl& c, const MTouchEventArgs& e)
	{
		try
		{
			IVisualControl& con(dynamic_cast<IVisualControl&>(c));

			if(p_TouchDown != &c)
			{
				InputStatus::SetDragOffset();
				return false;
			}
			c.TouchHeld(con, e);
		}
		catch(std::bad_cast&)
		{
			return false;
		}
		return true;
	}

	bool
	ResponseKeyUpBase(MVisualControl& c, const MKeyEventArgs& e)
	{
		InputStatus::ResetKeyHeldState();
		try
		{
			IVisualControl& con(dynamic_cast<IVisualControl&>(c));

			if(p_KeyDown == &c)
			{
				if(InputStatus::GetKeyHeldState() == InputStatus::KeyFree)
					c.KeyPress(con, e);
				p_KeyDown = NULL;
			}
			c.KeyUp(con, e);
			c.Leave(con, e);
		}
		catch(std::bad_cast&)
		{
			return false;
		}
		return true;
	}
	bool
	ResponseKeyDownBase(MVisualControl& c, const MKeyEventArgs& e)
	{
		p_KeyDown = &c;
		try
		{
			IVisualControl& con(dynamic_cast<IVisualControl&>(c));

			c.Enter(con, e);
			c.KeyDown(con, e);
		}
		catch(std::bad_cast&)
		{
			return false;
		}
		return true;
	}
	bool
	ResponseKeyHeldBase(MVisualControl& c, const MKeyEventArgs& e)
	{
		if(p_KeyDown != &c)
		{
			InputStatus::ResetKeyHeldState();
			return false;
		}
		try
		{
			c.KeyHeld(dynamic_cast<IVisualControl&>(c), e);
		}
		catch(std::bad_cast&)
		{
			return false;
		}
		return true;
	}

	bool
	ResponseTouchBase(IWidgetContainer& con, HTouchCallback f)
	{
		Point pt(f);
		MVisualControl* pVC(GetTouchedVisualControl(con, pt));

		return pVC != NULL ? f(*pVC, f) : false;
	}
	bool
	ResponseKeyBase(YDesktop& d, HKeyCallback f)
	{
		MVisualControl* const p(GetFocusedEnabledVisualControlPtr(d));

		return f(p != NULL ? *p : d);
	}
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

