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
\version r402
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:28:02 +0800
\par 修改时间:
	2013-04-11 14:02 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::DSScreen
*/


#ifndef Inc_Helper_DSScreen_h_
#define Inc_Helper_DSScreen_h_ 1

#include "Helper/DSMain.h" // for ScreenWidth, ScreenHeight, Host::Environment;
#include "YSLib/Core/ydevice.h"
#include "ScreenBuffer.h"
#if YCL_MULTITHREAD == 1
#	include <mutex>
#endif

YSL_BEGIN

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
	/*!
	\brief 宿主窗口句柄。
	\warning 应在调用 UpdateToSurface 前初始化为非空句柄。
	\since build 398
	*/
	Host::NativeWindowHandle WindowHandle;

private:
	//! \since build 387
	Host::ScreenRegionBuffer rbuf;

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
		sf.Update(rbuf, Offset);
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

