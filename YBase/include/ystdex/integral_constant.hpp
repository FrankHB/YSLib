/*
	© 2012, 2015-2016, 2018-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file integral_constant.hpp
\ingroup YStandardEx
\brief 整数类型常量。
\version r1937
\author FrankHB <frankhb1989@gmail.com>
\since build 832
\par 创建时间:
	2018-07-23 17:22:36 +0800
\par 修改时间:
	2019-01-19 18:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::IntegralConstant
*/


#ifndef YB_INC_ystdex_integral_constant_hpp_
#define YB_INC_ystdex_integral_constant_hpp_ 1

#include "../ydef.h" // for <type_traits>, __cpp_lib_bool_constant,
//	std::conditional;

namespace ystdex
{

/*!	\defgroup template_meta_programming Template Meta Programming
\brief 模板元编程。
\note 作为 ISO C++ 定义的扩展，以下类别中的接口包括也类模板对应的别名模板。
\since build 288
*/

/*!	\defgroup meta_types Meta Types
\ingroup template_meta_programming
\brief 元类型。
\since build 288
*/

/*!	\defgroup meta_operations Meta Operations
\ingroup template_meta_programming
\brief 元操作。
\since build 288
*/

/*!	\defgroup metafunctions Metafunctions
\ingroup meta_operations
\brief 元函数。
\warning 其中的类类型通常不使用对象，一般可被继承但非虚析构。
\see http://www.boost.org/doc/libs/1_50_0/libs/mpl/doc/refmanual/metafunction.html 。
\since build 333
*/


/*!
\brief 包含 ISO C++ 2011 引入的名称的命名空间。
\since build 607
*/
inline namespace cpp2011
{

//! \since build 245
//@{
using std::integral_constant;
using std::true_type;
using std::false_type;
//@}

} // inline namespace cpp2011;


//! \ingroup meta_types
//@{
/*!
\brief 整数常量类型别名。
\since build 729
*/
//@{
#define YB_Impl_DeclIntT(_n, _t) \
	template<_t _vInt> \
	using _n = integral_constant<_t, _vInt>;
#define YB_Impl_DeclIntTDe(_t) YB_Impl_DeclIntT(_t##_, _t)

YB_Impl_DeclIntTDe(bool)
YB_Impl_DeclIntTDe(char)
YB_Impl_DeclIntTDe(int)
YB_Impl_DeclIntT(llong_, long long)
YB_Impl_DeclIntTDe(long)
YB_Impl_DeclIntTDe(short)
YB_Impl_DeclIntT(ullong_, unsigned long long)
YB_Impl_DeclIntT(ulong_, unsigned long)
YB_Impl_DeclIntT(uint_, unsigned)
YB_Impl_DeclIntT(ushort_, unsigned short)
YB_Impl_DeclIntTDe(size_t)
YB_Impl_DeclIntTDe(ptrdiff_t)

#undef YB_Impl_DeclIntTDe
#undef YB_Impl_DeclIntT

using true_ = bool_<true>;
using false_ = bool_<false>;
//@}


/*!
\brief 包含 ISO C++ 2017 引入的名称的命名空间。
\since build 831
*/
inline namespace cpp2017
{

/*!
\brief bool 常量。
\see WG21 N4389 。
\see WG21 N4527 20.10.3[meta.help] 。
\since build 617
*/
#if __cpp_lib_bool_constant >= 201505L
using std::bool_constant;
#else
template<bool _b>
using bool_constant = bool_<_b>;
#endif
//@}

} // inline namespace cpp2017;


/*!
\ingroup metafunctions
\brief 逻辑操作元函数。
*/
//@{
/*!
\note 接口和 libstdc++ 实现以及 Boost.MPL 兼容。
\since build 578
*/
//@{
//! \brief 逻辑与。
//@{
template<class...>
struct and_;

template<>
struct and_<> : true_
{};

template<class _b1>
struct and_<_b1> : _b1
{};

//! \since build 671
template<class _b1, class _b2, class... _bn>
struct and_<_b1, _b2, _bn...>
	: std::conditional<_b1::value, and_<_b2, _bn...>, _b1>::type
{};
//@}


//! \brief 逻辑或。
//@{
template<class...>
struct or_;

template<>
struct or_<> : false_
{};

template<class _b1>
struct or_<_b1> : _b1
{};

//! \since build 671
template<class _b1, class _b2, class... _bn>
struct or_<_b1, _b2, _bn...>
	: std::conditional<_b1::value, _b1, or_<_b2, _bn...>>::type
{};
//@}


//! \brief 逻辑非。
template<class _b>
struct not_ : bool_<!_b::value>
{};
//@}


//! \since build 851
//@{
//! \brief 逻辑与非。
template<class... _bSeq>
struct nand_ : not_<and_<_bSeq...>>
{};

//! \brief 逻辑或非。
template<class... _bSeq>
struct nor_ : not_<or_<_bSeq...>>
{};
//@}
//@}


inline namespace cpp2017
{

/*!
\ingroup YBase_replacement_features
\see WG21 P0013R1 。
\see WG21 N4606 20.15.8[meta.logical] 。
\since build 723
*/
//@{
template<class... _b>
using conjunction = and_<_b...>;

template<class... _b>
using disjunction = or_<_b...>;

template<class _b>
using negation = not_<_b>;
//@}

} // inline namespace cpp2017;

} // namespace ystdex;

#endif

