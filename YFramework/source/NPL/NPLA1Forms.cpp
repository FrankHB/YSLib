/*
	© 2016-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Forms.cpp
\ingroup NPL
\brief NPLA1 语法形式。
\version r19632
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2014-02-15 11:19:51 +0800
\par 修改时间:
	2020-11-22 12:51 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1Forms
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1Forms // for YSLib, std::next, ReduceOnceLifted,
//	NPL::IsMovable, function, NPL::TryAccessReferencedTerm, ystdex::value_or,
//	ThrowInsufficientTermsError, NPL::Deref, A1::NameTypedReducerHandler,
//	ReduceReturnUnspecified, RemoveHead, IsBranch, AccessFirstSubterm,
//	ReduceNextCombinedBranch, std::placeholders, ystdex::as_const, IsLeaf,
//	ystdex::ref_eq, ContextHandler, shared_ptr, string, unordered_map,
//	Environment, lref, list, IsBranchedList, TokenValue, NPL::TryAccessLeaf,
//	ValueObject, weak_ptr, any_ops::use_holder, in_place_type, ystdex::type_id,
//	YSLib::allocate_shared, InvalidReference, MoveFirstSubterm, ShareMoveTerm,
//	ThrowInvalidSyntaxError, ReduceCombinedBranch, ResolvedTermReferencePtr,
//	LiftOtherOrCopy, ResolveTerm, ystdex::equality_comparable,
//	std::allocator_arg, NPL::AsTermNode, ystdex::exchange,
//	NPL::SwitchToFreshEnvironment, TermTags, ystdex::expand_proxy,
//	NPL::AccessRegular, TermReference, GetLValueTagsOf, RegularizeTerm,
//	ThrowValueCategoryError, ThrowListTypeErrorForNonlist, ystdex::update_thunk,
//	NPL::TryAccessReferencedLeaf, ystdex::invoke_value_or,
//	ystdex::call_value_or, RelaySwitched, LiftMovedOther, LiftCollapsed,
//	ystdex::make_transform, NPL::AllocateEnvironment, NPL::TryAccessTerm,
//	std::mem_fn;
#include "NPLA1Internals.h" // for A1::Internals API;
#include YFM_NPL_SContext // for Session;

using namespace YSLib;

namespace NPL
{

//! \since build 863
//@{
#ifndef NDEBUG
#	define NPL_Impl_NPLA1_TraceVauCall true
#else
#	define NPL_Impl_NPLA1_TraceVauCall false
#endif
//@}

namespace A1
{

//! \since build 685
namespace
{

//! \since build 868
//@{
using Forms::CallRegularUnaryAs;
//! \since build 874
using Forms::CallResolvedUnary;
using Forms::CallUnary;
using Forms::Retain;
using Forms::RetainN;
//@}


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


//! \since build 876
YB_ATTR_nodiscard YB_PURE bool
ExtractBool(const TermNode& term)
{
	return ystdex::value_or(NPL::TryAccessReferencedTerm<bool>(term), true);
}

//! \since build 860
//@{
YB_ATTR_nodiscard TNIter
CondClauseCheck(TermNode& clause)
{
	if(YB_UNLIKELY(clause.empty()))
		ThrowInsufficientTermsError(clause, {});
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
		auto& clause(NPL::Deref(i));
		auto j(CondClauseCheck(clause));

		// NOTE: This also supports TCO.
		return ReduceSubsequent(NPL::Deref(j), ctx,
			A1::NameTypedReducerHandler([&, i, j](ContextNode& c){
			if(CondTest(clause, j))
				return ReduceOnceLifted(term, c, clause);
			return CondImpl(term, c, std::next(i));
		}, "eval-cond-list"));
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
	else
		ThrowInsufficientTermsError(term, {});
}

ReductionStatus
And2(TermNode& term, ContextNode& ctx, TNIter i)
{
	if(ExtractBool(NPL::Deref(i)))
	{
		term.Remove(i);
		return ReduceNextCombinedBranch(term, ContextState::Access(ctx));
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
		return ReduceNextCombinedBranch(term, ContextState::Access(ctx));
	}
	LiftOther(term, tm);
	return ReductionStatus::Retained;
}

ReductionStatus
AndOr(TermNode& term, ContextNode& ctx,
	ReductionStatus (&and_or)(TermNode&, ContextNode&, TNIter))
{
	Retain(term);

	auto i(term.begin());

	if(++i != term.end())
		return std::next(i) == term.end() ? ReduceOnceLifted(term, ctx, *i)
			: ReduceSubsequent(*i, ctx,
			A1::NameTypedReducerHandler(std::bind(and_or, std::ref(term),
			std::placeholders::_1, i), "eval-booleans"));
	term.Value = and_or == And2;
	return ReductionStatus::Clean;
}
//@}

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
		return ReferenceTerm(x).Value;
	});
}

template<typename _func>
void
EqTermReference(TermNode& term, _func f)
{
	EqTermRet(term, [f](const TermNode& x, const TermNode& y) YB_PURE{
		return IsLeaf(x) && IsLeaf(y) ? f(x.Value, y.Value)
			: ystdex::ref_eq<>()(x, y);
	}, static_cast<const TermNode&(&)(const TermNode&)>(ReferenceTerm));
}

YB_PURE inline PDefH(bool, TermUnequal, const TermNode& x, const TermNode& y)
	ImplRet(x.size() != y.size() || x.Value != y.Value)

ReductionStatus
EqualSubterm(bool& r, ContextNode& ctx, bool orig, TNCIter first1,
	TNCIter first2, TNCIter last1)
{
	if(first1 != last1)
	{
		auto& x(ReferenceTerm(*first1));
		auto& y(ReferenceTerm(*first2));

		if(TermUnequal(x, y))
		{
			r = {};
			return ReductionStatus::Clean;
		}
		yunseq(++first1, ++first2);
		RelaySwitched(ctx, [&, first1, first2, last1]{
			return EqualSubterm(r, ctx, {}, first1, first2, last1);
		});
		return RelaySwitched(ctx, [&]{
			return
				EqualSubterm(r, ctx, {}, x.begin(), y.begin(), x.end());
		});
	}
	return orig ? ReductionStatus::Clean : ReductionStatus::Partial;
}
//@}


// NOTE: See $2019-03 @ %Documentation::Workflow.
//! \since build 782
class RecursiveThunk final
{
private:
	//! \since build 784
	using shared_ptr_t = shared_ptr<ContextHandler>;
	//! \since build 784
	unordered_map<string, shared_ptr_t> store{};

public:
	//! \since build 894
	shared_ptr<Environment> RecordPtr;
	//! \since build 893
	lref<const TermNode> TermRef;

	//! \since build 894
	RecursiveThunk(const shared_ptr<Environment>& p_env, const TermNode& t)
		ynothrow
		: RecordPtr(p_env), TermRef(t)
	{
		Fix(RecordPtr, TermRef);
	}
	//! \since build 841
	DefDeMoveCtor(RecursiveThunk)

	//! \since build 841
	DefDeMoveAssignment(RecursiveThunk)

private:
	//! \since build 894
	void
	Fix(const shared_ptr<Environment>& p_env, const TermNode& t)
	{
		auto& env(NPL::Deref(p_env));

		if(IsBranchedList(t))
			for(const auto& tm : t)
				Fix(p_env, tm);
		else if(const auto p = NPL::TryAccessLeaf<TokenValue>(t))
		{
			const auto& n(*p);

			// XXX: This is served as the addtional static environment.
			CheckParameterLeafToken(n, [&]{
				// NOTE: The symbol can be bound more than once. Only one
				//	instance is supported.
				if(store.find(n) == store.cend())
					// NOTE: The bound symbol can then be rebound to an ordinary
					//	(non-sharing) object.
					env.Bind(n, TermNode(TermNode::Container(t.get_allocator()),
						ValueObject(any_ops::use_holder, in_place_type<
						HolderFromPointer<shared_ptr_t>>,
						store[n] = YSLib::allocate_shared<ContextHandler>(
						t.get_allocator(), ThrowInvalidCyclicReference))));
			});
		}
	}

	//! \since build 780
	YB_NORETURN static ReductionStatus
	ThrowInvalidCyclicReference(TermNode&, ContextNode&)
	{
		throw InvalidReference("Invalid cyclic reference found.");
	}
};


//! \since build 894
YB_ATTR_nodiscard inline PDefH(const shared_ptr<Environment>&,
	FetchDefineOrSetEnvironment, ContextNode& ctx) ynothrow
	ImplRet(ctx.GetRecordPtr())
//! \since build 894
YB_ATTR_nodiscard inline PDefH(const shared_ptr<Environment>&,
	FetchDefineOrSetEnvironment, ContextNode&,
	const shared_ptr<Environment>& p_env)
	ImplRet(Environment::EnsureValid(p_env), p_env)

//! \since build 904
YB_NORETURN inline void
ThrowInsufficientTermsErrorFor(const TermNode& term, InvalidSyntax&& e)
{
	TryExpr(ThrowInsufficientTermsError(term, {}))
	CatchExpr(..., std::throw_with_nested(std::move(e)))
}

//! \since build 899
void
BindParameterChecked(const shared_ptr<Environment>& p_env, const TermNode& t,
	TermNode& o)
{
	if(!p_env->Frozen)
		BindParameter(p_env, t, o);
	else
		throw TypeError("Cannot define variables in a frozen environment.");
}

//! \since build 868
//@{
template<typename _func>
auto
DoDefineSet(TermNode& term, size_t n, _func f) -> decltype(f())
{
	Retain(term);
	if(term.size() > n)
		return f();
	ThrowInsufficientTermsErrorFor(term,
		InvalidSyntax("Invalid syntax found in definition."));
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
		return ReduceSubsequent(term, ctx,
			A1::NameTypedReducerHandler(std::bind([&](const TermNode& saved,
			const shared_ptr<Environment>& p_e, const _tParams&...){
			BindParameterChecked(p_e, saved, term);
			return ReduceReturnUnspecified(term);
		}, std::move(formals), std::move(p_env), std::move(args)...),
			"match-ptree"));
#else
		yunseq(0, args...);
		// NOTE: This does not support PTC.
		CallImpl(term, ctx, p_env, formals);
		return ReduceReturnUnspecified(term);
#endif
	}

#if !NPL_Impl_NPLA1_Enable_Thunked
	//! \since build 894
	static void
	CallImpl(TermNode& term, ContextNode& ctx,
		const shared_ptr<Environment>& p_env, TermNode& formals)
	{
		ReduceOnce(term, ctx);
		BindParameterChecked(p_env, formals, term);
	}
#endif
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
#if NPL_Impl_NPLA1_Enable_Thunked
		// XXX: Terms shall be moved and saved into the actions.
		auto p_saved(ShareMoveTerm(formals));

		// TODO: Avoid %shared_ptr.
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		return ReduceSubsequent(term, ctx,
			A1::NameTypedReducerHandler(std::bind([&](const
			shared_ptr<TermNode>&, const shared_ptr<RecursiveThunk>& p_gd,
			const _tParams&...){
			BindParameterChecked(p_gd->RecordPtr, p_gd->TermRef, term);
			// NOTE: This support PTC only when the thunk cleanup is not existed
			//	at the tail context.
			return ReduceReturnUnspecified(term);
		}, std::move(p_saved), YSLib::allocate_shared<RecursiveThunk>(
			term.get_allocator(), std::move(p_env), *p_saved),
			std::move(args)...), "match-ptree-recursive"));
#else
		// NOTE: This does not support PTC.
		RecursiveThunk gd(std::move(p_env), formals);

		yunseq(0, args...);
		DoDefineOrSet<>::CallImpl(term, ctx, gd.RecordPtr, formals);
		// NOTE: This support PTC only when the thunk cleanup is not existed at
		//	the tail context.
		return ReduceReturnUnspecified(term);
#endif
	}
};


template<bool _bRecur = false>
struct DefineOrSetDispatcher final
{
	TermNode& TermRef;
	ContextNode& Context;

	template<typename... _tParams>
	inline ReductionStatus
	operator()(TermNode& formals, _tParams&&... args) const
	{
		return DoDefineOrSet<_bRecur>::Call(TermRef, Context,
			FetchDefineOrSetEnvironment(Context, args...), formals,
			yforward(args)...);
	}
};
//@}


//! \since build 860
//@{
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsIgnore, const TokenValue& s) ynothrow
	// XXX: This is more efficient than cast to %basic_string_view if the
	//	%basic_string implementation is optimized.
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


//! \since build 898
//@{
// XXX: Same to %RelayForEvalOrDirect, except that %no_lift is always %true, and
//	no TCO is enforced.
template<typename _fNext>
YB_FLATTEN inline ReductionStatus
RelayNextGuarded(ContextNode& ctx, TermNode& term, EnvironmentGuard&& gd,
	_fNext&& next)
{
	// XXX: See %RelayForEvalOrDirect.
#if NPL_Impl_NPLA1_Enable_Thunked
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.	
	return A1::RelayCurrentNext(term, ctx, yforward(next), MakeMoveGuard(gd));
#else
	yunused(gd);
	return A1::RelayDirect(ctx, next, term);
#endif
}

// XXX: Same to %RelayForEvalOrDirect, except that %no_lift is always %true.
template<typename _fNext>
YB_FLATTEN inline ReductionStatus
RelayNextGuardedTail(ContextNode& ctx, TermNode& term,
	EnvironmentGuard&& gd, _fNext&& next)
{
	// XXX: See %RelayForEvalOrDirect.
#if NPL_Impl_NPLA1_Enable_TCO
	PrepareTCOEvaluation(ctx, term, std::move(gd));
	return A1::RelayCurrentOrDirect(ctx, yforward(next), term);
#else
	return A1::RelayNextGuarded(ctx, term, std::move(gd), yforward(next));
#endif
}

// XXX: A combination of an operative and its operand shall always be evaluated
//	in a tail context. However, for a combiner call, sometimes it is not in the
//	tail context in the enclosing context. If the call needs no creation of
//	%TCOAction (e.g. fully implemented native code), avoiding calling
//	%PrepareTCOEvaluation needs no precondition of the existence of %TCOAction,
//	and this can be an optimization.
#if false
YB_FLATTEN inline ReductionStatus
RelayCombined(ContextNode& ctx, TermNode& term, shared_ptr<Environment> p_env)
{
	// NOTE: %ReduceFirst and other passes would not be called again. The
	//	unwrapped combiner should have been evaluated and it would not be
	//	wrongly evaluated again.
	// XXX: Consider optimization when the combiner subterm is known regular.
	// NOTE: Term is set in %A1::RelayNextGuarded, even for direct
	//	inlining call of %ReduceCombinedBranch.
	return A1::RelayNextGuarded(ctx, term,
		EnvironmentGuard(ctx, ctx.SwitchEnvironment(std::move(p_env))),
		std::ref(ReduceCombinedBranch));
}
#endif

YB_FLATTEN inline ReductionStatus
RelayCombinedTail(ContextNode& ctx, TermNode& term,
	shared_ptr<Environment> p_env)
{
	// NOTE: See %RelayCombined.
	// NOTE: Term is set in %A1::RelayNextGuardedTail, even for direct
	//	inlining call of %ReduceCombinedBranch.
	return A1::RelayNextGuardedTail(ctx, term,
		EnvironmentGuard(ctx, ctx.SwitchEnvironment(std::move(p_env))),
		std::ref(ReduceCombinedBranch));
}
//@}

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
	Session sess(ctx.get_allocator());

	SContext::Analyze(std::allocator_arg, ctx.get_allocator(), sess,
		sess.Process(NPL::ResolveRegular<const string>(expr)))
		.SwapContainer(expr);
	return EvalImplUnchecked(term, ctx, no_lift);
}
//@}


//! \since build 897
YB_ATTR_nodiscard YB_PURE inline
	PDefH(InvalidSyntax, MakeFunctionAbstractionError, ) ynothrow
	ImplRet(InvalidSyntax("Invalid syntax found in function abstraction."))


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
		NPL::AsTermNode(term.get_allocator())))), p_call(eformal.empty()
		? &VauHandler::CallStatic : &VauHandler::CallDynamic), NoLifting(nl)
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
			// XXX: The call has been performed on the handler, see the notes in
			//	%DoCall.
			throw NPLException("Invalid handler of call found.");
		}
		throw LoggedEvent("Invalid composition found.", Alert);
	}

private:
	//! \since build 847
	//@{
	void
	BindEnvironment(ContextNode& ctx, ValueObject&& vo) const
	{
		YAssert(!eformal.empty(), "Empty dynamic environment name found.");
		// NOTE: Bound the dynamic environment.
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

public:
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
	ReductionStatus
	DoCall(TermNode& term, ContextNode& ctx, EnvironmentGuard& gd) const
	{
		YAssert(p_eval_struct, "Invalid evaluation structure found.");

		const bool move(p_eval_struct.use_count() == 1
			&& bool(term.Tags & TermTags::Temporary));

		// NOTE: Since the first term is expected to be saved (e.g. by
		//	%ReduceCombined), it is safe to be removed directly.
		RemoveHead(term);
		// NOTE: Forming beta-reducible terms using parameter binding, to
		//	substitute them as arguments for later closure reduction.
		// XXX: Do not lift terms if provable to be safe?
		// NOTE: Since now binding does not rely on temporaries stored elsewhere
		//	(by using %TermTags::Temporary instead), this should be safe even
		//	with TCO.
		// NOTE: The environment is assumed not frozen, so no need to use
		//	%BindParameterChecked.
		BindParameter(ctx.GetRecordPtr(), NPL::Deref(p_formals), term);
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
YB_NORETURN ReductionStatus
ThrowForUnwrappingFailure(const ContextHandler& h)
{
	throw TypeError(ystdex::sfmt("Unwrapping failed with type '%s'.",
		h.target_type().name()));
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
	return ystdex::expand_proxy<ReductionStatus(ContextHandler&,
		const ResolvedTermReferencePtr&)>::call(f2, h, p_ref);
}

template<typename _func, typename _func2>
ReductionStatus
WrapUnwrapResolve(TermNode& term, _func f, _func2 f2)
{
	return NPL::ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		return DispatchContextHandler(
			NPL::AccessRegular<ContextHandler>(nd, p_ref), p_ref, f, f2);
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
	//	%BindParameterObject::operator().
	if(const auto p = NPL::TryAccessLeaf<TermReference>(o))
		// NOTE: Reference collapsed by move.
		mv(std::move(o.GetContainerRef()),
			TermReference(BindReferenceTags(*p), std::move(*p)));
	else
		// NOTE: As %MarkTemporaryTerm in %NPLA1.
		(mv(std::move(o.GetContainerRef()),
			std::move(o.Value))).get().Tags |= TermTags::Temporary;
}

void
BindMoveLocalObjectInPlace(TermNode& o)
{
	// NOTE: Simplified from 'sigil == '&' and 'can_modify && temp' branch in
	//	%BindParameterObject::operator().
	if(const auto p = NPL::TryAccessLeaf<TermReference>(o))
		// NOTE: Reference collapsed by move.
		p->SetTags(BindReferenceTags(*p));
	else
		// NOTE: As %MarkTemporaryTerm in %NPLA1.
		o.Tags |= TermTags::Temporary;
}

void
BindMoveNextNLocalSubobjectInPlace(TNIter i, size_t n)
{
	while(n-- != 0)
		BindMoveLocalObjectInPlace(*++i);
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
		if(const auto p = NPL::TryAccessLeaf<TermReference>(o))
			return TermReference(BindReferenceTags(*p), *p);
		return TermReference(GetLValueTagsOf(o.Tags | o_tags), o, r_env);
	}());
}

YB_ATTR_nodiscard YB_ATTR(always_inline) inline TermNode
EvaluateToLValueReference(TermNode& term, const shared_ptr<Environment>& p_env)
{
	return NPL::AsTermNode(term.get_allocator(), EnsureLValueReference(
		TermReference(term.Tags, term, NPL::Nonnull(p_env))));
}
//@}

#if false
// NOTE: The bound term cannot be reused later because %term can be the
//	referent.
//! \since build 894
YB_ATTR_nodiscard TermNode
EvaluateBoundLValue(TermNode& term, const shared_ptr<Environment>& p_env)
{
	if(const auto p = NPL::TryAccessLeaf<TermReference>(term))
		return TermNode(std::allocator_arg, term.get_allocator(),
			term.GetContainer(), EnsureLValueReference(*p));
	return EvaluateToLValueReference(term, p_env);
}

//! \since build 897
//@{
// NOTE: The bound term cannot be reused later because %term can be the
//	referent.
YB_ATTR_nodiscard TermNode
EvaluateBoundLValueMoved(TermNode& term, const shared_ptr<Environment>& p_env)
{
	if(const auto p = NPL::TryAccessLeaf<TermReference>(term))
	{
		*p = EnsureLValueReference(std::move(*p));
		return std::move(term);
	}
	return EvaluateToLValueReference(term, p_env);
}
#endif

// NOTE: See %BindMoveLocalObjectInPlace and %EvaluateBoundLValue.
//! \since build 898
YB_ATTR_nodiscard TermNode
EvaluateLocalObject(TermNode& o, const shared_ptr<Environment>& p_env)
{
	// NOTE: Make unique reference as bound temporary object.
	if(const auto p = NPL::TryAccessLeaf<TermReference>(o))
	{
		p->SetTags(BindReferenceTags(*p));
		return TermNode(std::allocator_arg, o.get_allocator(),
			o.GetContainer(), EnsureLValueReference(*p));
	}
	o.Tags |= TermTags::Temporary;
	return EvaluateToLValueReference(o, p_env);
}

// NOTE: See %BindMoveLocalObjectInPlace and %EvaluateBoundLValueMoved.
YB_ATTR_nodiscard TermNode
EvaluateLocalObjectMoved(TermNode& o, const shared_ptr<Environment>& p_env)
{
	// NOTE: Make unique reference as bound temporary object.
	if(const auto p = NPL::TryAccessLeaf<TermReference>(o))
	{
		// NOTE: See also %EnsureLValueReference.
		*p = TermReference(BindReferenceTags(*p) & ~TermTags::Unique,
			std::move(*p));
		return std::move(o);
	}
	o.Tags |= TermTags::Temporary;
	return EvaluateToLValueReference(o, p_env);
}

void
ForwardToUnwrapped(TermNode& comb, ContextNode& ctx)
{
	using namespace std::placeholders;

	RegularizeTerm(comb, WrapUnwrapResolve(comb, std::bind(UnwrapResolved,
		std::ref(comb), std::ref(ctx), _1, _2), ThrowForUnwrappingFailure));
}

//! \since build 899
//@{
// XXX: Preserving %con is better for performance, at least in code generation
//`by x86_64-pc-linux G++ 10.2.
YB_ATTR_nodiscard TermNode
EvaluateBoundUnwrappedLValueDispatch(TermNode::allocator_type a,
	const TermNode::Container& con, TermReference ref, ContextNode& ctx,
	TermNode& nd)
{
	auto& h(NPL::AccessRegular<ContextHandler>(nd, true));

	if(const auto p = h.target<FormContextHandler>())
	{
		auto& fch(*p);

		if(fch.Wrapping != 0)
		{
			TermNode term(std::allocator_arg, a, con);

			ReduceForCombinerRef(term, ctx, ref, fch.Handler, fch.Wrapping - 1);
			return term;
		}
		throw TypeError("Unwrapping failed on an operative argument.");
	}
	ThrowForUnwrappingFailure(h);
}

// NOTE: As %EvaluateBoundLValue and %ForwardToUnwrapped.
YB_ATTR_nodiscard TermNode
EvaluateBoundLValueUnwrapped(TermNode& term, ContextNode& ctx,
	const shared_ptr<Environment>& p_env)
{
	if(const auto p = NPL::TryAccessLeaf<TermReference>(term))
		return EvaluateBoundUnwrappedLValueDispatch(
			term.get_allocator(), term.GetContainer(), *p, ctx, p->get());
	return EvaluateBoundUnwrappedLValueDispatch(term.get_allocator(),
		{}, TermReference(term.Tags, term, NPL::Nonnull(p_env)), ctx, term);
}
//@}

//! \since build 875
//@{
// NOTE: As %ReduceSubsequent.
// NOTE: This does not guarantee PTC.
template<typename _fCurrent>
ReductionStatus
ReduceCallSubsequent(TermNode& term, ContextNode& ctx,
	shared_ptr<Environment> p_env, _fCurrent&& next)
{
	using namespace std::placeholders;

	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	return A1::ReduceCurrentNext(term, ctx,
		std::bind([](shared_ptr<Environment>& p_e, TermNode& t, ContextNode& c){
#if true
		// NOTE: This does not rely on the existence of the %TCOAction, as it
		//	will call %EnsureTCOAction. Since the call would rely on %TCOAction,
		//	anyway, creating the TCO action here instead of a new action
		//	to restore the environment is more efficient.
		SetupTailContext(c, t);
		return RelayCombinedTail(c, t, std::move(p_e));
#else
		return RelayCombined(c, t, std::move(p_e));
#endif
	}, std::move(p_env), _1, _2), yforward(next));
}

#if false
//! \since build 898
template<typename _fCurrent>
ReductionStatus
ReduceCallSubsequentTail(TermNode& term, ContextNode& ctx,
	shared_ptr<Environment> p_env, _fCurrent&& next)
{
	using namespace std::placeholders;

	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	return A1::ReduceCurrentNext(term, ctx,
		std::bind([](shared_ptr<Environment>& p_e, TermNode& t, ContextNode& c){
		// NOTE: This does not calls %EnsureTCOAction, so an %TCOAction shall be
		//	already existed in the context.
		return RelayCombinedTail(c, t, std::move(p_e));
	}, std::move(p_env), _1, _2), yforward(next));
}
#endif


//! \since build 874
template<typename _func>
YB_FLATTEN ReductionStatus
FirstOrVal(TermNode& term, _func f)
{
	return CallResolvedUnary([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		if(IsBranchedList(nd))
			return f(AccessFirstSubterm(nd), p_ref);
		ThrowInsufficientTermsError(nd, p_ref);
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
			throw ListTypeError(
				ystdex::sfmt("Expected a non-empty list, got '%s'.",
				TermToStringWithReferenceMark(nd, true).c_str()));
	}
	else
		ThrowValueCategoryError(nd);
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
				ThrowListTypeErrorForNonlist(nd_y, p_ref_y);
		}, y);
	}, term);
}


//! \since build 899
pair<lref<Environment>, lref<const TokenValue>>
CheckToUndefine(TermNode& term, ContextNode& ctx)
{
	Retain(term);
	if(term.size() == 2)
	{
		const auto&
			n(NPL::ResolveRegular<const TokenValue>(*std::next(term.begin())));

		if(IsNPLASymbol(n))
		{
			auto& env(ctx.GetRecordRef());

			if(!env.Frozen)
				return {env, n};
			throw
				TypeError("Cannot remove a variable in a frozen environment.");
		}
		else
			ThrowInvalidSyntaxError(ystdex::sfmt("Invalid token '%s' found as"
				" name to be undefined.", n.c_str()));
	}
	else
		ThrowInvalidSyntaxError(
			"Expected exact one term as name to be undefined.");
}

//! \since build 888
template<typename _func>
inline auto
CheckFunctionCreation(_func f) -> decltype(f())
{
	TryRet(f())
	CatchExpr(..., std::throw_with_nested(MakeFunctionAbstractionError()))
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
	ThrowInsufficientTermsErrorFor(term, MakeFunctionAbstractionError());
}

ReductionStatus
LambdaImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return CreateFunction(term, [&, no_lift]{
		term.Value = CheckFunctionCreation([&]() -> ContextHandler{
			// NOTE: Protect the reference count of the environment at first.
			auto p_env(ctx.ShareRecord());
			auto i(term.begin());
			auto formals(ShareMoveTerm(NPL::Deref(++i)));

			term.erase(term.begin(), ++i);
			// NOTE: The wrapping count 1 implies strict evaluation of arguments
			//	in %FormContextHandler::operator().
			return FormContextHandler(VauHandler({},
				std::move(formals), std::move(p_env), {}, term, no_lift), 1);
		});
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
		term.Value = CheckFunctionCreation([&]{
			return
				CreateVau(term, no_lift, term.begin(), ctx.ShareRecord(), {});
		});
		return ReductionStatus::Clean;
	}, 2);
}

ReductionStatus
VauWithEnvironmentImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return CreateFunction(term, [&, no_lift]{
		auto i(term.begin());
		auto& tm(NPL::Deref(++i));

		return ReduceSubsequent(tm, ctx,
			A1::NameTypedReducerHandler([&, i, no_lift]{
			term.Value = CheckFunctionCreation([&]{
				// XXX: List components are ignored.
				auto p_env_pr(ResolveEnvironment(tm));

				return CreateVau(term, no_lift, i, std::move(p_env_pr.first),
					p_env_pr.second);
			});
			return ReductionStatus::Clean;
		}, "eval-vau"));
	}, 3);
}
//@}


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
		return ReduceForCombinerRef(term, ctx, *p_ref, fch.Handler, n);
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
	mutable TermNode TermRef;

	Encapsulation(shared_ptr<void> p, TermNode term)
		: EncapsulationBase(std::move(p)), TermRef(std::move(term))
	{}
	DefDeCopyMoveCtorAssignment(Encapsulation)

	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const Encapsulation& x, const Encapsulation& y) ynothrow
		ImplRet(x.Get() == y.Get()
			&& Equal(ReferenceTerm(x.TermRef), ReferenceTerm(y.TermRef)))

	using EncapsulationBase::Get;
	using EncapsulationBase::GetType;

private:
	//! \since build 904
	static bool
	Equal(const TermNode& x, const TermNode& y)
	{
		if(TermUnequal(x, y))
			return {};

		// NOTE: This is a trampoline to eliminate the call depth limitation.
		// XXX: This is not like %SeparatorPass in %NPLA1 inserting subnodes for
		//	each turn of the search which is not friendly to short-curcuit
		//	evaluation.
		bool r(true);
		Action act{};

		EqualSubterm(r, act, x.begin(), y.begin(), x.end());
		while(act)
		{
			const auto a(std::move(act));

			a();
		}
		return r;
	}

	static void
	EqualSubterm(bool& r, Action& act, TNCIter first1, TNCIter first2,
		TNCIter last1)
	{
		if(first1 != last1)
		{
			auto& x(ReferenceTerm(*first1));
			auto& y(ReferenceTerm(*first2));

			if(TermUnequal(x, y))
				r = {};
			else
			{
				yunseq(++first1, ++first2);
				act = [&, first1, first2, last1]{
					if(r)
						EqualSubterm(r, act, first1, first2, last1);
				};
				ystdex::update_thunk(act, [&, first1, first2, last1]{
					if(r)
						EqualSubterm(r, act, x.begin(), y.begin(), x.end());
				});
			}
		}
	}
};


class Encapsulate final : private EncapsulationBase
{
public:
	Encapsulate(shared_ptr<void> p)
		: EncapsulationBase(std::move(p))
	{}
	DefDeCopyMoveCtorAssignment(Encapsulate)

	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const Encapsulate& x, const Encapsulate& y) ynothrow
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
			if(Get() == enc.Get())
			{
				auto& tm(enc.TermRef);

				return MakeValueOrMove(p_ref, [&]() -> ReductionStatus{
					// NOTE: As an lvalue reference, the object in %tm cannot be
					//	destroyed.
					if(const auto p
						= NPL::TryAccessLeaf<const TermReference>(tm))
						term.SetContent(tm.GetContainer(), *p);
					else
					{
						// XXX: Subterms cleanup is deferred.
						// XXX: Allocators are not used here for performance in
						//	most cases.
						term.Value = TermReference(tm,
							FetchTailEnvironmentReference(*p_ref, ctx));
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
};
//@}


//! \since build 859
ReductionStatus
ApplyImpl(TermNode& term, ContextNode& ctx, shared_ptr<Environment> p_env)
{
	auto i(term.begin());
	auto& comb(NPL::Deref(++i));

	ForwardToUnwrapped(comb, ctx);

	TermNode expr(std::allocator_arg, term.get_allocator(), {std::move(comb)});

	ConsItem(expr, NPL::Deref(++i));
	term = std::move(expr);
	return RelayCombinedTail(ctx, term, std::move(p_env));
}

//! \since build 860
void
ListAsteriskImpl(TermNode& term)
{
	Retain(term);

	auto i(std::next(term.begin()));

	if(i != term.end())
	{
		auto& head(NPL::Deref(i));

		if(++i != term.end())
		{
			const auto last(std::prev(term.end()));

			ConsItem(term, Deref(last));
			term.erase(last),
			RemoveHead(term);
		}
		else
			LiftOther(term, head);
	}
	else
		ThrowInsufficientTermsError(term, {});
}

//! \since build 898
//@{
template<typename _func>
YB_FLATTEN ReductionStatus
AccImpl(_func f, TermNode& term, ContextNode& ctx)
{
	YAssert(FetchArgumentN(term) == 8, "Invalid recursive call found.");

	auto i(term.begin());
	auto& l(*++i);
	auto& pred(*++i);
	const auto& d(ctx.GetRecordPtr());
	auto lv_pred(EvaluateBoundLValueUnwrapped(pred, ctx, d));
	auto& con(term.GetContainerRef());
	auto& nterm(con.back());
	auto& lv_l(*++con.rbegin());

	lv_l = EvaluateLocalObject(l, d);
	nterm
		= TermNode::Container({std::move(lv_pred), lv_l}, term.get_allocator());
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	return ReduceCallSubsequent(nterm, ctx, d, A1::NameTypedReducerHandler(
		std::bind([&, d, f](TNIter& i_0) -> ReductionStatus{
		auto& base(*++i_0);

		if(!ExtractBool(nterm))
		{
			auto& head(*++i_0);
			auto lv_head(EvaluateBoundLValueUnwrapped(head, ctx, d));

			nterm.GetContainerRef() = {std::move(lv_head), lv_l};
			nterm.Value.Clear();
			return ReduceCallSubsequent(nterm, ctx, d,
				A1::NameTypedReducerHandler(
				std::bind([&, d, f](TNIter& i_1){
				return f(l, base, lv_l, nterm, d, i_1);
			}, std::move(i_0)), "eval-acc-head-next"));
		}
		else
		{
			LiftOther(term, base);
			return ReductionStatus::Retained;
		}
	}, std::move(i)), "eval-acc-nested"));
}

YB_FLATTEN ReductionStatus
AccLImpl(TermNode& term, ContextNode& ctx)
{
	return AccImpl([&](TermNode& l, TermNode& base, TermNode& lv_l, TermNode&
		nterm, const shared_ptr<Environment>& d, TNIter& i) YB_FLATTEN{
		auto& tail(*++i);
		auto lv_sum(EvaluateBoundLValueUnwrapped(*++i, ctx, d));

		base.GetContainerRef() = TermNode::Container({std::move(lv_sum),
			std::move(nterm), std::move(base)}, term.get_allocator());
		return ReduceCallSubsequent(base, ctx, d,
			A1::NameTypedReducerHandler([&, d]() YB_FLATTEN{
			auto lv_tail(EvaluateBoundLValueUnwrapped(tail, ctx, d));

			nterm.GetContainerRef() = {std::move(lv_tail), std::move(lv_l)};
			return ReduceCallSubsequent(nterm, ctx, d,
				A1::NameTypedReducerHandler([&]() YB_FLATTEN{
				l = std::move(nterm);
				return AccLImpl(term, ctx);
			}, "eval-accl-accl"));
		}, "eval-accl-tail"));
	}, term, ctx);
}

YB_FLATTEN ReductionStatus
AccRImpl(TermNode& term, ContextNode& ctx, TermNode& sum)
{
	return AccImpl([&](TermNode& l, TermNode&, TermNode& lv_l, TermNode& nterm,
		const shared_ptr<Environment>& d, TNIter& i) YB_FLATTEN{
		auto& con(term.GetContainerRef());
		const auto a(term.get_allocator());
		auto& n2term(*std::next(con.rbegin(), 2));
		auto lv_tail(EvaluateBoundLValueUnwrapped(*++i, ctx, d));

		n2term.GetContainerRef()
			= TermNode::Container({std::move(lv_tail), std::move(lv_l)}, a);
		return ReduceCallSubsequent(n2term, ctx, d,
			A1::NameTypedReducerHandler([&, a, d]() YB_FLATTEN{
			l = std::move(n2term);

			auto lv_sum(EvaluateBoundLValueUnwrapped(sum, ctx, d));
			auto& rterm(*term.emplace(ystdex::exchange(con, TermNode::Container(
				{std::move(lv_sum), std::move(nterm)}, a))));

			RelaySwitched(ctx, A1::NameTypedReducerHandler([&, d]() YB_FLATTEN{
				SetupTailContext(ctx, term);
				return RelayCombinedTail(ctx, term, d);
			}, "eval-accr-sum"));
			return AccRImpl(rterm, ctx, sum);
		}, "eval-accr-accr"));
	}, term, ctx);
}
//@}

//! \since build 899
//@{
void
LiftFirst(TermNode& term, TermNode& tm, bool move)
{
	if(const auto p = NPL::TryAccessLeaf<const TermReference>(tm))
	{
		if(!p->IsReferencedLValue())
		{
			LiftMovedOther(term, *p, p->IsMovable());
			return;
		}
	}
	// XXX: Term tags are currently not respected in prvalues.
	LiftOtherOrCopy(term, tm, move);
}

YB_FLATTEN TermNode&
FoldRMap1Impl(TermNode& term, TermNode& nd, ResolvedTermReferencePtr p_ref,
	TermNode& l)
{
	auto& con(term.GetContainerRef());
	auto& nterm(con.back());

	if(IsBranchedList(nd))
	{
		auto& tm_first(AccessFirstSubterm(nd));

		YAssert(nterm.empty(), "Invalid term found.");
		if(NPL::Deref(p_ref).IsReferencedLValue())
		{
			if(const auto p
				= NPL::TryAccessLeaf<const TermReference>(tm_first))
				nterm.SetContent(tm_first);
			else
				ReduceToReferenceAt(nterm, tm_first, p_ref);

			const auto a(nd.get_allocator());
			TermNode::Container ncon(a);
			auto j(nd.begin());

			while(++j != nd.end())
			{
				auto& tm(*j);
	
				if(IsReferenceTerm(tm))
					ncon.emplace_back(tm);
				else
					// XXX: Same to %ReduceToReferenceAt.
					ncon.emplace_back(NPL::AsTermNode(TermReference(
						tm.Tags, tm,
						NPL::Deref(p_ref).GetEnvironmentReference())));
			}
			l.Value.Clear();
			l.GetContainerRef() = std::move(ncon);
		}
		else
		{
			LiftFirst(nterm, tm_first, !p_ref || p_ref->IsModifiable());
			nd.GetContainerRef().pop_front();
		}
		return nterm;
	}
	else
		ThrowInsufficientTermsError(nd, p_ref);
}

YB_FLATTEN ReductionStatus
FoldR1Impl(TermNode& term, ContextNode& ctx, TermNode& kons)
{
	YAssert(FetchArgumentN(term) == 4, "Invalid recursive call found.");

	auto i(term.begin());
	auto& knil(*++(++i));
	auto& l(*++i);
	const auto& d(ctx.GetRecordPtr());
	auto lv_l(EvaluateLocalObject(l, d));

	// XXX: Keep %lv_l is a slightly more efficient.
	return NPL::ResolveTerm(
		[&, d](TermNode& nd, ResolvedTermReferencePtr p_ref) -> ReductionStatus{
		if(!IsEmpty(nd))
		{
			auto& nterm(FoldRMap1Impl(term, nd, p_ref, l));
			auto lv_kons(EvaluateBoundLValueUnwrapped(kons, ctx, d));
			auto& rterm(*term.emplace(ystdex::exchange(term.GetContainerRef(),
				TermNode::Container({std::move(lv_kons), std::move(nterm)},
				term.get_allocator()))));

			RelaySwitched(ctx, A1::NameTypedReducerHandler([&, d]() YB_FLATTEN{
				SetupTailContext(ctx, term);
				return RelayCombinedTail(ctx, term, d);
			}, "eval-foldr1-kons"));
			nterm.ClearContainer();
			return RelaySwitched(ctx, [&]{
				return FoldR1Impl(rterm, ctx, kons);
			});
		}
		LiftOther(term, knil);
		return ReductionStatus::Retained;
	}, lv_l);
}

YB_FLATTEN ReductionStatus
Map1Impl(TermNode& term, ContextNode& ctx, TermNode& appv)
{
	YAssert(FetchArgumentN(term) == 3, "Invalid recursive call found.");

	auto i(term.begin());
	auto& l(*++(++i));
	const auto& d(ctx.GetRecordPtr());
	auto lv_l(EvaluateLocalObject(l, d));

	// XXX: Keep %lv_l is a slightly more efficient.
	return NPL::ResolveTerm(
		[&, d](TermNode& nd, ResolvedTermReferencePtr p_ref) -> ReductionStatus{
		if(!IsEmpty(nd))
		{
			auto& nterm(FoldRMap1Impl(term, nd, p_ref, l));
			auto lv_appv(EvaluateBoundLValueUnwrapped(appv, ctx, d));
			const auto a(term.get_allocator());
			auto& rterm(*term.emplace(ystdex::exchange(term.GetContainerRef(),
				TermNode::Container({TermNode({std::move(lv_appv),
				std::move(nterm)}, a)}, a))));

			RelaySwitched(ctx, A1::NameTypedReducerHandler([&, d]() YB_FLATTEN{
				return ReduceCallSubsequent(*term.begin(), ctx, d,					
					A1::NameTypedReducerHandler([&]() YB_FLATTEN{
					auto i_term(term.begin());

					ConsItem(term, NPL::Deref(++i_term));
					term.erase(i_term);
					return ReductionStatus::Retained;
				}, "eval-map1-cons"));
			}, "eval-map1-appv"));
			nterm.ClearContainer();
			return RelaySwitched(ctx, [&]{
				return Map1Impl(rterm, ctx, appv);
			});
		}
		term.Clear();
		return ReductionStatus::Regular;
	}, lv_l);
}

YB_FLATTEN void
AccFoldR1(TermNode& term, ContextNode& ctx, TermNode& rterm, ptrdiff_t n)
{
	auto& sum(*std::next(rterm.begin(), n));

	term.emplace(std::move(sum));
	sum.Value.Clear();
	rterm.GetContainerRef().emplace_back();
	RelaySwitched(ctx, A1::NameTypedReducerHandler([&]{
		LiftOther(term, rterm);
		return ctx.LastStatus;	
	}, "eval-lift-sum"));
}

template<typename _func>
YB_FLATTEN ReductionStatus
DoFoldRMap1(TermNode& term, ContextNode& ctx, _func f)
{
	auto& con(term.GetContainerRef());
	auto& rterm(*term.emplace(ystdex::exchange(con,
		TermNode::Container(term.get_allocator()))));

	AccFoldR1(term, ctx, rterm, 1);
	return f(rterm, ctx, con.back());
}
//@}

} // unnamed namespace;


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

	if(n == m)
		return n;
	throw ArityMismatch(m, n);
}


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
		term.Value = true;
		return EqualSubterm(term.Value.GetObject<bool>(), ctx, true, x.begin(),
			y.begin(), x.end());
	}, static_cast<const TermNode&(&)(const TermNode&)>(ReferenceTerm));
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
		return ReduceSubsequent(*i, ctx, A1::NameTypedReducerHandler([&, i]{
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
Not(TermNode& term)
{
	auto& tm(NPL::ReferenceTerm(term));

	return IsLeaf(tm) && tm.Value == false;
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
	auto& op(*i);
	const auto i_list_app(++i);
	auto& app(*++i);
	auto& l(*++i);
	const auto a(term.get_allocator());
	const auto& d(ctx.GetRecordPtr());
	TermNode::Container con({EvaluateLocalObjectMoved(l, d)}, a);

	ForwardToUnwrapped(*i_list_app, ctx);
	con.splice(con.cbegin(), term.GetContainerRef(), i_list_app);
	op = TermNode(std::allocator_arg, a, std::move(con));
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	return ReduceCallSubsequent(op, ctx, d, A1::NameTypedReducerHandler(
		std::bind([&, d](shared_ptr<Environment>& p_e0) YB_FLATTEN{
		return NPL::ResolveTerm(
			[&](TermNode& nd, ResolvedTermReferencePtr p_ref) YB_FLATTEN{
			if(IsBranchedList(nd))
			{
				const auto assign_term(
					[&](TermNode::Container&& c, ValueObject&& vo) YB_FLATTEN{
					// XXX: The term %l is reused as a shared bound operand as
					//	the referent.
					l = TermNode(std::allocator_arg, term.get_allocator(),
						std::move(c), std::move(vo));
					return std::ref(l);
				});
				auto& x(AccessFirstSubterm(nd));

				if(p_ref)
					BindLocalReference(p_ref->GetTags()
						& (TermTags::Unique | TermTags::Nonmodifying), x,
						assign_term, p_ref->GetEnvironmentReference());
				else
					BindMoveLocalObject(x, assign_term);
				ForwardToUnwrapped(app, ctx);
#if true
				// XXX: This is more efficient.
				term.GetContainerRef() = {std::move(app), std::move(l)};
#else
				term.GetContainerRef().pop_front();
#endif
				return RelayCombinedTail(ctx, term, std::move(p_e0));
			}
			else
				ThrowInsufficientTermsError(nd, p_ref);
		}, op);
	}, d), "eval-forward-list-first"));
}

ReductionStatus
First(TermNode& term)
{
	return FirstOrVal(term,
		[&](TermNode& tm, ResolvedTermReferencePtr p_ref) -> ReductionStatus{
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
		const auto tr([&](TNIter iter){
			return ystdex::make_transform(iter, [&](TNIter i) -> ValueObject{
				// XXX: Like %LiftToReturn, but for %Value only.
				if(const auto p = NPL::TryAccessLeaf<const TermReference>(*i))
				{
					if(!p->IsMovable())
						return p->get().Value;
					return std::move(p->get().Value);
				}
				return std::move(i->Value);
			});
		});
#if true
		// XXX: This is slightly more efficient.
		ValueObject parent;

		parent.emplace<EnvironmentList>(tr(std::next(term.begin())),
			tr(term.end()), a);
		term.Value = NPL::AllocateEnvironment(a, std::move(parent));
#else
		term.Value = NPL::AllocateEnvironment(a, ValueObject(std::allocator_arg,
			a, in_place_type<EnvironmentList>, tr(std::next(term.begin())),
			tr(term.end()), a));
#endif
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
	DoDefine(term, std::bind(BindParameterChecked, ctx.GetRecordPtr(),
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
	const auto pr(CheckToUndefine(term, ctx));

	term.Value = pr.first.get().Remove(pr.second.get());
}

void
UndefineChecked(TermNode& term, ContextNode& ctx)
{
	const auto pr(CheckToUndefine(term, ctx));

	pr.first.get().RemoveChecked(pr.second.get());
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
		return p_ref ? ReduceForCombinerRef(term, ctx, *p_ref, h, 1)
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


ReductionStatus
CheckEnvironment(TermNode& term)
{
	RetainN(term);

	auto& tm(*std::next(term.begin()));

	Environment::EnsureValid(ResolveEnvironment(tm).first);
	MoveRValueToReturn(term, tm);
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

	term.GetContainerRef() = {NPL::AsTermNode(a, tag, std::allocator_arg, a,
		FormContextHandler(Encapsulate(p_type), 1)),
		NPL::AsTermNode(a, tag, std::allocator_arg, a,
		FormContextHandler(Encapsulated(p_type), 1)),
		NPL::AsTermNode(a, tag, std::allocator_arg, a,
		FormContextHandler(Decapsulate(p_type), 1))};
#else
	// XXX: Mixing the %p_type above to following assignment code can be worse
	//	in performance.
	shared_ptr<void> p_type(YSLib::allocate_shared<yimpl(byte)>(a));

	term.GetContainerRef() = {NPL::AsTermNode(a, std::allocator_arg, a, tag,
		std::allocator_arg, a, FormContextHandler(Encapsulate(p_type), 1)),
		NPL::AsTermNode(a, std::allocator_arg, a, tag,
		std::allocator_arg, a, FormContextHandler(Encapsulated(p_type), 1)),
		NPL::AsTermNode(a, std::allocator_arg, a, tag,
		std::allocator_arg, a, FormContextHandler(Decapsulate(p_type), 1))};
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

ReductionStatus
AccL(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 6);
	// XXX: This preprocessing binds all arguments but the abstract list object.
	//	It is necessary for recursive calls, but redundant for the non-recursive
	//	case. Implement it here for better overall performance as non-recursive
	//	cases are relatively rare.
	BindMoveNextNLocalSubobjectInPlace(std::next(term.begin()), 5);

	auto& con(term.GetContainerRef());

	con.emplace_back();
	con.emplace_back();
	return AccLImpl(term, ctx);
}

ReductionStatus
AccR(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 6);
	// XXX: Ditto.
	BindMoveNextNLocalSubobjectInPlace(std::next(term.begin()), 5);

	auto& con(term.GetContainerRef());
	auto& rterm(*term.emplace(ystdex::exchange(con,
		TermNode::Container(term.get_allocator()))));

	rterm.GetContainerRef().emplace_back();
	AccFoldR1(term, ctx, rterm, 6);
	return AccRImpl(rterm, ctx, con.back());
}

ReductionStatus
FoldR1(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 3);
	// XXX: Like %AccL.
	BindMoveNextNLocalSubobjectInPlace(term.begin(), 2);
	return DoFoldRMap1(term, ctx, FoldR1Impl);
}

ReductionStatus
Map1(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 2);
	// XXX: Like %AccL.
	BindMoveNextNLocalSubobjectInPlace(term.begin(), 2);
	return DoFoldRMap1(term, ctx, Map1Impl);
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

