/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cassert.cpp
\ingroup YStandardEx
\brief ISO C 断言/调试跟踪扩展。
\version r67
\author FrankHB <frankhb1989@gmail.com>
\since build 432
\par 创建时间:
	2012-07-27 04:13:34 +0800
\par 修改时间:
	2013-07-27 15:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CAssert
*/


#include "ystdex/cassert.h"
#include <exception> // for std::terminate;
#include <cstdarg>

namespace ystdex
{

#ifdef YB_Use_YAssert
void
yassert(bool expr, const char* expr_str, const char* file, int line,
	const char* msg)
{
	if(YB_UNLIKELY(!expr))
	{
		std::fprintf(stderr, "Assertion failed @ \"%s\":%i:\n"
			" %s .\nMessage: \n%s\n", file, line, expr_str, msg);
		std::terminate();
	}
}
#endif

#if YB_Use_YTrace
void
ytrace(std::FILE* stream, std::uint8_t lv, std::uint8_t t, const char* file,
	int line, const char* msg, ...)
{
	if(lv < t)
	{
		std::fprintf(stream, "Trace[0x%X] @ \"%s\":%i:\n", unsigned(lv), file,
			line);

		std::va_list args;
		va_start(args, msg);
		std::vfprintf(stream, msg, args);
		va_end(args);
	}
}
#endif

} // namespace ystdex;

