/*
	© 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLAMath.cpp
\ingroup NPL
\brief NPLA 数学功能。
\version r27437
\author FrankHB <frankhb1989@gmail.com>
\since build 930
\par 创建时间:
	2021-11-03 12:50:49 +0800
\par 修改时间:
	2021-12-21 02:25 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLAMath
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLAMath // for ValueObject, ResolvedArg, size_t, type_info,
//	TermNode, ystdex::identity, ystdex::conditional_t, ystdex::cond_t,
//	std::is_signed, ystdex::_t, NPL::TryAccessValue, std::string, TypeError,
//	ystdex::sfmt, std::isfinite, std::nearbyint, ystdex::exclude_self_t,
//	ystdex::enable_if_t, std::is_floating_point, std::fmod, std::to_string,
//	ystdex::and_, std::is_unsigned, std::abs, InvalidSyntax, ptrdiff_t,
//	std::ldexp, std::pow, string_view, ReductionStatus, std::isinf, std::isnan,
//	std::floor, std::log2, ystdex::is_explicitly_constructible, ystdex::byte;
#include <ystdex/exception.h> // for ystdex::unsupported;
#include <ystdex/cstdint.hpp> // for std::numeric_limits,
//	ystdex::make_widen_int;
#include <ystdex/functional.hpp> // for ystdex::retry_on_cond, ystdex::equal_to,
//	ystdex::less, ystdex::greater, ystdex::less_equal, ystdex::greater_equal;
// XXX: The type 'long double' may be also IEC 60559 binary64 format, e.g. in
//	Microsoft VC++.
#include <cfloat> // for FLT_*, DBL_*, LDBL_*;
#if DBL_MANT_DIG == LDBL_MANT_DIG && DBL_MIN_EXP == LDBL_MIN_EXP \
	&& DBL_MAX_EXP == LDBL_MAX_EXP && DBL_MIN_10_EXP == LDBL_MIN_10_EXP \
	&& DBL_MAX_10_EXP == LDBL_MAX_10_EXP
#	define NPL_Impl_NPLAMath_LongDoubleAsDouble true
#else
#	define NPL_Impl_NPLAMath_LongDoubleAsDouble false
#endif
// NOTE: Detecting the 128-bit unsigned type.
// NOTE: The availablity is target-dependent, see
//	https://gcc.gnu.org/onlinedocs/gcc/_005f_005fint128.html#g_t_005f_005fint128,
//	In particular, 32-bit GCC usually has no such support, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60846.
//	Clang usually has a policy to be binary compatible to GCC (e.g.
//	https://reviews.llvm.org/D43105?id=133546), so it is also limited here.
// XXX: There are also legacy %__uint128_t type supported by older versions of
//	compilers, but no one is in the supported platforms. So they are ignored.
#ifndef NPL_Impl_NPLAMath_has_uint128
// XXX: Since the support is specific to targets, the detection via additional
//	macro %__SIZEOF_INT128__ is needed. However, it is still not precise, e.g.
//	Clang++ may support 128-bit integer types (since 3.1) without such macro
//	definition (before 3.3). Just not bother it as it is too old to support
//	here. See also https://stackoverflow.com/a/54815033 for more details about
//	detecting the support. However, it may even broken on some other platforms
//	not formally supported by YFramework, e.g. NVCC does not support it even
//	when the macro is defined (see https://svn.boost.org/trac10/ticket/10418).
// TODO: Support %__INTEL_COMPILER?
// TODO: Exclude %__CUDACC__?
#	if __SIZEOF_INT128__ == 16 && YB_IMPL_GNUC
#		define NPL_Impl_NPLAMath_has_uint128 true
#	else
#		define NPL_Impl_NPLAMath_has_uint128 false
#	endif
#endif
// NOTE: See https://gcc.gnu.org/onlinedocs/gcc/Floating-Types.html.
// NOTE: Use <quadmath.h> requires the linker option '-lquadmath'.
// XXX: This does not requires '-std=gnu++11' or '-fext-numeric-literals' since
//	the nonconforming features are not relied on. See also
//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=87274.
#ifndef NPL_Impl_NPLAMath_UseQuadMath
// XXX: This is still not precise, e.g. Cray C seems has no support. NVCC also
//	does not support it (see https://svn.boost.org/trac10/ticket/11852).
// TODO: Exclude %__CRAYC and __CUDACC__?
#	if __has_include(<quadmath.h>)
// XXX: This is supported since GCC 4.6. Before GCC 4.8, it also requires
//	'extern "C"'. Anyway the old compilers are not supported here, so no more
//	inclusion conditions to detect. See https://stackoverflow.com/a/13781711.
#		include <quadmath.h> // for ::floorq, ::scalbnq, ::finiteq, ::powq,
//	::log2q;
#		ifdef QUADMATH_H
#			define NPL_Impl_NPLAMath_UseQuadMath true
#		else
#			define NPL_Impl_NPLAMath_UseQuadMath false
#		endif
#	else
#		define NPL_Impl_NPLAMath_UseQuadMath false
#	endif
#elif NPL_Impl_NPLAMath_UseQuadMath
#	include <quadmath.h>
#	ifndef QUADMATH_H
#		error "No compatible header found for NPL_Impl_NPLAMath_UseQuadMath."
#	endif
#endif
#include <ystdex/operators.hpp> // for ystdex::operators, ystdex::shiftable;
#include <ystdex/bit.hpp> // for ystdex::countr_zero_narrow;
#if YB_IMPL_MSCPP >= 1400
#	include <intrin.h>
#	if defined(_M_AMD64)
#		define NPL_Impl_NPLAMath_has_umul128 true
#		pragma intrinsic(_umul128)
#	endif
#endif
#include <ystdex/algorithm.hpp> // for ystdex::trivially_fill_n;

namespace NPL
{

inline namespace Math
{

//! \since build 929
namespace
{

YB_ATTR_nodiscard ValueObject
MoveUnary(ResolvedArg<>& x)
{
	ValueObject res;

	if(x.IsMovable())
		res = std::move(x.get().Value);
	else
		res = x.get().Value;
	return res;
}


YB_NORETURN void
ThrowForUnsupportedNumber()
{
	throw ystdex::unsupported("Invalid numeric type found.");
}


enum NumCode : size_t
{
	SChar = 0,
	UChar,
	Short,
	UShort,
	Int,
	UInt,
	Long,
	ULong,
	LongLong,
	ULongLong,
	IntMax = ULongLong,
	// TODO: Use bigint.
	Float,
	Double,
	LongDouble,
	Max = LongDouble,
	None = size_t(-1)
};

//! \relates NumCode
//@{
YB_ATTR_nodiscard YB_PURE NumCode
MapTypeIdToNumCode(const type_info& ti) ynothrow
{
	if(IsTyped<int>(ti))
		return Int;
	if(IsTyped<unsigned>(ti))
		return UInt;
	if(IsTyped<long long>(ti))
		return LongLong;
	if(IsTyped<unsigned long long>(ti))
		return ULongLong;
	if(IsTyped<double>(ti))
		return Double;
	// TODO: Use bigint.
	if(IsTyped<long>(ti))
		return Long;
	if(IsTyped<unsigned long>(ti))
		return ULong;
	if(IsTyped<short>(ti))
		return Short;
	if(IsTyped<unsigned short>(ti))
		return UShort;
	if(IsTyped<signed char>(ti))
		return SChar;
	if(IsTyped<unsigned char>(ti))
		return UChar;
	if(IsTyped<float>(ti))
		return Float;
	if(IsTyped<long double>(ti))
		return LongDouble;
	return None;
}
YB_ATTR_nodiscard YB_PURE inline
	PDefH(NumCode, MapTypeIdToNumCode, const ValueObject& vo) ynothrow
	ImplRet(MapTypeIdToNumCode(vo.type()))
YB_ATTR_nodiscard YB_PURE inline
	PDefH(NumCode, MapTypeIdToNumCode, const TermNode& nd) ynothrow
	ImplRet(MapTypeIdToNumCode(nd.Value))
YB_ATTR_nodiscard YB_PURE inline
	PDefH(NumCode, MapTypeIdToNumCode, const ResolvedArg<>& arg) ynothrow
	ImplRet(MapTypeIdToNumCode(arg.get()))
//@}

/*!
\ingroup transformation_traits
\pre 参数是非 cv 限定的精确数表示类型。
*/
//@{
template<typename _type>
struct ExtType : ystdex::identity<ystdex::conditional_t<ystdex::integer_width<
	decltype(std::declval<_type>() + 1)>::value == ystdex::integer_width<
	_type>::value, long long, int>>
{};

template<>
struct ExtType<int> : ystdex::identity<long long>
{};

template<>
struct ExtType<long> : ystdex::identity<long long>
{};

template<>
struct ExtType<long long> : ystdex::identity<unsigned long long>
{};

// TODO: Use bigint.
template<>
struct ExtType<unsigned long long> : ystdex::identity<double>
{};

template<typename _type>
struct NExtType : ystdex::cond_t<std::is_signed<_type>, ExtType<_type>,
	ystdex::identity<ystdex::conditional_t<(ystdex::integer_width<_type>::value
	<= ystdex::integer_width<int>::value), int, long long>>>
{};

// TODO: Use bigint.
template<>
struct NExtType<long long> : ystdex::identity<double>
{};


// TODO: Use bigint.
template<typename _type>
struct MulExtType : ystdex::conditional_t<ystdex::integer_width<_type>::value
	== 64, ystdex::identity<double>, ystdex::make_widen_int<_type>>
{};


template<typename _type>
using MakeExtType = ystdex::_t<ExtType<_type>>;

template<typename _type>
using MakeNExtType = ystdex::_t<NExtType<_type>>;

template<typename _type>
using MakeMulExtType = ystdex::_t<MulExtType<_type>>;
//@}


//! \since build 932
template<typename _type, typename _func, class _tValue>
YB_ATTR_nodiscard _type
DoNumLeaf(_tValue& x, _func f)
{
	if(const auto p_i = NPL::TryAccessValue<int>(x))
		return f(*p_i);
	if(const auto p_u = NPL::TryAccessValue<unsigned>(x))
		return f(*p_u);
	if(const auto p_ll = NPL::TryAccessValue<long long>(x))
		return f(*p_ll);
	if(const auto p_ull = NPL::TryAccessValue<unsigned long long>(x))
		return f(*p_ull);
	if(const auto p_d = NPL::TryAccessValue<double>(x))
		return f(*p_d);
	// TODO: Use bigint.
	if(const auto p_l = NPL::TryAccessValue<long>(x))
		return f(*p_l);
	if(const auto p_ul = NPL::TryAccessValue<unsigned long>(x))
		return f(*p_ul);
	if(const auto p_s = NPL::TryAccessValue<short>(x))
		return f(*p_s);
	if(const auto p_us = NPL::TryAccessValue<unsigned short>(x))
		return f(*p_us);
	if(const auto p_sc = NPL::TryAccessValue<signed char>(x))
		return f(*p_sc);
	if(const auto p_uc = NPL::TryAccessValue<unsigned char>(x))
		return f(*p_uc);
	if(const auto p_f = NPL::TryAccessValue<float>(x))
		return f(*p_f);
	if(const auto p_ld = NPL::TryAccessValue<long double>(x))
		return f(*p_ld);
	return f(x);
}

//! \since build 932
template<typename _type, typename _func, class... _tValue>
YB_ATTR_nodiscard _type
DoNumLeafHinted(NumCode code, _func f, _tValue&... xs)
{
	switch(code)
	{
	case Int:
		return f(xs.template GetObject<int>()...);
	case UInt:
		return f(xs.template GetObject<unsigned>()...);
	case LongLong:
		return f(xs.template GetObject<long long>()...);
	case ULongLong:
		return f(xs.template GetObject<unsigned long long>()...);
	case Double:
		return f(xs.template GetObject<double>()...);
	// TODO: Use bigint.
	case Long:
		return f(xs.template GetObject<long>()...);
	case ULong:
		return f(xs.template GetObject<unsigned long>()...);
	case Short:
		return f(xs.template GetObject<short>()...);
	case UShort:
		return f(xs.template GetObject<unsigned short>()...);
	case SChar:
		return f(xs.template GetObject<signed char>()...);
	case UChar:
		return f(xs.template GetObject<unsigned char>()...);
	case Float:
		return f(xs.template GetObject<float>()...);
	case LongDouble:
		return f(xs.template GetObject<long double>()...);
	default:
		return f(xs...);
	}
}


//! \since build 930
//@{
YB_NORETURN void
AssertMismatch() ynothrowv
{
	YAssert(false, "Invalid number type found for number leaf conversion.");
	YB_ASSUME(false);
}


YB_NORETURN void
ThrowTypeErrorForInteger(const std::string& val)
{
	throw TypeError(ystdex::sfmt(
		"Expected a value of type 'integer', got '%s'.", val.c_str()));
}

//! \since build 932
template<typename _type>
YB_ATTR_nodiscard YB_PURE bool
FloatIsInteger(const _type& x) ynothrow
{
#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Push
	YB_Diag_Ignore(float-equal)
#endif
	// NOTE: See $2021-11 @ %Documentation::Workflow.
	return std::isfinite(x) && std::nearbyint(x) == x;
#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Pop
#endif
}
//@}


//! \ingroup functors
//@{
//! \since build 930
template<typename _type = void>
struct GUAssertMismatch
{
	YB_NORETURN _type
	operator()(const ValueObject&) const ynothrowv
	{
		AssertMismatch();
	}
};


template<typename _type = void>
struct ReportMismatch
{
	YB_NORETURN _type
	operator()(const ValueObject&) const
	{
		ThrowForUnsupportedNumber();
	}
};


template<typename _tDst>
struct Cast : GUAssertMismatch<_tDst>
{
	//! \since build 930
	using GUAssertMismatch<_tDst>::operator();
	template<typename _tSrc,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _tSrc>)>
	YB_ATTR_nodiscard YB_PURE yconstfn _tDst
	operator()(const _tSrc& x) const ynothrow
	{
		return _tDst(x);
	}
};


struct DynNumCast : ReportMismatch<ValueObject>
{
	NumCode Code;

	//! \since build 932
	DynNumCast(NumCode code) ynothrow
		: Code(code)
	{}

	using ReportMismatch<ValueObject>::operator();
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>)>
	inline ValueObject
	operator()(const _type& x) const
	{
		// TODO: Use allocator?
		switch(Code)
		{
		case Int:
			return static_cast<int>(x);
		case UInt:
			return static_cast<unsigned>(x);
		case LongLong:
			return static_cast<long long>(x);
		case ULongLong:
			return static_cast<unsigned long long>(x);
		case Double:
			return static_cast<double>(x);
		// TODO: Use bigint.
		case Long:
			return static_cast<long>(x);
		case ULong:
			return static_cast<unsigned long>(x);
		case Short:
			return static_cast<short>(x);
		case UShort:
			return static_cast<unsigned short>(x);
		case SChar:
			return static_cast<signed char>(x);
		case UChar:
			return static_cast<unsigned char>(x);
		case Float:
			return static_cast<float>(x);
		case LongDouble:
			return static_cast<long double>(x);
		default:
			ThrowForUnsupportedNumber();
		}
	}
};


// XXX: Assume the integers have representations of 2's complement by default.
//	This implies the nagative of the minimum values can overflow. Otherwise,
//	there can be redundant underflow checks, but still correct. Note since ISO
//	C++20, only 2's complement is supported.
// XXX: In the following computations, signaling NaNs are assumed not existing
//	in valid inputs. Ignoring the checks should be safe even with
//	%FLT_EVAL_METHOD and quite NaNs taken into account.
// XXX: Operations on flonums are always closed even on overflow.


struct EqZero : GUAssertMismatch<bool>
{
	//! \since build 930
	using GUAssertMismatch<bool>::operator();
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>)>
	YB_ATTR_nodiscard YB_PURE yconstfn bool
	operator()(const _type& x) const ynothrow
	{
#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Push
	YB_Diag_Ignore(float-equal)
#endif
		return x == _type(0);
#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Pop
#endif
	}
};


//! \since build 930
//@{
struct Positive : GUAssertMismatch<bool>
{
	using GUAssertMismatch<bool>::operator();
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>)>
	YB_ATTR_nodiscard YB_PURE yconstfn bool
	operator()(const _type& x) const ynothrow
	{
		// XXX: Ditto.
		return x > _type(0);
	}
};


struct Negative : GUAssertMismatch<bool>
{
	using GUAssertMismatch<bool>::operator();
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>)>
	YB_ATTR_nodiscard YB_PURE yconstfn bool
	operator()(const _type& x) const ynothrow
	{
		return x < _type(0);
	}
};


struct Odd : GUAssertMismatch<bool>
{
	using GUAssertMismatch<bool>::operator();
	template<typename _type>
	inline yimpl(ystdex::exclude_self_t<ValueObject, _type, bool>)
	operator()(const _type& x) const
	{
		return Do(x);
	}

private:
	template<typename _type>
	static inline
		ystdex::enable_if_t<std::is_floating_point<_type>::value, bool>
	Do(const _type& x)
	{
#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Push
	YB_Diag_Ignore(float-equal)
#endif
		if(FloatIsInteger(x))
			return std::fmod(x, _type(2)) == _type(1);
#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Pop
#endif
		ThrowTypeErrorForInteger(std::to_string(x));
	}
	template<typename _type, yimpl(ystdex::enable_if_t<
		!std::is_floating_point<_type>::value, int> = 0)>
	static inline bool
	Do(const _type& x) ynothrow
	{
		return x % _type(2) != _type(0);
	}
};


struct Even : GUAssertMismatch<bool>
{
	using GUAssertMismatch<bool>::operator();
	template<typename _type>
	inline yimpl(ystdex::exclude_self_t<ValueObject, _type, bool>)
	operator()(const _type& x) const
	{
		return Do(x);
	}

private:
	template<typename _type>
	static inline
		ystdex::enable_if_t<std::is_floating_point<_type>::value, bool>
	Do(const _type& x)
	{
#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Push
	YB_Diag_Ignore(float-equal)
#endif
		if(FloatIsInteger(x))
			return std::fmod(x, _type(2)) == _type(0);
#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Pop
#endif
		ThrowTypeErrorForInteger(std::to_string(x));
	}
	template<typename _type, yimpl(ystdex::enable_if_t<
		!std::is_floating_point<_type>::value, int> = 0)>
	static inline bool
	Do(const _type& x) ynothrow
	{
		return x % _type(2) == _type(0);
	}
};


template<typename _type = ValueObject>
struct GBAssertMismatch
{
	YB_NORETURN _type
	operator()(const ValueObject&, const ValueObject&) const ynothrowv
	{
		AssertMismatch();
	}
};


template<class _tBase>
struct GBCompare : GBAssertMismatch<bool>, _tBase
{
	using GBAssertMismatch<bool>::operator();
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>)>
	inline bool
	operator()(const _type& x, const _type& y) const ynothrow
	{
		return _tBase::operator()(x, y);
	}
};


struct BMax : GBAssertMismatch<>
{
	using GBAssertMismatch<>::operator();
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>)>
	inline ValueObject
	operator()(const _type& x, const _type& y) const ynothrow
	{
		// TODO: Use allocator for bignum?
		return x > y ? x : y;
	}
};


struct BMin : GBAssertMismatch<>
{
	using GBAssertMismatch<>::operator();
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>)>
	inline ValueObject
	operator()(const _type& x, const _type& y) const ynothrow
	{
		// TODO: Use allocator for bignum?
		return x < y ? x : y;
	}
};
//@}


struct AddOne : GUAssertMismatch<>
{
	ValueObject& Result;

	AddOne(ValueObject& res)
		: Result(res)
	{}

	//! \since build 930
	using GUAssertMismatch<>::operator();
	template<typename _type>
	inline yimpl(ystdex::exclude_self_t<ValueObject, _type>)
	operator()(_type& x) const ynothrow
	{
		Perform(x);
	}

private:
	//! \since build 930
	template<typename _type>
	inline ystdex::enable_if_t<std::is_floating_point<_type>::value>
	Perform(_type& x) const ynothrow
	{
		x += _type(1);
	}
	//! \since build 930
	template<typename _type, yimpl(ystdex::enable_if_t<
		!std::is_floating_point<_type>::value, int> = 0)>
	inline void
	Perform(_type& x) const
	{
		if(x != std::numeric_limits<_type>::max())
			++x;
		else
			// TODO: Allocator?
			Result = ValueObject(MakeExtType<_type>(x) + 1);
	}
};

struct SubOne : GUAssertMismatch<>
{
	ValueObject& Result;

	SubOne(ValueObject& res)
		: Result(res)
	{}

	//! \since build 930
	using GUAssertMismatch<>::operator();
	template<typename _type>
	inline yimpl(ystdex::exclude_self_t<ValueObject, _type>)
	operator()(_type& x) const ynothrow
	{
		Perform(x);
	}

private:
	//! \since build 930
	template<typename _type>
	inline ystdex::enable_if_t<std::is_floating_point<_type>::value>
	Perform(_type& x) const ynothrow
	{
		x -= _type(1);
	}
	//! \since build 930
	template<typename _type, yimpl(ystdex::enable_if_t<
		!std::is_floating_point<_type>::value, int> = 0)>
	inline void
	Perform(_type& x) const
	{
		if(x != std::numeric_limits<_type>::min())
			--x;
		else
			// TODO: Allocator?
			Result = ValueObject(MakeNExtType<_type>(x) - 1);
	}
};


struct BPlus : GBAssertMismatch<>
{
	//! \since build 930
	using GBAssertMismatch<>::operator();
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>)>
	inline ValueObject
	operator()(const _type& x, const _type& y) const ynothrow
	{
		return Do(x, y);
	}

private:
	template<typename _type>
	static inline
		ystdex::enable_if_t<!std::is_integral<_type>::value, _type>
	Do(const _type& x, const _type& y) ynothrow
	{
		// TODO: Use bigint with allocator?
		return x + y;
	}
	template<typename _type, yimpl(ystdex::enable_if_t<
		ystdex::and_<std::is_integral<_type>,
		std::is_signed<_type>>::value, int> = 0)>
	static inline ValueObject
	Do(const _type& x, const _type& y)
	{
#if __has_builtin(__builtin_add_overflow)
		_type r;

		if(!__builtin_add_overflow(x, y, &r))
			return r;
		if(x > 0 && y > std::numeric_limits<_type>::max() - x)
			return MakeExtType<_type>(x) + MakeExtType<_type>(y);
		return MakeNExtType<_type>(x) + MakeNExtType<_type>(y);
#else
		if(x > 0 && y > std::numeric_limits<_type>::max() - x)
			return MakeExtType<_type>(x) + MakeExtType<_type>(y);
		if(x < 0 && y < std::numeric_limits<_type>::min() - x)
			return MakeNExtType<_type>(x) + MakeNExtType<_type>(y);
		return x + y;
#endif
	}
	template<typename _type, yimpl(ystdex::enable_if_t<
		std::is_unsigned<_type>::value, long> = 0L)>
	static inline ValueObject
	Do(const _type& x, const _type& y)
	{
#if __has_builtin(__builtin_add_overflow)
		_type r;

		if(!__builtin_add_overflow(x, y, &r))
			return r;
		// NOTE: Wrapped integers are more efficient.
#elif true
		const _type r(x + y);

		if(r >= x)
			return r;
#else
		if(y <= std::numeric_limits<_type>::max() - x)
			return x + y;
#endif
		return MakeExtType<_type>(x) + MakeExtType<_type>(y);
	}
};


struct BMinus : GBAssertMismatch<>
{
	//! \since build 930
	using GBAssertMismatch<>::operator();
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>)>
	inline ValueObject
	operator()(const _type& x, const _type& y) const ynothrow
	{
		return Do(x, y);
	}

private:
	template<typename _type>
	static inline
		ystdex::enable_if_t<!std::is_integral<_type>::value, _type>
	Do(const _type& x, const _type& y) ynothrow
	{
		// TODO: Use bigint with allocator?
		return x - y;
	}
	template<typename _type, yimpl(ystdex::enable_if_t<
		ystdex::and_<std::is_integral<_type>,
		std::is_signed<_type>>::value, int> = 0)>
	static inline ValueObject
	Do(const _type& x, const _type& y)
	{
#if __has_builtin(__builtin_sub_overflow)
		_type r;

		if(!__builtin_sub_overflow(x, y, &r))
			return r;
		if(YB_UNLIKELY(y == std::numeric_limits<_type>::min())
			|| (x > 0 && -y > std::numeric_limits<_type>::max() - x))
			return MakeExtType<_type>(x) - MakeExtType<_type>(y);
		return MakeNExtType<_type>(x) - MakeNExtType<_type>(y);
#else
		if(YB_UNLIKELY(y == std::numeric_limits<_type>::min())
			|| (x > 0 && -y > std::numeric_limits<_type>::max() - x))
			return MakeExtType<_type>(x) - MakeExtType<_type>(y);
		if(x < 0 && -y < std::numeric_limits<_type>::min() - x)
			return MakeNExtType<_type>(x) - MakeNExtType<_type>(y);
		return x - y;
#endif
	}
	template<typename _type, yimpl(ystdex::enable_if_t<
		std::is_unsigned<_type>::value, long> = 0L)>
	static inline ValueObject
	Do(const _type& x, const _type& y)
	{
		// XXX: This is efficient enough to avoid builtins.
		if(y <= x)
			return x - y;
		// TODO: Use bigint with allocator?
		return MakeExtType<_type>(x) - MakeExtType<_type>(y);
	}
};


struct BMultiplies : GBAssertMismatch<>
{
	//! \since build 930
	using GBAssertMismatch<>::operator();
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>)>
	inline ValueObject
	operator()(const _type& x, const _type& y) const ynothrow
	{
		return Do(x, y);
	}

private:
	template<typename _type>
	static inline
		ystdex::enable_if_t<!std::is_integral<_type>::value, _type>
	Do(const _type& x, const _type& y) ynothrow
	{
		// TODO: Use bigint with allocator?
		return x * y;
	}
	template<typename _type, yimpl(ystdex::enable_if_t<
		std::is_integral<_type>::value, int> = 0)>
	static inline ValueObject
	Do(const _type& x, const _type& y)
	{
#if __has_builtin(__builtin_mul_overflow)
		_type r;

		if(!__builtin_mul_overflow(x, y, &r))
			return r;
		return MakeMulExtType<_type>(x) * MakeMulExtType<_type>(y);
#else
		using r_t = MakeMulExtType<_type>;
		const r_t r(r_t(x) * r_t(y));

		if(r <= r_t(std::numeric_limits<_type>::max()))
			return _type(r);
		return r;
#endif
	}
};


//! \since build 930
struct BDivides : GBAssertMismatch<>
{
	using GBAssertMismatch<>::operator();
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>)>
	inline ValueObject
	operator()(const _type& x, const _type& y) const
	{
		return Do(x, y);
	}

private:
	template<typename _type>
	static inline
		ystdex::enable_if_t<!std::is_integral<_type>::value, _type>
	Do(const _type& x, const _type& y) ynothrow
	{
		// TODO: Use bigint with allocator?
		return x / y;
	}
	template<typename _type, yimpl(ystdex::enable_if_t<
		ystdex::and_<std::is_integral<_type>,
		std::is_signed<_type>>::value, int> = 0)>
	static inline ValueObject
	Do(const _type& x, const _type& y)
	{
		if(y != 0)
		{
			// NOTE: To avoid undefined behavior of when the value is in the 2's
			//	complement representation (required since ISO C++20).
			if(y != _type(-1) || x != std::numeric_limits<_type>::min())
				return DoInt(x, y);
			if(std::numeric_limits<_type>::min()
				== std::numeric_limits<long long>::min())
				return -double(x);
#if YB_IMPL_MSCPP >= 1200
	YB_Diag_Push
	YB_Diag_Ignore(4146)
	// NOTE: Same to the %ystdex::fixed_point::operator- implementation in
	//	%YStandardEx.Rational in YBase. A different reason to disable the
	//	warning instead of a workaround with prefix '0 - ' is theat such case is
	//	guaranteed in 2's complement representation, and not even occurs
	//	otherwise (plus all platforms supported by Microsoft VC++ imply it).
#endif
			return -MakeExtType<_type>(x);
#if YB_IMPL_MSCPP >= 1200
	YB_Diag_Pop
#endif
		}
		ThrowDivideByZero();
	}
	template<typename _type, yimpl(ystdex::enable_if_t<
		std::is_unsigned<_type>::value, long> = 0L)>
	static inline ValueObject
	Do(const _type& x, const _type& y)
	{
		if(y != 0)
			return DoInt(x, y);
		ThrowDivideByZero();
	}

	template<typename _type>
	static inline ValueObject
	DoInt(const _type& x, const _type& y) ynothrowv
	{
		static_assert(std::is_integral<_type>(), "Invalid type found.");

		YAssert(y != 0, "Invalid value found.");

		_type r(x / y);

		if(r * y == x)
			return r;
		return double(x) / double(y);
	}

	YB_NORETURN static ValueObject
	ThrowDivideByZero()
	{
		throw std::domain_error("Division by zero.");
	}
};


struct ReplaceAbs : GUAssertMismatch<>
{
	ValueObject& Result;

	ReplaceAbs(ValueObject& res)
		: Result(res)
	{}

	using GUAssertMismatch<>::operator();
	template<typename _type>
	inline yimpl(ystdex::exclude_self_t<ValueObject, _type>)
	operator()(_type& x) const
	{
		Perform(x);
	}

private:
	template<typename _type>
	inline ystdex::enable_if_t<!std::is_integral<_type>::value>
	Perform(_type& x) const
	{
		using std::abs;

		// XXX: Assume the type of the result is implicitly convertible to the
		//	original target type.
		x = abs(x);
	}
	template<typename _type, yimpl(ystdex::enable_if_t<
		ystdex::and_<std::is_integral<_type>,
		std::is_signed<_type>>::value, int> = 0)>
	inline void
	Perform(_type& x) const
	{
		// NOTE: To avoid undefined behavior of when the value is in the 2's
		//	complement representation (required since ISO C++20).
		if(x != std::numeric_limits<_type>::min())
			x = _type(std::abs(x));
		else
#if YB_IMPL_MSCPP >= 1200
	YB_Diag_Ignore(4146)
	// NOTE: See %BDivides::Do.
#endif
			Result = -MakeExtType<_type>(x);
#if YB_IMPL_MSCPP >= 1200
	YB_Diag_Push
	YB_Diag_Pop
#endif
	}
	template<typename _type, yimpl(ystdex::enable_if_t<
		std::is_unsigned<_type>::value, long> = 0L)>
	inline void
	Perform(_type&) const ynothrow
	{}
};
//@}


YB_ATTR_nodiscard YB_PURE ValueObject
Promote(NumCode code, const ValueObject& x, NumCode src_code)
{
	return DoNumLeafHinted<ValueObject>(src_code, DynNumCast(code), x);
}

//! \since build 930
template<class _fBinary>
bool
NumBinaryComp(const ValueObject& x, const ValueObject& y) ynothrow
{
	const auto xcode(MapTypeIdToNumCode(x));
	const auto ycode(MapTypeIdToNumCode(y));
	const auto ret_bin([](ValueObject u, ValueObject v, NumCode code){
		return DoNumLeafHinted<bool>(code, _fBinary(), u, v);
	});

	return size_t(xcode) >= size_t(ycode) ? ret_bin(x, Promote(xcode, y, ycode),
		xcode) : ret_bin(Promote(ycode, x, xcode), y, ycode);
}

template<class _fBinary>
ValueObject
NumBinaryOp(ResolvedArg<>& x, ResolvedArg<>& y)
{
	const auto xcode(MapTypeIdToNumCode(x));
	const auto ycode(MapTypeIdToNumCode(y));
	const auto ret_bin([](ValueObject u, ValueObject v, NumCode code){
		return DoNumLeafHinted<ValueObject>(code, _fBinary(), u, v);
	});

	return size_t(xcode) >= size_t(ycode) ?
		ret_bin(MoveUnary(x), Promote(xcode, y.get().Value, ycode), xcode)
		: ret_bin(Promote(ycode, x.get().Value, xcode), MoveUnary(y), ycode);
}


//! \since build 930
//@{
YB_NORETURN YB_NONNULL(1, 2) void
ThrowForInvalidLiteralSuffix(const char* sfx, const char* id)
{
	// TODO: Use %ThrowInvalidSyntaxError lift from %A1?
	throw InvalidSyntax(ystdex::sfmt(
		"Literal suffix '%s' is unsupported in identifier '%s'.", sfx, id));
}

template<typename _type>
YB_ATTR_nodiscard YB_STATELESS yconstfn _type
DecimalCarryAddDigit(_type x, char c)
{
	return x * 10 + _type(c - '0');
}

template<typename _tInt>
YB_ATTR_nodiscard YB_FLATTEN inline bool
DecimalAccumulate(_tInt& ans, char c)
{
	static yconstexpr const auto i_thr(std::numeric_limits<_tInt>::max() / 10);
	static yconstexpr const char
		c_thr(char('0' + size_t(std::numeric_limits<_tInt>::max() % 10)));

	if(ans < i_thr || YB_UNLIKELY(ans == i_thr && c <= c_thr))
	{
		ans = DecimalCarryAddDigit(ans, c);
		return true;
	}
	return {};
}
//@}

//! \since build 931
//@{
// XXX: Keeping the type of numeric literals without sign 'int' is also more
//	compatible to old operations.
using ReadIntType = int;
using ReadExtIntType = long long;
// XXX: %ReadCommonType is the type for inexact numbers parsing. Normally it is
//	assumed able to save more digits than floating-point type for all
//	non-negative finite values. This is at least true for IEEE-754 64-bit binary
//	format which is %double for most popluar implementations, but some other
//	values in formats like 80-bit extended floating-point numbers as
//	'long double' can sometimes fail. If the assumption is false, there may be
//	results with degraded quality, as expected. Anyway, precise reading is
//	already not required in the specification of the parsing of the inexact
//	numbers. This type usually needs explicit cast to eliminate warnings in
//	passing values (e.g. in signed types).
using ReadCommonType = ystdex::common_type_t<ReadExtIntType, std::uint64_t>;

YB_ATTR_nodiscard YB_STATELESS yconstfn PDefH(bool, IsDecimalPoint, char c)
	ynothrow
	ImplRet(c == '.')

YB_ATTR_nodiscard YB_STATELESS yconstfn PDefH(bool, IsExponent, char c) ynothrow
	ImplRet(c == 'e' || c == 'E' || c == 'd' || c == 'D' || c == 'f'
		|| c == 'F' || c == 'l' || c == 'L' || c == 's' || c == 'S')

// XXX: Some floating-point operations implemention may keep more precision as
//	expected in the internal represenatation leading to more imprecise results,
//	e.g. x87 FPU produces 4.3683000000000004e25 for 43683.0 * 1e21 instead of
//	4.3683e25.  See also https://arxiv.org/abs/cs/0701192 for a more detailed
//	explaination. Although here specific dependencies on concrete floating-point
//	formats are carefully prevented, better avoid on the excessive trailing
//	digits as possible.

//! \since build 932
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline _type
ScaleDecimalFlonum(char sign, _type ans, ptrdiff_t scale)
{
	// XXX: Cast explicitly to %_type to trucate for %std::pow early. This may
	//	loses more precision as expected. See the discussions of precision
	//	above. This also eliminates G++ warning: [-fdouble-conversion] in some
	//	instantiations.
	// XXX: Underflow shall be avoided to ensure the correctness when %ans and
	//	%scale are for a small subnormal value, e.g. IEEE-754 double 1.1e-323,
	//	which should not be directly multiplied with a underflow power value
	//	like 'std::pow(10., double(scale))'. Otherwise, the result would be the
	//	wrong 0.0.
	return std::ldexp(sign != '-' ? _type(ans) : -_type(ans), int(scale))
		* std::pow(_type(5), _type(scale));
}

//! \since build 932
void
SetDecimalFlonum(ValueObject& vo, char e, char sign, ReadCommonType ans,
	ptrdiff_t scale)
{
	switch(e)
	{
	case 'e':
	case 'E':
	case 'd':
	case 'D':
		vo = ScaleDecimalFlonum(sign, double(ans), scale);
		break;
	case 'f':
	case 'F':
	case 's':
	case 'S':
		vo = ScaleDecimalFlonum(sign, float(ans), scale);
		break;
	case 'l':
	case 'L':
		vo = ScaleDecimalFlonum(sign, static_cast<long double>(ans),
			scale);
		break;
	}
}

YB_ATTR_nodiscard YB_PURE ptrdiff_t
ReadDecimalExponent(string_view::const_iterator first, string_view id)
{
	bool minus = {};

	switch(*first)
	{
	case '-':
		minus = true;
		YB_ATTR_fallthrough;
	case '+':
		++first;
		YB_ATTR_fallthrough;
	default:
		break;
	}
	if(first != id.end())
	{
		ptrdiff_t res(0);

		ystdex::retry_on_cond(ystdex::id<>(), [&]() -> bool{
			if(ystdex::isdigit(*first))
			{
				if(DecimalAccumulate(res, *first))
					return ++first != id.end();
				// XXX: Assume the values (with or without the minus sign) are
				//	out of the range of the supported exponents.
				res = std::numeric_limits<ptrdiff_t>::max();
				return {};
			}
			throw InvalidSyntax(ystdex::sfmt("Invalid character '%c' found in"
				" the exponent of '%s'.", *first, id.data()));
		});
		return minus ? -res : res;
	}
	throw
		InvalidSyntax(ystdex::sfmt("Empty exponent found in '%s'.", id.data()));
}
//@}

//! \since build 931
void
ReadDecimalInexact(ValueObject& vo, string_view::const_iterator first,
	string_view id, ReadCommonType ans, string_view::const_iterator dpos)
{
	YAssert(!id.empty(), "Invalid lexeme found.");

	// NOTE: This is the beginning position known to ignore the subsequent
	//	digits after. Digits after this position are checked but then ignored.
	auto cut(id.end());
	ptrdiff_t scale;
	char e('e');

	ystdex::retry_on_cond(ystdex::id<>(), [&]() -> bool{
		if(ystdex::isdigit(*first))
		{
			if(cut == id.end())
			{
				// NOTE: The intermediate result cannot save more digits, do
				//	rounding.
				if(!DecimalAccumulate(ans, *first))
				{
					ans += *first >= '5' ? 1 : 0;
					cut = first;
				}
			}
		}
		else if(IsDecimalPoint(*first))
		{
			if(dpos == id.end())
				// NOTE: No exponent part has been found.
				dpos = first;
			else
				throw InvalidSyntax(ystdex::sfmt(
					"More than one decimal points found in '%s'.", id.data()));
		}
		else if(IsExponent(*first))
		{
			e = *first;
			// NOTE: If a decimal point in the representation of the cut digits
			//	is found, it is also shifted as one digit.
			scale = (cut == id.end() ? 0 : first - cut
				- (dpos != id.end() && dpos > cut ? 1 : 0))
				- (dpos == id.end() ? 0 : first - dpos - 1)
				+ ReadDecimalExponent(first + 1, id);
			return {};
		}
		else
			ThrowForInvalidLiteralSuffix(&*first, id.data());
		if(++first != id.end())
			return true;
		// NOTE: Similar to above without the exponent, as if the exponent
		//	letter is at the end.
		scale = (cut == id.end() ? 0 : id.end() - cut
			- (dpos != id.end() && dpos > cut ? 1 : 0))
			- (dpos == id.end() ? 0 : id.end() - dpos - 1);
		return {};
	});
	SetDecimalFlonum(vo, e, id[0], ans, scale);
}

//! \since build 931
template<typename _tInt>
YB_ATTR_nodiscard bool
ReadDecimalExact(ValueObject& vo, string_view id,
	string_view::const_iterator& first, _tInt& ans)
{
	YAssert(!id.empty(), "Invalid lexeme found.");
	return ystdex::retry_on_cond([&](ReductionStatus r) ynothrow -> bool{
		if(r == ReductionStatus::Retrying)
		{
			if(++first != id.end())
				return true;
			vo = id[0] != '-' ? ans : -ans;
		}
		return {};
	}, [&]() -> ReductionStatus{
		if(ystdex::isdigit(*first))
			return DecimalAccumulate(ans, *first) ? ReductionStatus::Retrying
				: ReductionStatus::Partial;
		if(IsDecimalPoint(*first))
		{
			ReadDecimalInexact(vo, first + 1, id, ReadCommonType(ans),
				first);
			return ReductionStatus::Clean;
		}
		if(IsExponent(*first))
		{
			SetDecimalFlonum(vo, *first, id[0], ReadCommonType(ans),
				ReadDecimalExponent(first + 1, id));
			return ReductionStatus::Clean;
		}
		ThrowForInvalidLiteralSuffix(&*first, id.data());
	}) == ReductionStatus::Partial;
}

//! \since build 932
//@{
// NOTE: IEC 60559 floating-point binary representation is preferred.
#ifndef NPL_Impl_NPLAMath_IEC_60559_BFP
#	if __STDC_IEC_60559_BFP__ >= yimpl(202000L) || __STDC_IEC_559__
#		define NPL_Impl_NPLAMath_IEC_60559_BFP true
#	elif FLT_IS_IEC_60559 >= 1 && DBL_IS_IEC_60559 >= 1
#		define NPL_Impl_NPLAMath_IEC_60559_BFP false
#	elif FLT_RADIX == 2 && DBL_MANT_DIG == 53 && DBL_MIN_EXP == -1021 \
		&& DBL_MAX_EXP == 1024 && DBL_MIN_10_EXP == -307 \
		&& DBL_MAX_10_EXP == 308
#		define NPL_Impl_NPLAMath_IEC_60559_BFP true
#	else
#		define NPL_Impl_NPLAMath_IEC_60559_BFP false
// XXX: This is still supported, albeit in very limited ways.
//#		error "IEC 60559 compatible floating-point types not detected."
#	endif
#endif
// NOTE: See https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html and
//	https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros.
#if _M_FP_FAST || __FAST_MATH__
// XXX: Actually, the only offensive one in GCC is -ffinite-math-only, which
//	inteferes the handling of infinities and NaNs. Nevertheless, there is no
//	separate test. There are similar concerns in other C++ implementations.
/*
Other GCC options enabled by -ffast-math are not interested here.
	-funsafe-math-optimizations:
		This is allowed by the loose precision rules.
		-fno-signed-zeros, -fno-trapping-math and -freciprocal-math:
			The results are unspecified respecting the possible behavior
				changes.
		-fassociative-math:
			This might affect the number of conversions to reach the final
				equivalence of round-trip conversions between number and
				strings. Nevertheless, it is a QoI issue.
	-fexcess-precision=fast:
		This is allowed by the loose precision rules, and it is unusable for
			language other than C or specific targets (e.g. -mfpmath=sse+387).
	-fno-math-errno:
		This implementation does not interact with %errno.
	-fcx-limited-range:
		Complex numbers are not used at current.
	-fno-rounding-math:
		No explicit rounding modes are relied on, and the default modes is
			sufficient.
	-fno-signaling-nans:
		SNaNs are documented to be not relied on.
Moreover, -ffloat-store is not required to enforce precision requirements.
*/
#	error "Nonconforming math options are not supported."
#endif
// NOTE: This requires ISO C++17. If not explicitly reported by the standard
//	library, treat it can have subnormal numbers.
// TODO: Detect subnormal numbers per type?
#if !defined(FLT_HAS_SUBNORM) || FLT_HAS_SUBNORM != 0 \
	|| !defined(DBL_HAS_SUBNORM) || DBL_HAS_SUBNORM != 0 \
	|| !defined(LDBL_HAS_SUBNORM) || LDBL_HAS_SUBNORM != 0
#	define NPL_Impl_NPLAMath_HasSubnorm true
#endif


#if NPL_Impl_NPLAMath_UseQuadMath
__extension__ using float128_t = __float128;
#endif

namespace yimpl(fpq)
{

using std::floor;
using std::ldexp;
using std::isfinite;
using std::pow;
using std::log2;

#if NPL_Impl_NPLAMath_UseQuadMath
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline float128_t
floor(float128_t x) ynothrow
{
	return ::floorq(x);
}

YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline float128_t
ldexp(float128_t x, int p) ynothrow
{
#if FLT_RADIX != 2
#	error "Wrong implementation found."
#else
	// XXX: Only finite values are used here and no %error no is required as in
	//	::ldexpq.
	return ::scalbnq(x, p);
#endif
}

YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline bool
isfinite(float128_t x) ynothrow
{
	return ::finiteq(x) != 0;
}

YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline float128_t
pow(float128_t x, float128_t y) ynothrow
{
	return ::powq(x, y);
}

YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline float128_t
log2(float128_t x) ynothrow
{
	return ::log2q(x);
}
#endif

} // namespace yimpl(fpq);


template<typename _type,
	yimpl(typename = ystdex::enable_if_convertible_t<_type, std::uint64_t>)>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yconstfn std::uint64_t
to_uint64(_type x) ynothrow
{
	return static_cast<std::uint64_t>(x);
}

#if NPL_Impl_NPLAMath_has_uint128
__extension__ using uint128_t = unsigned __int128;
#endif

YB_ATTR_always_inline inline void
umul_32_64(std::uint32_t a, std::uint64_t b, std::uint32_t& hi,
	std::uint64_t& lo)
{
#if NPL_Impl_NPLAMath_has_uint128
	const auto m(uint128_t(a) * b);

	yunseq(hi = std::uint64_t(m >> 64), lo = std::uint64_t(m));
#elif NPL_Impl_NPLAMath_has_umul128
	std::uint32_t h;

	lo = _umul128(a, b, &hi);
	hi = h;
#else
	const auto p00(std::uint64_t(a) * std::uint32_t(b)),
		m0(std::uint64_t(a) * (b >> 32) + (p00 >> 32));

	yunseq(hi = std::uint32_t(m0 >> 32), lo = (m0 << 32) | std::uint32_t(p00));
#endif
}

YB_ATTR_always_inline inline void
umul_64_64(std::uint64_t a, std::uint64_t b, std::uint64_t& hi,
	std::uint64_t& lo)
{
#if NPL_Impl_NPLAMath_has_uint128
	const auto m(uint128_t(a) * b);

	yunseq(hi = std::uint64_t(m >> 64), lo = std::uint64_t(m));
#elif NPL_Impl_NPLAMath_has_umul128
	lo = _umul128(a, b, &hi);
#else
	const std::uint64_t a0(static_cast<std::uint32_t>(a)), a1(a >> 32),
		b0(static_cast<std::uint32_t>(b)), b1(b >> 32), p00(a0 * b0),
		m0(a0 * b1 + (p00 >> 32)), m1(a1 * b0 + std::uint32_t(m0));

	yunseq(hi = a1 * b1 + std::uint32_t(m0 >> 32) + std::uint32_t(m1 >> 32),
		lo = (std::uint64_t(std::uint32_t(m1)) << 32) | std::uint32_t(p00));
#endif
}

YB_ATTR_always_inline inline std::uint64_t
umul_64_64_hi(std::uint64_t a, std::uint64_t b)
{
#if NPL_Impl_NPLAMath_has_uint128
	const auto m(uint128_t(a) * b);

	return std::uint64_t(m >> 64);
#elif NPL_Impl_NPLAMath_has_umul128
	std::uint64_t hi;

	_umul128(a, b, &hi);
	return hi;
#else
	const std::uint64_t a0(static_cast<std::uint32_t>(a)), a1(a >> 32),
		b0(static_cast<std::uint32_t>(b)), b1(b >> 32),
		m0(a0 * b1 + ((a0 * b0) >> 32));

	return a1 * b1 + std::uint32_t(m0 >> 32)
		+ std::uint32_t((a1 * b0 + std::uint32_t(m0)) >> 32);
#endif
}

YB_ATTR_always_inline inline void
umul_64_64_add(std::uint64_t a, std::uint64_t b, std::uint64_t c,
	std::uint64_t& hi, std::uint64_t& lo)
{
#if NPL_Impl_NPLAMath_has_uint128
	const auto m(uint128_t(a) * b + c);

	yunseq(hi = std::uint64_t(m >> 64), lo = std::uint64_t(m));
#else
	std::uint64_t h, l, t;

	umul_64_64(a, b, h, l);
	t = l + c;
	h += std::uint64_t((t < l ? 1 : 0) | (t < c ? 1 : 0));
	yunseq(hi = h, lo = t);
#endif
}

#if !NPL_Impl_NPLAMath_has_uint128
// XXX: This is a limited emulation of the GCC %uint128_t used in this
//	implementation. Other operations may be unipmlemented and no
//	%std::numeric_limits specialization is done yet.
class uint128_t final : private ystdex::operators<uint128_t>,
	private ystdex::shiftable<uint128_t>
{
private:
	std::uint64_t low, high;

public:
	YB_ATTR_always_inline yconstfn
	uint128_t() ynothrow
		: uint128_t(0, 0)
	{}
	template<typename _type,
		yimpl(ystdex::enable_if_t<std::is_integral<_type>::value
		&& std::numeric_limits<_type>::digits <= 64, int> = 0)>
	YB_ATTR_always_inline yconstfn
	uint128_t(_type x) ynothrow
		: uint128_t(std::uint64_t(x), 0)
	{}
	// NOTE: Converting from floating-point values are disabled to avoid
	//	misuses. Only finite values are used, which are better convert in the
	//	call site. Use %fp_to_int below instead for finite values.
	template<typename _type,	
#	if NPL_Impl_NPLAMath_UseQuadMath
		yimpl(typename = ystdex::enable_if_t<
			std::is_floating_point<_type>::value
			|| std::is_same<_type, float128_t>::value>)
#	else
		yimpl(typename
			= ystdex::enable_if_t<std::is_floating_point<_type>::value>)
#	endif
	>
	uint128_t(_type) = delete;

private:
	YB_ATTR_always_inline yconstfn
	uint128_t(uint64_t l, uint64_t h) ynothrow
		: low(l), high(h)
	{}

public:
	YB_ATTR_nodiscard YB_ATTR_always_inline YB_PURE yconstfn uint128_t
	operator~() const ynothrow
	{
		return uint128_t(~low, ~high);
	}

	YB_ATTR_nodiscard YB_ATTR_always_inline YB_PURE yconstfn_relaxed uint128_t
	operator-() const ynothrow
	{
		auto res(~*this);

		++res;
		return res;
	}

	YB_ATTR_always_inline yconstfn_relaxed uint128_t&
	operator++() ynothrow
	{
		++low;
		if(YB_UNLIKELY(low == 0))
			++high;
		return *this;
	}

	YB_ATTR_always_inline uint128_t&
	operator+=(uint128_t v) ynothrow
	{
		const auto plo(low);

		low += v.low;
		high += v.high + (low < plo ? 1 : 0);
		return *this;
	}

	YB_ATTR_always_inline uint128_t&
	operator-=(uint128_t v) ynothrow
	{
		return *this += -v;
	}

	YB_ATTR_always_inline uint128_t&
	operator*=(uint128_t v) ynothrow
	{
		high = low * v.high + high * v.low + umul_64_64_hi(low, v.low);
		low *= v.low;
		return *this;
	}

	// XXX: %operator/= and %operator%= are not implemented.

	uint128_t&
	operator<<=(size_t n) ynothrowv
	{
		yconstraint(n < 128);

		if(n < 64)
		{
			high = high << n | (low & ~uint64_t() << (64 - n)) >> (64 - n);
			low <<= n;
		}
		else
		{
			high = low << (n - 64);
			low = 0;
		}
		return *this;
	}

	uint128_t&
	operator>>=(size_t n) ynothrowv
	{
		yconstraint(n < 128);

		if(n < 64)
		{
			low = low >> n | (high & ~uint64_t() >> (64 - n)) << (64 - n);
			high >>= n;
		}
		else
		{
			low = high >> (n - 64);
			high = 0;
		}
		return *this;
	}

	PDefHOp(uint128_t&, &=, uint128_t v) ynothrowv
		ImplRet(yunseq(high &= v.high, low &= v.low), *this);

	PDefHOp(uint128_t&, |=, uint128_t v) ynothrowv
		ImplRet(yunseq(high |= v.high, low |= v.low), *this);

	YB_ATTR_nodiscard YB_STATELESS YB_ATTR_always_inline friend yconstfn
		PDefHOp(bool, ==, uint128_t x, uint128_t y) ynothrow
		ImplRet(x.high == y.high && x.low == y.low)

	YB_ATTR_nodiscard YB_STATELESS friend yconstfn PDefHOp(bool, <, uint128_t x,
		uint128_t y) ynothrow
		ImplRet(x.high < y.high || (x.high == y.high && x.low < y.low))

	// XXX: Only integer types are supported. Assume there is no integral type
	//	has a larger range than %low.
	template<typename _type, yimpl(typename = ystdex::enable_if_t<
		std::is_integral<_type>::value>, typename = ystdex::enable_if_t<
		ystdex::is_explicitly_constructible<_type, std::uint64_t>::value>)>
	YB_ATTR_nodiscard YB_STATELESS YB_ATTR_always_inline explicit yconstfn
	operator _type() const ynoexcept_spec(_type(low))
	{
		return _type(low);
	}
	template<typename _type, yimpl(typename = ystdex::enable_if_t<
		std::is_integral<_type>::value>,
		ystdex::enable_if_convertible_t<std::uint64_t, _type, int> = 0)>
	YB_ATTR_nodiscard YB_STATELESS YB_ATTR_always_inline yconstfn
	operator _type() const ynoexcept_spec(_type(low))
	{
		return low;
	}
	// XXX: Assume that %float has less than 64 bit precision so there is no
	//	loss.
	//! \since build 933
	YB_ATTR_nodiscard YB_STATELESS YB_ATTR_always_inline explicit
		DefCvt(const ynothrow, float, std::ldexp(float(high), 64) + float(low))
#	if NPL_Impl_NPLAMath_UseQuadMath
	YB_ATTR_nodiscard YB_STATELESS YB_ATTR_always_inline explicit
		DefCvt(const ynothrow, float128_t,
		yimpl(fpq)::ldexp(float128_t(high), 64) + float128_t(low))
#	endif
};
#endif

template<typename _tValue, typename _type = int>
using enable_if_uint_t = ystdex::enable_if_t<ystdex::or_<
	std::is_unsigned<_tValue>, std::is_same<_tValue, uint128_t>>::value, _type>;

template<typename _type,
	yimpl(typename = ystdex::enable_if_convertible_t<_type, uint128_t>)>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yconstfn uint128_t
to_uint128(_type x) ynothrow
{
	return static_cast<uint128_t>(x);
}


// XXX: Not using %ystdex::countr_zero because it has no precondition on the
//	parameter.

//! \since build 933
//@{
using ystdex::countr_zero_narrow;
#if !NPL_Impl_NPLAMath_LongDoubleAsDouble
YB_ATTR_nodiscard YB_ATTR_always_inline YB_PURE inline int
countr_zero_narrow(uint128_t x) ynothrow
{
	const auto lo(static_cast<std::uint64_t>(x));

	return x == 0 ? countr_zero_narrow(std::uint64_t(x >> 64)) + 64
		: countr_zero_narrow(lo);
}
#endif
//@}

template<typename _type, yimpl(enable_if_uint_t<_type> = 0)>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yconstfn bool
is_odd(_type x) ynothrow
{
	return bool(x & 1);
}

template<typename _type>
YB_ATTR_nodiscard YB_STATELESS yconstfn _type
upow(_type x, unsigned e) noexcept
{
	return e == 0 ? _type(1)
		: e == 1 ? x : upow(x * x, e >> 1) * (is_odd(e) ? x : _type(1));
}

/*!
\pre 浮点参数是有限值。
\since build 933
*/
//@{
template<typename _tInt, typename _tFloat>
YB_ATTR_nodiscard _tInt
fp_to_int(_tFloat x, _tInt) ynothrow
{
	return _tInt(x);
}
template<typename _tFloat>
YB_ATTR_nodiscard uint128_t
fp_to_int(_tFloat x, uint128_t) ynothrowv
{
	using namespace yimpl(fpq);
	const auto hi(floor(ldexp(x, -64)));

	YAssert(isfinite(hi) && hi <= _tFloat(std::numeric_limits<
		std::uint64_t>::max()), "Unexpected overflow found.");
	return uint128_t(std::uint64_t(hi)) << 64
		| uint128_t(std::uint64_t(x - ldexp(hi, 64)));
}
template<typename _tInt, typename _tFloat>
YB_ATTR_nodiscard _tInt
fp_to_int(_tFloat x) ynothrowv
{
	return fp_to_int(x, _tInt());
}
//@}


#if !NPL_Impl_NPLAMath_LongDoubleAsDouble
// NOTE: This is an optimization of division, see
//	https://gmplib.org/~tege/divcnst-pldi94.pdf. Nevertheless, it is necessary
//	because %uint128_t replacement does not implement operator%=, since all use
//	cases using invariant divisors.
// NOTE: 128-bit constant divisor is not optimized by implementations as usual.
//	GCC >= 11.1 can optimize the divisors 10 and 100, but not 1000 or 10000. No
//	other known implementations optimize dividing to %uint128_t. See
//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97459. See
//	https://gcc.gnu.org/bugzilla/attachment.cgi?id=49520 for magic numbers. See
//	also https://danlark.org/2020/06/14/128-bit-division/ for some related other
//	runtime optimization (not used here).
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline unsigned
u128_mod_5(uint128_t v) ynothrow
{
	std::uint64_t a;

	// NOTE: 2^64 ≡ 1 (mod 5).
#if __has_builtin(__builtin_add_overflow)
	if(YB_UNLIKELY(__builtin_add_overflow(std::uint64_t(v >> 64),
		std::uint64_t(v), &a)))
#else
	a = std::uint64_t(v >> 64) + std::uint64_t(v);
	if(YB_UNLIKELY(a < std::uint64_t(v >> 64)))
#endif
		++a;
	return a % 5;
}

YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline uint128_t
u128_div_5(uint128_t v) ynothrow
{
	// NOTE: ⌈4/5 × 2^128⌉ × 5 ≡ 1 (mod 2^128),
	//	⌈4/5 × 2^128⌉ = 0xCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCD.
	return (v - u128_mod_5(v))
		* (uint128_t(0xCCCCCCCCCCCCCCCC) << 64 | 0xCCCCCCCCCCCCCCCD);
}

#	if true
// XXX: This should be more efficient in general.
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yconstfn unsigned
u128_mod_25(uint128_t v) ynothrow
{
	// NOTE: 2^60 ≡ 1 (mod 25).
	return (std::uint64_t(v & ((std::uint64_t(1) << 60) - 1))
		+ (std::uint64_t(v >> 60) & ((std::uint64_t(1) << 60) - 1))
		+ std::uint64_t(v >> 120)) % 25;
}

YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS yconstfn uint128_t
u128_div_25(uint128_t v) ynothrow
{
	// NOTE: ⌈4/25 × 2^128⌉ × 25 ≡ 1 (mod 2^128),
	//	⌈4/25 × 2^128⌉ = 0x28F5C28F5C28F5C28F5C28F5C28F5C29.
	return (v - u128_mod_25(v))
		* (uint128_t(0x28F5C28F5C28F5C2) << 64 | 0x8F5C28F5C28F5C29);
}
#	else
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline uint128_t
u128_div_25(uint128_t v) ynothrow
{
	return u128_div_5(u128_div_5(v));
}
#	endif

#	if LDBL_MANT_DIG == 64
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline uint128_t
u128_div_1e4(uint128_t v) ynothrow
{
	return u128_div_25(u128_div_25(v)) >> 4;
}
#	endif
#endif


//! \ingroup traits
//@{
template<typename _type>
struct fp_traits
{
	//! \brief 值表示位数。
	static yconstexpr const size_t value_bits = sizeof(_type) * CHAR_BIT;
	// XXX: There are no known C++ implementations having floating-point types
	//	larger than 128-bit at current. At least all platform configurations
	//	supported by YFramework have no known plans to support such extension in
	//	known future. It is very unlikely to change due to the already
	//	problematic ABI mess (esp. on 'long double', and '_Float' vs.
	//	'__float128' which is a C extension, but not likely to be imported in
	//	C++).
	static_assert(value_bits <= 128,
		"Unsupported floating-point format found.");

	//! \brief 能保存值表示的位掩码类型。
	// TODO: Support non-integral type when standard integer types are not
	//	available for %_type. This should include 'long double' which are larger
	//	than 64-bit.
	// XXX: This does not guarantee the carrier type having exactly %value_bits
	//	bits. Ideally it should be %unsigned_fast_type, but it may incur
	//	unnecessary penalty for calculations with automatic variables, e.g.
	//	mul/div overhead when uint_fast32_t is 64-bit in x86_64 glibc, though
	//	the overhead is typically depending on the microarchitecture of the CPU,
	//	and div is mostly avoided by an optimizing compiler in practice.
	using carrier_type
		= typename ystdex::make_width_int<value_bits>::yimpl(unsigned_type);
};

#if !NPL_Impl_NPLAMath_LongDoubleAsDouble
template<>
struct fp_traits<long double>
{
	// XXX: This is an upper bound. The may acutall have less effective value
	//	bits, e.g. x87 80-bit floating-point value is detected here as 96 bits
	//	or 128 bits, depending on the architeture. Nevertheless, this should not
	//	incur more nitches used only by %carrier_type.
	static yconstexpr const size_t value_bits = sizeof(long double) * CHAR_BIT;

	using carrier_type = ystdex::conditional_t<value_bits == 64, std::uint64_t,
		uint128_t>;
};
#endif
//@}


template<typename _type>
YB_ATTR_nodiscard YB_STATELESS yconstfn _type
udiv_10(_type v) ynothrow
{
	return v / 10;
}
#if !NPL_Impl_NPLAMath_LongDoubleAsDouble
YB_ATTR_nodiscard YB_STATELESS inline uint128_t
udiv_10(uint128_t v) ynothrow
{
	return u128_div_5(v) >> 1;
}
#endif


YB_ATTR_nodiscard YB_STATELESS std::uint_fast8_t
get_tz_2(std::uint_fast8_t idx) ynothrowv
{
	yconstraint(idx < 100);
	return (idx % 10 == 0 ? 1 : 0) + (idx == 0 ? 1 : 0);
}


//! \since build 933
//@{
// XXX: Decimal values more than 38 digits are not supported.
//! \ingroup transformation_traits
template<size_t _vLen>
using DigitsMinUInt = ystdex::conditional_t<_vLen <= 2, std::uint_fast8_t,
	ystdex::conditional_t<_vLen <= 4, std::uint16_t,
	ystdex::conditional_t<_vLen <= 8, std::uint32_t,
	ystdex::conditional_t<_vLen <= 19, std::uint64_t,
	ystdex::conditional_t<_vLen <= 38, uint128_t, yimpl(void)>>>>>;

template<size_t _vLen, yimpl(typename = void)>
struct DecimalDigits;

template<>
struct DecimalDigits<2> final
{
	using UInt = DigitsMinUInt<2>;

	static yconstexpr const UInt Divisor = 10;

	YB_ATTR_always_inline YB_NONNULL(1) static char*
	Write(char* buf, UInt val) ynothrowv
	{
		yconstraint(val < Divisor * Divisor);
		yunseq(buf[0] = '0' + char(val / Divisor),
			buf[1] = '0' + char(val % Divisor));
		return buf + 2;
	}
};

// XXX: Many '% 10000' in LLVM/Clang is not optimal. See
//	https://bugs.llvm.org/show_bug.cgi?id=38217. No workaround is applied yet
//	for the QoI issue.
template<size_t _vLen>
struct DecimalDigits<_vLen, ystdex::enable_if_t<(!is_odd(_vLen))>>
{
	using UInt = DigitsMinUInt<_vLen>;
	using HalfWriter = DecimalDigits<_vLen / 2>;

	static yconstexpr const UInt Divisor
		= UInt(HalfWriter::Divisor) * HalfWriter::Divisor;

	YB_ATTR_always_inline YB_NONNULL(1) static char*
	Write(char* buf, UInt val) ynothrowv
	{
		using HalfUInt = DigitsMinUInt<_vLen / 2>;

		yconstraint(val < Divisor * Divisor);
		yunseq(HalfWriter::Write(buf, HalfUInt(val / Divisor)),
			HalfWriter::Write(buf + _vLen / 2, HalfUInt(val % Divisor)));
		return buf + _vLen;
	}
};


template<size_t _vLen>
YB_ATTR_nodiscard YB_NONNULL(1) char*
WriteDecimalDigitsIn(char*, DigitsMinUInt<_vLen>) ynothrowv;
template<>
YB_ATTR_nodiscard YB_NONNULL(1) char*
WriteDecimalDigitsIn<2>(char* buf, DigitsMinUInt<2> val) ynothrowv
{
	yconstraint(val < 100);
	if(YB_UNLIKELY(val < 10))
	{
		*buf = '0' + char(val);
		return buf + 1;
	}
	DecimalDigits<2>::Write(buf, DigitsMinUInt<2>(val));
	return buf + 2;
}
template<>
YB_ATTR_nodiscard YB_NONNULL(1) char*
WriteDecimalDigitsIn<3>(char* buf, DigitsMinUInt<3> val) ynothrowv
{
	yconstraint(val < 1000);
	if(val < 100)
		return WriteDecimalDigitsIn<2>(buf, DigitsMinUInt<2>(val));
	buf[0] = '0' + char(val / 100);
	DecimalDigits<2>::Write(++buf, DigitsMinUInt<2>(val % 100));
	return buf + 2;
}
template<>
YB_ATTR_nodiscard YB_NONNULL(1) char*
WriteDecimalDigitsIn<4>(char* buf, DigitsMinUInt<4> val) ynothrowv
{
	yconstraint(val < 10000);
	if(val < 100)
		return WriteDecimalDigitsIn<2>(buf, DigitsMinUInt<2>(val));
	// NOTE: 3-4 digits: aabb.
	const auto aa(val / 100);

	if(aa < 10)
	{
		*buf = '0' + aa;
		--buf;
	}
	else
		DecimalDigits<2>::Write(buf, aa);
	DecimalDigits<2>::Write(buf + 2, val % 100);
	return buf + 4;
}
template<>
YB_ATTR_nodiscard YB_NONNULL(1) char*
WriteDecimalDigitsIn<7>(char* buf, DigitsMinUInt<8> val) ynothrowv
{
	yconstraint(val < 10000000);
	return val < 10000 ? WriteDecimalDigitsIn<4>(buf, DigitsMinUInt<4>(val))
		: DecimalDigits<4>::Write(WriteDecimalDigitsIn<3>(buf,
		DigitsMinUInt<3>(val / 10000)), DigitsMinUInt<4>(val % 10000));
}
template<>
YB_ATTR_nodiscard YB_NONNULL(1) char*
WriteDecimalDigitsIn<8>(char* buf, DigitsMinUInt<8> val) ynothrowv
{
	yconstraint(val < 100000000);
	return val < 10000 ? WriteDecimalDigitsIn<4>(buf, DigitsMinUInt<4>(val))
		: DecimalDigits<4>::Write(WriteDecimalDigitsIn<4>(buf,
		DigitsMinUInt<4>(val / 10000)), DigitsMinUInt<4>(val % 10000));
}
template<>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_NONNULL(1) inline char*
WriteDecimalDigitsIn<9>(char* buf, DigitsMinUInt<9> val) ynothrowv
{
	yconstraint(val < 1000000000);
	if(val < 100000000)
		return WriteDecimalDigitsIn<8>(buf, val);
	*buf = '0' + val / 100000000;
	return DecimalDigits<8>::Write(buf + 1, val % 100000000);
}
template<>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_NONNULL(1) inline char*
WriteDecimalDigitsIn<15>(char* buf, DigitsMinUInt<16> val) ynothrowv
{
	yconstraint(val < 1000000000000000);
	return val < 100000000 ? WriteDecimalDigitsIn<8>(buf,
		DigitsMinUInt<8>(val)) : DecimalDigits<8>::Write(
		WriteDecimalDigitsIn<7>(buf, DigitsMinUInt<7>(val / 100000000)),
		DigitsMinUInt<8>(val % 100000000));
}
template<>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_NONNULL(1) inline char*
WriteDecimalDigitsIn<16>(char* buf, DigitsMinUInt<16> val) ynothrowv
{
	yconstraint(val < 10000000000000000);
	return val < 100000000 ? WriteDecimalDigitsIn<8>(buf,
		DigitsMinUInt<8>(val)) : DecimalDigits<8>::Write(
		WriteDecimalDigitsIn<8>(buf, DigitsMinUInt<8>(val / 100000000)),
		DigitsMinUInt<8>(val % 100000000));
}
template<>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_NONNULL(1) inline char*
WriteDecimalDigitsIn<17>(char* buf, DigitsMinUInt<17> val) ynothrowv
{
	yconstraint(val < 100000000000000000);
	if(val < 10000000000000000)
		return WriteDecimalDigitsIn<16>(buf, val);

	// NOTE: 17 digits.
	const auto hi(val / 100000000);
	const auto one(DigitsMinUInt<8>(hi / 100000000));

	*buf = '0' + char(one);
	buf += one > 0 ? 1 : 0;
	yunseq(DecimalDigits<8>::Write(buf, hi % 100000000),
		DecimalDigits<8>::Write(buf + 8, val % 100000000));
	return buf + 16;
}
template<>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_NONNULL(1) inline char*
WriteDecimalDigitsIn<19>(char* buf, DigitsMinUInt<19> val) ynothrowv
{
	if(val < 100000000)
		return WriteDecimalDigitsIn<8>(buf, DigitsMinUInt<8>(val));
	const auto hi(val / 100000000);
	const auto lo(DigitsMinUInt<8>(val % 100000000));

	if(hi < 100000000)
		buf = WriteDecimalDigitsIn<8>(buf, DigitsMinUInt<8>(hi));
	else
		buf = DecimalDigits<8>::Write(WriteDecimalDigitsIn<8>(buf,
			DigitsMinUInt<8>(hi / 100000000)),
			DigitsMinUInt<8>(hi % 100000000));
	return DecimalDigits<8>::Write(buf, lo);
}
#if !NPL_Impl_NPLAMath_LongDoubleAsDouble
#	if LDBL_MANT_DIG == 64
#	define NPL_Impl_NPLAMath_PrintLargeSignificant false
template<>
YB_ATTR_nodiscard YB_NONNULL(1) char*
WriteDecimalDigitsIn<21>(char* buf, DigitsMinUInt<21> val) ynothrowv
{
	yconstraint(val < uint128_t(100000000000U) * 10000000000U);
	if(val < 100000000000000000)
		return WriteDecimalDigitsIn<17>(buf, DigitsMinUInt<17>(val));

	// NOTE: 18-21 digits.
	const auto hi(u128_div_1e4(val));

	return DecimalDigits<4>::Write(WriteDecimalDigitsIn<17>(buf,
		DigitsMinUInt<17>(hi)), DigitsMinUInt<4>(val - hi * 10000));
}
YB_ATTR_nodiscard YB_ATTR_always_inline YB_NONNULL(1) inline char*
WriteU128Trailing(char* buf, uint128_t val) ynothrowv
{
	return WriteDecimalDigitsIn<21>(buf, val);
}
#	elif LDBL_MANT_DIG > 64 && LDBL_MANT_DIG <= 113 \
	&& NPL_Impl_NPLAMath_has_uint128
#	define NPL_Impl_NPLAMath_PrintLargeSignificant true
template<>
YB_ATTR_nodiscard YB_NONNULL(1) char*
WriteDecimalDigitsIn<32>(char* buf, DigitsMinUInt<32> val) ynothrowv
{
	yconstraint(val
		< DigitsMinUInt<32>(10000000000000000) * 10000000000000000);

	return val < 100000000000000000 ? WriteDecimalDigitsIn<16>(buf,
		DigitsMinUInt<16>(val)) : DecimalDigits<16>::Write(WriteDecimalDigitsIn<
		16>(buf, DigitsMinUInt<16>(val / 10000000000000000))),
		DigitsMinUInt<16>(val % 10000000000000000));
}
template<>
YB_ATTR_nodiscard YB_NONNULL(1) char*
WriteDecimalDigitsIn<36>(char* buf, DigitsMinUInt<36> val) ynothrowv
{
	const auto d(DigitsMinUInt<36>(10000000000000000) * 10000000000000000);

	yconstraint(val < d * 10000U);
	return val < d ? WriteDecimalDigitsIn<32>(buf, val) : DecimalDigits<
		32>::Write(WriteDecimalDigitsIn<4>(buf, val / d), val % d);
}

YB_ATTR_nodiscard YB_ATTR_always_inline YB_NONNULL(1) inline char*
WriteU128Trailing(char* buf, uint128_t val) ynothrowv
{
	return WriteDecimalDigitsIn<36>(buf, val);
}
#	else
#		error "Not implemented."
#	endif
#endif

YB_ATTR_nodiscard YB_ATTR_nodiscard YB_NONNULL(1) inline char*
WriteDecimal15To17Trimmed(char* buf, DigitsMinUInt<17> sig) ynothrowv
{
	// NOTE: Decimal digits: abbccddeeffgghhii.
	const auto abbccddee(std::uint32_t(sig / 100000000));
	const auto abbcc(abbccddee / 10000);
	const auto abb(abbcc / 100);
	const auto a(abb / 100);

	buf[0] = '0' + a;
	buf += a > 0 ? 1 : 0;

	const auto bb(abb % 100);

	if(bb < 10 && a == 0)
	{
		*buf = '0' + bb;
		--buf;
	}
	else
		DecimalDigits<2>::Write(buf, bb);

	const auto cc(abbcc % 100);

	DecimalDigits<2>::Write(buf + 2, cc);

	const auto ddee(abbccddee % 10000);
	const auto ffgghhii(sig % 100000000);

	if(ffgghhii != 0)
	{
		const auto ffgg(ffgghhii / 10000);
		const auto ff(DigitsMinUInt<2>(ffgg / 100)),
			gg(DigitsMinUInt<2>(ffgg % 100));

		DecimalDigits<4>::Write(buf + 4, DigitsMinUInt<4>(ddee)),
		DecimalDigits<2>::Write(buf + 8, ff),
		DecimalDigits<2>::Write(buf + 10, DigitsMinUInt<2>(gg));

		const auto hhii(ffgghhii % 10000);

		if(hhii != 0)
		{
			const auto hh(DigitsMinUInt<2>(hhii / 100)),
				ii(DigitsMinUInt<2>(hhii % 100));

			DecimalDigits<2>::Write(buf + 12, hh),
			DecimalDigits<2>::Write(buf + 14, ii);
			return buf + 16 - (ii != 0 ? get_tz_2(ii) : get_tz_2(hh) + 2);
		}
		return buf + 12 - (gg != 0 ? get_tz_2(gg) : get_tz_2(ff) + 2);
	}
	else if(ddee != 0)
	{
		const auto dd(ddee / 100);
		const auto ee(ddee % 100);

		DecimalDigits<2>::Write(buf + 4, dd),
		DecimalDigits<2>::Write(buf + 6, ee);
		return buf + 8 - (ee != 0 ? get_tz_2(ee) : get_tz_2(dd) + 2);
	}
	return buf + 4 - (cc != 0 ? 0 : get_tz_2(abb % 100)) - get_tz_2(cc);
}
//@}

template<typename _type>
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) inline char*
WriteFPExponentAbs(char* buf, unsigned exp) ynothrowv
{
	static_assert(std::is_floating_point<_type>(), "Invalid type found.");

	return WriteDecimalDigitsIn<8>(buf, DigitsMinUInt<8>(exp));
}

#if NPL_Impl_NPLAMath_IEC_60559_BFP
template<>
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) inline char*
WriteFPExponentAbs<float>(char* buf, unsigned exp) ynothrowv
{
	return WriteDecimalDigitsIn<2>(buf, DigitsMinUInt<2>(exp));
}
template<>
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) inline char*
WriteFPExponentAbs<double>(char* buf, unsigned exp) ynothrowv
{
	return WriteDecimalDigitsIn<3>(buf, DigitsMinUInt<3>(exp));
}
#endif
// XXX: 'long double' is out of bless.

template<typename _type>
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) inline char*
WriteFPExponentSubnormalAbs(char* buf, unsigned exp) ynothrowv
{
	return WriteFPExponentAbs<_type>(buf, exp);
}

template<>
YB_ATTR_nodiscard YB_NONNULL(1) inline char*
WriteFPExponentSubnormalAbs<float>(char* buf, unsigned exp) ynothrowv
{
#if NPL_Impl_NPLAMath_IEC_60559_BFP
	DecimalDigits<2>::Write(buf, exp % 100);
	return buf + 2;
#else
	return WriteDecimalDigitsIn<8>(buf, DigitsMinUInt<8i>(exp));
#endif
}
template<>
YB_ATTR_nodiscard YB_NONNULL(1) inline char*
WriteFPExponentSubnormalAbs<double>(char* buf, unsigned exp) ynothrowv
{
#if NPL_Impl_NPLAMath_IEC_60559_BFP
	buf[0] = '0' + char(exp / 100);
	DecimalDigits<2>::Write(++buf, exp % 100);
	return buf + 2;
#else
	return WriteDecimalDigitsIn<8>(buf, DigitsMinUInt<8>(exp));
#endif
}
// XXX: 'long double' is out of bless.

template<typename _type>
YB_ATTR_nodiscard YB_NONNULL(1) inline char*
WriteFPExponentSubnormal(char* buf, int exp) ynothrowv
{
	YAssert(exp > 0, "Unexpected non-positive exponent value found.");
	buf[0] = '-';
	return WriteFPExponentSubnormalAbs<_type>(++buf, unsigned(exp));
}


// XXX: Precision specification is needed to shrink the value e.g. IEC 60559
//	binary64 1.2, 1.2e-20 and 1.2e-308 (otherwise, there would be typically
//	trailing digits, like 1.2000000000000002, 1.1999999999999998e-20 and
//	1.2000000000000003e-308) in the finally output. To make it precise, the
//	internal conversion should also be precise, but not strictly guaranteed
//	without table-assisted high-precision multiplcations. The concrete errors
//	are unspecificed, but expected small enough. Without
//	%NPL_Impl_NPLAMath_UseQuadMath, some values are known not precisely
//	converted, e.g. binary64 1e-7 is in 9.999999999999996e-8 with exact IEC
//	60559 binary 64 implementations (e.g. SSE), or still imprecise results
//	with a slightly less error for other configurations (probably correctly
//	rounded, e.g. for 1e-6 with x87). Even with %NPL_Impl_NPLAMath_UseQuadMath,
//	some binary64 values are known not correctly rounded if 128-bit integer
//	arithmetic is not used, e.g. 1e-11 is converted to 9.999999999999999e-12,
//	due to the internal precision loss. On the other hand, 'long double'
//	implemented by 'double' conversion are always imprecise and the error would
//	be shown in some cases even the value is rounded. These imprecision results
//	are tolerated.
//! \ingroup transformation_traits
//@{
template<typename>
struct ExtFPConv
{
	// XXX: Without some arbitrary precision arithmetic or some tables, the
	//	precision is not guaranteed. Use 'long double' to do the best effort. It
	//	would still keep the right digits if it has more than 64 bit precision,
	//	or it would be less precise when the type is just same to 'double'.
	//	(Typically, 'double' is of IEC 60559 binary64 having 52 bit precision.)
	//	Although there are unreliable trailing digits, the remaining algorithm
	//	is not modified as all numbers being compared still have same magnitude
	//	of the possible error from the same source, so it still works (mostly).
#	if NPL_Impl_NPLAMath_UseQuadMath
	using type = float128_t;
#	else
	using type = long double;
#	endif
};

template<>
struct ExtFPConv<float>
{
	// XXX: Not necessarily sufficient, but seems OK for IEC 60559 BFPs.
	using type = double;
};
//@}


/*!
\brief 取指定十进制指数对应的 10 的幂的正规分解值的偏移量。
\return 偏移量的值，作用相当于 Schubfach 算法实现中的参数 h - q 。

偏移量作用在二进制指数上，对应 Schubfach 算法的参数 h ，
	但定义因为对应的查找表格式和支持的浮点数位宽不同而不同。
*/
//@{
template<typename>
YB_ATTR_nodiscard YB_STATELESS yconstfn int
FPBinaryToDecimal_GetOffset(int exp_dec) ynothrow
{
	// NOTE: The expoenent part for 10^(-k), denoted 'h' in the Schubfach paper
	//	§9.9 (9), is a slightly differently defined here as
	//	q + ⌊(㏒₂10^(-k))⌋ + 1. The difference is using '+ 1' instead of '+ 2',
	//	due to using unsigned 64-bit integer instead of %long in Java for the
	//	table entry unit, leading to different precision (127-bit instead of
	//	126-bit). (See also the original Java implementation at
	//	https://mail.openjdk.java.net/pipermail/core-libs-dev/2018-September/055698.html
	//	and the C++ port at
	//	https://github.com/abolz/Drachennest/blob/master/src/schubfach_64.cc for
	//	the difference.) The entry width is 128-bit, but the highest bit is
	//	always 1, so only 127 bits vary. Whatever the highest bit is, the
	//	unsigned nature of the low half makes multiplication operations
	//	significantly simpler (esp. more friendly to existing compiler
	//	builtins). It also makes the generation of the table easier. Moreover,
	//	the entry format is compatible to the normalized results from %frexp,
	//	hence a bit more potable, allowing getting rid of the table more easily,
	//	at the cost of reducing efficiency. This function compute the value
	//	h - q, defined as h - q = ⌊㏒₂10^(-k)⌋ + 1 = ⌊-k ㏒₂10⌋ + 1
	//	= ⌊3.32192809... (-k)⌋ + 1 ≈ -3.321929931640625k + 1
	//	= (-k) × 217706 / 2^16 + 1, where 'q' is handled in the caller.
	// XXX: The type %int is not guaranteed to prevent overflow here. Using
	//	%std::int_fast32_t is not more efficient for typical implementations.
	return ((-std::int32_t(exp_dec) * 217706) >> 16) + 1;
}
template<>
YB_ATTR_nodiscard YB_STATELESS yconstfn int
FPBinaryToDecimal_GetOffset<float>(int exp_dec) ynothrow
{
	// NOTE: This is less precise than binary64 case, by throwing the lower 64
	//	bits away in the multiplication. The result is occasionally same to the
	//	Schubfach paper §14 because both implementation assumes exactly one sign
	//	bit in the entry of the powers of 10 table.
	return
		FPBinaryToDecimal_GetOffset<double>(exp_dec) + ((128 - 64) - (96 - 64));
}
//@}

template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline _type
CBLshift(_type x, unsigned lshift)
{
	return x - lshift;
}

template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline _type
CBRshift(_type x)
{
	return x + 2;
}

YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline std::uint64_t
Round128OddHi(std::uint64_t hi, std::uint64_t lo, std::uint64_t cp) ynothrow
{
	std::uint64_t x_hi, r_hi, r_lo;

	x_hi = umul_64_64_hi(cp, lo);
	umul_64_64_add(cp, hi, x_hi, r_hi, r_lo);
	return r_hi | (r_lo > 1 ? 1 : 0);
}

YB_ATTR_nodiscard YB_ATTR_always_inline YB_STATELESS inline std::uint32_t
Round96OddHi(std::uint64_t hi, std::uint32_t cp) ynothrow
{
	std::uint32_t r_hi;
	std::uint64_t r_lo;

	umul_32_64(cp, hi, r_hi, r_lo);
	return r_hi | (r_lo > 1 ? 1 : 0);
}

#if !NPL_Impl_NPLAMath_LongDoubleAsDouble
YB_ATTR_nodiscard YB_STATELESS inline uint128_t
RoundFPOddHi(ystdex::_t<ExtFPConv<long double>> v, uint128_t cp) ynothrow
{
	return fp_to_int<uint128_t>(v * ystdex::_t<ExtFPConv<long double>>(cp));
}
#endif

template<typename _type>
void
FPBinaryToDecimal_1(unsigned lshift, int exp10, unsigned h, unsigned odd,
	typename fp_traits<_type>::carrier_type cb, typename
	fp_traits<_type>::carrier_type& vb, typename fp_traits<_type>::carrier_type&
	lower, typename fp_traits<_type>::carrier_type& upper) ynothrow
{
	YAssert(h >= 1 && h < 5, "Invalid shift value found.");

	using ext_t = ystdex::_t<ExtFPConv<_type>>;
	// NOTE: Using ADL in declarations from the namespace.
	using namespace yimpl(fpq);
	// NOTE: The following are equivalent when %FLT_RADIX is a diviso of 10
	//	(otherwise, there will be some additional precision loss) except that
	//	the base 10 variants may overflow for valid inputs. For example, with
	//	IEC 60559 binary64 it cannot compute %pow10d correctly for any input
	//	where %exp10 is greater than 292 (the orignal input is less than
	//	2^(52 - 1023) ≈ 5.010420900022432e-293 in this case). Using %ext_t
	//	instead of %_type may or may not avoid the overflow, but 5-based powers
	//	will.
	// XXX: Typically the 128-bit intermediate precision is required otherwise
	//	there can be loss of precision, e.g. the 1e-11 case for binary64. In
	//	fact by the Schubfach paper §9.9, the least precision to prevent the
	//	loss in the result is 123 bits, and IEC 60559 binary128 (114 bits) or
	//	double-double precison (106 bit) are still lossy.
#if false
	// XXX: Ignored.
	int ed;
	// XXX: Assume %frepx works for proper %ext_t.
	auto pow10d(frexp(pow(ext_t(10), ext_t(exp10)), &ed));

	pow10d = ldexp(pow10d, 128);
#else
	auto pow10d(ldexp(pow(ext_t(5), ext_t(exp10)),
		-int(floor(exp10 * log2(ext_t(5)))) + 63));
	const auto pow10hi{std::uint64_t(pow10d)};

	pow10d = ldexp(pow10d, 64);

	// XXX: This should not overflow.
	auto pow10lo(static_cast<std::uint64_t>(pow10d));

#endif
	// NOTE: Minimum and maximum exact decimal exponent in powers of 10 are 0
	//	and 55.
	pow10lo += exp10 < 0 || exp10 > 55 ? 1 : 0;
	yunseq(vb = Round128OddHi(pow10hi, pow10lo, cb << h), lower
		= Round128OddHi(pow10hi, pow10lo, CBLshift(cb, lshift) << h) + odd,
		upper = Round128OddHi(pow10hi, pow10lo, CBRshift(cb) << h) - odd);
}
template<>
void
FPBinaryToDecimal_1<float>(unsigned lshift, int exp10, unsigned h, unsigned odd,
	typename fp_traits<float>::carrier_type cb, typename
	fp_traits<float>::carrier_type& vb, typename fp_traits<float>::carrier_type&
	lower, typename fp_traits<float>::carrier_type& upper) ynothrow
{
	YAssert(h >= 33 && h <= 37, "Invalid shift value found.");

	using ext_t = ystdex::_t<ExtFPConv<float>>;
	auto pow10hi(std::uint64_t(std::ldexp(std::pow(ext_t(5), ext_t(exp10)),
		-int(std::floor(exp10 * std::log2(ext_t(5)))) + 63)));

	// NOTE: Minimum and maximum exact decimal exponent in powers of 10 are 0
	//	and 27.
	pow10hi += exp10 < 0 || exp10 > 27 ? 1 : 0;
	yunseq(vb = Round96OddHi(pow10hi, cb << h),
		lower = Round96OddHi(pow10hi, (cb - lshift) << h) + odd,
		upper = Round96OddHi(pow10hi, (cb + 2) << h) - odd);
}
#if !NPL_Impl_NPLAMath_LongDoubleAsDouble
template<>
void
FPBinaryToDecimal_1<long double>(unsigned lshift, int exp10, unsigned h,
	unsigned odd, typename fp_traits<long double>::carrier_type cb,
	typename fp_traits<long double>::carrier_type& vb,
	typename fp_traits<long double>::carrier_type& lower,
	typename fp_traits<long double>::carrier_type& upper) ynothrow
{
	YAssert(h >= 1 && h <= 6, "Invalid shift value found.");

	using ext_t = ystdex::_t<ExtFPConv<long double>>;
	using namespace yimpl(fpq);
	// XXX: Shift less than 64 bits to double.
	auto pow10d(ldexp(pow(ext_t(5), ext_t(exp10)),
		-int(floor(exp10 * log2(ext_t(5)))) - 1));
	// XXX: This is not precise.
	yunseq(vb = RoundFPOddHi(pow10d, cb << h),
		lower = RoundFPOddHi(pow10d, CBLshift(cb, lshift) << h) + odd,
		upper = RoundFPOddHi(pow10d, CBRshift(cb) << h) - odd);
}
#endif

template<typename _type, yimpl(enable_if_uint_t<_type> = 0)>
void
FPBinaryToDecimal_2(const _type& vb, const _type& upper, const _type& lower,
	_type& sig_dec, int& exp_dec) ynothrow
{
	if(sig_dec >= 10)
	{
		const auto sp(udiv_10(sig_dec));
		const bool w_in(upper >= 40 * sp + 40);

		if((lower <= 40 * sp) != w_in)
		{
			yunseq(sig_dec = sp + (w_in ? 1 : 0), ++exp_dec);
			return;
		}
	}

	const bool w_in(upper >= (sig_dec << 2) + 4);

	if((lower <= (sig_dec << 2)) != w_in)
		sig_dec += w_in ? 1 : 0;
	else
	{
		const auto mid((sig_dec << 2) + 2);

		sig_dec += vb > mid || (vb == mid && (sig_dec & 1) != 0) ? 1 : 0;
	}
}

YB_ATTR_nodiscard YB_ATTR_always_inline yconstfn int
ComputeDecimalExponent(int exp_bin, bool shorter) ynothrow
{
	// NOTE: Given that 'e' is the true value of the %exp_bin here, The value of
	//	'k' (the Schubfach paper §8 R10, or the negation of 'k₀' in the
	//	Dragonbox paper) is computed as ⌊㏒₁₀(2^e)⌋ (equals to ⌊e ㏒₁₀2⌋) for
	//	the normal case or ⌊(㏒₁₀((2^e) × 3 / 4)⌋ (equals to
	//	⌊e ㏒₁₀2 + ㏒₁₀0.75⌋) otherwise, results a value in the interval
	//	[-324, 292] for IEC 60559 binary64.
	// XXX: See also the comments in %FPBinaryToDecimal_GetOffset.
#if false
	//	To compute 'k', manual optimization is used, given that: ㏒₁₀2
	//	= 0.30102999... ≈ 0.30103015899658203125 = 315653 / 2^20, and ㏒₁₀0.75
	//	= -0.12493873... ≈ -0.12493896484375 = -131008 / 2^20. The 'floor' can
	//	be removed away, as it can be verified the approximation does not
	//	introduced error greater than 1.
	// XXX: This fails at -1335, which is not sufficient for typical
	//	'long double' implementations.
	return (std::int32_t(exp_bin) * 315653 - (shorter ? 131008 : 0)) >> 20;
	// NOTE: Similar methods are used in the Dragonbox paper Section 5.4, which
	//	has a different default parameter u = 22 instead of 20 above. The method
	//	above is actually better than the floating-point value used in the
	//	Dragonbox paper. However, u = 22 is insufficient for 'long double' (as
	//	it fails at 2937 and -3471). Same results to u = 23. As of u = 24, it
	//	still fails for -3471. The first u sufficient here is 26.
#else
	// XXX: For larger intervals, the parameter u should be better greater. For
	//	instance, u = 26 should work with all |e| <= 40703. Larger exponent
	//	needs more correction terms, e.g. IEC 60559 binary256 can use
	//	⌊(e⌊2^u ㏒₁₀2⌋ - ⌊2^u ㏒₁₀(4/3)⌋) / 2^u⌋ + e × 1.1e-10 for |e| < 262144.
	//	However, greater 'u' needs more bits in the integer used here, hence
	//	likely ineffieicent. (Similar in the discussion for 'Q' in Schubfach
	//	paper §9.1.1, but more conservative decision is made here.)
	return
		(std::int_fast64_t(exp_bin) * 20201781 - (shorter ? 8384497 : 0)) >> 26;
#endif
}

// NOTE: This function converts double number from binary to decimal components.
//	The output significand is shortest decimal but may have trailing zeros.
/*!
This function use the Schubfach algorithm:
Raffaello Giulietti, The Schubfach way to render doubles, 2020.
v2 2020-03-16
https://drive.google.com/open?id=1luHhyQF9zKlM8yJ1nebU0OgVYhfC6CBN
v3 2021-11-02
https://drive.google.com/file/d/1JoQBN5igZ8bMI3ua7l5DrHx_GYsjF0Dy
v4 2021-11-07
https://drive.google.com/file/d/1IEeATSVnEE6TkrHlCYNY2GjaraBjOT4f
A C++ port:
https://github.com/abolz/Drachennest

The referenced Schubfach paper in comments is v4.

See also:
Dragonbox: A New Floating-Point Binary-to-Decimal Conversion Algorithm, 2020.
https://github.com/jk-jeon/dragonbox/blob/master/other_files/Dragonbox.pdf

There are no dedicated tables for floating-point numbers larger than IEC 60559
	binary64 format. As a result, no precisie results are guaranteed at all.

\pre The input shall not consist a non-zero finite floating-point number.
\param shorter Indicate the lower bound is closer.
\param sig_bin The binary value of significand.
\param exp_bin The binary value of exponent.
\param sig_dec The output decimal value of significand.
\param exp_dec The output decimal value of exponent.
*/
template<typename _type>
void
FPBinaryToDecimal(bool shorter, typename fp_traits<_type>::carrier_type sig_bin,
	int exp_bin, typename fp_traits<_type>::carrier_type& sig_dec, int& exp_dec)
	ynothrow
{
	using traits = fp_traits<_type>;
	using carrier_type = typename traits::carrier_type;
	// NOTE: This algorithm requires extra 2 bits in %sig_bin to prevent
	//	overflow. Every conforming floating-point format shall meet the
	//	requirement, since the sign bit and the exponent bits shall be no less
	//	than 2 bits.
	static_assert(size_t(std::numeric_limits<_type>::digits + 2) <=
		ystdex::integer_width<carrier_type>::value,
		"Invalid carrier type found.");

	// NOTE: Ditto.
	YAssert(sig_bin >> (traits::value_bits - 2) == carrier_type(),
		"Invalid significand value found.");

	// NOTE: %shorter indicates the lower bound is closer, i.e. the "shorter
	//	interval case" in Dragonbox, or identically, the "irregular spacing" in
	//	Schubfach. The value of %exp_bin is in the interval [-1074, 971] for IEC
	//	60559 binary64.
	exp_dec = ComputeDecimalExponent(exp_bin, shorter);
	// NOTE: Now %exp_dec has the value 'k'. The expression '-exp_dec' now
	//	evaluates to '-k', i.e. the exponent value in decimal for the 10^(-k)
	//	value to be used later. The value stored in %exp_dec would be adjusted
	//	for the final output laterly on demand.

	carrier_type vb, lower, upper;

	// NOTE: See the comment in %FPBinaryToDecimal_GetOffset. The parameter 'q'
	//	is the value stored in %exp_bin here. Note the sum is greater than 0.
	FPBinaryToDecimal_1<_type>(shorter ? 1 : 2, -exp_dec,
		unsigned(exp_bin + FPBinaryToDecimal_GetOffset<_type>(exp_dec)),
		is_odd(sig_bin), sig_bin << 2, vb, lower, upper);
	sig_dec = vb >> 2;
	FPBinaryToDecimal_2(vb, upper, lower, sig_dec, exp_dec);
}

/*
NOTE: The current output uses the following NPLA flonum external format
	specification.

First of all, the output values shall guarantee they are round-trip convertible
	to the NPLAMath flonum literal syntax accepted by the reader. The precision
	of the numerical value is not guaranteed preserved currently, though. (And
	the precision of the flonum may not be preserved if insufficient precision
	specifiation is specified).
The written external representation is strings follows NPLAMath literal
	specification for special values (infinities and NaNs). Otherwise, the value
	is a floating-point number.
The output for a number is the the printed form of decimal fractional number of
	direct style or scientific notation, with an optional prefixed minus sign
	('-'). The sign exists if and only if it is not a value of a positive
	number. (Note '-0.0' is different to '0.0'). If the value is an integer, the
	output format is in the direct style of the decimal representation,
	otherwise the shorter representation of the two is chosen.
In either notations of the numerical output, trailing zeros are trimmed. In the
	direct style, one trailing zero is kept after the decimal point. Otherwise,
	all zeros are removed, as well as the decimal point if it is the last
	character in the fraction part of the trimmed result.
The exponent letter can be specified. There is no check to guarantee it
	compatible to the NPLA flonum literals.
Some corollaries:
The decimal point is always presented in the direct style of the number.
The numeric format is same to ECMA-262 specification to print floating point
	numbers, with the following changes:
1. The negative sign of '-0.0' is kept.
2. Intergers have trailing '.0', to keep it could be read as a flonum.
3. Alternative exponent letters (other than 'e') are supported.
*/

template<typename _type, yimpl(enable_if_uint_t<_type> = 0)>
YB_ATTR_nodiscard size_t
RoundSignificand(size_t digits, size_t prec, _type& sig_dec) ynothrowv
{
	if(digits > prec)
	{
		// NOTE: Force rounding as the specified precision.
		const auto drop(digits - prec);
		const auto pow10u(upow(_type(10), drop));

		// XXX: Do not rely on %std::numeric_limits.
		YAssert(sig_dec <= _type() - 1 - (pow10u >> 2),
			"Unexpected overflow detected.");
		sig_dec += pow10u >> 1;
		sig_dec /= pow10u;
		return drop;
	}
	return 0;
}
#if !NPL_Impl_NPLAMath_LongDoubleAsDouble
#	if !NPL_Impl_NPLAMath_has_uint128
YB_ATTR_nodiscard size_t
RoundSignificand(size_t digits, size_t prec, uint128_t& sig_dec) ynothrowv
{
	if(digits > prec)
	{
		const auto drop(digits - prec);
		const auto pow10u(upow(uint128_t(10), drop));

		YAssert(sig_dec <= uint128_t() - 1 - (pow10u >> 2),
			"Unexpected overflow detected.");
		sig_dec += pow10u >> 1;
		{
			auto n(drop);

			while(n > 1)
			{
				sig_dec = u128_div_25(sig_dec) >> 2;
				n -= 2;
			}
			if(n != 0)
				sig_dec = udiv_10(sig_dec);
		}
		return drop;
	}
	return 0;
}
#	endif
#endif

template<size_t _vLen, typename _type>
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) char*
WriteFPInteger(char* buf, _type val)
{
	buf = WriteDecimalDigitsIn<_vLen>(buf, val);
	yunseq(buf[0] = '.', buf[1] = '0');
	return buf + 2;
}

template<typename _type, yimpl(enable_if_uint_t<_type> = 0)>
YB_ATTR_nodiscard YB_STATELESS int
CountSignificandDigits(_type sig_dec) ynothrow
{
	// XXX: GCC and Clang builtins like %__builtin_log10 seem not able to
	//	produce better binary code than %std ones.
	// XXX: Assume the range of %floor is enough for %uint128_t significand
	//	produced by the supported types. Since the guaranteed minimum value of
	//	%FLT_MAX is only 1E+37 in ISO C++ (following ISO C), This may not
	//	applicable for arbitrary %uint128_t.
	// NOTE: There are known optimizations, but this is not used for the cost of
	//	tables and the table lookup is also not that optimized (esp. for
	//	%uint128_t). See https://lemire.me/blog/2021/06/03/computing-the-number-of-digits-of-an-integer-even-faster/
	//	for the table approach.
	// TODO: Adopt the table approach which seems still worthy for
	//	%std::uint32_t.
#if LDBL_MANT_DIG <= 113
	return int(std::floor(std::log10(float(sig_dec))));
#else
#	error "Not implemented."
#endif
}

template<typename _type>
using CarrierArgOf = ystdex::cond_t<std::is_integral<_type>,
	typename fp_traits<_type>::carrier_type,
	const typename fp_traits<_type>::carrier_type&>;

//! \ingrouop functors
//@{
template<typename _type>
struct FPWriterTraits
{
	YB_ATTR_nodiscard YB_ATTR_always_inline YB_PURE static int
	CountNormalSignificandDigits(CarrierArgOf<_type> sig_dec) ynothrow
	{
		return CountSignificandDigits(sig_dec) + 1;
	}
};

// XXX: Optimize only for specific binary formats.
#if FLT_RADIX == 2
// NOTE: As per ISO C11 5.2.4.2.2/11, With b binary digits, the minmum and
//	maximum decimal digit numbers are ⌊(b - 1) ㏒₁₀2⌋ and ⌈b ㏒₁₀2⌉ + 1, which
//	should be the same to %digits10 and %max_digits10 in the %std::numeric_limit
//	instance.
#	if FLT_MANT_DIG == 24
template<>
struct FPWriterTraits<float>
{
	YB_ATTR_nodiscard YB_STATELESS static int
	CountNormalSignificandDigits(CarrierArgOf<float> sig_dec) ynothrowv
	{
		// NOTE: The length of the significand is 6 to 9.
		YAssert(sig_dec < 1000000000U, "Invalid significand found.");
		YAssert(sig_dec >= 100000U, "Invalid significand found.");

		int sig_len(std::numeric_limits<float>::max_digits10);

		sig_len -= sig_dec < 100000000U ? 1 : 0;
		sig_len -= sig_dec < 10000000U ? 1 : 0;
		sig_len -= sig_dec < 1000000U ? 1 : 0;
		return sig_len;
	}
};
#	endif

#	if DBL_MANT_DIG == 53
template<>
struct FPWriterTraits<double>
{
	YB_ATTR_nodiscard YB_STATELESS static int
	CountNormalSignificandDigits(CarrierArgOf<double> sig_dec) ynothrowv
	{
		// NOTE: The length of the significand is 15 to 17. See also the
		//	Schubfach paper paper §8.1 D6.
		YAssert(sig_dec < 100000000000000000U, "Invalid significand found.");
		YAssert(sig_dec >= 100000000000000U, "Invalid significand found.");

		int sig_len(std::numeric_limits<double>::max_digits10);

		sig_len -= sig_dec < 10000000000000000U ? 1 : 0;
		sig_len -= sig_dec < 1000000000000000U ? 1 : 0;
		return sig_len;
	}
};
#	endif

#	if NPL_Impl_NPLAMath_has_uint128
#		if LDBL_MANT_DIG == 64
template<>
struct FPWriterTraits<long double>
{
	YB_ATTR_nodiscard YB_PURE static int
	CountNormalSignificandDigits(CarrierArgOf<long double> sig_dec) ynothrowv
	{
		// NOTE: The length of the significand is 18 to 21.
		YAssert(sig_dec < uint128_t(100000000000U) * 10000000000U,
			"Invalid significand found.");
		YAssert(sig_dec >= 100000000000000000U, "Invalid significand found.");

		int sig_len(std::numeric_limits<long double>::max_digits10);

		sig_len -= sig_dec < uint128_t(10000000000U) * 10000000000U ? 1 : 0;
		sig_len -= sig_dec < 10000000000000000000U ? 1 : 0;
		sig_len -= sig_dec < 1000000000000000000U ? 1 : 0;
		return sig_len;
	}
};
#		elif LDBL_MANT_DIG == 113
template<>
struct FPWriterTraits<long double>
{
	YB_ATTR_nodiscard YB_PURE static int
	CountNormalSignificandDigits(CarrierArgOf<long double> sig_dec) ynothrowv
	{
		// NOTE: The length of the significand is 33 to 36.
		YAssert(sig_dec < uint128_t(1000000000000000000U)
			* 1000000000000000000U, "Invalid significand found.");
		YAssert(sig_dec >= uint128_t(10000000000000000U) * 10000000000000000U,
			"Invalid significand found.");

		int sig_len(std::numeric_limits<long double>::max_digits10);

		sig_len -= sig_dec < uint128_t(1000000000000000000U)
			* 100000000000000000U ? 1 : 0;
		sig_len -= sig_dec < uint128_t(1000000000000000000U)
			* 10000000000000000U ? 1 : 0;
		sig_len -= sig_dec < uint128_t(1000000000000000000U)
			* 1000000000000000U ? 1 : 0;
		return sig_len;
	}
};
#		endif
#	endif
#endif

#if NPL_Impl_NPLAMath_LongDoubleAsDouble \
	|| (FLT_RADIX == 2 && LDBL_MANT_DIG == 53)
template<>
struct FPWriterTraits<long double> : FPWriterTraits<double>
{};
#endif
//@}

//! \pre 已输出的字符数不为零。
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) inline char*
TrimTrailingZeros(char* buf) ynothrowv
{
	YAssertNonnull(buf);
	while(*--buf == '0')
		;
	return ++buf;
}

// XXX: Variable %sig_dic is reference to allow the value modified in an
//	argument to a call to %WriteFraction, for ease of rounding, Otherwise, the
//	modification on the prvalue to intialize %sig_dec converted from the object
//	may cause undefined behavior.
template<typename _type>
YB_ATTR_returns_nonnull YB_NONNULL(1) inline char*
WriteFraction(char* buf, const typename fp_traits<_type>::carrier_type& sig_dec,
	int len) ynothrowv
{
	YAssert(len > 0, "Invalid length found.");
	YAssert(sig_dec != typename fp_traits<double>::carrier_type(),
		"Invalid zero fraction found.");
#if FLT_MANT_DIG <= 24
	YAssert(len <= 9, "Invalid length found.");
	yunused(len);
	return TrimTrailingZeros(WriteDecimalDigitsIn<9>(buf, sig_dec));
#elif FLT_MANT_DIG <= 53
	YAssert(len <= 17, "Invalid length found.");
	return len < 15 ? TrimTrailingZeros(WriteDecimalDigitsIn<15>(buf, sig_dec))
		: WriteDecimal15To17Trimmed(buf, sig_dec);
#else
#	error "Unimplemented."
#endif
}
template<>
YB_ATTR_returns_nonnull YB_NONNULL(1) inline char*
WriteFraction<double>(char* buf, const typename fp_traits<double>::carrier_type&
	sig_dec, int len) ynothrowv
{
	YAssert(len > 0, "Invalid length found.");
	YAssert(len <= 17, "Invalid length found.");
	YAssert(sig_dec != typename fp_traits<double>::carrier_type(),
		"Invalid zero fraction found.");
#if DBL_MANT_DIG <= 53
	return len < 15 ? TrimTrailingZeros(WriteDecimalDigitsIn<15>(buf, sig_dec))
		: WriteDecimal15To17Trimmed(buf, sig_dec);
#else
#	error "Unimplemented."
#endif
}
#if !NPL_Impl_NPLAMath_LongDoubleAsDouble
template<>
YB_ATTR_returns_nonnull YB_NONNULL(1) inline char*
WriteFraction<long double>(char* buf, const
	typename fp_traits<long double>::carrier_type& sig_dec, int len) ynothrowv
{
	YAssert(len > 0, "Invalid length found.");
	YAssert(sig_dec != typename fp_traits<double>::carrier_type(),
		"Invalid zero fraction found.");
	yunused(len);
	return TrimTrailingZeros(WriteU128Trailing(buf, to_uint128(sig_dec)));
}
#endif

template<typename _type>
YB_ATTR_returns_nonnull YB_NONNULL(1) inline char*
WriteFractionRounded(char* buf, const typename fp_traits<_type>::carrier_type&
	sig_dec, int sig_len, int drop) ynothrowv
{
	// XXX:%FPWriterTraits<_type>::CountNormalSignificandDigits is not fit here
	//	because the rounded value may be smaller than the lower bound of the
	//	expected length.
	const auto len(drop == 0 ? sig_len : CountSignificandDigits(sig_dec) + 1);

	return WriteFraction<_type>(buf + sig_len - drop - len, sig_dec, len);
}

// XXX: Assume the decomposed values can be stored in the correspoinding integer
//	types.
template<typename _type>
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) char*
WriteFPNormalCommon(char* buf, typename fp_traits<_type>::carrier_type sig_bin,
	int exp_bin, char e, size_t prec, std::uint_fast8_t left_max,
	std::uint_fast8_t right_max)
{
	using limits = std::numeric_limits<_type>;
	using carrier_type = typename fp_traits<_type>::carrier_type;

	// NOTE: This is the fast path for small integer numbers without fraction.
	if(-limits::digits < exp_bin && exp_bin <= 0
		&& countr_zero_narrow(sig_bin) + exp_bin >= 0)
		// NOTE: The value is an integer in the interval [1, 2^(p - 1)], where
		//	'p' is the maximum binary digits of the significand (the precision).
		//	When the significand bits are not greater than ⌊㏒₂(10^16)⌋ = 54,
		//	the maximum length is 16.
		// NOTE: Write an unsigned integer with a length of 1 to 16.
		return WriteFPInteger<limits::digits <= 54 ? 16 : (limits::digits
			<= 64 ? 19 : 36)>(buf, sig_bin >> size_t(-exp_bin));

	int exp_dec;
	carrier_type sig_dec;

	FPBinaryToDecimal<_type>(sig_bin == carrier_type(1) << (limits::digits - 1)
		&& exp_bin > limits::min_exponent - limits::digits, sig_bin, exp_bin,
		sig_dec, exp_dec);

	const int
		sig_len(FPWriterTraits<_type>::CountNormalSignificandDigits(sig_dec));
	// NOTE: This is the decimal point position relative to the first digit.
	int dpos(sig_len + exp_dec);
	// NOTE: Do not modify %sig_len during the rounding since %exp_dec is to be
	//	used (if any) together with the original value. This may change
	//	%sig_dec.
	const auto sig_round([&](size_t digits) ynothrowv{
		return int(RoundSignificand(digits, prec, sig_dec));
	});

	if(-left_max < dpos && dpos <= right_max)
	{
		// NOTE: The exponent part is not needed. Here are '-exp_dec' precise
		//	digits after the decimal point.
		if(dpos <= 0)
		{
			// NOTE: The decimal point is before any non-zero digit.
			yunseq(buf[0] = '0', buf[1] = '.');
			buf += 2;
			ystdex::trivially_fill_n(buf, size_t(-dpos), ystdex::byte('0'));
			return WriteFractionRounded<_type>(buf - dpos,
				sig_dec, sig_len, sig_round(size_t(-exp_dec)));
		}
		// NOTE: The decimal point is after some non-zero digits.
		ystdex::trivially_fill_n(buf, 24, ystdex::byte('0'));

		const auto end(WriteFractionRounded<_type>(buf + 1, sig_dec, sig_len,
			exp_dec >= 0 ? 0 : sig_round(size_t(-exp_dec))));

		for(size_t i(0); i < size_t(dpos); ++i)
			buf[i] = buf[i + 1];
		buf[dpos] = '.';
		return std::max(buf + dpos + 2, end);
	}

	// NOTE: Write the number value with scientific notation.
	auto end(WriteFractionRounded<_type>(buf + 1, sig_dec, sig_len,
		sig_round(size_t(sig_len) - 1)));

	exp_dec += sig_len - 1;
	buf[0] = buf[1];
	*++buf = '.';
	// NOTE: Do not keep the trailing '.' before the exponent part. This
	//	is consistent to subnormal numbers.
	end -= ++buf == end ? 1 : 0;
	end[0] = e;
	++end;
	end[0] = exp_bin >= 0 ? '+' : '-';
	return WriteFPExponentAbs<_type>(++end,
		unsigned(exp_bin >= 0 ? exp_dec : -exp_dec));
}

YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) char*
WriteFPSpecial(char* buf, int fc, char e) ynothrowv
{
	if(fc == FP_INFINITE)
		yunseq(buf[0] = 'i', buf[1] = 'n', buf[2] = 'f');
	else
		yunseq(buf[0] = 'n', buf[1] = 'a', buf[2] = 'n');
	buf[3] = '.';
	switch(e)
	{
	case 'f':
		buf[4] = 'f';
		break;
	case 'l':
		buf[4] = 't';
		break;
	default:
		buf[4] = '0';
	}
	return buf + 5;
}

YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) char*
WriteFPZero(char* buf) ynothrowv
{
	yunseq(buf[0] = '0', buf[1] = '.', buf[2] = '0');
	return buf + 3;
}

template<typename _type>
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) char*
WriteFPNormal(char* buf, _type val, char e, size_t prec,
	std::uint_fast8_t left_max, std::uint_fast8_t right_max)
{
	// XXX: %std::ilogb is generally inefficient for checking of the special
	//	values already excluded here.
	const auto
		exp_bin(int(std::logb(val)) - std::numeric_limits<_type>::digits + 1);

	return WriteFPNormalCommon<_type>(buf, fp_to_int<typename
		fp_traits<_type>::carrier_type>(std::ldexp(val, -exp_bin)),
		exp_bin, e, prec, left_max, right_max);
}
#if NPL_Impl_NPLAMath_LongDoubleAsDouble
template<>
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) inline char*
WriteFPNormal(char* buf, long double val, char e, size_t prec,
	std::uint_fast8_t left_max, std::uint_fast8_t right_max)
{
	return WriteFPNormal(buf, double(val), e, prec, left_max, right_max);
}
#endif

#if NPL_Impl_NPLAMath_HasSubnorm
YB_ATTR_nodiscard YB_NONNULL(1) char*
WriteFPSubnormalFraction(char* buf, std::uint32_t val) ynothrowv
{
	return WriteDecimalDigitsIn<9>(buf, val);
}
YB_ATTR_nodiscard YB_NONNULL(1) char*
WriteFPSubnormalFraction(char* buf, std::uint64_t val) ynothrowv
{
	return WriteDecimalDigitsIn<17>(buf, val);
}
#	if !NPL_Impl_NPLAMath_LongDoubleAsDouble
YB_ATTR_nodiscard YB_NONNULL(1) char*
WriteFPSubnormalFraction(char* buf, uint128_t val) ynothrowv
{
	return WriteU128Trailing(buf, val);
}
#	endif

template<typename _type>
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) char*
WriteFPSubnormal(char* buf, _type val, char e, size_t prec) ynothrowv
{
	using carrier_type = typename fp_traits<_type>::carrier_type;
	const auto exp_bin(std::numeric_limits<_type>::min_exponent
		- std::numeric_limits<_type>::digits);
	int exp_dec;
	carrier_type sig_dec;

	FPBinaryToDecimal<_type>({}, fp_to_int<carrier_type>(
		std::ldexp(val, -exp_bin)), exp_bin, sig_dec, exp_dec);

	const auto digits(size_t(CountSignificandDigits(sig_dec)));

	exp_dec += digits > prec ? int(RoundSignificand(digits, prec, sig_dec)) : 0;

	auto hdr(buf);

	buf = WriteFPSubnormalFraction(buf + 1, sig_dec);
	hdr[0] = hdr[1];
	hdr[1] = '.';
	while(*--buf == '0')
		++exp_dec;
	exp_dec += buf - hdr - 1;
	buf += *buf != '.' ? 1 : 0;
	buf[0] = e;
	++buf;
	buf[0] = '-';
	return WriteFPExponentSubnormalAbs<_type>(++buf, unsigned(-exp_dec));
}
#	if NPL_Impl_NPLAMath_LongDoubleAsDouble
template<>
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) inline char*
WriteFPSubnormal(char* buf, long double val, char e, size_t prec) ynothrowv
{
	return WriteFPSubnormal(buf, double(val), e, prec);
}
#	endif
#endif
//@}

} // unnamed namespace

bool
IsExactValue(const ValueObject& vo) ynothrow
{
	// TODO: Add rationals.
	return IsFixnumValue(vo) || false;
}

bool
IsInexactValue(const ValueObject& vo) ynothrow
{
	// NOTE: This is the complement of %IsFixnumValue in the type universe numbers.
	// NOTE: Every flonum is inexact. Currently there are no types other than
	//	flonums supported as inexact reals. There is currently no complex
	//	numbers support, hence, also no other inexact numbers exist.
	return IsFlonumValue(vo);
}

bool
IsFixnumValue(const ValueObject& vo) ynothrow
{
	return IsTyped<int>(vo) || IsTyped<unsigned>(vo) || IsTyped<long long>(vo)
		|| IsTyped<unsigned long long>(vo) || IsTyped<long>(vo)
		|| IsTyped<unsigned long>(vo) || IsTyped<short>(vo)
		|| IsTyped<unsigned short>(vo) || IsTyped<signed char>(vo)
		|| IsTyped<unsigned char>(vo);
}

bool
IsFlonumValue(const ValueObject& vo) ynothrow
{
	return
		IsTyped<double>(vo) || IsTyped<float>(vo) || IsTyped<long double>(vo);
}

bool
IsRationalValue(const ValueObject& vo) ynothrow
{
	if(const auto p_d = NPL::TryAccessValue<double>(vo))
		return std::isfinite(*p_d);
	if(const auto p_f = NPL::TryAccessValue<float>(vo))
		return std::isfinite(*p_f);
	if(const auto p_ld = NPL::TryAccessValue<long double>(vo))
		return std::isfinite(*p_ld);
	return IsExactValue(vo);
}

bool
IsIntegerValue(const ValueObject& vo) ynothrow
{
	if(const auto p_d = NPL::TryAccessValue<double>(vo))
		return FloatIsInteger(*p_d);
	if(const auto p_f = NPL::TryAccessValue<float>(vo))
		return FloatIsInteger(*p_f);
	if(const auto p_ld = NPL::TryAccessValue<long double>(vo))
		return FloatIsInteger(*p_ld);
	return IsExactValue(vo);
}


bool
IsFinite(const ValueObject& x) ynothrowv
{
	if(const auto p_d = NPL::TryAccessValue<double>(x))
		return std::isfinite(*p_d);
	if(const auto p_f = NPL::TryAccessValue<float>(x))
		return std::isfinite(*p_f);
	if(const auto p_ld = NPL::TryAccessValue<long double>(x))
		return std::isfinite(*p_ld);
	return true;
}

bool
IsInfinite(const ValueObject& x) ynothrowv
{
	if(const auto p_d = NPL::TryAccessValue<double>(x))
		return std::isinf(*p_d);
	if(const auto p_f = NPL::TryAccessValue<float>(x))
		return std::isinf(*p_f);
	if(const auto p_ld = NPL::TryAccessValue<long double>(x))
		return std::isinf(*p_ld);
	return {};
}

bool
IsNaN(const ValueObject& x) ynothrowv
{
	if(const auto p_d = NPL::TryAccessValue<double>(x))
		return std::isnan(*p_d);
	if(const auto p_f = NPL::TryAccessValue<float>(x))
		return std::isnan(*p_f);
	if(const auto p_ld = NPL::TryAccessValue<long double>(x))
		return std::isnan(*p_ld);
	return {};
}


bool
Equal(const ValueObject& x, const ValueObject& y) ynothrowv
{
	return NumBinaryComp<GBCompare<ystdex::equal_to<>>>(x, y);
}

bool
Less(const ValueObject& x, const ValueObject& y) ynothrowv
{
	return NumBinaryComp<GBCompare<ystdex::less<>>>(x, y);
}

bool
Greater(const ValueObject& x, const ValueObject& y) ynothrowv
{
	return NumBinaryComp<GBCompare<ystdex::greater<>>>(x, y);
}

bool
LessEqual(const ValueObject& x, const ValueObject& y) ynothrowv
{
	return NumBinaryComp<GBCompare<ystdex::less_equal<>>>(x, y);
}

bool
GreaterEqual(const ValueObject& x, const ValueObject& y) ynothrowv
{
	return NumBinaryComp<GBCompare<ystdex::greater_equal<>>>(x, y);
}


bool
IsZero(const ValueObject& x) ynothrowv
{
	return DoNumLeaf<bool>(x, EqZero());
}

bool
IsPositive(const ValueObject& x) ynothrowv
{
	return DoNumLeaf<bool>(x, Positive());
}

bool
IsNegative(const ValueObject& x) ynothrowv
{
	return DoNumLeaf<bool>(x, Negative());
}

bool
IsOdd(const ValueObject& x) ynothrowv
{
	return DoNumLeaf<bool>(x, Odd());
}

bool
IsEven(const ValueObject& x) ynothrowv
{
	return DoNumLeaf<bool>(x, Even());
}


ValueObject
Max(ResolvedArg<>&& x, ResolvedArg<>&& y)
{
	return NumBinaryOp<BMax>(x, y);
}

ValueObject
Min(ResolvedArg<>&& x, ResolvedArg<>&& y)
{
	return NumBinaryOp<BMin>(x, y);
}

ValueObject
Add1(ResolvedArg<>&& x)
{
	auto res(MoveUnary(x));

	DoNumLeaf<void>(res, AddOne(res));
	return res;
}

ValueObject
Sub1(ResolvedArg<>&& x)
{
	auto res(MoveUnary(x));

	DoNumLeaf<void>(res, SubOne(res));
	return res;
}

ValueObject
Plus(ResolvedArg<>&& x, ResolvedArg<>&& y)
{
	return NumBinaryOp<BPlus>(x, y);
}

ValueObject
Minus(ResolvedArg<>&& x, ResolvedArg<>&& y)
{
	return NumBinaryOp<BMinus>(x, y);
}

ValueObject
Multiplies(ResolvedArg<>&& x, ResolvedArg<>&& y)
{
	return NumBinaryOp<BMultiplies>(x, y);
}

ValueObject
Divides(ResolvedArg<>&& x, ResolvedArg<>&& y)
{
	return NumBinaryOp<BDivides>(x, y);
}

ValueObject
Abs(ResolvedArg<>&& x)
{
	auto res(MoveUnary(x));

	DoNumLeaf<void>(res, ReplaceAbs(res));
	return res;
}


void
ReadDecimal(ValueObject& vo, string_view id, string_view::const_iterator first)
{
	// NOTE: These types are fixed to ensure values only different by signs
	//	would be likely of the same type. Otherwise, if non-negative values are
	//	unsigned types, operations like minus would result in different types
	//	depending on types, and the type of negative values computed from
	//	unsinged values by these operations would be promoted too quick.
	YAssert(!id.empty(), "Invalid lexeme found.");
	YAssert(first >= id.begin() && size_t(first - id.begin()) < id.size(),
		"Invalid first iterator found.");

	// NOTE: Skip leading zeros.
	while(YB_UNLIKELY(*first == '0'))
		if(first + 1 != id.end())
			++first;
		else
			break;
	yconstexpr_if(std::numeric_limits<ReadIntType>::max()
		< std::numeric_limits<ReadExtIntType>::max())
	{
		ReadIntType ans(0);

		if(YB_UNLIKELY(ReadDecimalExact(vo, id, first, ans)))
		{
			ReadExtIntType lans(ans);

			if(YB_UNLIKELY(ReadDecimalExact(vo, id, first, lans)))
				// NOTE: The cast is safe even when %ReadCommonType is unsigned
				//	because %ReadDecimalExact may only add the minus sign on a
				//	complete parse.
				// TODO: Use a bigint instead of the inexact number?
				ReadDecimalInexact(vo, first, id, ReadCommonType(lans),
					id.end());
		}
	}
	else
	{
		ReadIntType ans(0);

		if(YB_UNLIKELY(ReadDecimalExact(vo, id, first, ans)))
			// NOTE: Ditto.
			ReadDecimalInexact(vo, first, id, ReadCommonType(ans), id.end());
	}
}

//! \since build 932
//@{
template<typename _type>
YB_NONNULL(1) char*
WriteFPString(char* buf, _type val, char e, size_t prec, std::uint_fast8_t left_max,
	std::uint_fast8_t right_max) ynothrowv
{
	static_assert(std::is_floating_point<_type>(), "Invalid type found.");

	// XXX: Actually less buffer bytes are used. However, keep it to reserve for
	//	the future ues.
#if __has_builtin(__builtin_object_size)
	YAssert(__builtin_object_size(buf, 0) >= StringBufferSize,
		"Insufficient buffer found.");
#endif

	const bool sign(std::signbit(val));
	const auto fc(std::fpclassify(val));

	buf[0] = sign ? '-' : '+';
	buf += sign || fc == FP_INFINITE || fc == FP_NAN ? 1 : 0;
	switch(YB_EXPECT(fc, FP_NORMAL))
	{
	// NOTE: This requires 6 characters in NPLAMath.
	case FP_INFINITE:
	case FP_NAN:
		return WriteFPSpecial(buf, fc, e);
	// NOTE: This requires 4 characters in NPLAMath.
	case FP_ZERO:
		return WriteFPZero(buf);
	// NOTE: Otherwise, this requires H + L + 3 characters, for at most H digits
	//	of the significand and L digits of the exponent. This may vary, but 64
	//	is assumed large enough. (Example: for IEC 60559 binary64 format, H = 17
	//	and L = 3.) H is defined in the Schubfach paper §8.1 D6.
#if NPL_Impl_NPLAMath_HasSubnorm
	case FP_SUBNORMAL:
		return WriteFPSubnormal(buf, val, e, prec);
#endif
	default:
		return WriteFPNormal(buf, val, e, prec, left_max, right_max);
	}
}

template char*
WriteFPString(char*, float, char, size_t, std::uint_fast8_t, std::uint_fast8_t)
	ynothrowv;
template char*
WriteFPString(char*, double, char, size_t, std::uint_fast8_t, std::uint_fast8_t)
	ynothrowv;
template char*
WriteFPString(char*, long double, char, size_t, std::uint_fast8_t,
	std::uint_fast8_t) ynothrowv;

// XXX: Precision specification is needed to shrink the value e.g. IEC 60559
//	binary64 1.2, 1.2e-20 and 1.2e-308 (otherwise, there would be typically
//	trailing digits, like 1.2000000000000002, 1.1999999999999998e-20 and
//	1.2000000000000003e-308). Note some value is not precisely rounded in the
//	current setting without a table, e.g. binary64 1e-7 is in
//	9.999999999999996e-8 without internal exact 64-bit arithmetic (as SSE
//	instead of x87) and %NPL_Impl_NPLAMath_UseQuadMath, or with less bised but
//	still imprecise results (probably correctly rounded, e.g. for 1e-6 in x86).
//	This is tolerated.
string
FPToString(float x, string::allocator_type a)
{
	char buf[StringBufferSize];

	return string(&buf[0], WriteFPString(buf, x, 'f'), a);
}
string
FPToString(double x, string::allocator_type a)
{
	char buf[StringBufferSize];

	return string(&buf[0], WriteFPString(buf, x, 'e'), a);
}
string
FPToString(long double x, string::allocator_type a)
{
	char buf[StringBufferSize];

	return string(&buf[0], WriteFPString(buf, x, 'l'), a);
}

#undef NPL_Impl_NPLAMath_PrintLargeSignificant
#undef NPL_Impl_NPLAMath_HasSubnorm
#undef NPL_Impl_NPLAMath_IEC_60559_BFP
//@}

} // inline namespace Math;

} // namespace NPL;

#undef NPL_Impl_NPLAMath_has_umul128
#undef NPL_Impl_NPLAMath_LongDoubleAsDouble

