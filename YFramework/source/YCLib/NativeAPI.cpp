/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NativeAPI.cpp
\ingroup YCLib
\brief 通用平台应用程序接口描述。
\version r493
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2012-03-26 13:36:28 +0800
\par 修改时间:
	2013-05-06 18:18 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::NativeAPI
*/


#include "YCLib/NativeAPI.h"
#include <ydef.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "CHRLib/chrproc.h"

using namespace CHRLib;

namespace
{

}

#if YCL_DS


#elif YCL_MINGW32

extern "C"
{
} // extern "C";

#endif

namespace platform
{
}

