/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file algorithm.hpp
\ingroup YStandardEx
\brief 泛型算法。
\version r1246;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-23 06:10:59 +0800;
\par 修改时间:
	2011-10-25 16:12 +0800;
\par 字符集:
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
	*/


	/*!
	\ingroup algorithms
	\brief 取值类型最小值。
	*/
	template<typename _type>
	inline _type
	vmin(_type x, _type y)
	{
		return y < x ? y : x;
	}
	/*!
	\ingroup algorithms
	\brief 取值类型最小值。
	\note 使用指定判断操作。
	*/
	template<typename _type, typename _fCompare>
	inline _type
	vmin(_type x, _type y, _fCompare comp)
	{
		return comp(y, x) ? y : x;
	}

	/*!
	\ingroup algorithms
	\brief 取值类型最大值。
	*/
	template<typename _type>
	inline _type
	vmax(_type x, _type y)
	{
		return x < y ? y : x;
	}
	/*!
	\ingroup algorithms
	\brief 取值类型最大值。
	\note 使用指定判断操作。
	*/
	template<typename _type, typename _fCompare>
	inline _type
	vmax(_type x, _type y, _fCompare comp)
	{
		return comp(x, y) ? y : x;
	}


	/*!	\defgroup pod_operations POD Type Operations
	\ingroup algorithms
	\brief POD 类型操作。
	\tparam _type 指定对象类型。
	\pre 静态断言： <tt>std::is_pod<typename std::remove_reference<_type>::type>
		::value</tt> 。
	\note 不检查指针是否有效。
	*/
	//@{
	template <class _type>
	inline _type*
	pod_fill(_type* first, _type* last, _type&& value)
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
	\ingroup algorithms
	\brief 删除指定标准容器中所有相同元素。
	*/
	template<typename _tContainer>
	typename _tContainer::size_type
	erase_all(_tContainer& c, const typename _tContainer::value_type& val)
	{
		const auto s(c.size());

		c.erase(std::remove(c.begin(), c.end(), val), c.end());

		return s - c.size();
	}

	/*!
	\ingroup algorithms
	\brief 删除指定标准容器中所有满足条件元素。
	*/
	template<typename _tContainer, typename _fPredicate>
	typename _tContainer::size_type
	erase_all_if(_tContainer& c, _fPredicate pred)
	{
		const auto s(c.size());

		c.erase(std::remove_if(c.begin(), c.end(), pred), c.end());

		return s - c.size();
	}

	/*!
	\ingroup algorithms
	\brief 按指定键值搜索指定映射。
	\return 一个用于表示结果的 std::pair 对象，其成员 first 为迭代器，
		second 表示是否不存在而需要插入。
	\note 行为类似 std::map::operator[] 。
	*/
	template<class _tMap>
	std::pair<typename _tMap::iterator, bool>
	search_map(_tMap& m, const typename _tMap::key_type& k)
	{
		auto i(m.lower_bound(k));

		return std::make_pair(i, (i == m.end() || m.key_comp()(k, i->first)));
	}
}

#ifndef YCL_HAS_BUILTIN_NULLPTR
using ystdex::nullptr;
#endif

#endif

