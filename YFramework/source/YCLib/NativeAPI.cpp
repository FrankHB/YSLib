/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NativeAPI.cpp
\ingroup YCLib
\brief 通用平台应用程序接口描述。
\version r511
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2012-03-26 13:36:28 +0800
\par 修改时间:
	2014-07-22 18:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::NativeAPI
*/


#include "YCLib/YModules.h"
#include "CHRLib/YModules.h"
#include YFM_YCLib_NativeAPI
#include <ydef.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include YFM_CHRLib_CharacterProcessing

using namespace CHRLib;

namespace
{

}

#if YCL_DS


#elif YCL_MinGW

extern "C"
{

#	if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
// See FileSystem.cpp, line 40.
//! \since build 465
struct ::tm* __cdecl __MINGW_NOTHROW
_gmtime32(const ::__time32_t* p)
{
	return std::gmtime(p);
}
#	endif

} // extern "C";

#endif

namespace platform
{
}

