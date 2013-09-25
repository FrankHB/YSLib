/*
	Copyright by FrankHB 2011-2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file rational.hpp
\ingroup YStandardEx
\brief 有理数运算。
\version r1496
\author FrankHB <frankhb1989@gmail.com>
\since build 260
\par 创建时间:
	2011-11-12 23:23:47 +0800
\par 修改时间:
	2013-09-23 12:18 +0805
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Rational
*/


#ifndef YB_INC_ystdex_rational_hpp_
#define YB_INC_ystdex_rational_hpp_ 1

#include "cstdint.hpp"
#include "operators.hpp"
#include <cmath> // for std::llround;

namespace ystdex
{

/*!
\brief 取算术类型的正规化后的最大值。
\since build 439
\todo 静态断言限制类型。
*/
//@{
template<typename _type, std::uintmax_t _vNum = 1, std::uintmax_t _vDen = 1,
	bool _bIsFloat = is_floating_point<_type>::value>
struct normalized_max;

template<typename _type, std::uintmax_t _vNum, std::uintmax_t _vDen>
struct normalized_max<_type, _vNum, _vDen, true>
{
	static yconstexpr _type value = double(_vNum / _vDen);

	//! \since build 440
	static yconstfn _type
	get()
	{
		return value;
	}
};

template<typename _type, std::uintmax_t _vNum, std::uintmax_t _vDen>
struct normalized_max<_type, _vNum, _vDen, false>
{
	static yconstexpr _type value = std::numeric_limits<_type>::max();

	//! \since build 440
	static yconstfn _type
	get()
	{
		return value;
	}
};
//@}


/*!
\ingroup unary_type_trait
\brief 判断类型是否可满足归一化要求。
\since build 442
*/
//@{
template<typename _type>
struct is_normalizable
	: integral_constant<bool, is_floating_point<_type>::value>
{};

template<>
struct is_normalizable<bool> : true_type
{};
//@}


/*!
\brief 定点数乘除法中间类型。
\since build 260
\todo 保持 64 位类型精度。
*/
template<typename _type>
struct fixed_multiplicative
{
	using type = typename make_signed_c<typename make_width_int<integer_width<
		_type>::value << 1>::type, is_signed<_type>::value>::type;
};

template<>
struct fixed_multiplicative<std::int64_t>
{
	using type = std::int64_t;
};

template<>
struct fixed_multiplicative<std::uint64_t>
{
	using type = std::uint64_t;
};


/*!
\brief 通用定点数。

基于整数算术实现的确定有限精度二进制定点小数类型。可用于替换内建的浮点数类型。
是否有符号同基本整数类型参数。若有符号，则最高有效位为符号位。
逻辑布局： [符号位]|整数部分|小数部分 。各个部分的内部为补码表示。
\tparam _tBase 基本整数类型。
\tparam _vInt 整数部分（若有符号则不包括符号位，下同）二进制位数。
\tparam _vFrac 分数部分二进制位数。
\note 默认保留 6 位二进制小数。
\note 部分实现参考： http://www.codeproject.com/KB/cpp/fp_math.aspx 。
\warning 基本整数类型需要具有补码表示。
\warning 非虚析构。
\warning 算术运算可能溢出。
\since build 260
\todo 实现模除和位操作。
\todo 根据范围禁止算术类型隐式转换。
*/
template<typename _tBase = std::int32_t,
	size_t _vInt = std::numeric_limits<_tBase>::digits - 6U,
	size_t _vFrac = std::numeric_limits<_tBase>::digits - _vInt>
class fixed_point : public operators<fixed_point<_tBase, _vInt, _vFrac>>
{
	static_assert(is_integral<_tBase>::value, "Non-integral type found.");
	static_assert(_vInt < size_t(std::numeric_limits<_tBase>::digits),
		"No sufficient fractional bits found.");
	static_assert(_vInt + _vFrac == size_t(std::numeric_limits<_tBase>::digits),
		"Wrong total bits found.");

	template<typename _OtherBase, size_t _vOtherInt, size_t _vOtherFrac>
	friend class fixed_point;
	friend class std::numeric_limits<fixed_point<_tBase, _vInt, _vFrac>>;

public:
	using base_type = _tBase;

	//! \brief 整数部分二进制位数。
	static yconstexpr size_t int_bit_n = _vInt;
	//! \brief 小数部分二进制位数。
	static yconstexpr size_t frac_bit_n = _vFrac;
	//! \brief 非符号位的二进制位数。
	static yconstexpr size_t digit_bit_n = int_bit_n + frac_bit_n;

private:
	base_type value;

public:
	/*!
	\brief 无参数构造。
	\warning 基本整数类型成员未被初始化，具有未决定值 ，使用可能造成未定义行为。
	*/
	yconstfn
	fixed_point() ynothrow
	{}

	//! \since build 439
	//@{
	yconstfn
	fixed_point(base_type v, raw_tag) ynothrow
		: value(v)
	{}
	template<typename _tInt>
	yconstfn
	fixed_point(_tInt val, enable_if_t<is_integral<_tInt>::value, _tInt*> = {})
		ynothrow
		: value(base_type(val) << frac_bit_n)
	{}
	template<typename _tFloat>
	yconstfn
	fixed_point(_tFloat val,
		enable_if_t<is_floating_point<_tFloat>::value, _tFloat*> = {}) ynothrow
		: value(::llround(base_element() * val))
	{
		// TODO: Use std::llround.
	}
	template<typename _tFirst, typename _tSecond>
	yconstfn
	fixed_point(_tFirst x, _tSecond y, enable_if_t<is_integral<_tFirst>::value
		&& !is_same<_tSecond, raw_tag>::value, _tFirst*> = {})
		: value((x << frac_bit_n) / y)
	{}
	template<typename _tFirst, typename _tSecond>
	yconstfn
	fixed_point(_tFirst x, _tSecond y, enable_if_t<is_floating_point<_tFirst>
		::value && !is_same<_tSecond, raw_tag>::value, _tFirst*> = {})
		: fixed_point(x / y)
	{}
	//! \since build 260
	yconstfn
	fixed_point(const fixed_point&) = default;
	template<size_t _vOtherInt, size_t _vOtherFrac>
	yconstfn
	fixed_point(const fixed_point<base_type, _vOtherInt, _vOtherFrac>& f,
		enable_if_t<(_vOtherInt < int_bit_n), base_type*> = {}) ynothrow
		: value(f.value >> (int_bit_n - _vOtherInt))
	{}
	template<size_t _vOtherInt, size_t _vOtherFrac>
	yconstfn
	fixed_point(const fixed_point<base_type, _vOtherInt, _vOtherFrac>& f,
		enable_if_t<(int_bit_n < _vOtherInt), base_type*> = {}) ynothrow
		: value(f.value << (_vOtherInt - int_bit_n))
	{}
	template<typename _tOtherBase, size_t _vOtherInt, size_t _vOtherFrac>
	yconstfn
	fixed_point(const fixed_point<_tOtherBase, _vOtherInt, _vOtherFrac>& f,
		enable_if_t<(frac_bit_n == _vOtherFrac), base_type*> = {}) ynothrow
		: value(f.value)
	{}
	template<typename _tOtherBase, size_t _vOtherInt, size_t _vOtherFrac>
	yconstfn
	fixed_point(const fixed_point<_tOtherBase, _vOtherInt, _vOtherFrac>& f,
		enable_if_t<(frac_bit_n < _vOtherFrac), base_type*> = {}) ynothrow
		: value(f.value >> (_vOtherFrac - frac_bit_n))
	{}
	template<typename _tOtherBase, size_t _vOtherInt, size_t _vOtherFrac>
	yconstfn
	fixed_point(const fixed_point<_tOtherBase, _vOtherInt, _vOtherFrac>& f,
		enable_if_t<(_vOtherFrac < frac_bit_n), base_type*> = {}) ynothrow
		: value(f.value << (frac_bit_n - _vOtherFrac))
	{}
	//@}

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
		value = mul<frac_bit_n + is_signed<base_type>::value>(value,
			f.value, integral_constant<bool, is_signed<
			typename fixed_multiplicative<base_type>::type>::value>());
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

	//! \since build 439
	template<typename _type,
		typename = enable_if_t<is_arithmetic<_type>::value, _type>>
	yconstfn
	operator _type() const
	{
		return this->cast<_type>();
	}

	//! \since build 439
	yconstfn base_type
	get() const
	{
		return value;
	}

private:
	//! \since build 439
	template<typename _type>
	yconstfn enable_if_t<is_integral<_type>::value, _type>
	cast() const
	{
		return value >> frac_bit_n;
	}
	//! \since build 439
	template<typename _type>
	enable_if_t<is_floating_point<_type>::value, _type>
	cast() const
	{
		return _type(value) / base_element();
	}

	template<size_t _vShiftBits>
	static yconstfn base_type
	mul(base_type x, base_type y, true_type)
	{
		return mul_signed<_vShiftBits>(
			typename fixed_multiplicative<base_type>::type(x * y));
	}
	template<size_t _vShiftBits>
	static yconstfn base_type
	mul(base_type x, base_type y, false_type)
	{
		// NOTE: Only fit for unsigned type, due to there exists
		//	implementation-defined behavior in conversion and right shifting on
		//	operands of signed types.
		return (typename fixed_multiplicative<base_type>::type(x) * y)
			>> _vShiftBits;
	}

	template<size_t _vShiftBits>
	static yconstfn base_type
	mul_signed(typename fixed_multiplicative<base_type>::type tmp)
	{
		return tmp < 0 ? -(-tmp >> _vShiftBits) : tmp >> _vShiftBits;
	}

public:
	/*!
	\brief 取 \c base_type 表达的单位元。

	取值等于 1 的元素，使用 \c base_type 表达。
	*/
	static yconstfn base_type
	base_element() ynothrow
	{
		return base_type(1) << frac_bit_n;
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
		return fixed_point(
			(x.value + base_element() - 1) & ~(base_element() - 1), raw_tag());
	}

	friend yconstfn fixed_point
	floor(fixed_point x)
	{
		return fixed_point(x.value & ~(base_element() - 1), raw_tag());
	}

	friend yconstfn fixed_point
	round(fixed_point x)
	{
		return fixed_point((x.value + (base_element() >> 1))
			& ~(base_element() - 1), raw_tag());
	}
};

//!\since build 440
#define YB_FIX_POINT_TMPL_HEAD_2 \
	template<typename _tBase1, size_t _vInt1, size_t _vFrac1, \
		typename _tBase2, size_t _vInt2, size_t _vFrac2>
//!\since build 440
#define YB_FIX_POINT_TMPL_OP_2_PARAMS_BODY(_op) \
	operator _op(const fixed_point<_tBase1, _vInt1, _vFrac1> x, \
		const fixed_point<_tBase2, _vInt2, _vFrac2>& y) \
	{ \
		using result_type = common_type_t<fixed_point<_tBase1, _vInt1, \
			_vFrac1>, fixed_point<_tBase2, _vInt2, _vFrac2>>; \
	\
		return result_type(x) _op result_type(y); \
	}

/*!
\brief 不同模板参数的二元算术操作符。
\relates fixed_point
\since build 439
*/
//@{
//!\since build 440
#define YB_FIX_POINT_ARITHMETIC_2(_op) \
	YB_FIX_POINT_TMPL_HEAD_2 \
	yconstfn common_type_t<fixed_point<_tBase1, _vInt1, _vFrac1>, \
		fixed_point<_tBase2, _vInt2, _vFrac2>> \
	YB_FIX_POINT_TMPL_OP_2_PARAMS_BODY(_op)

YB_FIX_POINT_ARITHMETIC_2(+)
YB_FIX_POINT_ARITHMETIC_2(-)
YB_FIX_POINT_ARITHMETIC_2(*)
YB_FIX_POINT_ARITHMETIC_2(/)

#undef YB_FIX_POINT_ARITHMETIC_2
//@}

/*!
\brief 不同模板参数的二元关系操作符。
\relates fixed_point
\since build 440
*/
//@{
//!\since build 440
#define YB_FIX_POINT_RATIONAL_2(_op) \
	YB_FIX_POINT_TMPL_HEAD_2 \
	yconstfn bool \
	YB_FIX_POINT_TMPL_OP_2_PARAMS_BODY(_op)

YB_FIX_POINT_RATIONAL_2(==)
YB_FIX_POINT_RATIONAL_2(!=)
YB_FIX_POINT_RATIONAL_2(<)
YB_FIX_POINT_RATIONAL_2(<=)
YB_FIX_POINT_RATIONAL_2(>)
YB_FIX_POINT_RATIONAL_2(>=)

#undef YB_FIX_POINT_RATIONAL_2
//@}

#undef YB_FIX_POINT_TMPL_HEAD_2
#undef YB_FIX_POINT_TMPL_OP_2_PARAMS_BODY


/*!
\brief modular_arithmetic 的 fixed_point 特化类型。
\note 使用保留公共整数类型和整数位数策略选取公共类型。
\since build 440
*/
template<typename _tBase, size_t _vInt, size_t _vFrac>
struct modular_arithmetic<fixed_point<_tBase, _vInt, _vFrac>>
	: modular_arithmetic<typename fixed_point<_tBase, _vInt, _vFrac>::base_type>
{};


/*!
\brief is_normalizable 的 fixed_point 特化类型。
\since build 442
*/
template<typename _tBase, size_t _vInt, size_t _vFrac>
struct is_normalizable<fixed_point<_tBase, _vInt, _vFrac>> : true_type
{};

} // namespace ystdex;


namespace std
{

/*!
\brief std::common_type 的 ystdex::fixed_point 特化类型。
\note 使用保留公共整数类型和整数位数策略选取公共类型。
\since build 439
*/
template<typename _tBase1, size_t _vInt1, size_t _vFrac1, typename _tBase2,
	size_t _vInt2, size_t _vFrac2>
struct common_type<ystdex::fixed_point<_tBase1, _vInt1, _vFrac1>,
	ystdex::fixed_point<_tBase2, _vInt2, _vFrac2>>
{
private:
	using common_base_type = ystdex::common_type_t<_tBase1, _tBase2>;

	static yconstexpr size_t int_size = _vInt1 < _vInt2 ? _vInt2 : _vInt1;

public:
    using type = ystdex::fixed_point<common_base_type, int_size,
		std::numeric_limits<common_base_type>::digits - int_size>;
};


/*!
\brief std::numeric_traits 的 ystdex::fixed_point 特化类型。
\since build 260
*/
template<typename _tBase, ystdex::size_t _vInt, ystdex::size_t _vFrac>
class numeric_limits<ystdex::fixed_point<_tBase, _vInt, _vFrac>>
{
private:
	using fp_type = ystdex::fixed_point<_tBase, _vInt, _vFrac>;
	using base_type = typename fp_type::base_type;

public:
	static yconstexpr bool is_specialized = true;

	static yconstfn fp_type
	min() ynothrow
	{
		return fp_type(std::numeric_limits<base_type>::min(),
			ystdex::raw_tag());
	}

	static yconstfn fp_type
	max() ynothrow
	{
		return fp_type(std::numeric_limits<base_type>::max(),
			ystdex::raw_tag());
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
	static yconstexpr bool is_integer = {};
	static yconstexpr bool is_exact = true;
	static yconstexpr int radix = 2;

	static yconstfn fp_type
	epsilon() ynothrow
	{
		return fp_type(1, typename fp_type::raw_tag());
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

	static yconstexpr bool has_infinity = {};
	static yconstexpr bool has_quiet_NaN = {};
	static yconstexpr bool has_signaling_NaN = has_quiet_NaN;
	static yconstexpr float_denorm_style has_denorm = denorm_absent;
	static yconstexpr bool has_denorm_loss = {};

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

	static yconstexpr bool is_iec559 = {};
	static yconstexpr bool is_bounded = true;
	static yconstexpr bool is_modulo = numeric_limits<base_type>::is_modulo;

	static yconstexpr bool traps = numeric_limits<base_type>::traps;
	static yconstexpr bool tinyness_before = {};
	static yconstexpr float_round_style round_style = round_toward_zero;
};

} // namespace std;

#endif

