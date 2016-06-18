/*
	© 2013-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSWindow.cpp
\ingroup Helper
\ingroup DS
\brief DS 宿主窗口。
\version r177
\author FrankHB <frankhb1989@gmail.com>
\since build 398
\par 创建时间:
	2013-04-11 10:36:43 +0800
\par 修改时间:
	2016-06-19 05:04 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper_(DS)::DSWindow
*/


#include "DSWindow.h"
#include "DSScreen.h" // for GSurface;
#if YCL_Win32
#	include YFM_YSLib_UI_YDesktop // for UI::Desktop;
#	include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;
#endif

namespace YSLib
{

#if YF_Hosted
using Devices::DSScreen;
using namespace Drawing;

namespace Host
{

DSWindow::DSWindow(NativeWindowHandle h_wnd, DSScreen& s_up, DSScreen& s_dn,
	GUIHost& h)
	: Window(h_wnd, h), scr_up(s_up), scr_dn(s_dn)
{
#	if YCL_Win32
	yunseq(
	MessageMap[WM_DESTROY] += []{
		YSLib::PostQuitMessage(0);
		// NOTE: Try to make sure all shells are released before destructing the
		//	instance of %DSApplication.
	},
	MessageMap[WM_PAINT] += [this]{
		GSurface<WindowRegionDeviceContext> sf(Nonnull(GetNativeHandle()));
		const auto& r(sf.GetInvalidatedArea());

		UpdateScreen(sf, scr_up, r);
		UpdateScreen(sf, scr_dn, r);
	}
	);
	Show();
#	endif
}

#	if YCL_Win32
Point
DSWindow::MapPoint(const Point& pt) const
{
	const Rect
		bounds(0, MainScreenHeight, MainScreenWidth, MainScreenHeight << 1);

	return bounds.Contains(pt) ? pt - bounds.GetPoint() : Point::Invalid;
}

void
DSWindow::UpdateTextInputFocus(UI::IWidget& wgt, const Point& pt)
{
	auto loc(pt);
	auto& top_level(UI::FetchTopLevel(wgt, loc));

	YAssert(&ystdex::polymorphic_downcast<const DSScreen&>(
		ystdex::polymorphic_downcast<Desktop&>(top_level)
		.GetScreen()) == &scr_dn, "Wrong screen of text input found");
	yunused(top_level);
	InputHost.UpdateCandidateWindowLocation(loc + Point(0, MainScreenHeight));
}
#	endif

} // namespace Host;
#endif

} // namespace YSLib;

