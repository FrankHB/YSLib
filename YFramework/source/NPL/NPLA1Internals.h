/*
	© 2017-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Internals.h
\ingroup NPL
\brief NPLA1 内部接口。
\version r19838
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2020-02-15 13:20:08 +0800
\par 修改时间:
	2020-06-06 05:23 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	NPL::NPLA1Internals
*/


#ifndef NPL_INC_NPLA1Internals_h_
#define NPL_INC_NPLA1Internals_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA1 // for ContextState, ReductionStatus, ContextNode,
//	Reducer, YSLib::tuple, YSLib::get, RegularizeTerm, EnvironmentReference,
//	TermReference;
#include <ystdex/ref.hpp> // for ystdex::unref;

namespace NPL
{

namespace A1
{

/*!
\brief 内部接口。
\since build 882

用于实现 NPLA1Forms 和其它 NPL 模块的非公开 API 。
*/
inline namespace Internals
{

// NOTE: The inlined synchronous calls are more effecient than asynchronous
//	ones. It is intended to prevent unneeded uncapturable continuation being
//	construced by reduction action composition calls (e.g. of %RelaySwitched).
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
#define NPL_Impl_NPLA1_Enable_ThunkedSeparatorPass NPL_Impl_NPLA1_Enable_Thunked

//! \since build 820
static_assert(!NPL_Impl_NPLA1_Enable_TCO || NPL_Impl_NPLA1_Enable_Thunked,
	"Invalid combination of build options found.");

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
	ctx.SetupFront(std::move(act));
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
using FrameRecord = YSLib::tuple<ContextHandler, shared_ptr<Environment>>;
//! \since build 882
using YSLib::get;

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
		: Term(term), req_lift_result(lift), xgds(ctx.get_allocator()),
		EnvGuard(ctx), RecordList(ctx.get_allocator())
	{}
	// XXX: Not used, but provided for well-formness.
	//! \since build 819
	TCOAction(const TCOAction& a)
		// XXX: Some members are moved. This is only safe when newly constructed
		//	object always live longer than the older one.
		: Term(a.Term), req_combined(a.req_combined),
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

//! \since build 886
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TCOAction*, AccessTCOAction, ContextNode& ctx) ynothrow
	ImplRet(ctx.AccessCurrentAs<TCOAction>())
// NOTE: There is no need to check term like 'if(&p->Term.get() == &term)'. It
//	should be same to saved enclosing term unless a nested TCO action is needed
//	explicitly (by following %SetupTailAction rather than %EnsureTCOAction).

//! \since build 840
YB_ATTR_nodiscard YB_FLATTEN TCOAction&
EnsureTCOAction(ContextNode& ctx, TermNode& term);

//! \since build 887
YB_ATTR_nodiscard YB_FLATTEN TCOAction&
EnsureTCOActionUnchecked(ContextNode& ctx, TermNode& term);

//! \since build 840
YB_ATTR_nodiscard inline PDefH(TCOAction&, RefTCOAction, ContextNode& ctx)
	// NOTE: The TCO action should have been created by a previous call of
	//	%EnsureTCOAction, typically in the call of %CombinerReturnThunk in
	//	calling a combiner from %ReduceCombinedBranch.
	ImplRet(NPL::Deref(AccessTCOAction(ctx)))

//! \since build 886
inline
	PDefH(void, SetupTailTCOAction, ContextNode& ctx, TermNode& term, bool lift)
	ImplExpr(SetupTailAction(ctx, TCOAction(ctx, term, lift)))
#	endif
#endif


//! \since build 879
//@{
#if NPL_Impl_NPLA1_Enable_Thunked && !NPL_Impl_NPLA1_Enable_InlineDirect
YB_ATTR(always_inline) inline ReductionStatus
RelayCurrent(ContextNode& ctx, Continuation&& cur)
{
	return RelaySwitched(ctx, std::move(cur));
}
YB_ATTR(always_inline) inline ReductionStatus
RelayCurrent(ContextNode& ctx, std::reference_wrapper<Continuation> cur)
{
	return RelaySwitched(ctx, cur);
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
	RelaySwitched(ctx, yforward(next));
	return RelaySwitched(ctx, yforward(cur));
}
template<typename _fNext>
YB_ATTR(always_inline) inline ReductionStatus
RelayNextOrDirect(ContextNode& ctx,
	std::reference_wrapper<Continuation> cur, _fNext&& next)
{
	RelaySwitched(ctx, yforward(next));
	return RelaySwitched(ctx, cur);
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
// XXX: This is a workaround for G++'s LTO bug.
#if !YB_IMPL_GNUCPP || !NPL_Impl_NPLA1_Enable_Thunked \
	|| NPL_Impl_NPLA1_Enable_TCO
YB_FLATTEN 
#endif
inline ReductionStatus
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

//! \since build 884
template<typename _fCurrent>
inline ReductionStatus
ReduceSubsequentCombinedBranch(TermNode& term, ContextNode& ctx,
	_fCurrent&& next)
{
	return A1::ReduceCurrentNext(term, ctx,
		Continuation(std::ref(ReduceCombinedBranch), ctx), yforward(next));
}


//! \since build 869
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TermReference, EnsureLValueReference, TermReference&& ref)
	// XXX: Use %TermReference::SetTags is not efficient here.
	ImplRet(TermReference(ref.GetTags() & ~TermTags::Unique, std::move(ref)))


//! \since build 859
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


YB_ATTR_nodiscard YB_PURE EnvironmentReference
FetchTailEnvironmentReference(const TermReference&, ContextNode&);

//! \since build 878
ReductionStatus
ReduceForCombinerRef(TermNode&, ContextNode&,
	const TermReference&, const ContextHandler&, size_t);

} // inline namespace Internals;

} // namesapce A1;

} // namespace NPL;

#endif

