/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ScreenBuffer.h
\ingroup Helper
\brief 屏幕缓冲区。
\version r212
\author FrankHB <frankhb1989@gmail.com>
\since build 387
\par 创建时间:
	2013-03-08 11:34:28 +0800
\par 修改时间:
	2013-06-13 14:23 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::ScreenBuffer
*/


#ifndef Inc_Helper_ScreenBuffer_h_
#define Inc_Helper_ScreenBuffer_h_ 1

#include "YSLib/Core/ygdibase.h"
#include "YCLib/NativeAPI.h"
#if YCL_MULTITHREAD == 1
#	include <mutex>
#endif

YSL_BEGIN

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)
#	if YCL_MINGW32

/*!
\brief 虚拟屏幕缓存。
\note 像素格式和 platform::PixelType 兼容。
\since build 379
*/
class ScreenBuffer
{
private:
	//! \since build 386
	Drawing::Size size;

protected:
	Drawing::BitmapPtr pBuffer;
	::HBITMAP hBitmap;

public:
	ScreenBuffer(const Drawing::Size&);
	//! \since build 386
	ScreenBuffer(ScreenBuffer&&) ynothrow;
	~ScreenBuffer();

	//! \since build 386
	//@{
	DefGetter(const ynothrow, Drawing::BitmapPtr, BufferPtr, pBuffer)
	DefGetter(const ynothrow, ::HBITMAP, NativeHandle, hBitmap)
	DefGetter(const ynothrow, const Drawing::Size&, Size, size)

	/*!
	\brief 从缓冲区更新。
	\warning 直接复制，没有边界和大小检查。
	*/
	void
	UpdateFrom(Drawing::BitmapPtr) ynothrow;
	//@}
};


//! \since build 387
class ScreenRegionBuffer : private ScreenBuffer
{
private:
	std::mutex mtx;

public:
	ScreenRegionBuffer(const Drawing::Size& s)
		: ScreenBuffer(s), mtx()
	{}

	using ScreenBuffer::GetBufferPtr;
	using ScreenBuffer::GetNativeHandle;
	using ScreenBuffer::GetSize;
	DefGetter(ynothrow, ScreenBuffer&, ScreenBufferRef, *this)

	void
	UpdateFrom(Drawing::BitmapPtr) ynothrow;

	void
	UpdateTo(::HWND, const Drawing::Point& = {}) ynothrow;
};


//! \since build 386
//@{
/*!
\brief 窗口内存表面：储存窗口上的二维图形绘制状态。
\note 仅对于内存上下文有所有权。
*/
class WindowMemorySurface
{
private:
	::HDC h_owner_dc, h_mem_dc;

public:
	WindowMemorySurface(::HDC h_dc)
		: h_owner_dc(h_dc), h_mem_dc(::CreateCompatibleDC(h_dc))
	{}
	~WindowMemorySurface()
	{
		::DeleteDC(h_mem_dc);
	}

	DefGetter(const ynothrow, ::HDC, OwnerHandle, h_owner_dc)
	DefGetter(const ynothrow, ::HDC, NativeHandle, h_mem_dc)

	//! \since build 387
	void
	Update(ScreenBuffer&, const Drawing::Point& = {}) ynothrow;
	//! \since build 387
	void
	Update(ScreenRegionBuffer& rbuf, const Drawing::Point& pt = {}) ynothrow
	{
		Update(rbuf.GetScreenBufferRef(), pt);
	}
};


class WindowDeviceContextBase
{
protected:
	::HWND hWindow;
	::HDC hDC;

	WindowDeviceContextBase(::HWND h_wnd, ::HDC h_dc)
		: hWindow(h_wnd), hDC(h_dc)
	{}
	DefDeDtor(WindowDeviceContextBase)

public:
	DefGetter(const ynothrow, ::HDC, DeviceContextHandle, hDC)
	DefGetter(const ynothrow, ::HWND, WindowHandle, hWindow)
};


/*!
\brief 窗口设备上下文。
\note 仅对于设备上下文有所有权。
*/
class WindowDeviceContext : public WindowDeviceContextBase
{
protected:
	WindowDeviceContext(::HWND h_wnd)
		: WindowDeviceContextBase(h_wnd, ::GetDC(h_wnd))
	{}
	~WindowDeviceContext()
	{
		::ReleaseDC(hWindow, hDC);
	}
};


/*!
\brief 窗口区域设备上下文。
\note 仅对于设备上下文有所有权。
*/
class WindowRegionDeviceContext : public WindowDeviceContextBase
{
private:
	::PAINTSTRUCT ps;

protected:
	WindowRegionDeviceContext(::HWND h_wnd)
		: WindowDeviceContextBase(h_wnd, ::BeginPaint(h_wnd, &ps))
	{}
	~WindowRegionDeviceContext()
	{
		::EndPaint(hWindow, &ps);
	}
};


//! \brief 显式区域表面：储存显式区域上的二维图形绘制状态。
template<typename _type = WindowDeviceContext>
class GSurface : public _type, public WindowMemorySurface
{
public:
	GSurface(::HWND h_wnd)
		: _type(h_wnd), WindowMemorySurface(_type::GetDeviceContextHandle())
	{}
};
//@}

#	endif
YSL_END_NAMESPACE(Host)
#endif

YSL_END

#endif

