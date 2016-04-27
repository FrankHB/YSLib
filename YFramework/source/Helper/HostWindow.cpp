/*
	© 2013-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostWindow.cpp
\ingroup Helper
\brief 宿主环境窗口。
\version r661
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-18 18:18:46 +0800
\par 修改时间:
	2016-04-27 23:27 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostWindow
*/


#include "Helper/YModules.h"
#include YFM_Helper_Environment // for Host::Window, ::RAWINPUT, WM_*,
//	ystdex::pun_storage_t, ystdex::replace_cast;
#include YFM_Helper_GUIApplication // for FetchGUIHost;
#include YFM_YCLib_Input // for platform::ClearKeyStates;
#if YCL_Win32
#	include YFM_YSLib_UI_YUIContainer // for UI::FetchTopLevel;
#endif

namespace YSLib
{

using namespace Drawing;
using namespace UI;

#if YF_Hosted
namespace Host
{

Window::Window(NativeWindowHandle h_wnd)
	: Window(h_wnd, FetchGUIHost())
{}
Window::Window(NativeWindowHandle h_wnd, GUIHost& h)
	: HostWindow(h_wnd), host(h)
#if YCL_Win32
	, InputHost(*this)
#endif
{
	h.AddMappedItem(h_wnd, make_observer(this));
}
Window::~Window()
{
	host.get().RemoveMappedItem(GetNativeHandle());
}

void
Window::UpdateFrom(Drawing::ConstBitmapPtr p, ScreenBuffer& buf)
{
	const auto h_wnd(GetNativeHandle());

#	if YCL_Win32
	if(UseOpacity)
	{
		buf.Premultiply(p);
		buf.UpdatePremultipliedTo(h_wnd, Opacity);
	}
	else
#	endif
	{
		buf.UpdateFrom(p);
		buf.UpdateTo(h_wnd);
	}
}

#	if YCL_Win32
void
Window::UpdateFromBounds(Drawing::ConstBitmapPtr p, ScreenBuffer& buf,
	const Rect& r, const Point& sp)
{
	const auto h_wnd(GetNativeHandle());

	if(UseOpacity)
	{
		buf.Premultiply(p);
		buf.UpdatePremultipliedTo(h_wnd, Opacity);
	}
	else
	{
		buf.UpdateFromBounds(p, r);
		buf.UpdateToBounds(h_wnd, r, sp);
	}
}

void
Window::UpdateTextInputFocus(IWidget& wgt, const Point& pt)
{
	auto loc(pt);
	auto& top_level(FetchTopLevel(wgt, loc));

	yunused(top_level);
	InputHost.UpdateCandidateWindowLocation(loc);
}
#	endif

} // namespace Host;
#endif

} // namespace YSLib;

