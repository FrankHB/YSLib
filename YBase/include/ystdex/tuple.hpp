/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file tuple.hpp
\ingroup YStandardEx
\brief 元组类型和操作。
\version r411
\author FrankHB <frankhb1989@gmail.com>
\since build 333
\par 创建时间:
	2013-09-24 22:29:55 +0800
\par 修改时间:
	2015-07-23 14:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Tuple
*/


#ifndef YB_INC_ystdex_tuple_hpp_
#define YB_INC_ystdex_tuple_hpp_ 1

#include "type_op.hpp"
#include "integer_sequence.hpp"
#include <tuple>

namespace ystdex
{

/*!
\ingroup metafunctions
\since build 477
*/
template<size_t _vIdx, typename _type>
using tuple_element_t = typename std::tuple_element<_vIdx, _type>::type;


//! \since build 589
//@{
namespace vseq
{

template<typename... _types>
struct seq_size<std::tuple<_types...>>
	: std::tuple_size<std::tuple<_types...>>::type
{};


template<typename... _types>
struct clear<std::tuple<_types...>>
{
	using type = std::tuple<>;
};


template<typename... _types1, typename... _types2>
struct concat<std::tuple<_types1...>, std::tuple<_types2...>>
{
	using type = std::tuple<_types1..., _types2...>;
};


template<typename... _types>
struct back<std::tuple<_types...>>
{
	using type = at_t<std::tuple<_types...>,
		seq_size<std::tuple<_types...>>::value - 1>;
};


template<typename _tHead, typename... _tTail>
struct front<std::tuple<_tHead, _tTail...>>
{
	using type = std::tuple<_tHead>;
};


template<typename... _types>
struct pop_back<std::tuple<_types...>>
{
	using type = split_n_t<seq_size<std::tuple<_types...>>::value - 1,
		std::tuple<_types...>>;
};


template<typename _tHead, typename... _tTail>
struct pop_front<std::tuple<_tHead, _tTail...>>
{
	using type = std::tuple<_tTail...>;
};


template<typename... _types, typename _type>
struct push_back<std::tuple<_types...>, _type>
{
	using type = std::tuple<_types..., _type>;
};


template<typename... _types, typename _type>
struct push_front<std::tuple<_types...>, _type>
{
	using type = std::tuple<_type, _types...>;
};


template<typename... _types, size_t... _vIdxSeq>
struct project<std::tuple<_types...>, index_sequence<_vIdxSeq...>>
{
private:
	//! \since build 503
	using tuple_type = std::tuple<_types...>;

public:
#if YB_IMPL_MSCPP
	//! \since build 454 as workaround for Visual C++ 2013
	using type = std::tuple<typename
		std::tuple_element<_vIdxSeq, tuple_type>::type...>;
#else
	using type = std::tuple<tuple_element_t<_vIdxSeq, tuple_type>...>;
#endif
};


template<class _fBinary, typename _tState>
struct fold<_fBinary, _tState, std::tuple<>>
{
	using type = _tState;
};

template<class _fBinary, typename _tState, typename _type>
struct fold<_fBinary, _tState, std::tuple<_type>>
{
	using type = typename _fBinary::template apply<_tState, _type>::type;
};

template<class _fBinary, typename _tState, typename... _types>
struct fold<_fBinary, _tState, std::tuple<_types...>>
{
private:
	using parts = split_n_t<sizeof...(_types) / 2, std::tuple<_types...>>;
	using head = typename parts::type;
	using tail = typename parts::tail;

public:
	using type = fold_t<_fBinary, fold_t<_fBinary, _tState, head>, tail>;
};


template<typename _tInt, _tInt... _vSeq1, _tInt... _vSeq2>
struct vec_add<std::tuple<integral_constant<_tInt, _vSeq1>...>,
	std::tuple<integral_constant<_tInt, _vSeq2>...>>
{
	using type = std::tuple<integral_constant<_tInt, _vSeq1 + _vSeq2>...>;
};


template<typename _tInt, _tInt... _vSeq1, _tInt... _vSeq2>
struct vec_subtract<std::tuple<integral_constant<_tInt, _vSeq1>...>,
	std::tuple<integral_constant<_tInt, _vSeq2>...>>
{
	using type = std::tuple<integral_constant<_tInt, _vSeq1 - _vSeq2>...>;
};

} // namespace vseq;
//@}


//! \since build 447
//@{
namespace details
{

template<class, class, class>
struct tuple_element_convertible;

template<class _type1, class _type2>
struct tuple_element_convertible<_type1, _type2, index_sequence<>>
	: true_type
{};

template<typename... _types1, typename... _types2, size_t... _vSeq,
	size_t _vHead>
struct tuple_element_convertible<std::tuple<_types1...>, std::tuple<_types2...>,
	index_sequence<_vHead, _vSeq...>>
{
	static_assert(sizeof...(_types1) == sizeof...(_types2),
		"Mismatched sizes of tuple found.");

private:
	using t1 = std::tuple<_types1...>;
	using t2 = std::tuple<_types2...>;

public:
	static yconstexpr const bool value
		= std::is_convertible<vseq::at<t1, _vHead>, vseq::at<t2, _vHead>>::value
		&& tuple_element_convertible<t1, t2, index_sequence<_vSeq...>>::value;
};

} // namespace details;


template<typename... _tFroms, typename... _tTos>
struct is_covariant<std::tuple<_tFroms...>, std::tuple<_tTos...>>
	: bool_constant<details::tuple_element_convertible<std::tuple<_tFroms...>,
	std::tuple<_tTos...>, index_sequence_for<_tTos...>>::value>
{};


template<typename... _tFroms, typename... _tTos>
struct is_contravariant<std::tuple<_tFroms...>, std::tuple<_tTos...>>
	: bool_constant<details::tuple_element_convertible<std::tuple<_tTos...>,
	std::tuple<_tFroms...>, index_sequence_for<_tTos...>>::value>
{};

} // namespace ystdex;

#endif

