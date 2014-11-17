/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file algorithm.hpp
\ingroup YStandardEx
\brief 泛型算法。
\version r606
\author FrankHB <frankhb1989@gmail.com>
\since build 254
\par 创建时间:
	2010-05-23 06:10:59 +0800
\par 修改时间:
	2014-11-15 10:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Algorithm
*/


#ifndef YB_INC_ystdex_algorithm_hpp_
#define YB_INC_ystdex_algorithm_hpp_ 1

#include "../ydef.h"
#include <algorithm>
#include "iterator.hpp" // for ystdex::is_pod, ystdex::is_undereferenceable;
#include <cstring> // for std::memcpy, std::memmove;

namespace ystdex
{

/*!	\defgroup algorithms Gerneral Algorithms
\brief 算法。
\note 范围包含序列容器及内建数组等。容器概念和容器要求参见 ISO C++ Clause 23 。
\see WG21/N3936 25.1[algorithms.general] 。
\since build 189
*/
//@{
/*!	\defgroup nonmodifying_algorithms Non-modifying Sequence Operations
\brief 非修改序列操作。
\see WG21/N3936 25.2[alg.nonmodifying] 。
\since build 531
*/
//@{
/*
\tparam _func 用于遍历范围的操作的可调用类型。
\param first 输入范围起始迭代器。
\param last 输入范围终止迭代器。
\param f 遍历操作。
\pre _func 满足 MoveConstructible 要求。
\pre _type 满足 EqualityComparable 要求。
\return 转移的 f 。
\note 时间复杂度： 不大于 <tt>last - first</tt> 次 \c f 的调用。
\see ISO WG21/N3936 25.2.4[alg.foreach] 。
\see 其它参考实现： http://stackoverflow.com/questions/234482/using-stl-to-find-all-elements-in-a-vector 。

分别应用操作至范围 <tt>[first, last)</tt> 内的解应用的迭代器 \c i 满足以下条件：
\li <tt>*i == value</tt>
\li <tt>pred(*i) != false</tt>
*/
//@{
template<typename _tIn, typename _func, typename _type>
_func
for_each_equal(_tIn first, _tIn last, const _type& value, _func f)
{
	for(; first != last; first = std::find(++first, last, value))
		f(*first);
	return f;
}

template<typename _tIn, typename _func, typename _fPred>
_func
for_each_if(_tIn first, _tIn last, _fPred pred, _func f)
{
	for(; first != last; first = std::find_if(++first, last, pred))
		f(*first);
	return f;
}
//@}
//@}


/*!	\defgroup mutating_algorithms Mutating Sequence Operations
\brief 可变序列操作。
\see WG21/N3936 25.3[alg.modifying.operations] 。
\since build 531
*/
//@{
/*!
\brief 指定数量的序列转换。
\tparam _fOp 序列操作类型。
\tparam _tOut 表示结果的输出迭代器类型。
\tparam _tIns 输入迭代器类型。
\pre 迭代器可解引用。
\warning 不检查越界。
\since build 517
*/
template<typename _fOp, typename _tOut, typename... _tIns>
void
transform_n(_fOp op, _tOut result, size_t n, _tIns... iters)
{
	while(n-- != 0)
	{
		yunseq((yconstraint(!is_undereferenceable(result)), 0),
			(yconstraint(!is_undereferenceable(iters)), void(iters), 0)...);
		*result = op((*iters)...);
		yunseq(++result, ++iters...);
	}
}

/*!	\defgroup pod_operations POD Type Operations
\brief POD 类型操作。
\tparam _type 指定对象类型。
\pre 静态断言： <tt>is_pod<remove_reference_t<_type>>::value</tt> 。
\warning 不检查指针是否有效。
\since build 304
*/
//@{
template<class _type>
inline _type*
pod_fill(_type* first, _type* last, const _type& value)
{
	static_assert(is_pod<remove_reference_t<_type>>::value,
		"Non-POD type found @ pod_fill;");

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

template<class _type>
inline _type*
pod_copy_n(const _type* first, size_t n, _type* result)
{
	static_assert(is_pod<remove_reference_t<_type>>::value,
		"Non-POD type found @ pod_copy_n;");

	std::memcpy(result, first, sizeof(*first) * n);
	return result + n;
}

template<class _type>
inline _type*
pod_copy(const _type* first, const _type* last, _type* result)
{
	return ystdex::pod_copy_n(first, last - first, result);
}

template<class _type>
inline _type*
pod_move_n(const _type* first, size_t n, _type* result)
{
	static_assert(is_pod<remove_reference_t<_type>>::value,
		"Non-POD type found @ pod_move_n;");

	std::memmove(result, first, sizeof(*first) * n);
	return result + n;
}

template<class _type>
inline _type*
pod_move(const _type* first, const _type* last, _type* result)
{
	return ystdex::pod_move_n(first, last - first, result);
}
//@}


/*!
\brief 去除迭代器指定的范围中的重复元素，且不改变元素之间的相对顺序。
\param first 输入范围起始迭代器。
\param last 输入范围终止迭代器。
\return 输出范围终止迭代器。
\note 输入和输出范围的起始迭代器相同。
\note 输出范围元素之间的相对顺序和输入的范围保持一致。
\note 时间复杂度： O(n^2) ，其中 n 满足 <tt>std::advance(first, n) == last</tt> 。
\note 使用 ADL 交换。
\see ISO WG21/N3936 25.3.9[alg.unique] 。
\since build 531
*/
//@{
template<typename _tFwd>
_tFwd
stable_unique(_tFwd first, _tFwd last)
{
	_tFwd result(first);

	for(_tFwd i(first); i != last; ++i)
		if(std::find(first, result, *i) == result)
		{
			using std::swap;

			swap(*i, *result);
			++result;
		}
	return result;
}
//! \param pred 比较等价的二元谓词。
template<typename _tFwd, typename _fPred>
_tFwd
stable_unique(_tFwd first, _tFwd last, _fPred pred)
{
	_tFwd result(first);

	for(_tFwd i(first); i != last; ++i)
		if(std::find_if(first, result,
			std::bind(pred, std::ref(*i), std::placeholders::_1)) == result)
		{
			using std::swap;

			swap(*i, *result);
			++result;
		}
	return result;
}
//@}
//@}


/*!	\defgroup sorting_and_related_algorithms Sorting and Related Operations
\brief 排序相关操作。
\see WG21/N3936 25.4[alg.sorting] 。
\since build 531
*/
//@{
/*!
\brief 排序指定序列范围，保留不重复元素的区间。
\tparam _tRandom 随机迭代器类型。
\param first 输入范围起始迭代器。
\param last 输入范围终止迭代器。
\note 时间复杂度：令 N = last - first ， O(N log(N)) + N - 1 次比较。
\return 不重复元素的区间末尾。
\since build 414
*/
template<typename _tRandom>
inline _tRandom
sort_unique(_tRandom first, _tRandom last)
{
	std::sort(first, last);
	return std::unique(first, last);
}
//@}
//@}

} // namespace ystdex;

#endif

