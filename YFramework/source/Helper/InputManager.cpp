/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file InputManager.cpp
\ingroup Helper
\brief 输入管理器。
\version r387
\author FrankHB <frankhb1989@gmail.com>
\since build 323
\par 创建时间:
	2012-07-06 11:23:21 +0800
\par 修改时间:
	2014-04-14 19:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::InputManager
*/


#include "Helper/YModules.h"
#include YFM_Helper_InputManager
#include YFM_YCLib_Input
#include YFM_Helper_Host
#include YFM_Helper_HostRenderer // for Host::Window, Host::RenderWindow;
#include YFM_Helper_GUIApplication // for FetchEnvironment;

namespace YSLib
{

using namespace UI;

namespace Devices
{

InputManager::InputManager()
	: GUI_state(FetchGUIState()), cursor_state()
#if YF_Hosted
	, env(Host::FetchEnvironment())
#endif
{}

#if YCL_DS
#	define YCL_CURSOR_VALID
#elif YF_Hosted
#	define YCL_CURSOR_VALID if(cursor_state != Point::Invalid)
#else
#	error "Unsupported platform found."
#endif
void
InputManager::DispatchInput(IWidget& wgt)
{
	auto& st(GUI_state.get());
	const auto disp([&](const KeyInput& keyset, VisualEvent key_evt,
		VisualEvent touch_evt){
		if(keyset[KeyCodes::Primary] || keyset[KeyCodes::Secondary]
			|| keyset[KeyCodes::Tertiary])
		{
			YCL_CURSOR_VALID
			{
				CursorEventArgs e(wgt, keyset, cursor_state);

				st.ResponseCursor(e, touch_evt);
			}
		}
		else if(keyset.any())
		{
			KeyEventArgs e(wgt, keyset);

			st.ResponseKey(e, key_evt);
		}
	});
	KeyInput keys(platform_ex::FetchKeyUpState());

	disp(keys, KeyUp, TouchUp);
#if YF_Hosted
	YCL_CURSOR_VALID
	{
		CursorEventArgs e(wgt, keys, cursor_state);

		st.ResponseCursor(e, CursorOver);
	}
#endif
	keys = platform_ex::FetchKeyDownState();
	disp(keys, KeyDown, TouchDown);
	keys = platform_ex::FetchKeyState();
	disp(keys, KeyHeld, TouchHeld);
#if YCL_Win32
	const UI::WheelDelta raw_mouse(env.get().RawMouseButton);

	if(raw_mouse != 0)
	{
		CursorWheelEventArgs e(wgt, raw_mouse, keys, cursor_state);

		st.ResponseCursor(e, CursorWheel);
		env.get().RawMouseButton = 0;
	}
#endif
}

IWidget*
InputManager::Update()
{
#if YF_Hosted
	const auto p_wnd(env.get().GetForegroundWindow());

	if(!p_wnd)
		return {};

#endif
	using namespace platform::KeyCodes;

	// FIXME: [DS] crashing after sleeping(default behavior of closing then
	//	reopening lid) on real machine due to LibNDS default interrupt
	//	handler for power management.
//	platform::AllowSleep(true);
	platform_ex::UpdateKeyStates();

#if YCL_DS
	if(platform_ex::FetchKeyState()[Touch])
#endif
	{
#if YCL_DS
		cursor_state = platform_ex::FetchCursor();
#elif YCL_Win32
		::POINT cursor;

		::GetCursorPos(&cursor);
		::ScreenToClient(p_wnd->GetNativeHandle(), &cursor);

		const auto& pr(p_wnd->GetInputBounds());

		if(YB_LIKELY(pr.first.X != pr.second.X && pr.first.Y != pr.second.Y)
			&& (!p_wnd->BoundsLimited
			|| (IsInInterval<::LONG>(cursor.x, pr.first.X, pr.second.X)
			&& IsInInterval<::LONG>(cursor.y, pr.first.Y, pr.second.Y))))
			yunseq(cursor_state.X = cursor.x - pr.first.X,
				cursor_state.Y = cursor.y - pr.first.Y);
		else
			cursor_state = Drawing::Point::Invalid;
#endif
	}
#if YF_Hosted
	if(const auto p_render_wnd = dynamic_cast<Host::RenderWindow*>(p_wnd))
		return &p_render_wnd->GetRenderer().GetWidgetRef();
#endif
	return {};
}
#undef YCL_CURSOR_VALID

} // namespace Devices;

} // namespace YSLib;

