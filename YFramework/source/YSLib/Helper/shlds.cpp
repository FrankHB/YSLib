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
\version r2086;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-03-13 14:17:14 +0800;
\par 修改时间:
	2012-04-01 08:45 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Helper::Shell_DS;
*/


#include "YSLib/Helper/DSMain.h"
#include "YSLib/Helper/yglobal.h"
#include "YSLib/UI/ydesktop.h"
#include "YSLib/UI/ygui.h"

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
	YAssert(bool(desktop_up_ptr),
		"Null up desktop handle found @ ShlDS::ShlDS;");
	YAssert(bool(desktop_down_ptr),
		"Null down desktop handle found @ ShlDS::ShlDS;");

	YSL_ Components::FetchGUIState().Reset();
}

int
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
		return 0;
	case SM_INPUT:
		//平台相关输入处理。
		{
			//等待图形用户界面输入。

			// NOTE: no real necessity to put input content into message queue,
			//	for the content is serialized in form of exactly one instance
			//	to be accepted at one time and no input signal is handled
			//	through interrupt to be buffered.
			static KeysInfo keys;
			static Drawing::Point cursor_pos;

			// FIXME: crashing after sleeping(default behavior of closing then
			// reopening lid) on real machine due to LibNDS default interrupt
			// handler for power management;
		//	platform::AllowSleep(true);
			platform::WriteKeys(keys);
			if(keys.Held & KeySpace::Touch)
			{
				CursorInfo cursor;

				platform::WriteCursor(cursor);
				yunseq(cursor_pos.X = cursor.GetX(),
					cursor_pos.Y = cursor.GetY());
			}

			const KeysInfo& k(keys);
			Desktop& d(*desktop_down_ptr); // TODO: assertion & etc;
		//	Desktop& d(FetchGlobalInstance().GetTouchableDesktop());

			using namespace YSL_ KeySpace;
			using namespace YSL_ Components;

			auto& st(FetchGUIState());

			if(k.Up & Touch)
			{
				TouchEventArgs e(d, cursor_pos);

				st.ResponseTouch(e, TouchUp);
			}
			else if(k.Up)
			{
				KeyEventArgs e(d, k.Up);

				st.ResponseKey(e, KeyUp);
			}
			if(k.Down & Touch)
			{
				TouchEventArgs e(d, cursor_pos);

				st.ResponseTouch(e, TouchDown);
			}
			else if(k.Down)
			{
				KeyEventArgs e(d, k.Down);

				st.ResponseKey(e, KeyDown);
			}
			if(k.Held & Touch)
			{
				TouchEventArgs e(d, cursor_pos);

				st.ResponseTouch(e, TouchHeld);
			}
			else if(k.Held)
			{
				KeyEventArgs e(d, k.Held);

				st.ResponseKey(e, KeyHeld);
			}
		}
		OnInput();
		return 0;
	default:
		break;
	}
	return Shell::OnGotMessage(msg);
}

void
ShlDS::OnInput()
{
	PostMessage<SM_PAINT>(FetchShellHandle(), 0xE0, nullptr);
}

void
ShlDS::OnPaint()
{}

YSL_END_NAMESPACE(DS)

YSL_END

