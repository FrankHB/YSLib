/*
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
\version r19257
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-02 18:02:47 +0800
\par 修改时间:
	2020-06-12 21:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1Forms // for YSLib, NPL, Forms, RelaySwitched,
//	type_index, std::hash, ystdex::equal_to, YSLib::unordered_map,
//	YSLib::AllocatedHolderOperations, NPL::TryAccessLeaf, TermReference,
//	IsBranch, NPL::Access, shared_ptr, NPL::Deref, ystdex::ref_eq, IsLeaf,
//	ContextHandler, NPL::make_observer, TermTags, TokenValue,
//	std::allocator_arg, lref, stack, vector, function, std::find_if,
//	Environment, map, set, ystdex::get_less, list, ystdex::type_id,
//	InvalidReference, std::make_move_iterator, ystdex::exists, ystdex::id,
//	ystdex::ref, ystdex::retry_on_cond, ystdex::id, ystdex::cast_mutable,
//	ystdex::expand_proxy, EnvironmentSwitcher, std::placeholders,
//	GetLValueTagsOf, NPL::IsMovable, ResolveTerm, ystdex::update_thunk,
//	AccessFirstSubterm, ystdex::bind1, IsBranchedList, NoContainer,
//	in_place_type, ystdex::make_transform, ystdex::try_emplace,
//	ystdex::unique_guard, NPL::AsTermNode, CategorizeBasicLexeme,
//	DeliteralizeUnchecked, CheckReducible, Deliteralize, ystdex::isdigit,
//	ResolveIdentifier, NPL::TryAccessTerm, LiteralHandler,
//	IsNPLAExtendedLiteral, YSLib::share_move, NPL::TryAccessReferencedTerm,
//	ystdex::call_value_or, bad_any_cast;
#include "NPLA1Internals.h" // for A1::Internals API;
#include YFM_NPL_SContext // for Session;

using namespace YSLib;

namespace NPL
{

//! \since build 863
//@{
#ifndef NDEBUG
#	define NPL_Impl_NPLA1_AssertParameterMatch true
#else
#	define NPL_Impl_NPLA1_AssertParameterMatch false
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

//! \since build 868
using Forms::ThrowInsufficientTermsError;
//! \since build 859
using Forms::ThrowInvalidSyntaxError;


#if NPL_Impl_NPLA1_Enable_Thunked
//! \since build 841
void
PushActionsRange(EvaluationPasses::const_iterator first,
	EvaluationPasses::const_iterator last, TermNode& term, ContextNode& ctx)
{
	if(first != last)
	{
		// NOTE: Retrying is recognized from the result since 1st administrative
		//	pass is reduced. The non-skipped administrative reductions are
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
#endif

//! \since build 892
//@{
using NameTable = YSLib::unordered_map<type_index, string_view,
	std::hash<type_index>, ystdex::equal_to<type_index>,
	std::allocator<std::pair<const type_index, string_view>>>;

template<class _tKey>
YB_ATTR_nodiscard inline NameTable&
FetchNameTableRef()
{
	return ystdex::parameterize_static_object<NameTable, _tKey>();
}

template<typename _type>
YB_ATTR_nodiscard YB_NONNULL(2) YB_PURE inline _type
NameHandler(_type&& x, const char* desc)
{
	static struct Init final
	{
		Init(string_view sv)
		{
			FetchNameTableRef<NPLA1Tag>().emplace(std::piecewise_construct,
				std::forward_as_tuple(ystdex::type_id<_type>()),
				std::forward_as_tuple(sv));
		}
	} init(desc);

	return yforward(x);
}
//@}

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
		Continuation(NameHandler([first, last](TermNode&, ContextNode& c){
		return ReduceChildrenOrderedAsync(first, last, c);
	}, "eval-argument-list"), ctx));
}

YB_ATTR(always_inline) inline ReductionStatus
ReduceChildrenOrderedAsync(TNIter first, TNIter last, ContextNode& ctx)
{
	return first != last ? ReduceChildrenOrderedAsyncUnchecked(first, last, ctx)
		: ReductionStatus::Neutral;
}
#endif


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
	RelaySwitched(ctx, std::bind([&](const _tParams&...){
		// NOTE: Captured argument pack is only needed when %h actually shares.
		return RegularizeTerm(term, ctx.LastStatus);
	}, std::move(args)...));
	return RelaySwitched(ctx, Continuation(std::ref(h), ctx));
#else

	yunseq(0, args...);
	return RegularizeTerm(term, h(term, ctx));
#endif
}


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


//! \since build 891
//@{
using SourcedByteAllocator = pmr::polymorphic_allocator<yimpl(byte)>;

using SourceInfoMetadata = lref<const SourceInformation>;


template<typename _type, class _tByteAlloc = SourcedByteAllocator>
class SourcedHolder : public AllocatorHolder<_type, _tByteAlloc>
{
public:
	using Creation = IValueHolder::Creation;
	using value_type = _type;

private:
	using base = AllocatorHolder<_type, _tByteAlloc>;

	SourceInformation source_information;

public:
	using base::value;

	template<typename... _tParams>
	inline
	SourcedHolder(const SourceName& name, const SourceLocation& src_loc,
		_tParams&&... args)
		: base(yforward(args)...), source_information(name, src_loc)
	{}
	template<typename... _tParams>
	inline
	SourcedHolder(const SourceInformation& src_info, _tParams&&... args)
		: base(yforward(args)...), source_information(src_info)
	{}
	DefDeCopyMoveCtorAssignment(SourcedHolder)

	YB_ATTR_nodiscard any
	Create(Creation c, const any& x) const ImplI(IValueHolder)
	{
		// XXX: Always use the allocator to hold the source information.
		return YSLib::AllocatedHolderOperations<SourcedHolder,
			_tByteAlloc>::CreateHolder(c, x, value, source_information, value);
	}

	YB_ATTR_nodiscard YB_PURE PDefH(any, Query, uintmax_t) const ynothrow
		ImplI(IValueHolder)
		ImplRet(ystdex::ref(source_information))	

	using base::get_allocator;
};
//@}


//! \since build 881
//@{
using Action = function<void()>;

class SeparatorPass
{
private:
	using Filter = decltype(ystdex::bind1(HasValue<TokenValue>,
		std::declval<TokenValue&>()));

	TermPasses::allocator_type alloc;
	TokenValue delim{";"};
	TokenValue delim2{","};
	Filter filter{ystdex::bind1(HasValue<TokenValue>, delim)};
	Filter filter2{ystdex::bind1(HasValue<TokenValue>, delim2)};
	// XXX: More allocators are not used here for performance in most cases.
	ValueObject pfx{std::allocator_arg, alloc, ContextHandler(Forms::Sequence)};
	ValueObject pfx2{std::allocator_arg, alloc,
		ContextHandler(FormContextHandler(ReduceBranchToList, 1))};
#if NPL_Impl_NPLA1_Enable_ThunkedSeparatorPass
	//! \since build 882
	using TermStack = stack<lref<TermNode>, vector<lref<TermNode>>>;

	//! \since build 882
	mutable TermStack remained{alloc};
#endif

public:
	//! \since build 888
	SeparatorPass(TermPasses::allocator_type a)
		: alloc(a)
	{}

	YB_FLATTEN ReductionStatus
	operator()(TermNode& term) const
	{
#if NPL_Impl_NPLA1_Enable_ThunkedSeparatorPass
		YAssert(remained.empty(), "Invalid state found.");
		Transform(term, remained);
		while(!remained.empty())
		{
			const auto term_ref(std::move(remained.top()));

			remained.pop();
			for(auto& tm : term_ref.get())
				Transform(tm, remained);
		}
#else
		Transform(term);
#endif
		return ReductionStatus::Clean;
	}

private:
#if NPL_Impl_NPLA1_Enable_ThunkedSeparatorPass
	//! \since build 882
	void
	Transform(TermNode& term, TermStack& terms) const
	{
		terms.push(term);
		if(std::find_if(term.begin(), term.end(), filter) != term.end())
			term = SeparatorTransformer::Process(std::move(term), pfx, filter);
		if(std::find_if(term.begin(), term.end(), filter2) != term.end())
			term = SeparatorTransformer::Process(std::move(term), pfx2,
				filter2);
	}
#else
	void
	Transform(TermNode& term) const
	{
		if(std::find_if(term.begin(), term.end(), filter) != term.end())
			term = SeparatorTransformer::Process(std::move(term), pfx, filter);
		if(std::find_if(term.begin(), term.end(), filter2) != term.end())
			term = SeparatorTransformer::Process(std::move(term), pfx2,
				filter2);
		for(auto& tm : term)
			Transform(tm);
	}
#endif
};
//@}


#if NPL_Impl_NPLA1_Enable_TCO
//! \since build 827
struct RecordCompressor final
{
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

	// XXX: All checks rely on recursive calls which do not respect nested
	//	safety currently.
	//! \since build 860
	void
	AddParents(Environment& e)
	{
		Traverse(e, e.Parent, [this](Environment& dst, const Environment&){
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

			Traverse(e, e.Parent, [this](Environment& dst){
				auto& count(Universe.at(dst));

				YAssert(count > 0, "Invalid count found in trace record.");
				--count;
				return false;
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
				Traverse(e, e.get().Parent, [&](Environment& dst) ynothrow{
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

		// TODO: Full support of PTC by direct DFS traverse.
		ReferenceSet accessed;

		ystdex::retry_on_cond(ystdex::id<>(), [&]() -> bool{
			bool collected = {};

			Traverse(Root, Root.get().Parent, [&](Environment& dst,
				Environment& src, ValueObject& parent) -> bool{
				if(accessed.insert(src).second)
				{
					if(!ystdex::exists(Universe, ystdex::ref(dst)))
						return true;
					// NOTE: Variable %parent can be a single parent in a list
					//	of parent environments not equal to %src.Parent.
					parent = dst.Parent;
					collected = true;
				}
				return {};
			});
			return collected;
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

	//! \since build 882
	template<typename _fTracer>
	static void
	Traverse(Environment& e, ValueObject& parent, const _fTracer& trace)
	{
		using proxy_t = ystdex::expand_proxy<void(Environment&, Environment&,
			ValueObject&)>;
		const auto& tp(parent.type());

		if(tp == ystdex::type_id<EnvironmentList>())
		{
			for(auto& vo : parent.GetObject<EnvironmentList>())
				Traverse(e, vo, trace);
		}
		else if(tp == ystdex::type_id<EnvironmentReference>())
		{
			// XXX: The shared pointer should not be locked to ensure it neutral
			//	to nested calls.
			if(const auto p
				= parent.GetObject<EnvironmentReference>().Lock().get())
				if(proxy_t::call(trace, *p, e, parent))
					Traverse(*p, p->Parent, trace);
		}
		else if(tp == ystdex::type_id<shared_ptr<Environment>>())
		{
			// NOTE: The reference counts should not be effected here.
			if(const auto p = parent.GetObject<shared_ptr<Environment>>().get())
				if(proxy_t::call(trace, *p, e, parent))
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


//! \since build 876
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
	//! \since build 882
	lref<const EnvironmentReference> Referenced;

	//! \since build 882
	BindParameterObject(const EnvironmentReference& r_env)
		: Referenced(r_env)
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

					// XXX: Allocators are not used here on %TermReference for
					//	performance in most cases.
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
					ValueObject(std::allocator_arg, o.get_allocator(),
					in_place_type<TermReference>,
					GetLValueTagsOf(o.Tags | o_tags), o, Referenced));
			else
				cp(o);
		}
		else if(!temp)
			mv(TermNode::Container(o.get_allocator()),
				ValueObject(std::allocator_arg, o.get_allocator(),
				in_place_type<TermReference>, o_tags & TermTags::Nonmodifying,
				o, Referenced));
		else
			throw
				InvalidReference("Invalid operand found on binding sigil '@'.");
	}
};


//! \since build 882
//@{
#if false
// NOTE: Examples of types for %GParameterMatcher for exposition only,
//	equivalent to the instance used in %MatchParameter.
using GParameterBinder
	= function<void(_tParams..., TermTags, const EnvironmentReference&)>;

using _fBindTrailing = GParameterBinder<TNIter, TNIter, const TokenValue&>;
using _fBindValue = GParameterBinder<const TokenValue&, TermNode&>;
#endif

template<typename _fBindTrailing, typename _fBindValue>
class GParameterMatcher
{
	//! \since build 863
	//@{
public:
	_fBindTrailing BindTrailing;
	_fBindValue BindValue;

private:
	// XXX: Allocators are not used here for performance in most cases.
	mutable Action act{};
	//@}

public:
	template<class _type, class _type2>
	GParameterMatcher(_type&& arg, _type2&& arg2)
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
	void
	operator()(const TermNode& t, TermNode& o, TermTags o_tags,
		const EnvironmentReference& r_env) const
	{
		// NOTE: This is a trampoline to eliminate the call depth limitation.
		Match(t, o, o_tags, r_env);
		while(act)
		{
			const auto a(std::move(act));

			a();
		}
	}

private:
	void
	Match(const TermNode& t, TermNode& o, TermTags o_tags,
		const EnvironmentReference& r_env) const
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
							tags, p_ref ? p_ref->GetEnvironmentReference()
							: r_env, ellipsis
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
				ystdex::update_thunk(act, [&, o_tags]{
					Match(t.Value.GetObject<TermReference>().get(), o, o_tags,
						r_env);
				});
			else if(tp == ystdex::type_id<TokenValue>())
				BindValue(t.Value.GetObject<TokenValue>(), o, o_tags, r_env);
			else
				throw ParameterMismatch(ystdex::sfmt("Invalid parameter value"
					" '%s' found.", TermToString(t).c_str()));
		}
	}

	void
	MatchSubterms(TNCIter i, TNCIter last, TNIter j, TNIter o_last,
		TermTags tags, const EnvironmentReference& r_env, bool ellipsis
#if NPL_Impl_NPLA1_AssertParameterMatch
		, TNCIter t_end
#endif
		) const
	{
		if(i != last)
		{
			ystdex::update_thunk(act, [=, &r_env]{
				return MatchSubterms(std::next(i), last, std::next(j), o_last,
#if NPL_Impl_NPLA1_AssertParameterMatch
					tags, r_env, ellipsis, t_end);
#else
					tags, r_env, ellipsis);
#endif
			});
			YAssert(j != o_last, "Invalid state of operand found.");
			Match(NPL::Deref(i), NPL::Deref(j), tags, r_env);
		}
		else if(ellipsis)
		{
			const auto& lastv(NPL::Deref(last).Value);

			YAssert(lastv.type() == ystdex::type_id<TokenValue>(),
				"Invalid ellipsis sequence token found.");
			BindTrailing(j, o_last, lastv.GetObject<TokenValue>(), tags, r_env);
#if NPL_Impl_NPLA1_AssertParameterMatch
			YAssert(std::next(last) == t_end, "Invalid state found.");
#endif
		}
	}
};

//! \relates GParameterMatcher
template<typename _fBindTrailing, typename _fBindValue>
YB_ATTR_nodiscard inline GParameterMatcher<_fBindTrailing, _fBindValue>
MakeParameterMatcher(_fBindTrailing bind_trailing_seq,
	_fBindValue bind_value)
{
	return GParameterMatcher<_fBindTrailing, _fBindValue>(std::move(
		bind_trailing_seq), std::move(bind_value));
}
//@}

} // unnamed namespace;


ContextState::ContextState(pmr::memory_resource& rsrc)
	: ContextNode(rsrc)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	// NOTE: The guard object shall be fresh on the calls for reentrancy.
	Guard += [](TermNode&, ContextNode& ctx){
		// TODO: Support guarding for other states?
		return A1::Guard(std::allocator_arg, ctx.get_allocator(),
			in_place_type<ContextNode::ReductionGuard>, ctx);
	};
#endif
}
ContextState::ContextState(const ContextState& ctx)
	: ContextNode(ctx),
	EvaluateLeaf(ctx.EvaluateLeaf), EvaluateList(ctx.EvaluateList),
	EvaluateLiteral(ctx.EvaluateLiteral), Guard(ctx.Guard),
	ReduceOnce(ctx.ReduceOnce)
{}
ContextState::ContextState(ContextState&& ctx)
	: ContextNode(std::move(ctx)),
	EvaluateLeaf(ctx.EvaluateLeaf), EvaluateList(ctx.EvaluateList),
	EvaluateLiteral(ctx.EvaluateLiteral), Guard(ctx.Guard),
	ReduceOnce(ctx.ReduceOnce)
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
		if(term.size() != 1)
			return DoAdministratives(cs.EvaluateList, term, ctx);

		// XXX: This may be slightly more efficient, and more importantly,
		//	respecting to nested call safety on %ReduceAgain for the thunked
		//	implementation well by only allow one level of direct recursion.
		auto term_ref(ystdex::ref(term));

		ystdex::retry_on_cond([&]{
			return term_ref.get().size() == 1;
		}, [&]{
			term_ref = AccessFirstSubterm(term_ref);
		});
		return ReduceAgainLifted(term, ctx, term_ref);
	}
	return ReductionStatus::Retained;
}

ReductionStatus
ContextState::RewriteGuarded(TermNode& term, Reducer reduce)
{
	const auto gd(Guard(term, *this));
	const auto unwind(ystdex::make_guard([this]() ynothrow{
		UnwindCurrent();
	}));

	return Rewrite(std::move(reduce));
}

ReductionStatus
ContextState::RewriteTerm(TermNode& term)
{
	SetNextTermRef(term);
	return Rewrite(NPL::ToReducer(get_allocator(), std::ref(ReduceOnce)));
}

ReductionStatus
ContextState::RewriteTermGuarded(TermNode& term)
{
	SetNextTermRef(term);
	return RewriteGuarded(term,
		NPL::ToReducer(get_allocator(), std::ref(ReduceOnce)));
}

ReductionStatus
Reduce(TermNode& term, ContextNode& ctx)
{
	return ContextState::Access(ctx).RewriteTermGuarded(term);
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
	cs.Guard += std::bind([name](TermNode& term, ContextNode& ctx,
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
		Continuation(NameHandler([&](TermNode& t, ContextNode& c){
		YAssert(!t.empty(), "Invalid term found.");
		ReduceChildrenOrderedAsyncUnchecked(std::next(t.begin()), t.end(), c);
		return ReductionStatus::Partial;
	}, "eval-combine-operator"), ctx),
		NPL::ToReducer(ctx.get_allocator(), [&, n](ContextNode& c){
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
			if(CheckReducible(DefaultEvaluateLeaf(term, id)))
				term.Value = ValueObject(std::allocator_arg, a,
					in_place_type<TokenValue>, id, a);
				// NOTE: This is to be evaluated as identifier later.
			break;
			// XXX: Empty token is ignored.
			// XXX: Remained reducible?
		case LexemeCategory::Data:
			// XXX: This should be prevented being passed to second pass in
			//	%TermToNamePtr normally. This is guarded by normal form handling
			//	in the loop in %ContextNode::Rewrite with %ReduceOnce.
			term.Value = ValueObject(std::allocator_arg, a,
				in_place_type<string>, Deliteralize(id), a);
			YB_ATTR_fallthrough;
		default:
			break;
			// XXX: Handle other categories of literal?
		}
	return term;
}

TermNode
ParseLeafWithSourceInformation(string_view id, const shared_ptr<string>& name,
	const SourceLocation& src_loc, TermNode::allocator_type a)
{
	// NOTE: Most are %ParseLeaf, except for additional source information mixed
	//	into the values of %TokenValue.
	YAssertNonnull(id.data());
	auto term(NPL::AsTermNode(a));

	if(!id.empty())
		switch(CategorizeBasicLexeme(id))
		{
		case LexemeCategory::Code:
			id = DeliteralizeUnchecked(id);
			YB_ATTR_fallthrough;
		case LexemeCategory::Symbol:
			if(CheckReducible(DefaultEvaluateLeaf(term, id)))
				term.Value = ValueObject(std::allocator_arg, a,
					any_ops::use_holder, in_place_type<SourcedHolder<
					TokenValue>>, name, src_loc, id, a);
			break;
		case LexemeCategory::Data:
			term.Value = ValueObject(std::allocator_arg, a, any_ops::use_holder,
				in_place_type<SourcedHolder<string>>, name, src_loc,
				Deliteralize(id), a);
			YB_ATTR_fallthrough;
		default:
			break;
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
		// XXX: Allocators are not used here on %TermReference to avoid G++ from
		//	folding code with other basic blocks with more inefficient
		//	implementations.
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
			[&]() YB_FLATTEN{
				term.Value = TermReference(env.MakeTermTags(bound)
					& ~TermTags::Unique, bound, env.shared_from_this());
			}();
		}
		EvaluateLiteralHandler(term, ctx, *p_rterm);
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
	return IsBranchedList(term) ? ReduceCombinedBranch(term, ctx)
		: ReductionStatus::Clean;
}

ReductionStatus
ReduceCombinedBranch(TermNode& term, ContextNode& ctx)
{
	YAssert(IsBranchedList(term), "Invalid term found for combined term.");

	auto& fm(AccessFirstSubterm(term));
	const auto p_ref_fm(NPL::TryAccessLeaf<const TermReference>(fm));

	// NOTE: The tag is intended to be consumed by %VauHandler in %NPLA1Forms.
	//	As the irregular representation has a handler of %RefContextHandler,
	//	the tag is consumed only by the underlying handler, so the irregular
	//	representation behaves same to glvalue here. The value of %term.Tags may
	//	indicate a temporary of %TermReference value of %fm. This shall be
	//	cleared if the object represented by %fm is not a prvalue.
	if(p_ref_fm)
	{
		term.Tags &= ~TermTags::Temporary;
#if false
		// XXX: This converts the term with irregular representation at first.
		//	See %ReduceForCombinerRef for the introduction of the irregular
		//	representation.
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
		}
		else
#endif
		if(const auto p_handler
			= NPL::TryAccessLeaf<const ContextHandler>(p_ref_fm->get()))
			// NOTE: This is neutral to %NPL_Impl_NPLA1_Enable_Thunked.
			return CombinerReturnThunk(*p_handler, term, ctx);
	}
	else
		term.Tags |= TermTags::Temporary;
	// NOTE: Converted terms are also handled here.
	if(const auto p_handler = NPL::TryAccessTerm<ContextHandler>(fm))
#if NPL_Impl_NPLA1_Enable_TCO
		return
			CombinerReturnThunk(*p_handler, term, ctx, std::move(*p_handler));
#elif NPL_Impl_NPLA1_Enable_Thunked
	{
		// XXX: Optimize for performance using context-dependent store?
		// XXX: This should ideally be a member of handler. However, it makes no
		//	sense before allowing %ContextHandler overload for ref-qualifier
		//	'&&'.
		auto p(YSLib::share_move(ctx.get_allocator(), *p_handler));

		return CombinerReturnThunk(*p, term, ctx, std::move(p));
	}
#else
		return CombinerReturnThunk(ContextHandler(std::move(*p_handler)), term,
			ctx);
#endif
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

ReductionStatus
ReduceCombinedReferent(TermNode& term, ContextNode& ctx, const TermNode& fm)
{
	term.Tags &= ~TermTags::Temporary;
	if(const auto p_handler = NPL::TryAccessLeaf<const ContextHandler>(fm))
		return CombinerReturnThunk(*p_handler, term, ctx);
	throw ListReductionFailure(ystdex::sfmt("No matching combiner '%s'"
		" for operand with %zu argument(s) found.",
		TermToStringWithReferenceMark(fm, true).c_str(), FetchArgumentN(term)));
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
MatchParameter(const TermNode& t, TermNode& o, function<void(TNIter, TNIter,
	const TokenValue&, TermTags, const EnvironmentReference&)>
	bind_trailing_seq, function<void(const TokenValue&, TermNode&, TermTags,
	const EnvironmentReference&)> bind_value, TermTags o_tags,
	const EnvironmentReference& r_env)
{
	MakeParameterMatcher(std::move(bind_trailing_seq), std::move(bind_value))(t,
		o, o_tags, r_env);
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
	// NOTE: The call is essentially same as %MatchParameter, with a bit better
	//	performance by avoiding %function instances.
	MakeParameterMatcher([&, check_sigil](TNIter first, TNIter last,
		string_view id, TermTags o_tags, const EnvironmentReference& r_env){
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
					BindParameterObject{r_env}(sigil, o_tags,
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
	}, [&](const TokenValue& n, TermNode& b, TermTags o_tags,
		const EnvironmentReference& r_env){
		CheckParameterLeafToken(n, [&]{
			if(!n.empty())
			{
				string_view id(n);
				const char sigil(check_sigil(id));

				if(!id.empty())
					BindParameterObject{r_env}(sigil, o_tags, b,
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
	})(t, o, TermTags::Temporary, env.shared_from_this());
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
				return ReduceCombinedBranch(term, c);
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
	// NOTE: This implies the %RegularizeTerm call when necessary.
	// XXX: This should be the last of list pass for current TCO
	//	implementation, assumed by TCO action.
	passes += ReduceCombined;
#endif
	cs.EvaluateList = std::move(passes);
	// NOTE: This implies the %RegularizeTerm call when necessary.
	cs.EvaluateLeaf = ReduceLeafToken;
}


observer_ptr<const SourceInformation>
QuerySourceInformation(const ValueObject& vo)
{
	const auto val(vo.Query());

	return ystdex::call_value_or([](const SourceInfoMetadata& r) ynothrow{
		return make_observer(&r.get());
	}, val.try_get_object_ptr<SourceInfoMetadata>());
}

string_view
QueryContinuationName(const Continuation& cont)
{
	const auto& tbl(FetchNameTableRef<NPLA1Tag>());
	const auto& t_info(cont.Handler.target_type());

	if(t_info == ystdex::type_id<ContextHandler>())
		return "eval-combine-operands";

	const auto i(tbl.find(cont.Handler.target_type()));

	if(i != tbl.cend())
		return i->second;
	return {};
}


REPLContext::REPLContext(pmr::memory_resource& rsrc)
	: REPLContext([this](const GParsedValue<ByteParser>& str){
		return ParseLeaf(YSLib::make_string_view(str), Allocator);
	}, [this](const GParsedValue<SourcedByteParser>& val){
		return ParseLeafWithSourceInformation(YSLib::make_string_view(
			val.second), CurrentSource, val.first, Allocator);
	}, rsrc)
{}
REPLContext::REPLContext(Tokenizer leaf_conv,
	SourcedTokenizer sourced_leaf_conv, pmr::memory_resource& rsrc)
	: Allocator(&rsrc), Root(rsrc), Preprocess(SeparatorPass(Allocator)),
	ConvertLeaf(std::move(leaf_conv)),
	ConvertLeafSourced(std::move(sourced_leaf_conv))
{
	SetupDefaultInterpretation(Root, EvaluationPasses(Allocator));
}

bool
REPLContext::IsAsynchronous() const ynothrow
{
	return NPL_Impl_NPLA1_Enable_Thunked;
}

TermNode
REPLContext::ReadFrom(LoadOptionTag<>, std::streambuf& buf, ContextNode& ctx)
	const
{
	using s_it_t = std::istreambuf_iterator<char>;
	Session sess(ctx.get_allocator());

	if(UseSourceLocation)
	{
		SourcedByteParser parse(sess.Lexer, sess.get_allocator());

		return Prepare(sess,
			sess.Process(s_it_t(&buf), s_it_t(), ystdex::ref(parse)));
	}
	return Prepare(sess, sess.Process(s_it_t(&buf), s_it_t()));
}
TermNode
REPLContext::ReadFrom(LoadOptionTag<WithSourceLocation>, std::streambuf& buf,
	ContextNode& ctx) const
{
	using s_it_t = std::istreambuf_iterator<char>;
	Session sess(ctx.get_allocator());
	SourcedByteParser parse(sess.Lexer, sess.get_allocator());

	return
		Prepare(sess, sess.Process(s_it_t(&buf), s_it_t(), ystdex::ref(parse)));
}
TermNode
REPLContext::ReadFrom(LoadOptionTag<NoSourceInformation>, std::streambuf& buf,
	ContextNode& ctx) const
{
	using s_it_t = std::istreambuf_iterator<char>;
	Session sess(ctx.get_allocator());

	return Prepare(sess, sess.Process(s_it_t(&buf), s_it_t()));
}
TermNode
REPLContext::ReadFrom(LoadOptionTag<>, string_view unit, ContextNode& ctx) const
{
	YAssertNonnull(unit.data());
	Session sess(ctx.get_allocator());

	if(UseSourceLocation)
	{
		SourcedByteParser parse(sess.Lexer, sess.get_allocator());

		return Prepare(sess, sess.Process(unit, ystdex::ref(parse)));
	}
	return Prepare(sess, sess.Process(unit));
}
TermNode
REPLContext::ReadFrom(LoadOptionTag<WithSourceLocation>, string_view unit,
	ContextNode& ctx) const
{
	YAssertNonnull(unit.data());

	Session sess(ctx.get_allocator());
	SourcedByteParser parse(sess.Lexer, sess.get_allocator());

	return Prepare(sess, sess.Process(unit, ystdex::ref(parse)));
}
TermNode
REPLContext::ReadFrom(LoadOptionTag<NoSourceInformation>, string_view unit,
	ContextNode& ctx) const
{
	YAssertNonnull(unit.data());

	Session sess(ctx.get_allocator());

	return Prepare(sess, sess.Process(unit));
}

} // namesapce A1;

} // namespace NPL;

