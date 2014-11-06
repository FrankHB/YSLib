/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ScreenBuffer.h
\ingroup Helper
\brief 屏幕缓冲区。
\version r386
\author FrankHB <frankhb1989@gmail.com>
\since build 387
\par 创建时间:
	2013-03-08 11:34:28 +0800
\par 修改时间:
	2014-11-04 17:43 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::ScreenBuffer
*/


#ifndef INC_Helper_ScreenBuffer_h_
#define INC_Helper_ScreenBuffer_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YGDIBase
#include YFM_YCLib_NativeAPI
#if YCL_Win32
#	include YFM_MinGW32_YCLib_Win32GUI
#elif YCL_Android
#	include YFM_Android_YCLib_Android
#endif

namespace YSLib
{

#if YF_Hosted
namespace Host
{

//! \since build 428
using namespace platform_ex;

} // namespace Host;
#endif

} // namespace YSLib;

#endif

