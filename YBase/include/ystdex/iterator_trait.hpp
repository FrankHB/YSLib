/*
	© 2014-2016, 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file iterator_trait.hpp
\ingroup YStandardEx
\brief 迭代器特征。
\version r127
\author FrankHB <frankhb1989@gmail.com>
\since build 627
\par 创建时间:
	2015-08-30 16:29:07 +0800
\par 修改时间:
	2019-09-04 11:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::IteratorTrait
*/


#ifndef YB_INC_ystdex_iterator_trait_hpp_
#define YB_INC_ystdex_iterator_trait_hpp_ 1

#include "type_traits.hpp" // for std::declval, enable_if_t, are_same,
//	indirect_t, increment_t;
#include <iterator> // for std::iterator_traits;

namespace ystdex
{

//! \ingroup binary_type_traits
//@{
/*!
\brief 判断迭代器是否具有相同的指定的类别。
\since build 865
*/
template<class _tTag, typename _tIter>
struct has_iterator_category : is_convertible<typename
	std::iterator_traits<_tIter>::iterator_category, _tTag>
{};


/*!
\brief 判断迭代器是否具有相同的指定的值类型。
\since build 866
*/
template<typename _type, typename _tIter>
using has_iterator_value_type
	= is_same<typename std::iterator_traits<_tIter>::value_type, _type>;
//@}


/*!
\ingroup type_traits_operations
\brief 判断若干个迭代器是否具有相同的指定的类别。
\since build 627
*/
template<class _tTag, typename... _tIters>
struct have_same_iterator_category : and_<is_convertible<
	typename std::iterator_traits<_tIters>::iterator_category, _tTag>...>
{};


//! \since build 865
namespace details
{

template<typename _tParam>
using iterator_reqirement_t = is_same<increment_t<_tParam&>, _tParam&>;

template<typename _tParam>
using input_iterator_reqirement_t = and_<iterator_reqirement_t<_tParam>,
	is_convertible<typename std::iterator_traits<_tParam>::iterator_category,
	std::input_iterator_tag>>;

} // namespace details;

/*!
\ingroup metafunctions
\note 约束的迭代器类型条件仅判断自增、间接操作（解引用）和（可选的）类别。
\sa enable_if_t
*/
//@{
/*!
\brief 选择迭代器类型的特定重载避免和其它类型冲突。
\since build 483
*/
template<typename _tParam, typename _type = void, yimpl(typename
	= indirect_t<_tParam&>, typename = increment_t<_tParam&>)>
using enable_for_iterator_t
	= enable_if_t<details::iterator_reqirement_t<_tParam>::value, _type>;

/*!
\brief 选择输入迭代器类型的特定重载避免和其它类型冲突。
\since build 865
*/
template<typename _tParam, typename _type = void, yimpl(typename
	= indirect_t<_tParam&>, typename = increment_t<_tParam&>)>
using enable_for_input_iterator_t
	= enable_if_t<details::input_iterator_reqirement_t<_tParam>::value, _type>;
//@}

} // namespace ystdex;

#endif

