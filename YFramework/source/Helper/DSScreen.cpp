/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSScreen.cpp
\ingroup Helper
\brief DS 屏幕。
\version r182
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:27:29 +0800
\par 修改时间:
	2013-03-07 12:11 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::DSScreen
*/


#include "DSScreen.h"
#include "Helper/ShellHelper.h" // for YCL_DEBUG_PUTS, YSL_DEBUG_DECL_TIMER;
#include "Host.h"
#ifdef YCL_DS
#	include "YSLib/Service/yblit.h" // for Drawing::FillPixel;
#endif

YSL_BEGIN

using Devices::DSScreen;
using namespace Drawing;

YSL_BEGIN_NAMESPACE(Host)
#if YCL_MINGW32
ScreenBuffer::ScreenBuffer(const Size& s)
	: size(s), hBitmap([this]{
		::BITMAPINFO bmi{{sizeof(::BITMAPINFO::bmiHeader), size.Width,
			-size.Height - 1, 1, 32, BI_RGB,
			sizeof(PixelType) * size.Width * size.Height, 0, 0, 0, 0}, {}};

		return ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS,
			&reinterpret_cast<void*&>(pBuffer), NULL, 0);
	}())
{}
ScreenBuffer::ScreenBuffer(ScreenBuffer&& sbuf) ynothrow
	: size(sbuf.size), hBitmap(sbuf.hBitmap)
{
	sbuf.hBitmap = NULL;
}
ScreenBuffer::~ScreenBuffer()
{
	::DeleteObject(hBitmap);
}

void
ScreenBuffer::UpdateFrom(BitmapPtr buf) ynothrow
{
	std::copy_n(buf, size.Width * size.Height, GetBufferPtr());
}


void
WindowMemorySurface::Update(const ScreenBuffer& sbuf, const Point& pt) ynothrow
{
	const auto h_old(::SelectObject(h_mem_dc, sbuf.GetNativeHandle()));
	const auto& s(sbuf.GetSize());

	// NOTE: Unlocked intentionally for performance.
	::BitBlt(h_owner_dc, pt.X, pt.Y, s.Width, s.Height, h_mem_dc, 0, 0,
		SRCCOPY);
	::SelectObject(h_mem_dc, h_old);
}
#endif
YSL_END_NAMESPACE(Host)


YSL_BEGIN_NAMESPACE(Devices)

#if YCL_DS
DSScreen::DSScreen(bool b) ynothrow
	: Devices::Screen(MainScreenWidth, MainScreenHeight)
{
	pBuffer = (b ? DS::InitScrDown : DS::InitScrUp)(bg);
}

void
DSScreen::Update(BitmapPtr buf) ynothrow
{
	DS::ScreenSynchronize(GetCheckedBufferPtr(), buf);
}
void
DSScreen::Update(Color c)
{
	FillPixel<PixelType>(GetCheckedBufferPtr(), GetAreaOf(GetSize()), c);
}
#elif YCL_MINGW32
DSScreen::DSScreen(bool b) ynothrow
	: Devices::Screen(MainScreenWidth, MainScreenHeight),
	Offset(), env(FetchGlobalInstance().GetHost()),
	gbuf(Size(MainScreenWidth, MainScreenHeight)), update_mutex()
{
	pBuffer = gbuf.GetBufferPtr();
	if(b)
		Offset.Y = MainScreenHeight;
}

void
DSScreen::Update(Drawing::BitmapPtr buf) ynothrow
{
	YSL_DEBUG_DECL_TIMER(tmr, "DSScreen::Update")
	std::lock_guard<std::mutex> lck(update_mutex);

	std::copy_n(buf, size.Width * size.Height, gbuf.GetBufferPtr());
//	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	YCL_DEBUG_PUTS("Screen buffer updated.");
	YSL_DEBUG_DECL_TIMER(tmrx, "DSScreen::Update!UpdateWindow")

	env.get().UpdateWindow(*this);
}
#else
#	error Unsupported platform found!
#endif

YSL_END_NAMESPACE(Devices)

YSL_END

