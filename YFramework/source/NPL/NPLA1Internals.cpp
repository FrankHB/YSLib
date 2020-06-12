/*
	© 2018-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Internals.cpp
\ingroup NPL
\brief NPLA1 内部接口。
\version r20162
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2020-02-15 13:20:08 +0800
\par 修改时间:
	2020-06-05 21:53 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	NPL::NPLA1Internals
*/


#include "NPL/YModules.h"
#include "NPLA1Internals.h"

using namespace YSLib;

namespace NPL
{

namespace A1
{

inline namespace Internals
{

#if NPL_Impl_NPLA1_Enable_Thunked
#	if NPL_Impl_NPLA1_Enable_TCO
TCOAction&
EnsureTCOAction(ContextNode& ctx, TermNode& term)
{
	auto p(AccessTCOAction(ctx));

	if(!p)
	{
		if(YB_UNLIKELY(!ctx.IsAlive()))
			ctx.SetupCurrent([&]() ynothrow{
				return ctx.LastStatus;
			});
		SetupTailTCOAction(ctx, term, {});
		p = AccessTCOAction(ctx);
	}
	return NPL::Deref(p);
}

TCOAction&
EnsureTCOActionUnchecked(ContextNode& ctx, TermNode& term)
{
	YAssertNonnull(ctx.IsAlive());

	auto p(AccessTCOAction(ctx));

	if(!p)
	{
		SetupTailTCOAction(ctx, term, {});
		p = AccessTCOAction(ctx);
	}
	return NPL::Deref(p);
}
#	endif
#endif


EnvironmentReference
FetchTailEnvironmentReference(const TermReference& ref, ContextNode& ctx)
{
	auto r_env(ref.GetEnvironmentReference());

	// NOTE: Unsafe term reference is enforced to be safe with current
	//	environment as the holder.
	return r_env.GetAnchorPtr() ? r_env
		: EnvironmentReference(ctx.GetRecordRef().shared_from_this());
}

ReductionStatus
ReduceForCombinerRef(TermNode& term, ContextNode& ctx,
	const TermReference& ref, const ContextHandler& h, size_t n)
{
	const auto& r_env(FetchTailEnvironmentReference(ref, ctx));
	const auto a(term.get_allocator());
	auto p_sub(YSLib::allocate_shared<TermNode>(a, TermNode(std::allocator_arg,
		a, NoContainer, ContextHandler(FormContextHandler(RefContextHandler(h,
		r_env), n)))));
	auto& sub(NPL::Deref(p_sub));
	TermNode tm(std::allocator_arg, a, {{std::allocator_arg, a, NoContainer,
		std::move(p_sub)}}, std::allocator_arg, a, TermReference(sub, r_env));

	term.SetContent(std::move(tm));
	return ReductionStatus::Retained;
}

} // inline namespace Internals;

} // namesapce A1;

} // namespace NPL;

