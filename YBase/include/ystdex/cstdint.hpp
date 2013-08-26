/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstdint.hpp
\ingroup YStandardEx
\brief ISO C 标准整数类型操作。
\version r138
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2013-08-24 20:28:18 +0800
\par 修改时间:
	2013-08-24 20:34 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CStandardInteger
*/


#ifndef YB_INC_ystdex_cstdint_hpp_
#define YB_INC_ystdex_cstdint_hpp_ 1

#include "type_op.hpp"

namespace ystdex
{

/*!
\ingroup meta_operations
\brief 取指定整数类型的位宽度。
\since build 260
*/
template<typename _tInt>
struct integer_width
	: public integral_constant<size_t, sizeof(_tInt) * CHAR_BIT>
{};


/*!
\ingroup meta_operations
\brief 取指定整数类型和条件表达式对应的有符号或无符号整数类型。
\since build 260
*/
//@{
template<typename _type, bool>
struct make_signed_c
{
	using type = typename std::make_signed<_type>::type;
};

template<typename _type>
struct make_signed_c<_type, false>
{
	using type = typename std::make_unsigned<_type>::type;
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

} // namespace ystdex;

#endif

