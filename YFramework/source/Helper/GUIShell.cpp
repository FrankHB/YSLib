/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GUIShell.cpp
\ingroup Helper
\brief GUI Shell 类。
\version r108
\author FrankHB <frankhb1989@gmail.com>
\since build 399
\par 创建时间:
	2013-04-14 05:42:22 +0800
\par 修改时间:
	2013-11-26 19:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::GUIShell
*/


#include "Helper/GUIShell.h"
#include "Helper/GUIApplication.h" // for FetchEnvironent;
#include "Helper/Host.h"
#include <thread> // for std::this_thread::sleep_for;

namespace YSLib
{

namespace Shells
{

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
	case SM_Paint:
		GUIShell::OnInput();
		return;
	case SM_Input:
		if(const auto p_wgt = imMain.Update())
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
	OnPaint();
#if YF_Hosted
	// TODO: Use host reference stored by shell.
	Host::FetchEnvironment().UpdateRenderWindows();
	std::this_thread::sleep_for(IdleSleep);
#endif
	// NOTE: Use code below if non-hosted delay is needed.
//	Timers::Delay(std::chrono::duration_cast<Timers::TimeSpan>(IdleSleep));
	// NOTE: Use code below instead if asynchronous posting is necessary.
//	PostMessage<SM_PAINT>(0xE0, nullptr);
}

void
GUIShell::OnPaint()
{}

} // namespace Shells;

} // namespace YSLib;

