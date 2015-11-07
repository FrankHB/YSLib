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
\version r468
\author FrankHB <frankhb1989@gmail.com>
\since build 333
\par 创建时间:
	2013-09-24 22:29:55 +0800
\par 修改时间:
	2015-11-06 12:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Tuple
*/


#ifndef YB_INC_ystdex_tuple_hpp_
#define YB_INC_ystdex_tuple_hpp_ 1

#include "integer_sequence.hpp" // vseq::defer, index_sequence;
#include <tuple>

namespace ystdex
{

inline namespace cpp2014
{

/*!
\ingroup transformation_traits
\since build 477
*/
//@{
#if __cpp_lib_tuple_element_t >= 201402 || __cplusplus > 201103L
using std::tuple_element_t;
#else
template<size_t _vIdx, typename _type>
using tuple_element_t = _t<std::tuple_element<_vIdx, _type>>;
#endif
//@}

} // inline namespace cpp2014;


//! \since build 589
//@{
namespace vseq
{

//! \since build 650
template<template<typename...> class _gfunc, typename... _types>
struct defer<_gfunc, std::tuple<_types...>, void_t<_gfunc<_types...>>>
{
	using type = _gfunc<_types...>;
};


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
struct seq_size<std::tuple<_types...>>
	: std::tuple_size<std::tuple<_types...>>::type
{};


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
	using head = _t<parts>;
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


/*!
\brief 可变参数类型列表延迟求值。
\sa vseq::defer
\since build 650
*/
template<template<typename...> class _gfunc, typename... _types>
struct vdefer : vseq::defer<_gfunc, std::tuple<_types...>>
{};

} // namespace ystdex;

#endif

