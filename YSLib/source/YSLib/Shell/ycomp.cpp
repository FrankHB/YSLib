/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycomp.cpp
\ingroup Shell
\brief 平台无关的组件和图形用户界面组件实现。
\version 0.1890;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-19 20:05:18 + 08:00;
\par 修改时间:
	2011-01-27 23:27 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YComponent;
*/


#include "ydesktop.h"
#include "../Helper/yglobal.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

void
Activate(YConsole& c, Drawing::Color fc, Drawing::Color bc)
{
	InitConsole(c.Screen, fc, bc);
}

void
Deactivate(YConsole&)
{
	InitVideo();
}

YSL_BEGIN_NAMESPACE(Widgets)


YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Shells)

using namespace Components::Widgets;

YGUIShell::YGUIShell()
	: YShell(),
	sWnds()
{}
YGUIShell::~YGUIShell() ythrow()
{}

void
YGUIShell::operator+=(HWND h)
{
	if(h)
		sWnds.push_back(h);
}
bool
YGUIShell::operator-=(HWND h)
{
	WNDs::iterator i(std::find(sWnds.begin(), sWnds.end(), h));

	if(i == sWnds.end())
		return false;
	sWnds.erase(i);
	return true;
}
YGUIShell::WNDs::size_type YGUIShell::RemoveAll(HWND h)
{
	return ystdex::erase_all(sWnds, h);
}

void
YGUIShell::RemoveWindow()
{
	sWnds.pop_back();
}

HWND
YGUIShell::GetFirstWindowHandle() const
{
	return HWND(sWnds.empty() ? NULL : sWnds.front());
}
HWND
YGUIShell::GetTopWindowHandle() const
{
	return HWND(sWnds.empty() ? NULL : sWnds.back());
}
HWND
YGUIShell::GetTopWindowHandle(YDesktop& d, const Point& p) const
{
	for(WNDs::const_iterator i(sWnds.begin()); i != sWnds.end(); ++i)
	{
		// TODO: assert(*i);

		if(FetchDirectDesktopPtr(**i) == &d && Contains(**i, p))
			return HWND(*i);
	}
	return NULL;
}

bool
YGUIShell::SendWindow(IWindow& w)
{
	if(std::find(sWnds.begin(), sWnds.end(), &w) != sWnds.end())
	{
		YDesktop* const pDsk(FetchDirectDesktopPtr(w));

		if(pDsk)
		{
			*pDsk += static_cast<IVisualControl&>(w);
			return true;
		}
	}
	return false;
}

void
YGUIShell::DispatchWindows()
{
	for(WNDs::const_iterator i(sWnds.begin()); i != sWnds.end(); ++i)
	{
		// TODO: assert(*i);

		YDesktop* const pDsk(FetchDirectDesktopPtr(**i));

		if(pDsk)
			*pDsk += *static_cast<IVisualControl*>(GetPointer(*i));
	}
}

void
YGUIShell::ClearScreenWindows(YDesktop& d)
{
	for(WNDs::const_iterator i(sWnds.begin()); i != sWnds.end(); ++i)
		d.RemoveAll(*static_cast<IVisualControl*>(GetPointer(*i)));
}

int
YGUIShell::ShlProc(const Message& msg)
{
	return ParentType::ShlProc(msg);
}

YSL_END_NAMESPACE(Shells)

YSL_END

