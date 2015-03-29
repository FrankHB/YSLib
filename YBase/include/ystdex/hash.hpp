/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file hash.hpp
\ingroup YStandardEx
\brief 散列接口。
\version r161
\author FrankHB <frankhb1989@gmail.com>
\since build 588
\par 创建时间:
	2015-03-28 22:12:11 +0800
\par 修改时间:
	2015-03-28 22:22 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Hash
*/


#ifndef YB_INC_ystdex_hash_hpp_
#define YB_INC_ystdex_hash_hpp_ 1

#include "tuple.hpp" // for ../ydef.h, ystdex::variadic_sequence, std::get,
//	std::tuple, std::declval, ystdex::make_natural_sequence_t, std::pair;
#include <functional> // for std::hash;
#include <numeric> // for std::accumulate;

namespace ystdex
{

/*!	\defgroup hash_extensions Hash Extensions
\brief 散列扩展接口。
\note 当前使用 Boost 定义的接口和近似实现。
\see http://www.boost.org/doc/libs/1_54_0/doc/html/hash/reference.html#boost.hash_combine 。
\since build 421
*/
//@{
/*!
\brief 重复计算散列。
\note <tt>(1UL << 31) / ((1 + std::sqrt(5)) / 4) == 0x9E3779B9</tt> 。
\warning 实现（ Boost 文档作为 Effects ）可能改变，不应作为接口依赖。
*/
template<typename _type>
inline void
hash_combine(size_t& seed, const _type& val)
{
	seed ^= std::hash<_type>()(val) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
}

/*!
\ingroup helper_functions
\brief 重复计算序列散列。
\sa hash_combine
*/
//@{
template<typename _type>
yconstfn size_t
hash_combine_seq(size_t seed, const _type& val)
{
	return ystdex::hash_combine(seed, val), seed;
}
template<typename _type, typename... _tParams>
yconstfn size_t
hash_combine_seq(size_t seed, const _type& x, const _tParams&... args)
{
	return ystdex::hash_combine_seq(ystdex::hash_combine_seq(seed, x), args...);
}
//@}

/*!
\brief 重复对范围计算散列。
\note 使用 ADL 。
*/
//@{
//! \since build 531
template<typename _tIn>
inline size_t
hash_range(size_t seed, _tIn first, _tIn last)
{
	return std::accumulate(first, last, seed,
		[](size_t s, decltype(*first) val){
		hash_combine(s, val);
		return s;
	});
}
template<typename _tIn>
inline size_t
hash_range(_tIn first, _tIn last)
{
	return ystdex::hash_range(0, first, last);
}
//@}


namespace details
{

//! \since build 586
//@{
template<class, class>
struct combined_hash_tuple;

template<typename _type, size_t... _vSeq>
struct combined_hash_tuple<_type, variadic_sequence<_vSeq...>>
{
	static yconstfn size_t
	call(const _type& tp)
		ynoexcept_spec(ystdex::hash_combine_seq(0, std::get<_vSeq>(tp)...))
	{
		return ystdex::hash_combine_seq(0, std::get<_vSeq>(tp)...);
	}
};
//@}

} // namespace details;

/*!
\brief 使用 std::hash 和 ystdex::hash_combine_seq 实现的对特定类型的散列。
\since build 468
*/
//@{
template<typename...>
struct combined_hash;

template<typename _type>
struct combined_hash<_type> : std::hash<_type>
{};

template<typename... _types>
struct combined_hash<std::tuple<_types...>>
{
	using type = std::tuple<_types...>;

	//! \since build 586
	yconstfn size_t
	operator()(const type& tp) const ynoexcept_spec(
		ystdex::hash_combine_seq(0, std::declval<const _types&>()...))
	{
		return details::combined_hash_tuple<type,
			make_natural_sequence_t<sizeof...(_types)>>::call(tp);
	}
};

template<typename _type1, typename _type2>
struct combined_hash<std::pair<_type1, _type2>>
	: combined_hash<std::tuple<_type1, _type2>>
{};
//@}
//@}

} // namespace ystdex;

#endif

