/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycommon.cpp
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version r2739
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-12 22:14:42 +0800
\par 修改时间:
	2014-10-02 02:10 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::YCommon
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Debug
#include <cstring>
#include <cerrno>
#include <cstdarg>
#include YFM_YCLib_NativeAPI
#if YCL_Win32
#	include YFM_MinGW32_YCLib_MinGW32
#endif

namespace platform
{

void
terminate() ynothrow
{
#if YCL_DS
	for(;;)
		::swiWaitForVBlank();
#else
	std::abort();
#endif
}

int
usystem(const char* cmd)
{
#if YCL_Win32
	return ::_wsystem(platform_ex::MBCSToWCS(cmd, CP_UTF8).c_str());
#else
	return std::system(cmd);
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
#elif YCL_Win32

#endif

}

