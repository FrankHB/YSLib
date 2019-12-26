/*
	© 2012-2015, 2017, 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup LibDefect Library for Defects
\ingroup YBase
\brief 标准库实现修正。
*/

/*!	\file string.h
\ingroup LibDefect
\brief 标准库实现 \c \<string> 修正。
\version r675
\author FrankHB <frankhb1989@gmail.com>
\since build 308
\par 创建时间:
	2012-05-14 20:41:08 +0800
\par 修改时间:
	2019-12-26 22:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	LibDefect::String
*/


#ifndef YB_INC_libdefect_string_h_
#define YB_INC_libdefect_string_h_ 1

#include <string>
#include <cwchar> // for std::vswprintf;
#include "cstdio.h" // for std::vsnprintf;

// See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52015.
// NOTE: Fixed @ 4.8 for MinGW-W64.

#if defined(__GLIBCXX__) \
	&& (defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L) \
	&& !(defined(_GLIBCXX_USE_C99) || defined(_GLIBCXX_USE_C99_STDLIB)) \
	&& !defined(_GLIBCXX_HAVE_BROKEN_VSWPRINTF)

#	include <ext/string_conversions.h>

namespace std _GLIBCXX_VISIBILITY(default)
{

#	if !(defined(_GLIBCXX_USE_C99) || defined(_GLIBCXX_USE_C99_STDLIB))

#		ifndef __BIONIC__
extern "C" long long int
(strtoll)(const char* __restrict, char** __restrict, int) throw();
extern "C" unsigned long long int
(strtoull)(const char* __restrict, char** __restrict, int) throw();
using ::strtold;
#		else
//! \since build 543
using ::strtoll;
//! \since build 543
using ::strtoull;
#		endif

using ::strtof;
using ::wcstol;
using ::wcstoul;
using ::wcstod;
#		ifndef __BIONIC__
using ::wcstoll;
using ::wcstoull;
using ::wcstof;
using ::wcstold;
#		endif

#	endif


_GLIBCXX_BEGIN_NAMESPACE_VERSION
#	if _GLIBCXX_USE_CXX11_ABI
_GLIBCXX_BEGIN_NAMESPACE_CXX11
#	endif

// 21.4 Numeric Conversions [string.conversions].

#	define YB_LibDefect_String_stoi(_s, _n, _t, _cfname) \
	inline _t \
	_n(const _s& __str, size_t* __idx = {}, int __base = 10) \
	{ \
		return __gnu_cxx::__stoa(&_cfname, #_n, __str.c_str(), __idx, __base); \
	}

inline int
stoi(const string& __str, size_t* __idx = {}, int __base = 10)
{
	return __gnu_cxx::__stoa<long, int>(&std::strtol, "stoi", __str.c_str(),
		__idx, __base);
}
YB_LibDefect_String_stoi(string, stol, long, std::strtol)
YB_LibDefect_String_stoi(string, stoul, unsigned long, std::strtoul)
YB_LibDefect_String_stoi(string, stoll, long long, std::strtoll)
YB_LibDefect_String_stoi(string, stoull, unsigned long long, std::strtoull)
#	if defined(_GLIBCXX_USE_WCHAR_T) \
	&& (!defined(_GLIBCXX_USE_C99_WCHAR) || _GLIBCXX_USE_C99_WCHAR)
inline int
stoi(const wstring& __str, size_t* __idx = {}, int __base = 10)
{
	return __gnu_cxx::__stoa<long, int>(&std::wcstol, "stoi", __str.c_str(),
		__idx, __base);
}
YB_LibDefect_String_stoi(wstring, stol, long, std::wcstol)
YB_LibDefect_String_stoi(wstring, stoul, unsigned long, std::wcstoul)
#		ifndef __BIONIC__
YB_LibDefect_String_stoi(wstring, stoll, long long, std::wcstoll)
YB_LibDefect_String_stoi(wstring, stoull, unsigned long long, std::wcstoull)
#		endif
#	endif

#	undef YB_LibDefect_String_stoi


#	define YB_LibDefect_String_stof(_s, _n, _t, _cfname) \
	inline _t \
	_n(const _s& __str, size_t* __idx = {}) \
	{ \
		return __gnu_cxx::__stoa(&_cfname, #_n, __str.c_str(), __idx); \
	}

#	if !(defined(__clang__) && defined(__BIONIC__))
// NOTE: Seems to be a bug of Clang++ 3.4.
// NOTE: strtof vs strtod.
YB_LibDefect_String_stof(string, stof, float, std::strtof)
YB_LibDefect_String_stof(string, stod, double, std::strtod)
#	endif
#	if !defined(__BIONIC__)
YB_LibDefect_String_stof(string, stold, long double, std::strtold)
#	endif
#	if defined(_GLIBCXX_USE_WCHAR_T) \
	&& (!defined(_GLIBCXX_USE_C99_WCHAR) || _GLIBCXX_USE_C99_WCHAR) \
	&& !defined(__BIONIC__)
// NOTE: wcstof vs wcstod.
YB_LibDefect_String_stof(wstring, stof, float, std::wcstof)
YB_LibDefect_String_stof(wstring, stod, double, std::wcstod)
YB_LibDefect_String_stof(wstring, stold, long double, std::wcstold)
#	endif

#	undef YB_LibDefect_String_stof


// NOTE: (v)snprintf vs sprintf.
#	define YB_LibDefect_String_tostri(_s, _t, _fmt, _cfname) \
	inline _s \
	to_##_s(_t __val) \
	{ \
		return __gnu_cxx::__to_xstring<_s>(&_cfname, \
			sizeof(_t) * 4, _fmt, __val); \
	}

// DR 1261.
YB_LibDefect_String_tostri(string, int, "%d", std::vsnprintf)
YB_LibDefect_String_tostri(string, unsigned, "%u", std::vsnprintf)
YB_LibDefect_String_tostri(string, long, "%ld", std::vsnprintf)
YB_LibDefect_String_tostri(string, unsigned long, "%lu", std::vsnprintf)
YB_LibDefect_String_tostri(string, long long, "%lld", std::vsnprintf)
YB_LibDefect_String_tostri(string, unsigned long long, "%llu", std::vsnprintf)
#	if !defined(_GLIBCXX_HAVE_BROKEN_VSWPRINTF) \
	&& defined(_GLIBCXX_USE_WCHAR_T) \
	&& (!defined(_GLIBCXX_USE_C99_WCHAR) || _GLIBCXX_USE_C99_WCHAR)
YB_LibDefect_String_tostri(wstring, int, L"%d", std::vswprintf)
YB_LibDefect_String_tostri(wstring, unsigned, L"%u", std::vswprintf)
YB_LibDefect_String_tostri(wstring, long, L"%ld", std::vswprintf)
YB_LibDefect_String_tostri(wstring, unsigned long, L"%lu", std::vswprintf)
YB_LibDefect_String_tostri(wstring, long long, L"%lld", std::vswprintf)
YB_LibDefect_String_tostri(wstring, unsigned long long, L"%llu", std::vswprintf)
#	endif

#	undef YB_LibDefect_String_tostri


#	define YB_LibDefect_String_tostrf(_s, _t, _fmt, _cfname) \
	inline _s \
	to_##_s(_t __val) \
	{ \
		return __gnu_cxx::__to_xstring<_s>(&_cfname, \
			__gnu_cxx::__numeric_traits<_t>::__max_exponent10 + 20, \
			_fmt, double(__val)); \
	}

YB_LibDefect_String_tostrf(string, float, "%f", std::vsnprintf)
YB_LibDefect_String_tostrf(string, double, "%f", std::vsnprintf)
YB_LibDefect_String_tostrf(string, long double, "%f", std::vsnprintf)
#	if !defined(_GLIBCXX_HAVE_BROKEN_VSWPRINTF) \
	&& defined(_GLIBCXX_USE_WCHAR_T) \
	&& (!defined(_GLIBCXX_USE_C99_WCHAR) || _GLIBCXX_USE_C99_WCHAR)
YB_LibDefect_String_tostrf(wstring, float, L"%f", std::vswprintf)
YB_LibDefect_String_tostrf(wstring, double, L"%f", std::vswprintf)
YB_LibDefect_String_tostrf(wstring, long double, L"%f", std::vswprintf)
#	endif

#	undef YB_LibDefect_String_tostrf

#	if _GLIBCXX_USE_CXX11_ABI
_GLIBCXX_END_NAMESPACE_CXX11
#	endif
_GLIBCXX_END_NAMESPACE_VERSION

} // namespace std;

#endif /* ... __GXX_EXPERIMENTAL_CXX0X__ ... */

#endif

