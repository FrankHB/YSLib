/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file tuple.hpp
\ingroup YStandardEx
\brief 元组类型和操作。
\version r142
\author FrankHB <frankhb1989@gmail.com>
\since build 333
\par 创建时间:
	2013-09-24 22:29:55 +0800
\par 修改时间:
	2014-02-17 00:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Tuple
*/


#ifndef YB_INC_ystdex_tuple_hpp_
#define YB_INC_ystdex_tuple_hpp_ 1

#include "../ydef.h"
#include "type_op.hpp"
#include "variadic.hpp"
#include <tuple>

namespace ystdex
{

/*!
\ingroup metafunctions
\since build 477
*/
template<size_t _vIdx, typename _type>
using tuple_element_t = typename std::tuple_element<_vIdx, _type>::type;


//! \since build 447
//@{
namespace details
{

template<class, class, class>
struct tuple_element_convertible;

template<class _type1, class _type2>
struct tuple_element_convertible<_type1, _type2, variadic_sequence<>>
	: true_type
{};

template<typename... _types1, typename... _types2, size_t... _vSeq,
	size_t _vHead>
struct tuple_element_convertible<std::tuple<_types1...>, std::tuple<_types2...>,
	variadic_sequence<_vHead, _vSeq...>>
{
	static_assert(sizeof...(_types1) == sizeof...(_types2),
		"Mismatched sizes of tuple found.");

private:
	using t1 = std::tuple<_types1...>;
	using t2 = std::tuple<_types2...>;

public:
	static yconstexpr bool value = std::is_convertible<
		sequence_element_t<_vHead, t1>, sequence_element_t<_vHead, t2>>::value
		&& tuple_element_convertible<t1, t2, variadic_sequence<_vSeq...>>
		::value;
};

} // namespace details;


template<typename... _tFroms, typename... _tTos>
struct is_covariant<std::tuple<_tFroms...>, std::tuple<_tTos...>>
	: integral_constant<bool, details::tuple_element_convertible<
	std::tuple<_tFroms...>, std::tuple<_tTos...>,
	make_natural_sequence_t<sizeof...(_tTos)>>::value>
{};


template<typename... _tFroms, typename... _tTos>
struct is_contravariant<std::tuple<_tFroms...>, std::tuple<_tTos...>>
	: integral_constant<bool, details::tuple_element_convertible<
	std::tuple<_tTos...>, std::tuple<_tFroms...>,
	make_natural_sequence_t<sizeof...(_tTos)>>::value>
{};


template<typename... _types1, typename... _types2>
struct sequence_cat<std::tuple<_types1...>, std::tuple<_types2...>>
{
	using type = std::tuple<_types1..., _types2...>;
};


template<size_t _vIdx, typename... _types>
struct sequence_element<_vIdx, std::tuple<_types...>>
	: std::tuple_element<_vIdx, std::tuple<_types...>>
{};


template<typename... _types, size_t... _vIdxSeq>
struct sequence_project<std::tuple<_types...>, variadic_sequence<_vIdxSeq...>>
{
	using type
		= std::tuple<tuple_element_t<_vIdxSeq, std::tuple<_types...>>...>;
};


template<typename... _types>
struct sequence_reverse<std::tuple<_types...>>
{
private:
	using tuple_type = std::tuple<_types...>;

public:
	using type = sequence_project_t<tuple_type, sequence_reverse_t<
		make_natural_sequence_t<std::tuple_size<tuple_type>::value>>>;
};
//@}


//! \since build 449
template<typename _tHead, typename... _tTail>
struct sequence_split<std::tuple<_tHead, _tTail...>>
{
	using type = _tHead;
	using tail = std::tuple<_tTail...>;
};

} // namespace ystdex;

#endif

