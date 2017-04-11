/*
	© 2012-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file InputManager.cpp
\ingroup Helper
\brief 输入管理器。
\version r580
\author FrankHB <frankhb1989@gmail.com>
\since build 323
\par 创建时间:
	2012-07-06 11:23:21 +0800
\par 修改时间:
	2017-04-08 14:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::InputManager
*/


#include "Helper/YModules.h"
#include YFM_Helper_InputManager // for ystdex::erase_left;
#include YFM_YCLib_Input
#include YFM_Helper_Environment
#include YFM_Helper_HostRenderer // for Host::Window, Host::RenderWindow;
#include YFM_Helper_GUIApplication // for FetchGUIHost;
#if YCL_Win32
#	include YFM_YSLib_UI_YControl // for UI::CallEvent;
#elif YCL_Android
#	include YFM_YSLib_UI_YDesktop // for Desktop converting to IWidget;
#endif

namespace YSLib
{

using namespace UI;

namespace Devices
{

InputManager::InputManager()
	: GUI_state(FetchGUIState()), cursor_state(), host(FetchGUIHost())
{}

#if YCL_DS
#	define YF_Impl_CursorValid
#elif YF_Hosted
#	define YF_Impl_CursorValid if(cursor_state != Point::Invalid)
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
			YF_Impl_CursorValid
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
#if YCL_Win32
	YF_Impl_CursorValid
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
	if(const auto p_wnd = host.get().GetForegroundWindow())
	{
		auto& input_host(p_wnd->InputHost);
		const UI::WheelDelta raw_mouse(input_host.RawMouseButton);

		if(raw_mouse != 0)
		{
			CursorWheelEventArgs e(wgt, raw_mouse, keys, cursor_state);

			st.ResponseCursor(e, CursorWheel);
			input_host.RawMouseButton = 0;
		}

		// TODO: Blocked. Use C++14 lambda initializers to simplify
		//	implementation.
		const auto p_input(st.ExternalTextInputFocusPtr);

		input_host.AccessInputString([=, &st](String& ustr){
			if(YB_UNLIKELY(p_input != p_text_focus_cache))
			{
				ustr.clear();
				yunseq(p_text_focus_cache = p_input,
					caret_location_cache = Point::Invalid);
			}
			else if(p_input)
			{
				// XXX: Save actual key state.
				KeyInput k;
				size_t n(0);

				for(const auto c : ustr)
				{
					++n;
					YTraceDe(Informative, "Host character %d found.", int(c));
					if(platform::IsPrint(c))
						// XXX: Redundant nonzero test on character value.
						CallInputEvent(c, k, *p_input);
					else
						break;
				}
				ystdex::erase_left(n, ustr);
				// TODO: Resolve duplicate locking.
				if(st.CaretLocation != caret_location_cache)
				{
					caret_location_cache = st.CaretLocation;
					p_wnd->UpdateTextInputFocus(Deref(p_text_focus_cache),
						caret_location_cache);
				}
			}
		});
	}
#endif
}

observer_ptr<IWidget>
InputManager::Update()
{
	using namespace platform::KeyCodes;

	// FIXME: [DS] crashing after sleeping(default behavior of closing then
	//	reopening lid) on real machine due to LibNDS default interrupt
	//	handler for power management.
//	platform::AllowSleep(true);
	platform_ex::UpdateKeyStates();

#if YF_Hosted
	observer_ptr<Host::Window> p_wnd;

#endif
#if YCL_Win32
	tie(p_wnd, cursor_state) = host.get().MapCursor();
	if(!p_wnd)
		return make_observer(&host.get().Desktop);
#elif YF_Hosted
	// TODO: Determine which inactive window should be used.
	cursor_state = host.get().MapCursor().second;
	if(const auto p_render_wnd = dynamic_cast<Host::RenderWindow*>(p_wnd.get()))
		return make_observer(&p_render_wnd->GetRenderer().GetWidgetRef());
#elif YCL_DS
	if(platform_ex::FetchKeyState()[Touch])
		cursor_state = platform_ex::FetchCursor();
#endif
#if YCL_Android
	return make_observer(&host.get().Desktop);
#else
	return {};
#endif
}
#undef YF_Impl_CursorValid

} // namespace Devices;

} // namespace YSLib;

