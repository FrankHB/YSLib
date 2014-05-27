/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cassert.h
\ingroup YStandardEx
\brief ISO C 断言/调试跟踪扩展。
\version r116
\author FrankHB <frankhb1989@gmail.com>
\since build 432
\par 创建时间:
	2013-07-27 04:11:53 +0800
\par 修改时间:
	2014-05-24 17:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CAssert
*/


#ifndef YB_INC_ystdex_cassert_h_
#define YB_INC_ystdex_cassert_h_ 1

#include "../ydef.h"
#include <cassert>
#include <cstdio>

namespace ystdex
{

/*!
\ingroup YBase_pseudo_keyword
\def yconstraint
\brief 约束：接口语义。
\note 和普通断言相比强调接口契约。对于移植特定的平台实现时应予以特别注意。
\since build 298

运行时检查的接口语义约束断言。不满足此断言的行为是接口明确地未定义的，行为不可预测。
*/
#define yconstraint assert

/*!
\ingroup YBase_pseudo_keyword
\def yassume
\brief 假定：环境语义。
\note 和普通断言相比强调非接口契约。对于移植特定的平台实现时应予以特别注意。
\since build 298

运行时检查的环境条件约束断言。用于明确地非 yconstraint 适用的情形。
*/
#define yassume assert


#if YB_Use_YAssert
#	undef YAssert

/*!
\brief YBase 默认断言函数。
\note 当定义宏 YB_Use_YAssert 不等于 0 时，宏 YAssert 操作由此函数实现。
\note 参数依次为：是否触发、表达式、文件名、行号和消息文本。
\since build 432
*/
YB_API void
yassert(bool, const char*, const char*, int, const char*);

#	define YAssert(_expr, _msg) \
	ystdex::yassert(_expr, #_expr, __FILE__, __LINE__, _msg)

#else
#	define YAssert(_expr, _msg) assert(exp)
#endif

//! \since build 495
#define YAssertNonnull(_expr) YAssert(bool(_expr), "Null pointer found.")

#if YB_Use_YTrace
//! \since build 432
//@{
#	undef YTrace

/*!
\brief YCLib 调试跟踪函数。
\note 当定义宏 YB_Use_YTrace 不等于 0 时，宏 YTrace 操作由此函数实现。
*/
YB_API YB_ATTR(format (printf, 6, 7)) void
ytrace(std::FILE*, std::uint8_t, std::uint8_t, const char*, int, const char*,
	...);

/*!
\brief YCLib 扩展调试跟踪。
\note 使用自定义的调试跟踪级别。
\sa ytrace
*/
#	define YTrace(_stream, _lv, _t, _msg, ...) \
	ystdex::ytrace(_stream, _lv, _t, __FILE__, __LINE__, _msg, __VA_ARGS__)

#else
#	define YTrace(...)
#endif
//@}

} // namespace ystdex;

#endif

