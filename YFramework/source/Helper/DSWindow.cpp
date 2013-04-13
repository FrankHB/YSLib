/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSWindow.cpp
\ingroup Helper
\brief DS 宿主窗口。
\version r72
\author FrankHB <frankhb1989@gmail.com>
\since build 398
\par 创建时间:
	2013-04-11 10:36:43 +0800
\par 修改时间:
	2013-04-11 11:08 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::DSWindow
*/


#include "DSWindow.h"
#include "DSScreen.h" // for GSurface;

YSL_BEGIN

#if YCL_HOSTED
using Devices::DSScreen;
using Drawing::Point;

YSL_BEGIN_NAMESPACE(Host)

DSWindow::DSWindow(NativeWindowHandle h, DSScreen& s_up, DSScreen& s_dn,
	Environment& e)
	: Window(h, e), scr_up(s_up), scr_dn(s_dn)
{}

pair<Point, Point>
DSWindow::GetInputBounds() const ynothrow
{
	return {Point(0, MainScreenHeight),
		Point(MainScreenWidth, MainScreenHeight << 1)};
}

void
DSWindow::OnDestroy()
{
	Window::OnDestroy(),
	YSLib::PostQuitMessage(0);
	// NOTE: Try to make sure all shells are released before destructing the
	//	instance of %DSApplication.
}

void
DSWindow::OnPaint()
{
	GSurface<WindowRegionDeviceContext> sf(GetNativeHandle());

	scr_up.UpdateToSurface(sf),
	scr_dn.UpdateToSurface(sf);
}

YSL_END_NAMESPACE(Host)
#endif

YSL_END

