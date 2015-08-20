/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file range.hpp
\ingroup YStandardEx
\brief 范围操作。
\version r141
\author FrankHB <frankhb1989@gmail.com>
\since build 624
\par 创建时间:
	2015-08-18 22:33:54 +0800
\par 修改时间:
	2015-08-19 00:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Range
*/


#ifndef YB_INC_ystdex_range_hpp_
#define YB_INC_ystdex_range_hpp_ 1

#include "../ydef.h"
#include <iterator> // for std::begin, std::end;

namespace ystdex
{

//! \since build 624
//@{
inline namespace cpp2011
{

using std::begin;
using std::end;

} // inline namespace cpp2011;

inline namespace cpp2014
{

#if (__cplusplus >= 201402L && (!defined(__GLIBCXX__) \
	|| __GLIBCXX__ >= 20150119)) || (_LIBCXX_VERSION >= 1101 \
	&& _LIBCPP_STD_VER > 11) || YB_IMPL_MSCPP >= 1800
using std::cbegin;
using std::cend;
using std::rbegin;
using std::rend;
using std::crbegin;
using std::crend;
#else
// TODO: 'constexpr' on 'std::begin' and 'std::end' for array types since
//	C++14?

template<typename _type>
yconstfn auto
cbegin(const _type& c) ynoexcept_spec(std::begin(c)) -> decltype(std::begin(c))
{
	return std::begin(c);
}

template<typename _type>
yconstfn auto
cend(const _type& c) ynoexcept_spec(std::end(c)) -> decltype(std::end(c))
{
	return std::end(c);
}

template <class _type>
auto rbegin(_type& c) -> decltype(c.rbegin())
{
	return c.rbegin();
}
template <class _type>
auto rbegin(const _type& c) -> decltype(c.rbegin())
{
	return c.rbegin();
}
template<typename _type, size_t _vN>
std::reverse_iterator<_type*>
rbegin(_type(&arr)[_vN])
{
	return std::reverse_iterator<_type*>(arr + _vN);
}
template<typename _type>
std::reverse_iterator<const _type*>
rbegin(std::initializer_list<_type> il)
{
	return std::reverse_iterator<const _type*>(il.end());
}

template <class _type>
auto rend(_type& c) -> decltype(c.rend())
{
	return c.rend();
}
template <class _type>
auto rend(const _type& c) -> decltype(c.rend())
{
	return c.rend();
}
template<typename _type, size_t _vN>
std::reverse_iterator<_type*>
rend(_type(&arr)[_vN])
{
	return std::reverse_iterator<_type*>(arr);
}
template<typename _type>
std::reverse_iterator<const _type*>
rend(std::initializer_list<_type> il)
{
	return std::reverse_iterator<const _type*>(il.begin());
}

template <typename _type>
auto crbegin(const _type& c) -> decltype(ystdex::rbegin(c))
{
	return ystdex::rbegin(c);
}

template <typename _type>
auto crend(const _type& c) -> decltype(ystdex::rend(c))
{
	return ystdex::rend(c);
}
#endif

} // inline namespace cpp2014;
//@}

} // namespace ystdex;

#endif

