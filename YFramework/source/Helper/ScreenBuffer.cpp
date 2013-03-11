/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ScreenBuffer.cpp
\ingroup Helper
\brief 屏幕缓冲区。
\version r101
\author FrankHB <frankhb1989@gmail.com>
\since build 387
\par 创建时间:
	2013-03-08 11:34:28 +0800
\par 修改时间:
	2013-03-10 18:27 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::ScreenBuffer
*/


#include "ScreenBuffer.h"
#include "Helper/ShellHelper.h" // for YCL_DEBUG_PUTS, YSL_DEBUG_DECL_TIMER;

YSL_BEGIN

using namespace Drawing;

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)
#	if YCL_MINGW32
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
ScreenRegionBuffer::UpdateFrom(BitmapPtr buf) ynothrow
{
	YSL_DEBUG_DECL_TIMER(tmr, "ScreenRegionBuffer::UpdateFrom")
	std::lock_guard<std::mutex> lck(mtx);

	ScreenBuffer::UpdateFrom(buf);
}

void
ScreenRegionBuffer::UpdateTo(::HWND h_wnd, const Point& pt) ynothrow
{
	YSL_DEBUG_DECL_TIMER(tmr, "ScreenRegionBuffer::UpdateTo")
	std::lock_guard<std::mutex> lck(mtx);
	GSurface<> sf(h_wnd);

//	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	sf.Update(*this, pt);
}


void
WindowMemorySurface::Update(ScreenBuffer& sbuf, const Point& pt) ynothrow
{
	const auto h_old(::SelectObject(h_mem_dc, sbuf.GetNativeHandle()));
	const auto& s(sbuf.GetSize());

	// NOTE: Unlocked intentionally for performance.
	::BitBlt(h_owner_dc, pt.X, pt.Y, s.Width, s.Height, h_mem_dc, 0, 0,
		SRCCOPY);
	::SelectObject(h_mem_dc, h_old);
}
#	endif
YSL_END_NAMESPACE(Host)
#endif

YSL_END

