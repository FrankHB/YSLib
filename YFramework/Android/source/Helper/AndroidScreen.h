/*
	© 2014-2015, 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file AndroidScreen.h
\ingroup Helper
\ingroup Android
\brief Android 屏幕。
\version r153
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2014-06-04 22:53:58 +0800
\par 修改时间:
	2019-09-06 02:22 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper_(Android)::AndroidScreen
*/


#ifndef Inc_Android_Helper_AndroidScreen_h_
#define Inc_Android_Helper_AndroidScreen_h_ 1

#include "Helper/YModules.h"
#include YFM_YSLib_Core_YDevice
#include YFM_Helper_ScreenBuffer
#if YCL_Android
#	include <android/native_window.h>
#else
//#	error "Currently only Android is supported."
#endif

namespace YSLib
{

#if YCL_Android

namespace Devices
{

/*!
\brief Android 屏幕。
\since build 502
*/
class AndroidScreen : public Screen
{
private:
	//! \since build 559
	lref<::ANativeWindow> window_ref;

public:
	/*!
	\note 使用 SetNativeBufferSize 设置缓冲区大小。
	\note 若缓冲区和屏幕大小不等，更新时自适应屏幕大小。
	\sa SetNativeBufferSize
	\since build 558
	*/
	//@{
	/*!
	\brief 初始化窗口：使用指定本机窗口引用和缓冲区大小。
	\throw LoggedEvent 调用 ::ANativeWindow_lock 失败。
	\note 直接查询大小，若失败（结果为 1x1 ）则锁定屏幕查询大小。
	\note Android 4.0 起（ API 等级不小于 17 ）锁定屏幕查询大小出错。
	\todo 在运行时判断 API 等级。
	*/
	AndroidScreen(::ANativeWindow&, const Drawing::Size& = {});
	//! \brief 初始化窗口：使用指定本机窗口引用、大小和缓冲区大小。
	AndroidScreen(::ANativeWindow&, const Drawing::Size&,
		const Drawing::Size&);
	//@}

	DefGetter(const ynothrow, ::ANativeWindow&, WindowRef, window_ref)

private:
	/*!
	\brief 设置本机缓存大小。
	\note 若为空则使用本机窗口的大小作为默认值。
	\since build 559
	*/
	void
	SetNativeBufferSize(const Drawing::Size&);

public:
	//! \since build 559
	void
	SetSize(const Drawing::Size&) override;

	/*!
	\brief 更新。
	\pre 间接断言：参数非空。
	\note 复制到本机窗口或本机窗口缓冲区。
	\note 部分线程安全：在不同线程上更新到屏幕和屏幕缓冲区之间线程间未决定有序。
	\since build 558
	*/
	YB_NONNULL(2) void
	Update(Drawing::ConstBitmapPtr) ynothrow override;
};

} // namespace Devices;

#endif

} // namespace YSLib;

#endif

