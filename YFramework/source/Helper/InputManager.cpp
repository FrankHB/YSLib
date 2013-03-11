/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file InputManager.cpp
\ingroup Helper
\brief 输入管理器。
\version r254
\author FrankHB <frankhb1989@gmail.com>
\since build 323
\par 创建时间:
	2012-07-06 11:23:21 +0800
\par 修改时间:
	2013-03-10 23:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::InputManager
*/


#include "Helper/InputManager.h"
#include "YCLib/Input.h"
#include "Host.h"

YSL_BEGIN

using namespace Components;

YSL_BEGIN_NAMESPACE(Devices)

InputManager::InputManager()
	: GUI_state(FetchGUIState()), cursor_state()
#if YCL_HOSTED
	, env(FetchGlobalInstance().GetHost())
#endif
{}

#if YCL_DS
#	define YCL_KEY_Touch KeyCodes::Touch
#	define YCL_CURSOR_VALID
#elif YCL_MINGW32
#	define YCL_KEY_Touch VK_LBUTTON
#	define YCL_CURSOR_VALID if(cursor_state != Point::Invalid)
#else
#	error Unsupported platform found!
#endif
void
InputManager::DispatchInput(IWidget& wgt)
{
	const auto disp([&](const KeyInput& keyset, VisualEvent key_evt,
		VisualEvent touch_evt){
		if(keyset[YCL_KEY_Touch])
		{
			YCL_CURSOR_VALID
			{
				TouchEventArgs e(wgt, cursor_state);

				GUI_state.get().ResponseTouch(e, touch_evt);
			}
		}
		else if(keyset.any())
		{
			KeyEventArgs e(wgt, keyset);

			GUI_state.get().ResponseKey(e, key_evt);
		}
	});
	KeyInput keys(platform_ex::FetchKeyUpState());

	disp(keys, KeyUp, TouchUp);
	keys = platform_ex::FetchKeyDownState();
	disp(keys, KeyDown, TouchDown);
	disp(platform_ex::FetchKeyState(), KeyHeld, TouchHeld);
}

IWidget*
InputManager::Update()
{
#if YCL_MINGW32
	const auto p_wnd(env.get().GetForegroundWindow());

	if(!p_wnd)
		return nullptr;

#endif
	using namespace platform::KeyCodes;

	// FIXME: [DS] crashing after sleeping(default behavior of closing then
	//	reopening lid) on real machine due to LibNDS default interrupt
	//	handler for power management.
//	platform::AllowSleep(true);
	platform_ex::UpdateKeyStates();

	if(platform_ex::FetchKeyState()[YCL_KEY_Touch])
	{
		using namespace Drawing;

		platform::CursorInfo cursor;

		platform_ex::WriteCursor(cursor);
#if YCL_DS
		cursor_state = cursor.operator Point();
#elif YCL_MINGW32
		cursor_state = p_wnd->AdjustCursor(cursor);
#endif
	}
	return nullptr;
}
#undef YCL_CURSOR_VALID
#undef YCL_KEY_Touch

YSL_END_NAMESPACE(Devices)

YSL_END

