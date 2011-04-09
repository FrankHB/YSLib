﻿/*
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
\version 0.1736;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-13 14:17:14 +0800;
\par 修改时间:
	2011-04-09 08:06 +0800;
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


YSL_END_NAMESPACE(Shells)


YSL_BEGIN_NAMESPACE(DS)

ShlDS::ShlDS(GHHandle<YDesktop> h_dsk_up, GHHandle<YDesktop> h_dsk_down)
	: YGUIShell(),
	hDskUp(h_dsk_up ? h_dsk_up : new YDesktop(theApp.GetPlatformResource()
		.GetScreenUp())),
	hDskDown(h_dsk_down ? h_dsk_down : new YDesktop(theApp.GetPlatformResource()
		.GetScreenDown()))
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
		return ParentType::ShlProc(msg);
	}
}

int
ShlDS::OnActivated(const Message& /*msg*/)
{
	YAssert(hDskUp, "Null up desktop handle found @ ShlDS::ShlDS;");
	YAssert(hDskDown, "Null down desktop handle found @ ShlDS::ShlDS;");

	ResetGUIStates();
	//使背景无效。
	hDskUp->SetBgRedrawed(false);
	hDskDown->SetBgRedrawed(false);
	//如果不添加此段且没有桌面没有被添加窗口等设置刷新状态的操作，
	//那么除了背景以外任何绘制都不会进行。
	hDskUp->SetRefresh(true);
	hDskDown->SetRefresh(true);
	return 0;
}

int
ShlDS::OnDeactivated(const Message& /*msg*/)
{
	YAssert(FetchGUIShellHandle() == this,
		"Invalid GUI shell found @ ShlDS::OnDeactivated");

	hDskUp->ClearContents();
	hDskDown->ClearContents();
	return 0;
}

/*void
ShlDS::SendDrawingMessage()
{
//	hDesktopUp->ClearContents();
//	hDesktopDown->ClearContents();
	DispatchWindows();
	SendMessage(GetCurrentShellHandle(), SM_PAINT, 0xE0,
		new GHandleContext<GHHandle<YDesktop> >(hDskUp));
	SendMessage(GetCurrentShellHandle(), SM_PAINT, 0xE0,
		new GHandleContext<GHHandle<YDesktop> >(hDskDown));
}*/

void
ShlDS::UpdateToScreen()
{
	hDskUp->Refresh();
	hDskDown->Refresh();
	hDskUp->Update();
	hDskDown->Update();
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
	YDesktop& d(theApp.GetPlatformResource().GetTouchableDesktop());

	using namespace Runtime::KeySpace;
	using namespace Components::Controls;

	if(k.Up & Touch)
	{
		Components::Controls::TouchEventArgs e(pContext->CursorLocation);

		hShl->ResponseTouch(d, e, TouchUp);
	}
	else if(k.Up)
	{
		Components::Controls::KeyEventArgs e(k.Up);

		hShl->ResponseKey(d, e, KeyUp);
	}
	if(k.Down & Touch)
	{
		Components::Controls::TouchEventArgs e(pContext->CursorLocation);

		hShl->ResponseTouch(d, e, TouchDown);
	}
	else if(k.Down)
	{
		Components::Controls::KeyEventArgs e(k.Down);

		hShl->ResponseKey(d, e, KeyDown);
	}
	if(k.Held & Touch)
	{
		Components::Controls::TouchEventArgs e(pContext->CursorLocation);

		hShl->ResponseTouch(d, e, TouchHeld);
	}
	else if(k.Held)
	{
		Components::Controls::KeyEventArgs e(k.Held);

		hShl->ResponseKey(d, e, KeyHeld);
	}
}

YSL_END_NAMESPACE(DS)

YSL_END

