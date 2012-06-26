/*
	Copyright (C) by Franksoft 2010 - 2012.

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
\version r2194;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-03-13 14:17:14 +0800;
\par 修改时间:
	2012-06-25 22:36 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Helper::Shell_DS;
*/


#include "Helper/DSMain.h"
#include "Helper/yglobal.h"
#include <YSLib/UI/ydesktop.h>
#include <YSLib/UI/ygui.h>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

using namespace Messaging;

int
ShlCLI::ExecuteCommand(const ucs2_t*)
{
	// TODO: impl;
	return 0;
}


YSL_END_NAMESPACE(Shells)


YSL_BEGIN_NAMESPACE(DS)

ShlDS::ShlDS(const shared_ptr<Desktop>& hUp, const shared_ptr<Desktop>& hDn)
	: Shell(),
	desktop_up_ptr(hUp ? hUp : make_shared<Desktop>(
		FetchGlobalInstance().GetScreenUp())),
	desktop_down_ptr(hDn ? hDn : make_shared<Desktop>(
		FetchGlobalInstance().GetScreenDown())),
	bUpdateUp(), bUpdateDown()
{
	YAssert(bool(desktop_up_ptr), "Null pointer found.");
	YAssert(bool(desktop_down_ptr), "Null pointer found.");

	YSL_ Components::FetchGUIState().Reset();
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
				h->Refresh(PaintContext(h->GetContext(), Point::Zero,
					Rect(Point::Zero, GetSizeOf(*h))));
		}
#endif
		{
			using Drawing::Rect;

			yunseq(bUpdateUp = desktop_up_ptr->Validate() != Rect::Empty,
				bUpdateDown = desktop_down_ptr->Validate() != Rect::Empty);
			OnPaint();
			if(bUpdateUp)
				desktop_up_ptr->Update();
			if(bUpdateDown)
				desktop_down_ptr->Update();
		}
		return;
	case SM_INPUT:
		// TODO: assertion & etc;
		DispatchInput(*desktop_down_ptr);
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
	PostMessage<SM_PAINT>(0xE0, nullptr);
}

void
ShlDS::OnPaint()
{}

YSL_END_NAMESPACE(DS)

YSL_END

