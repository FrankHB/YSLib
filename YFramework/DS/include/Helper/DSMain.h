﻿/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSMain.h
\ingroup Helper
\ingroup DS
\brief DS 平台框架。
\version r834
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2012-03-25 12:49:27 +0800
\par 修改时间:
	2013-06-05 09:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper_(DS)::DSMain
*/


#ifndef INC_Helper_DSMain_h_
#define INC_Helper_DSMain_h_ 1

#include "Helper/YModules.h"
#include YFM_Helper_GUIApplication
#include YFM_YCLib_Video

namespace YSLib
{

#if YCL_DS || YF_Hosted
#	ifndef SCREEN_WIDTH
#		define SCREEN_WIDTH 256
#	endif
#	ifndef SCREEN_HEIGHT
#		define SCREEN_HEIGHT 192
#	endif

/*!	\defgroup CustomGlobalConstants Custom Global Constants
\ingroup GlobalObjects
\brief 平台相关的全局常量。
\since build 173
*/
//@{
/*!
\brief 屏幕大小。
\since build 215
*/
const SDst MainScreenWidth(SCREEN_WIDTH), MainScreenHeight(SCREEN_HEIGHT);
//@}


namespace Devices
{
class DSScreen;
} // namespace Devices;


/*!
\brief 平台相关的应用程序类。
\note 含默认接口。
\since build 215
*/
class YF_API DSApplication : public GUIApplication,
	private platform_ex::DSVideoState
{
private:
	/*!
	\brief 屏幕组。
	\since build 380
	*/
	array<unique_ptr<Devices::DSScreen>, 2> scrs;

#	if YCL_Win32
	/*!
	\brief 宿主窗口线程。
	\since build 398
	*/
	unique_ptr<Host::WindowThread> p_wnd_thrd;
#	endif

public:
	/*!
	\brief \c private 构造函数：非内联。
	\pre 断言：进程唯一性。
	*/
	DSApplication();

	/*!
	\brief 析构：释放资源。
	\since build 269
	*/
	~DSApplication() override;

	//! \since build 429
	using DSVideoState::IsLCDMainOnTop;
	//! \since build 377
	DefPred(const ynothrow, ScreenReady, bool(scrs[0]) && bool(scrs[1]))

	/*!
	\brief 取 DS 上屏幕。
	\pre 断言：内部指针非空。
	\since build 383
	*/
	Devices::DSScreen&
	GetDSScreenUp() const ynothrow;
	/*!
	\brief 取 DS 下屏幕。
	\pre 断言：内部指针非空。
	\since build 383
	*/
	Devices::DSScreen&
	GetDSScreenDown() const ynothrow;
	/*!
	\brief 取上屏幕。
	\note 使用 GetDSScreenUp 实现。
	\since build 297
	*/
	Devices::Screen&
	GetScreenUp() const ynothrow;
	/*!
	\brief 取下屏幕。
	\note 使用 GetDSScreenDown 实现。
	\since build 297
	*/
	Devices::Screen&
	GetScreenDown() const ynothrow;

	/*!
	\brief 复位 GUI 状态后交换屏幕。
	\since build 429
	*/
	void
	SwapScreens();
};


/*!
\brief 以指定前景色和背景色初始化指定屏幕的控制台。
\since build 148
*/
YF_API bool
InitConsole(Devices::Screen&, Drawing::PixelType, Drawing::PixelType);

/*!
\brief 显示致命错误。
\since build 175
*/
YF_API void
ShowFatalError(const char*);

#	if YCL_Win32
namespace MinGW32
{

using namespace platform_ex;

YF_API void
TestFramework(size_t);

} // namespace MinGW32;
#	endif
#else
#	error "Only DS and hosted platform supported."
#endif

} // namespace YSLib;

#endif

