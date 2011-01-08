/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file shlds.cpp
\ingroup Helper
\ingroup DS
\brief Shell 类库 DS 版本。
\version 0.1552;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-13 14:17:14 + 08:00;
\par 修改时间:
	2011-01-07 21:52 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::Shell_DS;
*/


#include "shlds.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

using namespace Messaging;

int
ShlCLI::OnActivated(const Message&)
{
	return 0;
}

int
ShlCLI::OnDeactivated(const Message&)
{
	return 0;
}

int
ShlCLI::ExecuteCommand(const uchar_t* /*s*/)
{
	// TODO: impl;
	return 0;
}


int
ShlGUI::OnDeactivated(const Message&)
{
	Components::Controls::ResetGUIStates();
	ClearScreenWindows(*hDesktopUp);
	ClearScreenWindows(*hDesktopDown);
	return 0;
}

void
ShlGUI::SendDrawingMessage()
{
//	hDesktopUp->ClearDesktopObjects();
//	hDesktopDown->ClearDesktopObjects();
	DispatchWindows();
	SendMessage(GetCurrentShellHandle(), SM_PAINT, 0xE0,
		new GHandleContext<GHHandle<YDesktop> >(hDesktopUp));
	SendMessage(GetCurrentShellHandle(), SM_PAINT, 0xE0,
		new GHandleContext<GHHandle<YDesktop> >(hDesktopDown));
}

void
ShlGUI::UpdateToScreen()
{
	YAssert(hDesktopUp,
		"In function \"void\nDS::ShlGUI::UpdateToScreen()\":\n"
		"The desktop pointer is null.");
	YAssert(hDesktopDown,
		"In function \"void\nDS::ShlGUI::UpdateToScreen()\":\n"
		"The desktop pointer is null.");

	hDesktopUp->Refresh();
	hDesktopUp->Update();
	hDesktopDown->Refresh();
	hDesktopDown->Update();
}

YSL_END_NAMESPACE(Shells)


YSL_BEGIN_NAMESPACE(DS)

ShlDS::ShlDS()
	: ShlGUI()
{}

int
ShlDS::ShlProc(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_INPUT:
		ResponseInput(msg);
		return 0;

	default:
		return Shells::DefShellProc(msg);
	}
}

int
ShlDS::OnDeactivated(const Message& m)
{
	ShlGUI::OnDeactivated(m);
	GHHandle<YGUIShell> hShl(FetchGUIShellHandle());

	if(hShl)
	{
		*hShl -= hWndUp;
		*hShl -= hWndDown;
	}
	YReset(hWndUp);
	YReset(hWndDown);
	return 0;
}


void
ResponseInput(const Message& msg)
{
	using namespace Messaging;

	InputContext* const pContext(CastMessage<SM_INPUT>(msg));

	if(!pContext)
		return;

	Runtime::KeysInfo& k(pContext->Key);

	using namespace Runtime::KeySpace;

	if(k.Up & Touch)
		OnTouchUp(pContext->CursorLocation);
	else if(k.Up)
		OnKeyUp(k.Up);
	if(k.Down & Touch)
		OnTouchDown(pContext->CursorLocation);
	else if(k.Down)
		OnKeyDown(k.Down);
	if(k.Held & Touch)
		OnTouchHeld(pContext->CursorLocation);
	else if(k.Held)
		OnKeyHeld(k.Held);
}


void
ShlClearBothScreen(GHHandle<YGUIShell> h)
{
	if(h)
	{
		h->ClearScreenWindows(*hDesktopUp);
		h->ClearScreenWindows(*hDesktopDown);
	}
}

YSL_END_NAMESPACE(DS)

YSL_END

