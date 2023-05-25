/*
	© 2016-2017, 2019, 2022-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Extended.h
\ingroup NPL
\brief NPLA1 扩展环境语法形式。
\version r10573
\author FrankHB <frankhb1989@gmail.com>
\since build 974
\par 创建时间:
	2023-05-16 01:05:23 +0800
\par 修改时间:
	2023-05-25 04:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1Extended
*/


#ifndef NPL_INC_NPLA1Extended_h_
#define NPL_INC_NPLA1Extended_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA1 // for shared_ptr, TermNode, ReductionStatus, string,
//	TokenValue, AssertBranch, ystdex::exclude_self_t, ystdex::expand_proxy,
//	NPL::ResolveTerm, std::next, AssertValueTags, Access, NPL::Deref,
//	Forms::CallResolvedUnary, ResolvedTermReferencePtr, AccessRegular,
//	ystdex::make_expanded, std::ref, ystdex::invoke_nonvoid, TNIter,
//	AccessTypedValue, ystdex::make_transform, std::accumulate,
//	std::placeholders::_2, ystdex::bind1, ContextNode,
//	ystdex::equality_comparable, ystdex::exclude_self_params_t,
//	ystdex::examiners::equal_examiner, ystdex::size_t_, trivial_swap_t,
//	trivial_swap, Environment, ystdex::is_bitwise_swappable, ystdex::true_;

namespace NPL
{

//! \since build 665
namespace A1
{

/*!
\brief 续延帧检查。
\note 因对性能有影响，默认仅调试配置下启用。
\sa Continuation
\since build 943

若定义为 true ，则在续延调用时断言被捕获的帧在当前动作序列中存在且满足实现约束。
*/
#ifndef NPL_NPLA1Forms_CheckContinuationFrames
#	ifndef NDEBUG
#		define NPL_NPLA1Forms_CheckContinuationFrames true
#	else
#		define NPL_NPLA1Forms_CheckContinuationFrames false
#	endif
#endif


//! \since build 732
namespace Forms
{

//! \since build 943
//!@{
/*!
\brief 捕获一次续延，具现为一等续延作为参数调用合并子。
\warning 应确保实现选项以避免未定义行为。

参考调用文法：
<pre>call/1cc \<combiner></pre>

对捕获的续延，若被合并子调用，则可移除特定的对象语言活动记录。
调用捕获的合并子应确保启用实现选项 NPL_Impl_NPLA1_Enable_Thunked ，
	以确保随活动记录时同时在宿主语言中移除对相应资源访问；
否则，除非被抛出异常，之后在宿主语言（主调函数）中访问这些活动记录，
	总是存在未定义行为。
*/
YF_API ReductionStatus
Call1CC(TermNode&, ContextNode&);

/*!
\brief 捕获一次续延，具现为一等续延作为参数调用合并子。

参考调用文法：
<pre>continuation->applicative \<continuation></pre>
*/
YF_API ReductionStatus
ContinuationToApplicative(TermNode&);
//!@}

/*!
\brief 应用续延。
\since build 952

参考调用文法：
<pre>apply-continuation \<continuation> \<object></pre>
*/
YF_API ReductionStatus
ApplyContinuation(TermNode&, ContextNode&);


/*!
\brief 调用 UTF-8 字符串的系统命令，并保存 int 类型的结果到项的值中。
\sa usystem
\since build 741

参考调用文法：
<pre>system \<string></pre>
*/
YF_API void
CallSystem(TermNode&);

} // namespace Forms;

} // namesapce A1;

} // namespace NPL;

#endif

