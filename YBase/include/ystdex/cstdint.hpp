/*
	© 2012-2013, 2015-2016, 2018-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstdint.hpp
\ingroup YStandardEx
\brief ISO C 标准整数类型和相关扩展操作。
\version r634
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2013-08-24 20:28:18 +0800
\par 修改时间:
	2019-08-06 22:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CStandardInteger
*/


#ifndef YB_INC_ystdex_cstdint_hpp_
#define YB_INC_ystdex_cstdint_hpp_ 1

#include "iterator_op.hpp" // for CHAR_BIT, size_t_, make_signed, make_unsigned,
//	size_t, std::int64_t, std::uint64_t, is_signed, _t, common_type,
//	cond_t, and_, is_unsigned, bool_, yconstraint, YB_VerifyIterator,
//	ystdex::make_reverse_iterator;
#include <limits>
#include <numeric> // for std::accumulate;

namespace ystdex
{

//! \todo 使用单独的头文件。
//@{
/*!
\brief 字节序。
\since build 594
*/
enum class byte_order
{
	unknown = 0,
	neutral = 1,
	little = 2,
	big = 3,
	PDP = 4
};


//! \since build 695
//@{
namespace details
{

struct bit_order_tester
{
	unsigned char le : 4, : CHAR_BIT - 4;
};

union byte_order_tester
{
	std::uint_least32_t n;
	byte p[4];
};

} // namespace details;

// XXX: This cannot be 'constexpr'.
//! \brief 测试本机字节序。
YB_STATELESS inline byte_order
native_byte_order()
{
	yconstexpr const details::byte_order_tester x = {0x01020304};

	return x.p[0] == byte(4) ? byte_order::little : (x.p[0] == byte(1)
		? byte_order::big
		: (x.p[0] == byte(2) ? byte_order::PDP : byte_order::unknown));
}

//! \brief 测试本机位序。
YB_STATELESS yconstfn bool
native_little_bit_order()
{
	return bool(details::bit_order_tester{1}.le & 1);
}
//@}
//@}


/*!
\ingroup unary_type_traits
\brief 取指定整数类型的位宽度。
\since build 260
*/
template<typename _tInt>
struct integer_width : size_t_<sizeof(_tInt) * CHAR_BIT>
{};


/*!
\ingroup transformation_traits
\brief 取指定整数类型和条件表达式对应的有符号或无符号整数类型。
\since build 260
*/
//@{
template<typename _type, bool>
struct make_signed_c : make_signed<_type>
{};

template<typename _type>
struct make_signed_c<_type, false> : make_unsigned<_type>
{};
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


//! \ingroup transformation_traits
//@{
/*!
\brief 位加倍扩展。
\since build 587
\note 可用于定点数乘除法中间类型。
\todo 使用扩展整数类型保持 64 位类型精度。
*/
template<typename _type, bool _bSigned = is_signed<_type>::value>
struct make_widen_int
{
private:
	using width = integer_width<_type>;

public:
	using type = _t<make_signed_c<_t<make_width_int<(width::value << 1) <= 64
		? width::value << 1 : width::value>>, _bSigned>>;
};


/*!
\brief 公共整数类型。
\note 同 common_type 但如果可能，按需自动扩展整数位宽避免缩小数值范围。
\since build 744
*/
//@{
template<typename... _types>
struct common_int_type : common_type<_types...>
{};

template<typename _type1, typename _type2, typename... _types>
struct common_int_type<_type1, _type2, _types...>
{
private:
	using common_t = common_type_t<_type1, _type2>;

public:
	using type = typename common_int_type<cond_t<
		and_<is_unsigned<common_t>, or_<is_signed<_type1>, is_signed<_type2>>>,
		_t<make_widen_int<common_t, true>>, common_t>, _types...>::type;
};
//@}
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
struct have_same_modulo : bool_<std::uintmax_t(modular_arithmetic<
	_type1>::value) != 0 && std::uintmax_t(modular_arithmetic<_type1>::value)
	== std::uintmax_t(modular_arithmetic<_type2>::value)>
{};


//! \since build 842
//@{
//! \brief 判断无符号整数是否为 2 的整数次幂。
YB_ATTR_nodiscard yconstfn bool
is_power_of_2(std::uintmax_t n) ynothrow
{
	return n != 0 && (n & (n - 1)) == 0;
}

//! \since build 842
namespace details
{

//! \since build 849
YB_ATTR_nodiscard YB_ATTR(always_inline) YB_STATELESS inline size_t
floor_lb_shift(size_t n, true_) ynothrow
{
	return n >> 1;
}
//! \since build 849
YB_ATTR_nodiscard YB_ATTR(always_inline) YB_STATELESS inline size_t
floor_lb_shift(size_t n, false_) ynothrow
{
	return (n >> 1) + (n & 1 & size_t(n != 1));
}

template<size_t _vN, typename = enable_if_t<_vN != 32 && _vN != 64>>
YB_ATTR_nodiscard YB_STATELESS inline size_t
floor_lb_w(std::uintmax_t n, size_t_<_vN>) ynothrow
{
	yconstraint(n != 0);

	// NOTE: This is like %is_power_of_2, but 0 is already excluded.
	using is_pow_2_t = bool_<(_vN & (_vN - 1)) == 0>;
	size_t res(0);
	auto shifted(floor_lb_shift(_vN, is_pow_2_t()));

	while(shifted != 0)
	{
		const auto tmp(n >> shifted);

		if(tmp != 0)
			yunseq(res += shifted, n = tmp);
		shifted = floor_lb_shift(shifted, is_pow_2_t());
	}
	return res;
}
// NOTE: See http://stackoverflow.com/questions/11376288/fast-computing-of-log2-for-64-bit-integers.
//! \pre <tt>n != 0</tt> 。
YB_ATTR_nodiscard YB_STATELESS inline size_t
floor_lb_w(std::uint32_t n, size_t_<32>) ynothrow
{
	static yconstexpr const size_t deBruijn_bit_pos[32]{
		0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
		8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31};

	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	return deBruijn_bit_pos[std::uint32_t(n * 0x07C4ACDDU) >> 27];
}
//! \pre <tt>n != 0</tt> 。
YB_ATTR_nodiscard YB_STATELESS inline size_t
floor_lb_w(std::uint64_t n, size_t_<64>) ynothrow
{
	static yconstexpr const size_t deBruijn_bit_pos[64]{
		63,  0, 58,  1, 59, 47, 53,  2,
		60, 39, 48, 27, 54, 33, 42,  3,
		61, 51, 37, 40, 49, 18, 28, 20,
		55, 30, 34, 11, 43, 14, 22,  4,
		62, 57, 46, 52, 38, 26, 32, 41,
		50, 36, 17, 19, 29, 10, 13, 21,
		56, 45, 25, 31, 35, 16,  9, 12,
		44, 24, 15,  8, 23,  7,  6,  5};

    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
	return deBruijn_bit_pos[std::uint64_t((n - (n >> 1))
		* 0x07EDD5E59A4E28C2ULL) >> 58];
}

#if YB_IMPL_GNUCPP >= 30400 || (__has_builtin(__builtin_clz) \
	&& __has_builtin(__builtin_clzl) && __has_builtin(__builtin_clzll))
#	define YB_Impl_has_builtin_clz true
// TODO: Extract %builtin_ciz_dispatch to some public header.
template<typename _type>
struct builtin_clz_dispatch
{
	static_assert(is_unsigned<_type>(), "Unsupport type found.");

	YB_ATTR_nodiscard YB_STATELESS static inline size_t
	call(_type n) ynothrowv
	{
		yconstraint(n != 0);
		details::floor_lb_w(n, size_t_<sizeof(_type) * CHAR_BIT>());
	}
};

template<>
struct builtin_clz_dispatch<unsigned long long>
{
	//! \pre <tt>n != 0</tt> 。
	YB_ATTR_nodiscard YB_ATTR(always_inline) YB_STATELESS static inline size_t
	call(unsigned long long n) ynothrow
	{
		yconstraint(n != 0);
		return size_t(__builtin_clzll(n));
	}
};

template<>
struct builtin_clz_dispatch<unsigned long>
{
	//! \pre <tt>n != 0</tt> 。
	YB_ATTR_nodiscard YB_ATTR(always_inline) YB_STATELESS static inline size_t
	call(unsigned long n) ynothrow
	{
		yconstraint(n != 0);
		return size_t(__builtin_clzl(n));
	}
};

template<>
struct builtin_clz_dispatch<unsigned>
{
	//! \pre <tt>n != 0</tt> 。
	YB_ATTR_nodiscard YB_ATTR(always_inline) static inline size_t
	call(unsigned n) ynothrow
	{
		yconstraint(n != 0);
		return size_t(__builtin_clz(n));
	}
};
#endif

//! \pre <tt>n != 0</tt> 。
YB_ATTR_nodiscard YB_STATELESS inline size_t
floor_lb(std::uintmax_t n) ynothrow
{
#if YB_Impl_has_builtin_clz
	return sizeof(n) * CHAR_BIT - 1
		- builtin_clz_dispatch<std::uintmax_t>::call(n);
#else
	return floor_lb_w(n, size_t_<sizeof(n * CHAR_BIT)>());
	// NOTE: Intrinsics provided by implementation is not allowed in YStandard
	//	yet. Hopefully this can be recognized as a common pattern and optimized.
	// TODO: Detect intrinsics without machine-dependent macros for Microsoft
	//	VC++?
	// TODO: Optimize without builtins.
#endif
}
//@}
#undef YB_Impl_has_builtin_clz

} // namespace details;

/*!
\brief 计算 2 为底数的无符号整数的向下取整的对数值。
\pre <tt>n != 0</tt> 。
*/
YB_ATTR_nodiscard YB_STATELESS inline size_t
floor_lb(std::uintmax_t n) ynothrow
{
	return details::floor_lb(n);
}

/*!
\brief 计算 2 为底数的无符号整数向上取整的的对数值。
\pre <tt>n > 1</tt> 。
*/
YB_ATTR_nodiscard YB_STATELESS inline size_t
ceiling_lb(std::uintmax_t n) ynothrow
{
	return 1 + details::floor_lb(n - 1);
}
//@}


//! \pre 静态断言：整数宽度非零且为 byte 类型宽度的整数倍。
//@{
/*!
\brief 使用迭代器对指定的范围中的字节表示的序列构造无符号整数。
\pre 范围中的迭代器可解引用。
\since build 603
*/
template<size_t _vWidth, typename _tIn>
YB_ATTR_nodiscard YB_PURE typename make_width_int<_vWidth>::unsigned_type
pack_uint(_tIn first, _tIn last) ynothrowv
{
	static_assert(_vWidth != 0 && _vWidth
		% std::numeric_limits<unsigned char>::digits == 0,
		"Invalid integer width found.");
	using utype = typename make_width_int<_vWidth>::unsigned_type;

	YB_VerifyIterator(first);
	return std::accumulate(first, last, utype(), [](utype x, byte y){
		return utype(x << std::numeric_limits<unsigned char>::digits
			| utype(y));
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
	static_assert(_vWidth != 0 && _vWidth % std::numeric_limits<
		unsigned char>::digits == 0, "Invalid integer width found.");
	auto n(_vWidth);

	while(!(_vWidth < (n -= std::numeric_limits<unsigned char>::digits)))
	{
		YB_VerifyIterator(result);
		*result = byte(value >> n);
		++result;
	}
}
//@}

//! \pre 指针参数非空。
//@{
/*!
\brief 从字节缓冲区读取指定宽的大端序无符号整数。
\since build 623
*/
template<size_t _vWidth>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE inline
	typename make_width_int<_vWidth>::unsigned_type
read_uint_be(const byte* buf) ynothrowv
{
	yconstraint(buf);
	return ystdex::pack_uint<_vWidth>(buf,
		buf + _vWidth / std::numeric_limits<unsigned char>::digits);
}

//! \since build 608
//@{
//! \brief 从字节缓冲区读取指定宽的小端序无符号整数。
template<size_t _vWidth>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE inline
	typename make_width_int<_vWidth>::unsigned_type
read_uint_le(const byte* buf) ynothrowv
{
	yconstraint(buf);
	return ystdex::pack_uint<_vWidth>(ystdex::make_reverse_iterator(
		buf + _vWidth / std::numeric_limits<unsigned char>::digits),
		ystdex::make_reverse_iterator(buf));
}

/*!
\brief 向字节缓冲区写入指定宽的大端序无符号整数。
\since build 623
*/
template<size_t _vWidth>
YB_NONNULL(1) inline void
write_uint_be(byte* buf,
	typename make_width_int<_vWidth>::unsigned_type val) ynothrowv
{
	yconstraint(buf);
	ystdex::unpack_uint<_vWidth>(val, buf);
}

//! \brief 向字节缓冲区写入指定宽的小端序无符号整数。
template<size_t _vWidth>
YB_NONNULL(1) inline void
write_uint_le(byte* buf,
	typename make_width_int<_vWidth>::unsigned_type val) ynothrowv
{
	yconstraint(buf);
	ystdex::unpack_uint<_vWidth>(val, ystdex::make_reverse_iterator(buf
		+ _vWidth / std::numeric_limits<unsigned char>::digits));
}
//@}
//@}

} // namespace ystdex;

#endif

