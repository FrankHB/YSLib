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
\version r271
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 18:30:24 +0800
\par 修改时间:
	2014-05-18 23:44 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Android)::Android
*/


#include "YCLib/YModules.h"
#include YFM_Android_YCLib_Android
#include <android/native_activity.h>
#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_YGDI

using namespace YSLib;
using namespace Drawing;

namespace platform_ex
{

namespace Android
{

namespace
{

//! \since build 498
template<typename _tDst, typename _type>
inline _tDst
CheckScalar(_type val, const std::string& name)
{
	if(YB_UNLIKELY(val > std::numeric_limits<_tDst>::max()))
		throw Exception(name + " value out of range.");
	return _tDst(val);
}

//! \since build 498
template<typename _tDst, typename _type>
inline _tDst
CheckPositiveScalar(_type val, const std::string& name)
{
	if(val < 0)
		// XXX: Use more specified exception type.
		throw Exception("Failed getting " + name + " value.");
	return CheckScalar<_tDst>(val, name);
}

//! \since build 498
SDst
CheckStride(SDst buf_stride, SDst w)
{
	if(buf_stride < w)
		// XXX: Use more specified exception type.
		throw Exception("Stride is small than width");
	return buf_stride;
}

} // unnamed namespace;


SDst
WindowReference::GetWidth() const
{
	return
		CheckPositiveScalar<SDst>(::ANativeWindow_getWidth(hWindow), "width");
}
SDst
WindowReference::GetHeight() const
{
	return
		CheckPositiveScalar<SDst>(::ANativeWindow_getHeight(hWindow), "height");
}


HostWindow::HostWindow(NativeWindowHandle h_wnd)
	: WindowReference(h_wnd)
{
	::ANativeWindow_acquire(h_wnd);
}
HostWindow::~HostWindow()
{
	::ANativeWindow_release(GetNativeHandle());
}


class ScreenBufferData : public CompactPixmap
{
public:
	ScreenBufferData(const Size&, SDst);

	DefDeMoveCtor(ScreenBufferData)
};

ScreenBufferData::ScreenBufferData(const Size& s, SDst buf_stride)
	: CompactPixmap({}, CheckStride(buf_stride, s.Width), s.Height)
{}


ScreenBuffer::ScreenBuffer(const Size& s)
	: ScreenBuffer(s, s.Width)
{}
ScreenBuffer::ScreenBuffer(const Size& s, SDst buf_stride)
	: p_impl(new ScreenBufferData(s, buf_stride)), width(s.Width)
{}
ScreenBuffer::ScreenBuffer(ScreenBuffer&& sbuf) ynothrow
	: p_impl(new ScreenBufferData(std::move(*sbuf.p_impl))), width(sbuf.width)
{
	sbuf.width = 0;
}
ScreenBuffer::~ScreenBuffer()
{}

BitmapPtr
ScreenBuffer::GetBufferPtr() const ynothrow
{
	YAssertNonnull(p_impl);

	return p_impl->GetBufferPtr();
}
Size
ScreenBuffer::GetSize() const ynothrow
{
	YAssertNonnull(p_impl);

	return {width, p_impl->GetHeight()};
}
YSLib::SDst
ScreenBuffer::GetStride() const ynothrow
{
	YAssertNonnull(p_impl);

	return p_impl->GetWidth();
}

void
ScreenBuffer::Resize(const Size& s)
{
	// TODO: Expand stride for given width using a proper strategy.
	YAssertNonnull(p_impl);

	p_impl->SetSize(s);
	width = s.Width;
}

void
ScreenBuffer::UpdateFrom(BitmapPtr p_buf) ynothrow
{
	// TODO: Expand stride for given width using a proper strategy.
	YAssertNonnull(p_impl);

	std::copy_n(p_buf, GetAreaOf(GetSize()), p_impl->GetBufferPtr());
}

void
ScreenBuffer::swap(ScreenBuffer& sbuf) ynothrow
{
	YAssertNonnull(p_impl);

	p_impl->swap(*sbuf.p_impl),
	std::swap(width, sbuf.width);
}


ScreenRegionBuffer::ScreenRegionBuffer(const Size& s)
	: ScreenRegionBuffer(s, s.Width)
{}
ScreenRegionBuffer::ScreenRegionBuffer(const Size& s, SDst buf_stride)
	: ScreenBuffer(s, buf_stride),
	mtx()
{}

void
ScreenRegionBuffer::UpdateFrom(BitmapPtr buf) ynothrow
{
	std::lock_guard<std::mutex> lck(mtx);

	ScreenBuffer::UpdateFrom(buf);
}

void
ScreenRegionBuffer::UpdateTo(NativeWindowHandle h_wnd, const Point& pt) ynothrow
{
	YAssertNonnull(h_wnd);

	const Size& s(GetSize());
	::ANativeWindow_Buffer abuf{s.Width, s.Height, GetStride(),
		WINDOW_FORMAT_RGBA_8888, GetBufferPtr(), {0, 0, 0, 0, 0, 0}};
	::ARect arect{pt.X, pt.Y, pt.X + s.Width, pt.Y + s.Height};
	std::lock_guard<std::mutex> lck(mtx);

	::ANativeWindow_lock(h_wnd, &abuf, &arect);
	::ANativeWindow_unlockAndPost(h_wnd);
}

} // namespace Android;

} // namespace YSLib;

extern "C" void
ANativeActivity_onCreate(::ANativeActivity* p_activity, void*, ::size_t)
{
#ifndef NDEBUG
	platform::FetchCommonLogger().FilterLevel = platform::Descriptions::Debug;
#endif
	YTraceDe(Debug, "Creating: %p\n", static_cast<void*>(p_activity));
	YTraceDe(Notice, "YSLib test succeeded.");
}

