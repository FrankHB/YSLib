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
\version r148
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-18 18:18:46 +0800
\par 修改时间:
	2013-05-05 21:11 +0800
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

namespace
{

#if YCL_MINGW32
//! \since build 388
void
ResizeWindow(::HWND h_wnd, SDst w, SDst h)
{
	::SetWindowPos(h_wnd, nullptr, 0, 0, w, h,
		SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE
		| SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER);
}
#endif

} // unnamed namespace;


Window::Window(NativeWindowHandle h, Environment& e)
	: env(e), h_wnd(h)
{
	YAssert(::IsWindow(h), "Invalid window handle found.");
	YAssert(::GetWindowThreadProcessId(h, nullptr) == ::GetCurrentThreadId(),
		"Window not created on current thread found.");
	YAssert(::GetWindowLongPtrW(h, GWLP_USERDATA) == 0,
		"Invalid user data of window found.");

	wchar_t buf[ystdex::arrlen(WindowClassName)];

	::GetClassName(h_wnd, buf, ystdex::arrlen(WindowClassName));
	if(std::wcscmp(buf, WindowClassName) != 0)
		throw LoggedEvent("Wrong windows class name found.");
	::SetWindowLongPtrW(h_wnd, GWLP_USERDATA, ::LONG_PTR(this));
	::SetWindowPos(h_wnd, nullptr, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE
		| SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOSIZE
		| SWP_NOZORDER);
	e.AddMappedItem(h_wnd, this);
}
Window::~Window()
{
	::SetWindowLongPtrW(h_wnd, GWLP_USERDATA, ::LONG_PTR());
	env.get().RemoveMappedItem(h_wnd);
	// Note: The window could be already destroyed in window procedure.
	if(::IsWindow(h_wnd))
		::DestroyWindow(h_wnd);
}

pair<Point, Point>
Window::GetInputBounds() const ynothrow
{
	::RECT rect;

	::GetClientRect(h_wnd, &rect);

	YAssert(rect.right - rect.left >= 0 && rect.bottom - rect.top >= 0,
		"Invalid boundary found.");

	return {Point(rect.left, rect.top), Point(rect.right, rect.bottom)};
}

void
Window::Close()
{
	::SendNotifyMessageW(h_wnd, WM_CLOSE, 0, 0);
}

void
Window::Resize(const Size& s)
{
	ResizeWindow(GetNativeHandle(), s.Width, s.Height);
}

void
Window::ResizeClient(const Size& s)
{
	const auto h_wnd(GetNativeHandle());
	::RECT rect{0, 0, s.Width, s.Height};

	::AdjustWindowRect(&rect, ::GetWindowLongW(h_wnd, GWL_STYLE), FALSE);
	ResizeWindow(h_wnd, rect.right - rect.left, rect.bottom - rect.top);
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

void
Window::Show() ynothrow
{
	::ShowWindowAsync(h_wnd, SW_SHOWNORMAL);
}

YSL_END_NAMESPACE(Host)
#endif

YSL_END

