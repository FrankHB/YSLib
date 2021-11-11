/*
	© 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLAMath.h
\ingroup NPL
\brief NPLA 数学功能。
\version r11221
\author FrankHB <frankhb1989@gmail.com>
\since build 930
\par 创建时间:
	2021-11-03 12:49:54 +0800
\par 修改时间:
	2021-11-11 12:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLAMath
*/


#ifndef NPL_INC_NPLAMath_h_
#define NPL_INC_NPLAMath_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA // for NPL, any_ops, ValueObject, ResolvedArg,
//	string_view, TypedValueAccessor;

namespace NPL
{

/*!
\brief 数学功能支持。
\note 支持的数值类型和具体词法形式参考 Documentation::NPL 。
\since build 929

提供数值类型和相关操作。
*/
inline namespace Math
{

//! \since build 930
//@{
/*!
\brief 启用本机数值类型检查。

启用保证可原地存储的静态检查。
被检查的类型包含 fixnum 和 flonum 可被原地存储，同一些实现（如 Racket ）的典型假设。
检查预期以 YSLib::any_ops::is_in_place_storable 实现。
作为实现细节，兼容 ISO C++ 的实现不保证检查总是能被通过
	（虽然 ystdex 实现的 any_ops 可提供保证）。
检查在此默认被启用，因为已知的被支持的 YSLib 平台配置应允许检查通过。
否则，覆盖宏定义以禁用这项检查。
因为本实现不依赖具体假设保持正确性，不保证原地存储的这些类型不违反实现正确性要求。
但因为预期的性能问题，不满足可原地存储的 fixnum 和 flonum 很大程度上缺乏实用性。
一般地，这可能需要避免检查失败的（过大的）类型在本机实现中出现；
当前实现没有（以元编程方式等）提供这种选项，而需要用户使用替代实现。
*/
#ifndef NPL_NPLA_EnsureInPlaceNativeNumbers
#	define NPL_NPLA_EnsureInPlaceNativeNumbers true
#endif

#if NPL_NPLA_EnsureInPlaceNativeNumbers
// XXX: This still avoids %std::intmax_t and %std::uintmax_t even in practice
//	they may also work.
static_assert(any_ops::is_in_place_storable<long long>(),
	"Invalid native fixnum found.");
static_assert(any_ops::is_in_place_storable<long double>(),
	"Invalid native flonum found.");
#endif
//@}


//! \ingroup tags
//@{
//! \brief 数值叶节点值数据成员。
struct NumberLeaf
{};

//! \brief 数值项节点。
struct NumberNode
{};
//@}


/*!
\brief 判断参数表示精确数。

参考调用文法：
<pre>exact? \<number></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsExactValue(const ValueObject&) ynothrow;

/*!
\brief 判断参数表示不精确数。

参考调用文法：
<pre>inexact? \<number></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsInexactValue(const ValueObject&) ynothrow;

/*!
\brief 判断参数表示 fixnum 。

参考调用文法：
<pre>fixnum? \<number></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsFixnumValue(const ValueObject&) ynothrow;

/*!
\brief 判断参数表示 flonum 。

参考调用文法：
<pre>flonum? \<number></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsFlonumValue(const ValueObject&) ynothrow;

/*!
\brief 判断参数表示数值类型的值。

参考调用文法：
<pre>number? \<number></pre>
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsNumberValue, const ValueObject& vo) ynothrow
	ImplRet(IsExactValue(vo) || IsInexactValue(vo))

/*!
\brief 判断参数表示有理数数值。
\since build 930

参考调用文法：
<pre>rational? \<number></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsRationalValue(const ValueObject&) ynothrow;

/*!
\brief 判断参数表示整数数值。
\since build 930

参考调用文法：
<pre>integer? \<number></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsIntegerValue(const ValueObject&) ynothrow;


//! \pre 参数具有表示数值类型的值。
//@{
//! \since build 930
//@{
//! \note 同 Racket 但不同于 [R7RS] ，当前仅支持实数。
//@{
/*!
\brief 判断参数表示有限值。

参考调用文法：
<pre>finite? \<real></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsFinite(const ValueObject&) ynothrowv;

/*!
\brief 判断参数表示无限值。

参考调用文法：
<pre>infinite? \<real></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsInfinite(const ValueObject&) ynothrowv;

/*!
\brief 判断参数是否表示 NaN 值。

参考调用文法：
<pre>nan? \<real></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsNaN(const ValueObject&) ynothrowv;
//@}


/*!
\brief 比较相等。

参考调用文法：
<pre>=? \<number1> \<number2></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
Equal(const ValueObject&, const ValueObject&) ynothrowv;

/*!
\brief 比较小于。

参考调用文法：
<pre>\<? \<real1> \<real2></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
Less(const ValueObject&, const ValueObject&) ynothrowv;

/*!
\brief 比较大于。

参考调用文法：
<pre>\>? \<real1> \<real2></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
Greater(const ValueObject&, const ValueObject&) ynothrowv;

/*!
\brief 比较小于等于。

参考调用文法：
<pre>\<=? \<real1> \<real2></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
LessEqual(const ValueObject&, const ValueObject&) ynothrowv;

/*!
\brief 比较大于等于。

参考调用文法：
<pre>\>=? \<real1> \<real2></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
GreaterEqual(const ValueObject&, const ValueObject&) ynothrowv;


/*!
\brief 判断参数表示零值。
\note 使用准确的零值比较。非精确数和真值可因累积误差而使结果不可靠。
\note 浮点数 +0 和 -0 都是零值。

参考调用文法：
<pre>zero? \<number></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsZero(const ValueObject&) ynothrowv;

/*!
\brief 判断参数表示正数。

参考调用文法：
<pre>positive? \<real></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsPositive(const ValueObject&) ynothrowv;

/*!
\brief 判断参数表示负数。

参考调用文法：
<pre>negative? \<real></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsNegative(const ValueObject&) ynothrowv;

/*!
\brief 判断参数表示奇数。

参考调用文法：
<pre>positive? \<integer></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsOdd(const ValueObject&) ynothrowv;

/*!
\brief 判断参数表示偶数。

参考调用文法：
<pre>negative? \<integer></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsEven(const ValueObject&) ynothrowv;


/*!
\brief 计算最大值。

参考调用文法：
<pre>max \<number1> \<number2></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE ValueObject
Max(ResolvedArg<>&&, ResolvedArg<>&&);

/*!
\brief 计算最小值。

参考调用文法：
<pre>min \<number1> \<number2></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE ValueObject
Min(ResolvedArg<>&&, ResolvedArg<>&&);
//@}

/*!
\brief 计算参数加 1 。

参考调用文法：
<pre>add1 \<number></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE ValueObject
Add1(ResolvedArg<>&&);

/*!
\brief 计算参数减 1 。

参考调用文法：
<pre>sub1 \<number></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE ValueObject
Sub1(ResolvedArg<>&&);

/*!
\brief 加法。

参考调用文法：
<pre>+ \<number1> \<number2></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE ValueObject
Plus(ResolvedArg<>&&, ResolvedArg<>&&);

/*!
\brief 减法。

参考调用文法：
<pre>- \<number1> \<number2></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE ValueObject
Minus(ResolvedArg<>&&, ResolvedArg<>&&);

/*!
\brief 乘法。

参考调用文法：
<pre>* \<number1> \<number2></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE ValueObject
Multiplies(ResolvedArg<>&&, ResolvedArg<>&&);
//@}

//! \since build 930
//@{
/*!
\brief 除法。

参考调用文法：
<pre>/ \<number1> \<number2></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE ValueObject
Divides(ResolvedArg<>&&, ResolvedArg<>&&);

/*!
\brief 计算绝对值。
\note 当前实现同 Scheme 和 Kernel ，仅支持实数。

参考调用文法：
<pre>abs \<real></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE ValueObject
Abs(ResolvedArg<>&&);


/*!
\brief 读取指定位置的十进制数值表示。
\pre 第三参数是第二参数的迭代器。
\throw InvalidSyntax 数值表示不被支持。

第一参数指定保存结果的对象。
第二参数指定输入的数值表示的完整形式。
第三参数指定第二参数中开始解析的位置。
*/
void
ReadDecimal(ValueObject&, string_view, string_view::const_iterator);
//@}

} // inline namespace Math;

//! \since YSLib build 929
//@{
//! \relates NumberLeaf
template<>
struct TypedValueAccessor<NumberLeaf>
{
	template<class _tTerm>
	YB_ATTR_nodiscard YB_PURE inline auto
	operator()(_tTerm& term) const -> yimpl(decltype((term.Value)))
	{
		return NPL::ResolveTerm(
			[](_tTerm& nd, bool has_ref) -> yimpl(decltype((term.Value))){
			if(IsLeaf(nd))
			{
				if(IsNumberValue(nd.Value))
					return nd.Value;
				ThrowTypeErrorForInvalidType("number", nd, has_ref);
			}
			ThrowListTypeErrorForInvalidType("number", nd, has_ref);
		}, term);
	}
};

//! \relates NumberNode
template<>
struct TypedValueAccessor<NumberNode>
{
	template<class _tTerm>
	YB_ATTR_nodiscard YB_PURE inline ResolvedArg<>
	operator()(_tTerm& term) const
	{
		return NPL::ResolveTerm(
			[](_tTerm& nd, ResolvedTermReferencePtr p_ref) -> ResolvedArg<>{
			if(IsLeaf(nd))
			{
				if(IsNumberValue(nd.Value))
					return {nd, p_ref};
				ThrowTypeErrorForInvalidType("number", nd, p_ref);
			}
			ThrowListTypeErrorForInvalidType("number", nd, p_ref);
		}, term);
	}
};
//@}

} // namespace NPL;

#endif

