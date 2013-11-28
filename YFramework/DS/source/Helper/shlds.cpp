/*
	© 2010-2013 FrankHB.

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
\version r1360
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-13 14:17:14 +0800
\par 修改时间:
	2013-11-26 19:51 +0800
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
#include <YCLib/Input.h>

namespace YSLib
{

namespace Shells
{

using namespace Messaging;

int
ShlCLI::ExecuteCommand(const ucs2_t*)
{
	// TODO: Implementation.
	return 0;
}


} // namespace Shells;


namespace DS
{

void
ResetDSDesktops(Desktop& dsk_m, Desktop& dsk_s)
{
	auto& app(FetchGlobalInstance<DSApplication>());

	ResetDesktop(dsk_m, app.GetScreenUp()),
	ResetDesktop(dsk_s, app.GetScreenDown());
}


ShlDS::ShlDS(const shared_ptr<Desktop>& h_main,
	const shared_ptr<Desktop>& h_sub)
	: GUIShell(),
	main_desktop_ptr(h_main ? h_main : make_shared<Desktop>(
		FetchGlobalInstance<DSApplication>().GetScreenUp())),
	sub_desktop_ptr(h_sub ? h_sub : make_shared<Desktop>(
		FetchGlobalInstance<DSApplication>().GetScreenDown())),
	cursor_desktop_ptr(sub_desktop_ptr),
	bUpdateUp(), bUpdateDown()
{
	YAssert(bool(main_desktop_ptr), "Null pointer found.");
	YAssert(bool(sub_desktop_ptr), "Null pointer found.");
}

void
ShlDS::OnGotMessage(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_Paint:
		ShlDS::OnInput();
		return;
	case SM_Input:
#if YF_Hosted
		if(const auto p_wgt = imMain.Update())
			imMain.DispatchInput(*p_wgt);
		else
			imMain.DispatchInput(*cursor_desktop_ptr);
#else
		imMain.Update();
		imMain.DispatchInput(*cursor_desktop_ptr);
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

	yunseq(bUpdateUp = bool(main_desktop_ptr->Validate()),
		bUpdateDown = bool(sub_desktop_ptr->Validate()));
	GUIShell::OnInput();
	if(bUpdateUp)
		main_desktop_ptr->Update();
	if(bUpdateDown)
		sub_desktop_ptr->Update();
}

void
ShlDS::SwapDesktops()
{
	std::swap(main_desktop_ptr, sub_desktop_ptr);
}

void
ShlDS::SwapScreens()
{
	auto& app(FetchGlobalInstance<DSApplication>());

	app.SwapScreens();
	cursor_desktop_ptr = app.IsLCDMainOnTop() ? sub_desktop_ptr
		: main_desktop_ptr;
}

void
ShlDS::WrapForSwapScreens(YSLib::UI::IWidget& wgt, KeyInput& mask)
{
	using namespace YSLib::UI;

	FetchEvent<KeyDown>(wgt).Add([&, this](KeyEventArgs&& e){
		if(e.Strategy != RoutedEventArgs::Bubble && mask.any()
			&& (platform_ex::FetchKeyState() & mask) == mask)
		{
			SwapScreens();
			e.Handled = true;
		}
	}, 0xE0);
}

} // namespace DS;

} // namespace YSLib;

