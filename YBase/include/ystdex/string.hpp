/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file string.hpp
\ingroup YStandardEx
\brief YCLib ISO C++ 标准字符串扩展。
\version r259
\author FrankHB <frankhb1989@gmail.com>
\since build 304
\par 创建时间:
	2012-04-26 20:12:19 +0800
\par 修改时间:
	2013-02-04 17:52 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::String
*/


#ifndef YB_INC_YSTDEX_STRING_HPP_
#define YB_INC_YSTDEX_STRING_HPP_ 1

#include "container.hpp" // for ../ydef.h, ystdex::to_array,
//	std::underlying_type;
#include <libdefect/string.h> // for std::char_traits, std::initializer_list,
//	and std::to_string;
#include <cstdio> // for std::vsnprintf
#include <cstdarg>

namespace ystdex
{

/*!
\brief 字符串特征。
\since build 304
*/
template<typename _tString>
struct string_traits
{
	typedef typename std::remove_reference<_tString>::type string_type;
	typedef typename string_type::value_type value_type;
	typedef typename std::char_traits<value_type> traits_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef std::initializer_list<value_type> initializer;
};


/*!	\defgroup string_algorithms String Algorithms
\addtogroup algorithms
\brief 字符串算法。
\since build 304
*/

/*!
\ingroup string_algorithms
\brief 删除字符串中指定的连续前缀字符。
\since build 304
*/
template<class _tString>
inline _tString&
ltrim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return str.erase(0, str.find_first_not_of(t));
}

/*!
\ingroup string_algorithms
\brief 删除字符串中指定的连续后缀字符。
\since build 304
*/
template<class _tString>
inline _tString&
rtrim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return str.erase(str.find_last_not_of(t) + 1);
}

/*!
\ingroup string_algorithms
\brief 删除字符串中指定的连续前缀与后缀字符。
\since build 304
*/
template<class _tString>
inline _tString&
trim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return ystdex::ltrim(ystdex::rtrim(str, t));
}

/*!
\ingroup string_algorithms
\brief 取删除前缀和后缀的子字符串。
\pre 断言：删除的字符串不大于串长。
\since build 304
*/
//@{
template<typename _tString>
inline _tString
get_mid(const _tString& str, typename _tString::size_type l = 1)
{
	yassume(!(str.size() < l * 2));

	return str.substr(l, str.size() - l * 2);
}
template<typename _tString>
inline _tString
get_mid(const _tString& str, typename _tString::size_type l,
	typename _tString::size_type r)
{
	yassume(!(str.size() < l + r));

	return str.substr(l, str.size() - l - r);
}
//@}

/*!
\ingroup string_algorithms
\brief 以指定字符分割字符串。
\since build 304
*/
template<typename _fPred, typename _fInsert, typename _tIn>
_tIn
split(_tIn b, _tIn e, _fPred is_delim, _fInsert insert)
{
	_tIn i(b);

	while(b != e)
	{
		if(is_delim(*b) && i != b)
		{
			insert(i, b);
			i = b;
		}
		++b;
	}
	if(i != b)
		insert(i, b);
	return i;
}


/*!
\brief 转换为字符串。
\note 可与 <tt>std::to_string</tt> 共用以避免某些类型转换警告，
	如 g++ 的 [-Wsign-promo] 。
\since build 308
*/
//@{
inline std::string
to_string(unsigned char val)
{
	return std::to_string(unsigned(val));
}
inline std::string
to_string(unsigned short val)
{
	return std::to_string(unsigned(val));
}
template<typename _type>
inline std::string
to_string(_type val, typename
	std::enable_if<std::is_enum<_type>::value, int>::type = 0)
{
	using std::to_string;
	using ystdex::to_string;

	return to_string(typename std::underlying_type<_type>::type(val));
}
//@}


/*!
\brief 以 C 标准输出格式的输出 std::basic_string 实例的对象。
\todo 提供 char 以外的模板参数的正确实现。
\note Clang++ 对于模板声明直接提示格式字符串类型错误。
\since build 322
*/
template<typename _tChar>
std::basic_string<_tChar>
sfmt(const _tChar* fmt, ...)
{
    std::va_list args;

    va_start(args, fmt);

    std::string str(static_cast<size_t>(std::vsnprintf(nullptr, 0, fmt, args)),
		_tChar());

    std::vsprintf(&str[0], fmt, args);
    va_end(args);
    return str;
}

/*!
\brief 显式实例化：以 C 标准输出格式的输出 std::string 对象。
\see sfmt 模板声明。
\since build 350
*/
template
#if defined _WIN32 && !defined __USE_MINGW_ANSI_STDIO
YB_ATTR(format (ms_printf, 1, 2))
#else
YB_ATTR(format (printf, 1, 2))
#endif
std::string
sfmt<char>(const char*, ...);

} // namespace ystdex;

#endif

