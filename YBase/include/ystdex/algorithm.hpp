/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file algorithm.hpp
\ingroup YStandardEx
\brief 泛型算法。
\version r1319;
\author FrankHB<frankhb1989@gmail.com>
\since build 254 。
\par 创建时间:
	2010-05-23 06:10:59 +0800;
\par 修改时间:
	2012-04-26 20:46 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YStandardEx::Algorithms;
*/


#ifndef YCL_INC_YSTDEX_ALGORITHM_HPP_
#define YCL_INC_YSTDEX_ALGORITHM_HPP_

#include "../ydef.h"
#include <algorithm>

namespace ystdex
{

/*!	\defgroup algorithms Gerneral Algorithms
\brief 算法。
\since build 189 。
*/


/*!	\defgroup pod_operations POD Type Operations
\ingroup algorithms
\brief POD 类型操作。
\tparam _type 指定对象类型。
\pre 静态断言： <tt>std::is_pod<typename std::remove_reference<_type>::type>
	::value</tt> 。
\note 不检查指针是否有效。
\since build 304 。
*/
//@{
template <class _type>
inline _type*
pod_fill(_type* first, _type* last, const _type& value)
{
	static_assert(std::is_pod<typename std::remove_reference<_type>::type>
		::value, "Non-POD type found @ pod_fill;");

	switch((last - first) & 7)
	{
	case 0:
		while(first != last)
		{
			*first = value; ++first;
	case 7: *first = value; ++first;
	case 6: *first = value; ++first;
	case 5: *first = value; ++first;
	case 4: *first = value; ++first;
	case 3: *first = value; ++first;
	case 2: *first = value; ++first;
	case 1: *first = value; ++first;
		}
	}
	return last;
}

template <class _type>
inline _type*
pod_copy_n(const _type* first, std::size_t n, _type* result)
{
	static_assert(std::is_pod<typename std::remove_reference<_type>::type>
		::value, "Non-POD type found @ pod_copy_n;");

	std::memcpy(result, first, sizeof(*first) * n);
	return result + n;
}

template <class _type>
inline _type*
pod_copy(const _type* first, const _type* last, _type* result)
{
	return pod_copy_n(first, last - first, result);
}

template <class _type>
inline _type*
pod_move_n(const _type* first, std::size_t n, _type* result)
{
	static_assert(std::is_pod<typename std::remove_reference<_type>::type>
		::value, "Non-POD type found @ pod_move_n;");

	std::memmove(result, first, sizeof(*first) * n);
	return result + n;
}

template <class _type>
inline _type*
pod_move(const _type* first, const _type* last, _type* result)
{
	return pod_move_n(first, last - first, result);
}
//@}


/*!
\brief 去除迭代器指定的范围中的重复元素，且不改变元素之间的相对顺序。
\param b 输入范围起始迭代器。
\param e 输入范围终止迭代器。
\return 输出范围终止迭代器。
\note 输入和输出范围的起始迭代器相同。
\note 输出范围元素之间的相对顺序和输入的范围保持一致。
\note 时间复杂度： O(n^2) ，其中 n 满足 std::advance(b, n) == e 。
\since build 265 。
*/
template<typename _tForward>
_tForward
stable_range_unique(_tForward first, _tForward last)
{
	_tForward result(first);

	for(_tForward i(first); i != last; ++i)
		if(std::find(first, result, *i) == result)
		{
			std::swap(*i, *result);
			++result;
		}
	return result;
}


/*!
\ingroup algorithms
\brief 插入参数指定的元素到容器。
\since build 274 。
*/
//@{
template<class _tContainer, typename... _tParams>
inline void
assign(_tContainer& con, _tParams&&... args)
{
	con.assign(yforward(args)...);
}
template<class _tContainer, typename _type, size_t _vN>
inline void
assign(_tContainer& con, const _type(&arr)[_vN])
{
	con.assign(arr, arr + _vN);
}
//@}

/*!
\ingroup algorithms
\brief 删除指定序列容器中和指定值的相等的元素。
\since build 289 。
*/
template<typename _tContainer>
void
erase_all(_tContainer& c, const typename _tContainer::value_type& val)
{
	c.erase(std::remove(c.begin(), c.end(), val), c.end());
}
/*!
\ingroup algorithms
\brief 删除指定序列容器中迭代器区间中和指定值的相等的元素。
\pre first 和 last 是 c 的有效的迭代器或 <tt>c.end()</tt> 。
\since build 289 。
*/
template<typename _tContainer, typename _tIn, typename _tValue>
void
erase_all(_tContainer& c, _tIn first, _tIn last, const _tValue& value)
{
	while(first != last)
		if(*first == value)
			c.erase(first++);
		else
			++first;
}

/*!
\ingroup algorithms
\brief 删除指定序列容器中满足条件的元素。
\since build 289 。
*/
template<typename _tContainer, typename _fPredicate>
void
erase_all_if(_tContainer& c, _fPredicate pred)
{
	c.erase(std::remove_if(c.begin(), c.end(), pred), c.end());
}
/*!
\ingroup algorithms
\brief 删除指定序列容器中迭代器区间中满足条件的元素。
\pre first 和 last 是 c 的有效的迭代器或 <tt>c.end()</tt> 。
\since build 289 。
*/
template<typename _tContainer, typename _tIn, typename _fPredicate>
void
erase_all_if(_tContainer& c, _tIn first, _tIn last, _fPredicate pred)
{
	while(first != last)
		if(pred(*first))
			c.erase(first++);
		else
			++first;
}

/*!
\ingroup algorithms
\brief 按指定键值搜索指定映射。
\return 一个用于表示结果的 std::pair 对象，其成员 first 为迭代器，
	second 表示是否不存在而需要插入。
\note 行为类似 std::map::operator[] 。
\since build 173 。
*/
template<class _tMap>
std::pair<typename _tMap::iterator, bool>
search_map(_tMap& m, const typename _tMap::key_type& k)
{
	const auto i(m.lower_bound(k));

	return std::make_pair(i, (i == m.end() || m.key_comp()(k, i->first)));
}

} // namespace ystdex;

#endif

