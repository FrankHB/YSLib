/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file InputManager.cpp
\ingroup Helper
\brief 输入管理器。
\version r1156;
\author FrankHB<frankhb1989@gmail.com>
\since build 323 。
\par 创建时间:
	2012-07-06 11:23:21 +0800;
\par 修改时间:
	2012-07-08 09:07 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Helper::InputManager;
*/


#include "Helper/InputManager.h"
#include <YCLib/Input.h>
#include <YSLib/UI/ygui.h>
#include <YSLib/UI/ydesktop.h>
#include "Helper/DSMain.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Devices)

InputManager::InputManager()
	: cursor_state()
{}

void
InputManager::DispatchInput(Desktop& dsk)
{
#if YCL_DS
#	define YCL_KEY_Touch KeyCodes::Touch
#	define YCL_CURSOR_VALID
#elif YCL_MINGW32
#	define YCL_KEY_Touch VK_LBUTTON
#	define YCL_CURSOR_VALID if(cursor_state != Point::Invalid)
	if(::GetForegroundWindow() != Shells::HostDemon::FetchWindowHandle())
		return;
#else
#	error Unsupported platform found!
#endif

	using namespace platform::KeyCodes;
	using namespace YSL_ Components;

	// FIXME: [DS] crashing after sleeping(default behavior of closing then
	//	reopening lid) on real machine due to LibNDS default interrupt
	//	handler for power management.
//	platform::AllowSleep(true);
	platform_ex::UpdateKeyStates();

	KeyInput keys(platform_ex::FetchKeyUpState());

	if(platform_ex::FetchKeyState()[YCL_KEY_Touch])
	{
		using namespace Drawing;

		platform::CursorInfo cursor;

		platform_ex::WriteCursor(cursor);
#if YCL_DS
		cursor_state = cursor.operator Point();
#elif YCL_MINGW32
		::ScreenToClient(Shells::HostDemon::FetchWindowHandle(), &cursor);
		yunseq(cursor_state.X = cursor.x,
			cursor_state.Y = cursor.y - MainScreenHeight);
		if(!Rect(Point::Zero, MainScreenWidth, MainScreenHeight)
			.Contains(cursor_state))
			cursor_state = Point::Invalid;
#endif
	}

	auto& st(FetchGUIState());
	const auto disp([&](const KeyInput& keyset, VisualEvent key_evt,
		VisualEvent touch_evt){
		if(keyset[YCL_KEY_Touch])
		{
			YCL_CURSOR_VALID
			{
				TouchEventArgs e(dsk, cursor_state);

				st.ResponseTouch(e, touch_evt);
			}
		}
		else if(keyset.any())
		{
			KeyEventArgs e(dsk, keyset);

			st.ResponseKey(e, key_evt);
		}
	});

	disp(keys, KeyUp, TouchUp);
	keys = platform_ex::FetchKeyDownState();
	disp(keys, KeyDown, TouchDown);
	disp(platform_ex::FetchKeyState(), KeyHeld, TouchHeld);
#undef YCL_CURSOR_VALID
#undef YCL_KEY_Touch
}

YSL_END_NAMESPACE(Devices)

YSL_END

