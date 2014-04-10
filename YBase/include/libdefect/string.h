/*
	© 2012-2014 FrankHB.

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
\brief 标准库实现 <tt><string></tt> 修正。
\version r559
\author FrankHB <frankhb1989@gmail.com>
\since build 308
\par 创建时间:
	2012-05-14 20:41:08 +0800
\par 修改时间:
	2014-04-10 22:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	LibDefect::String
*/


#ifndef YB_INC_libdefect_string_h_
#define YB_INC_libdefect_string_h_ 1

#include <string>

// See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52015 .
// NOTE: Fixed @ 4.8 for MinGW-W64.

#if defined(__GLIBCXX__) \
	&& (defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L) \
	&& !(defined(_GLIBCXX_USE_C99) && !defined(_GLIBCXX_HAVE_BROKEN_VSWPRINTF))

#include <ext/string_conversions.h>

namespace std _GLIBCXX_VISIBILITY(default)
{

#ifndef _GLIBCXX_USE_C99

#	ifndef __BIONIC__
extern "C" long long int
(strtoll)(const char* __restrict, char** __restrict, int) throw();
extern "C" unsigned long long int
(strtoull)(const char* __restrict, char** __restrict, int) throw();
using ::strtold;
extern "C" int
(vsnprintf)(char* __restrict, std::size_t, const char* __restrict,
	__gnuc_va_list) throw();
#	else
//! \since build 492
using ::vsnprintf;
#	endif

using ::strtof;
using ::wcstol;
using ::wcstoul;
using ::wcstod;
#	ifndef __BIONIC__
using ::wcstoll;
using ::wcstoull;
using ::wcstof;
using ::wcstold;
#	endif

#endif


_GLIBCXX_BEGIN_NAMESPACE_VERSION

// 21.4 Numeric Conversions [string.conversions].

#define YB_LIBDEFECT_STOI(_s, _n, _t, _cfname) \
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
YB_LIBDEFECT_STOI(string, stol, long, std::strtol)
YB_LIBDEFECT_STOI(string, stoul, unsigned long, std::strtoul)
#ifndef __BIONIC__
YB_LIBDEFECT_STOI(string, stoll, long long, std::strtoll)
YB_LIBDEFECT_STOI(string, stoull, unsigned long long, std::strtoull)
#endif
#ifdef _GLIBCXX_USE_WCHAR_T
inline int
stoi(const wstring& __str, size_t* __idx = {}, int __base = 10)
{
	return __gnu_cxx::__stoa<long, int>(&std::wcstol, "stoi", __str.c_str(),
		__idx, __base);
}
YB_LIBDEFECT_STOI(wstring, stol, long, std::wcstol)
YB_LIBDEFECT_STOI(wstring, stoul, unsigned long, std::wcstoul)
#	ifndef __BIONIC__
YB_LIBDEFECT_STOI(wstring, stoll, long long, std::wcstoll)
YB_LIBDEFECT_STOI(wstring, stoull, unsigned long long, std::wcstoull)
#	endif
#endif

#undef YB_LIBDEFECT_STOI


#define YB_LIBDEFECT_STOF(_s, _n, _t, _cfname) \
	inline _t \
	_n(const _s& __str, size_t* __idx = {}) \
	{ \
		return __gnu_cxx::__stoa(&_cfname, #_n, __str.c_str(), __idx); \
	}

#	ifndef __BIONIC__
// NOTE: Seems to be a bug of Clang++ 3.4.
// NOTE: strtof vs strtod.
YB_LIBDEFECT_STOF(string, stof, float, std::strtof)
YB_LIBDEFECT_STOF(string, stod, double, std::strtod)
YB_LIBDEFECT_STOF(string, stold, long double, std::strtold)
#ifdef _GLIBCXX_USE_WCHAR_T
// NOTE: wcstof vs wcstod.
YB_LIBDEFECT_STOF(wstring, stof, float, std::wcstof)
YB_LIBDEFECT_STOF(wstring, stod, double, std::wcstod)
YB_LIBDEFECT_STOF(wstring, stold, long double, std::wcstold)
#endif
#	endif

#undef YB_LIBDEFECT_STOF


// NOTE: (v)snprintf vs sprintf.
#define YB_LIBDEFECT_TOSTRI(_s, _t, _fmt, _cfname) \
	inline _s \
	to_##_s(_t __val) \
	{ \
		return __gnu_cxx::__to_xstring<_s>(&_cfname, \
			sizeof(_t) * 4, _fmt, __val); \
	}

// DR 1261.
YB_LIBDEFECT_TOSTRI(string, int, "%d", std::vsnprintf)
YB_LIBDEFECT_TOSTRI(string, unsigned, "%u", std::vsnprintf)
YB_LIBDEFECT_TOSTRI(string, long, "%ld", std::vsnprintf)
YB_LIBDEFECT_TOSTRI(string, unsigned long, "%lu", std::vsnprintf)
YB_LIBDEFECT_TOSTRI(string, long long, "%lld", std::vsnprintf)
YB_LIBDEFECT_TOSTRI(string, unsigned long long, "%llu", std::vsnprintf)
#if !defined(_GLIBCXX_HAVE_BROKEN_VSWPRINTF) && defined(_GLIBCXX_USE_WCHAR_T)
YB_LIBDEFECT_TOSTRI(wstring, int, L"%d", std::vswprintf)
YB_LIBDEFECT_TOSTRI(wstring, unsigned, L"%u", std::vswprintf)
YB_LIBDEFECT_TOSTRI(wstring, long, L"%ld", std::vswprintf)
YB_LIBDEFECT_TOSTRI(wstring, unsigned long, L"%lu", std::vswprintf)
YB_LIBDEFECT_TOSTRI(wstring, long long, L"%lld", std::vswprintf)
YB_LIBDEFECT_TOSTRI(wstring, unsigned long long, L"%llu", std::vswprintf)
#endif

#undef YB_LIBDEFECT_TOSTRI


#define YB_LIBDEFECT_TOSTRF(_s, _t, _fmt, _cfname) \
	inline _s \
	to_##_s(_t __val) \
	{ \
		return __gnu_cxx::__to_xstring<_s>(&_cfname, \
			__gnu_cxx::__numeric_traits<_t>::__max_exponent10 + 20, \
			_fmt, __val); \
	}

YB_LIBDEFECT_TOSTRF(string, float, "%f", std::vsnprintf)
YB_LIBDEFECT_TOSTRF(string, double, "%f", std::vsnprintf)
YB_LIBDEFECT_TOSTRF(string, long double, "%f", std::vsnprintf)
#if !defined(_GLIBCXX_HAVE_BROKEN_VSWPRINTF) && defined(_GLIBCXX_USE_WCHAR_T)
YB_LIBDEFECT_TOSTRF(wstring, float, L"%f", std::vswprintf)
YB_LIBDEFECT_TOSTRF(wstring, double, L"%f", std::vswprintf)
YB_LIBDEFECT_TOSTRF(wstring, long double, L"%f", std::vswprintf)
#endif

#undef YB_LIBDEFECT_TOSTRF

_GLIBCXX_END_NAMESPACE_VERSION

} // namespace std;

#endif /* ... __GXX_EXPERIMENTAL_CXX0X__ ... */

#endif

