/*
	© 2016-2017, 2019, 2021, 2022-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Extended.cpp
\ingroup NPL
\brief NPLA1 扩展环境语法形式。
\version r34094
\author FrankHB <frankhb1989@gmail.com>
\since build 974
\par 创建时间:
	2023-05-16 01:05:53 +0800
\par 修改时间:
	2023-05-21 11:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1Extended
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1Extended // for RetainN, NPL::ResolveRegular,
//	ContextHandler, std::next, RemoveHead, RelaySwitched, MoveKeptGuard,
//	NPL::AsTermNode, Continuation, A1::NameTypedContextHandler, ystdex::bind1,
//	Retain, std::distance, type_id, ystdex::ref_eq, ReduceCombinedBranch,
//	ReduceForwardHandler, AssertValueTags, ResolveTerm,
//	ResolvedTermReferencePtr, AccessRegular, A1::MakeForm, MakeValueOrMove,
//	LiftToReturn, NPL::TransferSubtermsBefore, LiftOtherValue,
//	NPL::AllocateEnvironment, string, YSLib::usystem, std::mem_fn;
#include "NPLA1Internals.h" // for NPL_Impl_NPLA1_Enable_TCO, OneShotChecker,
//	RefTCOAction, SetupNextTerm;
#include YFM_NPL_NPLA1Forms // for Forms::CallRegularUnaryAs;
#include <ystdex/scope_guard.hpp> // for ystdex::guard;

namespace NPL
{

namespace A1
{

namespace Forms
{

ReductionStatus
Call1CC(TermNode& term, ContextNode& ctx)
{
	RetainN(term);
	yunused(
		NPL::ResolveRegular<const ContextHandler>(*std::next(term.begin())));
	RemoveHead(term);

	auto& current(ctx.GetCurrentRef());
	// NOTE: The checker is a barrier to mark the capture beginning. When
	//	called, the continuation is invalidated.
#if !NPL_Impl_NPLA1_Enable_TCO
	OneShotChecker check(ctx);

	RelaySwitched(ctx,
		MoveKeptGuard(ystdex::guard<OneShotChecker>(check)));
#endif

	const auto i_captured(current.begin());

	// TODO: Blocked. Use ISO C++14 lambda initializers to simplify
	//	the implementation.
	term.GetContainerRef().push_back(NPL::AsTermNode(term.get_allocator(),
		// XXX: The name is usually uninterested by direct invocation, but when
		//	the continuation is extended, it can be somewhat useful to identify
		//	the source in the capture-time instead of extending-time (although
		//	currently there is just one source here).
		Continuation(A1::NameTypedContextHandler(
		ystdex::bind1([&, i_captured](TermNode& t, OneShotChecker& osc){
		Retain(t);
		osc.Check();

		// XXX: It is necessary to back %current and %i_captured up since the
		//	unwinding below may likely to invalidate the reducer here, hence
		//	the captured variables will also be destroyed.
		auto& cur(current);
		// XXX: Assume the frame referenced by %i_captured is in the reduction
		//	sequence, as the check of %NPL_NPLA1Forms_CheckContinuationFrames.
		const auto i_top(i_captured);
		// NOTE: The term %t is not owned by the reducers. It shall be backed up
		//	to skip the possible calls for the local cleanup (e.g.
		//	%TCOAction::GuardFunction). It shall not be saved directly to %term,
		//	as the existing subnodes in %term are usually also subject to the
		//	cleanup and shall not be invalidated before the cleanup.
		// XXX: This is not guarded. If any exception is thrown, the values are
		//	to be discarded.
		auto con(std::move(t.GetContainerRef()));
		auto vo(std::move(t.Value));

#if NPL_NPLA1Forms_CheckContinuationFrames
		YAssert(std::distance(cur.begin(), i_captured) < std::distance(
			cur.begin(), cur.end()), "The top frame of the reinstated captured"
			" continuation is missing.");
#endif
		// NOTE: Now unwind the reducer sequence upon to the delimiter. This
		//	switches the control to the position to the captured one. Unwinding
		//	frames would need %NPL_Impl_NPLA1_Enable_Thunked, otherwise it would
		//	leat to undefined behavior when the removed frames are accessed
		//	later, because only thunked implementations guarantees the future
		//	accesses are also removed, rather than remaining in the activation
		//	records of the host language.
		cur.UnwindUntil(i_top);
#if NPL_Impl_NPLA1_Enable_TCO
#	if NPL_NPLA1Forms_CheckContinuationFrames
		YAssert(i_top->target_type() == ystdex::type_id<TCOAction>(),
			"TCO action missing from the captured top frame.");
		YAssert(ystdex::ref_eq<>()(*i_top->target<TCOAction>(),
			RefTCOAction(ctx)), "Mismatched captured top frames found.");
#	endif
		// NOTE: The tail action should not be dropped. Release the guard in the
		//	frame instead.
		RefTCOAction(ctx).ReleaseOneShotGuard();
#else
		// NOTE: Drop the top check frame itself.
		cur.pop_front();
#endif
		// NOTE: After the cleanup, %t is usually invalidated, but %term is not.
		yunseq(term.GetContainerRef() = std::move(con),
			term.Value = std::move(vo));
		SetupNextTerm(ctx, term);
		RemoveHead(term);
		return ReductionStatus::Retained;
	},
#if NPL_Impl_NPLA1_Enable_TCO
		RefTCOAction(ctx).MakeOneShotChecker()
#else
		std::move(check)
#endif
	), "captured-one-shot-continuation"), ctx)));
	// NOTE: Tail call.
	// XXX: Only the underlying operative is used and the operand is a
	//	self-evaluating value, so the wrapper count is irrelevant. However, just
	//	leave it untouched to avoid dig into the internals of
	//	%FormContextHandler, as well as to be neutral to extended combiners not
	//	having %FormContextHandler at all.
	return ReduceCombinedBranch(term, ctx);
}

ReductionStatus
ContinuationToApplicative(TermNode& term)
{
	return Forms::CallRegularUnaryAs<Continuation>(
		[&](Continuation& cont, ResolvedTermReferencePtr p_ref){
		return ReduceForwardHandler(term, cont.Handler, p_ref);
	}, term);
}

ReductionStatus
ApplyContinuation(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 2);

	auto i(term.begin());
	auto i_comb(++i);
	auto& comb(*i_comb);

	AssertValueTags(comb);
	[&] YB_LAMBDA_ANNOTATE(() , , flatten){
		// NOTE: As %ContinuationToApplicative and %Apply.
		ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
			auto& h(AccessRegular<Continuation>(nd, p_ref).Handler);

			comb.Value = A1::MakeForm(comb, MakeValueOrMove(p_ref, [&]{
				return h;
			}, [&]{
				return std::move(h);
			}));
		}, comb);
		comb.ClearContainer();

		auto& t(*++i);

		LiftToReturn(t);
		NPL::TransferSubtermsBefore(t, term, i_comb);
		LiftOtherValue(term, t);
	}();
	// XXX: Explicit inline expansion here does not make it more efficient at
	//	least in code generation by x86_64-pc-linux G++ 12.1 when not called.
	return Combine<TailCall>::RelayEnvSwitch(ctx, term,
		NPL::AllocateEnvironment(term.get_allocator()));
}


void
CallSystem(TermNode& term)
{
	CallUnaryAs<const string>(
		ystdex::compose(YSLib::usystem, std::mem_fn(&string::c_str)), term);
}

} // namespace Forms;

} // namesapce A1;

} // namespace NPL;

