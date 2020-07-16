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
\version r20134
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2020-02-15 13:20:08 +0800
\par 修改时间:
	2020-07-14 03:08 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	NPL::NPLA1Internals
*/


#ifndef NPL_INC_NPLA1Internals_h_
#define NPL_INC_NPLA1Internals_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA1 // for ContextState, ReductionStatus, ContextNode,
//	Reducer, YSLib::tuple, YSLib::get, list, RegularizeTerm,
//	EnvironmentReference, TermReference, ystdex::get_less, YSLib::map, set,
//	std::make_move_iterator, ystdex::exists, ystdex::retry_on_cond, ystdex::id;
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
	//! \since build 893
	lref<TermNode> TermRef;

private:
	//! \since build 854
	//@{
	bool req_combined = {};
	bool req_lift_result = {};
	//@}
	//! \since build 827
	mutable list<ContextHandler> xgds;

public:
	// See $2018-06 @ %Documentation::Workflow for details.
	//! \since build 825
	mutable observer_ptr<const ContextHandler> LastFunction{};
	//! \since build 820
	mutable EnvironmentGuard EnvGuard;
	//! \since build 825
	mutable FrameRecordList RecordList;
	//! \since build 895
	mutable TokenValue OperatorName;

	//! \since build 819
	TCOAction(ContextNode& ctx, TermNode& term, bool lift)
		: TermRef(term), req_lift_result(lift), xgds(ctx.get_allocator()),
		EnvGuard(ctx), RecordList(ctx.get_allocator()),
		OperatorName(ctx.get_allocator())
	{
		if(const auto p = term.Value.AccessPtr<TokenValue>())
			OperatorName = std::move(*p);
			// XXX: After the move, %term.Value is unspecified.
	}
	// XXX: Not used, but provided for well-formness.
	//! \since build 819
	TCOAction(const TCOAction& a)
		// XXX: Some members are moved. This is only safe when newly constructed
		//	object always live longer than the older one.
		: TermRef(a.TermRef), req_combined(a.req_combined),
		req_lift_result(a.req_lift_result), xgds(std::move(a.xgds)),
		EnvGuard(std::move(a.EnvGuard))
	{}
	DefDeMoveCtor(TCOAction)

	DefDeMoveAssignment(TCOAction)

	//! \since build 877
	ReductionStatus
	operator()(ContextNode&) const;

	//! \since build 857
	void
	AddRecord(shared_ptr<Environment>&& p_env)
	{
		// NOTE: The temporary function and the environment are saved in the
		//	frame record list as a new entry.
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

	//! \since build 893
	ReductionStatus
	HandleResultRequests(TermNode& term, ContextNode& ctx) const
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
		if(req_combined)
			RegularizeTerm(term, ctx.LastStatus);
		return ctx.LastStatus;
	}

	//! \since build 825
	YB_ATTR_nodiscard ContextHandler
	MoveFunction()
	{
		ContextHandler res(std::allocator_arg, xgds.get_allocator());

		if(LastFunction)
		{
			const auto i(std::find_if(xgds.rbegin(), xgds.rend(),
				[this](const ContextHandler& h) ynothrow{
				return NPL::make_observer(&h) == LastFunction;
			}));

			if(i != xgds.rend())
			{
				res = ContextHandler(std::allocator_arg, xgds.get_allocator(),
					std::move(*i));
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
	//@}
};

//! \since build 886
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TCOAction*, AccessTCOAction, ContextNode& ctx) ynothrow
	ImplRet(ctx.AccessCurrentAs<TCOAction>())
// NOTE: There is no need to check term like 'if(&p->TermRef.get() == &term)'. It
//	should be same to saved enclosing term unless a nested TCO action is needed
//	explicitly (by following %SetupTailAction rather than %EnsureTCOAction).

//! \since build 840
YB_ATTR_nodiscard YB_FLATTEN TCOAction&
EnsureTCOAction(ContextNode& ctx, TermNode& term);

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


//! \since build 827
struct RecordCompressor final
{
	using RecordInfo
		= YSLib::map<lref<Environment>, size_t, ystdex::get_less<>>;
	using ReferenceSet = set<lref<Environment>, ystdex::get_less<>>;

	//! \since build 894
	weak_ptr<Environment> RootPtr;
	ReferenceSet Reachable, NewlyReachable;
	RecordInfo Universe;

	//! \since build 894
	RecordCompressor(const shared_ptr<Environment>& p_root)
		: RecordCompressor(p_root, NPL::Deref(p_root).Bindings.get_allocator())
	{}
	//! \since build 894
	RecordCompressor(const shared_ptr<Environment>& p_root,
		Environment::allocator_type a)
		: RootPtr(p_root), Reachable({NPL::Deref(p_root)}, a),
		NewlyReachable(a), Universe(a)
	{
		AddParents(NPL::Deref(p_root));
	}

	// XXX: All checks rely on recursive calls which do not respect nested
	//	safety currently.
	//! \since build 860
	void
	AddParents(Environment& e)
	{
		Traverse(e, e.Parent,
			[this](const shared_ptr<Environment>& p_dst, const Environment&){
			return Universe.emplace(NPL::Deref(p_dst),
				CountReferences(p_dst)).second;
		});
	}

	void
	Compress();

	//! \since build 894
	YB_ATTR_nodiscard YB_PURE static size_t
	CountReferences(const shared_ptr<Environment>& p) ynothrowv
	{
		const auto acnt(NPL::Deref(p).GetAnchorCount());

		YAssert(acnt > 0, "Zero anchor count found for environment.");
		return CountStrong(p) + size_t(acnt) - 2;
	}

	//! \since build 894
	YB_ATTR_nodiscard YB_PURE static size_t
	CountStrong(const shared_ptr<Environment>& p) ynothrowv
	{
		const long scnt(p.use_count());

		YAssert(scnt > 0, "Zero shared count found for environment.");
		return size_t(scnt);
	}

	//! \since build 882
	template<typename _fTracer>
	static void
	Traverse(Environment& e, ValueObject& parent, const _fTracer& trace)
	{
		const auto& tp(parent.type());

		if(tp == ystdex::type_id<EnvironmentList>())
		{
			for(auto& vo : parent.GetObject<EnvironmentList>())
				Traverse(e, vo, trace);
		}
		else if(tp == ystdex::type_id<EnvironmentReference>())
		{
			if(auto p = parent.GetObject<EnvironmentReference>().Lock())
				TraverseForSharedPtr(e, parent, trace, p);
		}
		else if(tp == ystdex::type_id<shared_ptr<Environment>>())
		{
			if(auto p = parent.GetObject<shared_ptr<Environment>>())
				TraverseForSharedPtr(e, parent, trace, p);
		}
	}

private:
	//! \since build 894
	template<typename _fTracer>
	static void
	TraverseForSharedPtr(Environment& e, ValueObject& parent,
		const _fTracer& trace, shared_ptr<Environment>& p)
	{
		if(ystdex::expand_proxy<void(const shared_ptr<Environment>&,
			Environment&, ValueObject&)>::call(trace, p, e, parent))
		{
			auto& dst(*p);

			// NOTE: The shared pointer should not be locked to ensure it
			//	neutral to nested calls.
			p.reset();
			Traverse(dst, dst.Parent, trace);
		}
	}
};

// NOTE: See $2018-06 @ %Documentation::Workflow and $2019-06 @
//	%Documentation::Workflow for details.
//! \since build 861
inline void
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
	RecordCompressor(ctx.GetRecordPtr()).Compress();
}

//! \since build 878
//@{
/*!
\brief 准备 TCO 求值。
\param ctx 规约上下文。
\param term 被规约的项。
\param act TCO 动作。
*/
inline TCOAction&
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
	act.HandleResultRequests(term, ctx);
	return act;
}

/*!
\brief 按参数设置 TCO 动作提升请求。
\param act TCO 动作。
\param no_lift 指定是否避免保证规约后提升结果。
*/
inline void
SetupTCOLift(TCOAction& act, bool no_lift)
{
	// NOTE: The %act.LiftCallResult indicates a request for handling during
	//	next time (by %TCOAction::HandleResultRequests call above before the
	//	last one) before %TCOAction is finished. The last request would be
	//	handled by %TCOAction::operator(), which also calls
	//	%TCOAction::HandleResultRequests.
	if(!no_lift)
		act.RequestLiftResult();
}
//@}
#	else
//! \since build 879
ReductionStatus
MoveGuard(EnvironmentGuard& gd, ContextNode& ctx) ynothrow
{
	const auto egd(std::move(gd));

	return ctx.LastStatus;
}
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
// XXX: This fails to exclude ill-formed overloads not qualified to
//	'cur(term, ctx)' with 'std::bind' result from libstdc++, either with G++ or
//	Clang++. Make sure %_fCurrent accept two proper parameters.
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

//! \since build 878
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

