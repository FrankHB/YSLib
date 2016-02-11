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
\version r556
\author FrankHB <frankhb1989@gmail.com>
\since build 333
\par 创建时间:
	2013-09-24 22:29:55 +0800
\par 修改时间:
	2016-02-11 16:06 +0800
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


template<typename _tHead, typename... _tTail>
struct front<std::tuple<_tHead, _tTail...>>
{
	using type = _tHead;
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
	using type = apply_t<_fBinary, _tState, _type>;
};

template<class _fBinary, typename _tState, typename... _types>
struct fold<_fBinary, _tState, std::tuple<_types...>>
{
private:
	using parts = split_n<sizeof...(_types) / 2, std::tuple<_types...>>;
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
\since build 651
\see WG21/P0088R0 。
*/
//@{
//! \brief 表示没有找到的索引。
static yconstexpr const auto tuple_not_found(size_t(-1));


namespace details
{

template<class _tSize, class _tIndex>
using index_conv = integral_constant<size_t,
	_tSize::value == _tIndex::value ? tuple_not_found : _tIndex::value>;

} // namespace details;

/*!
\ingroup metafunctions
\brief 查找类型对应的索引位置，若不存在则为 tuple_not_found 。
\sa tuple_not_found
*/
//@{
template<typename, class>
struct tuple_find;

template<typename _type, class _tTuple>
struct tuple_find<_type, const _tTuple> : tuple_find<_type, _tTuple>
{};

template<typename _type, class _tTuple>
struct tuple_find<_type, volatile _tTuple> : tuple_find<_type, _tTuple>
{};

template<typename _type, class _tTuple>
struct tuple_find<_type, const volatile _tTuple> : tuple_find<_type, _tTuple>
{};

template<typename _type, typename... _types>
struct tuple_find<_type, std::tuple<_types...>>
	: details::index_conv<vseq::seq_size<std::tuple<_types...>>,
	vseq::find<std::tuple<_types...>, _type>>
{};

template<typename _type, typename _type1, typename _type2>
struct tuple_find<_type, std::pair<_type1, _type2>>
	: tuple_find<_type, std::tuple<_type1, _type2>>
{};
//@}
//@}


/*!
\ingroup metafunctions
\brief 可变参数类型列表延迟求值。
\sa vseq::defer
\since build 650
*/
template<template<typename...> class _gfunc, typename... _types>
struct vdefer : vseq::defer<_gfunc, std::tuple<_types...>>
{};

/*!
\brief 直接接受类型的二元操作合并应用。
\sa vseq::fold
\since build 671
*/
template<class _fBinary, typename _tState, typename... _types>
using vfold = vseq::fold_t<_fBinary, _tState, std::tuple<_types...>>;


namespace vseq
{

/*!
\ingroup metafunction_composition
\brief 引用：延迟求值变换。
\since b651
\todo 支持没有 \c apply 成员的非元函数。
*/
template<class _func>
struct _q
{
	// NOTE: Call of 'defer' is necessary in range-v3 meta. See https://github.com/ericniebler/range-v3/blob/master/include/meta/meta.hpp,
	//	also http://wg21.cmeerw.net/cwg/issue1430. However, here it is natural
	//	and no higher-ranked polymorphism (template template argument) is
	//	directly used.
	// NOTE: Ideally, the template argument should be limited to enable check.
	//	However this is impossible since the arity of '_func::apply' is
	//	unknown.
	template<typename... _types>
	using apply = vdefer<apply, _func, _types...>;
};

}

} // namespace ystdex;

#endif

