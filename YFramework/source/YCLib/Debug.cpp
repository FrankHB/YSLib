/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Debug.cpp
\ingroup YCLib
\brief YCLib 调试设施。
\version r1066;
\author FrankHB<frankhb1989@gmail.com>
\since build 299 。
\par 创建时间:
	2012-04-07 14:22:09 +0800;
\par 修改时间:
	2012-07-03 14:48 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::Debug;
*/


#include "YCLib/Debug.h"
#include "YCLib/Input.h"
#include "YCLib/Video.h" // for platform::ColorSpace;
#include <cstdarg>

namespace platform
{

namespace
{
	static bool bDebugStatus(false);
} // unnamed namespace;

void
YDebugSetStatus(bool s)
{
	bDebugStatus = s;
}

bool
YDebugGetStatus()
{
	return bDebugStatus;
}

void
YDebugBegin()
{
	if(bDebugStatus)
		YConsoleInit(false, ColorSpace::White, ColorSpace::Blue);
}

void
YDebug()
{
	if(bDebugStatus)
	{
		YDebugBegin();
		WaitForInput();
	}
}
void
YDebug(const char* s)
{
	if(bDebugStatus)
	{
		YDebugBegin();
		std::puts(s);
		WaitForInput();
	}
}

int
yprintf(const char* str, ...)
{
	int t = -1;

	if(bDebugStatus)
	{
		YDebugBegin();

		std::va_list list;

		va_start(list, str);

		t = std::vprintf(str, list);

		va_end(list);
		WaitForInput();
	}
	return t;
}

} // namespace platform;

