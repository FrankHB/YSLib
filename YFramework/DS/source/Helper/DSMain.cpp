﻿/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSMain.cpp
\ingroup Helper
\ingroup DS
\brief DS 平台框架。
\version r3204
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2012-03-25 12:48:49 +0800
\par 修改时间:
	2015-05-10 10:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper_(DS)::DSMain
*/


#include "DSScreen.h"
#include YFM_Helper_HostRenderer // for Host::WindowThread;
#include YFM_Helper_Initialization
#include YFM_YSLib_Adaptor_Font
#include "DSWindow.h"
#if YCL_MULTITHREAD == 1
#	include <thread> // for std::this_thread::*;
#endif
#ifdef YCL_DS
#	include YFM_YSLib_Service_YBlit // for Drawing::FillPixel;
#elif YCL_Android
#	include YFM_Android_Helper_AndroidHost // for
//	Android::FetchNativeHostInstance, Android::FetchDefaultWindow;
#endif
#include YFM_YCLib_Debug
#include YFM_Helper_Environment

namespace YSLib
{

using namespace Drawing;

namespace
{

#if YCL_Win32
yconstexpr double g_max_free_fps(1000);
std::chrono::nanoseconds host_sleep(std::uint64_t(1000000000 / g_max_free_fps));
#endif


//! \since build 499
DSApplication* ApplicationPtr;

} // unnamed namespace;


DSApplication::DSApplication()
	: GUIApplication(), DSVideoState(),
	scrs()
#if YCL_Win32
	, p_wnd_thrd()
#endif
{
	using Devices::DSScreen;

	YAssert(!ApplicationPtr, "Duplicate instance found.");
	ApplicationPtr = this;
#if YCL_DS
	FetchDefaultFontCache();
	InitVideo();
#endif
	Devices::InitDSScreen(scrs[0], scrs[1]);

	auto& scr0(Deref(scrs[0]));
	auto& scr1(Deref(scrs[1]));

#if YCL_DS
	FillPixel<Pixel>(scr0.GetCheckedBufferPtr(),
		GetAreaOf(scr0.GetSize()), ColorSpace::Blue),
	FillPixel<Pixel>(scr1.GetCheckedBufferPtr(),
		GetAreaOf(scr1.GetSize()), ColorSpace::Green);
#elif YCL_Win32

	using namespace Host;

	p_wnd_thrd.reset(new WindowThread([this]{
		return unique_ptr<Window>(new DSWindow(CreateNativeWindow(
			WindowClassName, {256, 384}, L"YSTest", WS_TILED | WS_CAPTION
			| WS_SYSMENU | WS_MINIMIZEBOX), Deref(scrs[0]), Deref(scrs[1]),
			GetEnvironmentRef()));
	}));
	while(!p_wnd_thrd->GetWindowPtr())
		// TODO: Resolve magic sleep duration.
		std::this_thread::sleep_for(host_sleep);

	const auto h_wnd(p_wnd_thrd->GetWindowPtr()->GetNativeHandle());

	GetEnvironmentRef().MapPoint = [this](const Point& pt){
		return GetEnvironmentRef().MapTopLevelWindowPoint(pt);
	};
#elif YCL_Android
	const auto h_wnd(&Android::FetchDefaultWindow());
//	auto& host(Android::FetchNativeHostInstance());

//	host.ResizeScreen({MainScreenWidth, MainScreenHeight << 1}),
	GetEnvironmentRef().MapPoint = [](const Point& pt) ynothrow{
		// XXX: Use alternative implementation rather than Win32's.
		yconstexpr const Rect
			bounds(0, MainScreenHeight, MainScreenWidth, MainScreenHeight << 1);

		return bounds.Contains(pt) ? pt - bounds.GetPoint() : Point::Invalid;
	};
#endif
#if YF_Hosted

	yunseq(scr0.WindowHandle = h_wnd, scr1.WindowHandle = h_wnd);
#endif
}

DSApplication::~DSApplication()
{
#if YCL_Win32
	p_wnd_thrd.reset();
	YTraceDe(Notice, "Host thread dropped.");
#endif
//	hShell = {};
	reset(scrs[0]),
	reset(scrs[1]);
	ApplicationPtr = {};
}

Devices::DSScreen&
DSApplication::GetDSScreenUp() const ynothrow
{
	return Deref(scrs[0]);
}
Devices::DSScreen&
DSApplication::GetDSScreenDown() const ynothrow
{
	return Deref(scrs[1]);
}
Devices::Screen&
DSApplication::GetScreenUp() const ynothrow
{
	return GetDSScreenUp();
}
Devices::Screen&
DSApplication::GetScreenDown() const ynothrow
{
	return GetDSScreenDown();
}

void
DSApplication::SwapScreens()
{
	UI::FetchGUIState().Reset();
	SwapLCD();
#if YF_Hosted
	std::swap(GetDSScreenUp().Offset, GetDSScreenDown().Offset);
#	if !YCL_Android
	if(const auto p_wnd = GetEnvironmentRef().GetForegroundWindow())
		p_wnd->Invalidate();
#	endif
#endif
}


bool
#if YCL_DS
InitConsole(Devices::Screen& scr, Drawing::Pixel fc, Drawing::Pixel bc)
{
	using namespace platform_ex;

	if(&FetchGlobalInstance<DSApplication>().GetScreenUp() == &scr)
		DSConsoleInit(true, fc, bc);
	else if(&FetchGlobalInstance<DSApplication>().GetScreenDown() == &scr)
		DSConsoleInit(false, fc, bc);
	else
		return {};
#else
InitConsole(Devices::Screen&, Drawing::Pixel, Drawing::Pixel)
{
#endif
	return true;
}

#if YCL_Win32
namespace Windows
{

void
TestFramework(size_t idx)
{
	YTraceDe(Notice, "%s",
		("Test began, idx = " + to_string(idx) + " .").c_str());
	YTraceDe(Notice, "Test ended.");
	yunused(idx);
}

} // namespace MinGW32;
#endif

} // namespace YSLib;

