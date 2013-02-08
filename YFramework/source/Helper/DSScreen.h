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
\version r143
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:28:02 +0800
\par 修改时间:
	2013-02-08 02:49 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::DSScreen
*/


#ifndef Inc_Helper_DSScreen_h_
#define Inc_Helper_DSScreen_h_ 1

#include "Helper/DSMain.h" // for ScreenWidth, ScreenHeight, Host::Window;
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
struct ScreenBuffer
{
	Drawing::BitmapPtr pBuffer;
	::HBITMAP hBitmap;

	ScreenBuffer(const Drawing::Size&);
	~ScreenBuffer();

private:
	::HBITMAP
	InitializeDIB(void*&, SDst, SDst);
};
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
	\brief 宿主窗口。
	\since build 379
	*/
	shared_ptr<Host::Window> p_host_wnd;
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

	/*!
	\brief 更新到宿主。
	\param hDC 宿主窗口设备上下文句柄。
	\param hMemDC 内存设备上下文句柄。
	\note 复制到宿主窗口。
	\since build 319
	*/
	void
	UpdateToHost(::HDC hDC, ::HDC hMemDC) ynothrow;
#else
#	error Unsupported platform found!
#endif
};

YSL_END_NAMESPACE(Devices)

YSL_END

#endif

