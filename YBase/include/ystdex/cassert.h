/*
	© 2012-2016, 2018-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cassert.h
\ingroup YStandardEx
\brief ISO C 断言/调试跟踪扩展。
\version r311
\author FrankHB <frankhb1989@gmail.com>
\since build 432
\par 创建时间:
	2013-07-27 04:11:53 +0800
\par 修改时间:
	2022-04-30 21:53 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CAssert
*/


#ifndef YB_INC_ystdex_cassert_h_
#define YB_INC_ystdex_cassert_h_ 1

#include "../ydef.h"
#include <cstdio> // for std::FILE;
#include <cstdint> // for std::uint8_t;

namespace ystdex
{

/*!
\brief YBase 默认断言函数。
\note 当定义宏 YB_Use_YAssert 不等于 0 时，宏 YAssert 操作由此函数实现。
\note 参数依次为：表达式、文件名、行号和消息文本。
\note 允许空指针参数，视为未知。
\note 调用 std::terminate 终止程序。
\since build 642
*/
YB_NORETURN YB_API void
yassert(const char*, const char*, int, const char*) ynothrow;

#if YB_Use_YTrace
/*!
\brief YBase 追踪记录函数。
\note 当定义宏 YB_Use_YTrace 非 false 时，宏 YTrace 操作由此函数实现。
\since build 553
*/
YB_API YB_ATTR_gnu_printf(6, 7) void
ytrace(std::FILE*, std::uint8_t, std::uint8_t, const char*, int, const char*,
	...) ynothrow;
#endif

} // namespace ystdex;

#endif // NOTE: Allow multiple for macro definitions.

#include <cassert> // for <cassert>, assert;

#undef yconstraint
#undef yassume

/*!
\ingroup YBase_pseudo_keyword
\note 类型为 void ，以避免重载 operator, 改变表达式的求值。
*/
//@{
/*!
\note 未指定表达式是否被求值。
\note 一般应避免表达式求值的副作用。预期的求值性质和 YB_ASSUME 的表达式相同。
\sa YB_ASSUME
\since build 535
*/
//@{
/*!
\def yassume
\brief 假定：环境语义。
\note 和普通断言相比强调非接口契约。

运行时检查的环境条件约束断言。
用于需要假设表达式求值结果为真但明确不通过接口直接约束的情形。
*/
#ifdef NDEBUG
#	define yassume(_expr) (YB_ASSUME(_expr), void())
#else
#	define yassume(_expr) (assert(_expr), void())
#endif

/*!
\def yconstraint
\brief 约束：接口语义。
\note 和普通断言相比强调接口契约。
\note 使用常量表达式在 ISO C++11 constexpr 模板中合式。
\see $2015-10 @ %Documentation::Workflow.
\see https://reviews.llvm.org/rL204776 。

可能在运行时检查的接口语义约束断言。
不满足此断言的行为是接口明确地未定义的，行为不可预测。
*/
#ifdef NDEBUG
#	define yconstraint(_expr) (YB_ASSUME(_expr), void())
#elif YB_IMPL_CLANGPP >= 30401
#	define yconstraint(_expr) \
	((YB_Diag_Push YB_Diag_Ignore(pointer-bool-conversion) _expr \
		YB_Diag_Pop) ? void() \
		: ystdex::yassert(#_expr, __FILE__, __LINE__, "Constraint violation."))
#else
#	define yconstraint(_expr) \
	((_expr) ? void() : ystdex::yassert(#_expr, __FILE__, __LINE__, \
		"Constraint violation."))
#endif
//@}

/*!
\def yverify
\brief 验证：执行语义。
\note 和普通断言相比强调执行中状态。
\since build 864

运行时检查的验证状态语义约束断言。
同标准库的宏 assert ，定义 NDEBUG 时表达式不被求值。
*/
#ifdef NDEBUG
#	define yverify(_expr) void()
#else
#	define yverify(_expr) \
	((_expr) ? void() : ystdex::yassert(#_expr, __FILE__, __LINE__, \
		"Verification failure."))
#endif
//@}


// NOTE: As %assert, this may be undefined and redefined.
#ifndef YAssert
#	if YB_Use_YAssert
#		define YAssert(_expr, _msg) \
	((_expr) ? void(0) : ystdex::yassert(#_expr, __FILE__, __LINE__, _msg))
#	else
// XXX: This does not use %yassume to prevent %_expr being evaluated.
#		define YAssert(_expr, _msg) assert(_expr)
#	endif
#endif

// NOTE: As %assert, this may be undefined and redefined.
#ifndef YAssertNonnull
//! \since build 495
#	define YAssertNonnull(_expr) YAssert(_expr, "Null reference found.")
#endif

#ifndef YTrace
//! \since build 432
//@{
/*!
\brief YCLib 扩展调试跟踪。
\note 使用自定义的调试跟踪级别。
\sa ytrace
*/
#	if YB_Use_YTrace
#		define YTrace(_stream, _lv, _t, _msg, ...) \
	ystdex::ytrace(_stream, _lv, _t, __FILE__, __LINE__, _msg, __VA_ARGS__)
#	else
#		define YTrace(...)
#	endif
#endif
//@}

