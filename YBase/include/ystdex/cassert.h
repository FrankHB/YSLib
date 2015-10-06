/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cassert.h
\ingroup YStandardEx
\brief ISO C 断言/调试跟踪扩展。
\version r201
\author FrankHB <frankhb1989@gmail.com>
\since build 432
\par 创建时间:
	2013-07-27 04:11:53 +0800
\par 修改时间:
	2015-10-04 15:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CAssert
*/


#ifndef YB_INC_ystdex_cassert_h_
#define YB_INC_ystdex_cassert_h_ 1

#include "../ydef.h"
#include <cstdio>

namespace ystdex
{

/*!
\brief YBase 默认断言函数。
\note 当定义宏 YB_Use_YAssert 不等于 0 时，宏 YAssert 操作由此函数实现。
\note 参数依次为：表达式、文件名、行号和消息文本。
\note 允许空指针参数，视为未知。
\note 调用 std::terminate 终止程序。
\since build 641
*/
YB_API YB_NORETURN void
yassert(const char*, const char*, int, const char*) ynothrow;

#if YB_Use_YTrace
/*!
\brief YCLib 调试跟踪函数。
\note 当定义宏 YB_Use_YTrace 不等于 0 时，宏 YTrace 操作由此函数实现。
\since build 553
*/
YB_API YB_ATTR_gnu_printf(6, 7) void
ytrace(std::FILE*, std::uint8_t, std::uint8_t, const char*, int, const char*,
	...) ynothrow;
#endif

} // namespace ystdex;

#endif // NOTE: Allow multiple for macro definitions.

#include <cassert>

#undef yconstraint
#undef yassume

/*!
\ingroup YBase_pseudo_keyword
\def yconstraint
\brief 约束：接口语义。
\note 和普通断言相比强调接口契约。对移植特定的平台实现时应予以特别注意。
\note 保证兼容 ISO C++11 constexpr 模板。
\see $2015-10 @ %Documentation::Workflow::Annual2015.
\since build 535

运行时检查的接口语义约束断言。不满足此断言的行为是接口明确地未定义的，行为不可预测。
*/
#ifdef NDEBUG
#	define yconstraint(_expr) YB_ASSUME(_expr)
#else
#	define yconstraint(_expr) \
	((_expr) ? void(0) : ystdex::yassert(#_expr, __FILE__, __LINE__, \
		"Constraint violation."))
#endif

/*!
\ingroup YBase_pseudo_keyword
\def yassume
\brief 假定：环境语义。
\note 和普通断言相比强调非接口契约。对移植特定的平台实现时应予以特别注意。
\since build 535

运行时检查的环境条件约束断言。用于明确地非 yconstraint 适用的情形。
*/
#ifdef NDEBUG
#	define yassume(_expr) YB_ASSUME(_expr)
#else
#	define yassume(_expr) assert(_expr)
#endif


#ifndef YAssert
#	if YB_Use_YAssert
#		define YAssert(_expr, _msg) \
	((_expr) ? void(0) : ystdex::yassert(#_expr, __FILE__, __LINE__, _msg))
#	else
#		define YAssert(_expr, _msg) yassume(_expr)
#	endif
#endif

#ifndef YAssertNonnull
//! \since build 495
#	define YAssertNonnull(_expr) YAssert(bool(_expr), "Null reference found.")
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

