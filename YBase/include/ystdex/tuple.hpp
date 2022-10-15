/*
	© 2013-2016, 2018-2019, 2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file tuple.hpp
\ingroup YStandardEx
\brief 元组类型和操作。
\version r917
\author FrankHB <frankhb1989@gmail.com>
\since build 333
\par 创建时间:
	2013-09-24 22:29:55 +0800
\par 修改时间:
	2022-10-15 05:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Tuple
*/


#ifndef YB_INC_ystdex_tuple_hpp_
#define YB_INC_ystdex_tuple_hpp_ 1

#include "apply.hpp" // for <tuple>, "apply.hpp", std::tuple_element_t,
//	std::tuple_element, std::tuple, tuple_size, index_sequence,
//	integral_constant, size_t_;

namespace ystdex
{

inline namespace cpp2014
{

/*!
\ingroup transformation_traits
\since build 477
*/
//@{
#if __cpp_lib_tuple_element_t >= 201402L
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

template<typename... _types>
struct seq_size<std::tuple<_types...>> : tuple_size<std::tuple<_types...>>::type
{};


template<typename... _types, size_t... _vIdxSeq>
struct project<std::tuple<_types...>, index_sequence<_vIdxSeq...>>
{
private:
	//! \since build 503
	using tuple_type = std::tuple<_types...>;

public:
	using type = std::tuple<tuple_element_t<_vIdxSeq, tuple_type>...>;
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
\see WG21 P0088R0 。
\see WG21 P0088R1 。
\since build 651
\todo 重命名为 \c variant_npos ，转移到合适的头文件。
*/
//@{
//! \brief 表示没有找到的索引。
static yconstexpr const auto tuple_not_found(size_t(-1));


namespace details
{

template<class _tSize, class _tIndex>
using index_conv = size_t_<_tSize::value == _tIndex::value
	? tuple_not_found : _tIndex::value>;

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

} // namespace ystdex;

#endif

