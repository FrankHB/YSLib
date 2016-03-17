/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file rational.hpp
\ingroup YStandardEx
\brief 有理数运算。
\version r2093
\author FrankHB <frankhb1989@gmail.com>
\since build 260
\par 创建时间:
	2011-11-12 23:23:47 +0800
\par 修改时间:
	2016-03-17 14:59 +0805
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Rational
*/


#ifndef YB_INC_ystdex_rational_hpp_
#define YB_INC_ystdex_rational_hpp_ 1

#include "cstdint.hpp" // for make_width_int, std::common_type, common_type_t,
//	std::numeric_limits;
#include "operators.hpp" // for operators;
#include <libdefect/cmath.h> // for std::llround;

namespace ystdex
{

//! \since build 588
//@{
//! \brief 判断指定的值是否为零值。
//@{
template<typename _type>
yconstfn yimpl(enable_if_t)<is_floating_point<_type>::value, bool>
is_zero(_type x)
{
	return std::fpclassify(x) == FP_ZERO;
}
template<typename _type,
	yimpl(typename = enable_if_t<!is_floating_point<_type>::value>)>
yconstfn bool
is_zero(_type x)
{
	return x == _type(0);
}
//@}


/*!
\brief 指定误差内舍入。
\pre 参数为有限值。
\warning 不检查参数。
*/
//@{
/*!
\note 不检查误差非零。
\note 使用 ADL \c round 。
*/
template<typename _type>
yconstfn _type
round_in_nocheck(_type x, _type err)
{
	using std::round;

	return round(x / err) * err;
}

template<typename _type>
yconstfn _type
round_in(_type x, _type err)
{
	return ystdex::is_zero(err) ? x : ystdex::round_in_nocheck(x, err);
}
//@}


//! \brief 计算整数次幂。
template<typename _type>
yconstfn _type
pow_int(_type x, size_t n)
{
	return n != 0 ? (pow_int(x, n / 2) * pow_int(x, n / 2)
		* (n % 2 == 1 ? _type(x) : _type(1))) : _type(1);
}

//! \brief 计算平方。
template<typename _type>
yconstfn _type
square(_type x)
{
	return x * x;
}

//! \brief 计算立方。
template<typename _type>
yconstfn _type
cube(_type x)
{
	return x * x * x;
}

//! \brief 计算四次方。
template<typename _type>
yconstfn _type
quad(_type x)
{
	return ystdex::square(ystdex::square(x));
}
//@}


/*!
\ingroup unary_type_traits
\brief 判断类型是否可满足正规化要求：在 0 和 1 之间存在值进行表示。
\since build 442
*/
template<typename _type>
struct is_normalizable : or_<is_floating_point<_type>>
{};


#define YB_Impl_Rational_fp_T fixed_point<_tBase, _vInt, _vFrac>
#define YB_Impl_Rational_fp_T1 fixed_point<_tBase1, _vInt1, _vFrac1>
#define YB_Impl_Rational_fp_T2 fixed_point<_tBase2, _vInt2, _vFrac2>
#define YB_Impl_Rational_fp_PList \
	typename _tBase, size_t _vInt, size_t _vFrac
#define YB_Impl_Rational_fp_PList1 \
	typename _tBase1, size_t _vInt1, size_t _vFrac1
#define YB_Impl_Rational_fp_PList2 \
	typename _tBase2, size_t _vInt2, size_t _vFrac2

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
class fixed_point : private operators<YB_Impl_Rational_fp_T>
{
	static_assert(is_integral<_tBase>(), "Non-integral type found.");
	static_assert(_vInt < size_t(std::numeric_limits<_tBase>::digits),
		"No sufficient fractional bits found.");
	static_assert(_vInt + _vFrac == size_t(std::numeric_limits<_tBase>::digits),
		"Wrong total bits found.");

	template<typename, size_t, size_t>
	friend class fixed_point;
	friend class std::numeric_limits<YB_Impl_Rational_fp_T>;

public:
	using base_type = _tBase;

	//! \brief 整数部分二进制位数。
	static yconstexpr const size_t int_bit_n = _vInt;
	//! \brief 小数部分二进制位数。
	static yconstexpr const size_t frac_bit_n = _vFrac;
	//! \brief 非符号位的二进制位数。
	static yconstexpr const size_t digit_bit_n = int_bit_n + frac_bit_n;

private:
	base_type value;

public:
	/*!
	\brief 无参数构造。
	\warning 基本整数类型成员未被初始化，具有未决定值，使用可能造成未定义行为。
	*/
	fixed_point() = default;
	//! \since build 439
	yconstfn
	fixed_point(base_type v, raw_tag) ynothrow
		: value(v)
	{}
	//! \since build 586
	template<typename _tInt>
	yconstfn
	fixed_point(_tInt val,
		yimpl(enable_if_t<is_integral<_tInt>::value, _tInt*> = {})) ynothrowv
		// XXX: Conversion to 'base_type' might be implementation-defined if
		//	it is signed.
		: value(base_type(val << frac_bit_n))
	{}
	//! \since build 581
	//@{
	template<typename _tFloat>
	explicit yconstfn
	fixed_point(_tFloat val, yimpl(enable_if_t<
		is_floating_point<_tFloat>::value, _tFloat*> = {})) ynothrow
		: value(base_type(std::llround(base_element() * val)))
	{}
	template<typename _tFirst, typename _tSecond>
	//! \since build 595
	//@{
	yconstfn
	fixed_point(_tFirst x, _tSecond y, yimpl(enable_if_t<and_<is_integral<
		_tFirst>, not_<is_same<_tSecond, raw_tag>>>::value, _tFirst*> = {}))
		ynothrowv
		: value((x << frac_bit_n) / y)
	{}
	template<typename _tFirst, typename _tSecond>
	explicit yconstfn
	fixed_point(_tFirst x, _tSecond y, yimpl(enable_if_t<and_<is_floating_point<
		_tFirst>, not_<is_same<_tSecond, raw_tag>>>::value, _tFirst*> = {}))
		ynothrowv
		: fixed_point(x / y)
	{}
	//@}
	//! \since build 260
	yconstfn
	fixed_point(const fixed_point&) = default;
	template<size_t _vOtherInt, size_t _vOtherFrac>
	yconstfn
	fixed_point(const fixed_point<base_type, _vOtherInt, _vOtherFrac>& f,
		yimpl(enable_if_t<(_vOtherInt < int_bit_n), base_type*> = {})) ynothrow
		: value(f.value >> (int_bit_n - _vOtherInt))
	{}
	template<size_t _vOtherInt, size_t _vOtherFrac>
	yconstfn
	fixed_point(const fixed_point<base_type, _vOtherInt, _vOtherFrac>& f,
		yimpl(enable_if_t<(int_bit_n < _vOtherInt), base_type*> = {})) ynothrow
		: value(f.value << (_vOtherInt - int_bit_n))
	{}
	template<typename _tOtherBase, size_t _vOtherInt, size_t _vOtherFrac>
	yconstfn
	fixed_point(const fixed_point<_tOtherBase, _vOtherInt, _vOtherFrac>& f,
		yimpl(enable_if_t<(frac_bit_n == _vOtherFrac), base_type*> = {}))
		ynothrow
		: value(f.value)
	{}
	template<typename _tOtherBase, size_t _vOtherInt, size_t _vOtherFrac>
	yconstfn
	fixed_point(const fixed_point<_tOtherBase, _vOtherInt, _vOtherFrac>& f,
		yimpl(enable_if_t<(frac_bit_n < _vOtherFrac), base_type*> = {}))
		ynothrow
		: value(f.value >> (_vOtherFrac - frac_bit_n))
	{}
	template<typename _tOtherBase, size_t _vOtherInt, size_t _vOtherFrac>
	yconstfn
	fixed_point(const fixed_point<_tOtherBase, _vOtherInt, _vOtherFrac>& f,
		yimpl(enable_if_t<(_vOtherFrac < frac_bit_n), base_type*> = {}))
		ynothrow
		: value(f.value << (frac_bit_n - _vOtherFrac))
	{}
	//@}

	//! \since build 586
	fixed_point&
	operator=(const fixed_point&) = default;

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

	//! \since build 595
	//@{
	fixed_point&
	operator++() ynothrowv
	{
		value += base_element();
		return *this;
	}

	fixed_point&
	operator--() ynothrowv
	{
		value -= base_element();
		return *this;
	}

	fixed_point&
	operator+=(const fixed_point& f) ynothrowv
	{
		value += f.value;
		return *this;
	}

	fixed_point&
	operator-=(const fixed_point& f) ynothrowv
	{
		value -= f.value;
		return *this;
	}

	fixed_point&
	operator*=(const fixed_point& f) ynothrowv
	{
		value = mul<frac_bit_n + is_signed<base_type>::value>(value,
			f.value, bool_constant<is_signed<typename
			make_widen_int<base_type>::type>::value>());
		return *this;
	}

	fixed_point&
	operator/=(const fixed_point& f) ynothrowv
	{
		using widen_type = _t<make_widen_int<base_type>>;

		value = base_type((widen_type(value) << widen_type(frac_bit_n))
			/ f.value);
		return *this;
	}
	//@}

	fixed_point&
	operator>>=(size_t s) ynothrow
	{
		value >>= s;
		return *this;
	}

	//! \since build 595
	fixed_point&
	operator<<=(size_t s) ynothrowv
	{
		value <<= s;
		return *this;
	}

	//! \since build 581
	//@{
	template<typename _type,
		yimpl(typename = enable_if_t<is_integral<_type>::value, _type>)>
	explicit yconstfn
	operator _type() const ynothrow
	{
		return _type(value >> base_type(frac_bit_n));
	}
	template<typename _type, yimpl(typename _type2 = _type,
		typename = enable_if_t<is_floating_point<_type2>::value, _type>)>
	yconstfn
	operator _type() const ynothrow
	{
		return _type(value) / base_element();
	}
	//@}

	//! \since build 595
	//@{
	yconstfn base_type
	get() const ynothrow
	{
		return value;
	}

private:
	template<size_t _vShiftBits>
	static yconstfn base_type
	mul(base_type x, base_type y, true_type) ynothrowv
	{
		return mul_signed<_vShiftBits>(
			_t<make_widen_int<base_type>>(x * y));
	}
	template<size_t _vShiftBits>
	static yconstfn base_type
	mul(base_type x, base_type y, false_type) ynothrowv
	{
		// NOTE: Only fit for unsigned type, due to there exists
		//	implementation-defined behavior in conversion and right shifting on
		//	operands of signed types.
		return base_type((_t<make_widen_int<base_type>>(x) * y)
			>> _vShiftBits);
	}
	//@}

	//! \since build 650
	template<size_t _vShiftBits>
	static yconstfn base_type
	mul_signed(_t<make_widen_int<base_type>> tmp) ynothrowv
	{
		return base_type(tmp < 0 ? -(-tmp >> _vShiftBits) : tmp >> _vShiftBits);
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

	friend yconstfn fixed_point
	fabs(fixed_point x) ynothrow
	{
		return x.value < 0 ? -x : x;
	}

	friend yconstfn fixed_point
	ceil(fixed_point x) ynothrow
	{
		return fixed_point(
			(x.value + base_element() - 1) & ~(base_element() - 1), raw_tag());
	}

	friend yconstfn fixed_point
	floor(fixed_point x) ynothrow
	{
		return fixed_point(x.value & ~(base_element() - 1), raw_tag());
	}

	friend yconstfn fixed_point
	round(fixed_point x) ynothrow
	{
		return fixed_point((x.value + (base_element() >> 1))
			& ~(base_element() - 1), raw_tag());
	}
};

//! \relates fixed_point
//@{
#define YB_Impl_Rational_fp_TmplHead_2 \
	template<YB_Impl_Rational_fp_PList1, YB_Impl_Rational_fp_PList2>
#define YB_Impl_Rational_fp_TmplHead_2_l \
	template<YB_Impl_Rational_fp_PList, typename _type>
#define YB_Impl_Rational_fp_TmplHead_2_r \
	template<typename _type, YB_Impl_Rational_fp_PList>
#define YB_Impl_Rational_fp_TmplBody_Impl_2(_op) \
	{ \
		using result_type \
			= common_type_t<decay_t<decltype(x)>, decay_t<decltype(y)>>; \
	\
		return result_type(x) _op result_type(y); \
	}
#define YB_Impl_Rational_fp_TmplSig_2(_op) \
	operator _op(const YB_Impl_Rational_fp_T1& x, \
		const YB_Impl_Rational_fp_T2& y)
#define YB_Impl_Rational_fp_TmplSig_2_l(_op) \
	operator _op(const YB_Impl_Rational_fp_T& x, const _type& y)
#define YB_Impl_Rational_fp_TmplSig_2_r(_op) \
	operator _op(const _type& x, const YB_Impl_Rational_fp_T& y)

/*!
\brief 不同模板参数的二元算术操作符。
\since build 439
*/
//@{
#define YB_Impl_Rational_fp_arithmetic2(_op) \
	YB_Impl_Rational_fp_TmplHead_2 \
	yconstfn common_type_t<YB_Impl_Rational_fp_T1, YB_Impl_Rational_fp_T2> \
	YB_Impl_Rational_fp_TmplSig_2(_op) \
	YB_Impl_Rational_fp_TmplBody_Impl_2(_op) \
	\
	YB_Impl_Rational_fp_TmplHead_2_l \
	yconstfn enable_if_t<std::is_floating_point<_type>::value, \
		common_type_t<YB_Impl_Rational_fp_T, _type>> \
	YB_Impl_Rational_fp_TmplSig_2_l(_op) \
	YB_Impl_Rational_fp_TmplBody_Impl_2(_op) \
	\
	YB_Impl_Rational_fp_TmplHead_2_r \
	yconstfn enable_if_t<std::is_floating_point<_type>::value, \
		common_type_t<_type, YB_Impl_Rational_fp_T>> \
	YB_Impl_Rational_fp_TmplSig_2_r(_op) \
	YB_Impl_Rational_fp_TmplBody_Impl_2(_op)

YB_Impl_Rational_fp_arithmetic2(+)
YB_Impl_Rational_fp_arithmetic2(-)
YB_Impl_Rational_fp_arithmetic2(*)
YB_Impl_Rational_fp_arithmetic2(/)

#undef YB_Impl_Rational_fp_ar2
//@}

/*!
\brief 不同模板参数的二元关系操作符。
\since build 440
*/
//@{
#define YB_Impl_Rational_fp_rational2(_op) \
	YB_Impl_Rational_fp_TmplHead_2 \
	yconstfn bool \
	YB_Impl_Rational_fp_TmplSig_2(_op) \
	YB_Impl_Rational_fp_TmplBody_Impl_2(_op) \
	\
	YB_Impl_Rational_fp_TmplHead_2_l \
	yconstfn bool \
	YB_Impl_Rational_fp_TmplSig_2_l(_op) \
	YB_Impl_Rational_fp_TmplBody_Impl_2(_op) \
	\
	YB_Impl_Rational_fp_TmplHead_2_r \
	yconstfn bool \
	YB_Impl_Rational_fp_TmplSig_2_r(_op) \
	YB_Impl_Rational_fp_TmplBody_Impl_2(_op)

YB_Impl_Rational_fp_rational2(==)
YB_Impl_Rational_fp_rational2(!=)
YB_Impl_Rational_fp_rational2(<)
YB_Impl_Rational_fp_rational2(<=)
YB_Impl_Rational_fp_rational2(>)
YB_Impl_Rational_fp_rational2(>=)

#undef YB_Impl_Rational_fp_rational2
//@}

#undef YB_Impl_Rational_fp_TmplSig_2_r
#undef YB_Impl_Rational_fp_TmplSig_2_l
#undef YB_Impl_Rational_fp_TmplSig_2
#undef YB_Impl_Rational_fp_TmplBody_Impl_2
#undef YB_Impl_Rational_fp_TmplHead_2_r
#undef YB_Impl_Rational_fp_TmplHead_2_l
#undef YB_Impl_Rational_fp_TmplHead_2

//! \since build 595
template<YB_Impl_Rational_fp_PList>
yconstfn YB_Impl_Rational_fp_T
abs(YB_Impl_Rational_fp_T x) ynothrow
{
	return fabs(x);
}
//@}


/*!
\brief modular_arithmetic 的 fixed_point 特化类型。
\note 使用保留公共整数类型和整数位数策略选取公共类型。
\since build 440
*/
template<YB_Impl_Rational_fp_PList>
struct modular_arithmetic<YB_Impl_Rational_fp_T>
	: modular_arithmetic<typename YB_Impl_Rational_fp_T::base_type>
{};


/*!
\brief is_normalizable 的 fixed_point 特化类型。
\since build 442
*/
template<YB_Impl_Rational_fp_PList>
struct is_normalizable<YB_Impl_Rational_fp_T> : true_type
{};


/*!
\brief 取包括指定有效二进制位和至少指定位整数的定点数类型。
\since build 561
*/
template<size_t _vFrac, size_t _vInt = 1, bool _bSigned = false>
using make_fixed_t = fixed_point<conditional_t<_bSigned,
	typename make_width_int<_vFrac + _vInt>::least_type,
	typename make_width_int<_vFrac + _vInt>::unsigned_least_type>, _vFrac>;

} // namespace ystdex;


namespace std
{

/*!
\brief std::common_type 的 ystdex::fixed_point 特化类型。
\note 使用保留公共整数类型和整数位数策略选取公共类型。
\since build 439
*/
template<YB_Impl_Rational_fp_PList1, YB_Impl_Rational_fp_PList2>
struct common_type<ystdex::YB_Impl_Rational_fp_T1,
	ystdex::YB_Impl_Rational_fp_T2>
{
private:
	using common_base_type = ystdex::common_type_t<_tBase1, _tBase2>;

	static yconstexpr const size_t int_size = _vInt1 < _vInt2 ? _vInt2 : _vInt1;

public:
	using type = ystdex::fixed_point<common_base_type, int_size,
		std::numeric_limits<common_base_type>::digits - int_size>;
};

/*!
\brief std::common_type 的 ystdex::fixed_point 和其它类型的特化类型。
\since build 558
\todo 支持范围不小于定点数的可以转换为 std::double_t 的类型为公共类型。

当其它类型是浮点数时即为公共类型，
否则 ystdex::fixed_point 的实例为公共类型。
*/
//@{
template<YB_Impl_Rational_fp_PList, typename _type>
struct common_type<ystdex::YB_Impl_Rational_fp_T, _type>
{
private:
	using fixed = ystdex::YB_Impl_Rational_fp_T;

public:
	using type = ystdex::cond_t<is_floating_point<_type>
#if 0
		|| !(std::double_t(std::numeric_limits<fixed>::min())
		< std::double_t(std::numeric_limits<_type>::min())
		|| std::double_t(std::numeric_limits<_type>::max())
		< std::double_t(std::numeric_limits<fixed>::max()))
#endif
		, _type, fixed>;
};

template<typename _type, YB_Impl_Rational_fp_PList>
struct common_type<_type, ystdex::YB_Impl_Rational_fp_T>
{
	using type = ystdex::common_type_t<ystdex::YB_Impl_Rational_fp_T, _type>;
};
//@}


/*!
\brief ystdex::fixed_point 散列支持。
\since build 590
*/
//@{
template<typename>
struct hash;

template<YB_Impl_Rational_fp_PList>
struct hash<ystdex::YB_Impl_Rational_fp_T>
{
	size_t
	operator()(const ystdex::YB_Impl_Rational_fp_T& k) const
		ynoexcept_spec(yimpl(hash<_tBase>{}(k.get())))
	{
		return hash<_tBase>{}(k.get());
	}
};
//@}


/*!
\brief std::numeric_rational 的 ystdex::fixed_point 特化类型。
\see http://wg21.cmeerw.net/lwg/issue201
\see http://stackoverflow.com/questions/16122912/is-it-ok-to-specialize-stdnumeric-limitst-for-user-defined-number-like-class
\since build 260
*/
template<YB_Impl_Rational_fp_PList>
class numeric_limits<ystdex::YB_Impl_Rational_fp_T>
{
private:
	using fp_type = ystdex::YB_Impl_Rational_fp_T;
	using base_type = typename fp_type::base_type;

public:
	static yconstexpr const bool is_specialized = true;

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

	static yconstexpr const int digits = _vInt;
	static yconstexpr const int digits10 = digits * 643L / 2136;
	static yconstexpr const int max_digits10 = 0;
	static yconstexpr const bool is_signed = numeric_limits<base_type>::is_signed;
	static yconstexpr const bool is_integer = {};
	static yconstexpr const bool is_exact = true;
	static yconstexpr const int radix = 2;

	static yconstfn fp_type
	epsilon() ynothrow
	{
		return fp_type(1, typename ystdex::raw_tag());
	}

	static yconstfn fp_type
	round_error() ynothrow
	{
		return 0.5;
	}

	static yconstexpr const int min_exponent = 0;
	static yconstexpr const int min_exponent10 = 0;
	static yconstexpr const int max_exponent = 0;
	static yconstexpr const int max_exponent10 = 0;

	static yconstexpr const bool has_infinity = {};
	static yconstexpr const bool has_quiet_NaN = {};
	static yconstexpr const bool has_signaling_NaN = has_quiet_NaN;
	static yconstexpr const float_denorm_style has_denorm = denorm_absent;
	static yconstexpr const bool has_denorm_loss = {};

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

	static yconstexpr const bool is_iec559 = {};
	static yconstexpr const bool is_bounded = true;
	static yconstexpr const bool is_modulo
		= numeric_limits<base_type>::is_modulo;

	static yconstexpr const bool traps = numeric_limits<base_type>::traps;
	static yconstexpr const bool tinyness_before = {};
	static yconstexpr const float_round_style round_style = round_toward_zero;
};

//! \since build 688
//@{
template<YB_Impl_Rational_fp_PList>
class numeric_limits<const ystdex::YB_Impl_Rational_fp_T>
	: yimpl(public) numeric_limits<ystdex::YB_Impl_Rational_fp_T>
{};

template<YB_Impl_Rational_fp_PList>
class numeric_limits<volatile ystdex::YB_Impl_Rational_fp_T>
	: yimpl(public) numeric_limits<ystdex::YB_Impl_Rational_fp_T>
{};

template<YB_Impl_Rational_fp_PList>
class numeric_limits<const volatile ystdex::YB_Impl_Rational_fp_T>
	: yimpl(public) numeric_limits<ystdex::YB_Impl_Rational_fp_T>
{};
//@}

#undef YB_Impl_Rational_fp_PList2
#undef YB_Impl_Rational_fp_PList1
#undef YB_Impl_Rational_fp_PList
#undef YB_Impl_Rational_fp_T2
#undef YB_Impl_Rational_fp_T1
#undef YB_Impl_Rational_fp_T

} // namespace std;

#endif

