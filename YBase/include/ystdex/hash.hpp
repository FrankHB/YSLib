/*
	© 2015, 2019, 2022-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file hash.hpp
\ingroup YStandardEx
\brief 散列接口。
\version r360
\author FrankHB <frankhb1989@gmail.com>
\since build 588
\par 创建时间:
	2015-03-28 22:12:11 +0800
\par 修改时间:
	2023-02-07 12:52 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Hash
*/


#ifndef YB_INC_ystdex_hash_hpp_
#define YB_INC_ystdex_hash_hpp_ 1

#include "tuple.hpp" // for "tuple.hpp", underlying_type_t, cond_t, is_enum,
//	std::declval, and_, is_object, is_copy_constructible, is_destructible,
//	is_detected_exact, size_t, std::tuple, index_sequence, std::get,
//	index_sequence_for, std::pair;
#include <functional> // for <functional>, std::hash, std::__is_fast_hash
//	conditionally;
#include <numeric> // for std::accumulate;

namespace ystdex
{

/*!	\defgroup hashers Hash Function Objects
\ingroup functors
\brief 散列操作函数对象。
\note 通常不使用 final 以允许空基类优化。
\see ISO C++20 [hash.requirements] 。
\since build 968
*/

//! \since build 967
//!@{
namespace details
{

// XXX: Fixes on further specializations since ISO C++20 are not implemented
//	here. Let it in %ystdex::hash.
// NOTE: See LWG 2148, LWG 2543 and LWG 2817.
// XXX: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88802.
// XXX: The fix in libstdc++ are only for ISO C++17 and modes.
#if __cplusplus >= 201703L && (defined(__GLIBCXX__) && __GLIBCXX__ > 20190111)
template<typename _type>
using hash_base_t = std::hash<_type>;
#else
//! \ingroup hashers
struct hash_nullptr_t
{
	YB_ATTR_nodiscard YB_STATELESS size_t
	operator()(nullptr_t) const ynothrow
	{
		return 0;
	}
};


// XXX: LWG 2148 requires '__cplusplus >= 201411L' (while excluding
//	implementations not support the resolution of LWG 2148), but it is not
//	ready for SFINAE support (LWG 2543). This is known broken in earlier
//	revisions of libc++ and Microsoft VC++, though the initial open-sourced
//	Microsoft STL commit already have it fixed (see <type_traits> in https://github.com/microsoft/STL/commit/219514876ea86491de191ceaa88632616bbc0f19).
//	Here avoid the buggy implementations as the best effort for supporting LWG
//	2543 in %ystdex::hash.
// XXX: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60970,
//	https://reviews.llvm.org/rL189831 and https://reviews.llvm.org/rL278300.
#if (__cplusplus >= 201411L && !defined(_LIBCPP_VERSION)) \
	|| (defined(__GLIBCXX__) && __GLIBCXX__ > 20150729) \
	|| (defined(_LIBCPP_VERSION) && _LIBCPP_VERSION > 4000)
template<typename _type>
using hash_base_enum_t = std::hash<_type>;
#	else
//! \ingroup hashers
template<typename _type>
struct hash_enum
{
	YB_ATTR_nodiscard YB_STATELESS size_t
	operator()(_type v) const ynothrow
	{
		return std::hash<underlying_type_t<_type>>()(v);
	}
};


template<typename _type>
using hash_base_enum_t = cond_t<is_enum<_type>, hash_enum<_type>,
	std::hash<_type>>;
#	endif

template<typename _type>
using hash_base_t = cond_t<is_same<_type, nullptr_t>, hash_nullptr_t,
	hash_base_enum_t<_type>>;
#endif

template<typename _type, typename _tParam>
using hash_apply_t = decltype(std::declval<_type&>()(std::declval<_tParam&>()));

} // unnamed namespace;


#if false
// XXX: This may be not available for libstdc++ and some other implementations
//	even in the future version of ISO C++ due to ABI compatibility issues.
using std::hash;
#else
/*!
\ingroup YBase_replacement_features
\brief 散列模板。
\see LWG 2148 。
\see LWG 2543 。
\see LWG 2817 。
\see LWG 2978 。
\see LWG 3705 。
\see WG21 P0513R0 。
\see WG21 P1406R1 。

同 ISO C++20 后修正部分实现的 std::hash ，但不支持程序定义的特化，
	而直接使用 std::hash 的程序定义的特化（若存在）。
从 ISO C++11 起提供 std::hash 的特化在不同标准版本中略有不同：
LWG 2148 的解决添加对枚举类型的特化；
LWG 2817 的解决添加对 std::nullptr_t 类型的特化；
LWG 2978 的解决添加对 pmr::basic_string 实例的特化，
	但之后被 LWG 3705 修改 std::basic_string 实例的特化覆盖。
LWG 2543 的解决依赖 std::hash 的实现支持，不保证生效。
类似地，WG21 P0513R0 指定主模板和部分停用的特化具有定义，依赖 std::hash 的实现支持。
*/
template<typename _type>
struct hash : details::hash_base_t<_type>
{};
#endif


//! \note 不排除假阳性结果。
//!@{
/*!
\ingroup unary_type_traits
\brief 判断参数组合满足 ISO C++20 的散列调用中对对象的要求。
\pre 第一参数是非类类型或完整的类类型。
\see ISO C++20 [hash.requirements] 。

以第一参数作为散列函数类型，以第二参数作为键类型，其值的调用满足 Cpp17Hash 。
*/
template<typename _type>
struct is_hash_object : and_<is_object<_type>, is_copy_constructible<_type>,
	is_destructible<_type>>
{};


/*!
\ingroup binary_type_traits
\brief 判断参数组合满足 ISO C++20 的散列调用要求。
\pre 第一参数是非类类型或完整的类类型。
\see ISO C++20 [hash.requirements] 。

以第一参数作为散列函数类型，以第二参数作为键类型，其值的调用满足 Cpp17Hash 。
*/
template<typename _type, typename _tParam>
struct is_applicable_hash : and_<is_hash_object<_type>,
	is_detected_exact<size_t, details::hash_apply_t, _type, _tParam>>
{};


/*!
\ingroup unary_type_traits
\brief 判断是否实现快速散列算法。
\note 快速散列算法可提示重复计算时开销较小，不使用缓存。
\note 支持 libstdc++ \c std::__is_fast_hash 。
\since build 967
*/
// NOTE: Use the documented extension of libstdc++ if possible.
// XXX: See https://gcc.gnu.org/git/?p=gcc.git;a=commit;h=4df047dd3494ad17122ea46134a951a319a81b27.
#if defined(__GLIBCXX__) && __GLIBCXX__ > 20130205
template<typename _fHash>
struct is_fast_hash : std::__is_fast_hash<_fHash>
{};
#else
// XXX: This is same to libstdc++ by default.
template<typename _fHash>
struct is_fast_hash : not_<is_same<_fHash, std::hash<long double>>>
{};
#endif

template<typename _type>
struct is_fast_hash<ystdex::hash<_type>> : is_fast_hash<std::hash<_type>>
{};
//!@}
//!@}


/*!	\defgroup hash_extensions Hash Extensions
\brief 散列扩展接口。
\note 当前使用 Boost 定义的接口和近似实现。
\see http://www.boost.org/doc/libs/1_54_0/doc/html/hash/reference.html#boost.hash_combine 。
\since build 594
*/
//!@{
/*!
\brief 重复计算散列。
\note <tt>(1UL << 31) / ((1 + std::sqrt(5)) / 4) == 0x9E3779B9</tt> 。
\warning 实现（ Boost 文档作为 Effects ）可能改变，不应作为接口依赖。
*/
template<typename _type>
inline void
hash_combine(size_t& seed, const _type& val)
	ynoexcept_spec(hash<_type>()(val))
{
	seed ^= hash<_type>()(val) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
}

/*!
\ingroup helper_functions
\brief 重复计算序列散列。
\sa hash_combine
*/
//!@{
template<typename _type>
yconstfn size_t
hash_combine_seq(size_t seed, const _type& val)
	ynoexcept_spec(hash<_type>()(val))
{
	return ystdex::hash_combine(seed, val), seed;
}
template<typename _type, typename... _tParams>
yconstfn size_t
hash_combine_seq(size_t seed, const _type& val, const _tParams&... args)
	ynoexcept_spec(hash<_type>()(val))
{
	return
		ystdex::hash_combine_seq(ystdex::hash_combine_seq(seed, val), args...);
}
//!@}

/*!
\brief 重复对范围计算散列。
\note 使用 ADL 。
*/
//!@{
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
//!@}


namespace details
{

/*!
\ingroup hashers
\since build 586
*/
//!@{
template<class, class>
struct combined_hash_tuple;

template<class _tTuple, size_t... _vSeq>
struct combined_hash_tuple<_tTuple, index_sequence<_vSeq...>>
{
	static yconstfn size_t
	call(const _tTuple& tp)
		ynoexcept_spec(ystdex::hash_combine_seq(0, std::get<_vSeq>(tp)...))
	{
		return ystdex::hash_combine_seq(0, std::get<_vSeq>(tp)...);
	}
};
//!@}

} // namespace details;

/*!
\ingroup hashers
\ingroup customization_points
\brief 使用 hash 和 ystdex::hash_combine_seq 实现的对特定类型的散列。
\note 支持对程序定义的类型的特化。
\note 特化应满足同 ISO C++17 [unord.hash] 中对 std::hash 特化的要求。
\since build 468
*/
//!@{
template<typename...>
struct combined_hash;

template<typename _type>
struct combined_hash<_type> : hash<_type>
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
			index_sequence_for<_types...>>::call(tp);
	}
};

template<typename _type1, typename _type2>
struct combined_hash<std::pair<_type1, _type2>>
	: combined_hash<std::tuple<_type1, _type2>>
{};

//! \since build 873
template<typename _type1, typename _type2, typename... _types>
struct combined_hash<_type1, _type2, _types...>
	: combined_hash<std::tuple<_type1, _type2, _types...>>
{};
//!@}
//!@}

} // namespace ystdex;

#endif

