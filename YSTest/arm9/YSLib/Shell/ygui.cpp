// YSLib::Shell::YGUI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58;
// UTime = 2010-7-14 13:56;
// Version = 0.2278;


#include "ygui.h"

YSL_BEGIN

using namespace Drawing;
using namespace Components;
using namespace Components::Controls;
using namespace Components::Widgets;

YSL_BEGIN_NAMESPACE(Runtime)

IWidget*
GetCursorWidgetPtr(HSHL hShl, YDesktop& d, const SPoint& pt)
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
SVec TouchStatus::v_DragOffset(SVec::FullScreen);

namespace
{
	MVisualControl* p_TouchDown;
	MVisualControl* p_KeyDown;

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
	ResponseTouchUpBase(MVisualControl& c, const YTouchEventArgs& e)
	{
		try
		{
			IVisualControl& con(dynamic_cast<IVisualControl&>(c));

			c.TouchUp(con, e);
			if(p_TouchDown == &c)
			{
				c.Click(con, e);
				p_TouchDown = NULL;
			}
		}
		catch(std::bad_cast&)
		{
			return false;
		}
		return true;
	}

	bool
	ResponseTouchDownBase(MVisualControl& c, const YTouchEventArgs& e)
	{
		p_TouchDown = &c;
		try
		{
			c.TouchDown(dynamic_cast<IVisualControl&>(c), e);
		}
		catch(std::bad_cast&)
		{
			return false;
		}
		TouchStatus::SetDragOffset();
		return true;
	}

	bool
	ResponseTouchHeldBase(MVisualControl& c, const YTouchEventArgs& e)
	{
		if(p_TouchDown != &c)
		{
			TouchStatus::SetDragOffset();
			return false;
		}
		try
		{
			c.TouchHeld(dynamic_cast<IVisualControl&>(c), e);
		}
		catch(std::bad_cast&)
		{
			return false;
		}
		return true;
	}

	bool
	ResponseKeyUpBase(MVisualControl& c, const YKeyEventArgs& e)
	{
		try
		{
			IVisualControl& con(dynamic_cast<IVisualControl&>(c));

			c.KeyUp(con, e);
			if(p_KeyDown == &c)
			{
				c.KeyPress(con, e);
				p_KeyDown = NULL;
			}
		}
		catch(std::bad_cast&)
		{
			return false;
		}
		return true;
	}

	bool
	ResponseKeyDownBase(MVisualControl& c, const YKeyEventArgs& e)
	{
		p_KeyDown = &c;
		try
		{
			c.KeyDown(dynamic_cast<IVisualControl&>(c), e);
		}
		catch(std::bad_cast&)
		{
			return false;
		}
		return true;
	}

	bool
	ResponseKeyHeldBase(MVisualControl& c, const YKeyEventArgs& e)
	{
		if(p_KeyDown != &c)
			return false;
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
	ResponseKeyBase(YDesktop& d, HKeyCallback f)
	{
		MVisualControl* const p(GetFocusedEnabledVisualControlPtr(d));

		return f(p ? *p : d);
	}
}

bool
ResponseTouchUp(IWidgetContainer& c, const YTouchEventArgs& e)
{
	IWidgetContainer* pCon(&c);
	IVisualControl* p;
	SPoint pt(e);

	while((p = pCon->GetTopVisualControlPtr(pt)))
	{
		if(!(pCon = dynamic_cast<IWidgetContainer*>(p)))
			break;
		pt -= pCon->GetLocation();
	}

	MVisualControl* pVC(p ? dynamic_cast<MVisualControl*>(p) : dynamic_cast<MVisualControl*>(pCon));

	return pVC ? ResponseTouchUpBase(*pVC, e) : false;
}

bool
ResponseTouchDown(IWidgetContainer& c, const YTouchEventArgs& e)
{
	IWidgetContainer* pCon(&c);
	IVisualControl* p;
	SPoint pt(e);

	while((p = pCon->GetTopVisualControlPtr(pt)))
	{
		p->RequestFocus();
		if(!(pCon = dynamic_cast<IWidgetContainer*>(p)))
			break;
		pt -= pCon->GetLocation();
		pCon->RequestToTop();
		pCon->ClearFocusingPtr();
	}

	MVisualControl* pVC(p ? dynamic_cast<MVisualControl*>(p) : dynamic_cast<MVisualControl*>(pCon));

	return pVC ? ResponseTouchDownBase(*pVC, e) : false;
}

bool
ResponseTouchHeld(IWidgetContainer& c, const YTouchEventArgs& e)
{
	IWidgetContainer* pCon(&c);
	IVisualControl* p;
	SPoint pt(e);

	while((p = pCon->GetTopVisualControlPtr(pt)))
	{
		p->RequestFocus();
		if(!(pCon = dynamic_cast<IWidgetContainer*>(p)))
			break;
		pt -= pCon->GetLocation();
	}

	MVisualControl* pVC(p ? dynamic_cast<MVisualControl*>(p) : dynamic_cast<MVisualControl*>(pCon));

	return pVC ? ResponseTouchHeldBase(*pVC, e) : false;
}

bool
ResponseKeyUp(YDesktop& d, const YKeyEventArgs& e)
{
	return ResponseKeyBase(d, HKeyCallback(e, ResponseKeyUpBase));
}
bool
ResponseKeyDown(YDesktop& d, const YKeyEventArgs& e)
{
	return ResponseKeyBase(d, HKeyCallback(e, ResponseKeyDownBase));
}
bool
ResponseKeyHeld(YDesktop& d, const YKeyEventArgs& e)
{
	return ResponseKeyBase(d, HKeyCallback(e, ResponseKeyHeldBase));
}

YSL_END_NAMESPACE(Runtime)

YSL_BEGIN_NAMESPACE(Drawing)

void
DrawBounds(YGIC& g, const SPoint& location, const SSize& size, PixelType c)
{
	DrawRect(g, location, SPoint(location + size - SVec(1, 1)), c);
}

void
DrawWindowBounds(HWND hWnd, PixelType c)
{
	YGIC g(GetGraphicInterfaceContext(hWnd));

	DrawBounds(g, SPoint::Zero, hWnd->GetSize(), c);
}

void
DrawWidgetBounds(IWidget& w, PixelType c)
{
	if(w.GetWindowHandle())
	{
		YGIC g(GetGraphicInterfaceContext(w.GetWindowHandle()));

		DrawBounds(g, w.GetLocation(), w.GetSize(), c);
	}
}

YSL_END_NAMESPACE(Drawing)

YSL_END

