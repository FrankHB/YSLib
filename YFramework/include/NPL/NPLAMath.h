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
\version r11307
\author FrankHB <frankhb1989@gmail.com>
\since build 930
\par 创建时间:
	2021-11-03 12:49:54 +0800
\par 修改时间:
	2021-12-15 22:22 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLAMath
*/


#ifndef NPL_INC_NPLAMath_h_
#define NPL_INC_NPLAMath_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA // for NPL, any_ops, ValueObject, ResolvedArg,
//	string_view, std::uint_fast8_t, TypedValueAccessor;
#include <limits> // for std::numeric_limits;

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


//! \since build 932
//@{
/*!
\brief 保留给默认数值输出使用的缓冲区字节大小。

缓冲区能保存 NPLAMath 数值字面量，包括浮点字面量的默认输出表示。
假定支持的浮点数使用科学记数法时，指数不超过 5 位十进制数字。
*/
yconstexpr const size_t StringBufferSize(yimpl(64));

static_assert((std::numeric_limits<long double>::max_digits10 + 5 + 3
	<= StringBufferSize), "The buffer size is not fit for the implementation");

/*!
\brief 写浮点数据到字符串缓冲区。
\pre 缓冲区应至少具有连续可读写的 StringBufferSize 字节。
\warning 当前不支持对不满足特定要求的 ISO C++ 实现，且不排除这些实现上的未定义行为。
\return 输出结束的指针。

输出和浮点数内部类型相关的经舍入的浮点数。
结果经有限次输出作为 NPLA 不精确数值字面量被解析再输出的往返转换后不损失精度；
	其中，零值和特殊值的任意一次转换是精确的。具体舍入方法和精度未指定。
对浮点数的转换结果的输出格式满足：
不依赖区域，没有分节符，小数点使用 . ；
除非使用科学记数法，对整数值总是保证结尾 .0 ；
以上情形外不保留小数点后结尾的连续的 0 ；
在可能移除结尾的 0 前，舍入时若值具有足够的小数点后的位数，保留至少 6 个小数位。
对浮点类型 T ，令 L = std::numeric_limits<T> ，则当前实现中，在移除结尾 0 前：
当且仅当值小于 1e-5 或具有超过 L::digits10 + 1 个十进制数字时，启用科学记数法；
使用小数点后的位数进行四舍五入，保留小数点后 L::digits10 个十进制数字。
浮点数转换时的内部表示使用足以满足上述要求的算法，
	但不保证浮点数二进制表示支持的完全精度。
对符合 IEC 60559 二进制格式的浮点数，补充适当的查找表可保证精确和高效实现：
	使用几个机器字位宽的整数乘法的开销保证满足完全精度。
但注意假定格式削弱 C++ 源代码意义上的可移植性，且完全精度没有在此被要求；
	因此，当前实现没有启用查找表。
当前实现的内部的二进制中间表示精度限于 long double 类型的有效数字位数；
	这通常对 float 以上不保证；
	但是，使用 IEC 60559 的格式仍可能相对其它格式更高效。
参数分别指定：
字符缓冲区的起始指针、浮点数据的值、浮点字母、数值在小数点后的保留的精度、
	使用科学记数法表示的小数点位置的下限（不含）和上限（含）的绝对值。
小数点位置由小数表示中消除结尾的 0 后，在小数点左侧存在的有效数字的个数定义：
	当小数点左侧存在非零数字，则小数点位置是正数；
	当小数点左侧不存在非零数字，则小数点位置是右侧补零数值的相反数。
当前实现假定：
	使用和 IEC 60559 兼容的浮点数据移码编码的格式；
	指数不超过区间 [-58810, 40704) 。
这较 ISO C++ 严格，但已支持一些不兼容 ISO 60559 的格式（虽然当前平台配置没有实例）：
	复用现有格式的 double-double 格式，如 GCC __ibm128 兼容的 long double ；
	符号位、指数和有效数字的表示不按 IEC 60559 顺序或不连续存储的格式。
指数的限制不支持 IEC 60559 binary256 ，但这仅被作为交换格式；
	也没有已知的 C++ 实现直接支持。
其它一些对浮点类型格式或者其它相关语言特性的限制可能直接不被构建支持。
对 ISO C++ 的其它实现限制和相关行为的约定参见 Documentation::YFramework 。
*/
//@{
template<typename _type>
YB_ATTR_returns_nonnull YB_NONNULL(1) char*
WriteFPString(char*, _type, char = 'e', size_t = std::numeric_limits<
	_type>::digits10, std::uint_fast8_t = 6, std::uint_fast8_t
	= std::numeric_limits<_type>::digits10) ynothrowv;
extern template YF_API YB_ATTR_returns_nonnull YB_NONNULL(1) char*
WriteFPString(char*, float, char, size_t, std::uint_fast8_t, std::uint_fast8_t)
	ynothrowv;
extern template YF_API YB_ATTR_returns_nonnull YB_NONNULL(1) char*
WriteFPString(char*, double, char, size_t, std::uint_fast8_t, std::uint_fast8_t)
	ynothrowv;
extern template YF_API YB_ATTR_returns_nonnull YB_NONNULL(1) char*
WriteFPString(char*, long double, char, size_t, std::uint_fast8_t,
	std::uint_fast8_t) ynothrowv;
//@}

/*!
\brief 转换浮点数为 NPLA flonum 外部格式的字符串。
\note 缓冲区使用第二参数指定的分配器创建。
\return 使用 WriteFPString 转换数据，具有第二参数指定的分配器的转换结果。
*/
//@{
YB_ATTR_nodiscard YF_API YB_PURE string
FPToString(float, string::allocator_type = {});
YB_ATTR_nodiscard YF_API YB_PURE string
FPToString(double, string::allocator_type = {});
YB_ATTR_nodiscard YF_API YB_PURE string
FPToString(long double, string::allocator_type = {});
//@}
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

