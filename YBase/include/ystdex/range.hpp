/*
	© 2015-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file range.hpp
\ingroup YStandardEx
\brief 范围操作。
\version r354
\author FrankHB <frankhb1989@gmail.com>
\since build 624
\par 创建时间:
	2015-08-18 22:33:54 +0800
\par 修改时间:
	2018-07-11 17:31 +0800
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
//! \since build 664
template<typename _type, size_t _vN>
yconstfn _type*
begin(_type(&&array)[_vN]) ynothrow
{
	return array;
}
using std::end;
//! \since build 664
template<typename _type, size_t _vN>
yconstfn _type*
end(_type(&&array)[_vN]) ynothrow
{
	return array + _vN;
}

} // inline namespace cpp2011;

inline namespace cpp2014
{

#if (__cpp_lib_nonmember_container_access__cplusplus >= 201411 \
	|| (__cplusplus >= 201402L && (!defined(__GLIBCXX__) \
	|| __GLIBCXX__ >= 20150119))) || (_LIBCXX_VERSION >= 1101 \
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

template<typename _tRange>
yconstfn auto
cbegin(const _tRange& c) ynoexcept_spec(std::begin(c))
	-> decltype(std::begin(c))
{
	return std::begin(c);
}

template<typename _tRange>
yconstfn auto
cend(const _tRange& c) ynoexcept_spec(std::end(c)) -> decltype(std::end(c))
{
	return std::end(c);
}

template<class _tRange>
auto
rbegin(_tRange& c) -> decltype(c.rbegin())
{
	return c.rbegin();
}
template<class _tRange>
auto
rbegin(const _tRange& c) -> decltype(c.rbegin())
{
	return c.rbegin();
}
template<typename _type, size_t _vN>
std::reverse_iterator<_type*>
rbegin(_type(&array)[_vN])
{
	return std::reverse_iterator<_type*>(array + _vN);
}

template<typename _tElem>
std::reverse_iterator<const _tElem*>
rbegin(std::initializer_list<_tElem> il)
{
	return std::reverse_iterator<const _tElem*>(il.end());
}

template<class _tRange>
auto rend(_tRange& c) -> decltype(c.rend())
{
	return c.rend();
}
template<class _tRange>
auto rend(const _tRange& c) -> decltype(c.rend())
{
	return c.rend();
}
template<typename _type, size_t _vN>
std::reverse_iterator<_type*>
rend(_type(&array)[_vN])
{
	return std::reverse_iterator<_type*>(array);
}
template<typename _tElem>
std::reverse_iterator<const _tElem*>
rend(std::initializer_list<_tElem> il)
{
	return std::reverse_iterator<const _tElem*>(il.begin());
}

template<typename _tRange>
auto
crbegin(const _tRange& c) -> decltype(ystdex::rbegin(c))
{
	return ystdex::rbegin(c);
}

template<typename _tRange>
auto
crend(const _tRange& c) -> decltype(ystdex::rend(c))
{
	return ystdex::rend(c);
}
#endif

//! \since build 664
//@{
template<typename _type, size_t _vN>
yconstfn _type*
cbegin(_type(&&array)[_vN]) ynothrow
{
	return array;
}

template<typename _type, size_t _vN>
yconstfn _type*
cend(_type(&&array)[_vN]) ynothrow
{
	return array + _vN;
}

template<typename _type, size_t _vN>
std::reverse_iterator<_type*>
rbegin(_type(&&array)[_vN])
{
	return std::reverse_iterator<_type*>(array + _vN);
}

template<typename _type, size_t _vN>
std::reverse_iterator<_type*>
rend(_type(&&array)[_vN])
{
	return std::reverse_iterator<_type*>(array);
}

#if __cplusplus <= 201402L
//! \see CWG 1591 。
//@{
template<typename _tElem>
yconstfn const _tElem*
cbegin(std::initializer_list<_tElem> il) ynothrow
{
	return il.begin();
}

//! \see CWG 1591 。
template<typename _tElem>
yconstfn const _tElem*
cend(std::initializer_list<_tElem> il) ynothrow
{
	return il.end();
}

template<typename _tElem>
yconstfn std::reverse_iterator<const _tElem*>
crbegin(std::initializer_list<_tElem> il) ynothrow
{
	return ystdex::rbegin(il);
}

template<typename _tElem>
yconstfn std::reverse_iterator<const _tElem*>
crend(std::initializer_list<_tElem> il) ynothrow
{
	return ystdex::rend(il);
}
//@}
#endif
//@}

} // inline namespace cpp2014;
//@}

/*!
\brief 类容器访问。
\see WG21 N4280 。
\see WG21 N4567 24.8[iterator.container] 。
\since build 663
*/
//@{
template<class _tRange>
yconstfn auto
size(const _tRange& c) -> decltype(c.size())
{
	return c.size();
}
template<typename _type, size_t _vN>
yconstfn size_t
size(const _type(&)[_vN]) ynothrow
{
	return _vN;
}
#if __cplusplus <= 201402L
//! \see CWG 1591 。
template<typename _tElem>
yconstfn size_t
size(std::initializer_list<_tElem> il) ynothrow
{
	return il.size();
}
#endif

template<class _tRange>
yconstfn auto
empty(const _tRange& c) -> decltype(c.empty())
{
	return c.empty();
}
template<typename _type, size_t _vN>
yconstfn bool
empty(const _type(&)[_vN]) ynothrow
{
	return {};
}
template<typename _type, size_t _vN>
yconstfn bool
empty(const _type(&&)[_vN]) ynothrow
{
	return {};
}
template<typename _tElem>
yconstfn bool
empty(std::initializer_list<_tElem> il) ynothrow
{
	return il.size() == 0;
}

template<typename _tRange>
yconstfn auto
data(_tRange& c) -> decltype(c.data())
{
	return c.data();
}
template<typename _tRange>
yconstfn auto
data(const _tRange& c) -> decltype(c.data())
{
	return c.data();
}
template<typename _type, size_t _vN>
yconstfn _type*
data(_type(&array)[_vN]) ynothrow
{
	return array;
}
template<typename _type, size_t _vN>
yconstfn _type*
data(_type(&&array)[_vN]) ynothrow
{
	return array;
}
template<typename _tElem>
yconstfn const _tElem*
data(std::initializer_list<_tElem> il) ynothrow
{
	return il.begin();
}
//@}

} // namespace ystdex;

#endif

