/*
	© 2014 FrankHB.

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
\version r102
\author FrankHB <frankhb1989@gmail.com>
\since build 502
\par 创建时间:
	2014-06-04 22:53:58 +0800
\par 修改时间:
	2014-06-09 10:49 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper_(Android)::AndroidScreen
*/


#include "AndroidScreen.h"
#include YFM_YSLib_UI_YComponent // for to_string;

namespace YSLib
{

using namespace Drawing;

namespace Devices
{

AndroidScreen::AndroidScreen(::ANativeWindow& wnd)
	: AndroidScreen(wnd, [&wnd]()->Size{
		// XXX: Surface might be 1x1.
		// See http://grokbase.com/t/gg/android-ndk/123ks0p3n1/resizing-nativeactivity-and-opengl-surfaces .
		// See http://www.klayge.org/2013/01/23/%E5%9C%A8android%E6%A8%A1%E6%8B%9F%E5%99%A8%E4%B8%8A%E7%9A%84%E4%B8%80%E4%BA%9B%E5%B0%8F%E9%99%B7%E9%98%B1/
		// TODO: Use direct window size query when API level >= 14.
		const auto& size(Host::WindowReference(&wnd).GetSize());

		if(size.Width > 1)
			return size;

		::ANativeWindow_Buffer abuf;
		::ANativeWindow_lock(&wnd, &abuf, {});
		::ANativeWindow_unlockAndPost(&wnd);

		return {CheckPositiveScalar<SDst>(abuf.width, "width"),
			CheckPositiveScalar<SDst>(abuf.height, "height")};
	}())
{}
AndroidScreen::AndroidScreen(::ANativeWindow& wnd, const Drawing::Size& size)
	: Screen(size),
	window_ref(wnd), rbuf(Drawing::Size(size))
{
	YTraceDe(Informative, "Screen created, size = %s.",
		to_string(size).c_str());
	pBuffer = rbuf.GetBufferPtr();
	::ANativeWindow_setBuffersGeometry(&wnd, 0, 0, WINDOW_FORMAT_RGBA_8888);
}

void
AndroidScreen::Update(Drawing::BitmapPtr p_buf) ynothrow
{
	rbuf.UpdateFrom(p_buf);
	rbuf.UpdateTo(&GetWindowRef(), Offset);
}

} // namespace Devices;

} // namespace YSLib;

