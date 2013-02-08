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
\version r131
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:27:29 +0800
\par 修改时间:
	2013-02-08 02:49 +0800
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
	: hBitmap(InitializeDIB(reinterpret_cast<void*&>(pBuffer),
	s.Width, s.Height))
{}
ScreenBuffer::~ScreenBuffer()
{
	::DeleteObject(hBitmap);
}

::HBITMAP
ScreenBuffer::InitializeDIB(void*& pBuffer, SDst w, SDst h)
{
	::BITMAPINFO bmi{{sizeof(::BITMAPINFO::bmiHeader), w,
		-h - 1, 1, 32, BI_RGB, sizeof(PixelType) * w * h, 0, 0, 0, 0}, {}};

	return ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pBuffer,
		NULL, 0);
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
	Offset(), p_host_wnd(FetchGlobalInstance().GetHost().Wait()),
	gbuf(Size(MainScreenWidth, MainScreenHeight)), update_mutex()
{
	pBuffer = gbuf.pBuffer;
	if(b)
		Offset.Y = MainScreenHeight;
}

void
DSScreen::Update(Drawing::BitmapPtr buf) ynothrow
{
	std::lock_guard<std::mutex> lck(update_mutex);

	std::memcpy(gbuf.pBuffer, buf,
		sizeof(PixelType) * size.Width * size.Height);
//	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	YCL_DEBUG_PUTS("Screen buffer updated.");
	YSL_DEBUG_DECL_TIMER(tmr, "DSScreen::Update")

	YAssert(bool(p_host_wnd), "Null pointer found.");

	::HDC hDC(::GetDC(p_host_wnd->GetNativeHandle()));
	::HDC hMemDC(::CreateCompatibleDC(hDC));

	UpdateToHost(hDC, hMemDC);
	::DeleteDC(hMemDC);
	::ReleaseDC(p_host_wnd->GetNativeHandle(), hDC);
}

void
DSScreen::UpdateToHost(::HDC hDC, ::HDC hMemDC) ynothrow
{
	const auto& size(GetSize());

	::SelectObject(hMemDC, gbuf.hBitmap);
	// NOTE: Unlocked intentionally for performance.
	::BitBlt(hDC, Offset.X, Offset.Y, size.Width, size.Height,
		hMemDC, 0, 0, SRCCOPY);
}
#else
#	error Unsupported platform found!
#endif

YSL_END_NAMESPACE(Devices)

YSL_END

