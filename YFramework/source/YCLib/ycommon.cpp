/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycommon.cpp
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version r2708
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-12 22:14:42 +0800
\par 修改时间:
	2013-08-05 21:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::YCommon
*/


#include "YCLib/Debug.h"
#include <cstring>
#include <cerrno>
#include <cstdarg>
#include "YCLib/NativeAPI.h"

namespace platform
{

void
terminate() ynothrow
{
#if YCL_DS
	for(;;)
		::swiWaitForVBlank();
#else
	std::terminate();
#endif
}

} // namespace platform;

namespace platform_ex
{

#if YCL_DS
bool
AllowSleep(bool b)
{
	static bool bSleepEnabled(true); //与 libnds 默认的 ARM7 电源管理同步。
	const bool b_old(bSleepEnabled);

	if(b != bSleepEnabled)
	{
		bSleepEnabled = b;
		::fifoSendValue32(FIFO_PM,
			b ? PM_REQ_SLEEP_ENABLE : PM_REQ_SLEEP_DISABLE);
	}
	return b_old;
}
#elif YCL_MinGW32

#endif

}

