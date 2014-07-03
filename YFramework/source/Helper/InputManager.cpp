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
\version r498
\author FrankHB <frankhb1989@gmail.com>
\since build 323
\par 创建时间:
	2012-07-06 11:23:21 +0800
\par 修改时间:
	2014-07-02 09:56 +0800
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
#if YCL_Win32
#	include YFM_YSLib_UI_YControl // for UI::CallEvent.
#elif YCL_Android
#	include YFM_Android_Helper_AndroidHost // for Android::NativeHost;
#	include YFM_YSLib_UI_YDesktop // for Desktop converting to IWidget;
#endif

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
#if YCL_Win32
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
	if(const auto p_wnd = env.get().GetForegroundWindow())
	{
		const UI::WheelDelta raw_mouse(p_wnd->RawMouseButton);

		if(raw_mouse != 0)
		{
			CursorWheelEventArgs e(wgt, raw_mouse, keys, cursor_state);

			st.ResponseCursor(e, CursorWheel);
			p_wnd->RawMouseButton = 0;
		}

		// TODO: Use C++14 lambda initializers to simplify implementation.
		const auto p_input(st.ExteralTextInputFocusPtr);

		p_wnd->AccessInputString([=, &st](String& ustr){
			if(YB_UNLIKELY(p_input != p_text_focus_cache))
			{
				ustr.clear();
				yunseq(p_text_focus_cache = p_input,
					caret_location_cache = Point::Invalid);
			}
			else if(p_input)
			{
				auto& wgt(*p_input);
				// XXX: Save actual key state.
				KeyInput k;
				size_t n(0);

				for(const auto c : ustr)
				{
					++n;
					YTraceDe(Informative, "Host character %d found.", int(c));
					if(platform::IsPrint(c))
						// XXX: Redundant nonzero test on character value.
						CallInputEvent(c, k, wgt);
					else
						break;
				}
				ustr.erase(ustr.begin(), ustr.begin() + n);
				// XXX: Duplicate locking.
				if(st.CaretLocation != caret_location_cache)
					p_wnd->UpdateCandidateWindowLocation(
						caret_location_cache = st.CaretLocation);
			}
		});
	}
#endif
}

IWidget*
InputManager::Update()
{
#if YF_Hosted
	const auto p_wnd(env.get().GetForegroundWindow());

#	if !YCL_Android
	if(!p_wnd)
		return {};

#	endif
#endif
	using namespace platform::KeyCodes;

	// FIXME: [DS] crashing after sleeping(default behavior of closing then
	//	reopening lid) on real machine due to LibNDS default interrupt
	//	handler for power management.
//	platform::AllowSleep(true);
	platform_ex::UpdateKeyStates();

#if YF_Hosted
	cursor_state = env.get().MapCursor();
#elif YCL_DS
	if(platform_ex::FetchKeyState()[Touch])
		cursor_state = platform_ex::FetchCursor();
#endif
#if YF_Hosted
	if(const auto p_render_wnd = dynamic_cast<Host::RenderWindow*>(p_wnd))
		return &p_render_wnd->GetRenderer().GetWidgetRef();
#endif
#if YCL_Android
	return &Android::FetchDefaultDesktop();
#else
	return {};
#endif
}
#undef YCL_CURSOR_VALID

} // namespace Devices;

} // namespace YSLib;

