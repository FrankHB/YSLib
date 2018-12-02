/*
	© 2011-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Video.cpp
\ingroup YCLib
\brief 平台相关的视频输出接口。
\version r539
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-26 20:19:54 +0800
\par 修改时间:
	2018-11-29 22:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Video
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Video
#if YCL_DS
#	include YFM_DS_YCLib_DSVideo // for platform::ResetVideo;
#	include YFM_YCLib_NativeAPI // for ::lcdMainOnTop;
#endif

namespace platform
{

//! \since build 441
static_assert(sizeof(Pixel) == sizeof(Pixel::traits_type::integer),
	"Wrong size of pixel type found.");
static_assert(yalignof(Pixel) == yalignof(Pixel::traits_type::integer),
	"Wrong alignment of pixel type found.");

void
InitVideo() yimpl(ynothrow)
{
#if YCL_DS
	platform_ex::ResetVideo();
	::lcdMainOnTop();
#endif
}

} // namespace platform;

