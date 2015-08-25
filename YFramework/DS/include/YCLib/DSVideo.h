/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSVideo.h
\ingroup YCLib
\ingroup DS
\brief DS 视频输出接口。
\version r120
\author FrankHB <frankhb1989@gmail.com>
\since build 585
\par 创建时间:
	2015-03-17 12:45:13 +0800
\par 修改时间:
	2015-08-25 21:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(DS)::DSVideo
*/


#ifndef YCL_DS_INC_DSVideo_h_
#define YCL_DS_INC_DSVideo_h_ 1

#include "YCLib/YModules.h"
#include YFM_YCLib_Video

namespace platform_ex
{

#if YCL_DS
/*!
\brief 复位屏幕显示模式：清除背景寄存器，使用 ::vramDefault 复位，设置双屏 Mode 5 。
*/
YF_API void
ResetVideo();


/*!
\brief 默认上屏初始化函数。
\since build 564
*/
YF_API platform::Pixel*
InitScrUp(int&);

/*!
\brief 默认下屏初始化函数。
\since build 564
*/
YF_API platform::Pixel*
InitScrDown(int&);

/*!
\brief 快速刷新缓存映像到显示屏缓冲区。
\note 第一参数为显示屏缓冲区，第二参数为源缓冲区。
\since build 319
*/
YF_API YB_NONNULL(1, 2) void
ScreenSynchronize(platform::Pixel*, const platform::Pixel*) ynothrow;

/*!
\brief 启动控制台。
\param use_customed 使用自定义而非 LibNDS 提供的默认例程初始化控制台。
\param fc 前景色。
\param bc 背景色。
\since build 626

初始化 DS 控制台。第一参数为 \c true 指定初始化上屏，否则调用 ::consoleDemoInit
初始化下屏。
*/
YF_API void
DSConsoleInit(bool use_customed,
	platform::Color fc = platform::ColorSpace::White,
	platform::Color bc = platform::ColorSpace::Black) ynothrow;
#endif


#if YCL_DS || YF_Hosted
/*!
\brief DS 显示状态。
\note 对 DS 提供实际的状态设置；对宿主实现，仅保存状态。
\since build 429
*/
class YF_API DSVideoState
{
#if YF_Hosted
private:
	bool LCD_main_on_top = true;
#endif

public:
	bool
	IsLCDMainOnTop() const;

	void
	SetLCDMainOnTop(bool);

	void
	SwapLCD();
};
#endif

} // namespace platform_ex;

#endif

