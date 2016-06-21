/*
	© 2014-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file AndroidScreen.cpp
\ingroup Helper
\ingroup Android
\brief Android 屏幕。
\version r143
\author FrankHB <frankhb1989@gmail.com>
\since build 502
\par 创建时间:
	2014-06-04 22:53:58 +0800
\par 修改时间:
	2016-06-19 19:25 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper_(Android)::AndroidScreen
*/


#include "AndroidScreen.h" // for Informative, CheckPositive, to_string;

namespace YSLib
{

#if YCL_Android

using namespace Drawing;

namespace Devices
{

AndroidScreen::AndroidScreen(::ANativeWindow& wnd,
	const Drawing::Size& scr_size)
	: AndroidScreen(wnd, [&wnd]() -> Size{
		// XXX: Surface might be 1x1.
		// See http://grokbase.com/t/gg/android-ndk/123ks0p3n1/resizing-nativeactivity-and-opengl-surfaces.
		// See http://www.klayge.org/2013/01/23/%E5%9C%A8android%E6%A8%A1%E6%8B%9F%E5%99%A8%E4%B8%8A%E7%9A%84%E4%B8%80%E4%BA%9B%E5%B0%8F%E9%99%B7%E9%98%B1/.
		// TODO: Use direct window size query when API level >= 14.
		const auto& size(Host::WindowReference(&wnd).GetSize());

		if(size.Width > 1)
			return size;

		::ANativeWindow_Buffer abuf;

		if(YB_UNLIKELY(::ANativeWindow_lock(&wnd, &abuf, {}) != 0))
			throw LoggedEvent("Failed call ::ANativeWindow_lock"
				" @ AndroidScreen::AndroidScreen.");
		::ANativeWindow_unlockAndPost(&wnd);
		return {CheckPositive<SDst>(abuf.width, "width"),
			CheckPositive<SDst>(abuf.height, "height")};
	}(), scr_size)
{}
AndroidScreen::AndroidScreen(::ANativeWindow& wnd, const Size& size,
	const Size& scr_size)
	: Screen(size),
	window_ref(wnd)
{
	YTraceDe(Informative, "Screen created, size = %s.",
		to_string(size).c_str());
	SetNativeBufferSize(scr_size);
}

void
AndroidScreen::SetNativeBufferSize(const Size& s)
{
	::ANativeWindow_setBuffersGeometry(&GetWindowRef(), s.Width, s.Height,
		WINDOW_FORMAT_RGBA_8888);
}

void
AndroidScreen::SetSize(const Size& s)
{
	GraphicDevice::SetSize(s);
	SetNativeBufferSize(s);
}

void
AndroidScreen::Update(ConstBitmapPtr p_buf) ynothrow
{
	const Size& s(GetSize());

	platform_ex::UpdateContentTo(&GetWindowRef(), s, ConstGraphics(p_buf, s));
}

} // namespace Devices;

#endif

} // namespace YSLib;

