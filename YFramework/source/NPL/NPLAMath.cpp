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
\version r24396
\author FrankHB <frankhb1989@gmail.com>
\since build 930
\par 创建时间:
	2021-11-03 12:50:49 +0800
\par 修改时间:
	2021-11-11 12:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLAMath
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLAMath // for size_t, InvalidSyntax, ReductionStatus;
#include "NPLA1Internals.h" // for A1::Internals API;
#include <cmath> // for std::isfinite, std::isinf, std::isnan, std::floor,
//	std::fmod, std::abs, std::pow;
#include <ystdex/cstdint.hpp> // for std::numeric_limits, ystdex::cond_t,
//	std::is_signed, ystdex::identity, ystdex::conditional_t, ystdex::_t,
//	std::is_floating_point, ystdex::and_, std::is_unsigned,
//	ystdex::make_widen_int;
#include <ystdex/functional.hpp> // for ystdex::retry_on_cond, ystdex::equal_to,
//	ystdex::less, ystdex::greater, ystdex::less_equal, ystdex::greater_equal;

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


template<typename _type, typename _func, class _tValue>
YB_ATTR_nodiscard _type
DoNumLeaf(_tValue& x, _func f) ynothrowv
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

template<typename _type, typename _func, class... _tValue>
YB_ATTR_nodiscard _type
DoNumLeafHinted(NumCode code, _func f, _tValue&... xs) ynothrowv
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

template<typename _type>
YB_ATTR_nodiscard YB_PURE bool
FloatIsInteger(const _type& x)
{
#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Push
	YB_Diag_Ignore(float-equal)
#endif
	// XXX: This can be any one of %std::(round, floor, ceil). The function
	//	%std::floor is choosed becuase it is probably more efficient (if any)
	//	on usual implementations.
	return std::isfinite(x) && std::floor(x) == x;
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

	DynNumCast(NumCode code)
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


//! \since build 930
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
			if(y != _type(-1) || x != std::numeric_limits<_type>::min())
				return DoInt(x, y);
			if(std::numeric_limits<_type>::min()
				== std::numeric_limits<long long>::min())
				return -double(x);
			return -MakeExtType<_type>(x);
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
		if(x != std::numeric_limits<_type>::min())
			x = _type(std::abs(x));
		else
			Result = -MakeExtType<_type>(x);
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

	return size_t(xcode) >= size_t(ycode) ?
		ret_bin(x, Promote(xcode, y, ycode), xcode)
		: ret_bin(Promote(ycode, x, xcode), y, ycode);
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

void
ReadDecimalInexact(ValueObject& vo, string_view::const_iterator first,
	string_view id, double ans, size_t shift = 0)
{
	YAssert(!id.empty(), "Invalid lexeme found.");
	YAssert(shift <= id.size(), "Invalid pointer position found.");
	ystdex::retry_on_cond(ystdex::id<>(), [&]() -> bool{
		if(ystdex::isdigit(*first))
			ans = DecimalCarryAddDigit(ans, *first);
		else if(*first == '.')
		{
			if(shift == 0)
				shift = size_t(id.end() - first);
			else
				throw InvalidSyntax(ystdex::sfmt(
					"More than one decimal points found in '%s'.", id.data()));
		}
		else
			ThrowForInvalidLiteralSuffix(&*first, id.data());
		if(++first != id.end())
			return true;
		vo = (id[0] != '-' ? ans : -ans) * std::pow(10, shift);
		return {};
	});
}

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
		if(*first == '.')
		{
			ReadDecimalInexact(vo, first + 1, id, ans,
				size_t(id.end() - first));
			return ReductionStatus::Clean;
		}
		ThrowForInvalidLiteralSuffix(&*first, id.data());
	}) == ReductionStatus::Partial;
}
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
	// XXX: Keeping the type of numeric literals without sign 'int' is also more
	//	compatible to old operations. 
	using int_type = int;
	using ext_int_type = long long;
	YAssert(!id.empty(), "Invalid lexeme found.");
	YAssert(first >= id.begin() && size_t(first - id.begin()) < id.size(),
		"Invalid first iterator found.");

	// NOTE: Skip leading zeros.
	while(YB_UNLIKELY(*first == '0'))
		if(YB_UNLIKELY(++first == id.end()))
			break;
	yconstexpr_if(std::numeric_limits<int_type>::max()
		!= std::numeric_limits<ext_int_type>::max())
	{
		int_type ans(0);

		if(YB_UNLIKELY(ReadDecimalExact(vo, id, first, ans)))
		{
			ext_int_type lans(ans);

			if(YB_UNLIKELY(ReadDecimalExact(vo, id, first, lans)))
				ReadDecimalInexact(vo, first, id, lans);
		}
	}
	else
	{
		int_type ans(0);

		if(YB_UNLIKELY(ReadDecimalExact(vo, id, first, ans)))
			ReadDecimalInexact(vo, first, id, ans);
	}
}

} // inline namespace Math;

} // namespace NPL;

