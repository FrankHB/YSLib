/*
	© 2014-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file iterator_trait.hpp
\ingroup YStandardEx
\brief 迭代器特征。
\version r67
\author FrankHB <frankhb1989@gmail.com>
\since build 627
\par 创建时间:
	2015-08-30 16:29:07 +0800
\par 修改时间:
	2016-02-11 17:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::IteratorTrait
*/


#ifndef YB_INC_ystdex_iterator_trait_hpp_
#define YB_INC_ystdex_iterator_trait_hpp_ 1

#include "type_traits.hpp" // for std::pair, std::declval, enable_if_t,
//	are_same;
#include <iterator> // for std::iterator_traits;

namespace ystdex
{

/*!
\ingroup type_traits_operations
\brief 判断若干个迭代器是否指定的类别。
\since build 627
*/
template<class _type, typename... _tIter>
struct have_same_iterator_category : are_same<_type,
	typename std::iterator_traits<_tIter>::iterator_category...>
{};

/*!
\ingroup metafunctions
\brief 选择迭代器类型的特定重载避免和其它类型冲突。
\sa enable_if_t
\since build 483
*/
template<typename _tParam, typename _type = void, typename = yimpl(std::pair<
	indirect_t<_tParam&>, decltype(++std::declval<_tParam&>())>)>
using enable_for_iterator_t = enable_if_t<
	is_same<decltype(++std::declval<_tParam&>()), _tParam&>::value, _type>;

} // namespace ystdex;

#endif

