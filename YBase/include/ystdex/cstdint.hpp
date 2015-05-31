/*
	© 2012-2013, 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstdint.hpp
\ingroup YStandardEx
\brief ISO C 标准整数类型操作。
\version r212
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2013-08-24 20:28:18 +0800
\par 修改时间:
	2015-05-29 19:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CStandardInteger
*/


#ifndef YB_INC_ystdex_cstdint_hpp_
#define YB_INC_ystdex_cstdint_hpp_ 1

#include "type_op.hpp"
#include <limits>

namespace ystdex
{

/*!
\brief 字节序。
\since build 594
\todo 使用单独的头文件。
*/
enum class byte_order
{
	unknown = 0,
	neutral = 1,
	little = 2,
	big = 3,
	PDP = 4
};


/*!
\ingroup unary_type_traits
\brief 取指定整数类型的位宽度。
\since build 260
*/
template<typename _tInt>
struct integer_width : integral_constant<size_t, sizeof(_tInt) * CHAR_BIT>
{};


/*!
\ingroup transformation_traits
\brief 取指定整数类型和条件表达式对应的有符号或无符号整数类型。
\since build 260
*/
//@{
template<typename _type, bool>
struct make_signed_c
{
	using type = make_signed_t<_type>;
};

template<typename _type>
struct make_signed_c<_type, false>
{
	using type = make_unsigned_t<_type>;
};
//@}


/*!
\ingroup meta_operations
\brief 取按指定宽度的整数类型。
\since build 439
\todo 允许不定义可选的具体整数类型。
*/
//@{
template<size_t _vWidth>
struct make_width_int
{
	static_assert(_vWidth <= 64, "Width too large found.");

	using fast_type = typename make_width_int<(_vWidth <= 8U ? 8U
		: (_vWidth <= 16U ? 16U : (_vWidth <= 32U ? 32U : 64U)))>::fast_type;
	using unsigned_fast_type = typename make_width_int<(_vWidth <= 8U ? 8U
		: (_vWidth <= 16U ? 16U : (_vWidth <= 32U ? 32U : 64U)))>
		::unsigned_fast_type;
	using least_type = typename make_width_int<(_vWidth <= 8U ? 8U
		: (_vWidth <= 16U ? 16U : (_vWidth <= 32U ? 32U : 64U)))>::least_type;
	using unsigned_least_type = typename make_width_int<(_vWidth <= 8U ? 8U
		: (_vWidth <= 16U ? 16U : (_vWidth <= 32U ? 32U : 64U)))>
		::unsigned_least_type;
};

template<>
struct make_width_int<8U>
{
	using type = std::int8_t;
	using unsigned_type = std::uint8_t;
	using fast_type = std::int_fast8_t;
	using unsigned_fast_type = std::uint_fast8_t;
	using least_type = std::int_least8_t;
	using unsigned_least_type = std::uint_least8_t;
};

template<>
struct make_width_int<16U>
{
	using type = std::int16_t;
	using unsigned_type = std::uint16_t;
	using fast_type = std::int_fast16_t;
	using unsigned_fast_type = std::uint_fast16_t;
	using least_type = std::int_least16_t;
	using unsigned_least_type = std::uint_least16_t;
};

template<>
struct make_width_int<32U>
{
	using type = std::int32_t;
	using unsigned_type = std::uint32_t;
	using fast_type = std::int_fast32_t;
	using unsigned_fast_type = std::uint_fast32_t;
	using least_type = std::int_least32_t;
	using unsigned_least_type = std::uint_least32_t;
};

template<>
struct make_width_int<64U>
{
	using type = std::int64_t;
	using unsigned_type = std::uint64_t;
	using fast_type = std::int_fast64_t;
	using unsigned_fast_type = std::uint_fast64_t;
	using least_type = std::int_least64_t;
	using unsigned_least_type = std::uint_least64_t;
};
//@}


/*!
\ingroup metafunctions
\brief 位加倍扩展。
\since build 587
\note 可用于定点数乘除法中间类型。
\todo 使用扩展整数类型保持 64 位类型精度。
*/
//@{
template<typename _type, bool _bSigned = is_signed<_type>::value>
struct make_widen_int
{
	using type = typename make_signed_c<typename
		make_width_int<integer_width<_type>::value << 1>::type, _bSigned>::type;
};

template<bool _bSigned>
struct make_widen_int<std::int64_t, _bSigned>
{
	using type = std::int64_t;
};

template<bool _bSigned>
struct make_widen_int<std::uint64_t, _bSigned>
{
	using type = std::uint64_t;
};
//@}


/*!
\ingroup metafunctions
\brief 模算术特性：取得不超过模值的最大值。
\note 不保证值是整数，因此不从 std::integral_constant 派生。
\note 模值 0 表示模为平凡值 1 或不支持模算术。
\note 正确性由用户保证。一般应至少保证 + 和 * 以及相关赋值操作满足模算术语义。
\since build 440
*/
template<typename _type>
struct modular_arithmetic
{
	static yconstexpr const _type value = is_unsigned<_type>::value
		? std::numeric_limits<_type>::max() : _type(0);
};


/*!
\ingroup binary_type_traits
\brief 判断两个类型是否具有相同的模值。
\since build 440
*/
template<typename _type1, typename _type2>
struct have_same_modulo : integral_constant<bool, uintmax_t(modular_arithmetic<
	_type1>::value) != 0 && uintmax_t(modular_arithmetic<_type1>::value)
	== uintmax_t(modular_arithmetic<_type2>::value)>
{};

} // namespace ystdex;

#endif

