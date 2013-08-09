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
\version r338
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-18 18:18:46 +0800
\par 修改时间:
	2013-08-08 20:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostWindow
*/


#include "Helper/Host.h"
#include "Helper/GUIApplication.h" // for FetchEnvironent;
#include "YCLib/Input.h" // for platform::ClearKeyStates;

namespace YSLib
{

using namespace Drawing;

#if YCL_HOSTED
namespace Host
{

#	if YCL_MinGW32
Window::Window(NativeWindowHandle h)
	: Window(h, FetchEnvironment())
{}
Window::Window(NativeWindowHandle h, Environment& e)
	: HostWindow(h), env(e)
{
	e.AddMappedItem(h, this);
}
Window::~Window()
{
	env.get().RemoveMappedItem(GetNativeHandle());
}

pair<Point, Point>
Window::GetInputBounds() const ynothrow
{
	::RECT rect;

	if(YB_UNLIKELY(!::GetClientRect(GetNativeHandle(), &rect)))
		return {};

	YAssert(rect.right - rect.left >= 0 && rect.bottom - rect.top >= 0,
		"Invalid boundary found.");

	return {Point(rect.left, rect.top), Point(rect.right, rect.bottom)};
}

void
Window::OnLostFocus()
{
	platform_ex::ClearKeyStates();
}

void
Window::UpdateFrom(YSLib::Drawing::BitmapPtr buf, ScreenRegionBuffer& rbuf)
{
	const auto h_wnd(GetNativeHandle());

	if(UseOpacity)
	{
		rbuf.Premultiply(buf);
		rbuf.UpdatePremultipliedTo(h_wnd, Opacity);
	}
	else
	{
		rbuf.UpdateFrom(buf);
		rbuf.UpdateTo(h_wnd);
	}
}
#	endif

} // namespace Host;
#endif

} // namespace YSLib;

