/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cmath.h
\ingroup LibDefect
\brief 标准库实现 \c \<cmath\> 修正。
\version r412
\author FrankHB <frankhb1989@gmail.com>
\since build 308
\par 创建时间:
	2014-11-24 06:53:27 +0800
\par 修改时间:
	2015-12-27 21:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	LibDefect::CMath
*/


#ifndef YB_INC_libdefect_cmath_h_
#define YB_INC_libdefect_cmath_h_ 1

#include <cmath>

#if defined(__GLIBCXX__) \
	&& (defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L) \
	&& !(defined(_GLIBCXX_USE_C99_MATH_TR1))

namespace std _GLIBCXX_VISIBILITY(default)
{

#undef acosh
#undef acoshf
#undef acoshl
#undef asinh
#undef asinhf
#undef asinhl
#undef atanh
#undef atanhf
#undef atanhl
#undef cbrt
#undef cbrtf
#undef cbrtl
#undef copysign
#undef copysignf
#undef copysignl
#undef erf
#undef erff
#undef erfl
#undef erfc
#undef erfcf
#undef erfcl
#undef exp2
#undef exp2f
#undef exp2l
#undef expm1
#undef expm1f
#undef expm1l
#undef fdim
#undef fdimf
#undef fdiml
#undef fma
#undef fmaf
#undef fmal
#undef fmax
#undef fmaxf
#undef fmaxl
#undef fmin
#undef fminf
#undef fminl
#undef hypot
#undef hypotf
#undef hypotl
#undef ilogb
#undef ilogbf
#undef ilogbl
#undef lgamma
#undef lgammaf
#undef lgammal
#undef llrint
#undef llrintf
#undef llrintl
#undef llround
#undef llroundf
#undef llroundl
#undef log1p
#undef log1pf
#undef log1pl
#undef log2
#undef log2f
#undef log2l
#undef logb
#undef logbf
#undef logbl
#undef lrint
#undef lrintf
#undef lrintl
#undef lround
#undef lroundf
#undef lroundl
#undef nan
#undef nanf
#undef nanl
#undef nearbyint
#undef nearbyintf
#undef nearbyintl
#undef nextafter
#undef nextafterf
#undef nextafterl
#undef nexttoward
#undef nexttowardf
#undef nexttowardl
#undef remainder
#undef remainderf
#undef remainderl
#undef remquo
#undef remquof
#undef remquol
#undef rint
#undef rintf
#undef rintl
#undef round
#undef roundf
#undef roundl
#undef scalbln
#undef scalblnf
#undef scalblnl
#undef scalbn
#undef scalbnf
#undef scalbnl
#undef tgamma
#undef tgammaf
#undef tgammal
#undef trunc
#undef truncf
#undef truncl


#ifdef __BIONIC__
	using float_t = float;
	using double_t = double;
#else
	using ::double_t;
	using ::float_t;
#endif

// TODO: Deferred. How to detect if 'long double' available?
#	define YB_LibDefect_CMath_using_f(_fn) \
	using ::_fn; \
	using ::_fn##f;
#	define YB_LibDefect_CMath_using_fl(_fn) \
	YB_LibDefect_CMath_using_f(_fn) \
	using ::_fn##l;

#ifndef __BIONIC__
#	define YB_LibDefect_CMath_using(_fn) \
	YB_LibDefect_CMath_using_fl(_fn)
#else
#	define YB_LibDefect_CMath_using(_fn) \
	YB_LibDefect_CMath_using_f(_fn)
#endif

	YB_LibDefect_CMath_using(acos)
	YB_LibDefect_CMath_using(asin)
	YB_LibDefect_CMath_using(atan)
	YB_LibDefect_CMath_using(cbrt)
	YB_LibDefect_CMath_using(copysign)
	YB_LibDefect_CMath_using(erf)
	YB_LibDefect_CMath_using(erfc)
	YB_LibDefect_CMath_using(exp2)
	YB_LibDefect_CMath_using(expm1)
	YB_LibDefect_CMath_using(fdim)
	YB_LibDefect_CMath_using(fma)
	YB_LibDefect_CMath_using(fmax)
	YB_LibDefect_CMath_using(fmin)
	YB_LibDefect_CMath_using(hypot)
	YB_LibDefect_CMath_using(ilogb)
	YB_LibDefect_CMath_using(lgamma)
	YB_LibDefect_CMath_using(llrint)
	YB_LibDefect_CMath_using(llround)
	YB_LibDefect_CMath_using(log1p)
#ifndef __BIONIC__
#	ifndef _NEWLIB_VERSION
	YB_LibDefect_CMath_using(log2)
#	else
	YB_LibDefect_CMath_using_f(log2)
#	endif
#endif
#ifndef _NEWLIB_VERSION
	YB_LibDefect_CMath_using_f(logb)
#else
	YB_LibDefect_CMath_using(lrint)
#endif
	YB_LibDefect_CMath_using(lround)
#ifndef __BIONIC__
	YB_LibDefect_CMath_using(nan)
#endif
	YB_LibDefect_CMath_using(nearbyint)
	YB_LibDefect_CMath_using(nextafter)
#ifndef _NEWLIB_VERSION
	YB_LibDefect_CMath_using(nexttoward)
#endif
	YB_LibDefect_CMath_using(remainder)
	YB_LibDefect_CMath_using(remquo)
	YB_LibDefect_CMath_using(rint)
	YB_LibDefect_CMath_using(round)
	YB_LibDefect_CMath_using(scalbln)
	YB_LibDefect_CMath_using(scalbn)
#ifndef __BIONIC__
	YB_LibDefect_CMath_using(tgamma)
#else
	using ::tgamma;
#endif
	YB_LibDefect_CMath_using(trunc)

#undef YB_LibDefect_CMath_using
#undef YB_LibDefect_CMath_using_fl
#undef YB_LibDefect_CMath_using_f

#define YB_LibDefect_CMath_builtin(_n, ...) \
	{ \
		return __builtin_##_n(__VA_ARGS__); \
	}
#define YB_LibDefect_CMath_fn(_t, _fn, _n) \
	constexpr _t \
	_fn(_t __x) \
	YB_LibDefect_CMath_builtin(_n, __x)
#define YB_LibDefect_CMath_fn2(_t, _fn, _n) \
	constexpr _t \
	_fn(_t __x, _t __y) \
	YB_LibDefect_CMath_builtin(_n, __x, __y)
#define YB_LibDefect_CMath_fns(_fn) \
	YB_LibDefect_CMath_fn(float, _fn, _fn##f) \
	YB_LibDefect_CMath_fn(long double, _fn, _fn##l) \
	template<typename _Tp> \
	constexpr typename \
		__gnu_cxx::__enable_if<__is_integer<_Tp>::__value, double>::__type \
	_fn(_Tp __x) \
	YB_LibDefect_CMath_builtin(_fn, __x)
#define YB_LibDefect_CMath_fns2(_fn) \
	YB_LibDefect_CMath_fn2(float, _fn, _fn##f) \
	YB_LibDefect_CMath_fn2(long double, _fn, _fn##l) \
	template<typename _Tp, typename _Up> \
	constexpr typename __gnu_cxx::__promote_2<_Tp, _Up>::__type \
	_fn(_Tp __x, _Up __y) \
	{ \
		using __type = typename __gnu_cxx::__promote_2<_Tp, _Up>::__type; \
		\
		return _fn(__type(__x), __type(__y)); \
	}

#define YB_LibDefect_CMath_trigo(_n) \
	YB_LibDefect_CMath_fns(a##_n##h)

	YB_LibDefect_CMath_trigo(cos)

	YB_LibDefect_CMath_trigo(sin)

	YB_LibDefect_CMath_trigo(tan)

#undef YB_LibDefect_CMath_trigo

	YB_LibDefect_CMath_fns(cbrt)

	YB_LibDefect_CMath_fns2(copysign)

	YB_LibDefect_CMath_fns(erf)

	YB_LibDefect_CMath_fns(erfc)

	YB_LibDefect_CMath_fns(exp2)

	YB_LibDefect_CMath_fns(expm1)

	YB_LibDefect_CMath_fns2(fdim)

	constexpr float
	fma(float __x, float __y, float __z)
	YB_LibDefect_CMath_builtin(fmaf, __x, __y, __z)
	constexpr long double
	fma(long double __x, long double __y, long double __z)
	YB_LibDefect_CMath_builtin(fmal, __x, __y, __z)
	template<typename _Tp, typename _Up, typename _Vp>
	constexpr typename __gnu_cxx::__promote_3<_Tp, _Up, _Vp>::__type
	fma(_Tp __x, _Up __y, _Vp __z)
	{
		using __type = typename __gnu_cxx::__promote_3<_Tp, _Up, _Vp>::__type;

		return fma(__type(__x), __type(__y), __type(__z));
	}

	YB_LibDefect_CMath_fns2(fmax)

	YB_LibDefect_CMath_fns2(fmin)

	YB_LibDefect_CMath_fns2(hypot)

	YB_LibDefect_CMath_fns(ilogb)

	YB_LibDefect_CMath_fns(lgamma)

	YB_LibDefect_CMath_fns(llrint)

	YB_LibDefect_CMath_fns(llround)

	YB_LibDefect_CMath_fns(log1p)

	// DR 568.
	YB_LibDefect_CMath_fns(log2)

	YB_LibDefect_CMath_fns(logb)

	YB_LibDefect_CMath_fns(lrint)

	YB_LibDefect_CMath_fns(lround)

	YB_LibDefect_CMath_fns(nearbyint)

	YB_LibDefect_CMath_fns2(nextafter)

	constexpr float
	nexttoward(float __x, long double __y)
	YB_LibDefect_CMath_builtin(nexttowardf, __x, __y)
	constexpr long double
	nexttoward(long double __x, long double __y)
	YB_LibDefect_CMath_builtin(nexttowardl, __x, __y)
	template<typename _Tp>
	constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
		double>::__type
	nexttoward(_Tp __x, long double __y)
	YB_LibDefect_CMath_builtin(nexttoward, __x, __y)

	YB_LibDefect_CMath_fns2(remainder)

	inline float
	remquo(float __x, float __y, int* __pquo)
	YB_LibDefect_CMath_builtin(remquof, __x, __y, __pquo)
	inline long double
	remquo(long double __x, long double __y, int* __pquo)
	YB_LibDefect_CMath_builtin(remquol, __x, __y, __pquo)
	template<typename _Tp, typename _Up>
	inline typename __gnu_cxx::__promote_2<_Tp, _Up>::__type
	remquo(_Tp __x, _Up __y, int* __pquo)
	{
		using __type = typename __gnu_cxx::__promote_2<_Tp, _Up>::__type;

		return remquo(__type(__x), __type(__y), __pquo);
	}

	YB_LibDefect_CMath_fns(rint)

	YB_LibDefect_CMath_fns(round)

	constexpr float
	scalbln(float __x, long __ex)
	YB_LibDefect_CMath_builtin(scalblnf, __x, __ex)
	constexpr long double
	scalbln(long double __x, long __ex)
	YB_LibDefect_CMath_builtin(scalblnl, __x, __ex)
	template<typename _Tp>
	constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
		double>::__type
	scalbln(_Tp __x, long __ex)
	YB_LibDefect_CMath_builtin(scalbln, __x, __ex)

	constexpr float
	scalbn(float __x, int __ex)
	YB_LibDefect_CMath_builtin(scalbnf, __x, __ex)
	constexpr long double
	scalbn(long double __x, int __ex)
	YB_LibDefect_CMath_builtin(scalbnl, __x, __ex)
	template<typename _Tp>
	constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
		double>::__type
	scalbn(_Tp __x, int __ex)
	YB_LibDefect_CMath_builtin(scalbn, __x, __ex)

	YB_LibDefect_CMath_fns(tgamma)

	YB_LibDefect_CMath_fns(trunc)

#undef YB_LibDefect_CMath_fns
#undef YB_LibDefect_CMath_fn2
#undef YB_LibDefect_CMath_fn
#undef YB_LibDefect_CMath_builtin

} // namespace std;

#endif

#endif

