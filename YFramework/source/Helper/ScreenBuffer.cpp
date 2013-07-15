/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ScreenBuffer.cpp
\ingroup Helper
\brief 屏幕缓冲区。
\version r161
\author FrankHB <frankhb1989@gmail.com>
\since build 387
\par 创建时间:
	2013-03-08 11:34:28 +0800
\par 修改时间:
	2013-07-13 07:49 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::ScreenBuffer
*/


#include "ScreenBuffer.h"
#include "Helper/ShellHelper.h" // for YCL_DEBUG_PUTS, YSL_DEBUG_DECL_TIMER;

YSL_BEGIN

using namespace Drawing;

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)
#	if YCL_MINGW32

#	endif
YSL_END_NAMESPACE(Host)
#endif

YSL_END

