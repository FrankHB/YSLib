﻿/*
	© 2014-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.cpp
\ingroup NPL
\brief NPLA1 公共接口。
\version r22575
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 18:02:47 +0800
\par 修改时间:
	2022-06-30 06:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1Forms // for NPL, EvaluationPasses, lref, ContextHandler,
//	RelaySwitched, trivial_swap, type_index, string_view, std::hash,
//	ystdex::equal_to, YSLib::unordered_map, std::piecewise_construct,
//	YSLib::lock_guard, YSLib::mutex, type_id, make_observer, IsBranch,
//	CheckReducible, IsNPLAExtendedLiteralNonDigitPrefix, IsAllSignLexeme,
//	AllocatorHolder, ystdex::ref, YSLib::IValueHolder,
//	YSLib::AllocatedHolderOperations, any, ystdex::as_const,
//	NPL::forward_as_tuple, uintmax_t, ystdex::bind1, TokenValue,
//	Forms::Sequence, std::allocator_arg, ReduceBranchToList, YSLib::stack,
//	YSLib::vector, TermTags, function, TermReference, GetLValueTagsOf,
//	TryAccessLeaf, ListReductionFailure, ystdex::sfmt, TryAccessLeafAtom,
//	PropagateTo, NPL::IsMovable, in_place_type, InvalidReference, NPL::Deref,
//	IsLeaf, ResolveTerm, ThrowInsufficientTermsError,
//	ThrowListTypeErrorForNonList, ystdex::update_thunk, Environment, shared_ptr,
//	NPL::AsTermNode, IsTyped, ystdex::retry_on_cond, AccessFirstSubterm,
//	ystdex::make_transform, IsBranchedList, std::placeholders, NoContainer,
//	ystdex::try_emplace, Access, YSLib::Informative, ystdex::unique_guard,
//	CategorizeBasicLexeme, DeliteralizeUnchecked, Deliteralize, ystdex::isdigit,
//	INT_MAX, ystdex::ref_eq, TryAccessTerm, YSLib::share_move,
//	ystdex::call_value_or, std::to_string, YSLib::Notice,
//	YSLib::FilterException, Session;
#include "NPLA1Internals.h" // for A1::Internals API;
#include YFM_NPL_NPLAMath // for ReadDecimal;
#include <limits> // for std::numeric_limits;
#include <ystdex/exception.h> // for ystdex::unsupported;
#include YFM_NPL_Dependency // for A1::OpenUnique;

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
//! \since build 948
//@{
// XXX: Different 'optimize' attributes of the caller and callees in GCC
//	prevents IPA inlining unconditionally (see %can_inline_edge_by_limits_p in
//	ipa-inline.cc) unless the caller has 'flatten' attribute or the callee has
//	'always_inline' attribute. These attributes should be propagated explicitly.
// So, they are not used in %GParameterMatcher. Only 'YB_ATTR_always_inline' is
//	conservatively used to reduce the overhead of inlining heuristics. Such
//	changes may reduce the typical build time (release, LTO) of libYFramework
//	with only changes in NPLA1.cpp using x86_64-pc-linux G++ 12.1 LTO for a
//	single instance of %GParameterMatcher (%BindParameterWellFormed only) from
//	360s to less than 90s, at the cost of a bit worse generated code. See also
//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=101279.
// NOTE: Optimization value for call expansions in %Forms::(LetAsterisk,
//	LetAsteriskRef), in range [0, 3]. The meanings of the value are:
//	0: Flatten no %BindParameterImpl instances
//		(defualt to debug or size-optimized releases).
//	1: Flatten only %BindParameterWellFormed.
//	2: Flatten all %BindParameterImpl instances (default for other releases).
//		This may even a bit better than 3 below for some compilation
//		configurations.
//	3: Flatten all %BindParameterImpl instances, (much) slower build (~7x).
#ifndef NPL_Impl_NPLA1_BindParameter_ExpandLevel
#	if !defined(NDEBUG) || __OPTIMIZE_SIZE__
#		define NPL_Impl_NPLA1_BindParameter_ExpandLevel 0
#	else
#		define NPL_Impl_NPLA1_BindParameter_ExpandLevel 2
#	endif
#endif
#if NPL_Impl_NPLA1_BindParameter_ExpandLevel == 1 \
		|| NPL_Impl_NPLA1_BindParameter_ExpandLevel == 2
#	define NPL_Impl_NPLA1_BindParameter_Inline YB_ATTR_always_inline
#	define NPL_Impl_NPLA1_BindParameter_InlineAnnotate(_arglist) \
	YB_LAMBDA_ANNOTATE(_arglist, , always_inline)
#else
#	define NPL_Impl_NPLA1_BindParameter_Inline
#	define NPL_Impl_NPLA1_BindParameter_InlineAnnotate(_arglist) _arglist
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
	case ValueToken::Ignore:
		return "ignore";
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

#if NPL_Impl_NPLA1_Enable_Thunked
//! \since build 897
struct PushedAction final
{
	EvaluationPasses::const_iterator First, Last;
	lref<const ContextHandler> HandlerRef;
	lref<TermNode> TermRef;
	lref<ContextNode> ContextRef;

	ReductionStatus
	operator()(ContextNode&) const;
};

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
			// XXX: By convention, no %SetupNextTerm call is needed here. Any
			//	necessary next term setup is in the concrete action handler.
			//	This avoids redundant calls if the action actually does know the
			//	current term from the argument is correct (e.g. the 1st
			//	action initialized by the call of
			//	%ContextState::DefaultReduceOnce) or all previous actions are
			//	known not to change the next term in the context. See
			//	%SetupDefaultInterpretation for example.
			RelaySwitched(ctx, trivial_swap,
				PushedAction{first, last, f, term, ctx});
		}
	}
	else
		// NOTE: This skips the remained incompleted sequence of passes in the
		//	subsequent evaluation (if any). Otherwise, %ReductionStatus::Neutral
		//	will be set before the 1st pass at %DoAdministratives for a complete
		//	evaluation.
		ctx.LastStatus = ReductionStatus::Retrying;
}

ReductionStatus
PushedAction::operator()(ContextNode&) const
{
	auto& term(TermRef.get());
	auto& ctx(ContextRef.get());

	PushActionsRange(First, Last, term, ctx);

	// NOTE: Calling the handler may change %ctx.LastStatus, so it should be
	//	saved at first.
	const auto r(ctx.LastStatus);
	const auto res(HandlerRef(term, ctx));

	if(res != ReductionStatus::Partial)
		// NOTE: This does maintain the right reduction status for each term,
		//	once the result of the call to %HandlerRef is trusted.
		ctx.LastStatus = CombineSequenceReductionResult(r, res);
	return ctx.LastStatus;
}
#endif

//! \since build 939
using YSLib::lock_guard;
//! \since build 939
using YSLib::mutex;
//! \since build 893
mutex NameTableMutex;

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
//@}

#if NPL_Impl_NPLA1_Enable_Thunked
//! \since build 810
YB_ATTR_always_inline inline ReductionStatus
ReduceChildrenOrderedAsync(TNIter, TNIter, ContextNode&);

//! \since build 879
ReductionStatus
ReduceChildrenOrderedAsyncUnchecked(TNIter first, TNIter last, ContextNode& ctx)
{
	YAssert(first != last, "Invalid range found.");

	auto& term(*first++);

	return ReduceSubsequent(term, ctx, Continuation(trivial_swap,
		NameTypedContextHandler([first, last](TermNode&, ContextNode& c){
		return ReduceChildrenOrderedAsync(first, last, c);
	}, "eval-argument-list"), ctx));
}

YB_ATTR_always_inline inline ReductionStatus
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
	// NOTE: See $2021-01 @ %Documentation::Workflow.
	static_assert(sizeof...(args) < 2, "Unsupported owner arguments found.");
#if NPL_Impl_NPLA1_Enable_TCO
	auto& act(EnsureTCOAction(ctx, term));

	ContextState::Access(ctx).ClearCombiningTerm();
	// XXX: This is necessary to the precondition for calling the handler.
	term.Value.Clear();
	SetupNextTerm(ctx, term);
	// XXX: %A1::RelayCurrentOrDirect is not used to allow the call to the
	//	underlying handler implementation (e.g. %FormContextHandler::CallN)
	//	optimized with %NPL_Impl_NPLA1_Enable_InlineDirect remaining the nested
	//	call safety.
	// XXX: The %std::reference_wrapper instance is specialized enough without
	//	%trivial_swap.
	return
		RelaySwitched(ctx, Continuation(act.Attach(h, yforward(args)...), ctx));
#else

	ContextState::Access(ctx).ClearCombiningTerm();
	// XXX: Ditto.
	term.Value.Clear();

	auto gd(ystdex::unique_guard([&]() ynothrow{
		// XXX: This term is fixed, as in the term cleanup in %TCOAction.
		term.Clear();
	}));
#	if NPL_Impl_NPLA1_Enable_Thunked
	// XXX: %A1::ReduceCurrentNext is not used to allow the underlying
	//	handler optimized with %NPL_Impl_NPLA1_Enable_InlineDirect.
	SetupNextTerm(ctx, term);
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	RelaySwitched(ctx, trivial_swap, A1::NameTypedReducerHandler(
		std::bind([&](decltype(gd)& g, const _tParams&...){
		ystdex::dismiss(g);
		// NOTE: Captured argument pack is only needed when %h actually shares.
		return RegularizeTerm(term, ctx.LastStatus);
	}, std::move(gd), std::move(args)...), "combine-return"));
	// XXX: The %std::reference_wrapper instance is specialized enough without
	//	%trivial_swap.
	return RelaySwitched(ctx, Continuation(std::ref(h), ctx));
#	else
	const auto res(RegularizeTerm(term, h(term, ctx)));

	ystdex::dismiss(gd);
	yunseq(0, args...);
	return res;
#	endif
#endif
}

/*!
\warning 若不满足 NPLA1 规约函数上下文状态类型要求，行为未定义。
\since build 948
*/
YB_NORETURN ReductionStatus
ThrowCombiningFailure(TermNode& term, const ContextNode& ctx,
	const TermNode& fm, bool has_ref)
{
	// NOTE: Try to extract the identifier set by %SetupTailOperatorName.
	string name(term.get_allocator());

	if(const auto p = ContextState::Access(ctx).TryGetTailOperatorName(term))
	{
		name = std::move(*p);
		name += ": ";
	}
	name += TermToStringWithReferenceMark(fm, has_ref).c_str();
	// XXX: All other value is ignored. As %CombinerReturnThunk, this is
	//	necessary to the precondition for calling %FetchArgumentN.
	term.Value.Clear();
	// TODO: Extract general form by some information extractor function.
	throw ListReductionFailure(ystdex::sfmt("No matching combiner '%s'"
		" for operand with %zu argument(s) found.", name.c_str(),
		FetchArgumentN(term)));
}


//! \since build 851
YB_ATTR_nodiscard ReductionStatus
DoAdministratives(const EvaluationPasses& passes, TermNode& term,
	ContextNode& ctx)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	// XXX: No %SetupNextTerm call is needed here because it should have been
	//	called before entering %ContextState::DefaultReduceOnce (e.g. in
	//	%ReduceOnce).
	AssertNextTerm(ctx, term);
	// XXX: Be cautious with overflow risks in call of %ContextNode::ApplyTail
	//	when TCO is not enabled.
	ctx.LastStatus = ReductionStatus::Neutral;
	PushActionsRange(passes.cbegin(), passes.cend(), term, ctx);
	return ReductionStatus::Partial;
#else
	return passes(term, ctx);
#endif
}

#if NPL_Impl_NPLA1_Enable_TCO
//! \since build 893
struct EvalSequence final
{
	lref<TermNode> TermRef;
	TNIter Iter;

	//! \since build 897
	ReductionStatus
	operator()(ContextNode&) const;
};

//! \since build 823
ReductionStatus
ReduceSequenceOrderedAsync(TermNode& term, ContextNode& ctx, TNIter i)
{
	YAssert(i != term.end(), "Invalid iterator found for sequence reduction.");
	// TODO: Allow capture next sequenced evaluations as a single continuation?
	return std::next(i) == term.end() ? ReduceOnceLifted(term, ctx, *i)
		: ReduceSubsequent(*i, ctx,
		NameTypedReducerHandler(EvalSequence{term, i}, "eval-sequence"));
}

ReductionStatus
EvalSequence::operator()(ContextNode& ctx) const
{
	return ReduceSequenceOrderedAsync(TermRef, ctx, TermRef.get().erase(Iter));
}
#else
//! \since build 896
void
ReduceOrderedResult(TermNode& term)
{
	if(IsBranch(term))
		LiftTerm(term, *term.rbegin());
	else
		term.Value = ValueToken::Unspecified;
}
#endif

//! \since build 915
void
EmplaceReference(TermNode::Container& con, TermNode& o, TermReference& ref,
	bool move)
{
	if(move)
		con.emplace_back(std::move(o.GetContainerRef()),
			ValueObject(std::allocator_arg, con.get_allocator(),
			in_place_type<TermReference>, std::move(ref)));
	else
		con.emplace_back(o.GetContainer(),
			ValueObject(std::allocator_arg, con.get_allocator(),
			in_place_type<TermReference>, ref.GetTags(), ref));
}


//! \since build 936
YB_ATTR_nodiscard YB_PURE bool
ParseSymbol(TermNode& term, string_view id)
{
	YAssertNonnull(id.data());
	YAssert(!id.empty(), "Invalid lexeme found.");
	if(CheckReducible(DefaultEvaluateLeaf(term, id)))
	{
		// NOTE: Supported liteals should have been comsumed in
		//	%DefaultEvaluateLeaf. Other extended literals are unsupported.
		if(id.size() > 1 && YB_UNLIKELY(IsNPLAExtendedLiteralNonDigitPrefix(
			id.front()) && !IsAllSignLexeme(id)))
			ThrowUnsupportedLiteralError(id);
		// NOTE: This is to be evaluated as identifier later.
		return true;
	}
	return {};
}


//! \since build 891
//@{
using SourcedByteAllocator = YSLib::default_allocator<yimpl(byte)>;

using SourceInfoMetadata = lref<const SourceInformation>;


template<typename _type, class _tByteAlloc = SourcedByteAllocator>
class SourcedHolder : public YSLib::AllocatorHolder<_type, _tByteAlloc>
{
public:
	using Creation = YSLib::IValueHolder::Creation;
	using value_type = _type;

private:
	using base = YSLib::AllocatorHolder<_type, _tByteAlloc>;

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
			_tByteAlloc>::CreateHolder(c, x, value, NPL::forward_as_tuple(
			source_information, ystdex::as_const(value)), NPL::forward_as_tuple(
			source_information, std::move(value)));
	}

	YB_ATTR_nodiscard YB_PURE PDefH(any, Query, uintmax_t) const ynothrow
		ImplI(IValueHolder)
		ImplRet(ystdex::ref(source_information))

	using base::get_allocator;
};
//@}


//! \since build 881
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
	// XXX: More allocators are not used here for performance.
	ValueObject pfx{std::allocator_arg, alloc, ContextHandler(Forms::Sequence)};
	ValueObject pfx2{std::allocator_arg, alloc,
		ContextHandler(FormContextHandler(ReduceBranchToList, 1))};
#if NPL_Impl_NPLA1_Enable_ThunkedSeparatorPass

	//! \since build 882
	using TermStack
		= YSLib::stack<lref<TermNode>, YSLib::vector<lref<TermNode>>>;

	//! \since build 882
	mutable TermStack remained{alloc};
#endif

public:
	//! \since build 888
	SeparatorPass(TermPasses::allocator_type a)
		: alloc(a)
	{}

	ReductionStatus
	operator()(TermNode& term) const
	{
#if NPL_Impl_NPLA1_Enable_ThunkedSeparatorPass
		YAssert(remained.empty(), "Invalid state found.");
		Transform(term);
#	if NPL_Impl_NPLA1_Enable_ThunkedThreshold == 0
		YAssert(!remained.empty(), "Invalid state found.");
		// XXX: For some reason, 'while' here is more efficient than 'do' in the
		//	generated code with x86_64-pc-linux G++ 11.1.
#	endif
		while(!remained.empty())
		{
			const auto term_ref(std::move(remained.top()));

			remained.pop();
			for(auto& tm : term_ref.get())
				Transform(tm);
		}
#else
		Transform(term);
#endif
		return ReductionStatus::Clean;
	}

private:
	YB_FLATTEN void
	Transform(TermNode& term
#	if defined(NPL_Impl_NPLA1_Enable_ThunkedSeparatorPass) \
	&& NPL_Impl_NPLA1_Enable_ThunkedThreshold != 0
		, size_t n = 0
#	endif
	) const
	{
#if NPL_Impl_NPLA1_Enable_ThunkedSeparatorPass
#	if NPL_Impl_NPLA1_Enable_ThunkedThreshold != 0
		if(++n < NPL_Impl_NPLA1_Enable_ThunkedThreshold)
		{
			TransformLeaf(term);
			for(auto& tm : term)
				Transform(tm, n);
			return;
		}
#	endif
		remained.push(term);
		TransformLeaf(term);
#else
		TransformLeaf(term);
		for(auto& tm : term)
			Transform(tm);
#endif
	}

	//! \since build 944
	void
	TransformLeaf(TermNode& term) const
	{
		SeparatorTransformer::ReplaceChildren(term, pfx, std::ref(filter));
		SeparatorTransformer::ReplaceChildren(term, pfx2, std::ref(filter2));
	}
};


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

	//! \since build 916
	template<typename _fCopy, typename _fMove>
	void
	operator()(char sigil, bool ref_temp, TermTags o_tags, TermNode& o,
		_fCopy cp, _fMove mv) const
	{
		// NOTE: This shall be %true if the operand is stored in a term tree to
		//	be reduced (and eventually cleanup). See also
		//	%GParameterMatcher::Match.
		const bool temp(bool(o_tags & TermTags::Temporary));

		// NOTE: The binding rules here should be carefully tweaked to make them
		//	exactly accept expression representations (in %TermNode) in NPLA1
		//	all around.
		if(sigil != '@')
		{
			const bool can_modify(!bool(o_tags & TermTags::Nonmodifying));
			const auto a(o.get_allocator());

			// NOTE: Subterms in arguments retained are also transferred for
			//	values.
			if(const auto p = TryAccessLeafAtom<TermReference>(o))
			{
				if(sigil != char())
				{
					const auto ref_tags(PropagateTo(ref_temp
						? BindReferenceTags(*p) : p->GetTags(), o_tags));

					// XXX: Allocators are not used here on %TermReference for
					//	performance in most cases.
					if(can_modify && temp)
						// NOTE: Reference collapsed by move.
						mv(std::move(o.GetContainerRef()),
							ValueObject(std::allocator_arg, a, in_place_type<
							TermReference>, ref_tags, std::move(*p)));
					else
						// NOTE: Reference collapsed by copy.
						mv(TermNode::Container(o.GetContainer()),
							ValueObject(std::allocator_arg, a,
							in_place_type<TermReference>, ref_tags, *p));
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
			// NOTE: This saves non-reference temporary objects into the
			//	environment. No temporary objects outside the environment (in
			//	the tree being reduced) can be referenced in the object language
			//	later. There is no need to save them elsewhere even in the TCO
			//	implementation.
			else if((can_modify || sigil == '%') && temp)
				// XXX: The object is bound to reference as temporary, implying
				//	copy elision in the object language.
				MarkTemporaryTerm(mv(std::move(o.GetContainerRef()),
					std::move(o.Value)), sigil);
			// NOTE: Binding on list prvalues is always unsafe. However, since
			//	%TermTags::Nonmodifying is not allowed on prvalues, being not
			//	%can_modify currently implies some ancestor of %o is a
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
					ValueObject(std::allocator_arg, a,
					in_place_type<TermReference>,
					GetLValueTagsOf(o.Tags | o_tags), o, Referenced));
			else
				cp(o);
		}
		else if(!temp)
			// XXX: Ditto, except that tags other than %TermTags::Nonmodifying
			//	as well as %o.Tags are ignored intentionally. This may cause
			//	differences on derivations using '@' or not, see $2021-08
			//	@ %Documentation::Workflow.
			mv(TermNode::Container(o.get_allocator()),
				ValueObject(std::allocator_arg, o.get_allocator(),
				in_place_type<TermReference>, o_tags & TermTags::Nonmodifying,
				o, Referenced));
		else
			throw
				InvalidReference("Invalid operand found on binding sigil '@'.");
	}
};


//! \since build 917
struct ParameterCheck final
{
	using HasReferenceArg = ystdex::true_;

	NPL_Impl_NPLA1_BindParameter_Inline static void
	CheckBack(const TermNode& t, bool t_has_ref)
	{
		if(YB_UNLIKELY(!IsList(t)))
			ThrowFormalParameterTypeError(t, t_has_ref);
	}

	template<typename _func>
	NPL_Impl_NPLA1_BindParameter_Inline static inline void
	HandleLeaf(_func f, const TermNode& t, bool t_has_ref)
	{
		HandleOrIgnore(std::ref(f), t, t_has_ref);
	}

	template<typename _func>
	NPL_Impl_NPLA1_BindParameter_Inline static inline void
	WrapCall(_func f)
	{
		try
		{
			f();
		}
		CatchExpr(ParameterMismatch&, throw)
		CatchExpr(..., ThrowNestedParameterTreeMismatch())
	}
};


//! \since build 917
struct NoParameterCheck final
{
	using HasReferenceArg = ystdex::false_;

	static void
	CheckBack(const TermNode& t)
	{
		yunused(t);
		YAssert(IsList(t), "Invalid parameter tree found.");
	}

	template<typename _func>
	NPL_Impl_NPLA1_BindParameter_Inline static inline void
	HandleLeaf(_func f, const TermNode& t)
	{
		if(!IsIgnore(t))
		{
			const auto p(TermToNamePtr(t));

			YAssert(p, "Invalid parameter tree found.");
			f(*p);
		}
	}

	template<typename _func>
	static inline void
	WrapCall(_func f)
	{
		f();
	}
};


//! \since build 882
//@{
#if false
// NOTE: Examples of types for %GParameterMatcher for exposition only,
//	equivalent to the instance used in %MatchParameter.
using GParameterBinder
	= function<void(_tParams..., TermTags, const EnvironmentReference&)>;

using _fBindTrailing
	= GParameterBinder<TermNode::Container&, TNIter, const TokenValue&>;
using _fBindValue = GParameterBinder<const TokenValue&, TermNode&>;
#endif
//@}

//! \since build 917
//@{
template<class _tTraits, typename _fBindTrailing, typename _fBindValue>
class GParameterMatcher final
{
	//! \since build 863
	//@{
public:
	_fBindTrailing BindTrailing;
	_fBindValue BindValue;

private:
	mutable Action act{};
	//@}

public:
	//! \since build 882
	template<class _type, class _type2>
	inline
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
	//! \since build 882
	void
	operator()(const TermNode& t, TermNode& o, TermTags o_tags,
		const EnvironmentReference& r_env) const
	{
		_tTraits::WrapCall([&] NPL_Impl_NPLA1_BindParameter_InlineAnnotate(()){
			// NOTE: This is a trampoline to eliminate the call depth
			//	limitation.
			DispatchMatch(typename _tTraits::HasReferenceArg(), t, o, o_tags,
				r_env, ystdex::false_());
			while(act)
			{
				const auto a(std::move(act));

				a();
			}
		});
	}

private:
	template<class _tArg>
	NPL_Impl_NPLA1_BindParameter_Inline inline void
	DispatchMatch(ystdex::true_, const TermNode& t, TermNode& o,
		TermTags o_tags, const EnvironmentReference& r_env, _tArg) const
	{
		Match(t, o, o_tags, r_env, _tArg::value);
	}
	template<class _tArg>
	NPL_Impl_NPLA1_BindParameter_Inline inline void
	DispatchMatch(ystdex::false_, const TermNode& t, TermNode& o,
		TermTags o_tags, const EnvironmentReference& r_env, _tArg) const
	{
		Match(t, o, o_tags, r_env);
	}

	// XXX: The initial %o_tags shall have %TermTags::Temporary unless it is
	//	known to bound to some non-temporary objects not stored in the term tree
	//	to be reduced.
	template<typename... _tParams>
	NPL_Impl_NPLA1_BindParameter_Inline YB_FLATTEN inline void
	Match(const TermNode& t, TermNode& o, TermTags o_tags,
		const EnvironmentReference& r_env, _tParams&&... args) const
	{
		if(IsList(t))
		{
			if(IsBranch(t))
			{
				const auto n_p(t.size());
				auto last(t.end());

				if(n_p > 0)
				{
					const auto&
						back(ReferenceTerm(NPL::Deref(std::prev(last))));

					// NOTE: Empty list lvalue arguments shall not be matched
					//	here.
					if(IsLeaf(back))
					{
						if(const auto p
							= TryAccessLeafAtom<TokenValue>(back))
						{
							if(!p->empty() && p->front() == '.')
								--last;
						}
						else if(!IsIgnore(back))
							_tTraits::CheckBack(back, yforward(args)...);
					}
				}
				// XXX: There is only one level of indirection. It should work
				//	with the correct implementation of the reference collapse.
				ResolveTerm([&, o_tags]
					NPL_Impl_NPLA1_BindParameter_InlineAnnotate(
					(TermNode& nd, ResolvedTermReferencePtr p_ref)){
					if(IsList(nd))
					{
						const bool ellipsis(last != t.end());
						const auto n_o(nd.size());

						if(n_p == n_o || (ellipsis && n_o >= n_p - 1))
						{
							auto tags(o_tags);

							// NOTE: All tags as type qualifiers should be
							//	checked here. Currently only glvalues can be
							//	qualified.
							// XXX: Term tags are currently not respected in
							//	prvalues.
							if(p_ref)
							{
								const auto ref_tags(p_ref->GetTags());

								// NOTE: Drop the temporary tag unconditionally.
								//	Even the referent is a list temporary
								//	object, its elements are not prvalues to be
								//	matched (whatever the types thereof).
								tags = (tags
									& ~(TermTags::Unique | TermTags::Temporary))
									| (ref_tags & TermTags::Unique);
								// NOTE: Propagate %TermTags::Nonmodifying to
								//	the referent.
								tags = PropagateTo(tags, ref_tags);
							}
							MatchSubterms(t.begin(), last, nd, nd.begin(), tags,
								p_ref ? p_ref->GetEnvironmentReference()
								: r_env, ellipsis
#if NPL_Impl_NPLA1_AssertParameterMatch
								, t.end()
#endif
								);
						}
						else if(!ellipsis)
							throw ArityMismatch(n_p, n_o);
						else
							ThrowInsufficientTermsError(nd, p_ref);
					}
					else
						ThrowListTypeErrorForNonList(nd, p_ref);
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
		else if(const auto p_t = TryAccessLeafAtom<const TermReference>(t))
		{
			auto& nd(p_t->get());

			ystdex::update_thunk(act, o.get_allocator(), trivial_swap,
				[&, o_tags] NPL_Impl_NPLA1_BindParameter_InlineAnnotate(()){
				DispatchMatch(typename _tTraits::HasReferenceArg(), nd, o,
					o_tags, r_env, ystdex::true_());
			});
		}
		else
			_tTraits::HandleLeaf([&] NPL_Impl_NPLA1_BindParameter_InlineAnnotate
				((const TokenValue& n)){
				BindValue(n, o, o_tags, r_env);
			}, t, yforward(args)...);
	}

	//! \since build 898
	NPL_Impl_NPLA1_BindParameter_Inline void
	MatchSubterms(TNCIter i, TNCIter last, TermNode& o_tm, TNIter j,
		TermTags tags, const EnvironmentReference& r_env, bool ellipsis
#if NPL_Impl_NPLA1_AssertParameterMatch
		, TNCIter t_end
#endif
		) const
	{
		if(i != last)
		{
			// XXX: Use explicit captures here to ensure ISO C++20
			//	compatibility.
			ystdex::update_thunk(act, o_tm.get_allocator(), trivial_swap,
				[this, i, j, last, tags, ellipsis,
#if NPL_Impl_NPLA1_AssertParameterMatch
				t_end,
#endif
				&o_tm, &r_env]{
				return MatchSubterms(std::next(i), last, o_tm, std::next(j),
#if NPL_Impl_NPLA1_AssertParameterMatch
					tags, r_env, ellipsis, t_end);
#else
					tags, r_env, ellipsis);
#endif
			});
			YAssert(j != o_tm.end(), "Invalid state of operand found.");
			DispatchMatch(typename _tTraits::HasReferenceArg(), NPL::Deref(i),
				NPL::Deref(j), tags, r_env, ystdex::false_());
		}
		else if(ellipsis)
		{
			const auto& lastv(NPL::Deref(last).Value);

			YAssert(lastv.type() == type_id<TokenValue>(),
				"Invalid ellipsis sequence token found.");
			BindTrailing(o_tm, j, lastv.GetObject<TokenValue>(), tags, r_env);
#if NPL_Impl_NPLA1_AssertParameterMatch
			YAssert(std::next(last) == t_end, "Invalid state found.");
#endif
		}
	}
};

//! \relates GParameterMatcher
template<class _tTraits, typename _fBindTrailing, typename _fBindValue>
YB_ATTR_nodiscard NPL_Impl_NPLA1_BindParameter_Inline inline
	GParameterMatcher<_tTraits, _fBindTrailing, _fBindValue>
MakeParameterMatcher(_fBindTrailing bind_trailing_seq, _fBindValue bind_value)
{
	return GParameterMatcher<_tTraits, _fBindTrailing, _fBindValue>(
		std::move(bind_trailing_seq), std::move(bind_value));
}

//! \since build 948
// XXX: 'YB_FLATTEN' cannot used with G++ 12.1.0 when %BindParameterImpl is also
//	using 'YB_FLATTEN', to avoid ICE in 'gimple_duplicate_bb' at
//	'tree-cfg.c:6420'. This is no more efficient otherwise.
void
BindRawSymbol(const EnvironmentReference& r_env, string_view id,
	TermNode& b, TermTags o_tags, Environment& env, char sigil)
{
	BindParameterObject{r_env}(sigil, sigil == '&', o_tags, b,
		[&](const TermNode& tm){
		CopyTermTags(env.Bind(id, tm), tm);
	}, [&](TermNode::Container&& c, ValueObject&& vo) -> TermNode&{
		// XXX: Allocators are not used here for performance.
		return env.Bind(id, TermNode(std::move(c), std::move(vo)));
	});
}

//! \since build 920
// XXX: 'YB_FLATTEN' does not work better than 'inline'.
NPL_Impl_NPLA1_BindParameter_Inline inline void
BindSymbolImpl(const EnvironmentReference& r_env, const TokenValue& n,
	TermNode& b, TermTags o_tags, Environment& env)
{
	string_view id(n);
	const char sigil(ExtractSigil(id));

	BindRawSymbol(r_env, id, b, o_tags, env, sigil);
}

template<class _tTraits>
// XXX: 'YB_FLATTEN' here may make the generated code a bit more efficient, but
//	significantly slow in compiling without manually annotated 'always_inline',
//	and the call sites may have other choices, so it is conditonally enabled.
//	(It is actually not better than combination with manually annotated
//	'always_inline', at least with x86_64-pc-linux G++ 12.1.) The 'inline' is
//	significant for the generated code quality when 'YB_FLATTEN' is missing with
//	almost same compilation efficient as before, and 'YB_ATTR_always_inline' is
//	worse. See %NPL_Impl_NPLA1_BindParameter_ExpandLevel for more details.
#if NPL_Impl_NPLA1_BindParameter_ExpandLevel >= 2
YB_FLATTEN
#endif
inline void
BindParameterImpl(const shared_ptr<Environment>& p_env, const TermNode& t,
	TermNode& o)
{
	auto& env(NPL::Deref(p_env));

	// NOTE: No duplication check here. Symbols can be rebound.
	// NOTE: The call is essentially same as %MatchParameter, with a bit better
	//	performance by avoiding %function instances.
	MakeParameterMatcher<_tTraits>([&](TermNode& o_tm, TNIter first,
		string_view id, TermTags o_tags, const EnvironmentReference& r_env){
		YAssert(ystdex::begins_with(id, "."), "Invalid symbol found.");
		id.remove_prefix(1);
		// NOTE: Ignore the name of single '.'.
		if(!id.empty())
		{
			const char sigil(ExtractSigil(id));

			// NOTE: Ignore the name of single '.' followed by a sigil.
			if(!id.empty())
			{
				const auto a(o_tm.get_allocator());
				const auto last(o_tm.end());
				TermNode::Container con(a);

				// XXX: As %TermReference::IsMovable for non-temporary objects.
				if((o_tags & (TermTags::Unique | TermTags::Nonmodifying))
					== TermTags::Unique || bool(o_tags & TermTags::Temporary))
				{
					if(sigil == char())
						LiftSubtermsToReturn(o_tm);
					// NOTE: This implements the copy elision of list members.
					con.splice(con.end(), o_tm.GetContainerRef(), first, last);
					MarkTemporaryTerm(env.Bind(id, TermNode(std::move(con))),
						sigil);
				}
				else
				{
					// NOTE: Make sublist reference.
					for(; first != last; ++first)
						BindParameterObject{r_env}(sigil, {}, o_tags,
							NPL::Deref(first), [&](const TermNode& tm){
							CopyTermTags(con.emplace_back(tm.GetContainer(),
								tm.Value), tm);
						}, [&](TermNode::Container&& c, ValueObject&& vo)
							-> TermNode&{
							con.emplace_back(std::move(c), std::move(vo));
							return con.back();
						});
					if(sigil == '&')
					{
						// NOTE: Irregular representation is constructed for the
						//	sublist reference.
						// XXX: As %ReduceAsSubobjectReference in
						//	NPLA1Internals.
						auto p_sub(YSLib::allocate_shared<TermNode>(a,
							std::move(con)));
						auto& sub(NPL::Deref(p_sub));

						env.Bind(id, TermNode(std::allocator_arg, a, [&]{
							TermNode::Container tcon(a);

							tcon.push_back(NPL::AsTermNodeTagged(a,
								TermTags::Sticky, std::move(p_sub)));
							return tcon;
						}(), std::allocator_arg, a, TermReference(sub, r_env)));
					}
					else
						MarkTemporaryTerm(env.Bind(id,
							TermNode(std::move(con))), sigil);
				}
			}
		}
	}, [&] NPL_Impl_NPLA1_BindParameter_InlineAnnotate((const TokenValue& n,
		TermNode& b, TermTags o_tags, const EnvironmentReference& r_env)){
		BindSymbolImpl(r_env, n, b, o_tags, env);
	})(t, o, TermTags::Temporary, p_env);
}
//@}

} // unnamed namespace;


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
ThrowUnsupportedLiteralError(const char* id)
{
	throw InvalidSyntax(ystdex::sfmt(
		id[0] != '#' ? "Unsupported literal prefix found in literal '%s'."
		: "Invalid literal '%s' found.", id));
}

void
ThrowValueCategoryError(const TermNode& term)
{
	throw ValueCategoryMismatch(ystdex::sfmt("Expected a reference for the 1st "
		"argument, got '%s'.", TermToString(term).c_str()));
}


ContextState::ContextState(pmr::memory_resource& rsrc)
	: ContextNode(rsrc)
{
	// NOTE: The guard object shall be fresh on the calls for reentrancy.
	// XXX: The empty type is specialized enough without %trivial_swap.
	Guard += GuardPasses::HandlerType(std::allocator_arg, get_allocator(),
		[](TermNode&, ContextNode& ctx){
		// TODO: Support guarding for other states?
		return A1::Guard(std::allocator_arg, ctx.get_allocator(),
			in_place_type<ReductionGuard>, ctx);
	});
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
	if(next_term_ptr)
		return *next_term_ptr;
	// NOTE: This should not occur unless there exists some invalid low-level
	//	interoperations on the next term pointer in the context.
	throw NPLException("No next term found to evaluation.");
}

ReductionStatus
ContextState::DefaultReduceOnce(TermNode& term, ContextNode& ctx)
{
	AssertValueTags(term);

	// NOTE: For efficiency, only quite a few kinds of terms would be reduced
	//	here. More specific handling can be implemented by context handlers or
	//	replacement of %ContextState::DefaultReduceOnce.
	auto& cs(ContextState::Access(ctx));
	const bool non_list(!IsList(term));

	// NOTE: Empty list or special value token has no-op to do with.
	if(non_list)
	{
		// NOTE: The reduction relies on proper handling of reduction status and
		//	proper tail action for the thunked implementations.
		if(!IsTyped<ValueToken>(term))
			return DoAdministratives(cs.EvaluateLeaf, term, ctx);
	}
	else if(IsBranch(term))
	{
		YAssert(term.size() != 0, "Invalid term found.");
		// NOTE: List with single element shall be reduced as the element.
		if(term.size() != 1)
			return DoAdministratives(cs.EvaluateList, term, ctx);

		// XXX: This may be slightly more efficient, and more importantly,
		//	respecting to the nested call safety on %ReduceOnce for the thunked
		//	implementation well by only allow one level of direct recursion.
		auto term_ref(ystdex::ref(term));

		ystdex::retry_on_cond([&]{
			auto& tm(term_ref.get());

			return IsList(tm) && tm.size() == 1;
		}, [&]{
			term_ref = AccessFirstSubterm(term_ref);
		});
		// XXX: No need to use %A1::RelayCurrent here since it should not be
		//	reentered after the %ystdex::retry_on_cond call above as per the
		//	semantic rules of the default NPLA1 reduction. This also implies no
		//	continuation or reducer name.
		return ReduceOnceLifted(term, ctx, term_ref);
	}
	return ReductionStatus::Retained;
}

ReductionStatus
ContextState::RewriteGuarded(TermNode& term, Reducer reduce)
{
	const auto gd(Guard(term, *this));
	const auto unwind(ystdex::make_guard([this]() ynothrow{
		TailAction = nullptr;
		UnwindCurrent();
	}));

	return Rewrite(std::move(reduce));
}

ReductionStatus
ContextState::RewriteTerm(TermNode& term)
{
	AssertValueTags(term);
	SetNextTermRef(term);
	// XXX: The %std::reference_wrapper instance is specialized enough without
	//	%trivial_swap.
	return Rewrite(NPL::ToReducer(get_allocator(), std::ref(ReduceOnce)));
}

ReductionStatus
ContextState::RewriteTermGuarded(TermNode& term)
{
	AssertValueTags(term);
	SetNextTermRef(term);
	// XXX: Ditto.
	return RewriteGuarded(term,
		NPL::ToReducer(get_allocator(), std::ref(ReduceOnce)));
}

bool
ContextState::TrySetTailOperatorName(TermNode& term) const ynothrow
{
	if(combining_term_ptr)
	{
		auto& comb(*combining_term_ptr);

		if(IsBranch(comb) && ystdex::ref_eq<>()(AccessFirstSubterm(comb), term))
		{
			OperatorName = std::move(term.Value);
			return true;
		}
	}
	return {};
}


ReductionStatus
Reduce(TermNode& term, ContextNode& ctx)
{
	return ContextState::Access(ctx).RewriteTermGuarded(term);
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
	// NOTE: This does not support PTC, but allow it to be called in routines
	//	which expect proper tail actions, given the guarnatee that the
	//	precondition of %Reduce is not violated.
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
	// XXX: Check explicitly here to prevent custom implementation of
	//	%ContextState::ReduceOnce from missing the check.
	AssertValueTags(term);
	// NOTE: See $2021-01 @ %Documentation::Workflow.
#if NPL_Impl_NPLA1_Enable_Thunked
	SetupNextTerm(ctx, term);
#endif
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
	AssertNextTerm(ctx, term);
	// XXX: %Continuation is specialized enough without %trivial_swap.
	return A1::RelayCurrentNext(ctx, term, Continuation(
		// XXX: The function after decayed is specialized enough without
		//	%trivial_swap.
		static_cast<ReductionStatus(&)(TermNode&, ContextNode&)>(
		ReduceChildrenOrdered), ctx), trivial_swap,
		A1::NameTypedReducerHandler([&]{
		ReduceOrderedResult(term);
		return ReductionStatus::Regular;
	}, "sequence-return"));
#	endif
#else
	const auto res(ReduceChildrenOrdered(term, ctx));

	ReduceOrderedResult(term);
	return res;
#endif
}

ReductionStatus
ReduceTail(TermNode& term, ContextNode& ctx, TNIter i)
{
	term.erase(term.begin(), i);
	// NOTE: This is neutral to %NPL_Impl_NPLA1_Enable_Thunked.
	return ReduceOnce(term, ctx);
}

ReductionStatus
ReduceToReferenceList(TermNode& term, ContextNode& ctx, TermNode& tm)
{
	return ResolveTerm([&] YB_LAMBDA_ANNOTATE(
		(TermNode& nd, ResolvedTermReferencePtr p_ref), , flatten){
		if(IsList(nd))
		{
			// XXX: As %BindParameterObject.
			if(!p_ref || p_ref->IsMovable())
				// NOTE: This allows %nd owned by %term.
				term.MoveContainer(std::move(nd));
			else
			{
				const auto a(term.get_allocator());
				TermNode::Container con(a);
				const auto& r_env(p_ref ? p_ref->GetEnvironmentReference()
					: ctx.WeakenRecord());
				const auto o_tags(p_ref ? p_ref->GetTags() & (TermTags::Unique
					| TermTags::Nonmodifying) : TermTags::Temporary);

				// XXX: As %BindParameter.
				for(auto& o : nd)
					if(const auto p = TryAccessLeafAtom<TermReference>(o))
						EmplaceReference(con, o, *p, !p_ref);
					else if(p_ref)
						con.emplace_back(TermNode::Container(o.get_allocator()),
							ValueObject(std::allocator_arg, a,
							in_place_type<TermReference>,
							GetLValueTagsOf(o.Tags | o_tags), o, r_env));
					else
					{
						con.emplace_back(std::move(o.GetContainerRef()),
							std::move(o.Value));
						con.back().Tags |= TermTags::Temporary;
					}
				con.swap(term.GetContainerRef());
			}
			return ReductionStatus::Retained;
		}
		else
			ThrowListTypeErrorForNonList(nd, p_ref);
	}, tm);
}

ReductionStatus
ReduceToReferenceUList(TermNode& term, TermNode& tm)
{
	// XXX: As %ReduceToReferenceList.
	return ResolveTerm([&] YB_LAMBDA_ANNOTATE(
		(TermNode& nd, ResolvedTermReferencePtr p_ref), , flatten){
		if(IsList(nd))
		{
			if(p_ref)
			{
				const auto a(term.get_allocator());
				TermNode::Container con(a);
				const auto add_tags(p_ref->GetTags() | TermTags::Unique);

				for(auto& o : nd)
					if(const auto p = TryAccessLeafAtom<TermReference>(o))
						EmplaceReference(con, o, *p, {});
					else
						con.emplace_back(TermNode::Container(o.get_allocator()),
							std::allocator_arg, a, in_place_type<TermReference>,
							o.Tags | add_tags, o,
							p_ref->GetEnvironmentReference());
				con.swap(term.GetContainerRef());
			}
			else
				term.MoveContainer(std::move(nd));
			return ReductionStatus::Retained;
		}
		else
			ThrowListTypeErrorForNonList(nd, p_ref);
	}, tm);
}


void
SetupTraceDepth(ContextState& cs, const string& name)
{
	using namespace std::placeholders;
	auto p_env(cs.ShareRecord());

	ystdex::try_emplace(p_env->GetMapRef(), name, NoContainer,
		in_place_type<size_t>);
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	cs.Guard += std::bind([name](TermNode& term, ContextNode& ctx,
		shared_ptr<Environment>& p_e){
		if(const auto p = p_e->LookupName(name))
		{
			using ystdex::pvoid;
			auto& depth(Access<size_t>(*p));

			yunused(term), yunused(ctx);
			YTraceDe(YSLib::Informative, "Depth = %zu, context = %p, semantics"
				" = %p.", depth, pvoid(&ctx), pvoid(&term));
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
TransformNode(const TermNode& term)
{
	auto s(term.size());

	if(s == 0)
		return MapNPLALeafNode(term);

	auto i(term.begin());

	if(s == 1)
		return TransformNode(*i);

	const auto& name(ParseNPLATermString(*i));

	if(!name.empty())
		yunseq(++i, --s);
	if(s == 1)
	{
		auto&& nd(TransformNode(*i));

		if(nd.GetName().empty())
			return YSLib::AsNode(ValueNode::allocator_type(
				term.get_allocator()), name, std::move(nd.Value));
		return
			{ValueNode::Container({std::move(nd)}, term.get_allocator()), name};
	}

	ValueNode::Container node_con;

	std::for_each(i, term.end(), [&](const TermNode& tm){
		InsertChild(TransformNode(tm), node_con);
	});
	return
		{std::allocator_arg, term.get_allocator(), std::move(node_con), name};
}


void
ParseLeaf(TermNode& term, string_view id)
{
	YAssertNonnull(id.data());
	// NOTE: The lexeme shall not be empty, although the code literal '' can be
	//	converted to an empty symbol after the processing here.
	YAssert(!id.empty(), "Invalid leaf token found.");
	switch(CategorizeBasicLexeme(id))
	{
	case LexemeCategory::Code:
		id = DeliteralizeUnchecked(id);
		term.SetValue(in_place_type<TokenValue>, id, term.get_allocator());
		break;
	case LexemeCategory::Symbol:
		if(ParseSymbol(term, id))
			term.SetValue(in_place_type<TokenValue>, id, term.get_allocator());
		break;
		// XXX: Remained reducible?
	case LexemeCategory::Data:
		// XXX: This should be prevented being passed to second pass in
		//	%TermToNamePtr normally. This is guarded by normal form handling
		//	in the loop in %ContextNode::Rewrite with %ReduceOnce.
		term.SetValue(in_place_type<string>, Deliteralize(id),
			term.get_allocator());
		YB_ATTR_fallthrough;
	default:
		break;
		// XXX: Handle other categories of literal?
	}
}

void
ParseLeafWithSourceInformation(TermNode& term, string_view id,
	const shared_ptr<string>& name, const SourceLocation& src_loc)
{
	// NOTE: Most are same to %ParseLeaf, except for additional source
	//	information mixed into the values of %TokenValue.
	YAssertNonnull(id.data());
	// NOTE: Ditto.
	YAssert(!id.empty(), "Invalid leaf token found.");
	switch(CategorizeBasicLexeme(id))
	{
	case LexemeCategory::Code:
		id = DeliteralizeUnchecked(id);
		term.SetValue(any_ops::use_holder, in_place_type<SourcedHolder<
			TokenValue>>, name, src_loc, id, term.get_allocator());
		break;
	case LexemeCategory::Symbol:
		if(ParseSymbol(term, id))
			term.SetValue(any_ops::use_holder, in_place_type<SourcedHolder<
				TokenValue>>, name, src_loc, id, term.get_allocator());
		break;
	case LexemeCategory::Data:
		term.SetValue(any_ops::use_holder, in_place_type<SourcedHolder<string>>,
			name, src_loc, Deliteralize(id), term.get_allocator());
		YB_ATTR_fallthrough;
	default:
		break;
	}
}


ReductionStatus
FormContextHandler::CallHandler(TermNode& term, ContextNode& ctx) const
{
#if NPL_Impl_NPLA1_Enable_Thunked
	// XXX: The %std::reference_wrapper instance is specialized enough
	//	without %trivial_swap.
	return RelayCurrentOrDirect(ctx, std::ref(Handler), term);
#else
	return Handler(term, ctx);
#endif
}

ReductionStatus
FormContextHandler::CallN(size_t n, TermNode& term, ContextNode& ctx) const
{
	// NOTE: This implementes arguments evaluation in applicative order when
	//	%Wrapping is not zero.
#if NPL_Impl_NPLA1_Enable_Thunked
	// XXX: No %SetupNextTerm call is needed because it should have been called
	//	in %CombinerReturnThunk.
	AssertNextTerm(ctx, term);
	// NOTE: Optimize for cases with no argument.
	if(n == 0 || term.size() <= 1)
		// XXX: Assume the term has been setup by the caller.
		return CallHandler(term, ctx);
	// XXX: The empty type is specialized enough without %trivial_swap.
	return A1::RelayCurrentNext(ctx, term, [](TermNode& t, ContextNode& c){
		YAssert(!t.empty(), "Invalid term found.");
		ReduceChildrenOrderedAsyncUnchecked(std::next(t.begin()), t.end(), c);
		return ReductionStatus::Partial;
	}, trivial_swap, NameTypedReducerHandler([&, n](ContextNode& c){
		SetupNextTerm(c, term);
		return CallN(n - 1, term, c);
	}, "eval-combine-operator"));
#else
	// NOTE: This does not support PTC. However, the loop among the wrapping
	//	calls is almost PTC in reality.
	while(n-- != 0)
		ReduceArguments(term, ctx);
	return CallHandler(term, ctx);
#endif
}

void
FormContextHandler::CheckArguments(size_t n, TermNode& term)
{
	YAssert(!(IsList(term) && HasStickySubterm(term)),
		"Invalid representation found.");
	if(YB_UNLIKELY(n != 0 && !IsList(term)))
		throw ListReductionFailure("List expected in the applicative call.");
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


ReductionStatus
DefaultEvaluateLeaf(TermNode& term, string_view id)
{
	YAssertNonnull(id.data());
	YAssert(!id.empty(), "Invalid leaf token found.");
	// NOTE: Assume allocators are not needed.
	switch(id.front())
	{
	case '#':
		id.remove_prefix(1);
		if(!id.empty())
			switch(id.front())
			{
			case 't':
				if(id.size() == 1 || id.substr(1) == "rue")
				{
					term.Value = true;
					return ReductionStatus::Clean;
				}
				break;
			case 'f':
				if(id.size() == 1 || id.substr(1) == "alse")
				{
					term.Value = false;
					return ReductionStatus::Clean;
				}
				break;
			case 'i':
				if(id.substr(1) == "nert")
					return ReduceReturnUnspecified(term);
				else if(id.substr(1) == "gnore")
				{
					term.Value = ValueToken::Ignore;
					return ReductionStatus::Clean;
				}
				break;
			}
	default:
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		ReadDecimal(term.Value, id, id.begin());
		return ReductionStatus::Clean;
	case '-':
		if(YB_UNLIKELY(IsAllSignLexeme(id)))
			break;
		if(id.size() == 6 && id[4] == '.')
		{
			if(id[1] == 'i' && id[2] == 'n' && id[3] == 'f')
			{
				switch(id[5])
				{
				case '0':
					term.Value = -std::numeric_limits<double>::infinity();
					return ReductionStatus::Clean;
				case 'f':
					term.Value = -std::numeric_limits<float>::infinity();
					return ReductionStatus::Clean;
				case 't':
					term.Value = -std::numeric_limits<long double>::infinity();
					return ReductionStatus::Clean;
				}
			}
			else if(id[1] == 'n' && id[2] == 'a' && id[3] == 'n')
			{
				switch(id[5])
				{
				case '0':
					term.Value = -std::numeric_limits<double>::quiet_NaN();
					return ReductionStatus::Clean;
				case 'f':
					term.Value = -std::numeric_limits<float>::quiet_NaN();
					return ReductionStatus::Clean;
				case 't':
					term.Value = -std::numeric_limits<long double>::quiet_NaN();
					return ReductionStatus::Clean;
				}
			}
		}
		if(id.size() > 1)
			ReadDecimal(term.Value, id, std::next(id.begin()));
		else
			term.Value = 0;
		return ReductionStatus::Clean;
	case '+':
		if(YB_UNLIKELY(IsAllSignLexeme(id)))
			break;
		if(id.size() == 6 && id[4] == '.')
		{
			if(id[1] == 'i' && id[2] == 'n' && id[3] == 'f')
			{
				switch(id[5])
				{
				case '0':
					term.Value = std::numeric_limits<double>::infinity();
					return ReductionStatus::Clean;
				case 'f':
					term.Value = std::numeric_limits<float>::infinity();
					return ReductionStatus::Clean;
				case 't':
					term.Value = std::numeric_limits<long double>::infinity();
					return ReductionStatus::Clean;
				}
			}
			else if(id[1] == 'n' && id[2] == 'a' && id[3] == 'n')
			{
				switch(id[5])
				{
				case '0':
					term.Value = std::numeric_limits<double>::quiet_NaN();
					return ReductionStatus::Clean;
				case 'f':
					term.Value = std::numeric_limits<float>::quiet_NaN();
					return ReductionStatus::Clean;
				case 't':
					term.Value = std::numeric_limits<long double>::quiet_NaN();
					return ReductionStatus::Clean;
				}
			}
		}
		YB_ATTR_fallthrough;
	case '0':
		if(id.size() > 1)
			ReadDecimal(term.Value, id, std::next(id.begin()));
		else
			term.Value = 0;
		return ReductionStatus::Clean;
	}
	return ReductionStatus::Retrying;
}

ReductionStatus
EvaluateIdentifier(TermNode& term, const ContextNode& ctx, string_view id)
{
	// NOTE: The value of the expression is the lvalue referring to the bound
	//	object. The referenced object is kept alive through the evaluation,
	//	guaranteed by the context. Referencing the object (instead of copying
	//	the value of the object) is necessary since the ownership of objects
	//	which are not temporaries in evaluated terms are assumed to always stay
	//	in some environment rather than the AST, and copying the values can have
	//	unexpected side effects (due to copy constrcutors of the native values).
	//	It would be safe if not passed directly and without rebinding. Note the
	//	access of objects denoted by invalid references after rebinding would
	//	cause undefined behavior in the object language.
	auto pr(ResolveName(ctx, id));

	if(pr.first)
	{
		auto& bound(*pr.first);
		TermNode* p_rterm;
		auto& cs(ContextState::Access(ctx));

		if(!cs.TrySetTailOperatorName(term))
			cs.OperatorName.Clear();
		// NOTE: This is essentially similar to a successful call to
		//	%ResolveIdentifier plus a call to %EnsureLValueReference, except
		//	that the term tags are always not touched and the term container is
		//	also not touched when the result is not a reference.
		// NOTE: Every reference to the object in the environment is assumed
		//	aliased (even the environment object is unique), so no
		//	%TermTags::Unique is preserved in the result. In the
		//	object language, this implies that id-expressions are always
		//	lvalues, as same to C++.
		// XXX: Allocators are not used here on %TermReference to avoid G++ from
		//	folding code with other basic blocks with more inefficient
		//	implementations.
		if(const auto p = TryAccessLeafAtom<const TermReference>(bound))
		{
			p_rterm = &p->get();
			// NOTE: If the bound object is a term reference, referencing it
			//	implies reference collapsing. See also %ResolveIdentifier in
			//	NPLA.cpp for a simpler implemenation of the collapse.
			SetEvaluatedReference(term, bound, *p);
		}
		else
		{
			p_rterm = &bound;
			// NOTE: This makes a reference value referencing the bound object
			//	with proper environment and tags.
			SetEvaluatedValue(term, bound, pr.second);
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
	// NOTE: The symbols to be called with %EvaluateIdentifier are lexical
	//	results from the analysis result of %ParseLeaf. If necessary, there can
	//	be inserted some additional cleanup to remove empty tokens, returning
	//	%ReductionStatus::Partial. Separators should have been handled in
	//	appropriate preprocessing passes like %REPLContext::Preprocess.
	// XXX: Asynchronous reduction is currently not supported.
	return cs.EvaluateLiteral.empty() || CheckReducible(cs.EvaluateLiteral(term,
		cs, id)) ? EvaluateIdentifier(term, cs, id) : ReductionStatus::Retained;
}

ReductionStatus
ReduceCombined(TermNode& term, ContextNode& ctx)
{
	return IsCombiningTerm(term) ? ReduceCombinedBranch(term, ctx)
		: ReductionStatus::Regular;
}

ReductionStatus
ReduceCombinedBranch(TermNode& term, ContextNode& ctx)
{
	YAssert(IsCombiningTerm(term), "Invalid term found for combined term.");

	auto& fm(AccessFirstSubterm(term));
	const auto p_ref_fm(TryAccessLeafAtom<const TermReference>(fm));

	// NOTE: If this call returns normally, the combiner object implied by %fm
	//	is not owned by %term.
	// XXX: %SetupNextTerm is to be called in %CombinerReturnThunk.
	// NOTE: The tag is intended to be consumed by %VauHandler in
	//	NPLA1Forms.cpp. As the irregular representation has a handler of
	//	%RefContextHandler, the tag is consumed only by the underlying handler,
	//	so the irregular representation behaves same to glvalue here. The value
	//	of %term.Tags may indicate a temporary of %TermReference value of %fm.
	//	This shall be cleared if the object represented by %fm is not a prvalue.
	if(p_ref_fm)
	{
		ClearCombiningTags(term);
		// XXX: The following irregular term conversion is not necessary. It is
		//	even better to be avoid for easier handling of reference values.
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

			YAssert(ystdex::ref_eq<>()(NPL::Deref(Access<
				shared_ptr<TermNode>>(*fm.begin())), referenced),
				"Invalid subobject reference found.");
			// XXX: Explicit copy is necessary as in the implementation of
			//	%LiftOtherOrCopy.
			fm.CopyContent(referenced);
			YAssert(IsLeaf(fm), "Wrong result of irregular"
				" representation conversion found.");
		}
		else
#endif
		// NOTE: The combiner object is in an lvalue. It is not saved by %term.
		if(const auto p_handler
			= TryAccessLeafAtom<const ContextHandler>(p_ref_fm->get()))
			// NOTE: This is neutral to %NPL_Impl_NPLA1_Enable_Thunked.
			return CombinerReturnThunk(*p_handler, term, ctx);
	}
	else
		term.Tags |= TermTags::Temporary;
	// NOTE: The combiner object is in a prvalue. It will be moved away from
	//	the combining term (i.e. %term) by the call to %CombinerReturnThunk.
	//	The implementation varies on different configurations.
	// XXX: Converted terms (if used, see above) are also handled here as in
	//	prvalues.
	// NOTE: To allow being moved, %p_handler is not qualified by 'const'.
	if(const auto p_handler = TryAccessTerm<ContextHandler>(fm))
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
	return ResolveTerm(std::bind(ThrowCombiningFailure, std::ref(term),
		std::ref(ctx), std::placeholders::_1, std::placeholders::_2), fm);
}

ReductionStatus
ReduceCombinedReferent(TermNode& term, ContextNode& ctx, const TermNode& fm)
{
	YAssert(IsCombiningTerm(term), "Invalid term found for combined term.");
	// XXX: %SetupNextTerm is to be called in %CombinerReturnThunk.
	ClearCombiningTags(term);
	if(const auto p_handler = TryAccessLeafAtom<const ContextHandler>(fm))
		return CombinerReturnThunk(*p_handler, term, ctx);
	return ThrowCombiningFailure(term, ctx, fm, true);
}

ReductionStatus
ReduceLeafToken(TermNode& term, ContextNode& ctx)
{
	const auto res(ystdex::call_value_or([&](string_view id){
		TryRet(EvaluateLeafToken(term, ctx, id))
		catch(BadIdentifier& e)
		{
			if(const auto p_si = QuerySourceInformation(term.Value))
				e.Source = *p_si;
			throw;
		}
	// XXX: A term without token is ignored. This is actually same to
	//	%ReductionStatus::Regular in the current implementation.
	}, TermToNamePtr(term), ReductionStatus::Retained));

	return CheckReducible(res) ? ReduceOnce(term, ctx) : res;
}


ReductionStatus
RelayForEval(ContextNode& ctx, TermNode& term, EnvironmentGuard&& gd,
	bool no_lift, Continuation next)
{
	// XXX: For thunked code, %next shall be a continuation before being
	//	captured and it is not capturable here.
	// XXX: No %SetupNextTerm call is needed because it should have been called
	//	in the caller.
	return TailCall::RelayNextGuardedProbe(ctx, term, std::move(gd), !no_lift,
		std::move(next));
}

ReductionStatus
RelayForCall(ContextNode& ctx, TermNode& term, EnvironmentGuard&& gd,
	bool no_lift)
{
	// NOTE: With TCO, the operand temporary is been indicated by
	//	%TermTags::Temporary, no lifetime extension needs to be cared here.
	// XXX: No %SetupNextTerm call is needed because it should have been called
	//	in the caller.
	return TailCall::RelayNextGuardedProbe(ctx, term, std::move(gd), !no_lift,
		std::ref(ContextState::Access(ctx).ReduceOnce));
}


TokenValue
Ensigil(TokenValue s)
{
	if(!s.empty() && s.front() != '&')
	{
		if(s.front() != '%')
			return '&' + s;
		s.front() = '&';
	}
	return s;
}

void
CheckParameterTree(const TermNode& term)
{
	MakeParameterValueMatcher([&](const TokenValue&) ynothrow{})(term);
}

string
CheckEnvironmentFormal(const TermNode& term)
{
	TryRet(ResolveTerm([&](const TermNode& nd, bool has_ref) -> string{
		if(const auto p = TermToNamePtr(nd))
			return string(*p, term.get_allocator());
		else if(!IsIgnore(nd))
			ThrowFormalParameterTypeError(nd, has_ref);
		return string(term.get_allocator());
	}, term))
	CatchExpr(..., std::throw_with_nested(InvalidSyntax("Failed checking for"
		" environment formal parameter (expected a symbol or #ignore).")))
}

void
MatchParameter(const TermNode& t, TermNode& o, function<void(TermNode&, TNIter,
	const TokenValue&, TermTags, const EnvironmentReference&)>
	bind_trailing_seq, function<void(const TokenValue&, TermNode&, TermTags,
	const EnvironmentReference&)> bind_value, TermTags o_tags,
	const EnvironmentReference& r_env)
{
	MakeParameterMatcher<ParameterCheck>(std::move(bind_trailing_seq),
		std::move(bind_value))(t, o, o_tags, r_env);
}

// XXX: 'YB_FLATTEN' is a bit better for the quality of the generated code with
//	significantly worse performance of compilation (at least as costly as
//	%BindParameterWellFormed), so it is not used here if compilation time is
//	concerned. See %NPL_Impl_NPLA1_BindParameter_ExpandLevel for more details.
void
BindParameter(const shared_ptr<Environment>& p_env, const TermNode& t,
	TermNode& o)
{
	BindParameterImpl<ParameterCheck>(p_env, t, o);
}

// XXX: 'YB_FLATTEN' is significant for the quality of generated code.
#if NPL_Impl_NPLA1_BindParameter_ExpandLevel == 1
YB_FLATTEN
#endif
void
BindParameterWellFormed(const shared_ptr<Environment>& p_env, const TermNode& t,
	TermNode& o)
{
	BindParameterImpl<NoParameterCheck>(p_env, t, o);
}

void
BindSymbol(const shared_ptr<Environment>& p_env, const TokenValue& n,
	TermNode& o)
{
	// NOTE: As %BindSymbolImpl expecting the parameter tree as a single symbol
	//	term without trailing handling.
	auto& env(NPL::Deref(p_env));

	BindSymbolImpl(p_env, n, o, TermTags::Temporary, env);
}
#undef NPL_Impl_NPLA1_BindParameter_InlineAttr
#undef NPL_Impl_NPLA1_BindParameter_Inline


void
SetupDefaultInterpretation(ContextState& cs, EvaluationPasses passes)
{
	using Pass = EvaluationPasses::HandlerType;
	const auto a(cs.get_allocator());

	// XXX: Empty types and functions after decayed are specialized enough
	//	without %trivial_swap.
#if true
	// NOTE: This is an example of merged passes.
	passes += Pass(std::allocator_arg, a,
		[](TermNode& term, ContextNode& ctx) -> ReductionStatus{
		ReduceHeadEmptyList(term);
		if(IsBranchedList(term))
		{
			ContextState::Access(ctx).SetCombiningTermRef(term);
			// NOTE: Asynchronous reduction on the 1st term is needed for the
			//	continuation capture.
			// XXX: Without %NPL_Impl_NPLA1_Enable_InlineDirect, the
			//	asynchronous calls are actually more inefficient than separated
			//	calls.
			return ReduceSubsequent(AccessFirstSubterm(term), ctx,
				A1::NameTypedReducerHandler(std::bind(ReduceCombinedBranch,
				std::ref(term), std::placeholders::_1),
				"eval-combine-operands"));
		}
		return ReductionStatus::Clean;
	});
#else
#	if true
	// XXX: Optimization based on the synchronous call of %ReduceHeadEmptyList.
	passes += Pass(std::allocator_arg, a, [](TermNode& term, ContextNode& ctx){
		ReduceHeadEmptyList(term);
		if(IsBranchedList(term))
			ContextState::Access(ctx).SetCombiningTermRef(term);
		// NOTE: This is needed for the continuation capture.
		return ReduceFirst(term, ctx);
	});
#	else
	passes += Pass(std::allocator_arg, a, ReduceHeadEmptyList);
	passes += Pass(std::allocator_arg, a, [](TermNode& term, ContextNode& ctx){
		if(IsBranchedList(term))
			ContextState::Access(ctx).SetCombiningTermRef(term);
		return ReductionStatus::Neutral;
	});
	passes += Pass(std::allocator_arg, a, ReduceFirst);
#	endif
	// TODO: Insert more optional optimized lifted form evaluation passes.
	// NOTE: This implies the %RegularizeTerm call when necessary.
	// XXX: This should be the last of list pass for current TCO
	//	implementation, assumed by TCO action.
	passes += Pass(std::allocator_arg, a, ReduceCombined);
#endif
	cs.EvaluateList = std::move(passes);
	// NOTE: This implies the %RegularizeTerm call when necessary.
	cs.EvaluateLeaf = Pass(std::allocator_arg, a, ReduceLeafToken);
}

void
SetupTailContext(ContextNode& ctx, TermNode& term)
{
	YAssert(IsCombiningTerm(term), "Invalid term found for combined term.");
#if NPL_Impl_NPLA1_Enable_TCO
	yunused(EnsureTCOAction(ctx, term));
#else
	yunused(ctx);
#endif
}


bool
AddTypeNameTableEntry(const type_info& ti, string_view sv)
{
	YAssertNonnull(sv.data());

	const lock_guard<mutex> gd(NameTableMutex);

	return FetchNameTableRef<NPLA1Tag>().emplace(std::piecewise_construct,
		NPL::forward_as_tuple(ti), NPL::forward_as_tuple(sv)).second;
}

string_view
QueryContinuationName(const Reducer& act)
{
	if(const auto p_cont = act.target<Continuation>())
		return QueryTypeName(p_cont->Handler.target_type());
#if NPL_Impl_NPLA1_Enable_Thunked
	if(const auto p_act = act.target<PushedAction>())
	{
		if(p_act->HandlerRef.get() == ReduceCombined)
			return "eval-combine-operands";
		// XXX: These type names are not normally used.
		return QueryTypeName(p_act->HandlerRef.get().target_type());
	}
#endif
#if NPL_Impl_NPLA1_Enable_TCO
	if(act.target_type() == type_id<TCOAction>())
		return "eval-tail";
	if(act.target_type() == type_id<EvalSequence>())
		return "eval-sequence";
#endif
	return QueryTypeName(act.target_type());
}

observer_ptr<const SourceInformation>
QuerySourceInformation(const ValueObject& vo)
{
	const auto val(vo.Query());

	return ystdex::call_value_or([](const SourceInfoMetadata& r) ynothrow{
		return make_observer(&r.get());
	}, val.try_get_object_ptr<SourceInfoMetadata>());
}

#if NPL_Impl_NPLA1_Enable_TCO
YB_PURE
#else
YB_STATELESS
#endif
observer_ptr<const ValueObject>
QueryTailOperatorName(const Reducer& act) ynothrow
{
#if NPL_Impl_NPLA1_Enable_TCO
	if(const auto p_act = act.target<TCOAction>())
		if(p_act->OperatorName.type() == type_id<TokenValue>())
			return make_observer(&p_act->OperatorName);
#else
	yunused(act);
#endif
	return {};
}

string_view
QueryTypeName(const type_info& ti)
{
	const lock_guard<mutex> gd(NameTableMutex);
	const auto& tbl(FetchNameTableRef<NPLA1Tag>());
	const auto i(tbl.find(ti));

	if(i != tbl.cend())
		return i->second;
	return {};
}

void
TraceBacktrace(const ContextNode::ReducerSequence& backtrace,
	YSLib::Logger& trace) ynothrow
{
	if(!backtrace.empty())
	{
		YSLib::FilterExceptions([&]{
			using YSLib::Notice;

			trace.TraceFormat(Notice, "Backtrace:");
			for(const auto& act : backtrace)
			{
				const auto name(QueryContinuationName(act));
				const auto p(name.data() ? name.data() :
#if NDEBUG
					"?"
#else
					// XXX: This is enabled for debugging only because the name
					//	is not guaranteed steady.
					ystdex::call_value_or([](const Continuation& cont)
						-> const type_info&{
						return cont.Handler.target_type();
					}, act.target<Continuation>(), act.target_type()).name()
#endif
				);
				const auto p_opn_vo(QueryTailOperatorName(act));
				// XXX: No %NPL::TryAccessValue is needed, since %p_opn_vo comes
				//	from %TCOAction::OperatorName, which is not a term value.
				const auto p_opn_t(p_opn_vo ? p_opn_vo->AccessPtr<TokenValue>()
					: nullptr);

				if(const auto p_o = p_opn_t ? p_opn_t->data() : nullptr)
				{
					// XXX: This clause relies on the source information for
					//	meaningful output. Assume it is used.
#if true
					if(const auto p_si = QuerySourceInformation(*p_opn_vo))
						trace.TraceFormat(Notice, "#[continuation: %s (%s) @"
							" %s (line %zu, column %zu)]", p_o, p,
							p_si->first ? p_si->first->c_str() : "<unknown>",
							p_si->second.Line + 1, p_si->second.Column + 1);
					else
#endif
						trace.TraceFormat(Notice, "#[continuation: %s (%s)]",
							p_o, p);
				}
				else
					trace.TraceFormat(Notice, "#[continuation (%s)]", p);
			}
		}, "guard unwinding for backtrace");
	}
}


REPLContext::REPLContext(pmr::memory_resource& rsrc)
	: REPLContext([this](const GParsedValue<ByteParser>& str){
		TermNode term(Allocator);
		const auto id(YSLib::make_string_view(str));

		if(!id.empty())
			ParseLeaf(term, id);
		return term;
	}, [this](const GParsedValue<SourcedByteParser>& val){
		TermNode term(Allocator);
		const auto id(YSLib::make_string_view(val.second));

		if(!id.empty())
			ParseLeafWithSourceInformation(term, id, CurrentSource, val.first);
		return term;
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

std::ostream&
REPLContext::GetOutputStreamRef() const
{
	if(OutputStreamPtr)
		return *OutputStreamPtr;
	throw ystdex::unsupported("Unsupported output stream found.");
}

TermNode
REPLContext::DefaultLoad(REPLContext& context, ContextNode& ctx,
	string filename)
{
	return context.ReadFrom(*A1::OpenUnique(context, std::move(filename)), ctx);
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
REPLContext::ReadFrom(LoadOptionTag<>, std::streambuf& buf, ReaderState& rs,
	ContextNode& ctx) const
{
	using s_it_t = std::istreambuf_iterator<char>;
	Session sess(ctx.get_allocator());

	if(UseSourceLocation)
	{
		SourcedByteParser parse(sess.Lexer, sess.get_allocator());

		return Prepare(sess, sess.ProcessOne(rs, s_it_t(&buf), s_it_t(),
			ystdex::ref(parse)).first);
	}
	return Prepare(sess, sess.ProcessOne(rs, s_it_t(&buf), s_it_t()).first);
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
REPLContext::ReadFrom(LoadOptionTag<WithSourceLocation>, std::streambuf& buf,
	ReaderState& rs, ContextNode& ctx) const
{
	using s_it_t = std::istreambuf_iterator<char>;
	Session sess(ctx.get_allocator());
	SourcedByteParser parse(sess.Lexer, sess.get_allocator());

	return Prepare(sess,
		sess.ProcessOne(rs, s_it_t(&buf), s_it_t(), ystdex::ref(parse)).first);
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
REPLContext::ReadFrom(LoadOptionTag<NoSourceInformation>, std::streambuf& buf,
	ReaderState& rs, ContextNode& ctx) const
{
	using s_it_t = std::istreambuf_iterator<char>;
	Session sess(ctx.get_allocator());

	return Prepare(sess, sess.ProcessOne(rs, s_it_t(&buf), s_it_t()).first);
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

