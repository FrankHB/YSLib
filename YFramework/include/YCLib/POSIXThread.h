/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file POSIXThread.h
\ingroup YCLib
\ingroup YCLibLimitedPlatforms
\brief POSIX 线程接口包装。
\version r62
\author FrankHB <frankhb1989@gmail.com>
\since build 553
\par 创建时间:
	2014-11-11 15:43:43 +0800
\par 修改时间:
	2014-12-22 13:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::POSIXThread
*/


#ifndef YCL_INC_POSIXThread_h_
#define YCL_INC_POSIXThread_h_ 1

#include "YModules.h"
#include YFM_YCLib_Platform
#include <ydef.h>
#if YF_Use_POSIXThread
#	include <pthread.h>
#	include YFM_YBaseMacro // for DefGetter, PDefH, ImplRet;

namespace platform_ex
{

class TLSKey
{
private:
	::pthread_key_t key;

public:
	TLSKey(void(*)(void*));
	~TLSKey();

	DefGetter(const ynothrow, void*, Value, ::pthread_getspecific(key))

	PDefH(int, SetValue, const void* value)
		ImplRet(::pthread_setspecific(key, value))
};

} // namespace platform_ex;

#endif

#endif

