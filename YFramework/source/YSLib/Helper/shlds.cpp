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
\version r1952;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-03-13 14:17:14 +0800;
\par 修改时间:
	2012-02-23 18:34 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Helper::Shell_DS;
*/


#include "YSLib/Helper/shlds.h"

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

ShlDS::ShlDS(const shared_ptr<Desktop>& h_dsk_up,
	const shared_ptr<Desktop>& h_dsk_down)
	: Shell(),
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

	YSL_ Components::FetchGUIState().Reset();
	return 0;
}

int
ShlDS::OnDeactivated()
{
	hDskUp->ClearContents();
	hDskDown->ClearContents();
	return 0;
}

int
ShlDS::OnGotMessage(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_ACTIVATED:
		return OnActivated(msg);
	case SM_DEACTIVATED:
		return OnDeactivated();
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

			auto content(FetchTarget<SM_INPUT>(msg));

			KeysInfo& k(content.Keys);
			Desktop& d(*hDskDown); // TODO: assertion & etc;
		//	Desktop& d(FetchGlobalInstance().GetTouchableDesktop());

			using namespace YSL_ KeySpace;
			using namespace YSL_ Components;

			auto& st(FetchGUIState());

			if(k.Up & Touch)
			{
				TouchEventArgs e(d, content.CursorLocation);

				st.ResponseTouch(e, TouchUp);
			}
			else if(k.Up)
			{
				KeyEventArgs e(d, k.Up);

				st.ResponseKey(e, KeyUp);
			}
			if(k.Down & Touch)
			{
				TouchEventArgs e(d, content.CursorLocation);

				st.ResponseTouch(e, TouchDown);
			}
			else if(k.Down)
			{
				KeyEventArgs e(d, k.Down);

				st.ResponseKey(e, KeyDown);
			}
			if(k.Held & Touch)
			{
				TouchEventArgs e(d, content.CursorLocation);

				st.ResponseTouch(e, TouchHeld);
			}
			else if(k.Held)
			{
				KeyEventArgs e(d, k.Held);

				st.ResponseKey(e, KeyHeld);
			}
		}
		SendMessage<SM_PAINT>(FetchShellHandle(), 0xE0, nullptr);
		return 0;
	default:
		break;
	}
	return Shell::OnGotMessage(msg);
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

