/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file rational.hpp
\ingroup YStandardEx
\brief 有理数运算。
\version r1156
\author FrankHB<frankhb1989@gmail.com>
\since build 260
\par 创建时间:
	2011-11-12 23:23:47 +0800
\par 修改时间:
	2012-09-04 12:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Rational
*/


#ifndef YB_INC_YSTDEX_RATIONAL_HPP_
#define YB_INC_YSTDEX_RATIONAL_HPP_ 1

#include "../ydef.h"
#include "type_op.hpp"
#include "operators.hpp"
#include <cmath> // for std::llround;
#include <limits>

namespace ystdex
{

/*!
\brief 取无符号数的以 2 为底的整数次幂。
\since build 260
*/
template<typename _tDst, typename _tSrc>
yconstfn _tDst
exp2u(_tSrc n) ynothrow
{
	return _tSrc(1) << n;
}


/*!
\brief 定点数乘除法中间类型。
\since build 260
\todo 保持 64 位类型精度。
*/
template<typename _type>
struct fixed_multiplicative
{
	typedef typename make_signed_c<typename make_fixed_width_int<
		integer_width<_type>::value << 1>::type,
		std::is_signed<_type>::value>::type type;
};

template<>
struct fixed_multiplicative<std::int64_t>
{
	typedef std::int64_t type;
};

template<>
struct fixed_multiplicative<std::uint64_t>
{
	typedef std::uint64_t type;
};


/*!
\brief 通用定点数。

基于整数算术实现的确定有限精度二进制定点小数类型。可用于替换内建的浮点数类型。
是否有符号同基本整数类型参数。若有符号，则最高有效位为符号位。
逻辑布局： [符号位]|整数部分|小数部分 。各个部分的内部为补码表示。
\tparam _tBase 基本整数类型。
\tparam _vInt 整数部分（若有符号则不包括符号位，下同）二进制位数。
\tparam _vFrac 分数部分二进制位数。
\warning 基本整数类型需要具有补码表示。
\warning 非虚析构。
\warning 算术运算可能溢出。
\note 默认保留 6 位二进制小数。
\note 部分实现参考： http://www.codeproject.com/KB/cpp/fp_math.aspx 。
\since build 260
*/
template<typename _tBase = std::int32_t,
	size_t _vInt = std::numeric_limits<_tBase>::digits - 6U,
	size_t _vFrac = std::numeric_limits<_tBase>::digits - _vInt>
class fixed_point : operators::ordered_field_operators<
	fixed_point<_tBase, _vInt, _vFrac>, operators::unit_steppable<
	fixed_point<_tBase, _vInt, _vFrac>, operators::shiftable<fixed_point<
	_tBase, _vInt, _vFrac>, std::size_t>>>
{
	static_assert(std::is_integral<_tBase>::value, "Non-integral type found.");
	static_assert(_vInt < size_t(std::numeric_limits<_tBase>::digits),
		"No sufficient fractional bits found.");
	static_assert(_vInt + _vFrac == size_t(std::numeric_limits<_tBase>::digits),
		"Wrong total bits found.");

	template<typename _OtherBase, size_t _vOtherInt, size_t _vOtherFrac>
	friend class fixed_point;
	friend class std::numeric_limits<fixed_point<_tBase, _vInt, _vFrac>>;

public:
	typedef _tBase base_type;

	//! \brief 整数部分二进制位数。
	static yconstexpr size_t int_bit_n = _vInt;
	//! \brief 小数部分二进制位数。
	static yconstexpr size_t frac_bit_n = _vFrac;
	//! \brief 非符号位的二进制位数。
	static yconstexpr size_t digit_bit_n = int_bit_n + frac_bit_n;

private:
	//! \brief 内部构造类型（ private 构造重载用）。
	typedef empty_base<fixed_point> internal_construct_tag;

	base_type value;

public:
	/*!
	\brief 无参数构造。
	\warning 基本整数类型成员未被初始化，具有未决定值 ，使用可能造成未定义行为。
	*/
	yconstfn
	fixed_point() ynothrow
	{}

private:
	yconstfn
	fixed_point(base_type v, internal_construct_tag) ynothrow
		: value(v)
	{}

public:
	template<typename _tInt>
	yconstfn
	fixed_point(_tInt val, typename std::enable_if<
		std::is_integral<_tInt>::value, int>::type = 0) ynothrow
		: value(base_type(val) << frac_bit_n)
	{}
	template<typename _tFloat>
	yconstfn
	fixed_point(_tFloat val, typename std::enable_if<std::is_floating_point<
		_tFloat>::value, int>::type = 0) ynothrow
		: value(::llround(base_element() * val))
	{
		// TODO: Use std::llround.
	}
	yconstfn
	fixed_point(const fixed_point&) = default;
	template<size_t _vOtherInt, size_t _vOtherFrac>
	yconstfn
	fixed_point(const fixed_point<base_type, _vOtherInt, _vOtherFrac>& f,
		typename std::enable_if<(_vOtherInt < int_bit_n), int>::type = 0)
		ynothrow
		: value(f.value >> (int_bit_n - _vOtherInt))
	{}
	template<size_t _vOtherInt, size_t _vOtherFrac>
	yconstfn
	fixed_point(const fixed_point<base_type, _vOtherInt, _vOtherFrac>& f,
		typename std::enable_if<(int_bit_n < _vOtherInt), int>::type = 0)
		ynothrow
		: value(f.value << (_vOtherInt - int_bit_n))
	{}

	bool
	operator<(const fixed_point& f) const ynothrow
	{
		return value < f.value;
	}

	bool
	operator==(const fixed_point& f) const ynothrow
	{
		return value == f.value;
	}

	bool
	operator!() const ynothrow
	{
		return value == 0;
	}

	fixed_point
	operator-() const ynothrow
	{
		fixed_point result;

		result.value = -value;
		return result;
	}

	fixed_point&
	operator++() ynothrow
	{
		value += base_element();
		return *this;
	}

	fixed_point&
	operator--() ynothrow
	{
		value -= base_element();
		return *this;
	}

	fixed_point&
	operator+=(const fixed_point& f) ynothrow
	{
		value += f.value;
		return *this;
	}

	fixed_point&
	operator-=(const fixed_point& f) ynothrow
	{
		value -= f.value;
		return *this;
	}

	fixed_point&
	operator*=(const fixed_point& f) ynothrow
	{
		typename fixed_multiplicative<base_type>::type
			tmp(value * f.value);
		yconstexpr size_t shift_bit_n(frac_bit_n
			+ std::is_signed<base_type>::value);

		value = tmp < 0 ? -(-tmp >> shift_bit_n) : tmp >> shift_bit_n;
	// NOTE: Code below is only fit for unsigned type, due to there exists
	//	implementation-defined in conversion and right shifting on
	//	operands of signed types.
	//	value = (typename fixed_multiplicative<base_type>::type(value)
	//		* f.value) >> shift_bit_n;
		return *this;
	}

	fixed_point&
	operator/=(const fixed_point& f) ynothrow
	{
		value = (typename fixed_multiplicative<base_type>::type(value)
			<< frac_bit_n) / f.value;
		return *this;
	}

	fixed_point&
	operator>>=(size_t s) ynothrow
	{
		value >>= s;
		return *this;
	}

	fixed_point&
	operator<<=(size_t s) ynothrow
	{
		value <<= s;
		return *this;
	}

	template<typename _type>
	inline
	operator _type() const
	{
		return this->cast<_type>();
	}

private:
	template<typename _type>
	inline typename std::enable_if<std::is_integral<_type>::value,
		_type>::type
	cast() const
	{
		return value >> frac_bit_n;
	}
	template<typename _type>
	typename std::enable_if<std::is_floating_point<_type>::value,
		_type>::type
	cast() const
	{
		return _type(value) / base_element();
	}

public:
	/*!
	\brief 取 \c base_type 表达的单位元。

	取值等于 1 的元素，使用 \c base_type 表达。
	*/
	static yconstfn base_type
	base_element() ynothrow
	{
		return ystdex::exp2u<base_type, base_type>(frac_bit_n);
	}

	/*!
	\brief 取单位元。

	取值等于 1 的元素。
	*/
	static yconstfn fixed_point
	identity() ynothrow
	{
		return fixed_point(base_element());
	}

	friend fixed_point
	fabs(fixed_point x)
	{
		return x.value < 0 ? -x : x;
	}

	friend yconstfn fixed_point
	ceil(fixed_point x)
	{
		return fixed_point((x.value + base_element() - 1)
			& ~(base_element() - 1), internal_construct_tag());
	}

	friend yconstfn fixed_point
	floor(fixed_point x)
	{
		return fixed_point(x.value & ~(base_element() - 1),
			internal_construct_tag());
	}

	friend yconstfn fixed_point
	round(fixed_point x)
	{
		return fixed_point((x.value + (base_element() >> 1))
			& ~(base_element() - 1), internal_construct_tag());
	}
};

} // namespace ystdex;


namespace std
{

/*!
\brief std::numeric_traits 的 ystdex::fixed_point 特化类型。
\since build 260
*/
template<typename _tBase, ystdex::size_t _vInt, ystdex::size_t _vFrac>
struct numeric_limits<ystdex::fixed_point<_tBase, _vInt, _vFrac>>
{
private:
	typedef ystdex::fixed_point<_tBase, _vInt, _vFrac> fp_type;
	typedef typename fp_type::base_type base_type;

public:
	static yconstexpr bool is_specialized = true;

	static yconstfn fp_type
	min() ynothrow
	{
		return fp_type(std::numeric_limits<base_type>::min(),
			fp_type::internal_construct_tag());
	}

	static yconstfn fp_type
	max() ynothrow
	{
		return fp_type(std::numeric_limits<base_type>::max(),
			fp_type::internal_construct_tag());
	}

	static yconstfn fp_type
	lowest() ynothrow
	{
		return min();
	}

	static yconstexpr int digits = _vInt;
	static yconstexpr int digits10 = digits * 643L / 2136;
	static yconstexpr int max_digits10 = 0;
	static yconstexpr bool is_signed = numeric_limits<base_type>::is_signed;
	static yconstexpr bool is_integer = false;
	static yconstexpr bool is_exact = true;
	static yconstexpr int radix = 2;

	static yconstfn fp_type
	epsilon() ynothrow
	{
		return fp_type(1, fp_type::internal_construct_tag());;
	}

	static yconstfn fp_type
	round_error() ynothrow
	{
		return 0.5;
	}

	static yconstexpr int min_exponent = 0;
	static yconstexpr int min_exponent10 = 0;
	static yconstexpr int max_exponent = 0;
	static yconstexpr int max_exponent10 = 0;

	static yconstexpr bool has_infinity = false;
	static yconstexpr bool has_quiet_NaN = false;
	static yconstexpr bool has_signaling_NaN = has_quiet_NaN;
	static yconstexpr float_denorm_style has_denorm = denorm_absent;
	static yconstexpr bool has_denorm_loss = false;

	static yconstfn fp_type
	infinity() ynothrow
	{
		return 0;
	}

	static yconstfn fp_type
	quiet_NaN() ynothrow
	{
		return 0;
	}

	static yconstfn fp_type
	signaling_NaN() ynothrow
	{
		return 0;
	}

	static yconstfn fp_type
	denorm_min() ynothrow
	{
		return 0;
	}

	static yconstexpr bool is_iec559 = false;
	static yconstexpr bool is_bounded = true;
	static yconstexpr bool is_modulo = numeric_limits<base_type>::is_modulo;

	static yconstexpr bool traps = numeric_limits<base_type>::traps;
	static yconstexpr bool tinyness_before = false;
	static yconstexpr float_round_style round_style = round_toward_zero;
};

} // namespace std;

#endif

