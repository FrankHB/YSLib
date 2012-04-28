/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file string.hpp
\ingroup YStandardEx
\brief YCLib ISO C++ 标准字符串扩展。
\version r1147;
\author FrankHB<frankhb1989@gmail.com>
\since build 304 。
\par 创建时间:
	2012-04-26 20:12:19 +0800;
\par 修改时间:
	2012-04-26 20:30 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YStandardEx::CString;
*/


#ifndef YCL_INC_STRING_HPP_
#define YCL_INC_STRING_HPP_

#include "utility.hpp" // for ../ydef.h, ystdex::make_array;
#include <string> // for std::char_traits and std::initializer_list;

namespace ystdex
{

/*!
\brief 字符串特征。
\since build 304 。
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
\since build 304 。
*/

/*!
\ingroup string_algorithms
\brief 删除字符串中指定的连续前缀字符。
\since build 304 。
*/
template<class _tString>
inline _tString&
ltrim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &make_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return str.erase(0, str.find_first_not_of(t));
}

/*!
\ingroup string_algorithms
\brief 删除字符串中指定的连续后缀字符。
\since build 304 。
*/
template<class _tString>
inline _tString&
rtrim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &make_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return str.erase(str.find_last_not_of(t) + 1);
}

/*!
\ingroup string_algorithms
\brief 删除字符串中指定的连续前缀与后缀字符。
\since build 304 。
*/
template<class _tString>
inline _tString&
trim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &make_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return ltrim(rtrim(str, t));
}

/*!
\ingroup string_algorithms
\brief 取删除前缀和后缀的子字符串。
\pre 断言检查：删除的字符串不大于串长。
\since build 304 。
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
\since build 304 。
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

} // namespace ystdex;

#endif

