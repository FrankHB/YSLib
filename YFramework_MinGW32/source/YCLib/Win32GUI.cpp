/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Win32GUI.cpp
\ingroup YCLib
\ingroup MinGW32
\brief Win32 GUI 接口。
\version r126
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 11:31:05 +0800
\par 修改时间:
	2013-07-11 07:17 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::Win32GUI
*/


#include "YCLib/Win32GUI.h"

using namespace YSLib;
using namespace Drawing;

namespace platform_ex
{

namespace Windows
{

namespace
{

//! \since build 388
void
ResizeWindow(::HWND h_wnd, SDst w, SDst h)
{
	if(YB_UNLIKELY(!::SetWindowPos(h_wnd, nullptr, 0, 0, w, h,
		SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE
		| SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER)))
		YF_Raise_Win32Exception("SetWindowPos");
}

//! \since build 427
::RECT
FetchWindowRect(::HWND h_wnd)
{
	::RECT rect;

	if(YB_UNLIKELY(!::GetWindowRect(h_wnd, &rect)))
		YF_Raise_Win32Exception("GetWindowRect");
	return rect;
}

} // unnamed namespace;


Size
WindowReference::GetSize() const
{
	const auto& rect(FetchWindowRect(hWindow));

	YAssert(rect.right - rect.left >= 0 && rect.bottom - rect.top >= 0,
		"Invalid boundary found.");

	return {rect.right - rect.left, rect.bottom - rect.top};
}
Point
WindowReference::GetLocation() const
{
	const auto& rect(FetchWindowRect(hWindow));

	return {rect.left, rect.top};
}

void
WindowReference::Close()
{
	if(YB_UNLIKELY(!::SendNotifyMessageW(hWindow, WM_CLOSE, 0, 0)))
		YF_Raise_Win32Exception("SendNotifyMessageW");
}

void
WindowReference::Move(const Point& pt)
{
	if(YB_UNLIKELY(!::SetWindowPos(hWindow, nullptr, pt.X, pt.Y, 0, 0,
		SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOREDRAW
		| SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOZORDER)))
		YF_Raise_Win32Exception("SetWindowPos");
}

void
WindowReference::Resize(const Size& s)
{
	ResizeWindow(hWindow, s.Width, s.Height);
}

void
WindowReference::ResizeClient(const Size& s)
{
	::RECT rect{0, 0, s.Width, s.Height};

	if(YB_UNLIKELY(!::AdjustWindowRect(&rect,
		::GetWindowLongW(hWindow, GWL_STYLE), false)))
		YF_Raise_Win32Exception("AdjustWindowRect");
	ResizeWindow(hWindow, rect.right - rect.left, rect.bottom - rect.top);
}

bool
WindowReference::Show() ynothrow
{
	return ::ShowWindowAsync(hWindow, SW_SHOWNORMAL) != 0;
}

} // namespace Windows;

} // namespace platform_ex;

