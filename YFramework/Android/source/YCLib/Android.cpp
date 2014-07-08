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
\version r390
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 18:30:24 +0800
\par 修改时间:
	2014-07-06 02:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Android)::Android
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YCL_Android
#	include YFM_Android_YCLib_Android
#	include <android/native_activity.h>
#	include <android/configuration.h>
#endif
#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_YGDI
#include YFM_YCLib_Input // for platform::SaveInput;
#include YFM_YSLib_Core_YCoreUtilities // for YSLib::CheckPositiveScalar;

using namespace YSLib;
using namespace Drawing;

namespace platform_ex
{

#if YCL_Android

namespace Android
{

namespace
{

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
const YSLib::Drawing::Graphics&
ScreenBuffer::GetContext() const ynothrow
{
	YAssertNonnull(p_impl);
	return p_impl->GetContext();
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
	YAssertNonnull(p_buf),
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
	::ANativeWindow_Buffer abuf;
	::ARect arect{pt.X, pt.Y, pt.X + s.Width, pt.Y + s.Height};
	std::lock_guard<std::mutex> lck(mtx);

	::ANativeWindow_lock(h_wnd, &abuf, &arect);
	CopyTo(static_cast<BitmapPtr>(abuf.bits), GetContext(),
		WindowReference(h_wnd).GetSize(), pt, {}, s);
	::ANativeWindow_unlockAndPost(h_wnd);
}


InputQueue::InputQueue(::ALooper& looper, ::AInputQueue& q)
	: queue_ref(q)
{
	YTraceDe(Debug, "Attaching input queue to looper.");
	::AInputQueue_attachLooper(&q, &looper, ALOOPER_POLL_CALLBACK,
		[](int, int, void* p_data){
			YAssertNonnull(p_data);

			auto& q(*static_cast<InputQueue*>(p_data));
			::AInputEvent* p_evt{};
			const auto p_queue(&q.queue_ref.get());

			while(::AInputQueue_getEvent(p_queue, &p_evt) >= 0)
			{
				YAssertNonnull(p_evt);
				YTraceDe(Debug, "New input event: type = %d.",
					::AInputEvent_getType(p_evt));
				if(::AInputQueue_preDispatchEvent(p_queue, p_evt))
					continue;
				SaveInput(*p_evt);
				::AInputQueue_finishEvent(p_queue, p_evt,
					::AInputEvent_getType(p_evt) == AINPUT_EVENT_TYPE_MOTION);
			}
			return 1;
		}, this);
}
InputQueue::~InputQueue()
{
	::AInputQueue_detachLooper(&queue_ref.get());
}


::ALooper&
FetchNativeLooper(bool allow_non_callbacks) ythrow(Exception)
{
	const auto p(::ALooper_prepare(allow_non_callbacks
		? ALOOPER_PREPARE_ALLOW_NON_CALLBACKS : 0));

	if(YB_LIKELY(p))
		return *p;
	throw Exception("Failed get native looper pointer.");
}

void
TraceConfiguration(::AConfiguration& cfg, platform::Logger::Level lv)
{
	char lang[2], country[2];

	::AConfiguration_getLanguage(&cfg, lang);
	::AConfiguration_getCountry(&cfg, country);
	YTraceDe(lv, "Configuration: MCC = %d, MNC = %d, language code = %c%c,"
		" country = %c%c, orientation = %d, touch screen = %d, density = %d, "
		" keyboard = %d, navigation = %d, keysHidden = %d, navHidden = %d,"
		" SDK version = %d, screen size = %d, screen long = %d,"
		" UI mode type = %d, UI mode night = %d.",
		int(::AConfiguration_getMcc(&cfg)), int(::AConfiguration_getMnc(&cfg)),
		lang[0], lang[1], country[0], country[1],
		int(::AConfiguration_getOrientation(&cfg)),
		int(::AConfiguration_getTouchscreen(&cfg)),
		int(::AConfiguration_getDensity(&cfg)),
		int(::AConfiguration_getKeyboard(&cfg)),
		int(::AConfiguration_getNavigation(&cfg)),
		int(::AConfiguration_getKeysHidden(&cfg)),
		int(::AConfiguration_getNavHidden(&cfg)),
		int(::AConfiguration_getSdkVersion(&cfg)),
		int(::AConfiguration_getScreenSize(&cfg)),
		int(::AConfiguration_getScreenLong(&cfg)),
		int(::AConfiguration_getUiModeType(&cfg)),
		int(::AConfiguration_getUiModeNight(&cfg)));
}

} // namespace Android;

#endif

} // namespace platform_ex;

