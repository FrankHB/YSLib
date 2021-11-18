/*
	© 2011-2019, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file type_traits.hpp
\ingroup YStandardEx
\brief ISO C++ 类型特征扩展。
\version r2038
\author FrankHB <frankhb1989@gmail.com>
\since build 201
\par 创建时间:
	2015-11-04 09:34:17 +0800
\par 修改时间:
	2021-11-11 20:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TypeTraits
*/


#ifndef YB_INC_ystdex_type_traits_hpp_
#define YB_INC_ystdex_type_traits_hpp_ 1

#include "invoke.hpp" // for "type_inspection.hpp", "integral_constant.hpp",
//	"invoke.hpp", <type_traits>, nonesuch, any_constructible, std::declval,
//	and_;
#include "swap.hpp" // for "swap.hpp";

namespace ystdex
{

//! \ingroup transformation_traits
//@{
/*!
\note 参数可能是不完整类型。
\note 保证不依赖非推导上下文实现简单类型操作，可用于可推导的转换函数的名称。
\note 和标准库对应的转换特征不同，引起 void 引用、引用的指针等的构造可能直接不合式。
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

//! \since build 865
template<typename _type>
using increment_t = decltype(++std::declval<_type>());

//! \since build 867
//@{
template<typename _type>
using increment_post_t = decltype(std::declval<_type>()--);

template<typename _type>
using decrement_t = decltype(++std::declval<_type>());

template<typename _type>
using decrement_post_t = decltype(std::declval<_type>()--);

template<typename _type, typename _type2>
using subscript_t = decltype(std::declval<_type>()[std::declval<_type2>()]);

#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Push
	YB_Diag_Ignore(float-equal)
#endif
template<typename _type, typename _type2 = _type>
using equal_t = decltype(std::declval<_type>() == std::declval<_type2>());

template<typename _type, typename _type2 = _type>
using not_equal_t = decltype(std::declval<_type>() != std::declval<_type2>());
#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Pop
#endif
//@}


//! \since build 830
//@{
template<typename _type>
using first_t = decltype(std::declval<_type>().first);

template<typename _type>
using second_t = decltype(std::declval<_type>().second);
//@}
//@}
//@}


//! \ingroup binary_type_traits
//@{
/*!
\brief 判断是否存在合式的结果为非 void 类型的 [] 操作符接受指定类型的表达式。
\since build 399
*/
template<typename _type1, typename _type2>
struct has_subscription
	: bool_<is_detected<subscript_t, _type1, _type2>::value
	&& !is_void<detected_t<subscript_t, _type1, _type2>>::value>
{};


/*!
\brief 判断是否存在合式的结果可转换为 bool 类型的 == 操作符接受指定类型的表达式。
\since build 649
*/
template<typename _type, typename _type2 = _type>
struct has_equality_operator
	: is_detected_convertible<bool, equal_t, _type, _type2>
{};
//@}

} // namespace ystdex;

#endif

