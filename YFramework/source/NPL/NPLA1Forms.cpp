/*
	© 2016-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Forms.cpp
\ingroup NPL
\brief NPLA1 语法形式。
\version r29077
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2014-02-15 11:19:51 +0800
\par 修改时间:
	2022-12-09 08:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1Forms
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1Forms // for ResolvedTermReferencePtr, NPL::IsMovable,
//	ystdex::value_or, TryAccessReferencedTerm, IsBranch,
//	ThrowInsufficientTermsError, NPL::Deref, ReduceSubsequent,
//	A1::NameTypedReducerHandler, ReduceOnceLifted, std::next,
//	ReduceReturnUnspecified, RetainList, CheckVariadicArity, RemoveHead,
//	AccessFirstSubterm, ReduceOrdered, LiftOtherValue, std::bind, std::ref,
//	std::placeholders, std::declval, RetainN, ystdex::as_const, ValueObject,
//	ReferenceLeaf, IsAtom, ReferenceTerm, ystdex::ref_eq, CountPrefix, IsSticky,
//	RelaySwitched, trivial_swap, shared_ptr, ContextHandler, string_view,
//	YSLib::unordered_map, Environment, std::throw_with_nested, string, TokenValue,
//	any_ops::use_holder, in_place_type, YSLib::HolderFromPointer,
//	YSLib::allocate_shared, InvalidReference, TypeError, BindParameter, Retain,
//	MoveFirstSubterm, ResolveEnvironment, BindParameterWellFormed, ystdex::sfmt,
//	TermToStringWithReferenceMark, ResolveTerm, LiftOtherOrCopy,
//	ClearCombiningTags, SContext::Analyze, std::allocator_arg,
//	NPL::ResolveRegular, ystdex::make_transform, TryAccessLeafAtom,
//	TermReference, EnvironmentList, NPL::AllocateEnvironment, IsTypedRegular,
//	TryAccessLeaf, BindSymbol, ystdex::equality_comparable, IsList,
//	ystdex::fast_all_of, IsEmpty, CheckParameterTree, AssertValueTags,
//	NPL::AsTermNode, ystdex::exchange, TermTags, YSLib::Debug, NPLException,
//	YSLib::sfmt, AssignParent, ThrowListTypeErrorForNonList,
//	GuardFreshEnvironment, A1::MakeForm, ystdex::expand_proxy, AccessRegular,
//	GetLValueTagsOf, RegularizeTerm, IsPair, LiftPropagatedReference, LiftOther,
//	IsLeaf, LiftMovedOther, LiftTerm, ThrowValueCategoryError,
//	ThrowListTypeErrorForAtom, ThrowInvalidSyntaxError, ShareMoveTerm,
//	ExtractEnvironmentFormal, LiftTermOrCopy, EnsureValueTags, type_id,
//	ystdex::update_thunk, AssertCombiningTerm, LiftToReturn,
//	ystdex::prefix_eraser, IsTyped, IsBranchedList, EnvironmentGuard,
//	AssignWeakParent, lref, A1::AsForm, ystdex::bind1, IsNPLASymbol,
//	ystdex::isdigit, std::strchr, ystdex::call_value_or, ystdex::equal_to,
//	LiftCollapsed, std::distance, ReduceCombinedBranch, YSLib::usystem,
//	std::mem_fn;
#include "NPLA1Internals.h" // for A1::Internals API;
#include YFM_NPL_SContext // for Session;
#include <ystdex/scope_guard.hpp> // for ystdex::unique_guard,
//	ystdex::make_unique_guard, ystdex::dismiss, ystdex::make_guard;

namespace NPL
{

//! \since build 959
//@{
// NOTE: Optimization value for call optimization in %Forms::VauHandler, in
//	range [0, 3].
#ifndef NPL_Impl_NPLA1Forms_VauHandler_OptimizeLevel
#	if __OPTIMIZE_SIZE__
#		define NPL_Impl_NPLA1Forms_VauHandler_OptimizeLevel 0
#	else
#		define NPL_Impl_NPLA1Forms_VauHandler_OptimizeLevel 3
#	endif
#endif
//@}
//! \since build 945
//@{
#ifndef NDEBUG
#	define NPL_Impl_NPLA1Forms_TraceVauCall true
#else
#	define NPL_Impl_NPLA1Forms_TraceVauCall false
#endif
// NOTE: Optimization value for call expansions in %Forms::(LetAsterisk,
//	LetAsteriskRef), in range [0, 3].
#ifndef NPL_Impl_NPLA1Forms_LetAsterisk_ExpandLevel
#	if __OPTIMIZE_SIZE__
#		define NPL_Impl_NPLA1Forms_LetAsterisk_ExpandLevel 2
#	else
#		define NPL_Impl_NPLA1Forms_LetAsterisk_ExpandLevel 3
#	endif
#endif
// NOTE: Optimization value for call expansions in %Forms::(Let, LetRef,
//	LetWithEnvironment, LetWithEnvironmentRef, LetAsterisk, LetAsteriskRef,
//	LetRec, LetRecRef) for parent environment evaluation switches, in range
//	[0, 3].
#ifndef NPL_Impl_NPLA1Forms_LetParent_ExpandLevel
#	define NPL_Impl_NPLA1Forms_LetParent_ExpandLevel \
	NPL_Impl_NPLA1Forms_LetAsterisk_ExpandLevel
#endif
//@}

namespace A1
{

//! \since build 685
namespace
{

//! \since build 868
using Forms::CallRegularUnaryAs;
//! \since build 874
using Forms::CallResolvedUnary;
//! \since build 868
using Forms::CallUnary;


//! \since build 874
// XXX: This is more efficient, at least in code generation by x86_64-pc-linux
//	G++ 9.2 for %WrapN, as well as G++ 11.1 and 12.1.
template<typename _fCopy, typename _fMove>
YB_ATTR_nodiscard YB_FLATTEN auto
MakeValueOrMove(ResolvedTermReferencePtr p_ref, _fCopy cp, _fMove mv)
	-> decltype(NPL::IsMovable(p_ref) ? mv() : cp())
{
	return NPL::IsMovable(p_ref) ? mv() : cp();
}


//! \since build 876
YB_ATTR_nodiscard YB_PURE bool
ExtractBool(const TermNode& term)
{
	return ystdex::value_or(TryAccessReferencedTerm<bool>(term), true);
}

//! \since build 860
//@{
YB_ATTR_nodiscard TNIter
CondClauseCheck(TermNode& clause)
{
	if(IsBranch(clause))
		return clause.begin();
	ThrowInsufficientTermsError(clause, {});
}

YB_ATTR_nodiscard bool
CondTest(TermNode& clause, TNIter j)
{
	if(ExtractBool(NPL::Deref(j)))
	{
		clause.erase(j);
		return true;
	}
	return {};
}

#if NPL_Impl_NPLA1_Enable_Thunked
//! \since build 917
ReductionStatus
ReduceCond(TermNode& term, ContextNode& ctx, TNIter i)
{
	if(i != term.end())
	{
		auto& clause(NPL::Deref(i));
		auto j(CondClauseCheck(clause));

		// NOTE: This also supports TCO.
		return ReduceSubsequent(NPL::Deref(j), ctx,
			A1::NameTypedReducerHandler([&, i, j](ContextNode& c){
			if(CondTest(clause, j))
				return ReduceOnceLifted(term, c, clause);
			return ReduceCond(term, c, std::next(i));
		}, "eval-cond-list"));
	}
	return ReduceReturnUnspecified(term);
}
#endif

ReductionStatus
WhenUnless(TermNode& term, ContextNode& ctx, bool is_when)
{
	RetainList(term);
	CheckVariadicArity(term, 0);
	RemoveHead(term);

	auto& test(AccessFirstSubterm(term));

	return ReduceSubsequent(test, ctx,
		A1::NameTypedReducerHandler([&, is_when](ContextNode& c){
		if(ExtractBool(test) == is_when)
		{
			RemoveHead(term);
			return ReduceOrdered(term, c);
		}
		return ReduceReturnUnspecified(term);
	}, "conditional-eval-sequence"));
}

ReductionStatus
And2(TermNode& term, ContextNode& ctx, TNIter i)
{
	if(ExtractBool(NPL::Deref(i)))
	{
		term.Remove(i);
		return Forms::And(term, ctx);
	}
	term.Value = false;
	return ReductionStatus::Clean;
}

ReductionStatus
Or2(TermNode& term, ContextNode& ctx, TNIter i)
{
	auto& tm(NPL::Deref(i));

	if(!ExtractBool(tm))
	{
		term.Remove(i);
		return Forms::Or(term, ctx);
	}
	LiftOtherValue(term, tm);
	return ReductionStatus::Retained;
}
//@}

//! \since build 918
template<ReductionStatus(&_rAndOr)(TermNode&, ContextNode&, TNIter)>
ReductionStatus
AndOr(TermNode& term, ContextNode& ctx)
{
	RetainList(term);

	auto i(term.begin());

	if(++i != term.end())
		return std::next(i) == term.end() ? ReduceOnceLifted(term, ctx, *i)
			: ReduceSubsequent(*i, ctx,
			A1::NameTypedReducerHandler(std::bind(_rAndOr, std::ref(term),
			std::placeholders::_1, i), "eval-booleans"));
	term.Value = _rAndOr == And2;
	return ReductionStatus::Clean;
}

//! \since build 904
//@{
template<typename _fComp, typename _func>
auto
EqTerm(TermNode& term, _fComp f, _func g) -> decltype(f(
	std::declval<_func&>()(std::declval<const TermNode&>()),
	std::declval<_func&>()(std::declval<const TermNode&>())))
{
	RetainN(term, 2);

	auto i(term.begin());
	const auto& x(*++i);

	return f(g(x), g(ystdex::as_const(*++i)));
}

template<typename _fComp, typename _func>
void
EqTermRet(TermNode& term, _fComp f, _func g)
{
	using type = decltype(g(term));

	EqTerm(term, [&, f](const type& x, const type& y){
		term.Value = f(x, y);
	}, g);
}

template<typename _func>
void
EqTermValue(TermNode& term, _func f)
{
	EqTermRet(term, f, [](const TermNode& x) -> const ValueObject&{
		return ReferenceLeaf(x).Value;
	});
}

template<typename _func>
void
EqTermReference(TermNode& term, _func f)
{
	EqTermRet(term,
		[f] YB_LAMBDA_ANNOTATE((const TermNode& x, const TermNode& y), , pure){
		return IsAtom(x) && IsAtom(y) ? f(x.Value, y.Value)
			: ystdex::ref_eq<>()(x, y);
	}, static_cast<const TermNode&(&)(const TermNode&)>(ReferenceTerm));
}

// XXX: Sizes are not compared here, as different count of subterms can consist
//	to equal representations when considering irregular representations of the
//	last element in the pairs.
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, TermUnequal, const TermNode& x, const TermNode& y)
	ImplRet(CountPrefix(x) != CountPrefix(y) || x.Value != y.Value)

#if NPL_Impl_NPLA1_Enable_Thunked
ReductionStatus
EqualSubterm(bool& r, ContextNode& ctx, bool orig, TNCIter first1,
	TNCIter first2, TNCIter last1)
{
	if(first1 != last1)
	{
		YAssert(!IsSticky(NPL::Deref(first1).Tags),
			"Invalid representation found."),
		YAssert(!IsSticky(NPL::Deref(first2).Tags),
			"Invalid representation found.");

		auto& x(ReferenceTerm(NPL::Deref(first1)));
		auto& y(ReferenceTerm(NPL::Deref(first2)));

		if(TermUnequal(x, y))
		{
			r = {};
			return ReductionStatus::Clean;
		}
		yunseq(++first1, ++first2);
		// XXX: The continuations in the middle are not required to be
		//	preserved.
		RelaySwitched(ctx, trivial_swap,
			A1::NameTypedReducerHandler([&, orig, first1, first2, last1]{
			// XXX: This is not effective if the result is known to be false.
			return r ? EqualSubterm(r, ctx, orig, first1, first2, last1)
				: ReductionStatus::Clean;
		}, "equal-siblings"));
		return RelaySwitched(ctx, A1::NameTypedReducerHandler([&]{
			return
				EqualSubterm(r, ctx, {}, x.begin(), y.begin(), x.end());
		}, "equal-subterm"));
	}
	return orig ? ReductionStatus::Clean : ReductionStatus::Partial;
}
#else
//! \since build 908
YB_ATTR_nodiscard YB_PURE bool
EqualSubterm(TNCIter first1, TNCIter first2, TNCIter last1)
{
	if(first1 != last1)
	{
		YAssert(!IsSticky(NPL::Deref(first1).Tags),
			"Invalid representation found."),
		YAssert(!IsSticky(NPL::Deref(first2).Tags),
			"Invalid representation found.");

		auto& x(ReferenceTerm(NPL::Deref(first1)));
		auto& y(ReferenceTerm(NPL::Deref(first2)));

		if(TermUnequal(x, y))
			return {};
		yunseq(++first1, ++first2);
		return EqualSubterm(x.begin(), y.begin(), x.end())
			&& EqualSubterm(first1, first2, last1);
	}
	return true;
}
#endif
//@}

//! \since build 961
YB_ATTR_nodiscard YB_PURE BindingMap&
FetchDefineMapRef(const shared_ptr<Environment>& p_env)
{
	TryRet(NPL::Deref(p_env).GetMapCheckedRef())
	CatchExpr(..., std::throw_with_nested(
		TypeError("Cannot define variables in a frozen environment.")))
}


// NOTE: This type is a thunk, but not used as a thunk value stored in
//	%TermNode::ValueObject, hence not an NPLA1 thunk value type.
// NOTE: See $2019-03 @ %Documentation::Workflow.
//! \since build 782
class RecursiveThunk final
{
private:
	//! \since build 784
	using shared_ptr_t = shared_ptr<ContextHandler>;
	//! \since build 784
	YSLib::unordered_map<string, shared_ptr_t> store;
public:
#if NPL_Impl_NPLA1_Enable_Thunked
	//! \since build 961
	TermNode Saved;
#endif

	//! \since build 961
#if NPL_Impl_NPLA1_Enable_Thunked
	RecursiveThunk(BindingMap& m, TermNode& formals)
		: store(formals.get_allocator()), Saved(std::move(formals))
#else
	RecursiveThunk(BindingMap& m, const TermNode& t)
		: store(t.get_allocator())
#endif
	{
#if NPL_Impl_NPLA1_Enable_Thunked
		const auto& t(Saved);
#endif
		const auto a(t.get_allocator());

		// XXX: This is served as the addtional static environment.
		MakeParameterValueMatcher(a, [&](const TokenValue& n){
			string_view id(n);

			ExtractSigil(id);

			string k(id, store.get_allocator());

			// NOTE: The symbol can be bound more than once. Only one
			//	instance is supported.
			if(store.find(k) == store.cend())
				// NOTE: This binds value to a local thunk value. The
				//	bound symbol can then be rebound to an ordinary
				//	(non-sharing object.
				Environment::Bind(m, k, TermNode(TermNode::Container(a),
					ValueObject(any_ops::use_holder, in_place_type<
					YSLib::HolderFromPointer<shared_ptr_t>>,
					store[k] = YSLib::allocate_shared<ContextHandler>(a,
					ThrowInvalidCyclicReference))));
		})(t);
	}
	//! \since build 841
	DefDeMoveCtor(RecursiveThunk)

	//! \since build 841
	DefDeMoveAssignment(RecursiveThunk)

private:
	//! \since build 780
	YB_NORETURN static ReductionStatus
	ThrowInvalidCyclicReference(TermNode&, ContextNode&)
	{
		throw InvalidReference("Invalid cyclic reference found.");
	}
};


// NOTE: The following checks are essentially type checks to the environment
//	object from the object language program input.
//! \since build 911
YB_ATTR_nodiscard inline PDefH(const shared_ptr<Environment>&,
	FetchValidEnvironment, const shared_ptr<Environment>& p_env)
	ImplRet(Environment::EnsureValid(p_env), p_env)
//! \since build 911
YB_ATTR_nodiscard inline PDefH(shared_ptr<Environment>&&,
	FetchValidEnvironment, shared_ptr<Environment>&& p_env)
	ImplRet(Environment::EnsureValid(p_env), std::move(p_env))

//! \since build 894
YB_ATTR_nodiscard inline PDefH(const shared_ptr<Environment>&,
	FetchDefineOrSetEnvironment, ContextNode& ctx) ynothrow
	ImplRet(ctx.GetRecordPtr())
//! \since build 894
YB_ATTR_nodiscard inline PDefH(const shared_ptr<Environment>&,
	FetchDefineOrSetEnvironment, ContextNode&,
	const shared_ptr<Environment>& p_env)
	ImplRet(FetchValidEnvironment(p_env))

//! \since build 904
YB_NORETURN inline void
ThrowInsufficientTermsErrorFor(const TermNode& term, InvalidSyntax&& e)
{
	TryExpr(ThrowInsufficientTermsError(term, {}))
	CatchExpr(..., std::throw_with_nested(std::move(e)))
}

//! \since build 919
void
CheckBindParameter(const shared_ptr<Environment>& p_env, const TermNode& t,
	TermNode& o)
{
	BindParameter(FetchDefineMapRef(p_env), t, o, p_env);
}

//! \since build 868
//@{
template<typename _func>
auto
DoDefineSet(TermNode& term, size_t n, _func f) -> decltype(f())
{
	Retain(term);
	if(term.size() >= n)
	{
		RemoveHead(term);
		ClearCombiningTags(term);
		return f();
	}
	// NOTE: The exception handling is not enabled for the evaluations in the
	//	body.
	ThrowInsufficientTermsErrorFor(term,
		InvalidSyntax("Invalid syntax found in definition."));
}

//! \since build 921
YB_ATTR_nodiscard inline TermNode
SplitFirstSubterm(TermNode& term)
{
	auto tm(MoveFirstSubterm(term));

	RemoveHead(term);
	return tm;
}

// XXX: %YB_FLATTEN is better on %DoDefine and %DoSet with previous
//	implementation this translation unit and old versions of G++ (e.g. G++ 9.1)
//	for better performance with significant differences. It is actually bit
//	less efficient with G++ 10.2.

//! \since build 921
template<typename _func>
// XXX: It also cannot used with G++ 12.1.0, to avoid ICE in
//	'gimple_duplicate_bb' at 'tree-cfg.c:6420'.
#if YB_IMPL_GNUCPP > 0 && YB_IMPL_GNUCPP < 100000
YB_FLATTEN
#endif
auto
DoDefine(TermNode& term, _func f) -> decltype(f(std::declval<TermNode>()))
{
	return DoDefineSet(term, 2, [&]{
		return f(SplitFirstSubterm(term));
	});
}

template<typename _func>
#if !(YB_IMPL_GNUCPP >= 110000 && YB_IMPL_GNUCPP < 120000)
YB_FLATTEN
#endif
ReductionStatus
DoSet(TermNode& term, ContextNode& ctx, _func f)
{
	return DoDefineSet(term, 3, [&]{
		auto& tm(*term.begin());

		return ReduceSubsequent(tm, ctx, A1::NameTypedReducerHandler([&, f]{
			auto p_env(ResolveEnvironment(tm).first);

			RemoveHead(term);
			// NOTE: It is necessary to save %p_env here. The optional host
			//	value check is in the call to %FetchDefineOrSetEnvironment.
			return f(SplitFirstSubterm(term), std::move(p_env));
		}, "set-eval-obj"));
	});
}


template<bool = false>
struct DoDefineOrSet final
{
	//! \since build 894
	template<typename... _tParams>
	static ReductionStatus
	Call(TermNode& term, ContextNode& ctx, shared_ptr<Environment> p_env,
		TermNode& formals, _tParams&&... args)
	{
#if NPL_Impl_NPLA1_Enable_Thunked
		// XXX: Terms shall be moved and saved into the actions.
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		return ReduceSubsequentPinned(term, ctx,
			A1::NameTypedReducerHandler(std::bind([&](const TermNode& saved,
			const shared_ptr<Environment>& p_e, const _tParams&...){
			CheckBindParameter(p_e, saved, term);
			return ReduceReturnUnspecified(term);
		}, std::move(formals), std::move(p_env), std::move(args)...),
			"match-ptree"));
#else
		yunseq(0, args...);
		// NOTE: This does not support PTC.
		ReduceOnce(term, ctx);
		CheckBindParameter(p_env, formals, term);
		return ReduceReturnUnspecified(term);
#endif
	}
};

template<>
struct DoDefineOrSet<true> final
{
	//! \since build 894
	template<typename... _tParams>
	static ReductionStatus
	Call(TermNode& term, ContextNode& ctx, shared_ptr<Environment> p_env,
		TermNode& formals, _tParams&&... args)
	{
		auto& m(FetchDefineMapRef(p_env));
#if NPL_Impl_NPLA1_Enable_Thunked

		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		return ReduceSubsequentPinned(term, ctx, A1::NameTypedReducerHandler(
			std::bind([&](const shared_ptr<Environment>& p_e,
			const RecursiveThunk& gd, const _tParams&...){
			return ReduceCallBind(m, p_e, gd.Saved, term);
			// XXX: Terms shall be moved and saved into the actions.
		}, std::move(p_env), RecursiveThunk(m, formals),
			std::move(args)...), "match-ptree-recursive"));
#else
		// NOTE: This does not support PTC.
		RecursiveThunk gd(m, formals);

		yunseq(0, args...);
		ReduceOnce(term, ctx);
		return ReduceCallBind(m, p_env, formals, term);
#endif
	}

private:
	//! \since build 961
	YB_ATTR_always_inline static ReductionStatus
	ReduceCallBind(BindingMap& m, const shared_ptr<Environment>& p_env,
		const TermNode& formals, TermNode& term)
	{
		// NOTE: The parameter tree shall have been checked in the
		//	initialization of %RecursiveThunk.
		// XXX: Using explicit anchor pointer is more efficient.
		BindParameterWellFormed(m, formals, term, EnvironmentReference(p_env,
			NPL::Deref(p_env).GetAnchorPtr()));
		// NOTE: This support PTC only when the thunk cleanup is not existed at
		//	the tail context.
		return ReduceReturnUnspecified(term);
	}
};


template<bool _bRecur = false>
struct DefineOrSetDispatcher final
{
	TermNode& TermRef;
	ContextNode& Context;

	//! \since build 921
	template<typename... _tParams>
	inline ReductionStatus
	operator()(TermNode&& formals, _tParams&&... args) const
	{
		return DoDefineOrSet<_bRecur>::Call(TermRef, Context,
			FetchDefineOrSetEnvironment(Context, args...), formals,
			yforward(args)...);
	}
};
//@}


//! \since build 909
YB_NORETURN void
ThrowInvalidEnvironmentType(const TermNode& term, bool has_ref)
{
	throw TypeError(ystdex::sfmt("Invalid environment formed from object '%s'"
		" found.", TermToStringWithReferenceMark(term, has_ref).c_str()));
}


/*!
\param no_lift 指定是否避免保证规约后提升结果。
\since build 835
*/
//@{
ReductionStatus
EvalImplUnchecked(TermNode& term, ContextNode& ctx, bool no_lift)
{
	AssertNextTerm(ctx, term);

	const auto i(std::next(term.begin()));
	auto p_env(ResolveEnvironment(NPL::Deref(std::next(i))).first);

	ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		LiftOtherOrCopy(term, nd, NPL::IsMovable(p_ref));
	}, NPL::Deref(i));
	// XXX: This implies %EnsureValueTags in the implementation.
	ClearCombiningTags(term);
	// NOTE: On %NPL_Impl_NPLA1_Enable_TCO, this assumes %term is same to the
	//	current term in %TCOAction, which is initialized by %CombinerReturnThunk
	//	in NPLA1.cpp.
	// XXX: The mandated host value check is in the call to
	//	%ContextNode::SwitchEnvironment.
	// XXX: %TailCall::RelayNextGuardedProbe is a bit inefficient, and missing
	//	of inlining of the call to %RelayForCall is significantly more
	//	inefficient.
	return [&] YB_LAMBDA_ANNOTATE(() , , flatten){
		return RelayForCall(ctx, term, EnvironmentGuard(ctx,
			ctx.SwitchEnvironment(std::move(p_env))), no_lift);
	}();
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
	Session sess(ctx.get_allocator());

	SContext::Analyze(std::allocator_arg, ctx.get_allocator(), sess,
		sess.Process(NPL::ResolveRegular<const string>(expr)))
		.SwapContent(expr);
	return EvalImplUnchecked(term, ctx, no_lift);
}

//! \since build 923
ReductionStatus
RemoteEvalImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	RetainN(term, 2);
	return ReduceSubsequent(term.GetContainerRef().back(), ctx,
		A1::NameTypedReducerHandler([&, no_lift]{
#if NPL_Impl_NPLA1_Enable_Thunked
		SetupNextTerm(ctx, term);
#endif
		return EvalImplUnchecked(term, ctx, no_lift);
	}, "eval-remote-eval-env"));
}
//@}


//! \since build 909
YB_ATTR_nodiscard ValueObject
MakeEnvironmentParent(TNIter first, TNIter last,
	const TermNode::allocator_type& a, bool nonmodifying)
{
	const auto tr([&](TNIter iter){
		return ystdex::make_transform(iter, [&](TNIter i) -> ValueObject{
			// XXX: Like %LiftToReturn, but for %Value only.
			if(const auto p
				= TryAccessLeafAtom<const TermReference>(NPL::Deref(i)))
			{
				if(nonmodifying || !p->IsMovable())
					return p->get().Value;
				return std::move(p->get().Value);
			}
			if(nonmodifying)
				return i->Value;
			return std::move(i->Value);
		});
	});
#if true
	// XXX: This is slightly more efficient.
	return ValueObject(EnvironmentList(tr(first), tr(last), a));
#elif true
	return ValueObject(in_place_type<EnvironmentList>, tr(first), tr(last), a);
#else
	return ValueObject(std::allocator_arg, a,
		in_place_type<EnvironmentList>, tr(first), tr(last), a);
#endif
}

//! \since build 918
YB_ATTR_nodiscard shared_ptr<Environment>
CreateEnvironment(TermNode& term)
{
	const auto a(term.get_allocator());

	// NOTE: The parent check is implied in the constructor of %Environment.
	return !term.empty() ? NPL::AllocateEnvironment(a,
		MakeEnvironmentParent(term.begin(), term.end(), a, {}))
		: NPL::AllocateEnvironment(a);
}


//! \since build 959
//@{
#if NPL_Impl_NPLA1Forms_VauHandler_OptimizeLevel >= 2
YB_ATTR_nodiscard YB_PURE bool
IsSymbolFormal(const TermNode& term) ynothrow
{
	return IsTypedRegular<TokenValue>(term);
}

YB_ATTR_nodiscard YB_PURE bool
IsNonTrailingSymbol(const TermNode& term) ynothrowv
{
	YAssert(IsSymbolFormal(term), "Invalid term found.");
	if(const auto p = TryAccessLeaf<TokenValue>(term))
		return p->empty() || p->front() != '.';
	return {};
}

YB_ATTR_nodiscard YB_PURE bool
IsOptimizableFormalList(const TermNode& formals) ynothrowv
{
	YAssert(IsList(formals), "Invalid term found.");
	return ystdex::fast_all_of(ystdex::cbegin(formals), ystdex::cend(formals),
		IsSymbolFormal)
		&& (formals.empty() || IsNonTrailingSymbol(*formals.rbegin()));
}
#endif

//! \since build 918
//@{
YB_ATTR_nodiscard YB_PURE static ValueObject
MakeParent(ValueObject&& vo)
{
	Environment::CheckParent(vo);
	return std::move(vo);
}

// XXX: The check on %p_env for the parent should be checked before
//	(by %ResolveEnvironment, etc.).
YB_ATTR_nodiscard YB_PURE static ValueObject
MakeParentSingle(TermNode::allocator_type a,
	pair<shared_ptr<Environment>, bool> pr)
{
	auto& p_env(pr.first);

	// NOTE: The host value check is similar to %Environment::CheckParent since
	//	the parent is a single environment, except that it does not respect to
	//	'NPL_NPLA_CheckParentEnvironment'.
	Environment::EnsureValid(p_env);
	if(pr.second)
		return ValueObject(std::allocator_arg, a,
			in_place_type<shared_ptr<Environment>>, std::move(p_env));
	return ValueObject(std::allocator_arg, a,
		in_place_type<EnvironmentReference>, std::move(p_env));
}

YB_ATTR_nodiscard YB_PURE static ValueObject
MakeParentSingleNonOwning(TermNode::allocator_type a,
	const shared_ptr<Environment>& p_env)
{
	// NOTE: Ditto.
	Environment::EnsureValid(p_env);
	return ValueObject(std::allocator_arg, a,
		in_place_type<EnvironmentReference>, p_env);
}
//@}

void
VauBind(ContextNode& ctx, const TermNode& formals, TermNode& term)
{
	// NOTE: Forming beta-reducible terms using parameter binding, to substitute
	//	them as arguments for later closure reduction.
	// NOTE: The environment is assumed not frozen, so no need to use
	//	%FetchDefineMapRef.
	// NOTE: The parameter tree shall have been checked in the initialization of
	//	%VauHandler.
	BindParameterWellFormed(ctx.GetRecordPtr(), formals, term);
}

#if NPL_Impl_NPLA1Forms_VauHandler_OptimizeLevel == 1
void
VauBindEmpty(ContextNode&, const TermNode& formals, TermNode& term)
{
	YAssert(IsEmpty(formals), "Invalid formals found.");
	yunused(formals);
	// XXX: As %BindParameterWellFormed.
	AssertValueTags(term);
	// XXX: As %GParameterMatcher<NoParameterCheck,
	//	DefaultBinder>::ThrowIfNonempty in NPLA1.cpp.
	CheckForEmptyParameter(term);
}
#endif

#if NPL_Impl_NPLA1Forms_VauHandler_OptimizeLevel >= 2
void
VauBindList(ContextNode& ctx, const TermNode& formals, TermNode& term)
{
	YAssert(IsOptimizableFormalList(formals), "Invalid formals found.");
	// XXX: As %BindParameterWellFormed.
	AssertValueTags(term);
	ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		if(IsList(nd))
		{
			const auto n_p(formals.size()), n_o(nd.size());

			if(n_p == n_o)
			{
				const auto& p_env(ctx.GetRecordPtr());
				auto i(term.begin());

				for(const auto& tm_n : formals)
				{
					BindSymbol(p_env, tm_n.Value.GetObject<TokenValue>(), *i);
					++i;
				}
			}
			else
				// NOTE: As %GParameterMatcher<NoParameterCheck,
				//	DefaultBinder>::MatchPair in NPLA1.cpp.
				throw ArityMismatch(n_p, n_o);
		}
		else
			// NOTE: Ditto.
			ThrowListTypeErrorForNonList(nd, p_ref);
	}, term);
}
#endif

#if NPL_Impl_NPLA1Forms_VauHandler_OptimizeLevel >= 3
void
VauBindOne(ContextNode& ctx, const TermNode& formals, TermNode& term)
{
	YAssert(IsList(formals) && formals.size() == 1 && IsSymbolFormal(
		AccessFirstSubterm(formals)) && IsNonTrailingSymbol(
		AccessFirstSubterm(formals)), "Invalid formals found.");
	// XXX: As %BindParameterWellFormed.
	AssertValueTags(term);
	ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		if(IsList(nd))
		{
			const auto n_o(term.size());

			if(n_o == 1)
				BindSymbol(ctx.GetRecordPtr(), AccessFirstSubterm(formals).Value
					.GetObject<TokenValue>(), AccessFirstSubterm(nd));
			else
				// NOTE: Ditto.
				throw ArityMismatch(1, n_o);
		}
		else
			// NOTE: Ditto.
			ThrowListTypeErrorForNonList(nd, p_ref);
	}, term);
}
#endif

void
VauPrepareCall(ContextNode& ctx, TermNode& term, ValueObject& parent,
	TermNode& eval_struct, bool move)
{
	AssertNextTerm(ctx, term);
	// NOTE: Since now bindings do not rely on temporary objects are stored
	//	elsewhere (by using %TermTags::Temporary instead), setting %term
	//	immediately after the call to %BindParameter should be safe even with
	//	TCO.
	// NOTE: Because the only possible (non-administrative) owner of the
	//	evaluation structure visible in the object language is the combiner
	//	object which is not possible in the %term here, the evaluation structure
	//	shall share nothing to the combining term being reduced here (i.e.
	//	%term), so it is safe to use %TermNode::SetContent instead of
	//	%LiftOther. (Using %LiftOtherOrCopy is even more inefficient.) See also
	//	%ReduceCombinedBranch in NPLA1.cpp for details. Tags for combining terms
	//	are also overwritten since then.
	if(move)
	{
		// NOTE: The static environment is bound as the base of the local
		//	environment by setting the parent environment pointer.
		AssignParent(ctx, std::move(parent));
		// NOTE: The evaluation structure does not need to be saved to the
		//	continuation, since it would be used immediately in the call to
		//	%RelayForCall.
		term.SetContent(std::move(eval_struct));
#if NPL_Impl_NPLA1_Enable_TCO
		// XXX: Assume the last function being handled in %TCOAction is the
		//	the vau handler object. This would invalidate the vau handler (i.e.
		//	'*this' in the expected caller of this function).
		RefTCOAction(ctx).PopTopFrame();
#endif
	}
	else
	{
		AssignParent(ctx, parent);
		term.SetContent(eval_struct);
	}
}
//@}


// NOTE: %VauHandler is the base class of the vau handlers, as well as a
//	specialized version of %DynamicVauHandler without %eformal. Since
//	%DynamicVauHandler is not supporting '#ignore' the dynamic environment,
//	it is necessary regardless of the value of
//	%NPL_Impl_NPLA1Forms_VauHandler_OptimizeLevel. The equality is respected
//	that no objects of different target types are considered equal.
/*!
\warning 非虚析构。
\since build 767
*/
class VauHandler : private ystdex::equality_comparable<VauHandler>
{
protected:
	//! \since build 959
	//@{
	using GuardCall
		= void(const VauHandler&, EnvironmentGuard&, TermNode&, ContextNode&);
	using GuardDispatch = void(ContextNode&, const TermNode&, TermNode&);

private:
	template<GuardDispatch& _rDispatch>
	struct GuardStatic final
	{
		//! \since build 962
		static void
		Call(const VauHandler& vau, EnvironmentGuard&, TermNode& term,
			ContextNode& ctx)
		{
			// NOTE: See %DynamicVauHandler::GuardDynamic.
			_rDispatch(ctx, vau.GetFormalsRef(), term);
		}
	};
	//@}

	/*!
	\brief 形式参数对象。
	\invariant \c bool(p_formals) 。
	\since build 771
	*/
	shared_ptr<TermNode> p_formals;
	//! \brief 调用环境守卫例程。
	GuardCall& guard_call;
	/*!
	\brief 父环境。
	\note 包含引入抽象时的静态环境。
	\note 共享所有权用于检查循环引用。
	\since build 909
	*/
	mutable ValueObject parent;
	/*!
	\brief 求值结构。
	\since build 861
	*/
	mutable shared_ptr<TermNode> p_eval_struct;

public:
	//! \brief 返回时不提升项以允许返回引用。
	bool NoLifting = {};

	/*!
	\pre 间接断言：形式参数对象指针非空。
	\pre 假定第二参数可表示父环境。
	\pre 第二参数满足 Environment::CheckParent 检查不抛出异常。
	\pre 间接断言：第三参数的标签可表示一等对象的值。
	\pre 间接断言：第三参数指向非共享值且指定的项具有可表示一等对象的值。
	\sa MakeParent
	\sa MakeParentSingle
	\sa MakeParentSingleNonOwning
	\since build 959
	*/
	//@{
protected:
	// XXX: Keep parameters of reference types, as it is more efficient, at
	//	least in code generation by x86_64-pc-linux G++ 11.1.
	VauHandler(shared_ptr<TermNode>&& p_fm, ValueObject&& vo,
		shared_ptr<TermNode>&& p_es, bool nl, GuardCall& gd_call)
		: p_formals(std::move(p_fm)), guard_call(gd_call),
		parent(std::move(vo)), p_eval_struct(std::move(p_es)), NoLifting(nl)
	{
		YAssert(p_eval_struct.use_count() == 1,
			"Unexpected shared evaluation structure found.");
		AssertValueTags(NPL::Deref(p_eval_struct)); 
	}

public:
	VauHandler(shared_ptr<TermNode>&& p_fm, ValueObject&& vo,
		shared_ptr<TermNode>&& p_es, bool nl)
		: VauHandler(std::move(p_fm), std::move(vo), std::move(p_es), nl,
		InitCall<GuardStatic>(p_fm))
	{}
	//@}
	// NOTE: Only the implicitly defined copy constructor is used. The move
	//	constructor can be defined, but not necessary for as target object of
	//	%ContextHandler. The assignment operators are not used, and it would be
	//	deleted if defined as defaulted (due to the type of %call).

	//! \since build 959
	DefGetter(const ynothrow, TermNode&, FormalsRef, NPL::Deref(p_formals))

	//! \since build 824
	YB_ATTR_nodiscard friend bool
	operator==(const VauHandler& x, const VauHandler& y)
	{
		return x.p_formals == y.p_formals && x.parent == y.parent
			&& x.NoLifting == y.NoLifting;
	}

	//! \since build 772
	ReductionStatus
	operator()(TermNode& term, ContextNode& ctx) const
	{
		Retain(term);
		if(p_eval_struct)
		{
			bool move = {};

			if(bool(term.Tags & TermTags::Temporary))
				// TODO: Concurrency safety of accessing the referernce count?
				ClearCombiningTags(term), move = p_eval_struct.use_count() == 1;
			// NOTE: Since the 1st subterm is expected to be saved (e.g. by
			//	%ReduceCombined), it is safe to be removed directly.
			RemoveHead(term);

			// NOTE: This call does not have the parent argument as it would be
			//	assigned later in %operator() by a call to %AssignParent. This
			//	allows conditionally moving (instead of copying) the parent.
			//	Since %parent is initialized from a value in the constructor of
			//	%DynamicVauHandler which only accepts checked values, it needs
			//	no redundant check in the constructor of %Environment, hence the
			//	argument should also not be used for %GuardFreshEnvironment.
			auto gd(GuardFreshEnvironment(ctx));

			guard_call(*this, gd, term, ctx);

			// NOTE: Saved the no lift flag here to avoid branching in the
			//	following implementation.
			const bool no_lift(NoLifting);

#if NPL_Impl_NPLA1Forms_TraceVauCall
			ctx.Trace.Log(YSLib::Debug, [&]{
				return YSLib::sfmt<string>("Function called, with %ld shared"
					" term(s), %zu parameter(s).",
					p_eval_struct.use_count(), p_formals->size());
			});
#endif
			VauPrepareCall(ctx, term, parent, *p_eval_struct, move);
			// NOTE: The precondition is same to the last call in
			//	%EvalImplUnchecked.
			// XXX: The 'flatten' attribute here does not make it more
			//	efficient.
			return RelayForCall(ctx, term, std::move(gd), no_lift);
		}
		// XXX: The call has been performed on the handler so the body can be
		//	moved (when it is in a function prvalue), see the comments in above.
		throw NPLException("Invalid handler of call found.");
	}
	// TODO: Avoid TCO when the static environment has something with side
	//	effects on destruction, to prevent semantic changes.

protected:
	//! \since build 959
	template<template<GuardDispatch&> class _func>
	YB_ATTR_nodiscard YB_PURE static GuardCall&
	InitCall(shared_ptr<TermNode>& p_fm)
	{
		auto& formals(NPL::Deref(p_fm));

#	if NPL_Impl_NPLA1Forms_VauHandler_OptimizeLevel == 1
		if(IsEmpty(formals))
			return _func<VauBindEmpty>::Call;
#	elif NPL_Impl_NPLA1Forms_VauHandler_OptimizeLevel >= 2
		if(IsList(formals))
		{
#		if NPL_Impl_NPLA1Forms_VauHandler_OptimizeLevel >= 3
			if(formals.size() == 1)
			{
				auto& sub(AccessFirstSubterm(formals));

				if(IsSymbolFormal(sub) && IsNonTrailingSymbol(sub))
					return _func<VauBindOne>::Call;
			}
#		endif
			if(IsOptimizableFormalList(formals))
				return _func<VauBindList>::Call;
		}
#endif
		CheckParameterTree(formals);
		return _func<VauBind>::Call;
	}
};


//! \since build 959
class DynamicVauHandler final
	: private VauHandler, private ystdex::equality_comparable<DynamicVauHandler>
{
private:
	template<GuardDispatch& _rDispatch>
	struct GuardDynamic final
	{
		/*!
		\pre 第一参数的动态类型是 DynamicVauHandler 。
		\since build 962
		*/
		static void
		Call(const VauHandler& vau, EnvironmentGuard& gd, TermNode& term,
			ContextNode& ctx)
		{
			// NOTE: Evaluation in the local context: using the activation
			//	record frame with outer scope bindings.
			// XXX: Reuse of frame cannot be done here unless it can be proved
			//	all bindings would behave as in the old environment, which is
			//	too expensive for direct execution of programs with first-class
			//	environments.
			// NOTE: Bound the dynamic environment.
			// XXX: The fresh environment is always not frozen.
			NPL::AddValueTo(ctx.GetRecordRef().GetMapRef(), static_cast<const
				DynamicVauHandler&>(vau).eformal, std::allocator_arg,
				ctx.get_allocator(), EnvironmentReference(gd.func.SavedPtr));
			// NOTE: The dynamic environment is either out of TCO action or
			//	referenced by other environments already in TCO action, so there
			//	is no need to treat as root.
			// XXX: Referencing escaped variables (now only parameters need to
			//	be cared) form the context would cause undefined behavior (e.g.
			//	returning a reference to automatic object in the host language).
			//	The lifting of call result is enabled to prevent this, unless
			//	%NoLifting is %true. See also %BindParameter.
			_rDispatch(ctx, vau.GetFormalsRef(), term);
		}
	};

	/*!
	\brief 动态环境名称。
	\since build 769
	*/
	string eformal{};

public:
	/*!
	\pre 同 VauHandler 构造函数。
	\sa MakeParent
	\sa MakeParentSingle
	\sa MakeParentSingleNonOwning
	*/
	DynamicVauHandler(shared_ptr<TermNode>&& p_fm,
		ValueObject&& vo, shared_ptr<TermNode>&& p_es, bool nl, string&& ename)
		: VauHandler(std::move(p_fm), std::move(vo), std::move(p_es), nl,
		InitCall<GuardDynamic>(p_fm)), eformal(std::move(ename))
	{}

	YB_ATTR_nodiscard friend bool
	operator==(const DynamicVauHandler& x, const DynamicVauHandler& y)
	{
		return static_cast<const VauHandler&>(x)
			== static_cast<const VauHandler&>(y) && x.eformal == y.eformal;
	}

	using VauHandler::operator();
};


//! \since build 953
//@{
void
ConsSplice(TermNode& t, TermNode& nd)
{
	// XXX: No other cyclic reference check.
	YAssert(!ystdex::ref_eq<>()(t, nd), "Invalid self move found.");
	t.GetContainerRef().splice(t.begin(), nd.GetContainerRef(), nd.begin());
}

YB_ATTR_nodiscard inline TNIter
ConsHead(TermNode& term)
{
	RetainN(term, 2);
	RemoveHead(term);
	return term.begin();
}

YB_FLATTEN ReductionStatus
ConsTail(TermNode& term, TermNode& t)
{
	ConsSplice(t, term);
	LiftOtherValue(term, t);
	return ReductionStatus::Retained;
}

void
ConsRest(TermNode& t, TermNode& nd)
{
	ConsSplice(t, nd);
	t.GetContainerRef().swap(nd.GetContainerRef());
	nd.Value = std::move(t.Value);
	// NOTE: %AssertValueTags shall not be called here because %nd.Tags may have
	//	%TermTags::Temporary at the beginning at the call. This is used in
	//	%(SetRest, SetRestRef).
}
//@}


//! \since build 859
//@{
YB_NORETURN ReductionStatus
ThrowForUnwrappingFailure(const ContextHandler& h)
{
	throw TypeError(ystdex::sfmt("Unwrapping failed with type '%s'.",
		h.target_type().name()));
}

//! \since build 921
YB_NORETURN void
ThrowUnwrappingFailureOnOperative()
{
	throw TypeError("Unwrapping failed on an operative argument.");
}

//! \since build 915
ReductionStatus
WrapH(TermNode& term, FormContextHandler h)
{
	// XXX: Allocators are not used here on the %ContextHandler value for
	//	performance.
	term.Value = ContextHandler(std::allocator_arg, term.get_allocator(),
		std::move(h));
	return ReductionStatus::Clean;
}
//! \since build 928
ReductionStatus
WrapH(TermNode& term, ContextHandler h, size_t n)
{
	// XXX: Allocators are not used here on the %ContextHandler value for
	//	performance.
	term.Value = A1::MakeForm(term, std::move(h), n);
	return ReductionStatus::Clean;
}

//! \since build 943
ReductionStatus
WrapO(TermNode& term, ContextHandler& h, ResolvedTermReferencePtr p_ref)
{
	// XXX: Allocators are not used on %FormContextHandler for performance in
	//	most cases.
	return WrapH(term, MakeValueOrMove(p_ref, [&]{
		return h;
	}, [&]{
		return std::move(h);
	}), 1);
}

//! \since build 913
ReductionStatus
UnwrapResolved(TermNode& term, FormContextHandler& fch,
	ResolvedTermReferencePtr p_ref)
{
	if(!fch.IsOperative())
		return MakeValueOrMove(p_ref, [&]{
			return ReduceForCombinerRef(term, NPL::Deref(p_ref),
				fch.Handler, fch.GetWrappingCount() - 1);
		}, [&]{
			fch.Unwrap();
			return WrapH(term, std::move(fch));
		});
	ThrowUnwrappingFailureOnOperative();
}

template<typename _func, typename _func2>
ReductionStatus
DispatchContextHandler(ContextHandler& h, ResolvedTermReferencePtr p_ref,
	_func f, _func2 f2)
{
	if(const auto p = h.target<FormContextHandler>())
		return f(*p, p_ref);
	return ystdex::expand_proxy<ReductionStatus(ContextHandler&,
		const ResolvedTermReferencePtr&)>::call(f2, h, p_ref);
}

template<typename _func, typename _func2>
ReductionStatus
WrapUnwrapResolve(TermNode& term, _func f, _func2 f2)
{
	return NPL::ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		return DispatchContextHandler(
			AccessRegular<ContextHandler>(nd, p_ref), p_ref, f, f2);
	}, term);
}
//@}


//! \since build 876
#if false
template<typename... _tParams>
TermNode&
AddTrailingTemporary(TermNode& term, _tParams&&... args)
{
	return *term.emplace(yforward(args)...);
}
#endif

//! \since build 897
//@{
// NOTE: The bound term can be reused later because no referent can be
//	invalidated unless %mv writes %o.
template<typename _fMove>
void
BindMoveLocalObject(TermNode& o, _fMove mv)
{
	// NOTE: Simplified from 'sigil == '&' and 'can_modify && temp' branch in
	//	%BindParameterObject::operator(), except that %TermTags::Temporary is
	//	not set as %MarkTemporaryTerm in NPLA1.cpp.
	if(const auto p = TryAccessLeafAtom<TermReference>(o))
		// NOTE: Reference collapsed by move.
		mv(std::move(o.GetContainerRef()),
			TermReference(BindReferenceTags(*p), std::move(*p)));
	else
		mv(std::move(o.GetContainerRef()), std::move(o.Value));
}

void
BindMoveLocalObjectInPlace(TermNode& o)
{
	// NOTE: Simplified from 'sigil == '&' and 'can_modify && temp' branch in
	//	%BindParameterObject::operator(), except that %TermTags::Temporary is
	//	not set as %MarkTemporaryTerm in NPLA1.cpp.
	if(const auto p = TryAccessLeafAtom<TermReference>(o))
		// NOTE: Reference collapsed by move.
		p->SetTags(BindReferenceTags(*p));
}

void
BindMoveNextNLocalSubobjectInPlace(TNIter i, size_t n)
{
	while(n-- != 0)
		BindMoveLocalObjectInPlace(NPL::Deref(++i));
}

// NOTE: The bound term cannot be reused later because %o can be the referent.
//	Neither %o can be overwritten by %mv.
template<typename _fMove>
void
BindLocalReference(TermTags o_tags, TermNode& o, _fMove mv,
	const EnvironmentReference& r_env)
{
	// NOTE: Simplified from '&' and not 'can_modify && temp' branch in
	//	%BindParameterObject::operator().
	mv(TermNode::Container(o.GetContainer()), [&]() -> TermReference{
		if(const auto p = TryAccessLeafAtom<TermReference>(o))
			return TermReference(BindReferenceTags(*p), *p);
		return TermReference(GetLValueTagsOf(o.Tags | o_tags), o, r_env);
	}());
}

YB_ATTR_nodiscard YB_ATTR_always_inline inline TermNode
EvaluateToLValueReference(TermNode& term, const shared_ptr<Environment>& p_env)
{
	return NPL::AsTermNode(term.get_allocator(), EnsureLValueReference(
		// XXX: Using explicit anchor pointer is more efficient.
		TermReference(term.Tags, term, NPL::Nonnull(p_env),
		NPL::Deref(p_env).GetAnchorPtr())));
}
//@}

#if false
// NOTE: The bound term cannot be reused later because %term can be the
//	referent.
//! \since build 894
YB_ATTR_nodiscard TermNode
EvaluateBoundLValue(TermNode& term, const shared_ptr<Environment>& p_env)
{
	if(const auto p = TryAccessLeafAtom<TermReference>(term))
		return TermNode(std::allocator_arg, term.get_allocator(),
			term.GetContainer(), EnsureLValueReference(*p));
	return EvaluateToLValueReference(term, p_env);
}

// NOTE: The bound term cannot be reused later because %term can be the
//	referent.
//! \since build 897
YB_ATTR_nodiscard TermNode
EvaluateBoundLValueMoved(TermNode& term, const shared_ptr<Environment>& p_env)
{
	if(const auto p = TryAccessLeafAtom<TermReference>(term))
	{
		*p = EnsureLValueReference(std::move(*p));
		return std::move(term);
	}
	return EvaluateToLValueReference(term, p_env);
}
#endif

// NOTE: See %BindMoveLocalObjectInPlace and %EvaluateBoundLValue.
// NOTE: The returned term represents a term reference.
//! \since build 898
YB_ATTR_nodiscard TermNode
EvaluateLocalObject(TermNode& o, const shared_ptr<Environment>& p_env)
{
	// NOTE: Make unique reference as a bound temporary object.
	if(const auto p = TryAccessLeafAtom<TermReference>(o))
	{
		p->SetTags(BindReferenceTags(*p));
		return TermNode(std::allocator_arg, o.get_allocator(),
			o.GetContainer(), EnsureLValueReference(*p));
	}
	// NOTE: Different to %BindMoveLocalObjectInPlace, tags are set as
	//	%MarkTemporaryTerm in NPLA1.cpp.
	o.Tags |= TermTags::Temporary;
	return EvaluateToLValueReference(o, p_env);
}

#if false
// NOTE: See %BindMoveLocalObjectInPlace and %EvaluateBoundLValueMoved.
//! \since build 897
YB_ATTR_nodiscard TermNode
EvaluateLocalObjectMoved(TermNode& o, const shared_ptr<Environment>& p_env)
{
	// NOTE: Make unique reference as a bound temporary object.
	if(const auto p = TryAccessLeafAtom<TermReference>(o))
	{
		// NOTE: See also %EnsureLValueReference.
		*p = TermReference(BindReferenceTags(*p) & ~TermTags::Unique,
			std::move(*p));
		return std::move(o);
	}
	// NOTE: Different to %BindMoveLocalObjectInPlace, tags are set as
	//	%MarkTemporaryTerm in NPLA1.cpp.
	o.Tags |= TermTags::Temporary;
	return EvaluateToLValueReference(o, p_env);
}
#endif

//! \since build 913
void
ForwardToUnwrapped(TermNode& comb)
{
	using namespace std::placeholders;

	RegularizeTerm(comb, WrapUnwrapResolve(comb, std::bind(UnwrapResolved,
		std::ref(comb), _1, _2), ThrowForUnwrappingFailure));
}

// XXX: Preserving %con is better for performance, at least in code generation
//`by x86_64-pc-linux G++ 10.2.
//! \since build 913
YB_ATTR_nodiscard TermNode
EvaluateBoundUnwrappedLValueDispatch(TermNode::allocator_type a,
	const TermNode::Container& con, TermReference ref,
	TermNode& nd)
{
	auto& h(AccessRegular<ContextHandler>(nd, true));

	if(const auto p = h.target<FormContextHandler>())
	{
		auto& fch(*p);

		if(fch.GetWrappingCount() != 0)
		{
			TermNode term(std::allocator_arg, a, con);

			ReduceForCombinerRef(term, ref, fch.Handler,
				fch.GetWrappingCount() - 1);
			return term;
		}
		ThrowUnwrappingFailureOnOperative();
	}
	ThrowForUnwrappingFailure(h);
}

// NOTE: As %EvaluateBoundLValue and %ForwardToUnwrapped.
//! \since build 913
YB_ATTR_nodiscard TermNode
EvaluateBoundLValueUnwrapped(TermNode& term,
	const shared_ptr<Environment>& p_env)
{
	if(const auto p = TryAccessLeafAtom<TermReference>(term))
		return EvaluateBoundUnwrappedLValueDispatch(
			term.get_allocator(), term.GetContainer(), *p, p->get());
	return EvaluateBoundUnwrappedLValueDispatch(term.get_allocator(),
		// XXX: Using explicit anchor pointer is more efficient.
		{}, TermReference(term.Tags, term, NPL::Nonnull(p_env),
		NPL::Deref(p_env).GetAnchorPtr()), term);
}


//! \since build 874
template<typename _func>
ReductionStatus
FirstOrVal(TermNode& term, _func f)
{
	return CallResolvedUnary([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		if(IsPair(nd))
		{
			auto& tm(AccessFirstSubterm(nd));

			YAssert(!IsSticky(tm.Tags), "Invalid representation found.");
			return f(tm, p_ref);
		}
		ThrowInsufficientTermsError(nd, p_ref);
	}, term);
}

//! \since build 917
void
LiftCopyPropagate(TermNode& term, TermNode& tm, TermTags tags)
{
	// XXX: Similar to the implementation of %ReduceToReference in NPLA.cpp.
#if true
	// NOTE: Propagate tags if it is a term reference.
	if(const auto p = TryAccessLeafAtom<TermReference>(term))
		LiftPropagatedReference(term, tm, tags);
	else
		term.CopyContent(tm);
#else
	term.CopyContent(tm);
	// NOTE: Propagate tags if it is a term reference.
	if(const auto p = TryAccessLeafAtom<TermReference>(term))
		p->PropagateFrom(tags);
#endif
	// XXX: Term tags are currently not respected in prvalues.
}
//! \since build 915
inline PDefH(void, LiftCopyPropagate, TermNode& term, TermNode& tm,
	const TermReference& ref)
	ImplExpr(LiftCopyPropagate(term, tm, ref.GetTags()))

//! \since build 940
void
LiftOtherOrCopyPropagate(TermNode& term, TermNode& tm, bool move, TermTags tags)
{
	if(move)
		// XXX: Using %LiftOther (from %LiftOtherOrCopy) instead of
		//	%LiftTermOrCopy is safe, because the referent is not allowed to have
		//	cyclic reference to %term. And %LiftTermOrCopy is in that case is
		//	still inapproiate anyway because copy should be elided in a
		//	forwarding operation.
		LiftOther(term, tm);
	else
		// XXX: The call to %NPL::IsMovable has guarantees %p_ref is nonnull
		//	here.
		LiftCopyPropagate(term, tm, tags);
}

//! \since build 939
void
LiftOtherOrCopyPropagateTags(TermNode& term, TermNode& tm, TermTags tags)
{
	return LiftOtherOrCopyPropagate(term, tm, NPL::IsMovable(tags), tags);
}

//! \since build 914
ReductionStatus
ReduceToFirst(TermNode& term, TermNode& tm, ResolvedTermReferencePtr p_ref)
{
#if YB_IMPL_GNUCPP < 110000
	// XXX: This is verbose but perhaps more efficient with old versions of G++,
	//	 however, not x86_64-pc-linux G++ 11.1.
	const bool list_not_move(!NPL::IsMovable(p_ref));

	if(const auto p = TryAccessLeafAtom<const TermReference>(tm))
	{
		if(list_not_move)
			LiftPropagatedReference(term, tm, p_ref->GetTags());
		else
			LiftMovedOther(term, *p, p->IsMovable());
		// XXX: %RegularizeTerm would imply %ClearCombiningTags, which in turn
		//	implies %EnsureValueTags in the implementation.
		return ReductionStatus::Retained;
	}
	else if(list_not_move)
		return ReduceToReferenceAt(term, tm, p_ref);
	// XXX: Term tags are currently not respected in prvalues.
	LiftOtherValue(term, tm);
	return ReductionStatus::Retained;
#else
	// NOTE: Any optimized implemenations shall be equivalent to this.
	if(NPL::IsMovable(p_ref))
	{
		LiftOtherValue(term, tm);
		return ReduceForLiftedResult(term);
	}
	// XXX: The call to %ReduceToReference should be safe, since the parent list
	//	is guaranteed an lvalue if it is not movable.
	return ReduceToReference(term, tm, p_ref);
#endif
}

/*!
\pre 间接断言：第一和第二参数指定不相同的项。
\pre 第一参数不是第二参数的直接或间接子项。
\since build 940
*/
void
BranchFirstReferenced(TermNode& term, TermNode& nd,
	const EnvironmentReference& r_env, TermTags tags)
{
	// NOTE: The parameter %nd is expected to be a list object in the reference
	//	term formed by the last 2 parameters. See also the comment in
	//	%ListRangeExtract. There used to be a call to %BindReferenceTags on
	//	%tags, but now for the similar reason, it is unnecessary.
#if true
	auto& tm(AccessFirstSubterm(nd));
	const bool move(NPL::IsMovable(tags));

	if(const auto p = TryAccessLeafAtom<const TermReference>(tm))
	{
		if(move)
		{
			if(!p->IsReferencedLValue())
				LiftMovedOther(term, *p, p->IsMovable());
			else
				LiftOtherValue(term, tm);
		}
		else
			// XXX: The term %term is not %nd or any of its descendants
			//	(including %x). However, %x can still be a descendant of %term,
			//	so %TermNode::SetContainer cannot be used on %x directly.
			LiftPropagatedReference(term, tm, tags);
	}
	else if(move)
		LiftOtherValue(term, tm);
	else
	{
		// XXX: Do not bother moving %oref because the branching is usually
		//	more costly than copying a reference, and the term range is likely
		//	not unique due to 2-pass iterations (e.g. it may be also used in
		//	a interleaved call to %BranchRestFwdReferenced with unspecified
		//	order to this call).
		term.SetValue(term.get_allocator(), in_place_type<TermReference>,
			PropagateTo(tm.Tags, tags), tm, r_env);
		// NOTE: No %TermNode::ClearContainer is here because %term is expected
		//	newly created.
		YAssert(IsLeaf(term), "Invalid term found");
	}
#else
	// NOTE: Any optimized implemenations shall be equivalent to this.
	TermReference ref(tags, nd, r_env);

	RegularizeTerm(term, ReduceToFirst(term, AccessFirstSubterm(nd), &ref));
#endif
}

//! \since build 951
//@{
ReductionStatus
CheckReference(TermNode& term, void(&f)(TermNode&, bool))
{
	return CallResolvedUnary([&](TermNode& nd, bool has_ref){
		f(nd, has_ref);
		// XXX: Similar to %DoIdFunc in %NPL.Dependency.
		LiftTerm(term, *std::next(term.begin()));
		return ReductionStatus::Regular;
	}, term);
}

// XXX: Following instances of %YB_FLATTEN keep the implementations same
//	efficient to those previously not using %ThrowListTypeErrorForNonList in
//	%CheckResolvedListReference.

//! \since build 859
YB_FLATTEN void
CheckResolvedListReference(TermNode& nd, bool has_ref)
{
	if(has_ref)
	{
		if(YB_UNLIKELY(!IsList(nd)))
			ThrowListTypeErrorForNonList(nd, true);
	}
	else
		ThrowValueCategoryError(nd);
}

YB_FLATTEN void
CheckResolvedPairReference(TermNode& nd, bool has_ref)
{
	if(has_ref)
	{
		if(YB_UNLIKELY(IsAtom(nd)))
			ThrowListTypeErrorForAtom(nd, true);
	}
	else
		ThrowValueCategoryError(nd);
}
//@}

//! \since build 874
template<typename _func>
ReductionStatus
FirstAtRef(TermNode& term, _func f)
{
	return CallResolvedUnary([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		CheckResolvedPairReference(nd, p_ref);
		// XXX: The call to %AccessFirstSubterm should be safe, since the parent
		//	list is guaranteed an lvalue by %CheckResolvedPairReference.
		return f(term, AccessFirstSubterm(nd), p_ref);
	}, term);
}

//! \since build 951
//@{
void
CopyRestContainer(TermNode::Container& tcon, TNIter first, TNIter last)
{
	const auto i(tcon.end());

	while(first != last)
		tcon.insert(i, *first++);
}

void
CopyRest(TermNode& t, TNIter first, TNIter last, const ValueObject& vo)
{
	CopyRestContainer(t.GetContainerRef(), first, last);
	t.Value = vo;
}

//! \since build 939
void
BranchRestFwdReferenced(TermNode& term, TermNode& nd, bool move)
{
	AssertBranch(nd);

	auto first(nd.begin());
	const auto last(nd.end());

	YAssert(!IsSticky(first->Tags), "Invalid representation found.");
	++first;
	// XXX: The transaction is not for providing more enhanced exception safety,
	//	but being more consistent to derived implementations.
	if(move)
	{
		TermNode::Container tcon(term.get_allocator());

		tcon.splice(tcon.end(), nd.GetContainerRef(), first, last);
		term.Value = std::move(nd.Value);
		tcon.swap(term.GetContainerRef());
	}
	else
	{
		TermNode t(term.get_allocator());

		CopyRest(t, first, last, nd.Value);
		swap(t, term);
	}
}

template<typename _func>
ReductionStatus
RestOrVal(TermNode& term, _func f)
{
	return CallResolvedUnary([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		if(IsPair(nd))
		{
			TermNode::Container tcon(term.get_allocator());

			YAssert(!IsSticky(nd.begin()->Tags),
				"Invalid representation found.");
			f(tcon, nd, p_ref);
			tcon.swap(term.GetContainerRef());
		}
		else
			ThrowInsufficientTermsError(nd, p_ref);
		return ReductionStatus::Retained;
	}, term);
}
//@}

//! \since build 834
template<typename _func>
void
SetFirstRest(_func f, TermNode& term)
{
	RetainN(term, 2);

	auto i(term.begin());

	ResolveTerm([&](TermNode& nd_x, bool has_ref){
		CheckResolvedPairReference(nd_x, has_ref);
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

		AssertValueTags(dst);
		if(const auto p = TryAccessLeafAtom<TermReference>(y))
			f(dst, y, *p);
		else
			LiftTerm(dst, y);
	}, term);
}


//! \since build 945
YB_ATTR_nodiscard ValueObject
MakeResolvedParent(TermNode& nd, ResolvedTermReferencePtr p_ref)
{
	if(IsList(nd))
		return MakeParent(MakeEnvironmentParent(
			nd.begin(), nd.end(), nd.get_allocator(), !NPL::IsMovable(p_ref)));
	// XXX: Currently all supported environment representations of a single
	//	object are in the leaf.
	if(IsLeaf(nd))
		return MakeParentSingle(nd.get_allocator(),
			ResolveEnvironment(nd.Value, NPL::IsMovable(p_ref)));
	ThrowInvalidEnvironmentType(nd, p_ref);
}

//! \since build 945
YB_ATTR_nodiscard YB_FLATTEN ValueObject
ResolveParentFrom(TermNode& term)
{
	return ResolveTerm(MakeResolvedParent, term);
}

//! \since build 961
YB_ATTR_nodiscard const TokenValue&
UndefineId(TermNode& term)
{
	Retain(term);
	if(term.size() == 2)
		return NPL::ResolveRegular<const TokenValue>(*std::next(term.begin()));
	ThrowInvalidSyntaxError("Expected exact one term as name to be undefined.");
}

//! \since build 961
YB_ATTR_nodiscard BindingMap&
FetchUndefineMapRef(ContextNode& ctx)
{
	TryRet(ctx.GetRecordRef().GetMapCheckedRef())
	CatchExpr(..., std::throw_with_nested(
		TypeError("Cannot remove a variable in a frozen environment.")))
}

// NOTE: This assumes %f is called synchrnously.
//! \since build 888
template<typename _func>
inline auto
CheckFunctionCreation(_func f) -> decltype(f())
{
	TryRet(f())
	CatchExpr(..., std::throw_with_nested(
		InvalidSyntax("Invalid syntax found in function abstraction.")))
}

//! \since build 959
//@{
template<typename _func>
inline ReductionStatus
ReduceCreateFunction(TermNode& term, _func f)
{
	term.Value = CheckFunctionCreation(f);
	return ReductionStatus::Clean;
}

YB_ATTR_nodiscard shared_ptr<TermNode>
MakeCombinerEvalStruct(TermNode& term, TNIter i)
{
	term.erase(term.begin(), i);
	ClearCombiningTags(term);
	return ShareMoveTerm(
		ystdex::exchange(term, NPL::AsTermNode(term.get_allocator())));
}

template<bool>
struct GReduceVau final
{
	template<typename... _tParams>
	YB_ATTR_nodiscard ReductionStatus
	operator()(TermNode& term, bool no_lift, TNIter i, ValueObject&& vo,
		_tParams&&... args) const
	{
		return ReduceCreateFunction(term, [&]() -> ContextHandler{
			auto formals(ShareMoveTerm(NPL::Deref(++i)));
			auto eformal(ExtractEnvironmentFormal(NPL::Deref(++i)));
			auto p_es(MakeCombinerEvalStruct(term, ++i));

			if(eformal)
				return A1::MakeForm(term, DynamicVauHandler(std::move(formals),
					std::move(vo), std::move(p_es), no_lift,
					std::move(*eformal)), yforward(args)...);
			return A1::MakeForm(term, VauHandler(std::move(formals),
				std::move(vo), std::move(p_es), no_lift), yforward(args)...);
		});
	}
};

template<>
struct GReduceVau<true> final
{
	template<typename... _tParams>
	YB_ATTR_nodiscard ReductionStatus
	operator()(TermNode& term, bool no_lift, TNIter i, ValueObject&& vo,
		_tParams&&... args) const
	{
		return ReduceCreateFunction(term, [&]{
			auto formals(ShareMoveTerm(NPL::Deref(++i)));

			return A1::MakeForm(term, VauHandler(std::move(formals),
				std::move(vo), MakeCombinerEvalStruct(term, ++i), no_lift),
				yforward(args)...);
		});
	}
};


template<size_t _vN, size_t _vWrapping>
YB_FLATTEN ReductionStatus
LambdaVau(TermNode& term, ContextNode& ctx, bool no_lift)
{
	static_assert(_vN <= 1, "Invalid term subterm count found.");

	CheckVariadicArity(term, _vN);
	return GReduceVau<_vN == 0>()(term, no_lift, term.begin(),
		MakeParentSingleNonOwning(term.get_allocator(), ctx.GetRecordPtr()),
		ystdex::size_t_<_vWrapping>());
}

template<size_t _vN, size_t _vWrapping>
YB_FLATTEN ReductionStatus
LambdaVauWithEnvironment(TermNode& term, ContextNode& ctx, bool no_lift)
{
	static_assert(_vN - 1 <= 1, "Invalid term subterm count found.");

	CheckVariadicArity(term, _vN);

	auto i(term.begin());
	auto& tm(NPL::Deref(++i));

	// XXX: As %EvalImplUnchecked, with in-place term modification.
	ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		LiftTermOrCopy(tm, nd, NPL::IsMovable(p_ref));
	}, tm);
	EnsureValueTags(tm.Tags);
	return ReduceSubsequent(tm, ctx,
		NameTypedReducerHandler([&, i, no_lift]{
		return GReduceVau<_vN == 1>()(term, no_lift, i, ResolveParentFrom(tm),
			ystdex::size_t_<_vWrapping>());
	}, "eval-vau-parent"));
}
//@}

//! \since build 840
//@{
ReductionStatus
LambdaImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LambdaVau<0, Strict>(term, ctx, no_lift);
}

//! \since build 918
ReductionStatus
LambdaWithEnvironmentImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LambdaVauWithEnvironment<1, Strict>(term, ctx, no_lift);
}

ReductionStatus
VauImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LambdaVau<1, Form>(term, ctx, no_lift);
}

ReductionStatus
VauWithEnvironmentImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LambdaVauWithEnvironment<2, Form>(term, ctx, no_lift);
}
//@}

//! \since build 921
ReductionStatus
WVauImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LambdaVau<1, Strict>(term, ctx, no_lift);
}

//! \since build 921
ReductionStatus
WVauWithEnvironmentImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LambdaVauWithEnvironment<2, Strict>(term, ctx, no_lift);
}


//! \since build 958
YB_ATTR_nodiscard size_t
AddWrappingCount(size_t n)
{
	++n;
	if(n != 0)
		return n;
	throw NPLException("Wrapping count overflow detected.");
}

//! \since build 859
//@{
YB_NORETURN ReductionStatus
ThrowForWrappingFailure(const type_info& ti)
{
	throw TypeError(ystdex::sfmt("Wrapping failed with type '%s'.", ti.name()));
}

//! \since build 913
ReductionStatus
WrapN(TermNode& term, ResolvedTermReferencePtr p_ref,
	FormContextHandler& fch, size_t n)
{
	// XXX: Allocators are not used on %FormContextHandler for performance.
	return WrapH(term, MakeValueOrMove(p_ref, [&]{
		return fch.Handler;
	}, [&]{
		return std::move(fch.Handler);
	}), n);
}

//! \since build 913
ReductionStatus
WrapRefN(TermNode& term, ResolvedTermReferencePtr p_ref,
	FormContextHandler& fch, size_t n)
{
	if(p_ref)
		return ReduceForCombinerRef(term, *p_ref, fch.Handler, n);
	// XXX: Ditto.
	return WrapH(term, std::move(fch.Handler), n);
}

template<typename _func, typename _func2>
ReductionStatus
WrapUnwrap(TermNode& term, _func f, _func2 f2)
{
	return CallRegularUnaryAs<ContextHandler>(
		[&](ContextHandler& h, ResolvedTermReferencePtr p_ref){
		return DispatchContextHandler(h, p_ref, f, f2);
	}, term);
}
//@}

//! \since build 918
template<ReductionStatus(&_rWrap)(TermNode&,
	ResolvedTermReferencePtr, FormContextHandler&, size_t), typename _func>
ReductionStatus
WrapOrRef(TermNode& term, _func f)
{
	return WrapUnwrap(term,
		[&](FormContextHandler& fch, ResolvedTermReferencePtr p_ref){
		return _rWrap(term, p_ref, fch,
			AddWrappingCount(fch.GetWrappingCount()));
	}, f);
}

//! \since build 918
template<ReductionStatus(&_rWrap)(TermNode&,
	ResolvedTermReferencePtr, FormContextHandler&, size_t)>
ReductionStatus
WrapOnceOrOnceRef(TermNode& term)
{
	return WrapUnwrap(term,
		[&](FormContextHandler& fch, ResolvedTermReferencePtr p_ref){
		return fch.IsOperative() ? _rWrap(term, p_ref, fch, 1)
			: ThrowForWrappingFailure(type_id<FormContextHandler>());
	}, [] YB_LAMBDA_ANNOTATE((const ContextHandler& h), , noreturn)
		-> ReductionStatus{
		ThrowForWrappingFailure(h.target_type());
	});
}


//! \since build 928
void
EqualSubterm(bool& r, Action& act, TermNode::allocator_type a, TNCIter first1,
	TNCIter first2, TNCIter last1)
{
	if(first1 != last1)
	{
		YAssert(!IsSticky(NPL::Deref(first1).Tags),
			"Invalid representation found."),
		YAssert(!IsSticky(NPL::Deref(first2).Tags),
			"Invalid representation found.");

		auto& x(ReferenceTerm(NPL::Deref(first1)));
		auto& y(ReferenceTerm(NPL::Deref(first2)));

		if(TermUnequal(x, y))
			r = {};
		else
		{
			yunseq(++first1, ++first2);
			ystdex::update_thunk(act, a, trivial_swap,
				[&, a, first1, first2, last1]{
				if(r)
					EqualSubterm(r, act, a, first1, first2, last1);
			});
			ystdex::update_thunk(act, a, trivial_swap, [&, a]{
				if(r)
					EqualSubterm(r, act, a, x.begin(), y.begin(), x.end());
			});
		}
	}
}


//! \since build 953
//@{
TNIter
ApplyHead(TermNode& term)
{
	RetainList(term);
#if true
	AssertCombiningTerm(term);
	if(YB_UNLIKELY(term.size() <= 2))
		ThrowInsufficientTermsError(term, {}, 1);
#else
	// NOTE: Any optimized implemenations shall be equivalent to this, except
	//	%CountPrefix can be replaced by %TermNode::size.
	CheckVariadicArity(term, 1);
#endif

	auto i(term.begin());

	ForwardToUnwrapped(*++i);
	return i;
}

ReductionStatus
ApplyTail(TermNode& term, ContextNode& ctx, TNIter i)
{
	const auto i_comb(i);
	auto& t(NPL::Deref(++i));

	[&] YB_LAMBDA_ANNOTATE(() , , flatten){
		LiftToReturn(t);
		t.GetContainerRef().splice(t.begin(), term.GetContainerRef(), i_comb);
	}();

	auto p_env([&]() -> shared_ptr<Environment>{
		if(term.size() == 2)
			return NPL::AllocateEnvironment(term.get_allocator());
		if(term.size() == 3)
			return FetchValidEnvironment(
				ResolveEnvironment(*++i).first);
		ThrowInvalidSyntaxError("Syntax error in applying form.");
	}());

	LiftOtherValue(term, t);
	// NOTE: The precondition is same to the last call in %EvalImplUnchecked.
	//	See also the precondition of %Combine<TailCall>::RelayEnvSwitch.
	return Combine<TailCall>::RelayEnvSwitch(ctx, term, std::move(p_env));
}

TNIter
ListAsteriskHead(TermNode& term)
{
	RetainList(term);
	CheckVariadicArity(term, 0);
	RemoveHead(term);
	return term.begin();
}

ReductionStatus
ListAsteriskTail(TermNode& term)
{
	YAssert(IsList(term), "Invalid representation found.");

	auto last(term.end());

	--last;
	term.GetContainerRef().splice(term.end(), last->GetContainerRef());
	term.Value = std::move(NPL::Deref(last).Value);
	term.Remove(last);
	return ReductionStatus::Retained;
}
//@}

//! \since build 942
//@{
using TermPrefixGuard
	= ystdex::unique_guard<ystdex::prefix_eraser<TermNode::Container>>;

YB_ATTR_nodiscard inline TermPrefixGuard
GuardTermPrefix(TermNode::Container& con) ynothrow
{
	return ystdex::make_unique_guard(
		ystdex::prefix_eraser<TermNode::Container>(con));
}

//! \since build 947
void
RemoveTermSuffix(TermPrefixGuard& gd) ynothrow
{
	auto& eraser(gd.func.func);
	auto& tcon(eraser.container);

	tcon.erase(eraser.position, tcon.end());
	ystdex::dismiss(gd);
}

template<class _tTerm>
void
AccSetTerm(TermNode& term, _tTerm&& lv_l, TermNode& tail,
	const shared_ptr<Environment>& d)
{
	{
		auto& con(term.GetContainerRef());
		auto gd(GuardTermPrefix(con));

		con.insert(con.insert(con.begin(), yforward(lv_l)),
			EvaluateBoundLValueUnwrapped(tail, d));
		RemoveTermSuffix(gd);
	}
	term.Value.Clear();
	// XXX: No %ClearCombiningTags is called, as it would be determined later
	//	in the subsequent combiner call.
}
//@}

/*!
\pre 第一参数的类型可平凡交换。
\since build 917
*/
template<typename _func>
YB_FLATTEN ReductionStatus
Acc(_func f, TermNode& term, ContextNode& ctx)
{
	YAssert(term.size() >= 8, "Invalid recursive call found.");

	auto i(term.begin());
	auto& nterm(*i);
	auto& l(*++i);
	auto& pred(*++i);
	const auto& d(ctx.GetRecordPtr());
	auto& con(term.GetContainerRef());
	auto& lv_l(con.back());
	const auto nterm_cons_combine([&, d](TermNode& tm){
		AccSetTerm(nterm, lv_l, tm, d);
	});

	// NOTE: This shall be stored separatedly to %l because %l is abstract,
	//	which can be a non-list. Also %l is not first-class since it may have
	//	%TermTags::Temporary.
	lv_l = EvaluateLocalObject(l, d);
	nterm_cons_combine(pred);
	return Combine<NonTailCall>::ReduceCallSubsequent(nterm, ctx, d,
		A1::NameTypedReducerHandler(
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		// XXX: Capture of %d by copy is a slightly more efficient.
		std::bind([&, d, f, nterm_cons_combine](TNIter& i_0) -> ReductionStatus{
		auto& base(*++i_0);

		if(!ExtractBool(nterm))
		{
			nterm_cons_combine(*++i_0);
			return Combine<NonTailCall>::ReduceCallSubsequent(nterm, ctx, d,
				A1::NameTypedReducerHandler(
				std::bind([&, d, f](TNIter& i_1){
				return f(l, base, lv_l, nterm, d, ++i_1);
			}, std::move(i_0)), "eval-acc-head-next"));
		}
		LiftOtherValue(term, base);
		return ReductionStatus::Retained;
	}, std::move(i)), "eval-acc-nested"));
}

//! \since build 917
//@{
YB_FLATTEN ReductionStatus
ReduceAccL(TermNode& term, ContextNode& ctx)
{
	// NOTE: Subterms are %nterm, %l, %pred, %base, %head, %tail, %lv_sum_op,
	//	%lv_l.
	YAssert(term.size() == 8, "Invalid recursive call found.");
	return Acc([&] YB_LAMBDA_ANNOTATE(
		(TermNode& l, TermNode& base, TermNode& lv_l, TermNode& nterm,
		const shared_ptr<Environment>& d, TNIter& i) , , flatten){
		auto& tail(*i);

		{
			TermNode::Container tcon(base.get_allocator());

			tcon.push_back(EvaluateBoundLValueUnwrapped(*++i, d));
			tcon.push_back(std::move(nterm));
			tcon.push_back(std::move(base));
			tcon.swap(base.GetContainerRef());
		}
		base.Value.Clear();
		return Combine<NonTailCall>::ReduceCallSubsequent(base, ctx, d,
			A1::NameTypedReducerHandler(
			// XXX: Capture of %d by copy is a slightly more efficient.
			[&, d] YB_LAMBDA_ANNOTATE(() , , flatten){
			AccSetTerm(nterm, std::move(lv_l), tail, d);
			return Combine<NonTailCall>::ReduceCallSubsequent(nterm, ctx, d,
				A1::NameTypedReducerHandler(
				[&] YB_LAMBDA_ANNOTATE(() , , flatten){
				l = std::move(nterm);
				return ReduceAccL(term, ctx);
			}, "eval-accl-accl"));
		}, "eval-accl-tail"));
	}, term, ctx);
}

YB_FLATTEN ReductionStatus
ReduceAccR(TermNode& term, ContextNode& ctx)
{
	// NOTE: Subterms are %nterm, %l, %pred, %base, %head, %tail, %lv_sum_op,
	//	%n2term, %lv_l.
	YAssert(term.size() == 9, "Invalid recursive call found.");
	return Acc([&] YB_LAMBDA_ANNOTATE(
		(TermNode& l, TermNode&, TermNode& lv_l, TermNode& nterm,
		const shared_ptr<Environment>& d, TNIter& i) , , flatten){
		auto& tail(*i);
		auto& n2term(*std::next(term.rbegin()));
		const auto& lv_sum_op(*++i);

		AccSetTerm(n2term, std::move(lv_l), tail, d);
		return Combine<NonTailCall>::ReduceCallSubsequent(n2term, ctx, d,
			// XXX: Capture of %d by copy is a slightly more efficient.
			A1::NameTypedReducerHandler(
			[&, d] YB_LAMBDA_ANNOTATE(() , , flatten){
			l = std::move(n2term);
			// XXX: %A1::ReduceCurrentNext would not prevent direct recursive
			//	calls to %ReduceAccR on %NPL_Impl_NPLA1_Enable_InlineDirect.
			return A1::ReduceCurrentNextThunked(
				*term.emplace(ystdex::exchange(term.GetContainerRef(), [&]{
				TermNode::Container tcon(term.get_allocator());

				tcon.push_back(lv_sum_op);
				tcon.push_back(std::move(nterm));
				return tcon;
			}())), ctx,
				// XXX: The function after decayed is specialized enough without
				//	%trivial_swap.
				ReduceAccR, trivial_swap, A1::NameTypedReducerHandler(
				[&, d] YB_LAMBDA_ANNOTATE(() , , flatten){
				return Combine<NonTailCall>::ReduceEnvSwitch(term, ctx, d);
			}, "eval-accr-sum"));
		}, "eval-accr-accr"));
	}, term, ctx);
}

// XXX: Direct assignment of %TermRange to %ValueObject is likely more efficient
//	than %SetValue.
//! \since build 916
struct TermRange final
{
	TNIter First, Last;
	// NOTE: The tags are used as the operand tags in
	//	%BindParameterObject::operator() in NPLA1.cpp. Implementations shall
	//	prepare the range with the tags reflecting the handling of the operand
	//	lists in %GParameterMatcher::operator() in NPLA1.cpp.
	// XXX: %TermTags::Temporary is actually not used at current.
	TermTags Tags = TermTags::Unqualified;

	TermRange(TermNode& nd, TermTags tags = TermTags::Unqualified) ynothrow
		: First(nd.begin()), Last(nd.end()), Tags(tags)
	{}

	YB_ATTR_nodiscard YB_PURE PDefH(bool, empty, ) const ynothrow
		ImplRet(First == Last)
};

//! \since build 940
YB_ATTR_nodiscard YB_PURE PDefH(bool, IsExpiredRange, TermNode& term)
	ImplRet(!bool(term.Tags & TermTags::Nonmodifying))

/*!
\brief 准备递归调用使用的列表对象：绑定对象为列表临时对象范围。
\since build 913
*/
void
PrepareFoldRList(TermNode& term)
{
	// NOTE: Conceptually, the 1st call to the list element can be applied to
	//	the reference to list, but the subsequent application to the collection
	//	of the rest list elements is only for the sublist (as an non-list
	//	object) due to the rest list is extract as if a call of 'rest%' to the
	//	reference to list, resulting in a sublist reference. %TermRange need to
	//	simulate as such way to avoid unexpected differences of observable
	//	behavior in the object language.
	NPL::ResolveTerm([&] YB_LAMBDA_ANNOTATE(
		(TermNode& nd, ResolvedTermReferencePtr p_ref) , , flatten){
		// NOTE: This is only needed for the outermost call.
		if(IsList(nd))
		{
			if(p_ref)
			{
				// NOTE: Not using %LiftOtherOrCopy to allow delaying the copy.
				if(p_ref->IsMovable())
				{
					LiftOther(term, nd);
					term.Value = TermRange(term);
				}
				else
					// NOTE: Encode the value category information of the list.
					//	This is done in the derivations by tags of reference
					//	values in one more level of indirection, which is less
					//	efficient and not available here. It should be safe
					//	once %TermRange is not visible as first-class objects.
					// XXX: Term tags are currently not respected in prvalues.
					//	This is used to indicate the list is an lvalue which
					//	shall be copied on the element accesses later. This is
					//	comsumed by %IsExpiredRange, only used to provide the
					//	3rd argument to the call to %ExtractRangeFirstOrCopy.
					// XXX: %TermTags::Nonmodifying may have same encoding of
					//	%TermTags::Sticky. This is safe because the term with
					//	%Value of %TermRange target is only used internally in
					//	specific context forms handlers, invisible to any
					//	first-class objects.
					yunseq(term.Tags |= TermTags::Nonmodifying,
						term.Value = TermRange(nd, p_ref->GetTags()));
			}
			else
				term.Value = TermRange(term);
		}
		else
			// NOTE: Always treat the list as an lvalue as in the derivation.
			//	This is done only once here, since all recursive calls still
			//	keep %term as a list.
			ThrowListTypeErrorForNonList(nd, true);
	}, term);
}

//! \since build 916
void
ExtractRangeFirstOrCopy(TermNode& term, TermRange& tr, bool move)
{
	YAssert(!tr.empty(), "Invalid term found.");
	LiftOtherOrCopyPropagate(term, NPL::Deref(tr.First), move, tr.Tags);
	EnsureValueTags(term.Tags), ++tr.First;
}

ReductionStatus
ReduceFoldR1(TermNode& term, ContextNode& ctx)
{
	// NOTE: Subterms are the underlying combiner of 'kons', 'knil',
	//	the term range of 'l' with optional temporary list.
	YAssert(term.size() == 3, "Invalid recursive call found.");

	auto& con(term.GetContainerRef());
	auto& tm(con.back());

	// XXX: This should have been guaranteed by the call to %PrepareFoldRList.
	YAssert(IsTyped<TermRange>(tm), "Invalid non-list term found.");

	auto& tr(tm.Value.GetObject<TermRange>());
	auto i(term.begin());

	if(!tr.empty())
		// XXX: %A1::ReduceCurrentNext would not prevent direct recursive calls
		//	to %ReduceFoldR1 on %NPL_Impl_NPLA1_Enable_InlineDirect.
		return A1::ReduceCurrentNextThunked(
			*term.emplace(ystdex::exchange(con, [&]{
			TermNode::Container tcon(term.get_allocator());

			tcon.push_back(*i);
			ExtractRangeFirstOrCopy(tcon.emplace_back(), tr,
				IsExpiredRange(tm));
			return tcon;
		}())), ctx, ReduceFoldR1, trivial_swap,
			A1::NameTypedReducerHandler(std::bind(
			// TODO: Blocked. Use C++14 lambda initializers to simplify the
			//	implementation.
			[&] YB_LAMBDA_ANNOTATE((shared_ptr<Environment>& d) , , flatten){
			return
				Combine<NonTailCall>::ReduceEnvSwitch(term, ctx, std::move(d));
		}, ctx.ShareRecord()), "eval-foldr1-kons"));
	LiftOtherValue(term, *++i);
	return ReductionStatus::Retained;
}

ReductionStatus
ReduceMap1(TermNode& term, ContextNode& ctx)
{
	// NOTE: Subterms are the underlying combiner of 'appv', the term range of
	//	'l' with optional temporary list, and the converted list elements (i.e.
	//	the results have been applied by the applicative).
	YAssert(term.size() >= 2, "Invalid recursive call found.");

	auto& con(term.GetContainerRef());
	auto& tm(*std::next(con.begin()));

	// XXX: This should have been guaranteed by the call to %PrepareFoldRList.
	YAssert(IsTyped<TermRange>(tm), "Invalid non-list term found.");

	auto& tr(tm.Value.GetObject<TermRange>());

	if(!tr.empty())
	{
		auto& nterm(con.emplace_back());
		auto& tcon(nterm.GetContainerRef());

		tcon.push_back(con.front());
		ExtractRangeFirstOrCopy(tcon.emplace_back(), tr, IsExpiredRange(tm));
		return Combine<NonTailCall>::ReduceCallSubsequent(nterm, ctx,
			ctx.ShareRecord(),
			A1::NameTypedReducerHandler([&] YB_LAMBDA_ANNOTATE(() , , flatten){
			return ReduceMap1(term, ctx);
		}, "eval-map1-appv"));
	}
	con.pop_front();
	con.pop_front();
	return ReductionStatus::Retained;
}

// NOTE: This is needed for the last operation called recursively. Also
//	unwrap here to avoid redundant unwrapping operations.
//! \since build 916
void
PrepareTailOp(TermNode& term, ContextNode& ctx, TermNode& op)
{
	op = EvaluateBoundLValueUnwrapped(*term.emplace(std::move(op)),
		ctx.GetRecordPtr());
}

// NOTE: This is only needed to maintain the lifetime of the tail operator
//	initialized by the call to %PrepareTailOp in the whole liftime of the
//	outmost call.
ReductionStatus
ReduceLiftSum(TermNode& term, ContextNode& ctx, TermNode& rterm,
	ReductionStatus(&f)(TermNode&, ContextNode&))
{
#if NPL_Impl_NPLA1_Enable_Thunked
	// XXX: The function after decayed is specialized enough without
	//	%trivial_swap.
	return A1::ReduceCurrentNext(rterm, ctx, f, trivial_swap,
		A1::NameTypedReducerHandler([&]{
		LiftOtherValue(term, rterm);
		return ctx.LastStatus;
	}, "eval-lift-sum"));
#else
	const auto res(f(rterm, ctx));

	LiftOtherValue(term, rterm);
	return res;
#endif
}

YB_FLATTEN ReductionStatus
ReduceFoldRMap1(TermNode& term, ContextNode& ctx,
	ReductionStatus(&f)(TermNode&, ContextNode&))
{
	auto& con(term.GetContainerRef());

	// NOTE: Bind the last argument as the local reference to list temporary
	//	object.
	PrepareFoldRList(con.back());

	auto i(con.begin());
	auto& rterm(NPL::Deref(i));

	rterm.Clear();
	rterm.GetContainerRef().splice(rterm.end(), con, ++i, con.end());
	// NOTE: Save 'kons' (for %FoldR1) or 'appv' (for %Map1).
	PrepareTailOp(term, ctx, *rterm.begin());
	return ReduceLiftSum(term, ctx, rterm, f);
}

// \pre 第二参数不是第一参数所在的项或其被引用项。
void
PrependList(TermNode::Container& tcon, TermNode& tm)
{
	// NOTE: Bind the term of a list object owning %tcon as the local reference
	//	to list temporary object (as %PrepareFoldRList for lvalues and lifting
	//	directly for prvalues) and prepend the list elements. The term %tm shall
	//	have been lifted by %LiftToReturn or guaranteed as a list prvalue.
	NPL::ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		if(IsList(nd))
		{
			if(p_ref)
			{
				if(p_ref->IsMovable())
				{
					LiftOther(tm, nd);
					EnsureValueTags(tm.Tags);
				}
				else
				{
					auto i(tcon.begin());

					for(auto j(nd.begin()); j != nd.end(); yunseq(++i, ++j))
					{
						i = tcon.emplace(i);
						LiftCopyPropagate(*i, *j, *p_ref);
						EnsureValueTags(i->Tags);
					}
					return;
				}
			}
		}
		else
			ThrowListTypeErrorForNonList(nd, true);
		tcon.splice(tcon.begin(), tm.GetContainerRef());
	}, tm);
}

template<typename _func, typename _func2>
void
ListRangeExtract(TermNode& term, _func f, _func2 f2)
{
	// NOTE: The term is the term range of 'l' with optional temporary list.
	auto& con(term.GetContainerRef());
	auto& tr(term.Value.GetObject<TermRange>());
	const auto tags(tr.Tags);
	const auto val_tags(IsExpiredRange(term) ? tags | TermTags::Unique : tags);
	TermNode::Container tcon(con.get_allocator());

	while(!tr.empty())
	{
		auto& nterm(tcon.emplace_back());
		auto& o(NPL::Deref(tr.First));

		if(const auto p = TryAccessLeafAtom<const TermReference>(o))
		{
			// NOTE: The tags of the term range shall be prepared in the
			//	call to %PrepareFoldRList or similar functions. The nonmodifying
			//	tag indicates the range is not movable, and the term references
			//	as subterms in the range are also not movable due to the
			//	propagated tag.
			// XXX: To bind the element with '&' sigil in the derivations,
			//	a call to %GetLValueTagsOf is needed before the tags passing to
			//	the callback. However, all known callbacks now just ignored
			//	%TermReference::Temporary, so this call is acually unnecessary.
			auto& nd(p->get());

			if(IsBranchedList(nd))
				f(nterm, nd, p->GetEnvironmentReference(),
					PropagateTo(p->GetTags(), tags));
			else
				ThrowInsufficientTermsError(nd, true);
		}
		else if(IsBranchedList(o))
			f2(nterm, o, val_tags);
		else
			ThrowInsufficientTermsError(o, {});
		++tr.First;
	}
	tcon.swap(con),
	term.Value.Clear(),
	// XXX: To cleanup the local tag introduced by %PrepareFoldRList,
	//	%EnsureValueTags is not necessarily enough.
	term.Tags = TermTags::Unqualified;
}

ReductionStatus
ReduceListExtractFirst(TermNode& term)
{
	ListRangeExtract(term, BranchFirstReferenced,
		[](TermNode& nterm, TermNode& o, TermTags o_tags){
		auto& tm(AccessFirstSubterm(o));
		const bool move(NPL::IsMovable(o_tags));

#if false
		if(const auto p = TryAccessLeafAtom<const TermReference>(tm))
		{
			LiftOtherOrCopy(nterm, p->get(), move ? p->IsMovable()
				: NPL::IsMovable(PropagateTo(p->GetTags(), o_tags)));
			EnsureValueTags(nterm.Tags);
		}
		else
			LiftOtherOrCopy(nterm, tm, move);
#else
		AssertValueTags(nterm);
		// NOTE: Any optimized implemenations shall be equivalent to this.
		if(move)
			LiftOtherValue(nterm, tm);
		else
		{
			AssertValueTags(tm);
			LiftCopyPropagate(nterm, tm, o_tags);
		}
		LiftToReturn(nterm);
#endif
	});
	return ReductionStatus::Retained;
}

ReductionStatus
ReduceListExtractRestFwd(TermNode& term)
{
	ListRangeExtract(term, [](TermNode& nterm, TermNode& nd,
		const EnvironmentReference&, TermTags tags){
		// NOTE: As binding of trailing lists, %BindReferenceTags is not called
		//	to make the list elements having %TermTags::Temporary.
		BranchRestFwdReferenced(nterm, nd, NPL::IsMovable(tags));
	}, [](TermNode& nterm, TermNode& o, TermTags o_tags){
		BranchRestFwdReferenced(nterm, o, NPL::IsMovable(o.Tags | o_tags));
	});
	return ReductionStatus::Retained;
}

void
PrepareListExtract(TermNode& term)
{
	RetainN(term);
	LiftOtherValue(term, term.GetContainerRef().back());
	// NOTE: Save the optional temporary list and bind the last argument as the
	//	local reference to list temporary object.
	PrepareFoldRList(term);
}
//@}

/*!
\pre 最后一个参数的类型退化后可平凡交换。
\pre 最后一个参数不依赖下一求值项或蕴含正确设置下一求值项。
\since build 925
*/
template<typename _fNext>
inline ReductionStatus
RelayApplicativeNext(ContextNode& ctx, TermNode& term, _fNext&& next)
{
	// XXX: Calling to %NPL::ToReducer can a bit more efficient here, since less
	//	instantiations are needed.
	return A1::RelayCurrentNext(ctx, term, trivial_swap,
		// NOTE: Capture the term regardless of the next term because
		//	continuation capture here is unsupported and the next term will be
		//	set later in %ReduceChildren.
		[&](TermNode&, ContextNode& c){
		// XXX: The %Value may be retained. It shall be removed later.
		ReduceChildren(term, c);
		// XXX: Normally here should be a call to %SetupNextTerm to ensure the
		//	correct term is to be reduced. However, the precondition allows it
		//	to be deferred on demand.
#if NPL_Impl_NPLA1_Enable_Thunked && false
		SetupNextTerm(ctx, term);
#endif
		return ReductionStatus::Partial;
	}, NPL::ToReducer(ctx.get_allocator(), trivial_swap, yforward(next)));
}

//! \since build 919
//@{
void
ExtractBindings(TermNode& formals, TermNode& operand)
{
	yunseq(formals.Value = operand.Value, formals.Tags = operand.Tags);
	ReduceListExtractFirst(formals);
	ReduceListExtractRestFwd(operand);
}

TermNode&
LetAddFormalsTerm(TermNode& term, bool skip)
{
	auto i(term.begin());

	// XXX: This is usually %with_env in the derivation, but not in all cases
	//	(e.g. see %ProvideLet).
	++i;
	if(skip)
		++i;
	// NOTE: Now subterms are 'bindings', optional 'e' or 'symbols',
	//	originally bound 'bindings', trailing 'body'.
	return *term.GetContainerRef().emplace(i);
}

void
LetBindingsExtract(TermNode& term, bool with_env)
{
	ExtractBindings(LetAddFormalsTerm(term, with_env),
		NPL::Deref(term.begin()));
}
//@}

//! \since build 945
ReductionStatus
LetCall(TermNode& term, ContextNode& ctx, EnvironmentGuard& gd, bool no_lift)
{
	// XXX: To Allow using with %RelayApplicativeNext, there should be a call to
	//	%SetupNextTerm. This is implied in the call to %RelayForCall, so it can
	//	be omitted again. However, it seems preserve it here is a bit more
	//	efficient, at least with x86_64-pc-linux G++ 11.1.
#if NPL_Impl_NPLA1_Enable_Thunked
	SetupNextTerm(ctx, term);
#endif
	// XXX: The 'flatten' attribute here does not make it more efficient.
	return RelayForCall(ctx, term, std::move(gd), no_lift);
}

//! \since build 945
//@{
// XXX: Having 'YB_ATTR_always_inline' here makes performace significantly
//	better, at least with x86_64-pc-linux G++ 12.1. However, it will also warns
//	about failing to inline with [-Wattributes]. To work it around, just ignore
//	the warning here (and it seems that there is no effect to ignore it around
//	the relavant call sites).
#if YB_IMPL_GNUCPP
	YB_Diag_Push
	YB_Diag_Ignore(attributes)
#endif
YB_ATTR_always_inline void
LetCallRemovePrefix(TermNode& term, TNIter i) ynothrow
{
	// NOTE: Remove 2 subterms to make the remaining term as lifting the
	//	trailing 'body' to %term.
	term.erase(term.erase(i));
}
#if YB_IMPL_GNUCPP
	YB_Diag_Pop
#endif

ReductionStatus
LetCallTail(TermNode& term, ContextNode& ctx, EnvironmentGuard& gd,
	bool no_lift, TNIter i)
{
	LetCallRemovePrefix(term, i);
	return LetCall(term, ctx, gd, no_lift);
}
ReductionStatus
LetCallTail(TermNode& term, ContextNode& ctx, EnvironmentGuard& gd,
	bool no_lift)
{
	return LetCallTail(term, ctx, gd, no_lift, term.begin());
}

ReductionStatus
LetCallTailAfterBind(TermNode& term, ContextNode& ctx, TermNode& formals,
	TermNode& operand, EnvironmentGuard& gd, bool no_lift)
{
	// NOTE: Remove any retained %Value before the binding.
	operand.Value.Clear();
	BindParameter(ctx.GetRecordPtr(), formals, operand);
	return LetCallTail(term, ctx, gd, no_lift, term.erase(term.begin()));
}

void
LetCheckBindings(TermNode& term, bool with_env) ynothrowv
{
	// NOTE: Subterms are %operand (the term range of 'bindings' with optional
	//	temporary list), optional 'e', originally bound 'bindings',
	//	trailing 'body'.
	YAssert(term.size() >= (with_env ? 3U : 2U), "Invalid nested call found.");
	yunused(term), yunused(with_env);
}
//@}

// NOTE: The preparation transforms the environment from the known layout of
//	subterms, to make ready to perform the evaluation on the derived expressions
//	later. The derived expressions are either in the form of applicative call to
//	a constructed lambda abstraction containing 'formal' and 'body', or a
//	sequenced definition followed by the 'body', for %LetCombineApplicative and
//	%LetCombineRec cases correspondingly. Extracted operands are extracted
//	arguments to the derived call for %LetCombineApplicative, or extracted
//	initializers for the derived definition for %LetCombineApplicative.
//	Extracted parameters are 'formals' for the lambda abstraction for
//	%LetCombineApplicative or 'formals' for the derived definition for
//	%LetCombineRec.

//! \since build 917
template<typename _func>
ReductionStatus
LetCombineBody(_func f, TermNode& term, ContextNode& ctx)
{
	// NOTE: Subterms are extracted operands plus the parent in %Value,
	//	extracted parameters, unused 'bindings', trailing 'body'.
	YAssert(term.size() >= 3, "Invalid nested call found.");

	auto gd(GuardFreshEnvironment(ctx));
	auto i(term.begin());
	auto& operand(*i);

	// NOTE: Set the parent of the dynamic environment to the static environment
	//	as in %VauBind.
	AssignParent(ctx, std::move(operand.Value));
	// XXX: The %Value in the 1st term shall be removed in the subsequent call
	//	to %f if it is used as the operand term.
	// XXX: The %Value is only used in lvalue 'bindings' in the 3rd term.
	//	Nevertheless, it would be removed later.
	return f(operand, *++i, gd);
}

//! \since build 919
ReductionStatus
LetCombineApplicative(TermNode& term, ContextNode& ctx, bool no_lift)
{
	// NOTE: This does not optimize for cases of zero arguments as in
	//	%FormContextHandler::CallN in NPLA1.cpp, since the fast path is
	//	preferred in %LetCombineEmpty (by a caller like %LetEmpty).
	// NOTE: Subterms are extracted arguments to the call plus the parent in
	//	%Value, extracted 'formals' for the lambda abstraction, unused
	//	'bindings', trailing 'body'.
	YAssert(term.size() >= 3, "Invalid nested call found.");
	return RelayApplicativeNext(ctx, *term.begin(),
		A1::NameTypedReducerHandler([&, no_lift]{
		return LetCombineBody(
			[&](TermNode& operand, TermNode& formals, EnvironmentGuard& gd){
			return
				LetCallTailAfterBind(term, ctx, formals, operand, gd, no_lift);
		}, term, ctx);
	}, "eval-let"));
}

//! \since build 919
ReductionStatus
LetCombineEmpty(TermNode& term, ContextNode& ctx, bool no_lift)
{
	// NOTE: This optimize for cases of zero arguments as in
	//	%FormContextHandler::CallN in NPLA1.cpp.
	// NOTE: Subterms are unused arguments to the call (an empty list)
	//	plus the parent in %Value, unused 'formals' for the lambda
	//	abstraction (an empty list), unused 'bindings' (an empty term),
	//	trailing 'body'.
	YAssert(term.size() >= 3, "Invalid nested call found.");

	auto gd(GuardFreshEnvironment(ctx));
	auto i(term.begin());

	// NOTE: As %LetCombineBody.
	AssignParent(ctx, std::move(i->Value));
	// XXX: The unused 'bindings' may be an empty term or a term representing
	//	the reference to an empty list if not cleanup before the call.
	//	Nevertheless, it would be removed later.
	YAssert(IsEmpty(*std::next(i, 2)), "Invalid term found");
	return LetCallTail(term, ctx, gd, no_lift, term.erase(i));
}

//! \since build 945
template<typename _func>
ReductionStatus
LetCombineNextParent(_func f, TermNode& term, ContextNode& ctx, TNIter i,
	ValueObject& parent)
{
	return ReduceSubsequent(NPL::Deref(i), ctx,
		A1::NameTypedReducerHandler([&, i, f]{
		AssignParent(parent, ResolveParentFrom(*i));
		term.Remove(i);
		return f();
	}, "eval-let-parent"));
}

// NOTE: The preparation transforms the environment from the known layout of
//	subterms.
//! \pre 第一参数的类型可平凡交换。
//@{
//! \since build 917
template<typename _func>
ReductionStatus
LetCombinePrepare(_func f, TermNode& term, ContextNode& ctx, bool with_env)
{
	// NOTE: Subterms are extracted operands, optional 'e',
	//	extracted parameters, originally bound 'bindings', trailing 'body'.
	YAssert(term.size() >= (with_env ? 4U : 3U), "Invalid nested call found.");

	auto i(term.begin());
	// NOTE: The original bound 'bindings' is not needed now. However,
	//	it is not reused to avoid redundant branch check of %with_env. Instead,
	//	the %Value in the 1st subterm is reused to represent the parent object
	//	of the dynamic environment to evaluate the body (i.e. the static
	//	environment in the equivalent closure in the derivation). The reusing of
	//	%Value is a bit more efficient than passing the parent object as an
	//	argument of the call to %f, esp. when %f can contain asynchronous calls
	//	which require to capture the parent object.
	auto& parent(i->Value);

	// XXX: This is guaranteed since the operand of an applicative (implied in
	//	the binding list, and used later as %operand below) shall be a list,
	//	i.e. implying 'IsList(*i)'.
	YAssert(!parent, "Invalid value found in the 1st subterm.");
	// NOTE: After %parent being constructed, the dynamic environment shall then
	//	be constructed in the call to %f.
	if(!with_env)
	{
		AssignWeakParent(parent, term, ctx);
		return f();
	}
	return LetCombineNextParent(f, term, ctx, ++i, parent);
}

#if NPL_Impl_NPLA1Forms_LetAsterisk_ExpandLevel < 3 \
	&& NPL_Impl_NPLA1Forms_LetParent_ExpandLevel >= 1
//! \since build 945
template<typename _func>
ReductionStatus
LetCombinePrepareNoEnv(_func f, TermNode& term, ContextNode& ctx)
{
	// NOTE: Subterms are extracted operands, extracted parameters,
	//	originally bound 'bindings', trailing 'body'.
	YAssert(term.size() >= 3, "Invalid nested call found.");

	auto i(term.begin());
	auto& parent(i->Value);

	YAssert(!parent, "Invalid value found in the 1st subterm.");
	AssignWeakParent(parent, term, ctx);
	return f();
}
#endif
//@}

//! \since build 945
ReductionStatus
LetCombineRec(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LetCombineBody(
		[&](TermNode& operand, TermNode& formals, EnvironmentGuard& egd){
		return RelayApplicativeNext(ctx, operand, A1::NameTypedReducerHandler(
			// TODO: Blocked. Use C++14 lambda initializers to simplify the
			//	implementation.
			std::bind([&, no_lift](EnvironmentGuard& gd){
			return
				LetCallTailAfterBind(term, ctx, formals, operand, gd, no_lift);
		}, std::move(egd)), "eval-letrec-bind"));
	}, term, ctx);
}

#if NPL_Impl_NPLA1Forms_LetAsterisk_ExpandLevel < 3
#	if NPL_Impl_NPLA1Forms_LetParent_ExpandLevel < 1
//! \since build 917
ReductionStatus
LetCommon(TermNode& term, ContextNode& ctx, bool no_lift, bool with_env)
{
	return LetCombinePrepare([&, no_lift]{
		// NOTE: Now subterms are extracted operands plus
		//	the parent in %Value, extracted parameters, unused 'bindings',
		//	trailing 'body'.
		YAssert(term.size() >= 3, "Invalid term found.");
#if false
		// NOTE: There is no need to check the parameter since it would be
		//	checked in the call to %BindParameter later.
		CheckParameterTree(*std::next(term.begin()));
#endif
		// NOTE: The 1st subterm can also have the %Value as the parent object.
		//	Nevertheless, its subterms are used as the argument list at first.
		return LetCombineApplicative(term, ctx, no_lift);
	}, term, ctx, with_env);
}
#	endif

//! \since build 945
ReductionStatus
LetCommonNoEnv(TermNode& term, ContextNode& ctx, bool no_lift)
{
#	if NPL_Impl_NPLA1Forms_LetParent_ExpandLevel >= 1
	return LetCombinePrepareNoEnv([&, no_lift]{
		return LetCombineApplicative(term, ctx, no_lift);
	}, term, ctx);
#	else
	return LetCommon(term, ctx, no_lift, {});
#	endif
}
#endif

//! \since build 945
template<typename _func>
ReductionStatus
LetEmptyGuardTail(_func f, TermNode& term, ContextNode& ctx)
{
	auto r_env(ctx.WeakenRecord());
	auto gd(GuardFreshEnvironment(ctx));

	AssignParent(ctx, term, std::move(r_env));
	return f(gd);
}

// NOTE: Specialized combination of %(LetCheckBindings, LetBindingsExtract,
//	LetCommon) implementation for empty binding lists.
//! \pre 绑定列表项是空项。
//@{
#if NPL_Impl_NPLA1Forms_LetParent_ExpandLevel >= 1
// NOTE: Specialized for %LetEmpty with 'false' %with_env.
//! \since build 945
ReductionStatus
LetEmptyNoEnv(TermNode& term, ContextNode& ctx, bool no_lift)
{
	// NOTE: Subterms are the unused combiner, unused originally bound
	//	'bindings' (an empty term), trailing 'body',
	//	compatible to %LetCheckBindings.
	LetCheckBindings(term, {});
	return LetEmptyGuardTail([&](EnvironmentGuard& gd){
		return LetCallTail(term, ctx, gd, no_lift);
	}, term, ctx);
}
#endif

//! \since build 917
ReductionStatus
LetEmpty(TermNode& term, ContextNode& ctx, bool no_lift, bool with_env)
{
#if NPL_Impl_NPLA1Forms_LetParent_ExpandLevel >= 2
	if(!with_env)
		return LetEmptyNoEnv(term, ctx, no_lift);
	// NOTE: Subterms are the unused combiner, 'e',
	//	unused originally bound 'bindings' (an empty term), trailing 'body',
	//	compatible to %LetCheckBindings.
	LetCheckBindings(term, true);

	auto i(term.begin());
	auto& parent(i->Value);

	i->Clear();
	return LetCombineNextParent([&]{
		auto gd(GuardFreshEnvironment(ctx));

		YAssert(term.size() >= 2, "Invalid nested call found.");
		AssignParent(ctx, std::move(term.begin()->Value));
		return LetCallTail(term, ctx, gd, no_lift);
	}, term, ctx, ++i, parent);
#else
	// NOTE: The optimized implemenation shall be equivalent to this.
	// NOTE: Subterms are the unused combiner, optional 'e',
	//	unused originally bound 'bindings' (an empty term), trailing 'body',
	//	compatible to %LetCheckBindings.
	LetCheckBindings(term, with_env);
	LetAddFormalsTerm(term, with_env);
	// NOTE: This is required by %LetCombinePrepare.
	term.begin()->Clear();
	// NOTE: Now subterms are unused arguments to the call (an empty list),
	//	optional 'e', unused 'formals' for the lambda abstraction (an empty
	//	list), unused 'bindings' (an empty term), trailing 'body'.
	return LetCombinePrepare([&, no_lift]{
		return LetCombineEmpty(term, ctx, no_lift);
	}, term, ctx, with_env);
#endif
}

#if !(NPL_Impl_NPLA1Forms_LetParent_ExpandLevel >= 1)
//! \since build 945
ReductionStatus
LetEmptyNoEnv(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LetEmpty(term, ctx, no_lift, {});
}
#endif
//@}

//! \since build 917
void
LetExpire(TermNode& term, TermNode& nd, const EnvironmentReference& r_env,
	TermTags o_tags)
{
	YAssert(IsList(nd), "Invalid term found.");
	// XXX: As %ReduceToReferenceUList for lvalues in terms of
	//	%BindParameterObject in NPLA1.cpp.
	const auto a(term.get_allocator());
	TermNode::Container con(a);

	for(auto& o : nd)
	{
		// XXX: This is guaranteed by the convention of tags.
		AssertValueTags(o);
		if(const auto p = TryAccessLeafAtom<TermReference>(o))
			con.emplace_back(o.GetContainer(), in_place_type<TermReference>,
				*p);
		else
			con.emplace_back(TermNode::Container(o.get_allocator()),
				std::allocator_arg, a, in_place_type<TermReference>,
				GetLValueTagsOf(o.Tags | o_tags) | TermTags::Unique, o, r_env);
	}
	con.swap(term.GetContainerRef());
	// NOTE: As %PrepareFoldRList.
	term.Value = TermRange(term, TermTags::Temporary);
}

//! \since build 918
void
LetExpireChecked(TermNode& term, TermNode& nd,
	const EnvironmentReference& r_env, TermTags o_tags)
{
	if(IsList(nd))
		LetExpire(term, nd, r_env, o_tags);
	else
		ThrowListTypeErrorForNonList(nd, true);
}

//! \since build 917
ReductionStatus
LetAsteriskImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	CheckVariadicArity(term, 0);

	auto& con(term.GetContainerRef());
	auto i(con.begin());

	// XXX: This might be relied on by reentrant calls.
#if NPL_Impl_NPLA1Forms_LetAsterisk_ExpandLevel < 3
	i->Value = A1::MakeForm(term.get_allocator(),
		no_lift ? Forms::LetAsteriskRef : Forms::LetAsterisk);
#endif

	auto& bindings(*++i);

	// NOTE: Optimize by the fast path for cases of zero arguments.
	if(IsEmpty(bindings))
		return LetEmptyNoEnv(term, ctx, no_lift);
	if(const auto p = TryAccessLeafAtom<TermReference>(bindings))
	{
		auto& nd(p->get());

		// NOTE: Optimize by the fast path for cases of zero arguments.
		if(IsEmpty(nd))
		{
			bindings.Clear();
			return LetEmptyNoEnv(term, ctx, no_lift);
		}
		if(p->IsMovable())
			con.splice(con.begin(), nd.GetContainerRef(), nd.begin());
		// NOTE: The variable %binding denotes a reference to a non-empty list,
		//	and it is converted to %TermRange as the internal representation.
		//	The following code is as %PrepareFoldRList, except that the setting
		//	of %TermTags::Nonmodifying on %bindings is omitted.
		else if(IsList(nd))
		{
			bindings.ClearContainer(),
			bindings.Value = TermRange(nd, p->GetTags());
			con.push_front(nd.GetContainer().front());
			++bindings.Value.GetObject<TermRange>().First;
		}
		else
			ThrowInsufficientTermsError(nd, true);
	}
	// NOTE: The %TermRange is from the converted result above.
	else if(const auto p_tr = TryAccessLeaf<TermRange>(bindings))
	{
		// XXX: Empty ranges shall not be converted to %TermRange above; they
		//	are from the handling of other non-empty %TermRange values.
		if(p_tr->empty())
		{
			bindings.Clear();
			return LetEmptyNoEnv(term, ctx, no_lift);
		}
		// XXX: This assume %TermTags::Nonmodifying even it is not on %bindings,
		//	since %TermRange here can only be converted from the initial
		//	%bindings, see above.
		con.push_front(NPL::Deref(p_tr->First++));
	}
	else
		con.splice(con.begin(), bindings.GetContainerRef(),
			bindings.begin());

	// NOTE: This is the 1st binding extracted for the immediate '$let' or
	//	'$let%' combination. This may represents a reference value.
	auto& extracted(con.front());
#if NPL_Impl_NPLA1Forms_LetAsterisk_ExpandLevel >= 2
	// NOTE: As the %LetExpireChecked call with a binding list prvalue
	//	containing 1 binding subterm (see below).
	const auto letc([&]{
#	if NPL_Impl_NPLA1Forms_LetAsterisk_ExpandLevel >= 3
		// XXX: As the following code with lower expansion level, but more
		//	specialized.
		YAssert(term.size() >= 3, "Invalid nested call found.");

		auto j(term.begin());
		auto& operand(*j);
		auto& formals(*++j);

		// XXX: The check is equivalent in %LetCombinePrepare to ensure %operand
		//	as a list which can be qualified to the operand of an applicative.
		YAssert(IsList(operand), "Invalid value found in the 1st subterm.");
		AssignWeakParent(operand.Value, term, ctx);
		YAssert(operand.size() == 1, "Invalid term found."),
		YAssert(formals.size() == 1, "Invalid term found.");

		auto& opr(AccessFirstSubterm(operand));

		return ReduceSubsequent(opr, ctx,
			// XXX: No 'YB_FLATTEN' here, to avoid G++ 11.1.0 ICE in
			//	'gimple_duplicate_bb' at 'tree-cfg.c:6375'.
			A1::NameTypedReducerHandler([&, no_lift]{
			auto gd(GuardFreshEnvironment(ctx));

			AssignParent(ctx, std::move(operand.Value));
			operand.Value.Clear();
			BindParameter(ctx.GetRecordPtr(), AccessFirstSubterm(formals), opr);
			LetCallRemovePrefix(term, term.erase(term.begin()));
			// XXX: As %LetCall.
#		if NPL_Impl_NPLA1_Enable_Thunked
			SetupNextTerm(ctx, term);
#		endif
			// XXX: Similar to %RelayForCall, but with a known combination (with
			//	operator '$let' or '$let%' as derivations in the object
			//	language).
			return TailCall::RelayNextGuardedProbe(ctx, term, std::move(gd),
				!no_lift, [&, no_lift](TermNode&, ContextNode&){
				const auto&
					h(no_lift ? Forms::LetAsterisk : Forms::LetAsteriskRef);

				ClearCombiningTags(term);
#		if NPL_Impl_NPLA1_Enable_TCO
				YAssert(AccessTCOAction(ctx), "Invalid context found");
#		endif
				ContextState::Access(ctx).ClearCombiningTerm();
				term.Value.Clear();
				// XXX: The usual nested 'combine-return' continuations in
				//	%ReduceCombined are omitted even when TCO is not available,
				//	since they are still idempotent in the tail context
				//	(guaranteed by the semantics of the derivation in the
				//	operation). The enclosing call initiating %LetAsterisk or
				//	%LetAsteriskRef shall still have the continuation.
#		if NPL_Impl_NPLA1_Enable_Thunked
				// XXX: This is not needed because it is called above.
			//	SetupNextTerm(ctx, term);
				// XXX: 'MakeGLContinuation(h)' is specialized enough without
				//	%trivial_swap.
				return
					A1::RelayCurrentOrDirect(ctx, MakeGLContinuation(h), term);
#		else
				return h(term, ctx);
#		endif
			});
		}, "eval-let*"));
#	else
		// XXX: The original 'bindings' (i.e. %extracted) is not a binding
		//	list. Nevertheless, %LetCommonNoEnv would not touch the term.
		return LetCommonNoEnv(term, ctx, no_lift);
#	endif
	});

	// XXX: The call to %GetLValueTagsOf on the tag argument for
	//	%BranchFirstReferenced is unnecessary. See the comments in
	//	%ListRangeExtract.
	if(const auto p = TryAccessLeafAtom<TermReference>(extracted))
	{
		auto& nd(p->get());

		if(IsBranchedList(nd))
		{
			BranchFirstReferenced(*con.emplace_front().emplace(), nd,
				p->GetEnvironmentReference(), p->GetTags());
			BranchRestFwdReferenced(*con.emplace_front().emplace(), nd,
				p->IsMovable());
			return letc();
		}
		ThrowInsufficientTermsError(nd, true);
	}
	else if(IsBranchedList(extracted))
	{
		// XXX: As %BranchFirstReferenced, with 'NPL::IsMovable(tags)' always
		//	'true'.
		auto& nterm(*con.emplace_front().emplace());
		auto& tm(AccessFirstSubterm(extracted));

		if(const auto p_ref = TryAccessLeafAtom<const TermReference>(tm))
		{
			if(!p_ref->IsReferencedLValue())
				LiftMovedOther(nterm, *p_ref, p_ref->IsMovable());
			else
				LiftOtherValue(nterm, tm);
		}
		else
			LiftOtherValue(nterm, tm);

		// XXX: As %BranchRestFwdReferenced, with %move always 'true'.
		auto& ncon(con.emplace_front().emplace()->GetContainerRef());

		ncon.splice(ncon.end(), extracted.GetContainerRef(),
			std::next(extracted.begin()), extracted.end());
		return letc();
	}
	ThrowInsufficientTermsError(extracted, true);
#elif NPL_Impl_NPLA1Forms_LetAsterisk_ExpandLevel == 1
	// NOTE: Ditto. This is a slightly succinct but probably less efficient than
	//	the implementation above.
	const auto letc([&](TermNode& nd, const EnvironmentReference& r_env,
		TermTags tags, bool move){
		// XXX: Ditto.
		if(IsBranchedList(nd))
		{
			BranchFirstReferenced(*con.emplace_front().emplace(), nd,
				r_env, tags);
			BranchRestFwdReferenced(*con.emplace_front().emplace(), nd, move);
			// XXX: Ditto.
			return LetCommonNoEnv(term, ctx, no_lift);
		}
		ThrowInsufficientTermsError(nd, true);
	});

	if(const auto p = TryAccessLeafAtom<TermReference>(extracted))
		return letc(p->get(), p->GetEnvironmentReference(), p->GetTags(),
			p->IsMovable());
	return letc(extracted, ctx.GetRecordPtr(),
		extracted.Tags | TermTags::Unique, true);
#else

	// NOTE: The optimized implemenation shall be equivalent to this.
	// NOTE: Make %extracted a binding list prvalue to fit to the expect of
	//	%LetExpireChecked.
	extracted.emplace(
		ystdex::exchange(extracted, TermNode(extracted.get_allocator())));
	LetExpireChecked(con.emplace_front(), extracted,
		ctx.GetRecordPtr(), extracted.Tags);
	LetCheckBindings(term, {});
	LetBindingsExtract(term, {});
	return LetCommonNoEnv(term, ctx, no_lift);
#endif
}

//! \since build 945
ReductionStatus
LetOrRecImpl(TermNode& term, ContextNode& ctx, ReductionStatus(&let_core)(
	TermNode&, ContextNode&, bool), bool no_lift, bool with_env = {})
{
	CheckVariadicArity(term, with_env ? 1 : 0);

	auto i(term.begin());
	// NOTE: This is to be the initial content of %operand in %let_core.
	auto& forwarded(*i);

	if(with_env)
		++i;

	auto& bindings(*++i);

	// NOTE: Optimize by the fast path for cases of zero arguments.
	if(IsEmpty(bindings))
		return LetEmpty(term, ctx, no_lift, with_env);
	if(const auto p = TryAccessLeafAtom<TermReference>(bindings))
	{
		auto& nd(p->get());

		// NOTE: Optimize by the fast path for cases of zero arguments.
		if(IsEmpty(nd))
		{
			bindings.Clear();
			return LetEmpty(term, ctx, no_lift, with_env);
		}
		if(p->IsMovable())
			LetExpireChecked(forwarded, nd, p->GetEnvironmentReference(),
				p->GetTags());
		else if(IsList(nd))
			// NOTE: As %PrepareFoldRList.
			yunseq(forwarded.Value = TermRange(nd, p->GetTags()),
				forwarded.Tags = TermTags::Nonmodifying);
		else
			ThrowInsufficientTermsError(nd, true);
	}
	else
		LetExpireChecked(forwarded, bindings, ctx.GetRecordPtr(),
			bindings.Tags);
	LetCheckBindings(term, with_env);
	LetBindingsExtract(term, with_env);
	// XXX: Currently the object language has no feature requiring %with_env.
	//	Nevertheless, keep it anyway for simplicity.
	return LetCombinePrepare([&, no_lift]() -> ReductionStatus{
		// NOTE: Now subterms are extracted operands plus the parent in %Value,
		//	extracted parameters, unused 'bindings', trailing 'body'.
		YAssert(term.size() >= 3, "Invalid term found.");
#if false
		// NOTE: There is no need to check the parameter since it would be
		//	checked in the call to %BindParameter later.
		CheckParameterTree(*std::next(term.begin()));
#endif
		// NOTE: The 1st subterm can also have the %Value as the parent object.
		//	Nevertheless, its subterms are used as the argument list at first.
		return let_core(term, ctx, no_lift);
	}, term, ctx, with_env);
}

//! \since build 918
//@{
ReductionStatus
ReduceMoveEnv1(TermNode& term, shared_ptr<Environment>& p_env)
{
	YAssert(term.size() == 1, "Invalid term found.");
	RemoveHead(term);
	term.SetValue(std::move(p_env));
	return ReductionStatus::Regular;
}

ReductionStatus
DoBindingsToEnvironment(TermNode& term, ContextNode& ctx, shared_ptr<
	Environment>& p_env, TNIter i, TermNode& bindings, TermNode& operand)
{
	auto& con(term.GetContainerRef());

	bindings.GetContainerRef().splice(bindings.end(), con, i, con.end());
	// XXX: %TermTags::Temporary is actually always ignored in the call.
	LetExpire(operand, bindings, ctx.GetRecordPtr(), TermTags::Unqualified);

	auto& formals(con.emplace_back());

	ExtractBindings(formals, operand);
	// NOTE: Now subterms are extracted initializers for the definition,
	//	originally bound 'bindings', extracted 'formals' for the definition.
	// XXX: As %LetCommon, with a slightly different layout.
	return RelayApplicativeNext(ctx, operand, A1::NameTypedReducerHandler(
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		std::bind([&](shared_ptr<Environment>& p_e){
		BindParameter(p_e, formals, operand);
		YAssert(term.size() == 3, "Invalid term found.");
		con.pop_front();
		con.pop_front();
		return ReduceMoveEnv1(term, p_e);
	}, std::move(p_env)), "bindings-to-env"));
}

YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, HasImportsSigil, const TokenValue& s) ynothrow
	ImplRet(!s.empty() && (s.front() == '&' || s.front() == '%'))
//@}

//! \since build 919
//@{
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TokenValue, CopyImportName, const TokenValue& s)
	ImplRet(HasImportsSigil(s) ? TokenValue(s.substr(1)) : s)
//@}

#if false
// NOTE: Lock a weak environment reference to a strong environment reference
//	accepting modifications later.
//! \since build 920
YB_ATTR_nodiscard pair<shared_ptr<Environment>, lref<BindingMap>>
LockEnvironmentToModify(const EnvironmentReference& r_env)
{
	auto p_env(r_env.Lock());

	// NOTE: This is like the type check in %SetWithNoRecursion. If %r_env is
	//	from the current environment saved previously, it only fails when the
	//	environment goes out of its lifetime.
	Environment::EnsureValid(p_env);
	// NOTE: Evaluate only once. Equivalent to %SymbolsToImports for more than
	//	one calls of assignment with less overhead.
	return {p_env, p_env->GetMapCheckedRef()};
}
#endif

//! \since build 961
YB_ATTR_nodiscard inline BindingMap&
ShareEnvironmentToModify(const shared_ptr<Environment>& p_env)
{
	// NOTE: This is like the type check in %SetWithNoRecursion, expecting
	//	%p_env nonempty.
	YAssert(p_env, "Invalid environment found.");
	// NOTE: Evaluate only once. Equivalent to %SymbolsToImports for more than
	//	one calls of assignment with less overhead.
	return p_env->GetMapCheckedRef();
}

//! \since build 920
void
BindImports(const shared_ptr<Environment>& p_env, TermNode& term, ContextNode&
	ctx, const shared_ptr<Environment>& p_src, bool ref_symbols = {})
{
	YAssert(IsList(term), "Invalid symbols term found.");
	if(IsBranch(term))
	{
		auto& m(ShareEnvironmentToModify(p_env));

		for(auto& x : term)
		{
			auto& n(NPL::ResolveRegular<TokenValue>(x));
			const auto s(CopyImportName(n));
			// XXX: As %ReduceLeafToken with symbols.
			auto pr(ctx.Resolve(p_src, s));

			if(pr.first)
			{
				auto& bound(*pr.first);
				TermNode nterm(bound.get_allocator());
				auto& env(NPL::Deref(p_env));

				// XXX: As %EvaluateIdentifier and the loop body in the
				//	applicative in %SymbolsToImports, with some optimizations
				//	(see below).
				// XXX: Different to %EvaluateIdentifier, identifiers here are
				//	not used as operators, so
				//	%ContextState::TrySetTailOperatorName is not called.
				if(const auto p = TryAccessLeafAtom<const TermReference>(bound))
				{
					if(p->IsTemporary())
						LiftOtherOrCopy(nterm, p->get(), p->IsModifiable());
					else
						SetEvaluatedReference(nterm, bound, *p);
				}
				else if(bool(bound.Tags & TermTags::Temporary))
					LiftOtherOrCopy(nterm, bound,
						!NPL::Deref(pr.second).IsFrozen());
				else
					SetEvaluatedValue(nterm, bound, pr.second);
				// XXX: Different to %EvaluateIdentifier, only
				//	first-class objects are expected here. Literal
				//	handlers are ignored.
				if(ref_symbols)
					n = Ensigil(n);
				EnsureValueTags(nterm.Tags);
				// XXX: Using explicit anchor pointer is more efficient.
				BindSymbol(m, n, nterm,
					EnvironmentReference(p_env, env.GetAnchorPtr()));
			}
			else
				TryExpr(throw BadIdentifier(s))
				catch(BadIdentifier& e)
				{
					// XXX: Use the source information from %x even if
					//	%s can be different to the string referred in
					//	%x.Value.
					if(const auto p_si
						= QuerySourceInformation(x.Value))
						e.Source = *p_si;
					throw;
				}
		}
	}
}

//! \since build 919
// NOTE: See $2022-05 @ %Documentation::Workflow.
#if YB_IMPL_GNUCPP >= 120000
// XXX: Command line options '-O0', '-Os' or '-fdisable-tree-einline' for all
//	TUs work. However, 'YB_ATTR(optimize("Os"))' here does not.
YB_ATTR(optimize("no-tree-pta"))
#endif
void
ProvideLetCommon(TermNode& term, ContextNode& ctx)
{
	// NOTE: Subterms are extracted arguments to the call, 'symbols',
	//	extracted 'formals' for the lambda abstraction,
	//	originally bound 'bindings', trailing 'body'.
	YAssert(term.size() >= 4, "Invalid nested call found.");

	auto& con(term.GetContainerRef());
	const auto a(con.get_allocator());
	auto i(con.begin());
	auto i_symbols(++i);
	auto& nd(ReferenceTerm(*i_symbols));

	// NOTE: Like %Forms::SymbolsToImports.
	if(IsList(nd))
		// NOTE: Checks are required before the evaluation of 'body'.
		for(const auto& x : nd)
			yunused(NPL::ResolveRegular<const TokenValue>(x));
	else
		ThrowListTypeErrorForNonList(nd, true);
	con.splice(++++++i, con, i_symbols);
	// NOTE: Now subterms are extracted arguments to the call, extracted
	//	'formals' for the lambda abstraction, originally bound 'bindings',
	//	'symbols', trailing 'body'.
	YAssert(term.size() >= 4, "Invalid term found.");
	// NOTE: Construct the delaying operative before 'symbols'. The applicative
	//	and other trailing subterms make up the constructed trailing body
	//	subterms, which will be evaluated later in the dynamic environment
	//	created immediately before the evaluation of the constructed body. The
	//	constructed body is the combination of the delaying operative and
	//	the operand, see below.
	// XXX: The form used as %LiteralHandler is not necessarily supported in
	//	this context (by the NPLA1 canonical evaluation algorithm), although it
	//	can work with the current implementation of %SetupDefaultInterpretation.
	con.emplace(i_symbols, A1::AsForm(a, ystdex::bind1(
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		[&, i](TermNode& t, ContextNode& c, shared_ptr<Environment>& p_env){
		// NOTE: Subterms are the unused combiner, 'symbols', trailing 'body'.
		YAssert(t.size() >= 2, "Invalid term found.");
		AssertNextTerm(c, t);

		auto& body(*t.begin());
		auto& tcon(body.GetContainerRef());

		body.Clear();
		tcon.splice(tcon.end(), con, i, con.end());
		// NOTE: Evaluate 'body', and then create the bindings based on the
		//	evaluated 'body'. The imports refer to some subobjects in the
		//	evaluated subterms; otherwise there will be an error during the
		//	creation.
		return ReduceSubsequent(body, c, A1::NameTypedReducerHandler(
			std::bind([&](shared_ptr<Environment>& p_env_0){
			BindImports(p_env_0, nd, c, c.GetRecordPtr());
			t.SetValue(c.ShareRecord());
			return ReductionStatus::Clean;
		}, std::move(p_env)), "provide-let-return"));
	}, std::placeholders::_2, ctx.ShareRecord())));
	// XXX: As %LetCombinePrepare, except that non-empty 'con.begin()->Value'
	//	before the assignment is allowed.
	AssignWeakParent(con.begin()->Value, a, ctx);
	// NOTE: Subterms are extracted arguments to the call plus the parent in
	//	%Value, extracted 'formals' for the lambda abstraction,
	//	unused 'bindings', constructed trailing body subterms (the delaying
	//	operative, 'symbols', trailing 'body'). This layout is compatible to the
	//	precondition of %LetCombineApplicative or %LetCombineEmpty.
}

//! \since build 920
ReductionStatus
ImportImpl(TermNode& term, ContextNode& ctx, bool ref_symbols)
{
	RetainList(term);
	CheckVariadicArity(term, 0);
	RemoveHead(term);

	const auto i(term.begin());

	return ReduceSubsequent(*i, ctx, A1::NameTypedReducerHandler(std::bind(
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		[&, i, ref_symbols](const shared_ptr<Environment>& d)
		-> ReductionStatus{
		const auto p_env(ResolveEnvironment(*i).first);
		auto& con(term.GetContainerRef());

		con.erase(i);
		BindImports(d, term, ctx, p_env, ref_symbols);
		return ReduceReturnUnspecified(term);
	}, ctx.ShareRecord()), "import-bindings"));
}

//! \since build 943
YB_FLATTEN YB_NONNULL(2) ReductionStatus
AssocImpl(TermNode& term, bool(*eq)(const ValueObject&, const ValueObject&))
{
	RetainN(term, 2);
	RemoveHead(term);

	auto i(term.begin());
	auto& nd_x(ReferenceTerm(*i));

	return ResolveTerm(
		[&](TermNode& nd, ResolvedTermReferencePtr p_ref) -> ReductionStatus{
		if(IsList(nd))
		{
			for(auto& tm : nd)
			{
				auto& sub(ReferenceTerm(tm));

				CheckResolvedPairReference(sub, true);

				auto& sub2(ReferenceTerm(AccessFirstSubterm(sub)));

				if(IsLeaf(nd_x) && IsLeaf(sub2) ? eq(nd_x.Value, sub2.Value)
					: ystdex::ref_eq<>()(nd_x, sub2))
				{
					if(!p_ref)
						LiftOther(term, tm);
					else
						LiftOtherOrCopyPropagateTags(term, tm,
							p_ref->GetTags());
					// XXX: %RegularizeTerm would imply %EnsureValueTags.
					return ReductionStatus::Retained;
				}
			}
			term.Clear();
			return ReductionStatus::Clean;
		}
		// XXX: This is known to be different to the derivation using 'first&'
		//	in the exception message.
		ThrowListTypeErrorForNonList(nd, true);
	}, *++i);
}

} // unnamed namespace;

bool
IsSymbol(const string& id) ynothrow
{
	return !id.empty() && IsNPLASymbol(id) && ystdex::fast_all_of(id.begin(),
		id.end(), [] YB_LAMBDA_ANNOTATE((char c), ynothrow, const){
			// XXX: Conservatively only check the basic character set. There
			//	could be false negative results.
			return ystdex::isdigit(c) || bool(std::strchr(
				"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
				"!$%&*./:<=>?@^_~+-", c));
		});
}

TokenValue
StringToSymbol(const string& str)
{
	// XXX: Allocators are not used here for performance in most cases.
	return TokenValue(str);
}
TokenValue
StringToSymbol(string&& str)
{
	// XXX: Ditto.
	return TokenValue(std::move(str));
}

const string&
SymbolToString(const TokenValue& s) ynothrow
{
	return s;
}


bool
Encapsulation::Equal(const TermNode& x, const TermNode& y)
{
	if(TermUnequal(x, y))
		return {};

	// NOTE: This is a trampoline to eliminate the call depth limitation.
	// XXX: This is not like %SeparatorPass in %NPLA1 inserting subnodes for
	//	each turn of the search which is not friendly to short-curcuit
	//	evaluation.
	bool r(true);
	Action act{};

	EqualSubterm(r, act, x.get_allocator(), x.begin(), y.begin(), x.end());
	while(act)
	{
		const auto a(std::move(act));

		a();
	}
	return r;
}


ReductionStatus
Encapsulate::operator()(TermNode& term) const
{
	return Forms::CallUnary([this] YB_LAMBDA_ANNOTATE((TermNode& tm), , pure){
		return Encapsulation(GetType(), std::move(tm));
	}, term);
}


ReductionStatus
EncapsulateValue::operator()(TermNode& term) const
{
	return Forms::CallUnary([this](TermNode& tm){
		LiftToReturn(tm);
		return Encapsulation(GetType(), std::move(tm));
	}, term);
}


ReductionStatus
Encapsulated::operator()(TermNode& term) const
{
	return CallUnary([this] YB_LAMBDA_ANNOTATE((TermNode& tm), , pure){
		return bool(ystdex::call_value_or([this] YB_LAMBDA_ANNOTATE(
			(const Encapsulation& enc), ynothrow, pure){
			return Get() == enc.Get();
		}, TryAccessReferencedTerm<Encapsulation>(tm)));
	}, term);
}


ReductionStatus
Decapsulate::operator()(TermNode& term) const
{
	return Forms::CallRegularUnaryAs<const Encapsulation>(
		[&](const Encapsulation& enc, ResolvedTermReferencePtr p_ref){
		if(Get() == enc.Get())
		{
			auto& tm(enc.TermRef);

			return MakeValueOrMove(p_ref, [&]() -> ReductionStatus{
				// NOTE: As an lvalue reference, the object in %tm cannot be
				//	destroyed.
				if(const auto p = TryAccessLeafAtom<const TermReference>(tm))
					term.SetContent(tm.GetContainer(), *p);
				else
				{
					// XXX: Subterms cleanup is deferred.
					// XXX: Allocators are not used here for performance.
					term.SetValue(term.get_allocator(), in_place_type<
						TermReference>, tm, p_ref->GetEnvironmentReference());
					return ReductionStatus::Clean;
				}
				return ReductionStatus::Retained;
			}, [&]{
				LiftTerm(term, tm);
				return ReductionStatus::Retained;
			});
		}
		throw TypeError("Mismatched encapsulation type found.");
	}, term);
}

namespace Forms
{

void
Eq(TermNode& term)
{
	EqTermReference(term, YSLib::HoldSame);
}

void
EqLeaf(TermNode& term)
{
	EqTermValue(term, ystdex::equal_to<>());
}

void
EqReference(TermNode& term)
{
	EqTermValue(term, YSLib::HoldSame);
}

void
EqValue(TermNode& term)
{
	EqTermReference(term, ystdex::equal_to<>());
}

ReductionStatus
EqualTermValue(TermNode& term, ContextNode& ctx)
{
	return EqTerm(term,
		[&](const TermNode& x, const TermNode& y) -> ReductionStatus{
		if(TermUnequal(x, y))
		{
			term.Value = false;
			return ReductionStatus::Clean;
		}
#if NPL_Impl_NPLA1_Enable_Thunked
		yunused(ctx);
		term.Value = true;
		return EqualSubterm(term.Value.GetObject<bool>(), ctx, true, x.begin(),
			y.begin(), x.end());
#else
		yunused(ctx);
		term.Value = EqualSubterm(x.begin(), y.begin(), x.end());
		return ReductionStatus::Clean;
#endif
	}, static_cast<const TermNode&(&)(const TermNode&)>(ReferenceTerm));
}


ReductionStatus
If(TermNode& term, ContextNode& ctx)
{
	RetainList(term);

	const auto size(term.size());

	if(size == 3 || size == 4)
	{
		auto i(std::next(term.begin()));

		// XXX: Use captured %ctx seems more efficient here.
		return ReduceSubsequent(*i, ctx, NameTypedReducerHandler([&, i]{
			auto j(i);

			if(!ExtractBool(*j))
				++j;
			return ++j != term.end() ? ReduceOnceLifted(term, ctx, *j)
				: ReduceReturnUnspecified(term);
		}, "select-clause"));
	}
	else
		ThrowInvalidSyntaxError("Syntax error in conditional form.");
}

ReductionStatus
Cond(TermNode& term, ContextNode& ctx)
{
	RetainList(term);
	RemoveHead(term);
#if NPL_Impl_NPLA1_Enable_Thunked
	return ReduceCond(term, ctx, term.begin());
#else
	for(auto i(term.begin()); i != term.end(); ++i)
	{
		auto& clause(NPL::Deref(i));
		auto j(CondClauseCheck(clause));

		ReduceOnce(NPL::Deref(j), ctx);
		if(CondTest(clause, j))
			return ReduceOnceLifted(term, ctx, clause);
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
Not(const TermNode& x)
{
	auto& nd(ReferenceTerm(x));

	return IsLeaf(nd) && nd.Value == false;
}

ReductionStatus
And(TermNode& term, ContextNode& ctx)
{
	return AndOr<And2>(term, ctx);
}

ReductionStatus
Or(TermNode& term, ContextNode& ctx)
{
	return AndOr<Or2>(term, ctx);
}


ReductionStatus
Cons(TermNode& term)
{
	auto i(ConsHead(term));

	LiftToReturn(*i);
	LiftToReturn(*++i);
	return ConsTail(term, *i);
}

ReductionStatus
ConsRef(TermNode& term)
{
	auto i(ConsHead(term));

	return ConsTail(term, *++i);
}

ReductionStatus
ForwardFirst(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 2);

	auto i(term.begin());
	auto& op(*i);
	auto& appv(*++i);

	return NPL::ResolveTerm([&] YB_LAMBDA_ANNOTATE(
		(TermNode& nd, ResolvedTermReferencePtr p_ref) , , flatten){
		if(IsPair(nd))
		{
			const auto assign_term([&] YB_LAMBDA_ANNOTATE(
				(TermNode::Container&& c, ValueObject&& vo) , , flatten){
				// XXX: The term %op is reused as a shared bound operand as
				//	the referent.
				op = TermNode(std::allocator_arg, term.get_allocator(),
					std::move(c), std::move(vo));
			});
			// NOTE: This is the term of the object referenced by variable 'x'
			//	in the specification.
			auto& tm(AccessFirstSubterm(nd));

			// NOTE: As %BindParameterObject::Match in NPLA1.cpp, never bind a
			//	temporary to a glvalue list element. Thus, no tag is checked in
			//	the condition here.
			if(p_ref)
				BindLocalReference(p_ref->GetTags()
					& (TermTags::Unique | TermTags::Nonmodifying), tm,
					assign_term, p_ref->GetEnvironmentReference());
			else
				BindMoveLocalObject(tm, assign_term);
			ForwardToUnwrapped(appv);
			{
				TermNode::Container tcon(term.get_allocator());

				tcon.push_back(std::move(appv));
				tcon.push_back(std::move(op));
				tcon.swap(term.GetContainerRef());
			}
			// NOTE: See the precondition of
			//	%Combine<TailCall>::ReduceEnvSwitch.
			return Combine<TailCall>::ReduceEnvSwitch(term, ctx,
				EnvironmentGuard(ctx, ctx.GetRecordPtr()));
		}
		else
			ThrowInsufficientTermsError(nd, p_ref);
	}, *++i);
}

ReductionStatus
First(TermNode& term)
{
	return FirstOrVal(term, [&](TermNode& tm, ResolvedTermReferencePtr p_ref){
		return ReduceToFirst(term, tm, p_ref);
	});
}

ReductionStatus
FirstAt(TermNode& term)
{
	return FirstAtRef(term, ReduceToReferenceAt);
}

ReductionStatus
FirstFwd(TermNode& term)
{
	return FirstOrVal(term, [&](TermNode& tm, ResolvedTermReferencePtr p_ref){
		if(!p_ref)
			LiftOther(term, tm);
		else
			LiftOtherOrCopyPropagateTags(term, tm, p_ref->GetTags());
		// XXX: %RegularizeTerm would imply %EnsureValueTags.
		return ReductionStatus::Retained;
	});
}

ReductionStatus
FirstRef(TermNode& term)
{
	// XXX: The pointer for the call to %ReduceToReference is guarnateed not
	//	null. However, explicit inline expansion here does not make it more
	//	efficient at least in code generation by x86_64-pc-linux G++ 12.1 when
	//	not called.
	return FirstAtRef(term, ReduceToReference);
}

ReductionStatus
FirstVal(TermNode& term)
{
	return FirstOrVal(term, [&](TermNode& tm, ResolvedTermReferencePtr p_ref){
		const bool move(NPL::IsMovable(p_ref));

		// NOTE: As %ReduceToReturn to another term, with restrictrion from
		//	propagated tags.
		// XXX: Simple 'ReduceToValue(term, tm)' is wrong because it may move
		//	non-unqiue reference object away.
		if(const auto p = TryAccessLeafAtom<const TermReference>(tm))
			LiftMovedOther(term, *p, move && p->IsMovable());
		else
			// XXX: Term tags are currently not respected in prvalues.
			LiftOtherOrCopy(term, tm, move);
		return ReductionStatus::Retained;
	});
}

ReductionStatus
RestFwd(TermNode& term)
{
	return RestOrVal(term, [&](TermNode::Container& tcon,
		TermNode& nd, ResolvedTermReferencePtr p_ref){
		auto first(nd.begin());
		const auto last(nd.end());

		++first;
		if(NPL::IsMovable(p_ref))
		{
			tcon.splice(tcon.end(), nd.GetContainerRef(), first, last);
			term.Value = std::move(nd.Value);
		}
		else
		{
			// NOTE: Not moving, but the tags are needed for propagation.
			const auto tags(NPL::Deref(p_ref).GetTags());

			// XXX: As %LiftElementsToReturn.
			for(; first != last && !IsSticky(first->Tags); ++first)
			{
				auto& nterm(tcon.emplace_back());

				LiftCopyPropagate(nterm, *first, tags);
				EnsureValueTags(nterm.Tags);
			}
			// XXX: As %LiftCopyPropagate, with range suffix instead of
			//	the subterm container.
			CopyRestContainer(tcon, first, last);
			term.Value = nd.Value;
			// NOTE: Propagate tags if it is a term reference.
			if(const auto p = TryAccessLeaf<TermReference>(term))
				p->PropagateFrom(tags);
		}
	});
}

ReductionStatus
RestRef(TermNode& term)
{
	return CallResolvedUnary([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		CheckResolvedPairReference(nd, p_ref);

		const auto o_tags(p_ref->GetTags()
			& (TermTags::Unique | TermTags::Nonmodifying));
		const auto a(term.get_allocator());
		auto first(nd.begin());

		++first;
		if(const auto p = TryAccessLeaf<TermReference>(nd))
		{
			term.GetContainerRef() = TermNode::Container(first, nd.end(), a),
			term.Value = ValueObject(std::allocator_arg, a,
				in_place_type<TermReference>, PropagateTo(BindReferenceTags(*p),
				o_tags), *p);
		}
		// NOTE: Irregular representation is constructed for the list subobject
		//	reference.
		else
		{
			// NOTE: As %BindParameterObject::operator()#2 in NPLA1.cpp.
			// XXX: Initial tags are trailing sequence tags for '&' bindings.
			const auto tags(GetLValueTagsOf(nd.Tags | o_tags));
			auto p_sub(A1::AllocateSharedTerm(a));
			auto& tcon(p_sub->GetContainerRef());
			const auto& r_env(p_ref->GetEnvironmentReference());

			for(; first != nd.end() && !IsSticky(first->Tags); ++first)
			{
#if true
				auto& tm(NPL::Deref(first));

				// NOTE: As %ReduceToReference.
				AssertValueTags(tm);
				if(const auto p_r = TryAccessLeafAtom<TermReference>(tm))
					// NOTE: As %LiftPropagatedReference, also like
					//	%EmplaceReference in NPLA1.cpp (without %move but with
					//	propagation enabled).
					tcon.emplace_back(TermNode::Container(tm.GetContainer(), a),
						ValueObject(in_place_type<TermReference>,
						PropagateTo(p_r->GetTags(), tags), *p_r));
				else
					// NOTE: As %ReduceToReferenceAt.
					tcon.emplace_back(TermNode::Container(a), ValueObject(
						std::allocator_arg, a, in_place_type<TermReference>,
						// XXX: Use %PropagateTo should be OK if xvalues are
						//	resolved.
						GetLValueTagsOf(tm.Tags | tags), tm, r_env));
#else
				// XXX: Same to %FirstRef.
				ReduceToReference(tcon.emplace_back(), *first, p_ref);
#endif
			}
			if(nd.Value)
				LiftTermRef(p_sub->Value, nd.Value);
			else
				YAssert(first == nd.end(), "Invalid representation found.");
			return
				ReduceAsSubobjectReference(term, std::move(p_sub), r_env, tags);
		}
		return ReductionStatus::Retained;
	}, term);
}

ReductionStatus
RestVal(TermNode& term)
{
	return RestOrVal(term, [&] YB_LAMBDA_ANNOTATE((TermNode::Container& tcon,
		TermNode& nd, ResolvedTermReferencePtr p_ref) , , flatten){
		auto first(nd.begin());
		const auto last(nd.end());

		++first;
		if(NPL::IsMovable(p_ref))
		{
			auto& con(nd.GetContainerRef());

			// NOTE: Split the rest subterms without invalidating the 1st
			//	subterm in %nd.
			tcon.splice(tcon.end(), con, first, last);
			// NOTE: Swap the containers to make %nd the rest.
			tcon.swap(con);

			const auto gd(ystdex::make_guard([&]() ynothrow{
				// NOTE: Swap back to restore the 1st subterm in %nd.
				tcon.swap(con);
			}));

			// XXX: As %LiftElementsToReturn without using extra containers.
			tcon.splice(tcon.end(), con, con.begin(), LiftPrefixToReturn(nd));
			LiftToReturn(nd);
			con.splice(con.begin(), tcon, std::next(tcon.begin()), tcon.end());
			term.Value = std::move(nd.Value);
		}
		else
		{
			// XXX: As %LiftElementsToReturn.
			for(; first != last && !IsSticky(first->Tags); ++first)
			{
				auto& tm(*first);

				// NOTE: As in %FirstVal, but move is not allowed. This
				//	restriction is assumed by propagated tags.
				if(const auto p = TryAccessLeafAtom<const TermReference>(tm))
					EnsureValueTags(tcon.insert(tcon.end(), p->get())->Tags);
				else
					tcon.push_back(tm);
				AssertValueTags(tcon.back());
			}
			// XXX: As above, with range suffix instead of the subterm
			//	container.
			if(const auto p = TryAccessValue<const TermReference>(nd.Value))
			{
				auto& src(p->get());
#if false
				// XXX: This is not that efficient.
				TermNode::Container ncon(src.get_allocator());

				CopyRestContainer(ncon, src.begin(), src.end());
				tcon.splice(tcon.end(), ncon);
#else
				tcon.splice(tcon.end(), TermNode::Container(src.GetContainer(),
					src.get_allocator()));
#endif
				term.Value = src.Value;
			}
			else
			{
				CopyRestContainer(tcon, first, last);
				term.Value = nd.Value;
			}
		}
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
		LiftOtherValue(dst, y);
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

YB_FLATTEN void
SetRest(TermNode& term)
{
	SetFirstRest([](TermNode& nd_x, TermNode& y){
		LiftToReturn(y);
		ConsRest(y, nd_x);
	}, term);
}

YB_FLATTEN void
SetRestRef(TermNode& term)
{
	SetFirstRest([](TermNode& nd_x, TermNode& y){
		ConsRest(y, nd_x);
	}, term);
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
EvalUnit(TermNode& term, ContextNode& ctx)
{
	CallUnaryAs<const string>([&](const string& unit){
		term.SetContent(A1::Perform(ContextState::Access(ctx), unit));
	}, term);
	return ReductionStatus::Retained;
}

ReductionStatus
RemoteEval(TermNode& term, ContextNode& ctx)
{
	return RemoteEvalImpl(term, ctx, {});
}

ReductionStatus
RemoteEvalRef(TermNode& term, ContextNode& ctx)
{
	return RemoteEvalImpl(term, ctx, true);
}


void
MakeEnvironment(TermNode& term)
{
	RetainList(term);
	RemoveHead(term);
	term.SetValue(CreateEnvironment(term));
}

void
GetCurrentEnvironment(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 0);
	[&] YB_LAMBDA_ANNOTATE(() , , flatten){
		term.SetValue(ctx.WeakenRecord());
	}();
}

void
LockCurrentEnvironment(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 0);
	[&] YB_LAMBDA_ANNOTATE(() , , flatten){
		term.SetValue(ctx.ShareRecord());
	}();
}


ReductionStatus
DefineLazy(TermNode& term, ContextNode& ctx)
{
	DoDefine(term, [&](const TermNode& t){
		CheckBindParameter(ctx.GetRecordPtr(), t, term);
	});
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
	const auto& id(UndefineId(term));

	term.Value = Environment::Remove(FetchUndefineMapRef(ctx), id);
}

void
UndefineChecked(TermNode& term, ContextNode& ctx)
{
	const auto& id(UndefineId(term));

	Environment::RemoveChecked(FetchUndefineMapRef(ctx), id);
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
LambdaWithEnvironment(TermNode& term, ContextNode& ctx)
{
	return LambdaWithEnvironmentImpl(term, ctx, {});
}

ReductionStatus
LambdaWithEnvironmentRef(TermNode& term, ContextNode& ctx)
{
	return LambdaWithEnvironmentImpl(term, ctx, true);
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
WVau(TermNode& term, ContextNode& ctx)
{
	return WVauImpl(term, ctx, {});
}

ReductionStatus
WVauRef(TermNode& term, ContextNode& ctx)
{
	return WVauImpl(term, ctx, true);
}

ReductionStatus
WVauWithEnvironment(TermNode& term, ContextNode& ctx)
{
	return WVauWithEnvironmentImpl(term, ctx, {});
}

ReductionStatus
WVauWithEnvironmentRef(TermNode& term, ContextNode& ctx)
{
	return WVauWithEnvironmentImpl(term, ctx, true);
}


ReductionStatus
Wrap(TermNode& term)
{
	return WrapOrRef<WrapN>(term,
		[&](ContextHandler& h, ResolvedTermReferencePtr p_ref){
		return WrapO(term, h, p_ref);
	});
}

ReductionStatus
WrapRef(TermNode& term)
{
	return WrapOrRef<WrapRefN>(term,
		[&](ContextHandler& h, ResolvedTermReferencePtr p_ref){
		// XXX: Allocators are not used, as %WrapO.
		return p_ref ? ReduceForCombinerRef(term, *p_ref, h, 1)
			: WrapH(term, std::move(std::move(h)), 1);
	});
}

ReductionStatus
WrapOnce(TermNode& term)
{
	return WrapOnceOrOnceRef<WrapN>(term);
}

ReductionStatus
WrapOnceRef(TermNode& term)
{
	return WrapOnceOrOnceRef<WrapRefN>(term);
}

ReductionStatus
Unwrap(TermNode& term)
{
	using namespace std::placeholders;

	return WrapUnwrap(term, std::bind(UnwrapResolved, std::ref(term),
		_1, _2), ThrowForUnwrappingFailure);
}


ReductionStatus
CheckEnvironment(TermNode& term)
{
	RetainN(term);

	auto& tm(*std::next(term.begin()));

	// XXX: The call of %as_const is needed to prevent modification.
	Environment::EnsureValid(ResolveEnvironment(ystdex::as_const(tm)).first);
	LiftOtherValue(term, tm);
	return ReductionStatus::Regular;
}

ReductionStatus
CheckParent(TermNode& term)
{
	RetainN(term);

	auto& tm(*std::next(term.begin()));

	CheckFunctionCreation([&]{
		ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
			if(!IsExtendedList(nd))
				// XXX: The call of %as_const is needed to prevent modification.
				Environment::EnsureValid(
					ResolveEnvironment(ystdex::as_const(nd)).first);
			else if(IsList(nd))
				Environment::CheckParent(MakeEnvironmentParent(nd.begin(),
					nd.end(), nd.get_allocator(), true));
			else
				ThrowInvalidEnvironmentType(nd, p_ref);
		}, tm);
	});
	LiftOtherValue(term, tm);
	return ReductionStatus::Regular;
}

ReductionStatus
CheckListReference(TermNode& term)
{
	return CheckReference(term, CheckResolvedListReference);
}

ReductionStatus
CheckPairReference(TermNode& term)
{
	return CheckReference(term, CheckResolvedPairReference);
}


// NOTE: As %ProvideLetCommon.
#if YB_IMPL_GNUCPP >= 120000
YB_ATTR(optimize("no-tree-pta"))
#endif
ReductionStatus
MakeEncapsulationType(TermNode& term)
{
	const auto a(term.get_allocator());
	// NOTE: The %p_type handle can be extended to point to a metadata block.
	{
		TermNode::Container tcon(a);
		// XXX: Allocator is not used here for better performance.
		shared_ptr<void> p_type(new yimpl(byte));
	//	shared_ptr<void> p_type(YSLib::allocate_shared<yimpl(byte)>(a));

		tcon.push_back(A1::AsForm(a, Encapsulate(p_type), Strict));
		tcon.push_back(A1::AsForm(a, Encapsulated(p_type), Strict));
		tcon.push_back(A1::AsForm(a, Decapsulate(p_type), Strict));
		tcon.swap(term.GetContainerRef());
	}
	return ReductionStatus::Retained;
}


ReductionStatus
Apply(TermNode& term, ContextNode& ctx)
{
	return ApplyTail(term, ctx, ApplyHead(term));
}

ReductionStatus
ApplyList(TermNode& term, ContextNode& ctx)
{
	auto i(ApplyHead(term));
	const auto i_comb(i);

	[&] YB_LAMBDA_ANNOTATE(() , , flatten){
		ResolveTerm([&](const TermNode& nd, bool has_ref){
			if(YB_UNLIKELY(!IsList(nd)))
				ThrowListTypeErrorForNonList(nd, has_ref);
		}, NPL::Deref(++i));
	}();
	return ApplyTail(term, ctx, i_comb);
}

ReductionStatus
Sequence(TermNode& term, ContextNode& ctx)
{
	RetainList(term);
	RemoveHead(term);
	return ReduceOrdered(term, ctx);
}

ReductionStatus
ListAsterisk(TermNode& term)
{
	auto i(ListAsteriskHead(term));
	const auto last(term.end());

	while(i != last)
		LiftToReturn(*i++);
	return ListAsteriskTail(term);
}

ReductionStatus
ListAsteriskRef(TermNode& term)
{
	ListAsteriskHead(term);
	return ListAsteriskTail(term);
}

ReductionStatus
AccL(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 6);
	// XXX: This preprocessing binds all arguments but the abstract list object.
	//	It is necessary for recursive calls, but redundant for the non-recursive
	//	case. Implement it here for better overall performance as non-recursive
	//	cases are relatively rare.
	BindMoveNextNLocalSubobjectInPlace(std::next(term.begin()), 5);
	// NOTE: This is for the store of lvalue list.
	term.emplace();
	return ReduceAccL(term, ctx);
}

ReductionStatus
AccR(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 6);
	// XXX: Ditto.
	BindMoveNextNLocalSubobjectInPlace(std::next(term.begin()), 5);

	auto& rterm(*term.emplace(ystdex::exchange(term.GetContainerRef(),
		TermNode::Container(term.get_allocator()))));

	// NOTE: Save 'sum'.
	PrepareTailOp(term, ctx, *rterm.rbegin());
	// NOTE: These are for the store of %n2term and the lvalue list.
	rterm.emplace();
	rterm.emplace();
	return ReduceLiftSum(term, ctx, rterm, ReduceAccR);
}

ReductionStatus
FoldR1(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 3);

	auto i(term.begin());

	BindMoveLocalObjectInPlace(*++i);
	BindMoveLocalObjectInPlace(*++i);
	return ReduceFoldRMap1(term, ctx, ReduceFoldR1);
}

ReductionStatus
Map1(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 2);

	auto i(term.begin());

	BindMoveLocalObjectInPlace(*++i);
	return ReduceFoldRMap1(term, ctx, ReduceMap1);
}

ReductionStatus
ListConcat(TermNode& term)
{
	RetainN(term, 2);
	RemoveHead(term);

	auto i(term.begin());
	auto& x(*i);
	auto& y(*++i);
	auto& ycon(y.GetContainerRef());

	LiftToReturn(y);
	PrependList(ycon, x);
	LiftOtherValue(term, y);
	return ReductionStatus::Retained;
}

ReductionStatus
Append(TermNode& term)
{
	RetainList(term);
#if true
	// XXX: This is likely a bit more efficient.
	RemoveHead(term);
	term.emplace(ystdex::exchange(term.GetContainerRef(),
		TermNode::Container(term.get_allocator())));
#else

	{
		auto i(term.begin());
		auto& tm(*i);

		tm.Clear();
		tm.GetContainerRef().splice(tm.end(), term.GetContainerRef(), ++i,
			term.end());
	}
#endif
	// NOTE: The subterm is %ls.
	YAssert(term.size() == 1, "Invalid call found.");

	auto& con(term.GetContainerRef());
	auto& ls(con.back());

	// XXX: This should have been guaranteed by preconditions in the caller
	//	site.
	YAssert(IsList(ls), "Invalid non-list term found.");
	if(IsBranch(ls))
	{
		auto& lcon(ls.GetContainerRef());
		TermNode::Container tcon(con.get_allocator());

		for(auto i(lcon.rbegin()); i != lcon.rend(); ++i)
			// NOTE: The source list %ls is a prvalue. Copy or move the 1st
			//	subterm of the source directly.
			// XXX: Terms are treated always movable without check for
			//	%TermTags::Nonmodifying, respecting copy elision with 'first%'.
			// NOTE: As %FirstFwd. There should ne no cycle in the caller sites,
			//	so %nterm and the 1st subterm of %ls is not the same.
			// XXX: Term tags are currently not respected in prvalues.
			PrependList(tcon, *i);
		con = std::move(tcon);
		return ReductionStatus::Retained;
	}
	term.Clear();
	return ReductionStatus::Regular;
}

ReductionStatus
ListExtractFirst(TermNode& term)
{
	PrepareListExtract(term);
	return ReduceListExtractFirst(term);
}

ReductionStatus
ListExtractRestFwd(TermNode& term)
{
	PrepareListExtract(term);
	return ReduceListExtractRestFwd(term);
}

ReductionStatus
ListPushFront(TermNode& term)
{
	RetainN(term, 2);

	auto i(term.begin());
	auto& l(*++i);

	ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		if(!p_ref || p_ref->IsModifiable())
		{
			if(IsList(nd))
			{
				auto& x(*++i);

				if(const auto p
					= TryAccessLeafAtom<const TermReference>(x))
				{
					if(!p->IsReferencedLValue())
						LiftMovedOther(x, *p, p->IsMovable());
				}
				nd.GetContainerRef().splice(nd.begin(), term.GetContainerRef(),
					i);
			}
			else
				ThrowListTypeErrorForNonList(nd, p_ref);
		}
		else
			throw TypeError("Modifiable object expected.");
	}, l);
	return ReduceReturnUnspecified(term);
}

ReductionStatus
Let(TermNode& term, ContextNode& ctx)
{
	return LetOrRecImpl(term, ctx, LetCombineApplicative, {});
}

ReductionStatus
LetRef(TermNode& term, ContextNode& ctx)
{
	return LetOrRecImpl(term, ctx, LetCombineApplicative, true);
}

ReductionStatus
LetWithEnvironment(TermNode& term, ContextNode& ctx)
{
	return LetOrRecImpl(term, ctx, LetCombineApplicative, {}, true);
}

ReductionStatus
LetWithEnvironmentRef(TermNode& term, ContextNode& ctx)
{
	return LetOrRecImpl(term, ctx, LetCombineApplicative, true, true);
}

ReductionStatus
LetAsterisk(TermNode& term, ContextNode& ctx)
{
	return LetAsteriskImpl(term, ctx, {});
}

ReductionStatus
LetAsteriskRef(TermNode& term, ContextNode& ctx)
{
	return LetAsteriskImpl(term, ctx, true);
}

ReductionStatus
LetRec(TermNode& term, ContextNode& ctx)
{
	return LetOrRecImpl(term, ctx, LetCombineRec, {});
}

ReductionStatus
LetRecRef(TermNode& term, ContextNode& ctx)
{
	return LetOrRecImpl(term, ctx, LetCombineRec, true);
}

ReductionStatus
AsEnvironment(TermNode& term, ContextNode& ctx)
{
	Retain(term);
	RemoveHead(term);

	// NOTE: As %LetEmpty.
	return LetEmptyGuardTail([&](EnvironmentGuard& gd){
		// XXX: As %LetCall.
		AssertNextTerm(ctx, term);
		// NOTE: The lifting is not enabled because the result is directly
		//	specified as a prvalue in the implementation.
		return TailCall::RelayNextGuarded(ctx, term, std::move(gd),
			[&](TermNode&, ContextNode&){
			return RelaySubsequent(ctx, term, A1::NameTypedReducerHandler(
				// TODO: Blocked. Use C++14 lambda initializers to simplify the
				//	implementation.
				ystdex::bind1([&](ContextNode&, shared_ptr<Environment>& p_e){
				term.ClearContainer(),
				term.SetValue(std::move(p_e));
				return ReductionStatus::Regular;
			}, ctx.ShareRecord()), "as-environment-return"));
		});
	}, term, ctx);
}

ReductionStatus
BindingsWithParentToEnvironment(TermNode& term, ContextNode& ctx)
{
	RetainList(term);
	CheckVariadicArity(term, 0);

	auto i(term.begin());
	auto& eterm(*++i);

	if(IsList(eterm))
		// XXX: Calling to %NPL::ToReducer can be a bit more efficient here.
		return A1::RelayCurrentNext(ctx, eterm, trivial_swap,
			// NOTE: Capture the term regardless of the next term because
			//	continuation capture here is unsupported.
			[&](TermNode&, ContextNode& c){
			ReduceChildren(eterm, c);
			return ReductionStatus::Partial;
		}, NPL::ToReducer(ctx.get_allocator(), trivial_swap,
			A1::NameTypedReducerHandler([&]{
			auto p_env(CreateEnvironment(eterm));
			auto& con(term.GetContainerRef());

			// NOTE: Optimize for cases of no bindings.
			if(con.size() > 2)
			{
				auto j(con.begin());
				auto& operand(*j);

				operand.Clear(),
				// NOTE: Reuse %eterm as 'bindings'.
				eterm.ClearContainer();
				return DoBindingsToEnvironment(term, ctx, p_env, ++++j,
					eterm, operand);
			}
			con.pop_front();
			return ReduceMoveEnv1(term, p_env);
		}, "eval-bindings-to-env")));
	ThrowInsufficientTermsError(eterm, true);
}

ReductionStatus
BindingsToEnvironment(TermNode& term, ContextNode& ctx)
{
	RetainList(term);

	auto& con(term.GetContainerRef());
	auto p_env(NPL::AllocateEnvironment(con.get_allocator()));

	// NOTE: Optimize for cases of no bindings.
	if(con.size() > 1)
	{
		auto i(con.begin());
		auto& bindings(*i);

		bindings.Clear();
		return DoBindingsToEnvironment(term, ctx, p_env, ++i, bindings,
			con.emplace_front());
	}
	return ReduceMoveEnv1(term, p_env);
}

ReductionStatus
SymbolsToImports(TermNode& term)
{
	RetainN(term);
	RemoveHead(term);
	return ResolveTerm(
		[&](TermNode& nd, ResolvedTermReferencePtr p_ref) -> ReductionStatus{
		if(IsList(nd))
		{
			if(IsBranch(nd))
			{
				auto& con(term.GetContainerRef());
				const auto a(con.get_allocator());
				const auto add_val([&](TokenValue s){
					con.push_back(NPL::AsTermNode(a, std::move(s)));
				});

				if(NPL::IsMovable(p_ref))
					for(auto& x : nd)
					{
						auto& s(NPL::ResolveRegular<TokenValue>(x));

						add_val(
							HasImportsSigil(s) ? s.substr(1) : std::move(s));
					}
				else
					for(const auto& x : nd)
						add_val(CopyImportName(
							NPL::ResolveRegular<const TokenValue>(x)));
				con.front() = A1::AsForm(a, [](TermNode& t){
					Retain(t);
					RemoveHead(t);
					YAssert(IsList(t), "Invalid term found.");
					for(auto& x : t)
						// NOTE: As %MoveRValueToForward, but with same term to
						//	be lifted.
						if(const auto p
							= TryAccessLeafAtom<const TermReference>(x))
							if(!p->IsReferencedLValue())
								LiftOtherOrCopy(x, p->get(), p->IsModifiable());
					return ReductionStatus::Retained;
				}, Strict);
				return ReductionStatus::Retained;
			}
			term.ClearContainer();
			YAssert(IsEmpty(term), "Invalid term found.");
			return ReductionStatus::Regular;
		}
		ThrowListTypeErrorForNonList(nd, true);
	}, *term.begin());
}

ReductionStatus
ProvideLet(TermNode& term, ContextNode& ctx)
{
	CheckVariadicArity(term, 1);

	// XXX: As %LetOrRecImpl.
	auto i(term.begin());
	auto& forwarded(*i);
	auto& bindings(*++++i);

	// TODO: Optimize the case of empty binding lists, as %LetEmpty.
	// NOTE: As %LetOrRecImpl.
	if(const auto p = TryAccessLeafAtom<TermReference>(bindings))
	{
		auto& nd(p->get());

		if(p->IsMovable())
			LetExpireChecked(forwarded, nd, p->GetEnvironmentReference(),
				p->GetTags());
		else if(IsList(nd))
			yunseq(forwarded.Value = TermRange(nd, p->GetTags()),
				forwarded.Tags = TermTags::Nonmodifying);
		else
			ThrowInsufficientTermsError(nd, true);
	}
	else
		LetExpireChecked(forwarded, bindings, ctx.GetRecordPtr(),
			bindings.Tags);
	// XXX: As %LetBindingsExtract.
	ExtractBindings(*term.GetContainerRef().emplace(i), forwarded);
	ProvideLetCommon(term, ctx);
	// XXX: As %LetCommon.
	return LetCombineApplicative(term, ctx, true);
}

ReductionStatus
Provide(TermNode& term, ContextNode& ctx)
{
	CheckVariadicArity(term, 0);

	// XXX: As %LetEmpty.
	auto& con(term.GetContainerRef());
	auto i(con.begin());

	i->Clear();
	con.emplace(++++i);
	con.emplace(i);
	ProvideLetCommon(term, ctx);
	// XXX: As %LetEmpty.
	return LetCombineEmpty(term, ctx, true);
}

ReductionStatus
Import(TermNode& term, ContextNode& ctx)
{
	return ImportImpl(term, ctx, {});
}

ReductionStatus
ImportRef(TermNode& term, ContextNode& ctx)
{
	return ImportImpl(term, ctx, true);
}

ReductionStatus
Assq(TermNode& term)
{
	return AssocImpl(term, YSLib::HoldSame);
}

ReductionStatus
Assv(TermNode& term)
{
	return AssocImpl(term, [] YB_LAMBDA_ANNOTATE(
		(const ValueObject& x, const ValueObject& y), , pure){
		return x == y;
	});
}


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
		A1::Continuation(A1::NameTypedContextHandler(
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
		// XXX: This is not guarded. If any exception is throw, the values are
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
		return WrapO(term, cont.Handler, p_ref);
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
		// NOTE: As %ContinuationToApplicative and %ApplyImpl.
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
		t.GetContainerRef().splice(t.begin(), term.GetContainerRef(), i_comb);
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

