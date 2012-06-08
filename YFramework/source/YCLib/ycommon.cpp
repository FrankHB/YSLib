/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycommon.cpp
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version r3616;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-12 22:14:42 +0800;
\par 修改时间:
	2012-06-05 21:19 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::YCommon;
*/


#include "YCLib/Debug.h"
#include <cstring>
#include <cerrno>
#include "YCLib/NativeAPI.h"

namespace platform
{

void*
mmbset(void* d, int v, std::size_t t)
{
#if YCL_DS
	// NOTE: DMA fill to main RAM is slower.
//	return safe_dma_fill(d, v, t) != 0 ? std::memset(d, v, t) : d;
#endif
	return std::memset(d, v, t);
}

void*
mmbcpy(void* d, const void* s, std::size_t t)
{
#if YCL_DS
	// NOTE: DMA copy to main RAM is even more slower.
	// TODO: use ASM optimization, like using LDMIA instructions;
//	return safe_dma_copy(d, s, t) != 0 ? std::memcpy(d, s, t) : d;
#endif
	return std::memcpy(d, s, t);
}


void
terminate()
{
#if YCL_DS
	for(;;)
		::swiWaitForVBlank();
#else
	std::terminate();
#endif
}


#ifdef YB_USE_YASSERT

void
yassert(bool exp, const char* expstr, const char* message,
	int line, const char* file)
{
	if(YB_UNLIKELY(!exp))
	{
		yprintf("Assertion failed: \n"
			"%s\nMessage: \n%s\nAt line %i in file \"%s\".\n",
			expstr, message, line, file);
		std::abort();
	}
}

#endif

}

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
#endif

}

