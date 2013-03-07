/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSScreen.h
\ingroup Helper
\brief DS 屏幕。
\version r280
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:28:02 +0800
\par 修改时间:
	2013-03-07 12:21 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::DSScreen
*/


#ifndef Inc_Helper_DSScreen_h_
#define Inc_Helper_DSScreen_h_ 1

#include "Helper/DSMain.h" // for ScreenWidth, ScreenHeight, Host::Environment;
#include "YSLib/Core/ydevice.h"
#if YCL_MULTITHREAD == 1
#	include <mutex>
#endif

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Host)
#if YCL_MINGW32
/*!
\brief 虚拟屏幕缓存。
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
	ScreenBuffer(ScreenBuffer&&) ynoexcept;
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
	UpdateFrom(Drawing::BitmapPtr) ynoexcept;
	//@}
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

	void
	Update(const ScreenBuffer&, const Drawing::Point& = {}) ynothrow;
};


class WindowDeviceContextBase
{
protected:
	::HWND hWindow;
	::HDC hDC;

	WindowDeviceContextBase(::HWND h_wnd, ::HDC h_dc)
		: hWindow(h_wnd), hDC(h_dc)
	{}
	DefEmptyDtor(WindowDeviceContextBase)

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
		: _type(h_wnd), WindowMemorySurface(::GetDC(h_wnd))
	{}
};
//@}
#endif
YSL_END_NAMESPACE(Host)


YSL_BEGIN_NAMESPACE(Devices)

/*!
\brief DS 屏幕。
\since 早于 build 218 。
*/
class DSScreen : public Screen
{
#if YCL_DS
public:
	typedef int BGType;

private:
	BGType bg;

public:
	/*!
	\brief 构造：指定是否为下屏。
	\since build 325
	*/
	DSScreen(bool) ynothrow;

	DefGetter(const ynothrow, const BGType&, BgID, bg)

	/*!
	\brief 更新。
	\note 复制到屏幕。
	\since build 319
	*/
	void
	Update(Drawing::BitmapPtr) ynothrow override;
	/*!
	\brief 更新。
	\note 以纯色填充屏幕。
	*/
	void
	Update(Drawing::Color = Drawing::Color());
#elif YCL_MINGW32
public:
	Drawing::Point Offset;

private:
	/*!
	\brief 宿主环境。
	\since build 380
	*/
	std::reference_wrapper<Host::Environment> env;
	Host::ScreenBuffer gbuf;
	//! \since build 322
	std::mutex update_mutex;

public:
	//! \since build 378
	DSScreen(bool) ynothrow;

	/*!
	\brief 更新。
	\note 复制到屏幕或屏幕缓冲区。
	\note 线程安全。
	\since build 319
	*/
	void
	Update(Drawing::BitmapPtr) ynothrow override;

	//! \since build 386
	template<typename _type>
	void
	UpdateToSurface(_type& sf)
	{
		sf.Update(gbuf, Offset);
	}
#else
#	error Unsupported platform found!
#endif
};


/*!
\brief 初始化 DS 屏幕。
\since build 380
*/
inline void
InitDSScreen(unique_ptr<DSScreen>& p_up, unique_ptr<DSScreen>& p_dn) ynothrow
{
	try
	{
		p_up.reset(new DSScreen(false));
		p_dn.reset(new DSScreen(true));
	}
	catch(...)
	{
		throw LoggedEvent("Screen initialization failed.");
	}
}

YSL_END_NAMESPACE(Devices)

YSL_END

#endif

