/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GUIShell.cpp
\ingroup Helper
\brief GUI Shell 类。
\version r84
\author FrankHB <frankhb1989@gmail.com>
\since build 399
\par 创建时间:
	2013-04-14 05:42:22 +0800
\par 修改时间:
	2013-04-14 06:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::GUIShell
*/


#include "Helper/shlds.h"
#include "Helper/DSMain.h"
#include "Helper/ShellHelper.h"
#include "YSLib/UI/ydesktop.h"
#include "YSLib/UI/ygui.h"
#include "Host.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

using namespace Messaging;

GUIShell::GUIShell()
	: Shell(),
	imMain()
{
	YSLib::UI::FetchGUIState().Reset();
}

void
GUIShell::OnGotMessage(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_PAINT:
		GUIShell::OnInput();
		return;
	case SM_INPUT:
		if(auto p_wgt = imMain.Update())
			imMain.DispatchInput(*p_wgt);
		OnInput();
		return;
	default:
		break;
	}
	Shell::OnGotMessage(msg);
}

void
GUIShell::OnInput()
{
#if YCL_HOSTED
	// TODO: Use host reference stored by shell.
	Host::FetchEnvironment().UpdateRenderWindows();
#endif
	// NOTE: Use code below instead if asynchronous posting is necessary.
//	PostMessage<SM_PAINT>(0xE0, nullptr);
}

void
GUIShell::OnPaint()
{}

YSL_END_NAMESPACE(Shells)

YSL_END

