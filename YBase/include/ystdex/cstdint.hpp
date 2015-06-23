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
\version r286
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2013-08-24 20:28:18 +0800
\par 修改时间:
	2015-06-22 00:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CStandardInteger
*/


#ifndef YB_INC_ystdex_cstdint_hpp_
#define YB_INC_ystdex_cstdint_hpp_ 1

#include "iterator_op.hpp" // for integral_constant, size_t, make_signed_t,
//	make_unsigned_t, std::int64_t, std::uint64_t, yconstraint,
//	is_undereferenceable, ystdex::make_reverse_iterator;
#include <limits>
#include <numeric> // for std::accumulate;

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


//! \pre 静态断言：整数宽度非零且为 byte 类型宽度的整数倍。
//@{
/*!
\brief 使用迭代器对指定的范围中的字节表示的序列构造无符号整数。
\pre 范围中的迭代器可解引用。
\since build 603
*/
template<size_t _vWidth, typename _tIn>
typename make_width_int<_vWidth>::unsigned_type
pack_uint(_tIn first, _tIn last) ynothrowv
{
	static_assert(_vWidth != 0 && _vWidth % std::numeric_limits<byte>::digits
		== 0, "Invalid integer width found.");
	using uint_t = typename make_width_int<_vWidth>::unsigned_type;

	yconstraint(!is_undereferenceable(first));
	return std::accumulate(first, last, uint_t(), [](uint_t x, byte y){
		return uint_t(x << std::numeric_limits<byte>::digits | y);
	});
}

/*!
\brief 分解无符号整数到迭代器对指定的字节范围。
\pre 断言：范围中的迭代器可解引用。
\since build 604
*/
template<size_t _vWidth, typename _tOut>
void
unpack_uint(typename ystdex::make_width_int<_vWidth>::unsigned_type value,
	_tOut result) ynothrow
{
	static_assert(_vWidth != 0 && _vWidth % std::numeric_limits<byte>::digits
		== 0, "Invalid integer width found.");
	auto n(_vWidth);

	while(!(_vWidth < (n -= std::numeric_limits<byte>::digits)))
	{
		yconstraint(!is_undereferenceable(result));
		*result = byte(value >> n);
		++result;
	}
}
//@}

//! \since build 608
//@{
//! \brief 从字节缓冲区读取指定宽的小端序无符号整数。
template<size_t _vWidth>
inline YB_NONNULL(1) typename make_width_int<_vWidth>::unsigned_type
read_uint_le(const byte* buf) ynothrowv
{
	yconstraint(buf);
	return ystdex::pack_uint<_vWidth>(ystdex::make_reverse_iterator(
		buf + _vWidth / std::numeric_limits<byte>::digits),
		ystdex::make_reverse_iterator(buf));
}

//! \brief 向字节缓冲区写入指定宽的小端序无符号整数。
template<size_t _vWidth>
inline YB_NONNULL(1) void
write_uint_le(byte* buf, typename make_width_int<_vWidth>::unsigned_type
	val) ynothrowv
{
	yconstraint(buf);
	ystdex::unpack_uint<_vWidth>(val, ystdex::make_reverse_iterator(buf
		+ _vWidth / std::numeric_limits<byte>::digits));
}
//@}

} // namespace ystdex;

#endif

