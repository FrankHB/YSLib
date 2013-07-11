/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostWindow.cpp
\ingroup Helper
\brief 宿主环境支持的用户界面。
\version r270
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-18 18:18:46 +0800
\par 修改时间:
	2013-07-10 16:35 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::HostWindow
*/


#include "Helper/HostWindow.h"
#include "Host.h" // for WindowClassName;
#include "YCLib/Input.h" // for platform::ClearKeyStates;

YSL_BEGIN

using namespace Drawing;

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

#	if YCL_MINGW32
Window::Window(NativeWindowHandle h, Environment& e)
	: WindowReference(h), env(e)
{
	YAssert(::IsWindow(h), "Invalid window handle found.");
	YAssert(::GetWindowThreadProcessId(h, nullptr) == ::GetCurrentThreadId(),
		"Window not created on current thread found.");
	YAssert(::GetWindowLongPtrW(h, GWLP_USERDATA) == 0,
		"Invalid user data of window found.");

	wchar_t buf[ystdex::arrlen(WindowClassName)];

	if(YB_UNLIKELY(!::GetClassNameW(hWindow, buf,
		ystdex::arrlen(WindowClassName))))
		YF_Raise_Win32Exception("GetClassNameW");
	if(std::wcscmp(buf, WindowClassName) != 0)
		throw LoggedEvent("Wrong windows class name found.");
	::SetLastError(0);
	if(YB_UNLIKELY(::SetWindowLongPtrW(hWindow, GWLP_USERDATA,
		::LONG_PTR(this)) == 0&& GetLastError() != 0))
		YF_Raise_Win32Exception("SetWindowLongPtrW");
	if(YB_UNLIKELY(!::SetWindowPos(hWindow, nullptr, 0, 0, 0, 0, SWP_NOACTIVATE
		| SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_NOSENDCHANGING
		| SWP_NOSIZE | SWP_NOZORDER)))
		YF_Raise_Win32Exception("SetWindowPos");

	::RAWINPUTDEVICE rid{0x01, 0x02, 0, nullptr};

	if(YB_UNLIKELY(!::RegisterRawInputDevices(&rid, 1, sizeof(rid))))
		YF_Raise_Win32Exception("RegisterRawInputDevices");
	e.AddMappedItem(hWindow, this);
}
Window::~Window()
{
	::SetWindowLongPtrW(hWindow, GWLP_USERDATA, ::LONG_PTR());
	env.get().RemoveMappedItem(hWindow);
	// Note: The window could be already destroyed in window procedure.
	if(::IsWindow(hWindow))
		::DestroyWindow(hWindow);
}

pair<Point, Point>
Window::GetInputBounds() const ynothrow
{
	::RECT rect;

	if(YB_UNLIKELY(!::GetClientRect(hWindow, &rect)))
		return {};

	YAssert(rect.right - rect.left >= 0 && rect.bottom - rect.top >= 0,
		"Invalid boundary found.");

	return {Point(rect.left, rect.top), Point(rect.right, rect.bottom)};
}

void
Window::OnDestroy()
{
	::PostQuitMessage(0);
}

void
Window::OnLostFocus()
{
	platform_ex::ClearKeyStates();
}

void
Window::OnPaint()
{}
#	endif

YSL_END_NAMESPACE(Host)
#endif

YSL_END

