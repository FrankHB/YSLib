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
\brief Shell 类库 DS 版本。
\version r1247
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-13 14:17:14 +0800
\par 修改时间:
	2013-03-13 13:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Shell_DS
*/


#include "Helper/shlds.h"
#include "Helper/DSMain.h"
#include "YSLib/UI/ydesktop.h"
#include "YSLib/UI/ygui.h"
#include "Host.h"

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

ShlDS::ShlDS(const shared_ptr<Desktop>& hUp, const shared_ptr<Desktop>& hDn)
	: Shell(),
	input_mgr(), desktop_up_ptr(hUp ? hUp : make_shared<Desktop>(
		FetchGlobalInstance().GetScreenUp())),
	desktop_down_ptr(hDn ? hDn : make_shared<Desktop>(
		FetchGlobalInstance().GetScreenDown())),
	bUpdateUp(), bUpdateDown()
{
	YAssert(bool(desktop_up_ptr), "Null pointer found.");
	YAssert(bool(desktop_down_ptr), "Null pointer found.");

	YSLib::UI::FetchGUIState().Reset();
}

void
ShlDS::OnGotMessage(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_PAINT:
#if 0
		{
			const auto h(FetchTarget<SM_PAINT>(msg));

			if(h)
				h->Refresh(PaintContext(h->GetContext(), Point(),
					GetSizeOf(*h)));
		}
#endif
		ShlDS::OnInput();
		return;
	case SM_INPUT:
#if YCL_HOSTED
		if(auto p_wgt = input_mgr.Update())
			input_mgr.DispatchInput(*p_wgt);
		else
			input_mgr.DispatchInput(*desktop_down_ptr);
#else
		input_mgr.Update();
		input_mgr.DispatchInput(*desktop_down_ptr);
#endif
		OnInput();
		return;
	default:
		break;
	}
	Shell::OnGotMessage(msg);
}

void
ShlDS::OnInput()
{
	using Drawing::Rect;

	yunseq(bUpdateUp = bool(desktop_up_ptr->Validate()),
		bUpdateDown = bool(desktop_down_ptr->Validate()));
	OnPaint();
	if(bUpdateUp)
		desktop_up_ptr->Update();
	if(bUpdateDown)
		desktop_down_ptr->Update();
#if YCL_HOSTED
	// TODO: Use host reference stored by shell.
	FetchGlobalInstance().GetHost().UpdateRenderWindows();
#endif
	// NOTE: Use code below instead if asynchronous posting is necessary.
//	PostMessage<SM_PAINT>(0xE0, nullptr);
}

void
ShlDS::OnPaint()
{}

YSL_END_NAMESPACE(DS)

YSL_END

