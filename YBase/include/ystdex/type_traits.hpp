/*
	© 2011-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file type_traits.hpp
\ingroup YStandardEx
\brief ISO C++ 类型特征扩展。
\version r1931
\author FrankHB <frankhb1989@gmail.com>
\since build 201
\par 创建时间:
	2015-11-04 09:34:17 +0800
\par 修改时间:
	2018-07-25 00:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TypeTraits
*/


#ifndef YB_INC_ystdex_type_traits_hpp_
#define YB_INC_ystdex_type_traits_hpp_ 1

#include "invoke.hpp" // for "invoke.hpp", <type_traits>, nonesuch,
//	any_constructible, std::declval, and_;

namespace ystdex
{

/*!
\brief 引入 std::swap 实现为 ADL 提供重载的命名空间。
\note 在这个命名空间中 swap 不应指称 ystdex 中的名称以避免歧义。
\since build 682
*/
namespace dep_swap
{

using std::swap;

//! \since build 496
nonesuch
swap(any_constructible, any_constructible);

template<typename _type, typename _type2>
struct yimpl(helper)
{
	static yconstexpr const bool value = !is_same<decltype(
		swap(std::declval<_type>(), std::declval<_type2>())), nonesuch>::value;

	//! \since build 694
	helper()
		ynoexcept_spec(swap(std::declval<_type>(), std::declval<_type2>()))
	{}
};

} // namespace dep_swap;

/*!
\ingroup type_traits_operations
\see ISO C++11 [swappable.requirements] 。
\since build 586
*/
//@{
//! \brief 判断是否可以调用 \c swap 。
//@{
//! \ingroup binary_type_traits
template<typename _type, typename _type2>
struct is_swappable_with
	: bool_<yimpl(dep_swap::helper<_type, _type2>::value)>
{};


//! \ingroup unary_type_traits
template<typename _type>
struct is_swappable
	: and_<is_referenceable<_type>, is_swappable_with<_type&, _type&>>
{};
//@}


//! \brief 判断是否可以无抛出地调用 \c swap 。
//@{
template<typename _type, typename _type2>
struct is_nothrow_swappable_with : is_nothrow_default_constructible<
	yimpl(dep_swap::helper<_type, _type2>)>
{};


template<typename _type>
struct is_nothrow_swappable
	: and_<is_referenceable<_type>, is_nothrow_swappable_with<_type&, _type&>>
{};
//@}
//@}


//! \ingroup transformation_traits
//@{
/*!
\note 保证不依赖非推导上下文实现简单类型操作，可用于可推导的转换函数的名称。
\sa CWG 395
\since build 756
*/
//@{
template<typename _type>
using id_t = _type;

template<typename _type>
using add_ptr_t = _type*;

template<typename _type>
using add_ref_t = _type&;

template<typename _type>
using add_rref_t = _type&&;
//@}


//! \since build 669
//@{
template<typename _type>
using addrof_t = decltype(&std::declval<_type>());

template<typename _type>
using indirect_t = decltype(*std::declval<_type>());
//@}

//! \since build 671
template<typename _type>
using indirect_element_t = remove_reference_t<indirect_t<_type>>;


//! \since build 830
//@{
template<typename _type>
using first_t = decltype(std::declval<_type>().first);

template<typename _type>
using second_t = decltype(std::declval<_type>().second);
//@}
//@}
//@}

} // namespace ystdex;

#endif

