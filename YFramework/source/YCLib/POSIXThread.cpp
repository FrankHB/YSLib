/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file POSIXThread.cpp
\ingroup YCLib
\ingroup YCLibLimitedPlatforms
\brief POSIX 线程接口包装。
\version r49
\author FrankHB <frankhb1989@gmail.com>
\since build 553
\par 创建时间:
	2014-11-11 15:44:02 +0800
\par 修改时间:
	2014-11-13 18:22 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::POSIXThread
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_POSIXThread

#if YF_Use_POSIXThread

namespace platform_ex
{

TLSKey::TLSKey(void(*dtor)(void*))
	: destructor(dtor)
{
	::pthread_key_create(&key, dtor);
}
TLSKey::~TLSKey()
{
	::pthread_key_delete(key);
}

} // namespace YSLib;

#endif

