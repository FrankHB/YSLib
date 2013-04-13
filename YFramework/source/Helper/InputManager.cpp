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
\version r236
\author FrankHB <frankhb1989@gmail.com>
\since build 323
\par 创建时间:
	2012-07-06 11:23:21 +0800
\par 修改时间:
	2013-04-13 12:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::InputManager
*/


#include "Helper/InputManager.h"
#include "YCLib/Input.h"
#include "Host.h"

YSL_BEGIN

using namespace UI;

YSL_BEGIN_NAMESPACE(Devices)

InputManager::InputManager()
	: GUI_state(FetchGUIState()), cursor_state()
#if YCL_HOSTED
	, env(Host::FetchEnvironment())
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
#if YCL_MINGW32
		if(keyset[YCL_KEY_Touch] || keyset[VK_RBUTTON])
#else
		if(keyset[YCL_KEY_Touch])
#endif
		{
			YCL_CURSOR_VALID
			{
				TouchEventArgs e(wgt, keyset, cursor_state);

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

#if YCL_MINGW32
	const auto& key_st(platform_ex::FetchKeyState());

	if(key_st[YCL_KEY_Touch] || key_st[VK_RBUTTON])
#else
	if(platform_ex::FetchKeyState()[YCL_KEY_Touch])
#endif
	{
		using namespace Drawing;

		platform::CursorInfo cursor;

		platform_ex::WriteCursor(cursor);
#if YCL_DS
		cursor_state = cursor.operator Point();
#elif YCL_MINGW32
		::ScreenToClient(p_wnd->GetNativeHandle(), &cursor);

		const auto& pr(p_wnd->GetInputBounds());

		if(!(IsInInterval< ::LONG>(cursor.x, pr.first.X, pr.second.X)
			&& IsInInterval< ::LONG>(cursor.y, pr.first.Y, pr.second.Y)))
		{
			if(GUI_state.get().GetTouchDownPtr())
			{
				RestrictInInterval(cursor.x, pr.first.X, pr.second.X),
				RestrictInInterval(cursor.y, pr.first.Y, pr.second.Y);
			}
			else
				return nullptr;
		}
		yunseq(cursor_state.X = cursor.x - pr.first.X,
			cursor_state.Y = cursor.y - pr.first.Y);
#endif
	}
#if YCL_HOSTED
	if(auto p_render_wnd = dynamic_cast<Host::RenderWindow*>(p_wnd))
		return &p_render_wnd->GetRenderer().GetWidgetRef();
#endif
	return nullptr;
}
#undef YCL_CURSOR_VALID
#undef YCL_KEY_Touch

YSL_END_NAMESPACE(Devices)

YSL_END

