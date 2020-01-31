﻿/*
	© 2014-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.cpp
\ingroup NPL
\brief NPLA1 公共接口。
\version r16114
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-02 18:02:47 +0800
\par 修改时间:
	2020-01-31 04:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1 // for YSLib, ystdex::bind1, TokenValue,
//	std::placeholders, std::make_move_iterator, NPL::AsTermNode, NPL::Deref,
//	IsBranch, HasValue, ystdex::retry_on_cond, RelaySwitched, ContextHandler,
//	Environment, shared_ptr, tuple, list, lref, observer_ptr, ystdex::ref_eq,
//	set, ystdex::erase_all, ystdex::as_const, std::find_if,
//	NPL::make_observer, ComposeSwitched, RelayNext, TermReference, ResolveTerm,
//	AccessFirstSubterm, NPL::IsMovable, TermTags, NPL::TryAccessReferencedTerm,
//	ystdex::value_or, RemoveHead, IsLeaf, unordered_map, IsBranchedList,
//	NPL::TryAccessLeaf, any_ops::use_holder, weak_ptr, in_place_type,
//	ystdex::type_id, YSLib::allocate_shared, InvalidReference, MoveFirstSubterm,
//	vector, ShareMoveTerm, ystdex::ref, ystdex::exists, ystdex::id,
//	ystdex::cast_mutable, EnvironmentSwitcher, ComposeActions, LiftOtherOrCopy,
//	ystdex::equality_comparable, std::allocator_arg, NoContainer,
//	ystdex::exchange, NPL::SwitchToFreshEnvironment, NPL::Access,
//	YSLib::share_move, GetLValueTagsOf, NPL::TryAccessReferencedLeaf,
//	ystdex::invoke_value_or, ystdex::call_value_or, ystdex::swap_guard,
//	ystdex::make_transform, ystdex::try_emplace, ystdex::unique_guard,
//	IsNPLAExtendedLiteralNonDigitPrefix, CheckReducible, IsNPLAExtendedLiteral,
//	ystdex::isdigit, ResolveIdentifier, NPL::TryAccessTerm, LiteralHandler,
//	CategorizeBasicLexeme, DeliteralizeUnchecked, Deliteralize,
//	LiftMovedOther, LiftCollapsed, NPL::AllocateEnvironment, std::mem_fn;
#include YFM_NPL_SContext // for Session;
#include <ystdex/ref.hpp> // for ystdex::unref;

using namespace YSLib;

namespace NPL
{

// NOTE: The inlined synchronous calls are more effecient than asynchronous
//	ones. It is intended to prevent unneeded uncapturable continuation being
//	construced by reduction action composition calls (e.g. of %RelayNext).
//	However, they are only safe where they are not directly called by
//	asynchronous implementations which expect the actions in these direct calls
//	are always strictly asynchronous. In thunked implementations, they shall
//	also be cared to avoid direct recursion of synchronous calls which make the
//	thunks potentially useless to prevent the host stack overflow.
#define NPL_Impl_NPLA1_Enable_InlineDirect true

// NOTE: The following options provide documented alternative implementations.
//	They are for exposition only. The code without TCO or asynchrous thunked
//	calls works without several guarantees in the specification (notably,
//	support of PTC), which is not conforming. Other documented cases not
//	supporting PTC are noted in implementations separatedly.
#define NPL_Impl_NPLA1_Enable_TCO true
#define NPL_Impl_NPLA1_Enable_Thunked true

// NOTE: Use %RelaySwitched instead of %RelayNext without %ComposeSwitched as
//	possible to be more friendly to TCO.

//! \since build 820
static_assert(!NPL_Impl_NPLA1_Enable_TCO || NPL_Impl_NPLA1_Enable_Thunked,
	"Invalid combination of build options found.");

//! \since build 863
//@{
#ifndef NDEBUG
#	define NPL_Impl_NPLA1_AssertParameterMatch true
#	define NPL_Impl_NPLA1_TraceVauCall true
#else
#	define NPL_Impl_NPLA1_AssertParameterMatch false
#	define NPL_Impl_NPLA1_TraceVauCall false
#endif
//@}

namespace A1
{

string
to_string(ValueToken vt)
{
	switch(vt)
	{
	case ValueToken::Null:
		return "null";
	case ValueToken::Undefined:
		return "undefined";
	case ValueToken::Unspecified:
		return "unspecified";
	case ValueToken::GroupingAnchor:
		return "grouping";
	case ValueToken::OrderedAnchor:
		return "ordered";
	}
	throw std::invalid_argument("Invalid value token found.");
}

//! \since build 685
namespace
{

//! \since build 852
//@{
template<typename _func, class _tTerm>
YB_ATTR_nodiscard TermNode
TransformForSeparatorCore(_func trans, _tTerm&& term, const ValueObject& pfx,
	const TokenValue& delim)
{
	using namespace std::placeholders;
	using it_t = decltype(std::make_move_iterator(term.end()));
	const auto a(term.get_allocator());
	auto res(NPL::AsTermNode(a, yforward(term).Value));

	if(IsBranch(term))
	{
		res.Add(NPL::AsTermNode(a, pfx));
		ystdex::split(std::make_move_iterator(term.begin()),
			std::make_move_iterator(term.end()), ystdex::bind1(
			HasValue<TokenValue>, std::ref(delim)), [&](it_t b, it_t e){
				const auto add([&](TermNode& node, it_t i){
					node.Add(trans(NPL::Deref(i)));
				});

				if(b != e)
				{
					// XXX: This guarantees a single element is converted with
					//	no redundant parentheses according to NPLA1 syntax,
					//	consistent to the trivial reduction for term with one
					//	subnode in %ReduceOnce.
					if(std::next(b) == e)
						add(res, b);
					else
					{
						auto child(NPL::AsTermNode(a));

						ystdex::retry_on_cond([&]() ynothrow{
							return b != e;
						}, [&]{
							add(child, b++);
						});
						res.Add(std::move(child));
					}
				}
			});
	}
	return res;
}

//! \since build 852
template<class _tTerm>
YB_ATTR_nodiscard TermNode
TransformForSeparatorTmpl(_tTerm&& term, const ValueObject& pfx,
	const TokenValue& delim)
{
	return TransformForSeparatorCore([&](_tTerm&& tm) ynothrow{
		return yforward(tm);
	}, yforward(term), pfx, delim);
}

//! \since build 852
template<class _tTerm>
YB_ATTR_nodiscard TermNode
TransformForSeparatorRecursiveTmpl(_tTerm&& term, const ValueObject& pfx,
	const TokenValue& delim)
{
	return TransformForSeparatorCore([&](_tTerm&& tm){
		return TransformForSeparatorRecursiveTmpl(yforward(tm), pfx, delim);
	}, yforward(term), pfx, delim);
}
//@}


//! \since build 868
//@{
using Forms::CallRegularUnaryAs;
//! \since build 874
using Forms::CallResolvedUnary;
using Forms::CallUnary;
using Forms::Retain;
using Forms::RetainN;
using Forms::ThrowInsufficientTermsError;
//! \since build 859
using Forms::ThrowInvalidSyntaxError;
using Forms::ThrowValueCategoryErrorForFirstArgument;
//@}


//! \since build 842
YB_FLATTEN inline PDefH(void, SetupNextTerm, ContextNode& ctx, TermNode& term)
	ImplExpr(ContextState::Access(ctx).SetNextTermRef(term))

// NOTE: See $2018-09 @ %Documentation::Workflow for rationale of the
//	implementation.
// XXX: First-class continuations are not implemented yet, due to lack of term
//	replacement mechanism in captured continuation. Kernel-style continuation
//	interception is also unsupported because no reference for parantage is
//	maintained in the context currently.

#if NPL_Impl_NPLA1_Enable_Thunked
#	if false
//! \since build 821
YB_ATTR_nodiscard inline
#		if true
PDefH(ReductionStatus, RelayTail, ContextNode& ctx, Reducer& cur)
	ImplRet(RelaySwitched(ctx, std::move(cur)))
#		else
// NOTE: For exposition only. This does not hold guarantee of TCO in unbounded
//	recursive cases.
PDefH(ReductionStatus, RelayTail, ContextNode&, const Reducer& cur)
	ImplRet(cur())
#		endif
#	endif

//! \since build 840
template<typename _func>
void
SetupTailAction(ContextNode& ctx, _func&& act)
{
	// XXX: Avoid direct use of %ContextNode::SetupTail even though it is safe
	//	because %Current is already saved in %act?
	ctx.SetupTail(std::move(act));
}

//! \since build 841
void
PushActionsRange(EvaluationPasses::const_iterator first,
	EvaluationPasses::const_iterator last, TermNode& term, ContextNode& ctx)
{
	if(first != last)
	{
		// NOTE: Retrying is recognized from the result since 1st administrative
		//	pass is reduced. The non-skipped administractive reductions are
		//	reduced in the tail context, otherwise they are dropped by no-op.
		if(ctx.LastStatus != ReductionStatus::Retrying)
		{
			const auto& f(first->second);

			++first;
			SetupNextTerm(ctx, term);
			RelaySwitched(ctx, [=, &f, &term, &ctx]{
				PushActionsRange(first, last, term, ctx);

				const auto res(f(term, ctx));

				if(res != ReductionStatus::Partial)
					ctx.LastStatus
						= CombineSequenceReductionResult(ctx.LastStatus, res);
				return ctx.LastStatus;
			});
		}
	}
	else
		ctx.LastStatus = ReductionStatus::Neutral;
}

#	if NPL_Impl_NPLA1_Enable_TCO
/*!
\brief 帧记录索引。
\note 顺序保持和 FrameRecord 的元素对应一致。
\since build 842
*/
enum RecordFrameIndex : size_t
{
	ActiveCombiner,
	ActiveEnvironmentPtr
};

/*!
\brief 帧记录。
\note 成员顺序和 RecordFrameIndex 中的项对应。
\since build 842
\sa RecordFrameIndex
*/
using FrameRecord = tuple<ContextHandler, shared_ptr<Environment>>;

/*!
\brief 帧记录列表。
\sa FrameRecord
\since build 827
*/
using FrameRecordList = yimpl(list)<FrameRecord>;

//! \since build 818
class TCOAction final
{
public:
	lref<TermNode> Term;
	//! \since build 821
	mutable Reducer Next;

private:
	//! \since build 854
	//@{
	bool req_combined = {};
	bool req_lift_result = {};
	bool req_retrying = {};
	//@}
	//! \since build 827
	mutable list<ContextHandler> xgds{};

public:
	// See $2018-06 @ %Documentation::Workflow for details.
	//! \since build 825
	mutable observer_ptr<const ContextHandler> LastFunction{};
	//! \since build 820
	mutable EnvironmentGuard EnvGuard;
	//! \since build 825
	mutable FrameRecordList RecordList;

	//! \since build 819
	TCOAction(ContextNode& ctx, TermNode& term, bool lift)
		: Term(term), Next(ctx.Switch()), req_lift_result(lift),
		xgds(ctx.get_allocator()), EnvGuard(ctx),
		RecordList(ctx.get_allocator())
	{}
	// XXX: Not used, but provided for well-formness.
	//! \since build 819
	TCOAction(const TCOAction& a)
		// XXX: Some members are moved. This is only safe when newly constructed
		//	object always live longer than the older one.
		: Term(a.Term), Next(a.Next), req_combined(a.req_combined),
		req_lift_result(a.req_lift_result), req_retrying(a.req_retrying),
		xgds(std::move(a.xgds)), EnvGuard(std::move(a.EnvGuard))
	{}
	DefDeMoveCtor(TCOAction)

	DefDeMoveAssignment(TCOAction)

	//! \since build 877
	ReductionStatus
	operator()(ContextNode& ctx) const
	{
		YAssert(ystdex::ref_eq<>()(EnvGuard.func.Context.get(), ctx),
			"Invalid context found.");

		RelaySwitched(ctx, std::move(Next));

		// NOTE: Lifting is optional, but is shall be performed before release
		//	of guards. See also $2018-02 @ %Documentation::Workflow.
		const auto res(HandleResultLiftRequest(Term, ctx));

		// NOTE: The order here is significant. The environment in the guards
		//	should be hold until lifting is completed.
		{
			const auto egd(std::move(EnvGuard));
		}
		while(!xgds.empty())
			xgds.pop_back();
		if(req_combined)
			RegularizeTerm(Term, res);
		// NOTE: This should be after the guards to ensure there is no term
		//	references the environment.
		while(!RecordList.empty())
		{
			// NOTE: The order is significant, as %FrameRecord destruction now
			//	is unspecified, and temporary objects have dependencies on
			//	environments.
			auto& front(RecordList.front());

			get<ActiveCombiner>(front) = {};
			RecordList.pop_front();
		}
		return req_retrying ? ReductionStatus::Retrying : res;
	}

	//! \since build 857
	void
	AddRecord(shared_ptr<Environment>&& p_env)
	{
		// NOTE: The temporary function, the environment and the
		//	temporary operand are saved in the frame record list as a new
		//	entry.
		RecordList.emplace_front(MoveFunction(), std::move(p_env));
	}

	//! \since build 825
	YB_ATTR_nodiscard lref<const ContextHandler>
	AttachFunction(ContextHandler&& h)
	{
		// NOTE: This scans guards to hold function prvalues, which are safe to
		//	be removed as per the equivalence (hopefully, of beta reduction)
		//	defined by %operator== of the handler. No new instance is to be
		//	added.
		ystdex::erase_all(xgds, h);
		xgds.emplace_back();
		// NOTE: Strong exception guarantee is kept here.
		swap(xgds.back(), h);
		return ystdex::as_const(xgds.back());
	}

	//! \since build 854
	ReductionStatus
	HandleResultLiftRequest(TermNode& term, ContextNode& ctx) const
	{
		// NOTE: This implies the call of %RegularizeTerm before lifting. Since
		//	the call of %RegularizeTerm is idempotent without term modification
		//	before the next reduction of the term or some other term, there is
		//	no need to call %RegularizeTerm if the lift is not needed.
		if(req_lift_result)
		{
			// NOTE: The call of %RegularizeTerm is for the previous reduction.
			//	The order of the calls is significant.
			RegularizeTerm(term, ctx.LastStatus);
			return ReduceForLiftedResult(term);
		}
		return ctx.LastStatus;
	}

	//! \since build 825
	YB_ATTR_nodiscard ContextHandler
	MoveFunction()
	{
		ContextHandler res;

		if(LastFunction)
		{
			const auto i(std::find_if(xgds.rbegin(), xgds.rend(),
				[this](const ContextHandler& h) ynothrow{
				return NPL::make_observer(&h) == LastFunction;
			}));

			if(i != xgds.rend())
			{
				res = std::move(*i);
				xgds.erase(std::next(i).base());
			}
			LastFunction = {};
		}
		return res;
	}

	//! \since build 854
	//@{
	void
	RequestCombined()
	{
		req_combined = true;
	}

	void
	RequestLiftResult()
	{
		req_lift_result = true;
	}

	void
	RequestRetrying()
	{
		req_retrying = true;
	}
	//@}
};

//! \since build 830
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TCOAction*, AccessTCOAction, ContextNode& ctx)
	ImplRet(ctx.Current.target<TCOAction>())
// NOTE: There is no need to check term like 'if(&p->Term.get() == &term)'. It
//	should be same to saved enclosing term unless a nested TCO action is needed
//	explicitly (by following %SetupTailAction rather than %EnsureTCOAction).

//! \since build 840
YB_ATTR_nodiscard TCOAction&
EnsureTCOAction(ContextNode& ctx, TermNode& term)
{
	auto p(AccessTCOAction(ctx));

	if(!p)
	{
		SetupTailAction(ctx, TCOAction(ctx, term, {}));
		p = AccessTCOAction(ctx);
	}
	return NPL::Deref(p);
}

//! \since build 840
YB_ATTR_nodiscard inline PDefH(TCOAction&, RefTCOAction, ContextNode& ctx)
	// NOTE: The TCO action should have been created by a previous call of
	//	%EnsureTCOAction, typically in the call of %CombinerReturnThunk in
	//	calling a combiner from %ReduceCombinedImpl.
	ImplRet(NPL::Deref(AccessTCOAction(ctx)))
#	endif
#endif


//! \since build 879
//@{
#if NPL_Impl_NPLA1_Enable_Thunked && !NPL_Impl_NPLA1_Enable_InlineDirect
YB_ATTR(always_inline) inline ReductionStatus
RelayCurrent(ContextNode& ctx, Continuation&& cur)
{
	return RelaySwitchedUnchecked(ctx, std::move(cur));
}
YB_ATTR(always_inline) inline ReductionStatus
RelayCurrent(ContextNode& ctx, std::reference_wrapper<Continuation> cur)
{
	return RelaySwitchedUnchecked(ctx, cur);
}
template<typename _fCurrent>
YB_ATTR(always_inline) inline auto
RelayCurrent(ContextNode& ctx, _fCurrent&& cur)
	-> decltype(cur(std::declval<TermNode&>(), ctx))
{
	return A1::RelayCurrent(ctx, Continuation(yforward(cur), ctx));
}

template<typename _fNext>
YB_ATTR(always_inline) inline ReductionStatus
RelayNextOrDirect(ContextNode& ctx, Continuation&& cur, _fNext&& next)
{
	return RelayNext(ctx, yforward(cur), ComposeSwitched(ctx, yforward(next)));
}
template<typename _fNext>
YB_ATTR(always_inline) inline ReductionStatus
RelayNextOrDirect(ContextNode& ctx,
	std::reference_wrapper<Continuation> cur, _fNext&& next)
{
	return RelayNext(ctx, cur, ComposeSwitched(ctx, yforward(next)));
}
template<typename _fCurrent, typename _fNext>
YB_ATTR(always_inline) inline auto
RelayNextOrDirect(ContextNode& ctx, _fCurrent&& cur, _fNext&& next)
	-> decltype(cur(std::declval<TermNode&>(), ctx))
{
	return A1::RelayNextOrDirect(ctx, Continuation(yforward(cur), ctx),
		yforward(next));
}
#endif
//@}

//! \since build 878
//@{
#if !NPL_Impl_NPLA1_Enable_Thunked || NPL_Impl_NPLA1_Enable_InlineDirect
template<typename _fCurrent>
YB_ATTR(always_inline) inline ReductionStatus
RelayDirect(ContextNode& ctx, _fCurrent&& cur)
{
	return cur(ctx);
}
template<typename _fCurrent>
YB_ATTR(always_inline) inline auto
RelayDirect(ContextNode& ctx, _fCurrent&& cur, TermNode& term)
	-> decltype(cur(term, ctx))
{
	// XXX: This workarounds %std::reference_wrapper in libstdc++ to function
	//	type in C++2a mode with Clang++. See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=93470.
	return ystdex::unref(cur)(term, ctx);
}
#endif
YB_ATTR(always_inline) inline ReductionStatus
RelayDirect(ContextNode& ctx, const Continuation& cur, TermNode& term)
{
	return cur.Handler(term, ctx);
}

#if NPL_Impl_NPLA1_Enable_Thunked
template<typename _fCurrent>
YB_ATTR(always_inline) inline ReductionStatus
RelayCurrentOrDirect(ContextNode& ctx, _fCurrent&& cur, TermNode& term)
{
#	if NPL_Impl_NPLA1_Enable_InlineDirect
	return A1::RelayDirect(ctx, yforward(cur), term);
#	else
	yunused(term);
	return A1::RelayCurrent(ctx, yforward(cur));
#	endif
}
#endif

template<typename _fCurrent, typename _fNext>
YB_FLATTEN inline ReductionStatus
ReduceCurrentNext(TermNode& term, ContextNode& ctx, _fCurrent&& cur,
	_fNext&& next)
{
	SetupNextTerm(ctx, term);
#if NPL_Impl_NPLA1_Enable_Thunked
#	if NPL_Impl_NPLA1_Enable_InlineDirect
	RelaySwitched(ctx, yforward(next));
	return A1::RelayDirect(ctx, yforward(cur), term);
#	else
	return A1::RelayNextOrDirect(ctx, yforward(cur), yforward(next));
#	endif
#else
	A1::RelayDirect(ctx, yforward(cur), term);
	return ystdex::expand_proxy<ReductionStatus(ContextNode&)>::call(
		yforward(next), ctx);
#endif
}
//@}


//! \since build 821
template<typename _fCurrent>
inline ReductionStatus
ReduceSubsequent(TermNode& term, ContextNode& ctx, _fCurrent&& next)
{
	return A1::ReduceCurrentNext(term, ctx,
		std::ref(ContextState::Access(ctx).ReduceOnce), yforward(next));
}

#if NPL_Impl_NPLA1_Enable_Thunked
//! \since build 810
YB_ATTR(always_inline) inline ReductionStatus
ReduceChildrenOrderedAsync(TNIter, TNIter, ContextNode&);

//! \since build 879
ReductionStatus
ReduceChildrenOrderedAsyncUnchecked(TNIter first, TNIter last, ContextNode& ctx)
{
	YAssert(first != last, "Invalid range found.");

	auto& term(*first++);

	return ReduceSubsequent(term, ctx,
		Continuation([first, last](TermNode&, ContextNode& c){
		return ReduceChildrenOrderedAsync(first, last, c);
	}, ctx));
}

YB_ATTR(always_inline) inline ReductionStatus
ReduceChildrenOrderedAsync(TNIter first, TNIter last, ContextNode& ctx)
{
	return first != last ? ReduceChildrenOrderedAsyncUnchecked(first, last, ctx)
		: ReductionStatus::Neutral;
}
#endif


//! \since build 858
bool
RegularizeForm(TermNode& fm)
{
	// XXX: As now, it needs to convert at first to save the subterm in the
	//	irregular representation. See %ReduceForCombinerRef for the introduction
	//	of the irregular representation.
	if(const auto p_ref_fm = NPL::TryAccessLeaf<const TermReference>(fm))
		if(IsBranch(fm))
		{
			YAssert(fm.size() == 1, "Invalid irregular representation of"
				" reference with multiple subterms found.");
			YAssert(IsLeaf(*fm.begin()), "Invalid irregular representation of"
				" reference with non-leaf 1st subterm found.");

			// XXX: Assume nonnull. This is guaranteed in construction
			//	in %ReduceForCombinerRef.
			const auto& referenced(p_ref_fm->get());

			YAssert(ystdex::ref_eq<>()(NPL::Deref(NPL::Access<
				shared_ptr<TermNode>>(*fm.begin())), referenced),
				"Invalid subobject reference found.");
			// XXX: %TermNode::SetContent cannot be used here due to subterm
			//	invalidation.
			fm.MoveContent(TermNode(referenced));
			YAssert(IsLeaf(fm), "Wrong result of irregular"
				" representation conversion found.");
			return true;
		}
	return {};
}

//! \since build 821
template<typename... _tParams>
ReductionStatus
CombinerReturnThunk(const ContextHandler& h, TermNode& term, ContextNode& ctx,
	_tParams&&... args)
{
	static_assert(sizeof...(args) < 2, "Unsupported owner arguments found.");
#if NPL_Impl_NPLA1_Enable_TCO
	auto& act(EnsureTCOAction(ctx, term));
	auto& lf(act.LastFunction);

	lf = {};
	// XXX: Blocked. 'yforward' cause G++ 5.3 crash: internal compiler
	//	error: Segmentation fault.
	yunseq(0, (lf = NPL::make_observer(
		&act.AttachFunction(std::forward<_tParams>(args)).get()), 0)...);
	act.RequestCombined();
	SetupNextTerm(ctx, term);
	// XXX: %A1::RelayCurrentOrDirect is not used to allow the underlying
	//	handler optimized with %NPL_Impl_NPLA1_Enable_InlineDirect.
	return RelaySwitched(ctx, Continuation(std::ref(lf ? *lf : h), ctx));
#elif NPL_Impl_NPLA1_Enable_Thunked

	// XXX: %A1::ReduceCurrentNext is not used to allow the underlying
	//	handler optimized with %NPL_Impl_NPLA1_Enable_InlineDirect.
	SetupNextTerm(ctx, term);
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	return RelayNext(ctx, Continuation(std::ref(h), ctx),
		ComposeSwitched(ctx, std::bind([&](const _tParams&...){
		// NOTE: Captured argument pack is only needed when %h actually shares.
		return RegularizeTerm(term, ctx.LastStatus);
	}, std::move(args)...)));
#else

	yunseq(0, args...);
	return RegularizeTerm(term, h(term, ctx));
#endif
}

//! \since build 859
ReductionStatus
ReduceCombinedImpl(TermNode& term, ContextNode& ctx)
{
	YAssert(IsBranchedList(term), "Invalid term found for combined term.");

	auto& fm(AccessFirstSubterm(term));
	const bool irregular(RegularizeForm(fm));

	// XXX: As now, it needs to convert at first to save the subterm in the
	//	irregular representation. See %ReduceForCombinerRef for the introduction
	//	of the irregular representation.
	if(irregular || IsLeaf(fm))
	{
		if(irregular)
			term.Tags &= ~TermTags::Temporary;
		else
			term.Tags |= TermTags::Temporary;
		if(const auto p_handler = NPL::TryAccessLeaf<ContextHandler>(fm))
#if NPL_Impl_NPLA1_Enable_TCO
			return CombinerReturnThunk(*p_handler, term, ctx,
				std::move(*p_handler));
#elif NPL_Impl_NPLA1_Enable_Thunked
		{
			// XXX: Optimize for performance using context-dependent store?
			// XXX: This should ideally be a member of handler. However, it
			//	makes no sense before allowing %ContextHandler overload for
			//	ref-qualifier '&&'.
			auto p(YSLib::share_move(ctx.get_allocator(), *p_handler));

			return CombinerReturnThunk(*p, term, ctx, std::move(p));
		}
#else
			return CombinerReturnThunk(ContextHandler(std::move(*p_handler)),
				term, ctx);
#endif
	}
	// NOTE: This is neutral to %NPL_Impl_NPLA1_Enable_Thunked.
	if(const auto p_handler = NPL::TryAccessReferencedTerm<ContextHandler>(fm))
	{
		term.Tags &= ~TermTags::Temporary;
		return CombinerReturnThunk(*p_handler, term, ctx);
	}
	return ResolveTerm([&](const TermNode& nd, bool has_ref)
		YB_ATTR_LAMBDA(noreturn) -> ReductionStatus{
		// TODO: Capture contextual information in error.
		// TODO: Extract general form information extractor function.
		throw ListReductionFailure(ystdex::sfmt("No matching combiner '%s'"
			" for operand with %zu argument(s) found.",
			TermToStringWithReferenceMark(nd, has_ref).c_str(),
			FetchArgumentN(term)));
	}, fm);
}


//! \since build 869
YB_ATTR_nodiscard YB_PURE Environment&
FetchTailEnvironment(const TermReference& ref)
{
	return NPL::Deref(shared_ptr<Environment>(
		ref.GetEnvironmentReference().GetPtr()));
}

//! \since build 869
YB_ATTR_nodiscard YB_PURE EnvironmentReference
FetchContextOrTailEnvironmentReference(const TermReference& ref,
	ContextNode& ctx)
{
	auto r_env(ref.GetEnvironmentReference());

	return r_env.GetAnchorPtr() ? std::move(r_env)
		: EnvironmentReference(ctx.GetRecordRef().shared_from_this());
}

//! \since build 874
// XXX: This is more efficient, at least in code generation by x86_64-pc-linux
//	G++ 9.2 for %WrapN.
template<typename _fCopy, typename _fMove>
YB_ATTR_nodiscard YB_FLATTEN auto
MakeValueOrMove(ResolvedTermReferencePtr p_ref, _fCopy cp, _fMove mv)
	-> decltype(NPL::IsMovable(p_ref) ? mv() : cp())
{
	return NPL::IsMovable(p_ref) ? mv() : cp();
}

//! \since build 869
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TermReference, EnsureLValueReference, TermReference&& ref)
	// XXX: Use %TermReference::SetTags is not efficient here.
	ImplRet(TermReference(ref.GetTags() & ~TermTags::Unique, std::move(ref)))

//! \since build 851
YB_ATTR_nodiscard ReductionStatus
DoAdministratives(const EvaluationPasses& passes, TermNode& term,
	ContextNode& ctx)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	// XXX: Be cautious with overflow risks in call of %ContextNode::ApplyTail
	//	when TCO is not enabled.
	ctx.LastStatus = ReductionStatus::Neutral;
	PushActionsRange(passes.cbegin(), passes.cend(), term, ctx);
	return ReductionStatus::Partial;
#else
	return passes(term, ctx);
#endif
}

//! \since build 823
ReductionStatus
ReduceSequenceOrderedAsync(TermNode& term, ContextNode& ctx, TNIter i)
{
	YAssert(i != term.end(), "Invalid iterator found for sequence reduction.");
	// TODO: Allow capture next sequenced evaluations as a single continuation?
	return std::next(i) == term.end() ? ReduceAgainLifted(term, ctx, *i)
		: ReduceSubsequent(*i, ctx, [&, i](ContextNode& c){
		return ReduceSequenceOrderedAsync(term, c, term.erase(i));
	});
}

//! \since build 861
ReductionStatus
ReduceReturnUnspecified(TermNode& term) ynothrow
{
	term.Value = ValueToken::Unspecified;
	return ReductionStatus::Clean;
}

//! \since build 876
YB_ATTR_nodiscard YB_PURE bool
ExtractBool(const TermNode& term)
{
	return ystdex::value_or(NPL::TryAccessReferencedTerm<bool>(term), true);
}

//! \since build 860
//@{
TNIter
CondClauseCheck(TermNode& clause)
{
	if(YB_UNLIKELY(clause.empty()))
		ThrowInsufficientTermsError();
	return clause.begin();
}

YB_ATTR_nodiscard bool
CondTest(TermNode& clause, TNIter j)
{
	if(ExtractBool(*j))
	{
		clause.erase(j);
		return true;
	}
	return {};
}

#if NPL_Impl_NPLA1_Enable_Thunked
ReductionStatus
CondImpl(TermNode& term, ContextNode& ctx, TNIter i)
{
	if(i != term.end())
	{
		auto& clause(Deref(i));
		auto j(CondClauseCheck(clause));

		// NOTE: This also supports TCO.
		return ReduceSubsequent(Deref(j), ctx, [&, i, j](ContextNode& c){
			if(CondTest(clause, j))
				return ReduceAgainLifted(term, c, clause);
			return CondImpl(term, c, std::next(i));
		});
	}
	return ReduceReturnUnspecified(term);
}
#endif

ReductionStatus
WhenUnless(TermNode& term, ContextNode& ctx, bool is_when)
{
	using namespace Forms;

	Retain(term);
	RemoveHead(term);
	if(IsBranch(term))
	{
		auto& test(AccessFirstSubterm(term));

		return ReduceSubsequent(test, ctx, [&, is_when](ContextNode& c){
			if(ExtractBool(test) == is_when)
			{
				RemoveHead(term);
				return ReduceOrdered(term, c);
			}
			return ReduceReturnUnspecified(term);
		});
	}
	else
		ThrowInsufficientTermsError();
}

ReductionStatus
And2(TermNode& term, ContextNode& ctx, TNIter i)
{
	if(ExtractBool(NPL::Deref(i)))
	{
		term.Remove(i);
		return ReduceAgain(term, ctx);
	}
	term.Value = false;
	return ReductionStatus::Clean;
}

ReductionStatus
Or2(TermNode& term, ContextNode& ctx, TNIter i)
{
	if(!ExtractBool(NPL::Deref(i)))
	{
		term.Remove(i);
		return ReduceAgain(term, ctx);
	}
	return ReduceAgainLifted(term, ctx, NPL::Deref(i));
}

ReductionStatus
AndOr(TermNode& term, ContextNode& ctx, ReductionStatus
	(&and_or)(TermNode&, ContextNode&, TNIter))
{
	Retain(term);

	auto i(term.begin());

	if(++i != term.end())
		return std::next(i) == term.end() ? ReduceAgainLifted(term, ctx, *i)
			: ReduceSubsequent(*i, ctx,
			std::bind(and_or, std::ref(term), std::placeholders::_1, i));
	term.Value = and_or == And2;
	return ReductionStatus::Clean;
}
//@}

//! \since build 804
//@{
template<typename _fComp, typename _func>
void
EqualTerm(TermNode& term, _fComp f, _func g)
{
	RetainN(term, 2);

	auto i(term.begin());
	const auto& x(*++i);

	term.Value = f(g(x), g(ystdex::as_const(*++i)));
}

template<typename _func>
void
EqualTermValue(TermNode& term, _func f)
{
	EqualTerm(term, f, [](const TermNode& x) -> const ValueObject&{
		return ReferenceTerm(x).Value;
	});
}

template<typename _func>
void
EqualTermReference(TermNode& term, _func f)
{
	EqualTerm(term, [f](const TermNode& x, const TermNode& y) YB_PURE{
		return IsLeaf(x) && IsLeaf(y) ? f(x.Value, y.Value)
			: ystdex::ref_eq<>()(x, y);
	}, static_cast<const TermNode&(&)(const TermNode&)>(ReferenceTerm));
}
//@}


//! \since build 782
class RecursiveThunk final
{
private:
	//! \since build 784
	using shared_ptr_t = shared_ptr<ContextHandler>;
	//! \since build 784
	unordered_map<string, shared_ptr_t> store{};

public:
	//! \since build 840
	lref<Environment> Record;
	//! \since build 840
	lref<const TermNode> Term;

	//! \since build 868
	RecursiveThunk(Environment& env, const TermNode& t)
		: Record(env), Term(t)
	{
		Fix(Record, Term);
	}
	//! \since build 841
	DefDeMoveCtor(RecursiveThunk)

	//! \since build 841
	DefDeMoveAssignment(RecursiveThunk)

private:
	//! \since build 853
	void
	Fix(Environment& env, const TermNode& t)
	{
		if(IsBranchedList(t))
			for(const auto& tm : t)
				Fix(env, tm);
		else if(const auto p = NPL::TryAccessLeaf<TokenValue>(t))
		{
			const auto& n(*p);

			// XXX: This is served as addtional static environment.
			CheckParameterLeafToken(n, [&]{
				if(store.find(n) == store.cend())
					// NOTE: The symbol can be rebound.
					env.Bind(n, TermNode(TermNode::Container(t.get_allocator()),
						ValueObject(any_ops::use_holder, in_place_type<
						HolderFromPointer<weak_ptr<ContextHandler>>>,
						store[n] = YSLib::allocate_shared<ContextHandler>(
						t.get_allocator(), ThrowInvalidCyclicReference))));
			});
		}
	}

	//! \since build 787
	void
	Restore(Environment& env, const TermNode& t)
	{
		if(IsBranchedList(t))
			for(const auto& tm : t)
				Restore(env, tm);
		else if(const auto p = NPL::TryAccessLeaf<TokenValue>(t))
			FilterExceptions([&]{
				const auto& n(*p);

				if(env[n].Value.type() == ystdex::type_id<ContextHandler>())
				{
					// XXX: The element should exist unless previously removed.
					const auto i(store.find(n));

					if(i != store.cend())
					{
						auto& p_strong(i->second);

						if(p_strong.use_count() > 1)
							// TODO: Blocked. Use C++14 lambda initializers
							//	to simplify the implementation.
							NPL::Deref(p_strong) = std::bind(RestoredThunk,
								std::placeholders::_1, std::placeholders::_2, n,
								env.shared_from_this());
						// NOTE: Entry shall be removed to prevent duplicate
						//	symbol access to state after being moved.
						store.erase(i);
					}
				}
			});
	}

	//! \since build 855
	static ReductionStatus
	RestoredThunk(TermNode& term, ContextNode& ctx, const TokenValue& n,
		const EnvironmentReference& env_ref)
	{
		if(const auto p_env = env_ref.Lock())
		{
			if(const auto p_f = p_env->LookupName(n))
				return NPL::ResolveRegular<ContextHandler>(*p_f)(term, ctx);
			throw BadIdentifier("Invalid name of shared object found.");
		}
		else
			throw InvalidReference(
				"Invalid reference for shared environment found.");
	}

	//! \since build 780
	YB_NORETURN static ReductionStatus
	ThrowInvalidCyclicReference(TermNode&, ContextNode&)
	{
		throw InvalidReference("Invalid cyclic reference found.");
	}

public:
	PDefH(void, Commit, )
		ImplExpr(Restore(Record, Term))
};


//! \since build 868
//@{
YB_ATTR_nodiscard inline PDefH(Environment&, FetchDefineOrSetEnvironment,
	ContextNode& ctx) ynothrow
	ImplRet(ctx.GetRecordRef())
//! \since build 872
YB_ATTR_nodiscard inline PDefH(Environment&, FetchDefineOrSetEnvironment,
	ContextNode&, const shared_ptr<Environment>& p_env)
	ImplRet(Environment::EnsureValid(p_env))

template<typename _func>
auto
DoDefineSet(TermNode& term, size_t n, _func f) -> decltype(f())
{
	Retain(term);
	if(term.size() > n)
		return f();
	ThrowInvalidSyntaxError("Insufficient terms in definition.");
}

// XXX: Both %YB_FLATTEN are needed in the following code. Otherwise,
//	optimization of G++ 9.1 would try reusing the basic blocks inside the
//	instantiated functions and the resulted code would be inefficient even all
//	calls in the client code of the public interface (e.g.
//	%Forms::DefineWithNoRecursion) are only relying on one of them.

template<typename _func>
YB_FLATTEN auto
DoDefine(TermNode& term, _func f) -> decltype(f(term))
{
	return DoDefineSet(term, 2, [&]{
		RemoveHead(term);

		auto formals(MoveFirstSubterm(term));

		RemoveHead(term);
		return f(formals);
	});
}

template<typename _func>
YB_FLATTEN ReductionStatus
DoSet(TermNode& term, ContextNode& ctx, _func f)
{
	return DoDefineSet(term, 3, [&]{
		RemoveHead(term);

		const auto i(term.begin());

		return ReduceSubsequent(*i, ctx, [&, f, i]{
			auto p_env(ResolveEnvironment(*i).first);

			RemoveHead(term);

			auto formals(MoveFirstSubterm(term));

			RemoveHead(term);
			// NOTE: It is necessary to save %p_env here. The necessary check is
			//	in the call of %FetchDefineOrSetEnvironment.
			return f(formals, std::move(p_env));
		});
	});
}


template<bool = false>
struct DoDefineOrSet final
{
	template<typename... _tParams>
	static ReductionStatus
	Call(TermNode& term, ContextNode& ctx, Environment& env,
		TermNode& formals, _tParams&&... args)
	{
#if NPL_Impl_NPLA1_Enable_Thunked
		// XXX: Terms shall be moved and saved into the actions.
		// TODO: Blocked. Use C++14 lambda initializers to simplify
		//	implementation.
		return ReduceSubsequent(term, ctx,
			std::bind([&](const TermNode& saved, const _tParams&...){
			BindParameter(env, saved, term);
			return ReduceReturnUnspecified(term);
		}, std::move(formals), std::move(args)...));
#else
		yunseq(0, args...);
		// NOTE: This does not support PTC.
		CallImpl(term, ctx, env, formals);
		return ReduceReturnUnspecified(term);
#endif
	}

#if !NPL_Impl_NPLA1_Enable_Thunked
	static void
	CallImpl(TermNode& term, ContextNode& ctx, Environment& env,
		TermNode& formals)
	{
		ReduceOnce(term, ctx);
		BindParameter(env, formals, term);
	}
#endif
};

template<>
struct DoDefineOrSet<true> final
{
	template<typename... _tParams>
	static ReductionStatus
	Call(TermNode& term, ContextNode& ctx, Environment& env,
		TermNode& formals, _tParams&&... args)
	{
#if NPL_Impl_NPLA1_Enable_Thunked
		// XXX: Terms shall be moved and saved into the actions.
		auto p_saved(ShareMoveTerm(formals));

		// TODO: Avoid %shared_ptr.
		// TODO: Blocked. Use C++14 lambda initializers to simplify
		//	implementation.
		return ReduceSubsequent(term, ctx,
			std::bind([&](const shared_ptr<TermNode>&,
			const shared_ptr<RecursiveThunk>& p_gd, const _tParams&...){
			BindParameter(env, p_gd->Term, term);
			// NOTE: This cannot support PTC because only the implicit commit
			//	operation is in the tail context.
			p_gd->Commit();
			return ReduceReturnUnspecified(term);
		}, std::move(p_saved), YSLib::allocate_shared<RecursiveThunk>(
			term.get_allocator(), env, *p_saved), std::move(args)...));
#else
		// NOTE: This does not support PTC.
		RecursiveThunk gd(env, formals);

		yunseq(0, args...);
		DoDefineOrSet<>::CallImpl(term, ctx, env, formals);
		// NOTE: This cannot support PTC because only the implicit commit
		//	operation is in the tail context.
		gd.Commit();
		return ReduceReturnUnspecified(term);
#endif
	}
};


template<bool _bRecur = false>
struct DefineOrSetDispatcher final
{
	TermNode& Term;
	ContextNode& Context;

	template<typename... _tParams>
	inline ReductionStatus
	operator()(TermNode& formals, _tParams&&... args) const
	{
		return DoDefineOrSet<_bRecur>::Call(Term, Context,
			FetchDefineOrSetEnvironment(Context, args...), formals,
			yforward(args)...);
	}
};
//@}


//! \since build 860
//@{
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsIgnore, const TokenValue& s) ynothrow
	ImplRet(s == "#ignore")

YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsIgnoreOrNPLASymbol, const TokenValue& s) ynothrow
	ImplRet(IsIgnore(s) || IsNPLASymbol(s))

YB_NONNULL(2) void
CheckVauSymbol(const TokenValue& s, const char* target, bool is_valid)
{
	if(YB_UNLIKELY(!is_valid))
		ThrowInvalidSyntaxError(ystdex::sfmt("Token '%s' is not a symbol or"
			" '#ignore' expected for %s.", s.data(), target));
}
//@}

//! \since build 799
YB_NORETURN YB_NONNULL(2) void
ThrowInvalidSymbolType(const TermNode& term, const char* n)
{
	ThrowInvalidSyntaxError(ystdex::sfmt("Invalid %s type '%s' found.", n,
		term.Value.type().name()));
}

//! \since build 781
YB_ATTR_nodiscard YB_PURE string
CheckEnvFormal(const TermNode& eterm)
{
	const auto& term(ReferenceTerm(eterm));

	if(const auto p = TermToNamePtr(term))
	{
		if(!IsIgnore(*p))
		{
			CheckVauSymbol(*p, "environment formal parameter",
				IsNPLASymbol(*p));
			return *p;
		}
	}
	else
		ThrowInvalidSymbolType(term, "environment formal parameter");
	return {};
}

#if NPL_Impl_NPLA1_Enable_TCO
//! \since build 827
struct RecordCompressor final
{
	using Tracer = function<bool(Environment&, Environment&)>;
	using RecordInfo = map<lref<Environment>, size_t, ystdex::get_less<>>;
	using ReferenceSet = set<lref<Environment>, ystdex::get_less<>>;

	// XXX: The order of destruction is unspecified.
	vector<shared_ptr<Environment>> Collected;
	lref<Environment> Root;
	ReferenceSet Reachable, NewlyReachable;
	RecordInfo Universe;

	RecordCompressor(Environment& root)
		: RecordCompressor(root, root.Bindings.get_allocator())
	{}
	//! \since build 851
	RecordCompressor(Environment& root, Environment::allocator_type a)
		: Collected(a), Root(root), Reachable({root}, a), NewlyReachable(a),
		Universe(a)
	{
		AddParents(root);
	}

	// NOTE: All checks rely on recursive calls do not support PTC currently.
	//! \since build 860
	void
	AddParents(Environment& e)
	{
		Traverse(e, e.Parent, [this](Environment& dst, Environment&){
			return Universe.emplace(dst, CountReferences(dst)).second;
		});
	}

	void
	Compress()
	{
		// NOTE: This is need to keep the root as external reference.
		const auto p_root(Root.get().shared_from_this());

		// NOTE: Trace.
		for(auto& pr : Universe)
		{
			auto& e(pr.first.get());

			Traverse(e, e.Parent,
				[this](Environment& dst, Environment&) -> bool{
				auto& count(Universe.at(dst));

				YAssert(count > 0, "Invalid count found in trace record.");
				--count;
				return {};
			});
		}
		for(auto i(Universe.cbegin()); i != Universe.cend(); )
			if(i->second > 0)
			{
				// TODO: Blocked. Use ISO C++17 container node API for efficient
				//	implementation.
				NewlyReachable.insert(i->first);
				i = Universe.erase(i);
			}
			else
				++i;
		for(ReferenceSet rs; !NewlyReachable.empty();
			NewlyReachable = std::move(rs))
		{
			for(const auto& e : NewlyReachable)
				Traverse(e, e.get().Parent,
					[&](Environment& dst, Environment&) ynothrow{
					rs.insert(dst);
					Universe.erase(dst);
					return false;
				});
			Reachable.insert(std::make_move_iterator(NewlyReachable.begin()),
				std::make_move_iterator(NewlyReachable.end()));
			for(auto i(rs.cbegin()); i != rs.cend(); )
				if(ystdex::exists(Reachable, *i))
					i = rs.erase(i);
				else
					++i;
		}
		// NOTE: Move to collect.
		for(const auto& pr : Universe)
			// XXX: The envionment would be finally collected after the last
			//	reference is destroyed.
			Collected.push_back(pr.first.get().shared_from_this());

		// TODO: Fully support of PTC by direct DFS traverse.
		ReferenceSet accessed;

		ystdex::retry_on_cond(ystdex::id<>(), [&]() -> bool{
			size_t collected(0);

			Traverse(Root, Root.get().Parent,
				[&](Environment& dst, Environment& src) -> bool{
				if(accessed.insert(src).second)
				{
					if(!ystdex::exists(Universe, ystdex::ref(dst)))
						return true;
					src.Parent = dst.Parent;
					++collected;
				}
				return {};
			});
			return collected != 0;
		});
	}

	static size_t
	CountReferences(const Environment& e) ynothrowv
	{
		const auto acnt(e.GetAnchorCount());

		YAssert(acnt > 0, "Zero anchor count found for environment.");
		return CountStrong(e.shared_from_this()) + size_t(acnt) - 2;
	}

	static size_t
	CountStrong(const shared_ptr<const Environment>& p) ynothrowv
	{
		const long scnt(p.use_count());

		YAssert(scnt > 0, "Zero shared count found for environment.");
		return size_t(scnt);
	}

	static void
	Traverse(Environment& e, const ValueObject& parent, Tracer trace)
	{
		const auto& tp(parent.type());

		if(tp == ystdex::type_id<EnvironmentList>())
		{
			for(const auto& vo : parent.GetObject<EnvironmentList>())
				Traverse(e, vo, trace);
		}
		else if(tp == ystdex::type_id<EnvironmentReference>())
		{
			// XXX: The shared pointer should not be locked to ensure it neutral
			//	to nested call levels.
			if(const auto p
				= parent.GetObject<EnvironmentReference>().Lock().get())
				if(trace(*p, e))
					Traverse(*p, p->Parent, trace);
		}
		else if(tp == ystdex::type_id<shared_ptr<Environment>>())
		{
			// NOTE: The reference count should not be effected here.
			if(const auto p = parent.GetObject<shared_ptr<Environment>>().get())
				if(trace(*p, e))
					Traverse(*p, p->Parent, trace);
		}
	}
};

// NOTE: See $2018-06 @ %Documentation::Workflow and $2019-06 @
//	%Documentation::Workflow for details.
//! \since build 861
void
CompressTCOFrames(ContextNode& ctx, TCOAction& act)
{
	auto& record_list(act.RecordList);
	auto i(record_list.cbegin());

	ystdex::retry_on_cond(ystdex::id<>(), [&]() -> bool{
		const auto orig_size(record_list.size());

		// NOTE: The following code searches the frames to be removed, in the
		//	order from new to old. After merging, the guard slot %EnvGuard owns
		//	the resources of the expression (and its enclosed subexpressions)
		//	being TCO'd.
		i = record_list.cbegin();
		while(i != record_list.cend())
		{
			auto& p_frame_env_ref(get<ActiveEnvironmentPtr>(
				*ystdex::cast_mutable(record_list, i)));

			if(p_frame_env_ref.use_count() != 1
				|| NPL::Deref(p_frame_env_ref).IsOrphan())
				// NOTE: The whole frame is to be removed. The function prvalue
				//	is expected to live only in the subexpression evaluation.
				//	This has equivalent effects of evlis tail recursion.
				i = record_list.erase(i);
			else
				++i;
		}
		return record_list.size() != orig_size;
	});
	RecordCompressor(ctx.GetRecordRef()).Compress();
}

//! \since build 878
//@{
/*!
\brief 准备 TCO 求值。
\param ctx 规约上下文。
\param term 被规约的项。
\param act TCO 动作。
*/
TCOAction&
PrepareTCOEvaluation(ContextNode& ctx, TermNode& term, EnvironmentGuard&& gd)
{
	auto& act(RefTCOAction(ctx));

	[&]{
		// NOTE: If there is no environment set in %act.EnvGuard yet, there is
		//	ideally no need to save the components to the frame record list
		//	for recursive calls. In such case, each operation making
		//	potentionally overwriting of %act.LastFunction will always get into
		//	this call and that time %act.EnvGuard should be set.
		if(act.EnvGuard.func.SavedPtr)
		{
			// NOTE: Operand saving is performed whether the frame compression
			//	is needed, once there is a saved environment set.
			if(auto& p_saved = gd.func.SavedPtr)
			{
				CompressTCOFrames(ctx, act);
				act.AddRecord(std::move(p_saved));
				return;
			}
			// XXX: Normally this should not occur, but this is allowed by the
			//	interface (for an object %EnvironmentSwitcher initialized
			//	without an environment).
		}
		else
			act.EnvGuard = std::move(gd);
		act.AddRecord({});
	}();
	// NOTE: The lift is handled according to the previous status of
	//	%act.LiftCallResult, rather than a seperated boolean value (e.g.
	//	the parameter %no_lift in %RelayForEval).
	act.HandleResultLiftRequest(term, ctx);
	return act;
}

/*!
\brief 按参数设置 TCO 动作提升请求。
\param act TCO 动作。
\param no_lift 指定是否避免保证规约后提升结果。
*/
void
SetupTCOLift(TCOAction& act, bool no_lift)
{
	// NOTE: The %act.LiftCallResult indicates a request for handling during
	//	next time (by %TCOAction::HandleResultLiftRequest call above before the
	//	last one) before %TCOAction is finished. The last request would be
	//	handled by %TCOAction::operator(), which also calls
	//	%TCOAction::HandleResultLiftRequest.
	if(!no_lift)
		act.RequestLiftResult();
}
//@}
#elif NPL_Impl_NPLA1_Enable_Thunked
//! \since build 879
ReductionStatus
MoveGuard(EnvironmentGuard& gd, ContextNode& ctx) ynothrow
{
	const auto egd(std::move(gd));

	return ctx.LastStatus;
}
#endif

//! \since build 878
//@{
template<typename _fNext>
ReductionStatus
RelayForEvalOrDirect(ContextNode& ctx, TermNode& term, EnvironmentGuard&& gd,
	bool no_lift, _fNext&& next)
{
	// XXX: For thunked code, %next shall likely be a %Continuation before being
	//	captured and it is not capturable here. No %SetupNextTerm needs to be
	//	called here. Otherwise, %next is not a %Contiuation and it shall still
	//	handle the capture of the term by itself. The %term is optinonally used
	//	in direct calls instead of the setup next term, while they shall be
	//	equivalent.
#if NPL_Impl_NPLA1_Enable_TCO
	SetupNextTerm(ctx, term);
	SetupTCOLift(PrepareTCOEvaluation(ctx, term, std::move(gd)), no_lift);
	return A1::RelayCurrentOrDirect(ctx, yforward(next), term);
#elif NPL_Impl_NPLA1_Enable_Thunked
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	auto act(std::bind(MoveGuard, std::move(gd), std::placeholders::_1));

	if(no_lift)
		return ReduceCurrentNext(term, ctx, yforward(next), std::move(act));

	// XXX: Term reused. Call of %SetupNextTerm is not needed as the next
	//	term is guaranteed not changed when %next is a continuation.
	Continuation cont([&]{
		// TODO: Avoid fixed continuation parameter.
		return ReduceForLiftedResult(term);
	}, ctx);

	RelaySwitched(ctx, std::move(act));
	return ReduceCurrentNext(term, ctx, yforward(next), std::move(cont));
#else
	yunused(gd);
	SetupNextTerm(ctx, term);

	const auto res(RelayDirect(ctx, next, term));

	return no_lift ? res : ReduceForLiftedResult(term);
#endif
}

// XXX: Same to %RelayForEvalOrDirect, except that %no_lift is always %true.
template<typename _fNext>
ReductionStatus
RelayForEvalOrDirectNoLift(ContextNode& ctx, TermNode& term,
	EnvironmentGuard&& gd, _fNext&& next)
{
	// XXX: See %RelayForEvalOrDirect.
#if NPL_Impl_NPLA1_Enable_TCO
	SetupNextTerm(ctx, term);
	PrepareTCOEvaluation(ctx, term, std::move(gd));
	return A1::RelayCurrentOrDirect(ctx, yforward(next), term);
#elif NPL_Impl_NPLA1_Enable_Thunked
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.	
	return ReduceCurrentNext(term, ctx, yforward(next), std::bind(MoveGuard,
		std::move(gd), std::placeholders::_1));
#else
	yunused(gd);
	SetupNextTerm(ctx, term);
	return A1::RelayDirect(ctx, next, term);
#endif
}
//@}

//! \since build 876
ReductionStatus
RelayForCombine(ContextNode& ctx, TermNode& term, shared_ptr<Environment> p_env)
{
	// NOTE: %ReduceFirst and other passes would not be called again. The
	//	unwrapped combiner should have been evaluated and it would not be
	//	wrongly evaluated again.
	// XXX: Consider optimization when the combiner subterm is known regular.
	// NOTE: Term is set in %A1::RelayForEvalOrDirectNoLift, even for direct
	//	inlining call of %ReduceCombinedImpl.
	return A1::RelayForEvalOrDirectNoLift(ctx, term, EnvironmentGuard(ctx,
		ctx.SwitchEnvironment(std::move(p_env))), std::ref(ReduceCombinedImpl));
}

/*!
\param no_lift 指定是否避免保证规约后提升结果。
\since build 835
*/
//@{
ReductionStatus
EvalImplUnchecked(TermNode& term, ContextNode& ctx, bool no_lift)
{
	const auto i(std::next(term.begin()));
	auto p_env(ResolveEnvironment(NPL::Deref(std::next(i))).first);

	ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		LiftOtherOrCopy(term, nd, NPL::IsMovable(p_ref));
	}, NPL::Deref(i));
	return RelayForEvalOrDirect(ctx, term, EnvironmentGuard(ctx,
		ctx.SwitchEnvironment(std::move(p_env))), no_lift,
		std::ref(ContextState::Access(ctx).ReduceOnce));
}

//! \since build 822
ReductionStatus
EvalImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	RetainN(term, 2);
	return EvalImplUnchecked(term, ctx, no_lift);
}

ReductionStatus
EvalStringImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	RetainN(term, 2);

	auto& expr(*std::next(term.begin()));
	auto unit(SContext::Analyze(Session(Deref(term.get_allocator().resource()),
		NPL::ResolveRegular<const string>(expr)), term.get_allocator()));

	unit.SwapContainer(expr);
	return EvalImplUnchecked(term, ctx, no_lift);
}
//@}


//! \since build 767
class VauHandler final : private ystdex::equality_comparable<VauHandler>
{
private:
	/*!
	\brief 动态环境名称。
	\since build 769
	*/
	string eformal{};
	/*!
	\brief 形式参数对象。
	\invariant \c bool(p_formals) 。
	\since build 771
	*/
	shared_ptr<TermNode> p_formals;
	/*!
	\brief 捕获静态环境作为父环境的引用，包含引入抽象时的静态环境。
	\note 共享所有权用于检查循环引用。
	\since build 823
	*/
	EnvironmentReference parent;
	/*!
	\brief 可选保持所有权的静态环境指针。
	\since build 861
	*/
	mutable shared_ptr<Environment> p_static;
	/*!
	\brief 求值结构。
	\since build 861
	*/
	mutable shared_ptr<TermNode> p_eval_struct;
	/*!
	\brief 调用指针。
	\since build 847
	*/
	ReductionStatus(VauHandler::*p_call)(TermNode&, ContextNode&) const;

public:
	//! \brief 返回时不提升项以允许返回引用。
	bool NoLifting = {};

	/*!
	\pre 间接断言：形式参数对象指针非空。
	\since build 842
	*/
	VauHandler(string&& ename, shared_ptr<TermNode>&& p_fm,
		shared_ptr<Environment>&& p_env, bool owning, TermNode& term, bool nl)
		: eformal(std::move(ename)), p_formals((CheckParameterTree(Deref(p_fm)),
		std::move(p_fm))), parent((Environment::EnsureValid(p_env), p_env)),
		// XXX: Optimize with region inference?
		p_static(owning ? std::move(p_env) : nullptr),
		p_eval_struct(ShareMoveTerm(ystdex::exchange(term,
		TermNode(std::allocator_arg, term.get_allocator(), NoContainer)))),
		p_call(eformal.empty() ? &VauHandler::CallStatic
		: &VauHandler::CallDynamic), NoLifting(nl)
	{}

	//! \since build 824
	friend bool
	operator==(const VauHandler& x, const VauHandler& y)
	{
		return x.eformal == y.eformal && x.p_formals == y.p_formals
			&& x.parent == y.parent && x.p_static == y.p_static
			&& x.NoLifting == y.NoLifting;
	}

	//! \since build 772
	ReductionStatus
	operator()(TermNode& term, ContextNode& ctx) const
	{
		if(IsBranchedList(term))
		{
			if(p_eval_struct)
				return (this->*p_call)(term, ctx);
			else
				// XXX: The call has been performed on the handler, see the
				//	notes in %DoCall.
				throw NPLException("Invalid handler of call found.");
		}
		else
			throw LoggedEvent("Invalid composition found.", Alert);
	}

	//! \since build 799
	static void
	CheckParameterTree(const TermNode& term)
	{
		for(const auto& child : term)
			CheckParameterTree(child);
		if(term.Value)
		{
			if(const auto p = TermToNamePtr(term))
				CheckVauSymbol(*p, "parameter in a parameter tree",
					IsIgnoreOrNPLASymbol(*p));
			else
				ThrowInvalidSymbolType(term, "parameter tree node");
		}
	}

private:
	//! \since build 847
	//@{
	void
	BindEnvironment(ContextNode& ctx, ValueObject&& vo) const
	{
		YAssert(!eformal.empty(), "Empty dynamic environment name found.");
		// NOTE: Bound dynamic environment.
		ctx.GetRecordRef().AddValue(eformal, std::move(vo));
		// NOTE: The dynamic environment is either out of TCO action or
		//	referenced by other environments already in TCO action, so there
		//	is no need to treat as root.
	}

	ReductionStatus
	CallDynamic(TermNode& term, ContextNode& ctx) const
	{
		// NOTE: Evaluation in the local context: using the activation
		//	record frame with outer scope bindings.
		auto wenv(ctx.WeakenRecord());
		// XXX: Reuse of frame cannot be done here unless it can be proved all
		//	bindings would behave as in the old environment, which is too
		//	expensive for direct execution of programs with first-class
		//	environments.
		EnvironmentGuard gd(ctx, NPL::SwitchToFreshEnvironment(ctx));

		BindEnvironment(ctx, std::move(wenv));
		// XXX: Referencing escaped variables (now only parameters need to be
		//	cared) form the context would cause undefined behavior (e.g.
		//	returning a reference to automatic object in the host language). The
		//	lifting of call result is enabled to prevent this, unless
		//	%NoLifting is %true. See also %BindParameter.
		return DoCall(term, ctx, gd);
	}

	ReductionStatus
	CallStatic(TermNode& term, ContextNode& ctx) const
	{
		// NOTE: See above.
		EnvironmentGuard gd(ctx, NPL::SwitchToFreshEnvironment(ctx));

		return DoCall(term, ctx, gd);
	}

	ReductionStatus
	DoCall(TermNode& term, ContextNode& ctx, EnvironmentGuard& gd) const
	{
		YAssert(p_eval_struct, "Invalid evaluation structure found.");

		const bool move(p_eval_struct.use_count() == 1
			&& bool(term.Tags & TermTags::Temporary));

		// NOTE: Since first term is expected to be saved (e.g. by
		//	%ReduceCombined), it is safe to be removed directly.
		RemoveHead(term);
		// NOTE: Forming beta-reducible terms using parameter binding, to
		//	substitute them as arguments for later closure reduction.
		// XXX: Do not lift terms if provable to be safe?
		// NOTE: Since now binding does not relying on temporaries stored
		//	elsewhere (by using %TermTags::Temporary instead), this should be
		//	safe even for TCO.
		BindParameter(ctx.GetRecordRef(), NPL::Deref(p_formals), term);
#if NPL_Impl_NPLA1_TraceVauCall
		ctx.Trace.Log(Debug, [&]{
			return sfmt<string>("Function called, with %ld shared term(s), %ld"
				" %s shared static environment(s), %zu parameter(s).",
				p_eval_struct.use_count(), parent.GetPtr().use_count(),
				p_static ? "owning" : "nonowning", p_formals->size());
		});
#endif
		// NOTE: The static environment is bound as the base of the local
		//	environment by setting the parent environment pointer.
		ctx.GetRecordRef().Parent = parent;
#if NPL_Impl_NPLA1_Enable_TCO
		// XXX: Assume the last function is this object.
		// TODO: Avoid TCO when the static environment has something with side
		//	effects on destruction.
		if(move)
		{
			TermNode eval_struct(std::move(Deref(p_eval_struct)));
			auto& act(RefTCOAction(ctx));

			if(p_static && p_static.use_count() == 1)
				act.RecordList.emplace_front(ContextHandler(),
					std::move(p_static));

			const bool no_lifting(NoLifting);

			// XXX: This would make '*this' invalid.
			yunused(act.MoveFunction());
			// XXX: The evaluation structure does not need to be saved, since it
			//	would be used immediately in %RelayForCall. The pointer is moved
			//	to indicate the error condition when it is called again.
			LiftOther(term, eval_struct);
			return RelayForCall(ctx, term, std::move(gd), no_lifting);
		}
		else
			term.SetContent(Deref(p_eval_struct));
#else
		// XXX: Ditto.
		LiftOtherOrCopy(term, Deref(p_eval_struct), move);
#endif
		return RelayForCall(ctx, term, std::move(gd), NoLifting);
	}
	//@}
};


//! \since build 874
YB_FLATTEN void
MakeValueListOrMove(TermNode& term, TermNode& nd,
	ResolvedTermReferencePtr p_ref)
{
	MakeValueOrMove(p_ref, [&]{
		for(const auto& sub : nd)
			term.Add(sub);
	}, [&]{
		// XXX: No cyclic reference check.
		term.GetContainerRef().splice(term.end(),
			std::move(nd.GetContainerRef()));
	});
}

//! \since build 859
//@{
YB_NORETURN void
ThrowConsError(TermNode& nd, ResolvedTermReferencePtr p_ref)
{
	throw ListTypeError(ystdex::sfmt(
		"Expected a list for the 2nd argument, got '%s'.",
		TermToStringWithReferenceMark(nd, p_ref).c_str()));
}

void
ConsItem(TermNode& term, TermNode& y)
{
	ResolveTerm([&](TermNode& nd_y, ResolvedTermReferencePtr p_ref){
		if(IsList(nd_y))
			MakeValueListOrMove(term, nd_y, p_ref);
		else
			ThrowConsError(nd_y, p_ref);
	}, y);
}

void
ConsImpl(TermNode& term)
{
	RetainN(term, 2);

	const auto i(std::next(term.begin(), 2));

	ConsItem(term, *i);
	term.erase(i);
	RemoveHead(term);
}
//@}


//! \since build 859
//@{
class RefContextHandler : private ystdex::equality_comparable<RefContextHandler>
{
private:
#if NPL_NPLA_CheckEnvironmentReferenceCount
	//! \since build 876
	EnvironmentReference environment_ref;
#else
	// XXX: The anchor pointer here is for more efficient native code generation
	//	(at least with x86_64-pc-linux G++ 9.2), though there are still more
	//	than necessary memory allocations should have been better avoided.
	AnchorPtr anchor_ptr;
#endif

public:
	lref<const ContextHandler> HandlerRef;

	//! \since build 869
	RefContextHandler(const ContextHandler& h,
		const EnvironmentReference& env_ref) ynothrow
#if NPL_NPLA_CheckEnvironmentReferenceCount
		: environment_ref(env_ref), HandlerRef(h)
#else
		: anchor_ptr(env_ref.GetAnchorPtr()), HandlerRef(h)
#endif
	{}
	DefDeCopyMoveCtorAssignment(RefContextHandler)

	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const RefContextHandler& x, const RefContextHandler& y)
		ImplRet(x.HandlerRef.get() == y.HandlerRef.get())

	PDefHOp(ReductionStatus, (), TermNode& term, ContextNode& ctx) const
		ImplRet(HandlerRef.get()(term, ctx))

// XXX: This is currently unused and Clang++ would complain with
//	[-Wunused-function].
#if !YB_IMPL_CLANGPP
#	if NPL_NPLA_CheckEnvironmentReferenceCount
	friend DefSwap(ynothrow, RefContextHandler,
		(std::swap(_x.environment_ref, _y.environment_ref),
		std::swap(_x.HandlerRef, _y.HandlerRef)))
#	else
	friend DefSwap(ynothrow, RefContextHandler,
		(std::swap(_x.anchor_ptr, _y.anchor_ptr),
		std::swap(_x.HandlerRef, _y.HandlerRef)))
#	endif
#endif
};


YB_NORETURN ReductionStatus
ThrowForUnwrappingFailure(const ContextHandler& h)
{
	throw TypeError(ystdex::sfmt("Unwrapping failed with type '%s'.",
		h.target_type().name()));
}

//! \since build 878
ReductionStatus
ReduceForCombinerRef(TermNode& term, ContextNode& ctx,
	const TermReference& ref, const ContextHandler& h, size_t n)
{
	const auto& r_env(FetchContextOrTailEnvironmentReference(ref, ctx));
	const auto a(term.get_allocator());
	auto p_sub(YSLib::allocate_shared<TermNode>(a, TermNode(std::allocator_arg,
		a, NoContainer, ContextHandler(FormContextHandler(RefContextHandler(h,
		r_env), n)))));
	auto& sub(NPL::Deref(p_sub));
	TermNode tm(std::allocator_arg, a, {{std::allocator_arg, a, NoContainer,
		std::move(p_sub)}});

	tm.Value = TermReference(sub, r_env);
	term.SetContent(std::move(tm));
	return ReductionStatus::Retained;
}

ReductionStatus
UnwrapResolved(TermNode& term, ContextNode& ctx, FormContextHandler& fch,
	ResolvedTermReferencePtr p_ref)
{
	if(fch.Wrapping != 0)
	{
		return MakeValueOrMove(p_ref, [&]{
			return ReduceForCombinerRef(term, ctx, NPL::Deref(p_ref),
				fch.Handler, fch.Wrapping - 1);
		}, [&]{
			--fch.Wrapping;
			term.Value = ContextHandler(std::move(fch));
			return ReductionStatus::Clean;
		});
	}
	throw TypeError("Unwrapping failed on an operative argument.");
}

template<typename _func, typename _func2>
ReductionStatus
DispatchContextHandler(ContextHandler& h, ResolvedTermReferencePtr p_ref,
	_func f, _func2 f2)
{
	if(const auto p = h.target<FormContextHandler>())
		return f(*p, p_ref);
	return ystdex::make_expanded<ReductionStatus(ContextHandler&,
		const ResolvedTermReferencePtr&)>(f2)(h, p_ref);
}

template<typename _func, typename _func2>
ReductionStatus
WrapUnwrapResolve(TermNode& term, _func f, _func2 f2)
{
	return NPL::ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		NPL::CheckRegular<ContextHandler>(nd, p_ref);
		return DispatchContextHandler(NPL::Access<ContextHandler>(nd), p_ref, f,
			f2);
	}, term);
}
//@}


//! \since build 876
//@{
template<typename... _tParams>
TermNode&
AddTrailingTemporary(TermNode& term, _tParams&&... args)
{
	return *term.emplace(yforward(args)...);
}

YB_ATTR_nodiscard YB_PURE TermTags
BindReferenceTags(const TermReference& ref) ynothrow
{
	auto ref_tags(GetLValueTagsOf(ref.GetTags()));

	// NOTE: An xvalue should also be bindable as a prvalue. Note xvalue is
	//	still distinguishable by both tag enumerators than one, which is not
	//	like variables bound by universal references in C++.
	return bool(ref_tags & TermTags::Unique) ? ref_tags | TermTags::Temporary
		: ref_tags;
}

template<typename _fMove>
void
BindMoveLocalReference(TermNode& o, _fMove mv)
{
	// NOTE: Simplified from %BindParameterObject::operator().
	if(const auto p = NPL::TryAccessLeaf<TermReference>(o))
		// NOTE: Reference collapsed by move.
		mv(std::move(o.GetContainerRef()),
			TermReference(BindReferenceTags(*p), std::move(*p)));
	else
		// NOTE: As %MarkTemporaryTerm;
		(mv(std::move(o.GetContainerRef()),
			std::move(o.Value))).Tags |= TermTags::Temporary;
}

template<typename _fMove>
void
BindNonmovableLocalReference(TermTags o_tags, TermNode& o, _fMove mv,
	const EnvironmentReference& r_env)
{
	// NOTE: Simplified from %BindParameterObject::operator().
	mv(TermNode::Container(o.GetContainer()), [&]{
		if(const auto p = NPL::TryAccessLeaf<TermReference>(o))
			return TermReference(BindReferenceTags(*p), *p);
		return TermReference(GetLValueTagsOf(o.Tags | o_tags), o, r_env);
	}());
}

// NOTE: As %PrepareCollapse without frozen environment protection and no
//	preservation of the operand term.
TermNode
EvaluateBoundValue(TermNode& term, Environment& env)
{
	if(const auto p = NPL::TryAccessLeaf<const TermReference>(term))
		return std::move(term);
	return NPL::AsTermNode(term.get_allocator(), TermReference(term.Tags, term,
		env.shared_from_this()));
}

TermNode
EvaluateBoundLValue(TermNode& term, Environment& env)
{
	if(const auto p = NPL::TryAccessLeaf<TermReference>(term))
	{
		*p = EnsureLValueReference(std::move(*p));
		return std::move(term);
	}
	return NPL::AsTermNode(term.get_allocator(), EnsureLValueReference(
		TermReference(term.Tags, term, env.shared_from_this())));
}
//@}

//! \since build 875
//@{
void
ForwardCombiner(TermNode& comb, ContextNode& ctx)
{
	using namespace std::placeholders;

	RegularizeTerm(comb, WrapUnwrapResolve(comb, std::bind(UnwrapResolved,
		std::ref(comb), std::ref(ctx), _1, _2), ThrowForUnwrappingFailure));
}

// NOTE: As %ReduceSubsequent.
template<typename _fCurrent>
ReductionStatus
ReduceCallSubsequent(TermNode& term, ContextNode& ctx,
	shared_ptr<Environment> p_env, _fCurrent&& next)
{
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	return A1::ReduceCurrentNext(term, ctx,
		std::bind([&](shared_ptr<Environment>& p_e){
#	if NPL_Impl_NPLA1_Enable_TCO
		// TODO: Optimize with known combiner calls. Ideally %EnsureTCOAction
		//	should not be called later in %CombinerReturnThunk where the term is
		//	actually a combiner call.
		yunused(EnsureTCOAction(ctx, term));
#	endif
		return RelayForCombine(ctx, term, std::move(p_e));
	}, std::move(p_env)), yforward(next));
}

//! \since build 874
template<typename _func>
YB_FLATTEN ReductionStatus
FirstOrVal(TermNode& term, _func f)
{
	return CallResolvedUnary([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		if(IsBranchedList(nd))
			return f(AccessFirstSubterm(nd), p_ref);
		ThrowInsufficientTermsError();
	}, term);
}
//@}

//! \since build 859
void
CheckResolvedListReference(TermNode& nd, bool has_ref)
{
	if(has_ref)
	{
		if(YB_UNLIKELY(!IsBranchedList(nd)))
			throw ListTypeError(ystdex::sfmt(
				"Expected a non-empty list for the 1st argument, got '%s'.",
				TermToStringWithReferenceMark(nd, true).c_str()));
	}
	else
		ThrowValueCategoryErrorForFirstArgument(nd);
}

//! \since build 874
template<typename _func>
YB_FLATTEN ReductionStatus
FirstAtRef(TermNode& term, _func f)
{
	return CallResolvedUnary([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		CheckResolvedListReference(nd, p_ref);
		// XXX: This should be safe, since the parent list is guaranteed an
		//	lvalue by %CheckResolvedListReference.
		return f(term, AccessFirstSubterm(nd), p_ref);
	}, term);
}

//! \since build 834
template<typename _func>
void
SetFirstRest(_func f, TermNode& term)
{
	RetainN(term, 2);

	auto i(term.begin());

	ResolveTerm([&](TermNode& nd_x, bool has_ref){
		CheckResolvedListReference(nd_x, has_ref);
		return f(nd_x, *++i);
	}, *++i);
	term.Value = ValueToken::Unspecified;
}

//! \since build 868
template<typename _func>
void
DoSetFirst(TermNode& term, _func f)
{
	SetFirstRest([f](TermNode& nd_x, TermNode& y){
		auto& dst(AccessFirstSubterm(nd_x));

		if(const auto p = NPL::TryAccessLeaf<TermReference>(y))
			f(dst, y, *p);
		else
			LiftTerm(dst, y);
	}, term);
}

//! \since build 853
void
LiftNoOp(TermNode&) ynothrow
{}

//! \since build 853
void
SetRestImpl(TermNode& term, void(&lift)(TermNode&))
{
	SetFirstRest([&lift](TermNode& nd_x, TermNode& y){
		ResolveTerm([&](TermNode& nd_y, ResolvedTermReferencePtr p_ref_y){
			// XXX: How to simplify? Merge with %BindParameterObject?
			if(IsList(nd_y))
			{
				const auto a(nd_x.get_allocator());
				TermNode nd_new(std::allocator_arg, a, {TermNode(a)});

				MakeValueListOrMove(nd_new, nd_y, p_ref_y);
				lift(nd_new);
				// XXX: The order is significant.
				AccessFirstSubterm(nd_new) = MoveFirstSubterm(nd_x);
				swap(nd_x, nd_new);
			}
			else
				throw ListTypeError(ystdex::sfmt("Expected a list for"
					" the 2nd argument, got '%s'.",
					TermToStringWithReferenceMark(nd_y, p_ref_y).c_str()));
		}, y);
	}, term);
}


//! \since build 867
template<typename _func>
void
UndefineImpl(TermNode& term, _func f)
{
	Retain(term);
	if(term.size() == 2)
	{
		const auto&
			n(NPL::ResolveRegular<const TokenValue>(*std::next(term.begin())));

		if(IsNPLASymbol(n))
			f(n);
		else
			ThrowInvalidSyntaxError(ystdex::sfmt("Invalid token '%s' found as"
				" name to be undefined.", n.c_str()));
	}
	else
		ThrowInvalidSyntaxError(
			"Expected exact one term as name to be undefined.");
}

//! \since build 840
//@{
template<typename _func>
ReductionStatus
CreateFunction(TermNode& term, _func f, size_t n)
{
	Retain(term);
	if(term.size() > n)
		return f();
	ThrowInvalidSyntaxError("Insufficient terms in function abstraction.");
}

ReductionStatus
LambdaImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return CreateFunction(term, [&, no_lift]{
		auto p_env(ctx.ShareRecord());
		auto i(term.begin());
		auto formals(ShareMoveTerm(NPL::Deref(++i)));

		term.erase(term.begin(), ++i);
		// NOTE: The wrapping count 1 implies strict evaluation of arguments in
		//	%FormContextHandler::operator().
		term.Value = ContextHandler(FormContextHandler(VauHandler({},
			std::move(formals), std::move(p_env), {}, term, no_lift), 1));
		return ReductionStatus::Clean;
	}, 1);
}

//! \since build 842
ContextHandler
CreateVau(TermNode& term, bool no_lift, TNIter i,
	shared_ptr<Environment>&& p_env, bool owning)
{
	auto formals(ShareMoveTerm(NPL::Deref(++i)));
	auto eformal(CheckEnvFormal(NPL::Deref(++i)));

	term.erase(term.begin(), ++i);
	return FormContextHandler(VauHandler(std::move(eformal), std::move(formals),
		std::move(p_env), owning, term, no_lift));
}

ReductionStatus
VauImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return CreateFunction(term, [&, no_lift]{
		term.Value = CreateVau(term, no_lift, term.begin(), ctx.ShareRecord(),
			{});
		return ReductionStatus::Clean;
	}, 2);
}

ReductionStatus
VauWithEnvironmentImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return CreateFunction(term, [&, no_lift]{
		auto i(term.begin());
		auto& tm(NPL::Deref(++i));

		return ReduceSubsequent(tm, ctx, [&, i, no_lift]{
			// XXX: List components are ignored.
			auto p_env_pr(ResolveEnvironment(tm));

			term.Value = CreateVau(term, no_lift, i,
				std::move(p_env_pr.first), p_env_pr.second);
			return ReductionStatus::Clean;
		});
	}, 3);
}
//@}

//! \since build 873
inline PDefH(void, CopyTermTags, TermNode& term, const TermNode& tm) ynothrow
	ImplExpr(term.Tags = GetLValueTagsOf(tm.Tags))

//! \since build 858
void
MarkTemporaryTerm(TermNode& term, char sigil) ynothrow
{
	if(sigil != char())
		// XXX: This is like lifetime extension of temporary objects with rvalue
		//	references in the host language.
		term.Tags |= TermTags::Temporary;
}

//! \since build 828
struct BindParameterObject
{
	// XXX: This is actually used for references of lvalues and it
	//	is never accurate (see below). It is still necessary to avoid release
	//	TCO frame records too early in next function calls.
	//! \since build 869
	EnvironmentReference Referenced;

	//! \since build 869
	BindParameterObject(Environment& e)
		: Referenced(e.shared_from_this())
	{}

	//! \since build 858
	template<typename _fCopy, typename _fMove>
	void
	operator()(char sigil, TermTags o_tags, TermNode& o, _fCopy cp, _fMove mv)
		const
	{
		const bool temp(bool(o_tags & TermTags::Temporary));

		// NOTE: The binding rules here should be carefully tweaked to make them
		//	exactly accept expression representations (in %TermNode) in NPLA1
		//	all around.
		if(sigil != '@')
		{
			const bool can_modify(!bool(o_tags & TermTags::Nonmodifying));

			// NOTE: Subterms in arguments retained are also transferred for
			//	values.
			if(const auto p = NPL::TryAccessLeaf<TermReference>(o))
			{
				if(sigil != char())
				{
					const auto ref_tags(sigil == '&' ? BindReferenceTags(*p)
						: p->GetTags());

					if(can_modify && temp)
						// NOTE: Reference collapsed by move.
						mv(std::move(o.GetContainerRef()),
							TermReference(ref_tags, std::move(*p)));
					else
						// NOTE: Reference collapsed by copy.
						mv(TermNode::Container(o.GetContainer()),
							TermReference(ref_tags, *p));
				}
				else
				{
					auto& src(p->get());

					// NOTE: Since it is passed by value copy, direct
					//	destructive lifting cannot be used.
					if(!p->IsMovable())
						cp(src);
					else
						mv(std::move(src.GetContainerRef()),
							std::move(src.Value));
				}
			}
			else if((can_modify || sigil == '%') && temp)
				// XXX: The object is bound to reference as temporary, implying
				//	copy elision in the object language.
				MarkTemporaryTerm(mv(std::move(o.GetContainerRef()),
					std::move(o.Value)), sigil);
			// NOTE: Binding on list prvalues is always unsafe. However,
			//	not %can_modify currently implies some ancestor of %o is a
			//	referenceable object, albeit not always an lvalue (which implies
			//	undefined behavior if used as an lvalue).
			else if(sigil == '&')
				// XXX: Always move because the value object is newly created.
				//	The anchor here (if any) is not accurate because it refers
				//	to the anchor saved by the reference (if any), not
				//	necessarily the original environment owning the referent.
				// TODO: Remove anchors without TCO (as it always interferes
				//	%NPL_NPLA_CheckEnvironmentReferenceCount for no
				//	benefits)?
				mv(TermNode::Container(o.get_allocator()),
					// XXX: Term tags on prvalues are reserved and should be
					//	ignored normally except for future internal use. Note
					//	that %TermTags::Temporary can be provided by a bound
					//	object (indicated by %o) in an environment.
					TermReference(GetLValueTagsOf(o.Tags | o_tags), o,
					Referenced));
			else
				cp(o);
		}
		else if(!temp)
			mv(TermNode::Container(o.get_allocator()),
				TermReference(o_tags & TermTags::Nonmodifying, o, Referenced));
		else
			throw
				InvalidReference("Invalid operand found on binding sigil '@'.");
	}
};


//! \since build 863
class ParameterMatcher
{
public:
	template<typename... _tParams>
	using GBinder = function<void(_tParams..., TermTags, Environment&)>;
	using Action = function<void()>;

	GBinder<TNIter, TNIter, const TokenValue&> BindTrailing;
	GBinder<const TokenValue&, TermNode&> BindValue;

private:
	// XXX: Allocators are not used here for performance in most cases.
	mutable Action act{};

public:
	template<class _type, class _type2>
	ParameterMatcher(_type&& arg, _type2&& arg2)
		: BindTrailing(yforward(arg)), BindValue(yforward(arg2))
	{}

	// NOTE: The environment argument is for rvalues in the caller. All prvalues
	//	are treated as rvalue references introduced in the current environment.
	//	This is necessary to keep the environment alive in the TCO
	//	implementation.
	// XXX: Currently there is no other effects (esp. collection) in binding,
	//	so without handling of the environment for TCO, it still works. Anyway,
	//	keeping it saner to be friendly to possible TCO compressions in future
	//	seems OK.
	//! \since build 869
	//@{
	void
	operator()(const TermNode& t, TermNode& o, TermTags o_tags,
		Environment& env) const
	{
		// NOTE: This is a trampoline to eliminate the call depth limitation.
		Match(t, o, o_tags, env);
		while(act)
		{
			const auto a(std::move(act));

			a();
		}
	}

private:
	void
	Match(const TermNode& t, TermNode& o, TermTags o_tags, Environment& env)
		const
	{
		if(IsList(t))
		{
			if(IsBranch(t))
			{
				const auto n_p(t.size());
				auto last(t.end());

				if(n_p > 0)
				{
					const auto& back(NPL::Deref(std::prev(last)));

					// NOTE: Empty list lvalue arguments shall not be matched
					//	here.
					if(IsLeaf(back))
					{
						if(const auto p = NPL::TryAccessLeaf<TokenValue>(back))
						{
							if(!p->empty() && p->front() == '.')
								--last;
						}
						else if(!IsList(back))
							throw ParameterMismatch(ystdex::sfmt(
								"Invalid term '%s' found for symbol parameter.",
								TermToString(back).c_str()));
					}
				}
				// XXX: There is only one level of indirection. It should work
				//	with the correct implementation of the reference collapse.
				ResolveTerm([&, n_p, o_tags](TermNode& nd,
					ResolvedTermReferencePtr p_ref){
					const bool ellipsis(last != t.end());
					const auto n_o(nd.size());

					if(n_p == n_o || (ellipsis && n_o >= n_p - 1))
					{
						auto tags(o_tags);

						// NOTE: All tags as type qualifiers should be checked
						//	here. Currently only glvalues can be qualified.
						// XXX: Term tags are currently not respected in
						//	prvalues.
						if(p_ref)
						{
							const auto ref_tags(p_ref->GetTags());

							tags = (tags
								& ~(TermTags::Unique | TermTags::Temporary))
								| (ref_tags & TermTags::Unique);
							tags |= ref_tags & TermTags::Nonmodifying;
						}
						MatchSubterms(t.begin(), last, nd.begin(), nd.end(),
							tags, p_ref ? FetchTailEnvironment(*p_ref) : env,
							ellipsis
#if NPL_Impl_NPLA1_AssertParameterMatch
							, t.end()
#endif
							);
					}
					else if(!ellipsis)
						throw ArityMismatch(n_p, n_o);
					else
						ThrowInsufficientTermsError();
				}, o);
			}
			else
				ResolveTerm([&](const TermNode& nd, bool has_ref){
					if(nd)
						throw ParameterMismatch(ystdex::sfmt("Invalid nonempty"
							" operand value '%s' found for empty list"
							" parameter.", TermToStringWithReferenceMark(nd,
							has_ref).c_str()));
				}, o);
		}
		else
		{
			const auto& tp(t.Value.type());
		
			if(tp == ystdex::type_id<TermReference>())
				UpdateAction([&, o_tags]{
					Match(t.Value.GetObject<TermReference>().get(), o, o_tags,
						env);
				});
			else if(tp == ystdex::type_id<TokenValue>())
				BindValue(t.Value.GetObject<TokenValue>(), o, o_tags, env);
			else
				throw ParameterMismatch(ystdex::sfmt("Invalid parameter value"
					" '%s' found.", TermToString(t).c_str()));
		}
	}

	void
	MatchSubterms(TNCIter i, TNCIter last, TNIter j, TNIter o_last,
		TermTags tags, Environment& env, bool ellipsis
#if NPL_Impl_NPLA1_AssertParameterMatch
		, TNCIter t_end
#endif
		) const
	{
		if(i != last)
		{
			UpdateAction(std::bind(&ParameterMatcher::MatchSubterms, this,
				std::next(i), last, std::next(j), o_last, tags, std::ref(env),
				ellipsis
#if NPL_Impl_NPLA1_AssertParameterMatch
				, t_end
#endif
				));
			YAssert(j != o_last, "Invalid state of operand found.");
			Match(NPL::Deref(i), NPL::Deref(j), tags, env);
		}
		else if(ellipsis)
		{
			const auto& lastv(NPL::Deref(last).Value);

			YAssert(lastv.type() == ystdex::type_id<TokenValue>(),
				"Invalid ellipsis sequence token found.");
			BindTrailing(j, o_last, lastv.GetObject<TokenValue>(), tags, env);
#if NPL_Impl_NPLA1_AssertParameterMatch
			YAssert(std::next(last) == t_end, "Invalid state found.");
#endif
		}
	}
	//@}

	template<typename _func>
	void
	UpdateAction(_func f) const
	{
		// TODO: Blocked. Use C++14 lambda initializers to simplify
		//	implementation.
		act = std::bind([this, f](Action& a){
			act = std::move(a);
			f();
		}, std::move(act));
	}
};


//! \since build 859
//@{
size_t
AddWrapperCount(size_t n)
{
	++n;
	if(n != 0)
		return n;
	throw NPLException("Wrapping count overflow detected.");
}

YB_NORETURN ReductionStatus
ThrowForWrappingFailure(const ystdex::type_info& tp)
{
	throw TypeError(ystdex::sfmt("Wrapping failed with type '%s'.", tp.name()));		
}

ReductionStatus
WrapH(TermNode& term, ContextHandler h)
{
	// XXX: Allocators are not used here for performance in most cases.
	term.Value = std::move(h);
	return ReductionStatus::Clean;
}

ReductionStatus
WrapN(TermNode& term, ContextNode&, ResolvedTermReferencePtr p_ref,
	FormContextHandler& fch, size_t n)
{
	return WrapH(term, MakeValueOrMove(p_ref, [&]{
		return FormContextHandler(fch.Handler, n);
	}, [&]{
		return
			FormContextHandler(std::move(fch.Handler), n);
	}));
}

ReductionStatus
WrapRefN(TermNode& term, ContextNode& ctx, ResolvedTermReferencePtr p_ref,
	FormContextHandler& fch, size_t n)
{
	if(p_ref)
		return ReduceForCombinerRef(term, ctx, *p_ref, fch.Handler,
			n);
	term.Value = ContextHandler(FormContextHandler(std::move(fch.Handler), n));
	return ReductionStatus::Clean;
}

template<typename _func, typename _func2>
YB_FLATTEN ReductionStatus
WrapUnwrap(TermNode& term, _func f, _func2 f2)
{
	return CallRegularUnaryAs<ContextHandler>(
		[&](ContextHandler& h, ResolvedTermReferencePtr p_ref){
		return DispatchContextHandler(h, p_ref, f, f2);
	}, term);
}

template<typename _func>
ReductionStatus
WrapOrRef(TermNode& term, ContextNode& ctx, ReductionStatus
	(&fwrap)(TermNode&, ContextNode&, ResolvedTermReferencePtr,
	FormContextHandler&, size_t), _func f)
{
	return WrapUnwrap(term,
		[&](FormContextHandler& fch, ResolvedTermReferencePtr p_ref){
		return fwrap(term, ctx, p_ref, fch, AddWrapperCount(fch.Wrapping));
	}, f);
}

ReductionStatus
WrapOnceOrOnceRef(TermNode& term, ContextNode& ctx, ReductionStatus
	(&fwrap)(TermNode&, ContextNode&, ResolvedTermReferencePtr,
	FormContextHandler&, size_t))
{
	return WrapUnwrap(term,
		[&](FormContextHandler& fch, ResolvedTermReferencePtr p_ref){
		return fch.Wrapping == 0 ? fwrap(term, ctx, p_ref, fch, 1)
			: ThrowForWrappingFailure(
			ystdex::type_id<FormContextHandler>());
	}, [](const ContextHandler& h) YB_ATTR_LAMBDA(noreturn) -> ReductionStatus{
		ThrowForWrappingFailure(h.target_type());
	});
}
//@}


//! \since build 834
//@{
class EncapsulationBase
{
private:
	// XXX: Is it possible to support %TermReference safety check here with
	//	anchors?
	// TODO: Add naming scheme and persistence interoperations?
	shared_ptr<void> p_type;

public:
	//! \since build 849
	EncapsulationBase(shared_ptr<void> p) ynothrow
		: p_type(std::move(p))
	{}
	DefDeCopyMoveCtorAssignment(EncapsulationBase)

	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const EncapsulationBase& x, const EncapsulationBase& y) ynothrow
		ImplRet(x.p_type == y.p_type)

	DefGetter(const ynothrow, const EncapsulationBase&, , *this)
	DefGetter(ynothrow, EncapsulationBase&, Ref, *this)
	DefGetter(const ynothrow, const shared_ptr<void>&, Type, p_type)
};


class Encapsulation final : private EncapsulationBase
{
public:
	mutable TermNode Term;

	Encapsulation(shared_ptr<void> p, TermNode term)
		: EncapsulationBase(std::move(p)), Term(std::move(term))
	{}
	DefDeCopyMoveCtorAssignment(Encapsulation)

	friend PDefHOp(bool, ==, const Encapsulation& x, const Encapsulation& y)
		ynothrow
		ImplRet(x.Get() == y.Get())

	using EncapsulationBase::Get;
	using EncapsulationBase::GetType;
};


class Encapsulate final : private EncapsulationBase
{
public:
	Encapsulate(shared_ptr<void> p)
		: EncapsulationBase(std::move(p))
	{}
	DefDeCopyMoveCtorAssignment(Encapsulate)

	friend PDefHOp(bool, ==, const Encapsulate& x, const Encapsulate& y)
		ynothrow
		ImplRet(x.Get() == y.Get())

	void
	operator()(TermNode& term) const
	{
		CallUnary([this](TermNode& tm) YB_PURE{
			return Encapsulation(GetType(), ystdex::invoke_value_or(
				&TermReference::get, NPL::TryAccessReferencedLeaf<
				const TermReference>(tm), std::move(tm)));
		}, term);
	}
};


class Encapsulated final : private EncapsulationBase
{
public:
	Encapsulated(shared_ptr<void> p)
		: EncapsulationBase(std::move(p))
	{}
	DefDeCopyMoveCtorAssignment(Encapsulated)

	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const Encapsulated& x, const Encapsulated& y) ynothrow
		ImplRet(x.Get() == y.Get())

	void
	operator()(TermNode& term) const
	{
		CallUnary([this](TermNode& tm) YB_ATTR_LAMBDA(pure) -> bool{
			return ystdex::call_value_or([this](const Encapsulation& enc)
				YB_ATTR_LAMBDA_QUAL(ynothrow, YB_PURE){
				return Get() == enc.Get();
			}, NPL::TryAccessReferencedTerm<Encapsulation>(tm));
		}, term);
	}
};


class Decapsulate final : private EncapsulationBase
{
public:
	Decapsulate(shared_ptr<void> p)
		: EncapsulationBase(std::move(p))
	{}
	DefDeCopyMoveCtorAssignment(Decapsulate)

	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const Decapsulate& x, const Decapsulate& y) ynothrow
		ImplRet(x.Get() == y.Get())

	//! \since build 856
	ReductionStatus
	operator()(TermNode& term, ContextNode& ctx) const
	{
		return CallRegularUnaryAs<const Encapsulation>(
			[&](const Encapsulation& enc, ResolvedTermReferencePtr p_ref){
			auto& tm(enc.Term);

			return MakeValueOrMove(p_ref, [&]() -> ReductionStatus{
				// NOTE: As lvalue references, the object in %tm cannot be
				//	destroyed.
				if(const auto p = NPL::TryAccessLeaf<const TermReference>(tm))
					term.SetContent(tm.GetContainer(), *p);
				else
				{
					// XXX: Subterms cleanup is deferred.
					term.Value = TermReference(tm,
						FetchContextOrTailEnvironmentReference(*p_ref, ctx));
					return ReductionStatus::Clean;
				}
				return ReductionStatus::Retained;
			}, [&]{
				LiftTerm(term, tm);		
				return ReductionStatus::Retained;
			});
		}, term);
	}
};
//@}


//! \since build 859
ReductionStatus
ApplyImpl(TermNode& term, ContextNode& ctx, shared_ptr<Environment> p_env)
{
	using namespace std::placeholders;
	auto i(term.begin());
	auto& comb(NPL::Deref(++i));

	RegularizeTerm(comb, WrapUnwrapResolve(comb, std::bind(UnwrapResolved,
		std::ref(comb), std::ref(ctx), _1, _2), ThrowForUnwrappingFailure));

	TermNode expr(std::allocator_arg, term.get_allocator(), {std::move(comb)});

	ConsItem(expr, NPL::Deref(++i));
	term = std::move(expr);
	return RelayForCombine(ctx, term, std::move(p_env));
}

//! \since build 860
void
ListAsteriskImpl(TermNode& term)
{
	Retain(term);

	auto i(std::next(term.begin()));

	if(i != term.end())
	{
		auto& head(NPL::Deref(++i));

		if(i != term.end())
		{
			const auto last(std::prev(term.end()));

			ConsItem(term, Deref(last));
			term.erase(last);
		}
		else
			LiftOther(term, head);
		RemoveHead(term);
	}
	else
		ThrowInsufficientTermsError();
}

} // unnamed namespace;


ContextState::ContextState(pmr::memory_resource& rsrc)
	: ContextNode(rsrc)
{}
ContextState::ContextState(const ContextState& ctx)
	: ContextNode(ctx),
	EvaluateLeaf(ctx.EvaluateLeaf), EvaluateList(ctx.EvaluateList),
	EvaluateLiteral(ctx.EvaluateLiteral), Guard(ctx.Guard)
{}
ContextState::ContextState(ContextState&& ctx)
	: ContextNode(std::move(ctx)),
	EvaluateLeaf(ctx.EvaluateLeaf), EvaluateList(ctx.EvaluateList),
	EvaluateLiteral(ctx.EvaluateLiteral), Guard(ctx.Guard)
{
	swap(next_term_ptr, ctx.next_term_ptr);
}
ContextState::ImplDeDtor(ContextState)

TermNode&
ContextState::GetNextTermRef() const
{
	if(const auto p = next_term_ptr)
		return *p;
	// NOTE: This should not occur unless there exists some invalid low-level
	//	interoperations on the next term pointer in the context.
	throw NPLException("No next term found to evaluation.");
}

void
ContextState::SetNextTermRef(TermNode& term)
{
	next_term_ptr = NPL::make_observer(&term);
}

ReductionStatus
ContextState::DefaultReduceOnce(TermNode& term, ContextNode& ctx)
{
	auto& cs(ContextState::Access(ctx));
	const bool non_list(term.Value);

	// NOTE: Empty list or special value token has no-op to do with.
	if(non_list)
	{
		// XXX: Add logic to directly handle special value tokens here?
		// NOTE: The reduction relies on proper handling of reduction status and
		//	proper tail action for the thunked implementations.
		if(term.Value.type() != ystdex::type_id<ValueToken>())
			return DoAdministratives(cs.EvaluateLeaf, term, ctx);
	}
	else if(IsBranch(term))
	{
		YAssert(term.size() != 0, "Invalid node found.");
		// NOTE: List with single element shall be reduced as the element.
		return term.size() != 1 ? DoAdministratives(cs.EvaluateList, term, ctx)
			: ReduceAgainLifted(term, ctx, AccessFirstSubterm(term));
	}
	return ReductionStatus::Retained;
}

ReductionStatus
ContextState::RewriteGuarded(TermNode& term, Reducer reduce)
{
	const auto gd(Guard(term, *this));

	return Rewrite(reduce);
}


ReductionStatus
Reduce(TermNode& term, ContextNode& ctx)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	// TODO: Support other states?
	ystdex::swap_guard<Reducer> gd(true, ctx.Current);

#endif
	SetupNextTerm(ctx, term);
	return ContextState::Access(ctx).RewriteGuarded(term,
		std::ref(ContextState::Access(ctx).ReduceOnce));
}

ReductionStatus
ReduceAgain(TermNode& term, ContextNode& ctx)
{
#if NPL_Impl_NPLA1_Enable_TCO
	EnsureTCOAction(ctx, term).RequestRetrying();
	SetupNextTerm(ctx, term);
	return A1::RelayCurrentOrDirect(ctx,
		std::ref(ContextState::Access(ctx).ReduceOnce), term);
#elif NPL_Impl_NPLA1_Enable_Thunked
	return ReduceSubsequent(term, ctx, []() ynothrow{
		return ReductionStatus::Retrying;
	});
#else
	return ReduceOnce(term, ctx);
#endif
}

void
ReduceArguments(TNIter first, TNIter last, ContextNode& ctx)
{
	if(first != last)
		// NOTE: This is neutral to %NPL_Impl_NPLA1_Enable_Thunked.
		// NOTE: The order of evaluation is unspecified by the language
		//	specification. It should not be depended on.
		ReduceChildren(++first, last, ctx);
	else
		ThrowInvalidSyntaxError("Invalid function application found.");
}

void
ReduceChildren(TNIter first, TNIter last, ContextNode& ctx)
{
	// NOTE: Separators or other sequence constructs are not handled here. The
	//	evaluation can be potentionally parallel, though the simplest one is
	//	left-to-right.
	// XXX: Use execution policies to be evaluated concurrently?
	// NOTE: This does not support PTC, but allow it to be called
	//	in routines which expect proper tail actions, given the guarnatee that
	//	the precondition of %Reduce is not violated.
	// XXX: The remained tail action would be dropped.
#if NPL_Impl_NPLA1_Enable_Thunked
	ReduceChildrenOrderedAsync(first, last, ctx);
#else
	std::for_each(first, last, ystdex::bind1(ReduceOnce, std::ref(ctx)));
#endif
}

ReductionStatus
ReduceChildrenOrdered(TNIter first, TNIter last, ContextNode& ctx)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	return ReduceChildrenOrderedAsync(first, last, ctx);
#else
	const auto tr([&](TNIter iter){
		return ystdex::make_transform(iter, [&](TNIter i){
			return ReduceOnce(*i, ctx);
		});
	});

	return ystdex::default_last_value<ReductionStatus>()(tr(first), tr(last),
		ReductionStatus::Neutral);
#endif
}

ReductionStatus
ReduceFirst(TermNode& term, ContextNode& ctx)
{
	// NOTE: This is neutral to %NPL_Impl_NPLA1_Enable_Thunked.
	return IsBranchedList(term) ? ReduceOnce(AccessFirstSubterm(term), ctx)
		: ReductionStatus::Regular;
}

ReductionStatus
ReduceOnce(TermNode& term, ContextNode& ctx)
{
	return A1::RelayDirect(ctx, ContextState::Access(ctx).ReduceOnce, term);
}

ReductionStatus
ReduceOrdered(TermNode& term, ContextNode& ctx)
{
#if NPL_Impl_NPLA1_Enable_Thunked
#	if NPL_Impl_NPLA1_Enable_TCO
	if(IsBranch(term))
		return ReduceSequenceOrderedAsync(term, ctx, term.begin());
	term.Value = ValueToken::Unspecified;
	return ReductionStatus::Retained;
#	else
	return A1::ReduceCurrentNext(term, ctx,
		Continuation(static_cast<ReductionStatus(&)(TermNode&,
		ContextNode&)>(ReduceChildrenOrdered), ctx), [&]{
		if(IsBranch(term))
			LiftTerm(term, *term.rbegin());
		else
			term.Value = ValueToken::Unspecified;
		return ReductionStatus::Regular;
	});
#	endif
#else
	const auto res(ReduceChildrenOrdered(term, ctx));

	if(IsBranch(term))
		LiftTerm(term, *term.rbegin());
	else
		term.Value = ValueToken::Unspecified;
	return res;
#endif
}

ReductionStatus
ReduceTail(TermNode& term, ContextNode& ctx, TNIter i)
{
	term.erase(term.begin(), i);
	// NOTE: This is neutral to %NPL_Impl_NPLA1_Enable_Thunked.
	return ReduceAgain(term, ctx);
}


void
SetupTraceDepth(ContextState& cs, const string& name)
{
	using namespace std::placeholders;
	auto p_env(cs.GetRecordRef().shared_from_this());

	ystdex::try_emplace(p_env->GetMapRef(), name, NoContainer,
		in_place_type<size_t>);
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	cs.Guard = std::bind([name](TermNode& term, ContextNode& ctx,
		shared_ptr<Environment>& p_e){
		if(const auto p = p_e->LookupName(name))
		{
			using ystdex::pvoid;
			auto& depth(NPL::Access<size_t>(*p));

			YTraceDe(Informative, "Depth = %zu, context = %p, semantics = %p.",
				depth, pvoid(&ctx), pvoid(&term));
			++depth;
			return ystdex::unique_guard([&]() ynothrow{
				--depth;
			});
		}
		else
			ValueNode::ThrowWrongNameFound(name);
	}, _1, _2, std::move(p_env));
}


void
InsertChild(ValueNode&& node, ValueNode::Container& con)
{
	con.insert(node.GetName().empty() ? YSLib::AsNode(node.get_allocator(),
		'$' + MakeIndex(con.size()), std::move(node.Value)) : std::move(node));
}

ValueNode
TransformNode(const TermNode& term, NodeMapper mapper, NodeMapper map_leaf_node,
	TermNodeToString term_to_str, NodeInserter insert_child)
{
	auto s(term.size());

	if(s == 0)
		return map_leaf_node(term);

	auto i(term.begin());
	const auto nested_call(ystdex::bind1(TransformNode, mapper, map_leaf_node,
		term_to_str, insert_child));

	if(s == 1)
		return nested_call(*i);

	const auto& name(term_to_str(*i));

	if(!name.empty())
		yunseq(++i, --s);
	if(s == 1)
	{
		auto&& nd(nested_call(*i));

		if(nd.GetName().empty())
			return YSLib::AsNode(ValueNode::allocator_type(
				term.get_allocator()), name, std::move(nd.Value));
		return
			{ValueNode::Container({std::move(nd)}, term.get_allocator()), name};
	}

	ValueNode::Container node_con;

	std::for_each(i, term.end(), [&](const TermNode& tm){
		insert_child(mapper ? mapper(tm) : nested_call(tm), node_con);
	});
	return
		{std::allocator_arg, term.get_allocator(), std::move(node_con), name};
}


TermNode
TransformForSeparator(const TermNode& term, const ValueObject& pfx,
	const TokenValue& delim)
{
	return TransformForSeparatorTmpl(term, pfx, delim);
}
TermNode
TransformForSeparator(TermNode&& term, const ValueObject& pfx,
	const TokenValue& delim)
{
	return TransformForSeparatorTmpl(std::move(term), pfx, delim);
}

TermNode
TransformForSeparatorRecursive(const TermNode& term, const ValueObject& pfx,
	const TokenValue& delim)
{
	return TransformForSeparatorRecursiveTmpl(term, pfx, delim);
}
TermNode
TransformForSeparatorRecursive(TermNode&& term, const ValueObject& pfx,
	const TokenValue& delim)
{
	return TransformForSeparatorRecursiveTmpl(std::move(term), pfx, delim);
}

ReductionStatus
ReplaceSeparatedChildren(TermNode& term, const ValueObject& pfx,
	const TokenValue& delim)
{
	if(std::find_if(term.begin(), term.end(),
		ystdex::bind1(HasValue<TokenValue>, std::ref(delim))) != term.end())
		term = TransformForSeparator(std::move(term), pfx, delim);
	return ReductionStatus::Clean;
}


ReductionStatus
FormContextHandler::CallN(size_t n, TermNode& term, ContextNode& ctx) const
{
	// NOTE: This implementes arguments evaluation in applicative order when
	//	%Wrapping is not zero.
#if NPL_Impl_NPLA1_Enable_Thunked
	// XXX: Optimize for cases with no argument.
	if(n == 0 || term.size() <= 1)
		// XXX: Assume the term has been setup by the caller.
		return RelayCurrentOrDirect(ctx, Continuation(std::ref(Handler), ctx),
			term);
	return A1::ReduceCurrentNext(term, ctx,
		Continuation([&](TermNode& t, ContextNode& c){
		YAssert(!t.empty(), "Invalid term found.");
		ReduceChildrenOrderedAsyncUnchecked(std::next(t.begin()), t.end(), c);
		return ReductionStatus::Partial;
	}, ctx), ComposeSwitched(ctx, [&, n](ContextNode& c){
		SetupNextTerm(c, term);
		return CallN(n - 1, term, c);
	}));
#else
	// NOTE: This does not support PTC. However, the loop among the wrapping
	//	calls is almost PTC in reality.
	while(n-- != 0)
		ReduceArguments(term, ctx);
	return Handler(term, ctx);
#endif
}

bool
FormContextHandler::Equals(const FormContextHandler& fch) const
{
	if(Wrapping == fch.Wrapping)
	{
		if(Handler == fch.Handler)
			return true;
		if(const auto p = Handler.target<RefContextHandler>())
			return p->HandlerRef.get() == fch.Handler;
		if(const auto p = fch.Handler.target<RefContextHandler>())
			return Handler == p->HandlerRef.get();
	}
	return {};
}


void
RegisterSequenceContextTransformer(EvaluationPasses& passes,
	const TokenValue& delim, bool ordered)
{
	const auto a(passes.get_allocator());

	passes += ystdex::bind1(ReplaceSeparatedChildren, ValueObject(
		std::allocator_arg, a, ordered ? ContextHandler(std::allocator_arg, a,
		Forms::Sequence) : ContextHandler(std::allocator_arg, a,
		FormContextHandler(ReduceBranchToList, 1))), delim);
}


TermNode
ParseLeaf(string_view id, TermNode::allocator_type a)
{
	YAssertNonnull(id.data());
	auto term(NPL::AsTermNode(a));

	if(!id.empty())
		switch(CategorizeBasicLexeme(id))
		{
		case LexemeCategory::Code:
			// XXX: When do code literals need to be evaluated?
			id = DeliteralizeUnchecked(id);
			YB_ATTR_fallthrough;
		case LexemeCategory::Symbol:
			if(CheckReducible(A1::DefaultEvaluateLeaf(term, id)))
				term.Value.emplace<TokenValue>(id, a);
				// NOTE: This is to be evaluated as identifier later.
			break;
			// XXX: Empty token is ignored.
			// XXX: Remained reducible?
		case LexemeCategory::Data:
			// XXX: This should be prevented being passed to second pass in
			//	%TermToNamePtr normally. This is guarded by normal form handling
			//	in the loop in %ContextNode::Rewrite with %ReduceOnce.
			term.Value.emplace<string>(Deliteralize(id), a);
			YB_ATTR_fallthrough;
		default:
			break;
			// XXX: Handle other categories of literal?
		}
	return term;
}

ReductionStatus
DefaultEvaluateLeaf(TermNode& term, string_view id)
{
	YAssertNonnull(id.data());
	if(!id.empty())
	{
		const char f(id.front());

		if(ystdex::isdigit(f))
		{
		    int ans(0);

			for(auto p(id.begin()); p != id.end(); ++p)
				if(ystdex::isdigit(*p))
				{
					if(unsigned((ans << 3) + (ans << 1) + *p - '0')
						<= unsigned(INT_MAX))
						ans = (ans << 3) + (ans << 1) + *p - '0';
					else
						ThrowInvalidSyntaxError(ystdex::sfmt("Value of"
							" identifier '%s' is out of the range of the"
							" supported integer.", id.data()));
				}
				else
					ThrowInvalidSyntaxError(ystdex::sfmt("Literal postfix is"
						" unsupported in identifier '%s'.", id.data()));
			term.Value = ans;
		}
		else if(id == "#t" || id == "#true")
			term.Value = true;
		else if(id == "#f" || id == "#false")
			term.Value = false;
		else if(id == "#n" || id == "#null")
			term.Value = nullptr;
		else if(id == "#inert")
			term.Value = ValueToken::Unspecified;
		else
			return ReductionStatus::Retrying;
		return ReductionStatus::Clean;
	}
	return ReductionStatus::Retrying;
}

ReductionStatus
EvaluateIdentifier(TermNode& term, const ContextNode& ctx, string_view id)
{
	// NOTE: This is the conversion of lvalue in object to value of expression.
	//	The referenced term is lived through the evaluation, which is guaranteed
	//	by the context. This is necessary since the ownership of objects which
	//	are not temporaries in evaluated terms needs to be always in the
	//	environment, rather than in the tree. It would be safe if not passed
	//	directly and without rebinding. Note access of objects denoted by
	//	invalid reference after rebinding would cause undefined behavior in the
	//	object language.
	auto pr(ResolveName(ctx, id));

	if(pr.first)
	{
		auto& bound(*pr.first);
		TermNode* p_rterm;

		// NOTE: This is essentially similar to a successful call to
		//	%ResolveIdentifier plus a call to %EnsureLValueReference, except
		//	that the term tags are always not touched and the term container is
		//	also not touched when the result is not a reference.
		// NOTE: Every reference to the object in the environment is assumed
		//	aliased, so no %TermTags::Unique is preserved.
		if(const auto p = NPL::TryAccessLeaf<const TermReference>(bound))
		{
			p_rterm = &p->get();
			term.SetContent(bound.GetContainer(),
				EnsureLValueReference(TermReference(*p)));
		}
		else
		{
			auto& env(pr.second.get());

			p_rterm = &bound;
			term.Value = TermReference(env.MakeTermTags(bound)
				& ~TermTags::Unique, bound, env.shared_from_this());
		}
		// NOTE: This is not guaranteed to be saved as %ContextHandler in
		//	%ReduceCombined.
		if(const auto p_handler = NPL::TryAccessTerm<LiteralHandler>(*p_rterm))
			return (*p_handler)(ctx);
		// NOTE: Unevaluated term shall be detected and evaluated. See also
		//	$2017-05 @ %Documentation::Workflow.
		return ReductionStatus::Neutral;
	}
	throw BadIdentifier(id);
}

ReductionStatus
EvaluateLeafToken(TermNode& term, ContextNode& ctx, string_view id)
{
	auto& cs(ContextState::Access(ctx));

	YAssertNonnull(id.data());
	// NOTE: Only string node of identifier is tested.
	if(!id.empty() && (cs.EvaluateLiteral.empty()
		|| CheckReducible(cs.EvaluateLiteral(term, cs, id))))
	{
		// NOTE: The symbols are lexical results from analysis by %ParseLeaf.
		//	The term would be normalized by %ReduceCombined. If necessary,
		//	there can be inserted some additional cleanup to remove empty
		//	tokens, returning %ReductionStatus::Partial. Separators should have
		//	been handled in appropriate preprocessing passes.
		// XXX: Asynchronous reduction is currently not supported.
		if(!IsNPLAExtendedLiteral(id))
			return EvaluateIdentifier(term, cs, id);
		ThrowInvalidSyntaxError(ystdex::sfmt(id.front() != '#'
			? "Unsupported literal prefix found in literal '%s'."
			: "Invalid literal '%s' found.", id.data()));
	}
	return ReductionStatus::Retained;
}

ReductionStatus
ReduceCombined(TermNode& term, ContextNode& ctx)
{
	return IsBranchedList(term) ? ReduceCombinedImpl(term, ctx)
		: ReductionStatus::Clean;
}

ReductionStatus
ReduceLeafToken(TermNode& term, ContextNode& ctx)
{
	const auto res(ystdex::call_value_or([&](string_view id){
		return EvaluateLeafToken(term, ctx, id);
	// XXX: A term without token is ignored. This is actually same to
	//	%ReductionStatus::Regular in the current implementation.
	}, TermToNamePtr(term), ReductionStatus::Retained));

	return CheckReducible(res) ? ReduceAgain(term, ctx) : res;
}


ReductionStatus
RelayForEval(ContextNode& ctx, TermNode& term, EnvironmentGuard&& gd,
	bool no_lift, Continuation next)
{
	// XXX: For thunked code, %next shall be a continuation before being
	//	captured and it is not capturable here. No %SetupNextTerm needs to be
	//	called in the caller.
	return RelayForEvalOrDirect(ctx, term, std::move(gd), no_lift,
		std::move(next));
}

ReductionStatus
RelayForCall(ContextNode& ctx, TermNode& term, EnvironmentGuard&& gd,
	bool no_lift)
{
	// NOTE: With TCO, the operand temporary is been indicated by
	//	%TermTags::Temporary, no lifetime extension needs to be cared here.
	return RelayForEvalOrDirect(ctx, term, std::move(gd), no_lift,
		std::ref(ContextState::Access(ctx).ReduceOnce));
}


void
MatchParameter(const TermNode& t, TermNode& o, function<void(TNIter,
	TNIter, const TokenValue&, TermTags, Environment&)> bind_trailing_seq,
	function<void(const TokenValue&, TermNode&, TermTags, Environment&)>
	bind_value, TermTags o_tags, Environment& env)
{
	ParameterMatcher(std::move(bind_trailing_seq), std::move(bind_value))(t, o,
		o_tags, env);
}

void
BindParameter(Environment& env, const TermNode& t, TermNode& o)
{
	const auto check_sigil([&](string_view& id){
		char sigil(id.front());

		if(sigil != '&' && sigil != '%' && sigil != '@')
			sigil = char();
		else
			id.remove_prefix(1);
		return sigil;
	});

	// NOTE: No duplication check here. Symbols can be rebound.
	// TODO: Additional ownership and lifetime check to kept away undefined
	//	behavior?
	MatchParameter(t, o, [&, check_sigil](TNIter first, TNIter last,
		string_view id, TermTags o_tags, Environment& e){
		YAssert(ystdex::begins_with(id, "."), "Invalid symbol found.");
		id.remove_prefix(1);
		if(!id.empty())
		{
			const char sigil(check_sigil(id));

			if(!id.empty())
			{
				TermNode::Container con(t.get_allocator());

				for(; first != last; ++first)
					// TODO: Blocked. Use C++17 sequence container return value.
					BindParameterObject{e}(sigil, o_tags,
						NPL::Deref(first), [&](const TermNode& tm){
						con.emplace_back(tm.GetContainer(), tm.Value);
						CopyTermTags(con.back(), tm);
					}, [&](TermNode::Container&& c, ValueObject&& vo)
						-> TermNode&{
						con.emplace_back(std::move(c), std::move(vo));
						return con.back();
					});
				MarkTemporaryTerm(env.Bind(id, TermNode(std::move(con))),
					sigil);
			}
		}
	}, [&](const TokenValue& n, TermNode& b, TermTags o_tags, Environment& e){
		CheckParameterLeafToken(n, [&]{
			if(!n.empty())
			{
				string_view id(n);
				const char sigil(check_sigil(id));

				if(!id.empty())
					BindParameterObject{e}(sigil, o_tags, b,
						[&](const TermNode& tm){
						CopyTermTags(env.Bind(id, tm), tm);
					}, [&](TermNode::Container&& c, ValueObject&& vo)
						-> TermNode&{
						// XXX: Allocators are not used here for performance.
						return env.Bind(id,
							TermNode(std::move(c), std::move(vo)));
					});
			}
		});
	}, TermTags::Temporary, env);
}


void
SetupDefaultInterpretation(ContextState& cs, EvaluationPasses passes)
{
#if true
	// NOTE: This is an example of merged passes.
	passes += [](TermNode& term, ContextNode& ctx) -> ReductionStatus{
		ReduceHeadEmptyList(term);
		if(IsBranchedList(term))
		{
			auto& fm(AccessFirstSubterm(term));

			// XXX: Without %NPL_Impl_NPLA1_Enable_InlineDirect, the
			//	asynchronous calls are actually more inefficient than separated
			//	calls.
			return ReduceSubsequent(fm, ctx, [&](ContextNode& c){
				SetupNextTerm(c, term);
				return ReduceCombinedImpl(term, c);
			});
		}
		return ReductionStatus::Clean;
	};
#else
#	if true
	// XXX: Optimized based synchronous call of %ReduceHeadEmptyList.
	passes += [](TermNode& term, ContextNode& ctx){
		ReduceHeadEmptyList(term);
		return ReduceFirst(term, ctx);
	};
#	else
	passes += ReduceHeadEmptyList;
	passes += ReduceFirst;
#	endif
	// TODO: Insert more optional optimized lifted form evaluation passes.
	// XXX: This should be the last of list pass for current TCO
	//	implementation, assumed by TCO action.
	passes += ReduceCombined;
#endif
	cs.EvaluateList = std::move(passes);
	cs.EvaluateLeaf = ReduceLeafToken;
}


REPLContext::REPLContext(pmr::memory_resource& rsrc)
	: Allocator(&rsrc), Root(rsrc), ConvertLeaf([&](const SmallString& str){
		return ParseLeaf(YSLib::make_string_view(str), Allocator);
	})
{
	using namespace std::placeholders;

	SetupDefaultInterpretation(Root,
		std::bind(std::ref(ListTermPreprocess), _1, _2));
}

bool
REPLContext::IsAsynchronous() const ynothrow
{
	return NPL_Impl_NPLA1_Enable_Thunked;
}

ReductionStatus
REPLContext::ReduceAndFilter(TermNode& term, ContextNode& ctx)
{
	TryRet(Reduce(term, ctx))
	CatchExpr(NPLException&, throw)
	// TODO: Use semantic exceptions.
	CatchThrow(bad_any_cast& e, LoggedEvent(ystdex::sfmt(
		"Mismatched types ('%s', '%s') found.", e.from(), e.to()), Warning))
	// TODO: Use nested exceptions?
	CatchThrow(std::exception& e, LoggedEvent(e.what(), Err))
}

TermNode
REPLContext::Perform(string_view unit, ContextNode& ctx)
{
	YAssertNonnull(unit.data());
	if(!unit.empty())
		return Process(Session(ctx.GetMemoryResourceRef(), unit), ctx);
	throw LoggedEvent("Empty token list found.", Alert);
}

void
REPLContext::Prepare(TermNode& term) const
{
	Preprocess(term);
}
TermNode
REPLContext::Prepare(const TokenList& token_list) const
{
	auto term(SContext::Analyze(token_list, ConvertLeaf));

	Prepare(term);
	return term;
}
TermNode
REPLContext::Prepare(const Session& session) const
{
	auto term(SContext::Analyze(session, ConvertLeaf, Allocator));

	Prepare(term);
	return term;
}

void
REPLContext::Process(TermNode& term, ContextNode& ctx) const
{
	Prepare(term);
	ReduceAndFilter(term, ctx);
}

TermNode
REPLContext::ReadFrom(std::istream& is) const
{
	if(is)
	{
		if(const auto p = is.rdbuf())
			return ReadFrom(*p);
		else
			throw std::invalid_argument("Invalid stream buffer found.");
	}
	else
		throw std::invalid_argument("Invalid stream found.");
}
TermNode
REPLContext::ReadFrom(std::streambuf& buf) const
{
	using s_it_t = std::istreambuf_iterator<char>;

	return Prepare(Session(s_it_t(&buf), s_it_t()));
}


namespace Forms
{

bool
IsSymbol(const string& id) ynothrow
{
	return IsNPLASymbol(id);
}

TokenValue
StringToSymbol(const string& s)
{
	// XXX: Allocators are not used here for performance in most cases.
	return TokenValue(s);
}
TokenValue
StringToSymbol(string&& s)
{
	// XXX: Ditto.
	return TokenValue(std::move(s));
}

const string&
SymbolToString(const TokenValue& s) ynothrow
{
	return s;
}


size_t
RetainN(const TermNode& term, size_t m)
{
	const auto n(FetchArgumentN(term));

	if(n != m)
		throw ArityMismatch(m, n);
	return n;
}


void
Equal(TermNode& term)
{
	EqualTermReference(term, YSLib::HoldSame);
}

void
EqualLeaf(TermNode& term)
{
	EqualTermValue(term, ystdex::equal_to<>());
}

void
EqualReference(TermNode& term)
{
	EqualTermValue(term, YSLib::HoldSame);
}

void
EqualValue(TermNode& term)
{
	EqualTermReference(term, ystdex::equal_to<>());
}


ReductionStatus
If(TermNode& term, ContextNode& ctx)
{
	Retain(term);

	const auto size(term.size());

	if(size == 3 || size == 4)
	{
		auto i(std::next(term.begin()));

		// XXX: Use captured %ctx seems more efficient here.
		return ReduceSubsequent(*i, ctx, [&, i]{
			auto j(i);

			if(!ExtractBool(*j))
				++j;
			return ++j != term.end() ? ReduceAgainLifted(term, ctx, *j)
				: ReduceReturnUnspecified(term);
		});
	}
	else
		ThrowInvalidSyntaxError("Syntax error in conditional form.");
}

ReductionStatus
Cond(TermNode& term, ContextNode& ctx)
{
	Retain(term);
	RemoveHead(term);
#if NPL_Impl_NPLA1_Enable_Thunked
	return CondImpl(term, ctx, term.begin());
#else
	for(auto i(term.begin()); i != term.end(); ++i)
	{
		auto& clause(Deref(i));
		auto j(CondClauseCheck(clause));

		ReduceOnce(Deref(j), ctx);
		if(CondTest(clause, j))
			return ReduceAgainLifted(term, ctx, clause);
	}
	return ReduceReturnUnspecified(term);
#endif
}

ReductionStatus
When(TermNode& term, ContextNode& ctx)
{
	return WhenUnless(term, ctx, true);
}

ReductionStatus
Unless(TermNode& term, ContextNode& ctx)
{
	return WhenUnless(term, ctx, {});
}

bool
Not(TermNode& term)
{
	return IsLeaf(term) && term.Value == false;
}

ReductionStatus
And(TermNode& term, ContextNode& ctx)
{
	return AndOr(term, ctx, And2);
}

ReductionStatus
Or(TermNode& term, ContextNode& ctx)
{
	return AndOr(term, ctx, Or2);
}


ReductionStatus
Cons(TermNode& term)
{
	ConsImpl(term);
	LiftSubtermsToReturn(term);
	return ReductionStatus::Retained;
}

ReductionStatus
ConsRef(TermNode& term)
{
	ConsImpl(term);
	return ReductionStatus::Retained;
}

ReductionStatus
ForwardListFirst(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 3);

	auto i(term.begin());
	auto& head(*i);
	const auto i_list_comb(++i);
	auto& comb(*++i);
	auto& l(*++i);
	const auto a(term.get_allocator());
	auto& env(ctx.GetRecordRef());
	const auto assign_head([&, a](TermNode::Container&& c, ValueObject&& vo)
		-> TermNode&{
		// XXX: The term %head is reused as the bound operand.
		head = TermNode(std::allocator_arg, a, std::move(c), std::move(vo));
		return head;
	});

	BindMoveLocalReference(l, assign_head);

	TermNode::Container con({EvaluateBoundLValue(head, env)}, a);

	ForwardCombiner(*i_list_comb, ctx);
	con.splice(con.cbegin(), term.GetContainerRef(), i_list_comb);
	l = TermNode(std::allocator_arg, a, std::move(con));
	return ReduceCallSubsequent(l, ctx, env.shared_from_this(),
		[&, a, assign_head]{
		return NPL::ResolveTerm(
			[&](TermNode& nd, ResolvedTermReferencePtr p_ref){
			if(IsBranchedList(nd))
			{
				auto& x(AccessFirstSubterm(nd));

				if(p_ref)
					BindNonmovableLocalReference(p_ref->GetTags()
						& (TermTags::Unique | TermTags::Nonmodifying), x,
						assign_head, p_ref->GetEnvironmentReference());
				else
					BindMoveLocalReference(x, assign_head);
				ForwardCombiner(comb, ctx);
				l = TermNode(std::allocator_arg, a, {std::move(comb),
					EvaluateBoundValue(head, env)});
#if NPL_Impl_NPLA1_Enable_Thunked
				return RelayNext(ctx, [&]{
#	if NPL_Impl_NPLA1_Enable_TCO
					// TODO: See %ReduceCallSubsequent.
					SetupTailAction(ctx, TCOAction(ctx, l, {}));
#	endif
					return RelayForCombine(ctx, l, env.shared_from_this());
				}, ComposeSwitched(ctx, [&]{
					LiftOther(term, l);
					return ReductionStatus::Retained;
				}));
#else
				RelayForCombine(ctx, l, ctx.ShareRecord());
				LiftOther(term, l);
				return ReductionStatus::Retained;
#endif
			}
			else
				ThrowInsufficientTermsError();
		}, l);
	});
}

ReductionStatus
First(TermNode& term)
{
	return FirstOrVal(term, [&](TermNode& tm, ResolvedTermReferencePtr p_ref)
		-> ReductionStatus{
#if true
		// XXX: This is verbose but likely more efficient with %YB_FLATTEN.
		const bool list_not_move(p_ref && p_ref->IsReferencedLValue());

		if(const auto p = NPL::TryAccessLeaf<const TermReference>(tm))
		{
			if(list_not_move)
			{
				term.SetContent(tm);
				return ReductionStatus::Retained;
			}
			if(!p->IsReferencedLValue())
			{
				LiftMovedOther(term, *p, p->IsMovable());
				return ReductionStatus::Retained;
			}
		}
		else if(list_not_move)
			return ReduceToReferenceAt(term, tm, p_ref);
		// XXX: Term tags are currently not respected in prvalues.
		LiftOtherOrCopy(term, tm, !p_ref || p_ref->IsModifiable());
		return ReductionStatus::Retained;
#else
		// NOTE: For exposition only. The optimized implemenation shall be
		//	equivalent to this.
		// XXX: This should be safe, since the parent list is guaranteed an
		//	lvalue by the false result of the call to %NPL::IsMovable.
		if(!(p_ref && p_ref->IsReferencedLValue()))
		{
			if(const auto p = NPL::TryAccessLeaf<const TermReference>(tm))
			{
				if(!p->IsReferencedLValue())
				{
					LiftMovedOther(term, *p, p->IsMovable());
					return ReductionStatus::Retained;
				}
			}
			// XXX: Term tags are currently not respected in prvalues.
			LiftOtherOrCopy(term, tm, !p_ref || p_ref->IsModifiable());
			return ReductionStatus::Retained;
		}
		return ReduceToReference(term, tm, p_ref);
#endif
	});
}

ReductionStatus
FirstAt(TermNode& term)
{
	return FirstAtRef(term, ReduceToReferenceAt);
}

ReductionStatus
FirstRef(TermNode& term)
{
	return FirstAtRef(term, ReduceToReference);
}

ReductionStatus
FirstVal(TermNode& term)
{
	return FirstOrVal(term, [&](TermNode& tm, bool has_ref){
		// XXX: Simple 'ReduceToValue(term, tm)' is wrong because it may
		//	move non-unqiue reference object away.
		if(const auto p = NPL::TryAccessLeaf<const TermReference>(tm))
			LiftMovedOther(term, *p, !has_ref && p->IsMovable());
		else
			// XXX: Term tags are currently not respected in prvalues.
			LiftOtherOrCopy(term, tm, !has_ref);
		return ReductionStatus::Retained;
	});
}

void
SetFirst(TermNode& term)
{
	DoSetFirst(term, [](TermNode& dst, TermNode&, const TermReference& ref){
		LiftMovedOther(dst, ref, ref.IsMovable());
	});
}

void
SetFirstAt(TermNode& term)
{
	DoSetFirst(term, [](TermNode& dst, TermNode& y, const TermReference&){
		LiftOther(dst, y);
	});
}

void
SetFirstRef(TermNode& term)
{
	DoSetFirst(term, [](TermNode& dst, TermNode& y, TermReference& ref){
		// XXX: No cyclic reference check except for self assignment.
		LiftCollapsed(dst, y, std::move(ref));
	});
}

void
SetRest(TermNode& term)
{
	SetRestImpl(term, LiftSubtermsToReturn);
}

void
SetRestRef(TermNode& term)
{
	SetRestImpl(term, LiftNoOp);
}


ReductionStatus
Eval(TermNode& term, ContextNode& ctx)
{
	return EvalImpl(term, ctx, {});
}

ReductionStatus
EvalRef(TermNode& term, ContextNode& ctx)
{
	return EvalImpl(term, ctx, true);
}

ReductionStatus
EvalString(TermNode& term, ContextNode& ctx)
{
	return EvalStringImpl(term, ctx, {});
}

ReductionStatus
EvalStringRef(TermNode& term, ContextNode& ctx)
{
	return EvalStringImpl(term, ctx, true);
}

ReductionStatus
EvalUnit(TermNode& term)
{
	CallBinaryAs<const string, REPLContext>(
		[&](const string& unit, REPLContext& rctx){
		term.SetContent(rctx.Perform(unit));
	}, term);
	return ReductionStatus::Retained;
}


void
MakeEnvironment(TermNode& term)
{
	Retain(term);

	const auto a(term.get_allocator());

	if(term.size() > 1)
	{
		ValueObject parent;
		const auto tr([&](TNCIter iter){
			return ystdex::make_transform(iter, [&](TNCIter i){
				return ReferenceTerm(*i).Value;
			});
		});

		parent.emplace<EnvironmentList>(tr(std::next(term.begin())),
			tr(term.end()), a);
		term.Value = NPL::AllocateEnvironment(a, std::move(parent));
	}
	else
		term.Value = NPL::AllocateEnvironment(a);
}

void
GetCurrentEnvironment(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 0);
	term.Value = ValueObject(ctx.WeakenRecord());
}

void
LockCurrentEnvironment(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 0);
	term.Value = ValueObject(ctx.ShareRecord());
}

ReductionStatus
ValueOf(TermNode& term, const ContextNode& ctx)
{
	RetainN(term);
	if(const auto p_id = NPL::TryAccessTerm<const string>(
		ReferenceTerm(*std::next(term.begin()))))
	{
		// XXX: This is needed since %EvaluateIdentifier assumes regularized
		//	term and it would not return %ReductionStatus::Clean to ensure
		//	additional regularzation.
		term.ClearContainer();
		TryRet(EvaluateIdentifier(term, ctx, *p_id))
		CatchIgnore(BadIdentifier&)
	}
	term.Value = ValueToken::Null;
	return ReductionStatus::Clean;
}


ReductionStatus
DefineLazy(TermNode& term, ContextNode& ctx)
{
	DoDefine(term, std::bind(BindParameter, std::ref(ctx.GetRecordRef()),
		std::placeholders::_1, std::ref(term)));
	return ReduceReturnUnspecified(term);
}

ReductionStatus
DefineWithNoRecursion(TermNode& term, ContextNode& ctx)
{
	return DoDefine(term, DefineOrSetDispatcher<>{term, ctx});
}

ReductionStatus
DefineWithRecursion(TermNode& term, ContextNode& ctx)
{
	return DoDefine(term, DefineOrSetDispatcher<true>{term, ctx});
}

ReductionStatus
SetWithNoRecursion(TermNode& term, ContextNode& ctx)
{
	return DoSet(term, ctx, DefineOrSetDispatcher<>{term, ctx});
}

ReductionStatus
SetWithRecursion(TermNode& term, ContextNode& ctx)
{
	return DoSet(term, ctx, DefineOrSetDispatcher<true>{term, ctx});
}

void
Undefine(TermNode& term, ContextNode& ctx)
{
	UndefineImpl(term, [&](const TokenValue& n){
		term.Value = ctx.GetRecordRef().Remove(n);
	});
}

void
UndefineChecked(TermNode& term, ContextNode& ctx)
{
	UndefineImpl(term, [&](const TokenValue& n){
		ctx.GetRecordRef().RemoveChecked(n);
	});
}


ReductionStatus
Lambda(TermNode& term, ContextNode& ctx)
{
	return LambdaImpl(term, ctx, {});
}

ReductionStatus
LambdaRef(TermNode& term, ContextNode& ctx)
{
	return LambdaImpl(term, ctx, true);
}

ReductionStatus
Vau(TermNode& term, ContextNode& ctx)
{
	return VauImpl(term, ctx, {});
}

ReductionStatus
VauRef(TermNode& term, ContextNode& ctx)
{
	return VauImpl(term, ctx, true);
}

ReductionStatus
VauWithEnvironment(TermNode& term, ContextNode& ctx)
{
	return VauWithEnvironmentImpl(term, ctx, {});
}

ReductionStatus
VauWithEnvironmentRef(TermNode& term, ContextNode& ctx)
{
	return VauWithEnvironmentImpl(term, ctx, true);
}


ReductionStatus
Wrap(TermNode& term, ContextNode& ctx)
{
	return WrapOrRef(term, ctx, WrapN,
		[&](ContextHandler& h, ResolvedTermReferencePtr p_ref){
		return WrapH(term, MakeValueOrMove(p_ref, [&]{
			return FormContextHandler(h, 1);
		}, [&]{
			return FormContextHandler(std::move(h), 1);
		}));
	});
}

ReductionStatus
WrapRef(TermNode& term, ContextNode& ctx)
{
	return WrapOrRef(term, ctx, WrapRefN,
		[&](ContextHandler& h, ResolvedTermReferencePtr p_ref){
		return p_ref
			? ReduceForCombinerRef(term, ctx, *p_ref, h, 1)
			: WrapH(term, FormContextHandler(std::move(std::move(h)), 1));
	});
}

ReductionStatus
WrapOnce(TermNode& term, ContextNode& ctx)
{
	return WrapOnceOrOnceRef(term, ctx, WrapN);
}

ReductionStatus
WrapOnceRef(TermNode& term, ContextNode& ctx)
{
	return WrapOnceOrOnceRef(term, ctx, WrapRefN);
}

ReductionStatus
Unwrap(TermNode& term, ContextNode& ctx)
{
	using namespace std::placeholders;

	return WrapUnwrap(term, std::bind(UnwrapResolved, std::ref(term),
		std::ref(ctx), _1, _2), ThrowForUnwrappingFailure);
}


void
ThrowInsufficientTermsError()
{
	throw ParameterMismatch("Insufficient term found for list parameter.");
}

void
ThrowInvalidSyntaxError(const char* str)
{
	throw InvalidSyntax(str);
}
void
ThrowInvalidSyntaxError(string_view sv)
{
	throw InvalidSyntax(sv);
}

void
ThrowNonmodifiableErrorForAssignee()
{
	throw TypeError("Destination operand of assignment shall be modifiable.");
}

void
ThrowValueCategoryErrorForFirstArgument(const TermNode& term)
{
	throw ValueCategoryMismatch(ystdex::sfmt("Expected a reference for the 1st "
		"argument, got '%s'.", TermToString(term).c_str()));
}

ReductionStatus
CheckEnvironment(TermNode& term)
{
	RetainN(term);

	auto& tm(*std::next(term.begin()));

	Environment::EnsureValid(ResolveEnvironment(tm).first);
	ReduceToValue(term, tm);
	return ReductionStatus::Regular;
}

ReductionStatus
CheckListReference(TermNode& term)
{
	return CallResolvedUnary([&](TermNode& nd, bool has_ref){
		CheckResolvedListReference(nd, has_ref);
		// XXX: Similar to %DoIdFunc in %NPL.Dependency.
		LiftTerm(term, NPL::Deref(std::next(term.begin())));
		return ReductionStatus::Regular;
	}, term);
}


ReductionStatus
MakeEncapsulationType(TermNode& term)
{
	const auto tag(in_place_type<ContextHandler>);
	const auto a(term.get_allocator());
	// NOTE: The %p_type handle can be extended to point to a metadata block.
#if true
	shared_ptr<void> p_type(new yimpl(byte));

	term.GetContainerRef() = {{std::allocator_arg, a, NoContainer,
		tag, std::allocator_arg, a, FormContextHandler(Encapsulate(p_type), 1)},
		{std::allocator_arg, a, NoContainer, tag, std::allocator_arg, a,
		FormContextHandler(Encapsulated(p_type), 1)},
		{std::allocator_arg, a, NoContainer, tag, std::allocator_arg, a,
		FormContextHandler(Decapsulate(p_type), 1)}};
#else
	// XXX: Mixing the %p_type above to following assignment code can be worse
	//	in performance.
	shared_ptr<void> p_type(YSLib::allocate_shared<yimpl(byte)>(a));

	term.GetContainerRef() = {{std::allocator_arg, a, NoContainer,
		std::allocator_arg, a, tag, std::allocator_arg, a,
		FormContextHandler(Encapsulate(p_type), 1)},
		{std::allocator_arg, a, NoContainer, std::allocator_arg, a, tag,
		std::allocator_arg, a, FormContextHandler(Encapsulated(p_type), 1)},
		{std::allocator_arg, a, NoContainer, std::allocator_arg, a, tag,
		std::allocator_arg, a, FormContextHandler(Decapsulate(p_type), 1)}};
#endif
	return ReductionStatus::Retained;
}


ReductionStatus
Sequence(TermNode& term, ContextNode& ctx)
{
	Retain(term);
	RemoveHead(term);
	return ReduceOrdered(term, ctx);
}

ReductionStatus
Apply(TermNode& term, ContextNode& ctx)
{
	Retain(term);

	const auto size(term.size());

	if(size == 3)
		return ApplyImpl(term, ctx,
			NPL::AllocateEnvironment(term.get_allocator()));
	if(size == 4)
		return ApplyImpl(term, ctx,
			ResolveEnvironment(*std::next(term.begin(), 3)).first);
	ThrowInvalidSyntaxError("Syntax error in applying form.");
}

ReductionStatus
ListAsterisk(TermNode& term)
{
	ListAsteriskImpl(term);
	LiftSubtermsToReturn(term);
	return ReductionStatus::Retained;
}

ReductionStatus
ListAsteriskRef(TermNode& term)
{
	ListAsteriskImpl(term);
	return ReductionStatus::Retained;
}


void
CallSystem(TermNode& term)
{
	CallUnaryAs<const string>(
		ystdex::compose(usystem, std::mem_fn(&string::c_str)), term);
}

} // namespace Forms;

} // namesapce A1;

} // namespace NPL;

