/*
	Copyright (C) by Franksoft 2009 - 2012.

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
\brief 标准库实现 <string> 修正。
\version r417
\author FrankHB<frankhb1989@gmail.com>
\since build 308
\par 创建时间:
	2012-05-14 20:41:08 +0800
\par 修改时间:
	2012-09-04 10:43 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	LibDefect::String
*/


#ifndef YB_INC_LIBDEFECT_STRING_H_
#define YB_INC_LIBDEFECT_STRING_H_ 1

#include <string>

#ifdef __GLIBCXX__

// TODO: Confirm exact versions affected.

#if (__GLIBCXX__ <= 20120614 && __GXX_EXPERIMENTAL_CXX0X__)
// && _GLIBCXX_USE_C99
// && _GLIBCXX_HAVE_BROKEN_VSWPRINTF

#include <ext/string_conversions.h>

namespace std _GLIBCXX_VISIBILITY(default)
{

// FIXME: check for missing functions;
#ifndef _GLIBCXX_USE_C99

extern "C" long long int
(strtoll)(const char * __restrict, char ** __restrict, int) throw();
extern "C" unsigned long long int
(strtoull)(const char * __restrict, char ** __restrict, int) throw();
using ::strtof;
using ::strtold;
extern "C" int
(vsnprintf)(char * __restrict, std::size_t, const char * __restrict,
	__gnuc_va_list) throw();
using ::wcstol;
using ::wcstoul;
using ::wcstoll;
using ::wcstoull;
using ::wcstof;
using ::wcstod;
using ::wcstold;

#endif


_GLIBCXX_BEGIN_NAMESPACE_VERSION

// 21.4 Numeric Conversions [string.conversions].

#define YB_LIBDEFECT_STOI(_name, _type, _cfname) \
	inline _type \
	_name(const string& __str, size_t* __idx = 0, int __base = 10) \
	{ \
		return __gnu_cxx::__stoa(&_cfname, #_name, __str.c_str(), __idx, \
			__base); \
	}

inline int
stoi(const string& __str, size_t* __idx = 0, int __base = 10)
{
	return __gnu_cxx::__stoa<long, int>(&std::strtol, "stoi", __str.c_str(),
	__idx, __base);
}
YB_LIBDEFECT_STOI(stol, long, std::strtol)
YB_LIBDEFECT_STOI(stoul, unsigned long, std::strtoul)
YB_LIBDEFECT_STOI(stoll, long long, std::strtoll)
YB_LIBDEFECT_STOI(stoull, unsigned long long, std::strtoull)

#undef YB_LIBDEFECT_STOI


#define YB_LIBDEFECT_STOF(_name, _type, _cfname) \
	inline _type \
	_name(const string& __str, size_t* __idx = 0) \
	{ \
		return __gnu_cxx::__stoa(&_cfname, #_name, __str.c_str(), __idx); \
	}

// NB: strtof vs strtod.
YB_LIBDEFECT_STOF(stof, float, std::strtof)
YB_LIBDEFECT_STOF(stod, double, std::strtod)
YB_LIBDEFECT_STOF(stold, long double, std::strtold)

#undef YB_LIBDEFECT_STOF


// NB: (v)snprintf vs sprintf.
#define YB_LIBDEFECT_TOSTRI(_type, _fmt) \
	inline string \
	to_string(_type __val) \
	{ \
		return __gnu_cxx::__to_xstring<string>(&std::vsnprintf, \
			4 * sizeof(_type), _fmt, __val); \
	}

// DR 1261.
YB_LIBDEFECT_TOSTRI(int, "%d")
YB_LIBDEFECT_TOSTRI(unsigned, "%u")
YB_LIBDEFECT_TOSTRI(long, "%ld")
YB_LIBDEFECT_TOSTRI(unsigned long, "%lu")
YB_LIBDEFECT_TOSTRI(long long, "%lld")
YB_LIBDEFECT_TOSTRI(unsigned long long, "%llu")

#undef YB_LIBDEFECT_TOSTRI


#define YB_LIBDEFECT_TOSTRF(_type, _fmt) \
	inline string \
	to_string(_type __val) \
	{ \
		return __gnu_cxx::__to_xstring<string>(&std::vsnprintf, \
			__gnu_cxx::__numeric_traits<_type>::__max_exponent10 + 20, \
			_fmt, __val); \
	}

YB_LIBDEFECT_TOSTRF(float, "%f")
YB_LIBDEFECT_TOSTRF(double, "%f")
YB_LIBDEFECT_TOSTRF(long double, "%f")

#undef YB_LIBDEFECT_TOSTRF


#ifdef _GLIBCXX_USE_WCHAR_T

inline int
stoi(const wstring& __str, size_t* __idx = 0, int __base = 10)
{ return __gnu_cxx::__stoa<long, int>(&std::wcstol, "stoi", __str.c_str(),
__idx, __base); }

inline long
stol(const wstring& __str, size_t* __idx = 0, int __base = 10)
{ return __gnu_cxx::__stoa(&std::wcstol, "stol", __str.c_str(),
__idx, __base); }

inline unsigned long
stoul(const wstring& __str, size_t* __idx = 0, int __base = 10)
{ return __gnu_cxx::__stoa(&std::wcstoul, "stoul", __str.c_str(),
__idx, __base); }

inline long long
stoll(const wstring& __str, size_t* __idx = 0, int __base = 10)
{ return __gnu_cxx::__stoa(&std::wcstoll, "stoll", __str.c_str(),
__idx, __base); }

inline unsigned long long
stoull(const wstring& __str, size_t* __idx = 0, int __base = 10)
{ return __gnu_cxx::__stoa(&std::wcstoull, "stoull", __str.c_str(),
__idx, __base); }

// NB: wcstof vs wcstod.
inline float
stof(const wstring& __str, size_t* __idx = 0)
{ return __gnu_cxx::__stoa(&std::wcstof, "stof", __str.c_str(), __idx); }

inline double
stod(const wstring& __str, size_t* __idx = 0)
{ return __gnu_cxx::__stoa(&std::wcstod, "stod", __str.c_str(), __idx); }

inline long double
stold(const wstring& __str, size_t* __idx = 0)
{ return __gnu_cxx::__stoa(&std::wcstold, "stold", __str.c_str(), __idx); }

#endif

_GLIBCXX_END_NAMESPACE_VERSION

} // namespace std;

#endif /* __GXX_EXPERIMENTAL_CXX0X__ && _GLIBCXX_USE_C99 ... */

#endif

#endif

