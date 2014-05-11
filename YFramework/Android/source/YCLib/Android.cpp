/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Android.cpp
\ingroup YCLib
\ingroup Android
\brief YCLib Android 平台公共扩展。
\version r97
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 18:30:24 +0800
\par 修改时间:
	2014-05-11 13:06 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Android)::Android
*/


#include "YCLib/YModules.h"
#include YFM_Android_YCLib_Android
#include <android/log.h>

using namespace YSLib;
using namespace Drawing;

namespace platform_ex
{

namespace Android
{

void
WindowReference::SetClientBounds(const Rect&)
{}

void
WindowReference::Close()
{}

void
WindowReference::Invalidate()
{}

bool
WindowReference::Show() ynothrow
{
	return {};
}

Rect
WindowReference::GetClientBounds() const
{
	return {};
}


HostWindow::HostWindow(NativeWindowHandle)
{}
HostWindow::~HostWindow()
{}


ScreenBuffer::ScreenBuffer(const Size&)
{}
ScreenBuffer::~ScreenBuffer()
{}

void
ScreenBuffer::Premultiply(BitmapPtr) ynothrow
{}

void
ScreenBuffer::Resize(const Size&)
{}


void
ScreenRegionBuffer::UpdateFrom(BitmapPtr) ynothrow
{}

void
ScreenRegionBuffer::UpdatePremultipliedTo(NativeWindowHandle , AlphaType,
	const Point&) ynothrow
{}

void
ScreenRegionBuffer::UpdateTo(NativeWindowHandle, const Point&)
	ynothrow
{}

} // namespace Android;

} // namespace YSLib;

extern "C" void
ANativeActivity_onCreate(ANativeActivity* p_activity, void*, ::size_t)
{
	__android_log_print(ANDROID_LOG_VERBOSE, "YFramework", "Creating: %p\n",
		static_cast<void*>(p_activity));
	__android_log_print(ANDROID_LOG_INFO, "YFramework",
		"YSLib test succeeded.");
}

