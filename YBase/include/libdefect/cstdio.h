/*
	© 2014-2015, 2017, 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstdio.h
\ingroup LibDefect
\brief 标准库实现 \c \<cstdio> 修正。
\version r123
\author FrankHB <frankhb1989@gmail.com>
\since build 580
\par 创建时间:
	2015-02-26 22:39:31 +0800
\par 修改时间:
	2019-12-26 22:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	LibDefect::CStandardIO
*/


#ifndef YB_INC_libdefect_cstdio_h_
#define YB_INC_libdefect_cstdio_h_ 1

#include <cstdio>

#undef snprintf
#undef vfscanf
#undef vscanf
#undef vsnprintf
#undef vsscanf

#if defined(__GLIBCXX__) \
	&& (defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L) \
	&& !(defined(_GLIBCXX_USE_C99))

#	if defined(__BIONIC__)
#		if !(__ISO_C_VISIBLE >= 1999 || __BSD_VISIBLE)
extern "C"
{

int
snprintf(char *, size_t, const char *, ...)
	__attribute__((__format__ (printf, 3, 4)))
	__attribute__((__nonnull__ (3)));

int
vfscanf(FILE *, const char *, __va_list)
	__attribute__((__format__ (scanf, 2, 0)))
	__attribute__((__nonnull__ (2)));

int
vscanf(const char *, __va_list)
	__attribute__((__format__ (scanf, 1, 0)))
	__attribute__((__nonnull__ (1)));

int
vsnprintf(char *, size_t, const char *, __va_list)
	__attribute__((__format__ (printf, 3, 0)))
	__attribute__((__nonnull__ (3)));

int
vsscanf(const char *, const char *, __va_list)
	__attribute__((__format__ (scanf, 2, 0)))
	__attribute__((__nonnull__ (2)));

} // extern "C";
#		endif
#	elif defined(_NEWLIB_VERSION)
#		if defined(__STRICT_ANSI__) && !(__STDC_VERSION__ >= 199901L) \
	&& !(__ISO_C_VISIBLE >= 1999)
extern "C"
{

int
_EXFUN(snprintf, (char *__restrict, size_t, const char *__restrict, ...)
	_ATTRIBUTE((__format__(__printf__, 3, 4))));

int
_EXFUN(vfscanf, (FILE *__restrict, const char *__restrict, __VALIST)
	_ATTRIBUTE((__format__(__scanf__, 2, 0))));

int
_EXFUN(vscanf, (const char *, __VALIST)
	_ATTRIBUTE((__format__(__scanf__, 1, 0))));

int
_EXFUN(vsnprintf, (char *__restrict, size_t, const char *__restrict, __VALIST)
	_ATTRIBUTE((__format__(__printf__, 3, 0))));

int
_EXFUN(vsscanf, (const char *__restrict, const char *__restrict, __VALIST)
	_ATTRIBUTE((__format__(__scanf__, 2, 0))));

} // extern "C";
#		endif
#	endif

namespace std
{

using ::snprintf;
using ::vfscanf;
using ::vscanf;
//! \since build 492
using ::vsnprintf;
using ::vsscanf;

} // namespace std;

#endif

#endif

