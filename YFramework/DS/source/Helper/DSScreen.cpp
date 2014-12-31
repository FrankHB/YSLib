/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSScreen.cpp
\ingroup Helper
\ingroup DS
\brief DS 屏幕。
\version r276
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:27:29 +0800
\par 修改时间:
	2014-12-29 05:24 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper_(DS)::DSScreen
*/


#include "DSScreen.h"

namespace YSLib
{

using namespace Drawing;

namespace Devices
{

#if YCL_DS
DSScreen::DSScreen(bool b) ynothrow
	: Screen({MainScreenWidth, MainScreenHeight})
{
	pBuffer = (b ? DS::InitScrDown : DS::InitScrUp)(bg);
}

void
DSScreen::Update(ConstBitmapPtr p_buf) ynothrow
{
	DS::ScreenSynchronize(GetCheckedBufferPtr(), p_buf);
}
#elif YCL_HostedUI
DSScreen::DSScreen(bool b) ynothrow
	: Screen({MainScreenWidth, MainScreenHeight}),
	Offset(), WindowHandle(),
	rbuf({MainScreenWidth, MainScreenHeight})
{
	pBuffer = rbuf.GetBufferPtr();
	if(b)
		Offset.Y = MainScreenHeight;
}

void
DSScreen::Update(ConstBitmapPtr p_buf) ynothrow
{
	rbuf.UpdateFrom(p_buf);
	rbuf.UpdateTo(Nonnull(WindowHandle), Offset);
}
#else
#	error "Unsupported platform found."
#endif

} // namespace Devices;

} // namespace YSLib;

