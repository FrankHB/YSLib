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
\version 0.1626;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-13 14:17:14 +0800;
\par 修改时间:
	2011-03-05 23:02 +0800;
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
	ResetGUIStates();
	ClearScreenWindows(theApp.GetPlatformResource().GetDesktopUp());
	ClearScreenWindows(theApp.GetPlatformResource().GetDesktopDown());
	return 0;
}

void
ShlGUI::SendDrawingMessage()
{
//	hDesktopUp->ClearDesktopObjects();
//	hDesktopDown->ClearDesktopObjects();
	DispatchWindows();
	SendMessage(GetCurrentShellHandle(), SM_PAINT, 0xE0,
		new GHandleContext<GHHandle<YDesktop> >(
		theApp.GetPlatformResource().GetDesktopUpHandle()));
	SendMessage(GetCurrentShellHandle(), SM_PAINT, 0xE0,
		new GHandleContext<GHHandle<YDesktop> >(
		theApp.GetPlatformResource().GetDesktopDownHandle()));
}

void
ShlGUI::UpdateToScreen()
{
	theApp.GetPlatformResource().GetDesktopUp().Refresh();
	theApp.GetPlatformResource().GetDesktopUp().Update();
	theApp.GetPlatformResource().GetDesktopDown().Refresh();
	theApp.GetPlatformResource().GetDesktopDown().Update();
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
	GHHandle<YGUIShell> hShl(FetchGUIShellHandle());

	using namespace Runtime::KeySpace;

	if(k.Up & Touch)
		OnTouchUp(hShl, pContext->CursorLocation);
	else if(k.Up)
		OnKeyUp(hShl, k.Up);
	if(k.Down & Touch)
		OnTouchDown(hShl, pContext->CursorLocation);
	else if(k.Down)
		OnKeyDown(hShl, k.Down);
	if(k.Held & Touch)
		OnTouchHeld(hShl, pContext->CursorLocation);
	else if(k.Held)
		OnKeyHeld(hShl, k.Held);
}


void
ShlClearBothScreen(GHHandle<YGUIShell> h)
{
	if(h)
	{
		h->ClearScreenWindows(theApp.GetPlatformResource().GetDesktopUp());
		h->ClearScreenWindows(theApp.GetPlatformResource().GetDesktopDown());
	}
}

YSL_END_NAMESPACE(DS)

YSL_END

