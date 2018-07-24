/*
	© 2013-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file tuple.hpp
\ingroup YStandardEx
\brief 元组类型和操作。
\version r737
\author FrankHB <frankhb1989@gmail.com>
\since build 333
\par 创建时间:
	2013-09-24 22:29:55 +0800
\par 修改时间:
	2018-07-20 02:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Tuple
*/


#ifndef YB_INC_ystdex_tuple_hpp_
#define YB_INC_ystdex_tuple_hpp_ 1

#include "integer_sequence.hpp" // for "integer_sequence.hpp", vseq::defer,
//	index_sequence, size_t_;
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
#if __cpp_lib_tuple_element_t >= 201402
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
struct seq_size<std::tuple<_types...>>
	: std::tuple_size<std::tuple<_types...>>::type
{};


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
\note 使用 ADL get 。
\see WG21 N4606 20.5.2.5[tuple.apply]/2 。
\see WG21 P0209R2 。
\since build 735
*/
//@{
namespace details
{

template<typename _type, class _tTuple, size_t... _vIdx>
yconstfn _type
make_from_tuple_impl(_tTuple&& t, index_sequence<_vIdx...>)
{
	using std::get;

	return _type(get<_vIdx>(yforward(t))...);
}

} // namespace details;

//! \brief 从元组构造指定类型的值。
template<typename _type, class _tTuple>
yconstfn _type
make_from_tuple(_tTuple&& t)
{
	return details::make_from_tuple_impl<_type>(yforward(t),
		make_index_sequence<std::tuple_size<decay_t<_tTuple>>::value>());
}
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

