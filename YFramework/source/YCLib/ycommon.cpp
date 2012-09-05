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
\version r2664
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-12 22:14:42 +0800
\par 修改时间:
	2012-09-04 11:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::YCommon
*/


#include "YCLib/Debug.h"
#include <cstring>
#include <cerrno>
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
#elif YCL_MINGW32
std::string
UTF8ToMBCS(const char* str, std::size_t len, int cp)
{
	if(cp == CP_UTF8)
		return str;

	const int w_len(::MultiByteToWideChar(CP_UTF8, 0, str, len, NULL, 0));
	std::wstring wstr(w_len, L'\0');
	wchar_t* w_str = &wstr[0];

	::MultiByteToWideChar(CP_UTF8, 0, str, len, w_str, w_len);

	return WCSToMBCS(w_str, w_len, cp);
}

std::string
WCSToMBCS(const wchar_t* str, std::size_t len, int cp)
{
	const int r_len(::WideCharToMultiByte(cp, 0, str, len,
		NULL, 0, NULL, NULL));
	std::string mbcs(r_len, '\0');

	::WideCharToMultiByte(cp, 0, str, len, &mbcs[0], r_len, NULL, NULL);
	return mbcs;
}
#endif

}

