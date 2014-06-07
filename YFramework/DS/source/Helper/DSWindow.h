/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSWindow.h
\ingroup Helper
\ingroup DS
\brief DS 宿主窗口。
\version r89
\author FrankHB <frankhb1989@gmail.com>
\since build 398
\par 创建时间:
	2013-04-11 10:32:56 +0800
\par 修改时间:
	2014-06-05 09:53 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper_(DS)::DSWindow
*/


#ifndef Inc_Helper_DSWindow_h_
#define Inc_Helper_DSWindow_h_ 1

#include "Helper/YModules.h"
#include YFM_Helper_HostWindow // for Environment, Host::Window;
#include "DSScreen.h" // for Devices::DSScreen;

namespace YSLib
{

#if YF_Hosted
namespace Host
{

/*!
\brief 双屏幕宿主窗口。
\since build 383
*/
class DSWindow : public Window
{
private:
	//! \since build 397
	Devices::DSScreen& scr_up;
	//! \since build 397
	Devices::DSScreen& scr_dn;

public:
	//! \since build 397
	DSWindow(NativeWindowHandle, Devices::DSScreen&, Devices::DSScreen&,
		Environment&);

	pair<Drawing::Point, Drawing::Point>
	GetInputBounds() const ynothrow override;
};

} // namespace Host;
#endif

} // namespace YSLib;

#endif

