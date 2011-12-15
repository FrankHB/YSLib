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
\version r1894;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-03-13 14:17:14 +0800;
\par 修改时间:
	2011-12-13 13:51 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::Shell_DS;
*/


#include "YSLib/Helper/shlds.h"

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
ShlCLI::ExecuteCommand(const ucs2_t*)
{
	// TODO: impl;
	return 0;
}


YSL_END_NAMESPACE(Shells)


YSL_BEGIN_NAMESPACE(DS)

ShlDS::ShlDS(const shared_ptr<Desktop>& h_dsk_up,
	const shared_ptr<Desktop>& h_dsk_down)
	: GUIShell(),
	hDskUp(h_dsk_up ? h_dsk_up : share_raw(new
		Desktop(FetchGlobalInstance().GetScreenUp()))),
	hDskDown(h_dsk_down ? h_dsk_down : share_raw(new
		Desktop(FetchGlobalInstance().GetScreenDown())))
{}

int
ShlDS::OnActivated(const Message&)
{
	YAssert(bool(hDskUp), "Null up desktop handle found @ ShlDS::ShlDS;");
	YAssert(bool(hDskDown), "Null down desktop handle found @ ShlDS::ShlDS;");

	ResetGUIStates();
	return 0;
}

int
ShlDS::OnDeactivated(const Message&)
{
	YAssert(&FetchGUIShell() == this,
		"Invalid GUI shell found @ ShlDS::OnDeactivated");

	hDskUp->ClearContents();
	hDskDown->ClearContents();
	return 0;
}

int
ShlDS::OnGotMessage(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_PAINT:
		{
			auto h(Messaging::FetchTarget<SM_PAINT>(msg));

			if(!h)
			{
				UpdateToScreen();
				return 0;
			}
		}
		break;
	case SM_INPUT:
		//平台相关输入处理。
		{
			using namespace Messaging;

			auto hContent(FetchTarget<SM_INPUT>(msg));

			if(hContent)
			{
				KeysInfo& k(hContent->Keys);
				Desktop& d(*hDskDown); // TODO: assertion & etc;
			//	Desktop& d(FetchGlobalInstance().GetTouchableDesktop());

				using namespace YSL_ KeySpace;
				using namespace YSL_ Components;

				if(k.Up & Touch)
				{
					TouchEventArgs e(d, hContent->CursorLocation);

					ResponseTouch(e, TouchUp);
				}
				else if(k.Up)
				{
					KeyEventArgs e(d, k.Up);

					ResponseKey(e, KeyUp);
				}
				if(k.Down & Touch)
				{
					TouchEventArgs e(d, hContent->CursorLocation);

					ResponseTouch(e, TouchDown);
				}
				else if(k.Down)
				{
					KeyEventArgs e(d, k.Down);

					ResponseKey(e, KeyDown);
				}
				if(k.Held & Touch)
				{
					TouchEventArgs e(d, hContent->CursorLocation);

					ResponseTouch(e, TouchHeld);
				}
				else if(k.Held)
				{
					KeyEventArgs e(d, k.Held);

					ResponseKey(e, KeyHeld);
				}
			}
		}
		SendMessage<SM_PAINT>(FetchShellHandle(), 0xE0, nullptr);
		return 0;
	default:
		break;
	}
	return GUIShell::OnGotMessage(msg);
}

void
ShlDS::UpdateToScreen()
{
	yunseq(hDskUp->Validate(), hDskDown->Validate());
	hDskUp->Update();
	hDskDown->Update();
}

YSL_END_NAMESPACE(DS)

YSL_END

