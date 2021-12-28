/*
	© 2015-2016, 2018-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file bit.hpp
\ingroup YStandardEx
\brief 位操作。
\version r1401
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2021-12-18 22:57:19 +0800
\par 修改时间:
	2021-12-27 18:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Bit
\see ISO C++20 [bit] 。
\see WG21 P0553R4 。

提供 ISO C++20 标准库头 \<bit> 兼容的替代和扩展接口。
当前仅提供对于整数操作的一部分接口。对标准库头的检查可能会随着提供接口的完善而变化。
同 ISO C++20 ，所有整数类型的值视为补码表示；
	使用扩展时，可能隐含实现仅使用补码。
当前没有提供 WG21 P0556R3（被 WG21 P1956R1 重命名）的接口，
	但提供类似功能的相关扩展。
*/


#ifndef YB_INC_ystdex_bit_hpp_
#define YB_INC_ystdex_bit_hpp_ 1

#include "cassert.h" // for std::uint_least32_t, std::uintmax_t, std::uint32_t,
//	std::uint_fast8_t, std::uint64_t, yconstraint;
#include "meta.hpp" // for CHAR_BIT, byte, enable_if_unsigned_t, size_t,
//	size_t_;
#include <limits> // for std::numeric_limits;
// NOTE: See https://docs.microsoft.com/cpp/visual-cpp-language-conformance and
//	https://docs.microsoft.com/cpp/preprocessor/predefined-macros.
#if (YB_IMPL_MSCPP >= 1925 && _MSVC_LANG >= 201907L) \
	|| (__cplusplus >= 202002L && __has_include(<bit>))
#	include <bit> // for __cpp_lib_bitops, std::countl_zero;
#endif
#if YB_IMPL_MSCPP >= 1400
#	include <intrin.h>
#	if defined(_M_AMD64) || defined(_M_ARM64) || defined(_M_ARM64EC)
#		define YB_Impl_has_BitScan64 true
#		pragma intrinsic(_BitScanForward64)
#		pragma intrinsic(_BitScanReverse64)
#	endif
#	define YB_Impl_has_BitScan true
#	pragma intrinsic(_BitScanForward)
#	pragma intrinsic(_BitScanReverse)
#endif

namespace ystdex
{

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
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline byte_order
native_byte_order() ynothrow
{
	yconstexpr const details::byte_order_tester x = {0x01020304};

	return x.p[0] == byte(4) ? byte_order::little : (x.p[0] == byte(1)
		? byte_order::big
		: (x.p[0] == byte(2) ? byte_order::PDP : byte_order::unknown));
}

//! \brief 测试本机位序。
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yconstfn bool
native_little_bit_order()
{
	return bool(details::bit_order_tester{1}.le & 1);
}
//@}


//! \brief 判断无符号整数是否为 2 的整数次幂。
template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yconstfn
	yimpl(enable_if_unsigned_t)<_type, bool>
is_power_of_2(_type x) ynothrow
{
	return (x & (x - 1)) == 0;
}

//! \brief 判断无符号整数是否为 2 的整数次幂的正整数。
template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_FLATTEN yconstfn
	yimpl(enable_if_unsigned_t)<_type, bool>
is_power_of_2_positive(_type x) ynothrow
{
	return x != 0 && is_power_of_2(x);
}

//! \since build 842
namespace details
{

//! \since build 933
//@{
// XXX: Current policy does not relies on features specific to concrete
//	microarchitectures. Consideration of '__builtin_ia32_lzcnt_u*' and
//	'__builtin_ia32_tzcnt_u*' is deferred. Note GCC 7.0 may not work, see
//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78057.
// TODO: Reconsider?
//! \pre <tt>x != 0</tt> 。
//@{
// XXX: There is no use of %__builtin_clzs, since it is less portable. See
#	if !(__has_builtin(__builtin_clz) && __has_builtin(__builtin_clzl) \
	&& __has_builtin(__builtin_clzll)) || !(YB_IMPL_GNUC >= 30400)
template<size_t>
struct dispath_w;

template<>
struct dispath_w<32>
{
	YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS static inline int
	clz(std::uint32_t x) ynothrow
	{
#		if YB_Impl_has_BitScan
		unsigned long lo;

		_BitScanReverse(&lo, std::uint32_t(x));
		return 31 - int(lo);
#				else
		// NOTE: See http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
		//	or https://stackoverflow.com/a/11398748.
		// NOTE: The alignment is enough and fit for most modern machines. Note
		//	%std::hardware_destructive_interference_size is not relied on, and
		//	it is less important for smaller arrays already aligned as the size.
		yalignas(32) static yconstexpr const std::uint_fast8_t
			deBruijn_bit_pos[]{
			0,  9,  1, 10, 13, 21,  2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
			8, 12, 20, 28, 15, 17, 24,  7, 19, 27, 23,  6, 26,  5, 4, 31
		};

		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return 31 - int(deBruijn_bit_pos[std::uint32_t(x * 0x07C4ACDDU) >> 27]);
#		endif
	}

	YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS static inline int
	ctz(std::uint32_t x) ynothrow
	{
#		if YB_Impl_has_BitScan
		unsigned long lo;

		_BitScanForward(&lo, std::uint32_t(x));
		return int(lo);
#		else
		// NOTE: See http://supertech.csail.mit.edu/papers/debruijn.pdf or
		//	http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn.
		yalignas(32) static yconstexpr const std::uint_fast8_t
			deBruijn_bit_pos[]{
			 0,  1, 28,  2, 29, 14, 24,  3, 30, 22, 20, 15, 25, 17,  4,  8,
			31, 27, 13, 23, 21, 19, 16,  7, 26, 12, 18,  6, 11,  5, 10,  9
		};

		return
			int(deBruijn_bit_pos[std::uint32_t((x & -x) * 0x077CB531U) >> 27]);
#		endif
	}
};

template<>
struct dispath_w<64>
{
	YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS static inline int
	clz(std::uint64_t x) ynothrowv
	{
		yconstraint(x != 0);

#		if YB_Impl_has_BitScan64
		unsigned long r;

		_BitScanReverse64(&r, x);
		return 63 - int(r);
#		elif YB_Impl_has_BitScan
		unsigned long hi, lo;
		bool hi_set = _BitScanReverse(&hi, std::uint32_t(x >> 32)) != 0;

		_BitScanReverse(&lo, std::uint32_t(x));
		hi |= 32;
		return 63 - int(hi_set ? hi : lo);
#				else
		// NOTE: See https://stackoverflow.com/a/23000588.
		// NOTE: Ditto. See the 32-bit implementation above.
		yalignas(64) static yconstexpr const std::uint_fast8_t
			deBruijn_bit_pos[]{
			 0, 58,  1, 59, 47, 53,  2, 60, 39, 48, 27, 54, 33, 42,  3, 61,
			51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22,  4, 62,
			57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21, 56,
			45, 25, 31, 35, 16,  9, 12, 44, 24, 15,  8, 23,  7,  6,  5, 63
		};

		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		x |= x >> 32;
		return 63 - int(
			deBruijn_bit_pos[std::uint64_t(x * 0x03F6EAF2CD271461ULL) >> 58]);
#		endif
	}

	YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS static inline int
	ctz(std::uint64_t x) ynothrowv
	{
		yconstraint(x != 0);

#		if YB_Impl_has_BitScan64
		unsigned long r;

		_BitScanForward64(&r, x);
		return int(r);
#		elif YB_Impl_has_BitScan
		unsigned long lo;
		const bool lo_set(_BitScanForward(&lo, std::uint32_t(x)) != 0);

		if(lo_set)
			return int(lo);

		unsigned long hi;

		_BitScanForward(&hi, std::uint32_t(x >> 32));
		return int(hi + 32);
#		else
		yalignas(64) static yconstexpr const std::uint_fast8_t
			deBruijn_bit_pos[]{
			 0,  1,  2, 53,  3,  7, 54, 27,  4, 38, 41,  8, 34, 55, 48, 28,
			62,  5, 39, 46, 44, 42, 22,  9, 24, 35, 59, 56, 49, 18, 29, 11,
			63, 52,  6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,
			51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12
		};

		return int(deBruijn_bit_pos[
			std::uint64_t((x & -x) * 0x022FDD63CC95386DULL) >> 58]);
#		endif
	}
};
#	endif

//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59874.
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline int
clz(unsigned x) ynothrowv
{
	// XXX: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81015. This is safe
	//	here since the type is explicitly 'unsigned'.
#	if __has_builtin(__builtin_clz) || YB_IMPL_GNUC >= 30400
	return __builtin_clz(x);
#	else
	return dispath_w<std::numeric_limits<unsigned>::digits>::clz(x);
#	endif
}
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline int
clz(unsigned long x) ynothrowv
{
#	if __has_builtin(__builtin_clzl) || YB_IMPL_GNUC >= 30400
	return __builtin_clzl(x);
#	else
	return dispath_w<std::numeric_limits<unsigned long>::digits>::clz(x);
#	endif
}
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline int
clz(unsigned long long x) ynothrowv
{
#	if __has_builtin(__builtin_clzll) || YB_IMPL_GNUC >= 30400
	return __builtin_clzll(x);
#	else
	return dispath_w<std::numeric_limits<unsigned long long>::digits>::clz(x);
#	endif
}
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline int
clz(unsigned char x) ynothrowv
{
	return details::clz(unsigned(x)) - (std::numeric_limits<unsigned>::digits
		- std::numeric_limits<unsigned char>::digits);
}
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline int
clz(unsigned short x) ynothrowv
{
	return details::clz(unsigned(x)) - (std::numeric_limits<unsigned>::digits
		- std::numeric_limits<unsigned short>::digits);
}

YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline int
ctz(unsigned x) ynothrowv
{
	// XXX: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81015. This is safe
	//	here since the type is explicitly 'unsigned'.
#	if __has_builtin(__builtin_ctz) || YB_IMPL_GNUC >= 30400
	return __builtin_ctz(x);
#	else
	return dispath_w<std::numeric_limits<unsigned>::digits>::ctz(x);
#	endif
}
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline int
ctz(unsigned long x) ynothrowv
{
#	if __has_builtin(__builtin_ctzl) || YB_IMPL_GNUC >= 30400
	return __builtin_ctzl(x);
#	else
	return dispath_w<std::numeric_limits<unsigned long>::digits>::ctz(x);
#	endif
}
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline int
ctz(unsigned long long x) ynothrowv
{
#	if __has_builtin(__builtin_ctzll) || YB_IMPL_GNUC >= 30400
	return __builtin_ctzll(x);
#	else
	return dispath_w<std::numeric_limits<unsigned long long>::digits>::ctz(x);
#	endif
}
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline int
ctz(unsigned char x) ynothrowv
{
	return details::ctz(unsigned(x)) - (std::numeric_limits<unsigned>::digits
		- std::numeric_limits<unsigned char>::digits);
}
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline int
ctz(unsigned short x) ynothrowv
{
	return details::ctz(unsigned(x)) - (std::numeric_limits<unsigned>::digits
		- std::numeric_limits<unsigned short>::digits);
}
//@}
//@}


//! \since build 849
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yconstfn size_t
floor_lb_shift(size_t x, true_) ynothrow
{
	return x >> 1;
}
//! \since build 849
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yconstfn size_t
floor_lb_shift(size_t x, false_) ynothrow
{
	return (x >> 1) + (x & 1 & size_t(x != 1));
}

/*!
\pre <tt>x != 0</tt> 。
\since build 933
*/
//@{
template<size_t _vN, typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline size_t
floor_lb_w(_type x, size_t_<_vN>) ynothrowv
{
	yconstraint(x != 0);

	// NOTE: This is like %is_power_of_2. Note 0 is already excluded.
	using is_pow_2_t = bool_<(_vN & (_vN - 1)) == 0>;
	size_t res(0);
	auto shifted(floor_lb_shift(_vN, is_pow_2_t()));

	while(shifted != 0)
	{
		const auto tmp(x >> shifted);

		if(tmp != 0)
			yunseq(res += shifted, x = tmp);
		shifted = floor_lb_shift(shifted, is_pow_2_t());
	}
	return res;
}
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline size_t
floor_lb_w(std::uint32_t x, size_t_<32>) ynothrowv
{
	// NOTE: See $2021-12 @ %Documentation::Workflow.
	// NOTE: Since common builtins are affected by the bug, target-specific
	//	builtins and inline assembly are used here.
	// TODO: Support more targets?
#if __has_builtin(__builtin_ia32_bsrsi)
	return size_t(__builtin_ia32_bsrsi(int(x)));
	// XXX: All suppored GCC versions are concerned.
#elif YB_IMPL_GNUCPP && (defined(__i386__) || defined(__x86_64__))
	unsigned r;

	asm("bsr %1, %0\n" : "=r"(r) : "r"(x));
	return r;
#else
	// XXX: The following code should restore the issue, i.e. as efficient as
	//	the old implementation in %CStandardInt before YSLib build 934, even
	//	when the implementations above are not used.
	return 31 - size_t(details::clz(x));
#endif
}
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline size_t
floor_lb_w(std::uint64_t x, size_t_<64>) ynothrowv
{
	// XXX: Ditto, except that it is only done for 64-bit targets, since on
	//	32-bit targets the code generated by GCC would not be obviously bad (and
	//	Clang does not seems better; see also
	//	https://stackoverflow.com/a/57643088).
	// TODO: Support more targets?
#if __has_builtin(__builtin_ia32_bsrdi)
	return size_t(__builtin_ia32_bsrdi(static_cast<long long>(x)));
#elif YB_IMPL_GNUCPP && defined(__x86_64__)
	// XXX: The size of 'unsigned long' varies on different x86_64 targets.
	unsigned long long r;

	asm("bsrq %1, %0\n" : "=r"(r) : "r"(x));
	return r;
#else
	return 63 - size_t(details::clz(x));
#endif
}
//! \since build 934
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline size_t
floor_lb_w(std::uint8_t x, size_t_<8>) ynothrowv
{
	return details::floor_lb_w(x, size_t_<32>());
}
//! \since build 934
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline size_t
floor_lb_w(std::uint16_t x, size_t_<16>) ynothrowv
{
	// XXX: %__lzcnt is not used now.
	return details::floor_lb_w(x, size_t_<32>());
}
//@}

} // namespace details;

/*!
\brief 计算 2 为底数的无符号整数的向下取整的对数值。
\pre <tt>x != 0</tt> 。
*/
template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline
	yimpl(enable_if_unsigned_t)<_type, size_t>
floor_lb(_type x) ynothrowv
{
	return details::floor_lb_w(x, size_t_<sizeof(x) * CHAR_BIT>());
}

/*!
\brief 计算 2 为底数的无符号整数向上取整的的对数值。
\pre <tt>x > 1</tt> 。
*/
template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline
	yimpl(enable_if_unsigned_t)<_type, size_t>
ceiling_lb(_type x) ynothrowv
{
	return 1 + details::floor_lb_w(x - 1, size_t_<sizeof(x) * CHAR_BIT>());
}


//! \pre <tt>x != 0</tt> 。
//@{
//! \brief 同 ISO C++20 的 countl_zero ，但对参数为 0 时结果未定义。
template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yimpl(yconstfn)
	yimpl(enable_if_unsigned_t)<_type, int>
countl_zero_narrow(_type x) ynothrowv
{
	return details::clz(x);
}

template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yimpl(yconstfn)
	yimpl(enable_if_unsigned_t)<_type, int>
countr_zero_narrow(_type x) ynothrowv
{
	return details::ctz(x);
}
//@}

inline namespace cpp2020
{

/*
\warning 尽管接口标注 yconstfn ，不保证调用是否为常量表达式。

替代实现允许使用语言实现提供的扩展；
扩展可能实际支持在要求常量表达式的上下文中使用，但通常没有公开保证。
替代实现只支持至多 64 位整数。
*/
//@{
#if __cpp_lib_bitops >= 201907L
using std::countl_zero;
using std::countr_zero;
#else
template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yimpl(yconstfn)
	yimpl(enable_if_unsigned_t)<_type, int>
countl_zero(_type x) ynothrow
{
	return x != 0 ? details::clz(x) : std::numeric_limits<_type>::digits;
}

template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yimpl(yconstfn)
	yimpl(enable_if_unsigned_t)<_type, int>
countr_zero(_type x) ynothrow
{
	return x != 0 ? details::ctz(x) : std::numeric_limits<_type>::digits;
}
#endif
//@}

} // inline namespace cpp2020;

} // namespace ystdex;

#undef YB_Impl_has_BitScan
#undef YB_Impl_has_BitScan64

#endif

