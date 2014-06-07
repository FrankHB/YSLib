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
\version r77
\author FrankHB <frankhb1989@gmail.com>
\since build 502
\par 创建时间:
	2014-06-04 22:53:58 +0800
\par 修改时间:
	2014-06-07 13:39 +0800
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

namespace Android
{

Drawing::Size
FetchScreenSize()
{
	// FIXME: Real implementation.
	return {480, 800};
}

} // namespace Android;

} // namespace YSLib;

