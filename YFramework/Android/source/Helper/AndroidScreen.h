/*
	© 2014 FrankHB.

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
\version r114
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2014-06-04 22:53:58 +0800
\par 修改时间:
	2014-07-06 02:35 +0800
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
\since build 502 。
*/
class AndroidScreen : public Screen
{
private:
	std::reference_wrapper<::ANativeWindow> window_ref;
	Host::ScreenRegionBuffer rbuf;

public:
	Drawing::Point Offset{};

	/*!
	\brief 初始化窗口：使用指定本机窗口引用。
	\note 直接查询大小，若失败（结果为 1x1 ）则锁定屏幕查询大小。
	\note Android 4.0 起（ API 等级不小于 17 ）锁定屏幕查询大小出错。
	\since build 504
	\todo 在运行时判断 API 等级。
	*/
	AndroidScreen(::ANativeWindow&);
	//! \brief 初始化窗口：使用指定本机窗口引用和大小。
	AndroidScreen(::ANativeWindow&, const Drawing::Size&);

	DefGetter(const ynothrow, ::ANativeWindow&, WindowRef, window_ref)

	/*!
	\brief 更新。
	\pre 断言：本机窗口上下文非空。
	\pre 间接断言：参数非空。
	\note 复制到本机窗口或本机窗口缓冲区。
	\note 部分线程安全：在不同线程上更新到屏幕和屏幕缓冲区之间线程间未决定有序。
	*/
	void
	Update(Drawing::BitmapPtr) ynothrow override;
};

} // namespace Devices;

#endif

} // namespace YSLib;

#endif

