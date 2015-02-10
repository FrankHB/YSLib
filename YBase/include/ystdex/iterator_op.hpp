/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file iterator_op.hpp
\ingroup YStandardEx
\brief 迭代器操作。
\version r107
\author FrankHB <frankhb1989@gmail.com>
\since build 576
\par 创建时间:
	2015-02-09 11:28:52 +0800
\par 修改时间:
	2015-02-10 13:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::IteratorOperation
*/


#ifndef YB_INC_ystdex_iterator_op_hpp_
#define YB_INC_ystdex_iterator_op_hpp_ 1

#include "type_op.hpp" // for std::pair, std::declval, ystdex::enable_if_t,
//	ystdex::is_same, std::pair, std::declval;
#include "cassert.h" // for yconstraint;
#include "deref_op.hpp" // for ystdex::is_undereferenceable;
#include <iterator> // for std::next, std::prev;

namespace ystdex
{

/*!
\ingroup metafunctions
\brief 选择迭代器类型的特定重载避免和其它类型冲突。
\sa enable_if_t
\since build 483
*/
template<typename _tParam, typename = yimpl(std::pair<
	decltype(*std::declval<_tParam&>()), decltype(++std::declval<_tParam&>())>)>
using enable_for_iterator_t = enable_if_t<
	is_same<decltype(++std::declval<_tParam&>()), _tParam&>::value, int>;


/*!	\defgroup iterator_operations Iterator Operations
\ingroup iterators
\brief 迭代器操作。
\since build 375
*/
//@{
/*!
\brief 迭代器指向的值满足条件时取邻接迭代器，否则取原值。
\param i 指定的迭代器。
\pre 迭代器可解引用，蕴含断言：<tt>!is_undereferenceable(i)</tt> 。
*/
//@{
template<typename _tIn, typename _fPred>
_tIn
next_if(_tIn i, _fPred f,
	typename std::iterator_traits<_tIn>::difference_type n = 1)
{
	yconstraint(!is_undereferenceable(i));
	return f(*i) ? std::next(i, n) : i;
}
template<typename _tIn, typename _type>
_tIn
next_if_eq(_tIn i, const _type& val,
	typename std::iterator_traits<_tIn>::difference_type n = 1)
{
	yconstraint(!is_undereferenceable(i));
	return *i == val ? std::next(i, n) : i;
}
//@}

/*!
\brief 迭代器指向的值满足条件时取反向邻接迭代器，否则取原值。
\pre 迭代器可解引用。
*/
//@{
template<typename _tBi, typename _fPred>
_tBi
prev_if(_tBi i, _fPred f,
	typename std::iterator_traits<_tBi>::difference_type n = 1)
{
	return f(*i) ? std::prev(i, n) : i;
}
template<typename _tBi, typename _type>
_tBi
prev_if_eq(_tBi i, const _type& val,
	typename std::iterator_traits<_tBi>::difference_type n = 1)
{
	return *i == val ? std::prev(i, n) : i;
}
//@}

//@}

} // namespace ystdex;

#endif

