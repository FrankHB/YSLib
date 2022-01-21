/*
	© 2013-2015, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cassert.cpp
\ingroup YStandardEx
\brief ISO C 断言/调试跟踪扩展。
\version r96
\author FrankHB <frankhb1989@gmail.com>
\since build 432
\par 创建时间:
	2012-07-27 04:13:34 +0800
\par 修改时间:
	2021-12-29 01:08 +0800
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

void
yassert(const char* expr_str, const char* file, int line, const char* msg)
	ynothrow
{
	const auto chk_null([](const char* s) ynothrow{
		return s && *s != char()? s : "<unknown>";
	});

	// XXX: Error from 'std::fprintf' is ignored.
	std::fprintf(stderr, "Assertion failed @ \"%s\":%i:\n"
		" %s .\nMessage: \n%s\n", chk_null(file), line, chk_null(expr_str),
		chk_null(msg));
	std::terminate();
}

#if YB_Use_YTrace
void
ytrace(std::FILE* stream, std::uint8_t lv, std::uint8_t t, const char* file,
	int line, const char* msg, ...) ynothrow
{
	if(lv < t)
	{
		std::fprintf(stream, "Trace[%#X] @ \"%s\":%i:\n", unsigned(lv), file,
			line);

		std::va_list args;

		va_start(args, msg);
		// XXX: Error from 'std::vfprintf' is ignored.
		std::vfprintf(stream, msg, args);
		va_end(args);
	}
}
#endif

} // namespace ystdex;

