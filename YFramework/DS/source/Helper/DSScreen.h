/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSScreen.h
\ingroup Helper
\ingroup DS
\brief DS 屏幕。
\version r455
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:28:02 +0800
\par 修改时间:
	2014-12-06 14:18 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper_(DS)::DSScreen
*/


#ifndef Inc_Helper_DSScreen_h_
#define Inc_Helper_DSScreen_h_ 1

#include "Helper/YModules.h"
#include YFM_DS_Helper_DSMain // for ScreenWidth, ScreenHeight;
#include YFM_YSLib_Core_YDevice
#include YFM_Helper_ScreenBuffer

namespace YSLib
{

namespace Devices
{

/*!
\brief DS 屏幕。
\since 早于 build 218
*/
class DSScreen : public Screen
{
#if YCL_DS
public:
	using BGType = int;

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
	\brief 取指针。
	\note 进行状态检查。
	\since build 558
	*/
	DefGetter(const ynothrow, Drawing::BitmapPtr, CheckedBufferPtr,
		Nonnull(GetBufferPtr()));

	/*!
	\brief 更新。
	\note 复制到屏幕。
	\since build 558
	*/
	void
	Update(Drawing::ConstBitmapPtr) ynothrow override;
#elif YCL_Win32 || YCL_Android
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
	\pre 断言：本机窗口句柄非空。
	\pre 间接断言：参数非空。
	\note 复制到屏幕或屏幕缓冲区。
	\note 部分线程安全：在不同线程上更新到屏幕和屏幕缓冲区之间线程间未决定有序。
	\since build 558
	*/
	void
	Update(Drawing::ConstBitmapPtr) ynothrow override;

	//! \since build 386
	template<typename _type>
	void
	UpdateToSurface(_type& sf)
	{
		sf.Update(rbuf, Offset);
	}
#else
#	error "Unsupported platform found."
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
		p_up.reset(new DSScreen({}));
		p_dn.reset(new DSScreen(true));
	}
	CatchThrow(..., LoggedEvent("Screen initialization failed."))
	YTraceDe(Notice, "DS screens initialized.");
}

} // namespace Devices;

} // namespace YSLib;

#endif

