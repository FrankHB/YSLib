/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ScreenBuffer.h
\ingroup Helper
\brief 屏幕缓冲区。
\version r368
\author FrankHB <frankhb1989@gmail.com>
\since build 387
\par 创建时间:
	2013-03-08 11:34:28 +0800
\par 修改时间:
	2013-08-05 20:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::ScreenBuffer
*/


#ifndef INC_Helper_ScreenBuffer_h_
#define INC_Helper_ScreenBuffer_h_ 1

#include "YSLib/Core/ygdibase.h"
#include "YCLib/NativeAPI.h"
#if YCL_MULTITHREAD == 1
#	include <mutex>
#endif
#if YCL_MinGW32
#	include "YCLib/Win32GUI.h"
#elif YCL_HOSTED
#		error "Currently only Windows is supported with host implementation."
#endif

namespace YSLib
{

#if YCL_HOSTED
namespace Host
{

//! \since build 428
using namespace platform_ex;

} // namespace Host;
#endif

} // namespace YSLib;

#endif

