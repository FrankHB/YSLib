/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file shlds.cpp
\ingroup Helper
\ingroup DS
\brief DS 平台 Shell 类。
\version r1303
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-13 14:17:14 +0800
\par 修改时间:
	2013-07-09 05:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Shell_DS
*/


#include "Helper/shlds.h"
#include "Helper/DSMain.h"
#include "Helper/ShellHelper.h"
#include "YSLib/UI/ydesktop.h"
#include "YSLib/UI/ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

using namespace Messaging;

int
ShlCLI::ExecuteCommand(const ucs2_t*)
{
	// TODO: Implementation.
	return 0;
}


YSL_END_NAMESPACE(Shells)


YSL_BEGIN_NAMESPACE(DS)

void
ResetDSDesktops(Desktop& dsk_up, Desktop& dsk_dn)
{
	auto& app(FetchGlobalInstance<DSApplication>());

	ResetDesktop(dsk_up, app.GetScreenUp()),
	ResetDesktop(dsk_dn, app.GetScreenDown());
}


ShlDS::ShlDS(const shared_ptr<Desktop>& hUp, const shared_ptr<Desktop>& hDn)
	: GUIShell(),
	desktop_up_ptr(hUp ? hUp : make_shared<Desktop>(
		FetchGlobalInstance<DSApplication>().GetScreenUp())),
	desktop_down_ptr(hDn ? hDn : make_shared<Desktop>(
		FetchGlobalInstance<DSApplication>().GetScreenDown())),
	bUpdateUp(), bUpdateDown()
{
	YAssert(bool(desktop_up_ptr), "Null pointer found.");
	YAssert(bool(desktop_down_ptr), "Null pointer found.");
}

void
ShlDS::OnGotMessage(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_PAINT:
		ShlDS::OnInput();
		return;
	case SM_INPUT:
#if YCL_HOSTED
		if(auto p_wgt = imMain.Update())
			imMain.DispatchInput(*p_wgt);
		else
			imMain.DispatchInput(*desktop_down_ptr);
#else
		imMain.Update();
		imMain.DispatchInput(*desktop_down_ptr);
#endif
		OnInput();
		return;
	default:
		break;
	}
	GUIShell::OnGotMessage(msg);
}

void
ShlDS::OnInput()
{
	using Drawing::Rect;

	yunseq(bUpdateUp = bool(desktop_up_ptr->Validate()),
		bUpdateDown = bool(desktop_down_ptr->Validate()));
	GUIShell::OnInput();
	if(bUpdateUp)
		desktop_up_ptr->Update();
	if(bUpdateDown)
		desktop_down_ptr->Update();
}

YSL_END_NAMESPACE(DS)

YSL_END

