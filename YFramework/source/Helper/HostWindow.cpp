/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostWindow.cpp
\ingroup Helper
\brief 宿主环境支持的用户界面。
\version r360
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-18 18:18:46 +0800
\par 修改时间:
	2014-05-02 12:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostWindow
*/


#include "Helper/YModules.h"
#include YFM_Helper_Host
#include YFM_Helper_GUIApplication // for FetchEnvironent;
#include YFM_YCLib_Input // for platform::ClearKeyStates;

namespace YSLib
{

using namespace Drawing;

#if YF_Hosted
namespace Host
{


Window::Window(NativeWindowHandle h)
	: Window(h, FetchEnvironment())
{}
Window::Window(NativeWindowHandle h, Environment& e)
	: HostWindow(h), env(e)
{
#	if YCL_Win32
	e.AddMappedItem(h, this);
	MessageMap[WM_KILLFOCUS] += []{
		platform_ex::ClearKeyStates();
	};
#	endif
}
Window::~Window()
{
	env.get().RemoveMappedItem(GetNativeHandle());
}

pair<Point, Point>
Window::GetInputBounds() const ynothrow
{
#	if YCL_Win32
	::RECT rect;

	if(YB_UNLIKELY(!::GetClientRect(GetNativeHandle(), &rect)))
		return {};

	YAssert(rect.right - rect.left >= 0 && rect.bottom - rect.top >= 0,
		"Invalid boundary found.");

	return {Point(rect.left, rect.top), Point(rect.right, rect.bottom)};
#	elif YCL_Android
	return {};
#	endif
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

} // namespace Host;
#endif

} // namespace YSLib;

