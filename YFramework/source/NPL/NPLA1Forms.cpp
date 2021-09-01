/*
	© 2016-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Forms.cpp
\ingroup NPL
\brief NPLA1 语法形式。
\version r25566
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2014-02-15 11:19:51 +0800
\par 修改时间:
	2021-09-02 00:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1Forms
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1Forms // for YSLib, std::next, ReduceOnceLifted,
//	ResolvedTermReferencePtr, NPL::IsMovable, NPL::TryAccessReferencedTerm,
//	ystdex::value_or, ThrowInsufficientTermsError, NPL::Deref,
//	A1::NameTypedReducerHandler, ReduceReturnUnspecified, RemoveHead, IsBranch,
//	AccessFirstSubterm, ReduceSubsequent, ReduceCombinedBranch,
//	std::placeholders, std::ref, std::bind, ystdex::as_const, IsLeaf,
//	ValueObject, ystdex::ref_eq, RelaySwitched, shared_ptr, ContextHandler,
//	YSLib::unordered_map, string, Environment, lref, TokenValue, IsIgnore,
//	IsNPLASymbol, any_ops::use_holder, YSLib::in_place_type,
//	YSLib::HolderFromPointer, YSLib::allocate_shared, InvalidReference,
//	BindParameter, MoveFirstSubterm, ResolveEnvironment, ShareMoveTerm,
//	BindParameterWellFormed, TermToStringWithReferenceMark, ResolveTerm,
//	LiftOtherOrCopy, SContext::Analyze, std::allocator_arg, NPL::ResolveRegular,
//	ystdex::make_transform, NPL::TryAccessLeaf, TermReference, EnvironmentList,
//	NPL::AllocateEnvironment, ystdex::equality_comparable, CheckParameterTree,
//	IsBranchedList, NPL::AsTermNode, ystdex::exchange, NPLException,
//	LoggedEvent, YSLib::Alert, NPL::SwitchToFreshEnvironment, TermTags,
//	YSLib::Debug, YSLib::sfmt, ystdex::expand_proxy, NPL::AccessRegular,
//	GetLValueTagsOf, RegularizeTerm, LiftMovedOther, ThrowValueCategoryError,
//	ThrowListTypeErrorForNonlist, ThrowInvalidSyntaxError,
//	CheckEnvironmentFormal, A1::MakeForm, ystdex::type_id, ystdex::update_thunk,
//	IsTyped, ystdex::invoke_value_or, NPL::TryAccessReferencedLeaf,
//	ystdex::call_value_or, ystdex::bind1, BindSymbol, A1::AsForm, LiftCollapsed,
//	std::mem_fn, YSLib::usystem;
#include "NPLA1Internals.h" // for A1::Internals API;
#include YFM_NPL_SContext // for Session;

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
using Forms::CallRegularUnaryAs;
//! \since build 874
using Forms::CallResolvedUnary;
//! \since build 868
using Forms::CallUnary;


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
	if(IsBranch(clause))
		return clause.begin();
	ThrowInsufficientTermsError(clause, {});
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
		return ReduceCombinedBranch(term, ctx);
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
		return ReduceCombinedBranch(term, ctx);
	}
	LiftOther(term, tm);
	return ReductionStatus::Retained;
}
//@}

//! \since build 918
template<ReductionStatus(&_rAndOr)(TermNode&, ContextNode&, TNIter)>
ReductionStatus
AndOr(TermNode& term, ContextNode& ctx)
{
	Retain(term);

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
		return ReferenceTerm(x).Value;
	});
}

template<typename _func>
void
EqTermReference(TermNode& term, _func f)
{
	EqTermRet(term,
		[f] YB_LAMBDA_ANNOTATE((const TermNode& x, const TermNode& y), , pure){
		return IsLeaf(x) && IsLeaf(y) ? f(x.Value, y.Value)
			: ystdex::ref_eq<>()(x, y);
	}, static_cast<const TermNode&(&)(const TermNode&)>(ReferenceTerm));
}

YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, TermUnequal, const TermNode& x, const TermNode& y)
	ImplRet(x.size() != y.size() || x.Value != y.Value)

#if NPL_Impl_NPLA1_Enable_Thunked
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
		// XXX: The continuations in the middle are not required to be
		//	preserved.
		RelaySwitched(ctx,
			A1::NameTypedReducerHandler([&, first1, first2, last1]{
			// XXX: This is not effective if the result is known to be false.
			return r ? EqualSubterm(r, ctx, {}, first1, first2, last1)
				: ReductionStatus::Neutral;
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
		auto& x(ReferenceTerm(*first1));
		auto& y(ReferenceTerm(*first2));

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
	//! \since build 894
	shared_ptr<Environment> RecordPtr;
	//! \since build 893
	lref<const TermNode> TermRef;

	//! \since build 917
	RecursiveThunk(const shared_ptr<Environment>& p_env, const TermNode& t)
		: store(t.get_allocator()), RecordPtr(p_env), TermRef(t)
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
		// XXX: This is served as the addtional static environment.
		auto& env(NPL::Deref(p_env));

		MakeParameterValueMatcher([&](const TokenValue& n){
			YAssert(!IsIgnore(n) && IsNPLASymbol(n), "Invalid token found.");

			string_view id(n);

			ExtractSigil(id);

			if(!id.empty())
			{
				string k(id, store.get_allocator());

				// NOTE: The symbol can be bound more than once. Only one
				//	instance is supported.
				if(store.find(k) == store.cend())
					// NOTE: This binds value to a local thunk value. The
					//	bound symbol can then be rebound to an ordinary
					//	(non-sharing object.
					env.Bind(k, TermNode(TermNode::Container(
						t.get_allocator()), ValueObject(any_ops::use_holder,
						YSLib::in_place_type<
						YSLib::HolderFromPointer<shared_ptr_t>>,
						store[k] = YSLib::allocate_shared<ContextHandler>(
						t.get_allocator(), ThrowInvalidCyclicReference))));
			}
		})(t);
	}

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

//! \since build 917
void
CheckFrozenEnvironment(const shared_ptr<Environment>& p_env)
{
	if(YB_UNLIKELY(NPL::Deref(p_env).Frozen))
		throw TypeError("Cannot define variables in a frozen environment.");
}

//! \since build 919
void
CheckBindParameter(const shared_ptr<Environment>& p_env, const TermNode& t,
	TermNode& o)
{
	CheckFrozenEnvironment(p_env);
	BindParameter(p_env, t, o);
}

//! \since build 868
//@{
template<typename _func>
auto
DoDefineSet(TermNode& term, size_t n, _func f) -> decltype(f())
{
	Retain(term);
	if(term.size() >= n)
		return f();
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
auto
DoDefine(TermNode& term, _func f) -> decltype(f(std::declval<TermNode>()))
{
	return DoDefineSet(term, 2, [&]{
		RemoveHead(term);
		return f(SplitFirstSubterm(term));
	});
}

template<typename _func>
ReductionStatus
DoSet(TermNode& term, ContextNode& ctx, _func f)
{
	return DoDefineSet(term, 3, [&]{
		RemoveHead(term);

		const auto i(term.begin());

		return ReduceSubsequent(*i, ctx, A1::NameTypedReducerHandler([&, f, i]{
			auto p_env(ResolveEnvironment(*i).first);

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
		return ReduceSubsequent(term, ctx,
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
#if NPL_Impl_NPLA1_Enable_Thunked
		// XXX: Terms shall be moved and saved into the actions.
		auto p_saved(ShareMoveTerm(formals));

		// TODO: Avoid %shared_ptr?
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		return ReduceSubsequent(term, ctx,
			A1::NameTypedReducerHandler(std::bind([&](const
			shared_ptr<TermNode>&, const shared_ptr<RecursiveThunk>& p_gd,
			const _tParams&...){
			CheckFrozenEnvironment(p_gd->RecordPtr);
			// NOTE: The parameter tree shall have been checked in the
			//	initialization of %RecursiveThunk.
			BindParameterWellFormed(p_gd->RecordPtr, p_gd->TermRef, term);
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
		ReduceOnce(term, ctx);
		CheckFrozenEnvironment(gd.RecordPtr);
		// NOTE: The parameter tree shall have been checked in the
		//	initialization of %RecursiveThunk.
		BindParameterWellFormed(gd.RecordPtr, formals, term);
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
	EnsureValueTags(term.Tags);
	// NOTE: On %NPL_Impl_NPLA1_Enable_TCO, this assumes %term is same to the
	//	current term in %TCOAction, which is initialized by %CombinerReturnThunk
	//	in NPLA1.cpp.
	// XXX: The mandated host value check is in the call to
	//	%ContextNode::SwitchEnvironment.
	return TailCall::RelayNextGuardedProbe(ctx, term,
		EnvironmentGuard(ctx, ctx.SwitchEnvironment(std::move(p_env))),
		!no_lift, std::ref(ContextState::Access(ctx).ReduceOnce));
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
			if(const auto p = NPL::TryAccessLeaf<const TermReference>(*i))
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
	// XXX: The following code may be better, but both G++ 10.2.0 and Clang++
	//	11.1.0 seem quite buggy and fail to compile by selecting the wrong
	//	overload candidate in %ystdex::any::any which %std::allocator_arg_t in
	//	the front of the parameter list.
//	return ValueObject(in_place_type<EnvironmentList>, tr(first), tr(last), a);
#else
	return ValueObject(std::allocator_arg, a,
		YSLib::in_place_type<EnvironmentList>, tr(first), tr(last), a);
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
	/*!
	\brief 调用例程。
	\since build 909
	*/
	ReductionStatus(&call)(const VauHandler&, TermNode&, ContextNode&);

public:
	//! \brief 返回时不提升项以允许返回引用。
	bool NoLifting = {};

	/*!
	\pre 间接断言：形式参数对象指针非空。
	\pre 第三参数满足 Environment::CheckParent 检查不抛出异常。
	\sa MakeParent
	\sa MakeParentSingle
	\since build 918
	*/
	VauHandler(string&& ename, shared_ptr<TermNode>&& p_fm,
		ValueObject&& vo, TermNode& term, bool nl)
		: eformal(std::move(ename)), p_formals((CheckParameterTree(Deref(p_fm)),
		std::move(p_fm))), parent(std::move(vo)), p_eval_struct(ShareMoveTerm(
		ystdex::exchange(term, NPL::AsTermNode(term.get_allocator())))),
		call(eformal.empty() ? CallStatic : CallDynamic), NoLifting(nl)
	{}

	//! \since build 824
	friend bool
	operator==(const VauHandler& x, const VauHandler& y)
	{
		return x.eformal == y.eformal && x.p_formals == y.p_formals
			&& x.parent == y.parent && x.NoLifting == y.NoLifting;
	}

	//! \since build 772
	ReductionStatus
	operator()(TermNode& term, ContextNode& ctx) const
	{
		Retain(term);
		if(p_eval_struct)
			return call(*this, term, ctx);
		// XXX: The call has been performed on the handler, see the notes in
		//	%DoCall.
		throw NPLException("Invalid handler of call found.");
	}

private:
	//! \since build 847
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

	//! \since build 909
	static ReductionStatus
	CallDynamic(const VauHandler& vau, TermNode& term, ContextNode& ctx)
	{
		// NOTE: Evaluation in the local context: using the activation
		//	record frame with outer scope bindings.
		auto wenv(ctx.WeakenRecord());
		// XXX: Reuse of frame cannot be done here unless it can be proved all
		//	bindings would behave as in the old environment, which is too
		//	expensive for direct execution of programs with first-class
		//	environments.
		EnvironmentGuard gd(ctx, NPL::SwitchToFreshEnvironment(ctx));

		vau.BindEnvironment(ctx, std::move(wenv));
		// XXX: Referencing escaped variables (now only parameters need to be
		//	cared) form the context would cause undefined behavior (e.g.
		//	returning a reference to automatic object in the host language). The
		//	lifting of call result is enabled to prevent this, unless
		//	%NoLifting is %true. See also %BindParameter.
		return vau.DoCall(term, ctx, gd);
	}

	//! \since build 909
	static ReductionStatus
	CallStatic(const VauHandler& vau, TermNode& term, ContextNode& ctx)
	{
		// NOTE: See above.
		EnvironmentGuard gd(ctx, NPL::SwitchToFreshEnvironment(ctx));

		return vau.DoCall(term, ctx, gd);
	}

	// TODO: Avoid TCO when the static environment has something with side
	//	effects on destruction, to prevent semantic changes.
	//! \since build 847
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
		// NOTE: The environment is assumed not frozen, so no need to use
		//	%CheckFrozenEnvironment.
		// NOTE: The parameter tree shall have been checked in the
		//	initialization of %VauHandler.
		BindParameterWellFormed(ctx.GetRecordPtr(), NPL::Deref(p_formals),
			term);
#if NPL_Impl_NPLA1_TraceVauCall
		ctx.Trace.Log(YSLib::Debug, [&]{
			return YSLib::sfmt<string>(
				"Function called, with %ld shared term(s), %zu parameter(s).",
				p_eval_struct.use_count(), p_formals->size());
		});
#endif
		AssertNextTerm(ctx, term);

		// NOTE: Saved the no lift flag here to avoid branching in the
		//	following implementation.
		const bool no_lift(NoLifting);

		// NOTE: Since now binding does not rely on temporary objects stored
		//	elsewhere (by using %TermTags::Temporary instead), setting %term
		//	immediately after the call to %BindParameter should be safe even
		//	with TCO.
		// NOTE: Because the only possible (non-administrative) owner of the
		//	evaluation structure visible in the object language is the combiner
		//	object which is not possible here in the %term, the evaluation
		//	structure shall share nothing to the combining term being reduced
		//	here (i.e. %term), so it is safe to use %TermNode::SetContent
		//	instead of %LiftOther. See %ReduceCombinedBranch in NPLA1.cpp for
		//	details.
		// XXX: Using %LiftOtherOrCopy is less efficient.
		if(move)
		{
			// NOTE: The static environment is bound as the base of the local
			//	environment by setting the parent environment pointer.
			ctx.GetRecordRef().Parent = std::move(parent);
			// NOTE: The evaluation structure does not need to be saved to the
			//	continuation, since it would be used immediately in
			//	the call to %RelayForCall.
			term.SetContent(std::move(Deref(p_eval_struct)));
#if NPL_Impl_NPLA1_Enable_TCO

			auto& act(RefTCOAction(ctx));

			// XXX: Assume the last function being handled in %TCOAction is this
			//	object. This would make '*this' invalid.
			yunused(act.MoveFunction());
#endif
		}
		else
		{
			ctx.GetRecordRef().Parent = parent;
			term.SetContent(Deref(p_eval_struct));
		}
		// NOTE: The precondition is same to the last call in
		//	%EvalImplUnchecked.
		return RelayForCall(ctx, term, std::move(gd), no_lift);
	}

public:
	//! \since build 918
	YB_ATTR_nodiscard YB_PURE static ValueObject
	MakeParent(ValueObject&& vo)
	{
		Environment::CheckParent(vo);
		return std::move(vo);
	}

	// XXX: The check on %p_env for the parent should be checked before
	//	(by %ResolveEnvironment, etc.). 
	//! \since build 918
	YB_ATTR_nodiscard YB_PURE static ValueObject
	MakeParentSingle(TermNode::allocator_type a,
		pair<shared_ptr<Environment>, bool> pr)
	{
		auto& p_env(pr.first);

		// NOTE: The host value check is similar to %Environment::CheckParent
		//	since the parent is a single environment, except that it does not
		//	respect to 'NPL_NPLA_CheckParentEnvironment'.
		Environment::EnsureValid(p_env);
		if(pr.second)
			return ValueObject(std::allocator_arg, a,
				in_place_type<shared_ptr<Environment>>, std::move(p_env));
		return ValueObject(std::allocator_arg, a,
			in_place_type<EnvironmentReference>, std::move(p_env));
	}

	//! \since build 918
	YB_ATTR_nodiscard YB_PURE static ValueObject
	MakeParentSingleNonOwning(TermNode::allocator_type a,
		const shared_ptr<Environment>& p_env)
	{
		// NOTE: Ditto.
		Environment::EnsureValid(p_env);
		return ValueObject(std::allocator_arg, a,
			in_place_type<EnvironmentReference>, p_env);
	}
};


//! \since build 874
void
MakeValueListOrMove(TermNode& term, TermNode& nd,
	ResolvedTermReferencePtr p_ref)
{
	MakeValueOrMove(p_ref, [&]{
		for(const auto& sub : nd)
			term.Add(sub);
	}, [&]{
		// XXX: No cyclic reference check.
		term.GetContainerRef().splice(term.end(), nd.GetContainerRef());
	});
}

//! \since build 859
YB_NORETURN void
ThrowConsError(TermNode& nd, ResolvedTermReferencePtr p_ref)
{
	throw ListTypeError(ystdex::sfmt(
		"Expected a list for the 2nd argument, got '%s'.",
		TermToStringWithReferenceMark(nd, p_ref).c_str()));
}

//! \since build 859
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

//! \since build 912
//@{
void
ConsMove(TermNode& term, TNIter i)
{
	ConsItem(term, NPL::Deref(i));
	term.erase(i);
}

ReductionStatus
ConsMoveSubNext(TermNode& term)
{
	ConsMove(term, std::next(term.begin()));
	return ReductionStatus::Retained;
}

ReductionStatus
ConsImpl(TermNode& term)
{
	RetainN(term, 2);
	RemoveHead(term);
	return ConsMoveSubNext(term);
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

//! \since build 913
ReductionStatus
UnwrapResolved(TermNode& term, FormContextHandler& fch,
	ResolvedTermReferencePtr p_ref)
{
	if(fch.Wrapping != 0)
	{
		return MakeValueOrMove(p_ref, [&]{
			return ReduceForCombinerRef(term, NPL::Deref(p_ref),
				fch.Handler, fch.Wrapping - 1);
		}, [&]{
			--fch.Wrapping;
			term.Value = ContextHandler(std::allocator_arg,
				term.get_allocator(), std::move(fch));
			return ReductionStatus::Clean;
		});
	}
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

// NOTE: The bound term cannot be reused later because %term can be the
//	referent.
//! \since build 897
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
// NOTE: The returned term represents a term reference.
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

#if false
// NOTE: See %BindMoveLocalObjectInPlace and %EvaluateBoundLValueMoved.
//! \since build 897
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
	auto& h(NPL::AccessRegular<ContextHandler>(nd, true));

	if(const auto p = h.target<FormContextHandler>())
	{
		auto& fch(*p);

		if(fch.Wrapping != 0)
		{
			TermNode term(std::allocator_arg, a, con);

			ReduceForCombinerRef(term, ref, fch.Handler, fch.Wrapping - 1);
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
	if(const auto p = NPL::TryAccessLeaf<TermReference>(term))
		return EvaluateBoundUnwrappedLValueDispatch(
			term.get_allocator(), term.GetContainer(), *p, p->get());
	return EvaluateBoundUnwrappedLValueDispatch(term.get_allocator(),
		{}, TermReference(term.Tags, term, NPL::Nonnull(p_env)), term);
}


//! \since build 874
template<typename _func>
ReductionStatus
FirstOrVal(TermNode& term, _func f)
{
	return CallResolvedUnary([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		if(IsBranchedList(nd))
			return f(AccessFirstSubterm(nd), p_ref);
		ThrowInsufficientTermsError(nd, p_ref);
	}, term);
}

//! \since build 917
void
LiftCopyPropagate(TermNode& term, TermNode& tm, TermTags tags)
{
	// XXX: Similar to the implementation of %ReduceToReference in NPLA.cpp.
	term.CopyContent(tm);
	// NOTE: Propagate tags if it is a term reference.
	if(const auto p = NPL::TryAccessLeaf<TermReference>(term))
		p->SetTags(PropagateTo(p->GetTags(), tags));
	// XXX: Term tags are currently not respected in prvalues.
}
//! \since build 915
inline PDefH(void, LiftCopyPropagate, TermNode& term, TermNode& tm,
	const TermReference& ref)
	ImplExpr(LiftCopyPropagate(term, tm, ref.GetTags()))

//! \since build 917
void
LiftOtherOrCopyPropagateRef(TermNode& term, TermNode& tm, const TermReference& ref)
{
	if(ref.IsMovable())
		// XXX: Using %LiftOther (from %LiftOtherOrCopy) instead of
		//	%LiftTermOrCopy is safe, because the referent is not allowed to have
		//	cyclic reference to %term. And %LiftTermOrCopy is in that case is
		//	still inapproiate anyway because copy should be elided in a
		//	forwarding operation.
		LiftOther(term, tm);
	else
		// XXX: The call to %NPL::IsMovable has guarantees %p_ref is nonnull
		//	here.
		LiftCopyPropagate(term, tm, ref);
}

//! \since build 915
void
LiftOtherOrCopyPropagate(TermNode& term, TermNode& tm,
	ResolvedTermReferencePtr p_ref)
{
	if(!p_ref)
		LiftOther(term, tm);
	else
		LiftOtherOrCopyPropagateRef(term, tm, *p_ref);
}

//! \since build 914
ReductionStatus
ReduceToFirst(TermNode& term, TermNode& tm, ResolvedTermReferencePtr p_ref)
{
#if true
	// XXX: This is verbose but likely more efficient with %YB_FLATTEN.
	const bool list_not_move(p_ref && p_ref->IsReferencedLValue());

	if(const auto p = NPL::TryAccessLeaf<const TermReference>(tm))
	{
		if(list_not_move)
		{
			LiftCopyPropagate(term, tm, NPL::Deref(p_ref));
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
}

//! \since build 859
void
CheckResolvedListReference(TermNode& nd, bool has_ref)
{
	if(has_ref)
	{
		if(YB_UNLIKELY(!IsBranchedList(nd)))
			throw ListTypeError(ystdex::sfmt("Expected a non-empty list, got"
				" '%s'.", TermToStringWithReferenceMark(nd, true).c_str()));
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

//! \since build 910
void
DoFirstRestVal(TermNode& term, TermNode& tm, bool has_ref)
{
	// XXX: Simple 'ReduceToValue(term, tm)' is wrong because it may move
	//	non-unqiue reference object away.
	if(const auto p = NPL::TryAccessLeaf<const TermReference>(tm))
		LiftMovedOther(term, *p, !has_ref && p->IsMovable());
	else
		// XXX: Term tags are currently not respected in prvalues.
		LiftOtherOrCopy(term, tm, !has_ref);
}

//! \since build 918
template<void(&_rLift)(TermNode&), typename _fInsert>
YB_FLATTEN ReductionStatus
ReduceToRestOrVal(TermNode& term, TermNode& nd, ResolvedTermReferencePtr p_ref,
	_fInsert insert)
{
	if(IsBranchedList(nd))
	{
		TermNode::Container con(term.get_allocator());

		auto first(nd.begin());
		const auto last(nd.end());

		++first;
		if(!p_ref || p_ref->IsMovable())
		{
			_rLift(nd);
			con.splice(con.end(), nd.GetContainerRef(), first, last);
		}
		else
			for(; first != last; ++first)
				insert(con.emplace_back(), *first, *p_ref);
		con.swap(term.GetContainerRef());
		return ReductionStatus::Retained;
	}
	ThrowInsufficientTermsError(nd, p_ref);
}

//! \since build 918
template<void(&_rLift)(TermNode&), typename _fInsert>
YB_FLATTEN ReductionStatus
RestOrVal(TermNode& term, _fInsert insert)
{
	return CallResolvedUnary([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		return ReduceToRestOrVal<_rLift>(term, nd, p_ref, insert);
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
LiftNoOp(TermNode&)
// XXX: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100472.
#if !(YB_IMPL_GNUCPP && YB_HAS_NOEXCEPT && __cplusplus >= 201703L)
	ynothrow
#endif
{}

//! \since build 918
template<void(&_rLift)(TermNode&)>
void
SetRestImpl(TermNode& term)
{
	SetFirstRest([](TermNode& nd_x, TermNode& y){
		ResolveTerm([&](TermNode& nd_y, ResolvedTermReferencePtr p_ref_y){
			// XXX: How to simplify? Merge with %BindParameterObject?
			if(IsList(nd_y))
			{
				const auto a(nd_x.get_allocator());
				TermNode nd_new(a);

				nd_new.emplace();
				MakeValueListOrMove(nd_new, nd_y, p_ref_y);
				_rLift(nd_new);
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

//! \since build 918
//@{
YB_ATTR_nodiscard YB_PURE string
ShiftEmpty(TNIter& i)
{
	return string(i->get_allocator());
}

YB_ATTR_nodiscard string
ShiftOne(TNIter& i)
{
	return CheckEnvironmentFormal(NPL::Deref(++i));
}

//! \since build 921
template<typename _func>
inline ReductionStatus
ReduceCreateFunction(TermNode& term, _func f, size_t wrap)
{
	term.Value = A1::MakeForm(term, CheckFunctionCreation(f), wrap);
	return ReductionStatus::Clean;
}

YB_ATTR_nodiscard VauHandler
MakeVau(TermNode& term, bool no_lift, string(&shift)(TNIter&),
	TNIter i, ValueObject&& vo)
{
	auto formals(ShareMoveTerm(NPL::Deref(++i)));
	auto eformal(shift(i));

	term.erase(term.begin(), ++i);
	return VauHandler(std::move(eformal),
		std::move(formals), std::move(vo), term, no_lift);
}

//! \since build 921
template<size_t _vN, string(&_rShift)(TNIter&)>
YB_FLATTEN ReductionStatus
LambdaVau(TermNode& term, ContextNode& ctx, size_t wrap, bool no_lift)
{
	CheckVariadicArity(term, _vN);
	return ReduceCreateFunction(term, [&]{
		return MakeVau(term, no_lift, _rShift,
			term.begin(), VauHandler::MakeParentSingleNonOwning(
			term.get_allocator(), ctx.GetRecordPtr()));
	}, wrap);
}

//! \since build 921
template<size_t _vN, string(&_rShift)(TNIter&)>
YB_FLATTEN ReductionStatus
LambdaVauWithEnvironment(TermNode& term, ContextNode& ctx, size_t wrap,
	bool no_lift)
{
	CheckVariadicArity(term, _vN);

	auto i(term.begin());
	auto& tm(NPL::Deref(++i));

	return ReduceSubsequent(tm, ctx,
		A1::NameTypedReducerHandler([&, i, wrap, no_lift]{
		return ReduceCreateFunction(term, [&]{
			return ResolveTerm([&](TermNode& nd,
				ResolvedTermReferencePtr p_ref){
				return MakeVau(term, no_lift, _rShift, i,
					[&]() -> ValueObject{
					if(IsList(nd))
						return VauHandler::MakeParent(MakeEnvironmentParent(
							nd.begin(), nd.end(), nd.get_allocator(),
							!NPL::IsMovable(p_ref)));
					if(IsLeaf(nd))
						return VauHandler::MakeParentSingle(
							term.get_allocator(), ResolveEnvironment(
							nd.Value, NPL::IsMovable(p_ref)));
					ThrowInvalidEnvironmentType(nd, p_ref);
				}());
			}, tm);
		}, wrap);
	}, "eval-vau-parent"));
}
//@}

//! \since build 840
//@{
ReductionStatus
LambdaImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LambdaVau<0, ShiftEmpty>(term, ctx, Strict, no_lift);
}

//! \since build 918
ReductionStatus
LambdaWithEnvironmentImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LambdaVauWithEnvironment<1, ShiftEmpty>(term, ctx, Strict, no_lift);
}

ReductionStatus
VauImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LambdaVau<1, ShiftOne>(term, ctx, Form, no_lift);
}

ReductionStatus
VauWithEnvironmentImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LambdaVauWithEnvironment<2, ShiftOne>(term, ctx, Form, no_lift);
}
//@}

//! \since build 921
ReductionStatus
WVauImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LambdaVau<1, ShiftOne>(term, ctx, Strict, no_lift);
}

//! \since build 921
ReductionStatus
WVauWithEnvironmentImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return LambdaVauWithEnvironment<2, ShiftOne>(term, ctx, Strict, no_lift);
}


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

//! \since build 915
ReductionStatus
WrapH(TermNode& term, FormContextHandler h)
{
	// XXX: Allocators are not used here for performance in most cases.
	term.Value = ContextHandler(std::allocator_arg, term.get_allocator(),
		std::move(h));
	return ReductionStatus::Clean;
}

//! \since build 913
ReductionStatus
WrapN(TermNode& term, ResolvedTermReferencePtr p_ref,
	FormContextHandler& fch, size_t n)
{
	// XXX: Allocators are not used on %FormContextHandler for performance in
	//	most cases.
	return WrapH(term, MakeValueOrMove(p_ref, [&]{
		return FormContextHandler(fch.Handler, n);
	}, [&]{
		return
			FormContextHandler(std::move(fch.Handler), n);
	}));
}

//! \since build 913
ReductionStatus
WrapRefN(TermNode& term, ResolvedTermReferencePtr p_ref,
	FormContextHandler& fch, size_t n)
{
	if(p_ref)
		return ReduceForCombinerRef(term, *p_ref, fch.Handler, n);
	// XXX: Ditto.
	return WrapH(term, FormContextHandler(std::move(fch.Handler), n));
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
//@}

//! \since build 918
template<ReductionStatus(&_rWrap)(TermNode&,
	ResolvedTermReferencePtr, FormContextHandler&, size_t), typename _func>
ReductionStatus
WrapOrRef(TermNode& term, _func f)
{
	return WrapUnwrap(term,
		[&](FormContextHandler& fch, ResolvedTermReferencePtr p_ref){
		return _rWrap(term, p_ref, fch, AddWrapperCount(fch.Wrapping));
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
		return fch.Wrapping == 0 ? _rWrap(term, p_ref, fch, 1)
			: ThrowForWrappingFailure(ystdex::type_id<FormContextHandler>());
	}, [] YB_LAMBDA_ANNOTATE((const ContextHandler& h), , noreturn)
		-> ReductionStatus{
		ThrowForWrappingFailure(h.target_type());
	});
}


//! \since build 924
void
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
			ystdex::update_thunk(act, [&]{
				if(r)
					EqualSubterm(r, act, x.begin(), y.begin(), x.end());
			});
		}
	}
}


//! \since build 859
ReductionStatus
ApplyImpl(TermNode& term, ContextNode& ctx, shared_ptr<Environment> p_env)
{
	auto i(term.begin());
	auto& comb(NPL::Deref(++i));

	ForwardToUnwrapped(comb);

	TermNode expr(std::allocator_arg, term.get_allocator(), {std::move(comb)});

	ConsItem(expr, NPL::Deref(++i));
	term = std::move(expr);
	// NOTE: The precondition is same to the last call in %EvalImplUnchecked.
	//	See also the precondition of %Combine<TailCall>::RelayEnvSwitch.
	return Combine<TailCall>::RelayEnvSwitch(ctx, term, std::move(p_env));
}

//! \since build 860
void
ListAsteriskImpl(TermNode& term)
{
	CheckVariadicArity(term, 0);

	auto i(term.begin());
	auto& head(NPL::Deref(++i));

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

//! \since build 917
template<typename _func>
YB_FLATTEN ReductionStatus
Acc(_func f, TermNode& term, ContextNode& ctx)
{
	auto i(term.begin());
	auto& nterm(*i);
	auto& l(*++i);
	auto& pred(*++i);
	const auto& d(ctx.GetRecordPtr());
	auto& con(term.GetContainerRef());
	auto& lv_l(con.back());
	const auto nterm_cons_combine([&, d](TermNode& tm){
		TermNode::Container tcon(nterm.get_allocator());

		tcon.push_back(EvaluateBoundLValueUnwrapped(tm, d));
		tcon.push_back(lv_l);
		tcon.swap(nterm.GetContainerRef());
		nterm.Value.Clear();
	});

	// NOTE: This shall be stored separatedly to %l because %l is abstract,
	//	which can be a non-list.
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
				return f(l, base, lv_l, nterm, d, i_1);
			}, std::move(i_0)), "eval-acc-head-next"));
		}
		LiftOther(term, base);
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
		auto& tail(*++i);

		{
			TermNode::Container tcon(base.get_allocator());

			tcon.push_back(EvaluateBoundLValueUnwrapped(*++i, d));
			tcon.push_back(std::move(nterm));
			tcon.push_back(std::move(base));
			tcon.swap(base.GetContainerRef());
		}
		base.Value.Clear();
		return Combine<NonTailCall>::ReduceCallSubsequent(base, ctx, d,
			// XXX: Capture of %d by copy is a slightly more efficient.
			A1::NameTypedReducerHandler(
			[&, d] YB_LAMBDA_ANNOTATE(() , , flatten){
			{
				TermNode::Container tcon(nterm.get_allocator());

				tcon.push_back(EvaluateBoundLValueUnwrapped(tail, d));
				tcon.push_back(std::move(lv_l));
				tcon.swap(nterm.GetContainerRef());
			}
			nterm.Value.Clear();
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
		auto& n2term(*std::next(term.rbegin()));
		auto& tail(*++i);
		const auto& lv_sum_op(*++i);

		{
			TermNode::Container tcon(n2term.get_allocator());

			tcon.push_back(EvaluateBoundLValueUnwrapped(tail, d));
			tcon.push_back(std::move(lv_l));
			tcon.swap(n2term.GetContainerRef());
		}
		n2term.Value.Clear();
		return Combine<NonTailCall>::ReduceCallSubsequent(n2term, ctx, d,
			// XXX: Capture of %d by copy is a slightly more efficient.
			A1::NameTypedReducerHandler(
			[&, d] YB_LAMBDA_ANNOTATE(() , , flatten){
			l = std::move(n2term);
			return A1::ReduceCurrentNextThunked(
				*term.emplace(ystdex::exchange(term.GetContainerRef(), [&]{
				TermNode::Container tcon(term.get_allocator());

				tcon.push_back(lv_sum_op);
				tcon.push_back(std::move(nterm));
				return tcon;
			}())), ctx, ReduceAccR, A1::NameTypedReducerHandler(
				[&, d] YB_LAMBDA_ANNOTATE(() , , flatten){
				return Combine<NonTailCall>::ReduceEnvSwitch(term, ctx, d);
			}, "eval-accr-sum"));
		}, "eval-accr-accr"));
	}, term, ctx);
}

//! \since build 916
struct TermRange final
{
	TNIter First, Last;
	TermTags Tags = TermTags::Unqualified;

	TermRange(TermNode& nd, TermTags tags = TermTags::Unqualified) ynothrow
		: First(nd.begin()), Last(nd.end()), Tags(tags)
	{}

	YB_ATTR_nodiscard YB_PURE PDefH(bool, empty, ) const ynothrow
		ImplRet(First == Last)
};

/*!
\brief 准备递归调用使用的列表对象：绑定对象为列表临时对象范围。
\since build 913
*/
YB_FLATTEN void
PrepareFoldRList(TermNode& term)
{
	// NOTE: The 1st call can be applied to reference to list, but the nested
	//	application is only for non-list objects due to the rest list is extract
	//	as if a call of 'rest%' to the references of the list subobjects.
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
					term.SetValue(TermRange(term));
				}
				else
					// XXX: Term tags are currently not respected in prvalues.
					//	This is used to indicate the list is an lvalue which
					//	shall be copied on the element accesses later.
					term.Tags |= TermTags::Nonmodifying,
						term.SetValue(TermRange(nd, p_ref->GetTags()));
			}
			else
				term.SetValue(TermRange(term));
		}
		else
			// NOTE: Always treat the list as an lvalue as in the derivation.
			//	This is done only once here, since all recursive calls still
			//	keep %term as a list.
			ThrowInsufficientTermsError(nd, true);
	}, term);
}

//! \since build 916
void
ExtractRangeFirstOrCopy(TermNode& term, TermRange& tr, bool move)
{
	YAssert(!tr.empty(), "Invalid term found.");
	if(move)
		LiftOther(term, NPL::Deref(tr.First));
	else
		LiftCopyPropagate(term, NPL::Deref(tr.First), tr.Tags);
	++tr.First;
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
	{
		return A1::ReduceCurrentNextThunked(
			*term.emplace(ystdex::exchange(con, [&]{
			TermNode::Container tcon(term.get_allocator());

			tcon.push_back(*i);
			ExtractRangeFirstOrCopy(tcon.emplace_back(), tr,
				!bool(tm.Tags & TermTags::Nonmodifying));
			return tcon;
		}())), ctx, ReduceFoldR1, A1::NameTypedReducerHandler(std::bind(
			// TODO: Blocked. Use C++14 lambda initializers to simplify the
			//	implementation.
			[&] YB_LAMBDA_ANNOTATE((shared_ptr<Environment>& d) , , flatten){
			return
				Combine<NonTailCall>::ReduceEnvSwitch(term, ctx, std::move(d));
		}, ctx.ShareRecord()), "eval-foldr1-kons"));
	}
	LiftOther(term, *++i);
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
		ExtractRangeFirstOrCopy(tcon.emplace_back(), tr,
			!bool(tm.Tags & TermTags::Nonmodifying));
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
	return A1::ReduceCurrentNext(rterm, ctx, f, A1::NameTypedReducerHandler([&]{
		LiftOther(term, rterm);
		return ctx.LastStatus;
	}, "eval-lift-sum"));
#else
	const auto res(f(rterm, ctx));

	LiftOther(term, rterm);
	return res;
#endif
}

YB_FLATTEN ReductionStatus
ReduceFoldRMap1(TermNode& term, ContextNode& ctx,
	ReductionStatus(&f)(TermNode&, ContextNode&))
{
	auto& con(term.GetContainerRef());

	con.pop_front();
	// NOTE: Bind the last argument as the local reference to list temporary
	//	object.
	PrepareFoldRList(con.back());

	auto& rterm(*term.emplace(ystdex::exchange(con,
		TermNode::Container(term.get_allocator()))));

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
					LiftOther(tm, nd);
				else
				{
					auto i(tcon.begin());

					for(auto j(nd.begin()); j != nd.end(); yunseq(++i, ++j))
					{
						i = tcon.emplace(i);
						LiftCopyPropagate(*i, *j, p_ref->GetTags());
					}
					return;
				}
			}
		}
		else
			ThrowInsufficientTermsError(nd, true);
		tcon.splice(tcon.begin(), tm.GetContainerRef());
	}, tm);
}

YB_FLATTEN ReductionStatus
ReduceAppend(TermNode& term)
{
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

template<typename _func, typename _func2>
void
ListRangeExtract(TermNode& term, _func f, _func2 f2)
{
	// NOTE: The term is the term range of 'l' with optional temporary list.
	auto& con(term.GetContainerRef());
	auto& tr(term.Value.GetObject<TermRange>());
	TermNode::Container tcon(con.get_allocator());

	while(!tr.empty())
	{
		auto& nterm(tcon.emplace_back());
		auto& o(NPL::Deref(tr.First));

		if(const auto p = NPL::TryAccessLeaf<TermReference>(o))
			// NOTE: The tags of the term range shall be prepared in the
			//	call to %PrepareFoldRList. The nonmodifying tag indicates
			//	the reference is not movable.
			f(nterm, *p,
				GetLValueTagsOf(PropagateTo(p->GetTags(), tr.Tags)));
		else if(IsBranchedList(o))
			f2(nterm, o, tr.Tags);
		else
			ThrowInsufficientTermsError(o, true);
		++tr.First;
	}
	tcon.swap(con),
	term.Value.Clear(),
	term.Tags = TermTags::Unqualified;
}

ReductionStatus
ReduceListExtractFirst(TermNode& term)
{
	ListRangeExtract(term,
		[](TermNode& nterm, TermReference& oref, TermTags tags){
		TermReference ref(BindReferenceTags(tags), oref);
		auto& nd(ref.get());

		if(IsBranchedList(nd))
			RegularizeTerm(nterm,
				ReduceToFirst(nterm, AccessFirstSubterm(nd), &ref));
		else
			ThrowInsufficientTermsError(nd, true);
	}, [](TermNode& nterm, TermNode& o, TermTags tm_tags){
		auto& x(AccessFirstSubterm(o));

		if(const auto p = NPL::TryAccessLeaf<const TermReference>(x))
		{
			if(!p->IsReferencedLValue())
			{
				LiftMovedOther(nterm, *p, p->IsMovable());
				return;
			}
		}
		// XXX: Term tags are currently not respected in prvalues.
		LiftOtherOrCopy(nterm, x,
			!bool((o.Tags | x.Tags | tm_tags) & TermTags::Nonmodifying));
	});
	return ReductionStatus::Retained;
}

ReductionStatus
ReduceListExtractRestFwd(TermNode& term)
{
	ListRangeExtract(term,
		[](TermNode& nterm, TermReference& oref, TermTags tags){
		TermReference ref(tags, oref);
		auto& nd(ref.get());

		ReduceToRestOrVal<LiftNoOp>(nterm, nd, &ref,
			[&](TermNode& dst, TermNode& tm, const TermReference&){
			LiftOtherOrCopy(dst, tm, ref.IsMovable());
		});
	}, [](TermNode& nterm, TermNode& o, TermTags tm_tags){
		TermNode::Container con(nterm.get_allocator());

		auto first(o.begin());
		const auto last(o.end());

		++first;
		if(!bool((o.Tags | tm_tags) & TermTags::Nonmodifying))
			con.splice(con.end(), o.GetContainerRef(), first, last);
		else
			for(; first != last; ++first)
				con.emplace_back().CopyContent(*first);
		con.swap(nterm.GetContainerRef());
	});
	return ReductionStatus::Retained;
}

void
PrepareListExtract(TermNode& term)
{
	RetainN(term);
	LiftOther(term, term.GetContainerRef().back());
	// NOTE: Save the optional temporary list and bind the last argument as the
	//	local reference to list temporary object.
	PrepareFoldRList(term);
}
//@}

//! \since build 925
template<typename _fNext>
inline ReductionStatus
RelayApplicativeNext(ContextNode& ctx, TermNode& term, _fNext&& next)
{
	return A1::RelayCurrentNext(ctx, term,
		// NOTE: Capture the term regardless of the next term because
		//	continuation capture here is unsupported and the next term will be
		//	set later in %ReduceChildren.
		[&](TermNode&, ContextNode& c){
		ReduceChildren(term, c);
		return ReductionStatus::Partial;
	}, NPL::ToReducer(ctx.get_allocator(), yforward(next)));
}

//! \since build 919
//@{
void
CollectSubterms(TermNode& dst, TermNode& nd, TNIter i)
{
	auto& con(nd.GetContainerRef());
	auto& tcon(dst.GetContainerRef());

	tcon.splice(tcon.end(), con, i, con.end());
}

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

//! \since build 917
//@{
ReductionStatus
LetCallBody(TermNode& term, ContextNode& ctx, TermNode& body,
	EnvironmentGuard& gd, bool no_lift)
{
	// NOTE: Set 'body'.
	LiftOther(term, body);
#if NPL_Impl_NPLA1_Enable_Thunked
	SetupNextTerm(ctx, term);
#endif
	return RelayForCall(ctx, term, std::move(gd), no_lift);
}

template<typename _func>
ReductionStatus
LetCombineBody(_func f, TermNode& term, ContextNode& ctx)
{
	// NOTE: Subterms are extracted arguments for the call plus the parent in
	//	%Value, extracted 'formals' for the lambda abstraction,
	//	unused 'bindings', trailing 'body'.
	YAssert(term.size() >= 3, "Invalid nested call found.");

	EnvironmentGuard gd(ctx, NPL::SwitchToFreshEnvironment(ctx));
	auto i(term.begin());
	auto& operand(*i);

	// NOTE: Set the parent of the dynamic environment to the static
	//	environment as in %VauHandler::DoCall.
	ctx.GetRecordRef().Parent = std::move(operand.Value);

	auto& formals(*++i);
	auto& body(*++i);

	body.ClearContainer();
	CollectSubterms(body, term, ++i);
	return f(operand, formals, body, gd);
}

//! \since build 919
ReductionStatus
LetCombineApplicative(TermNode& term, ContextNode& ctx, bool no_lift)
{
	// NOTE: This does not optimize for cases of zero arguments as in
	//	%FormContextHandler::CallN in NPLA1.cpp, since the fast path is
	//	preferred in %LetCombineEmpty (by a caller like %LetEmpty).
	// NOTE: Subterms are extracted arguments for the call plus the parent in
	//	%Value, extracted 'formals' for the lambda abstraction,
	//	unused 'bindings', trailing 'body'.
	YAssert(term.size() >= 3, "Invalid nested call found.");
	return RelayApplicativeNext(ctx, *term.begin(),
		A1::NameTypedReducerHandler([&, no_lift]{
		return LetCombineBody([&](TermNode& operand, TermNode& formals,
			TermNode& body, EnvironmentGuard& gd){
			operand.Value.Clear();
			BindParameter(ctx.GetRecordPtr(), formals, operand);
			return LetCallBody(term, ctx, body, gd, no_lift);
		}, term, ctx);
	}, "eval-let"));
}

//! \since build 919
ReductionStatus
LetCombineEmpty(TermNode& term, ContextNode& ctx, bool no_lift)
{
	// NOTE: This optimize for cases of zero arguments as in
	//	%FormContextHandler::CallN in NPLA1.cpp.
	// NOTE: Subterms are unused arguments for the call (an empty list)
	//	plus the parent in %Value, unused 'formals' for the lambda
	//	abstraction (an empty list), unused 'bindings', trailing 'body'.
	YAssert(term.size() >= 3, "Invalid nested call found.");

	EnvironmentGuard gd(ctx, NPL::SwitchToFreshEnvironment(ctx));
	auto i(term.begin());

	// NOTE: As %LetCombineBody.
	ctx.GetRecordRef().Parent = std::move(i->Value);

	auto& body(*++++i);

	body.ClearContainer();
	CollectSubterms(body, term, ++i);
	return LetCallBody(term, ctx, body, gd, no_lift);
}

template<typename _func>
ReductionStatus
LetCombinePrepare(_func f, TermNode& term, ContextNode& ctx, bool with_env)
{
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

	YAssert(!parent, "Invalid value found in the 1st subterm.");
	// NOTE: After %parent being constructed, the dynamic environment shall then
	//	be constructed in the call to %f.
	if(!with_env)
	{
		parent.assign(std::allocator_arg, term.get_allocator(),
			ctx.WeakenRecord());
		return f();
	}
	++i;
	return ReduceSubsequent(NPL::Deref(i), ctx,
		A1::NameTypedReducerHandler([&, i, f]{
		// XXX: As %VauWithEnvironmentImpl.
		ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
			if(IsList(nd))
				parent = MakeEnvironmentParent(nd.begin(), nd.end(),
					nd.get_allocator(), !NPL::IsMovable(p_ref));
			else if(IsLeaf(nd))
			{
				auto p_env_pr(ResolveEnvironment(nd.Value,
					NPL::IsMovable(p_ref)));

				// NOTE: Similar to %VauHandler::MakeParentSingle.
				Environment::EnsureValid(p_env_pr.first);
				parent = std::move(p_env_pr.first);
			}
			else
				ThrowInvalidEnvironmentType(nd, p_ref);
		}, *i);
		term.GetContainerRef().erase(i);
		return f();
	}, "eval-let-parent"));
}

ReductionStatus
LetCommon(TermNode& term, ContextNode& ctx, bool no_lift, bool with_env)
{
	// NOTE: Subterms are extracted arguments for the call, optional 'e',
	//	extracted 'formals' for the lambda abstraction,
	//	unused 'bindings', trailing 'body'.
	YAssert(term.size() >= (with_env ? 4U : 3U), "Invalid nested call found.");
	return LetCombinePrepare([&, no_lift]{
		// NOTE: Now subterms are extracted arguments for the call plus
		//	the parent in %Value, extracted 'formals' for the lambda
		//	abstraction, unused 'bindings', trailing 'body'.
		YAssert(term.size() >= 3, "Invalid term found.");
#if false
		// NOTE: There is no need to check the parameter since it would be
		//	checked in the call to %BindParameter later.
		CheckParameterTree(NPL::Deref(std::next(term.begin())));
#endif
		// NOTE: The 1st subterm can also have the %Value as the parent object.
		//	Nevertheless, its subterms are used as the argument list at first.
		return LetCombineApplicative(term, ctx, no_lift);
	}, term, ctx, with_env);
}

//! \since build 914
ReductionStatus
LetCore(TermNode& term, ContextNode& ctx, bool no_lift, bool with_env)
{
	// NOTE: Subterms are %operand (the term range of 'bindings' with optional
	//	temporary list), optional 'e', originally bound 'bindings',
	//	trailing 'body'.
	YAssert(term.size() >= (with_env ? 3U : 2U), "Invalid nested call found.");
	LetBindingsExtract(term, with_env);
	// NOTE: Now subterms are extracted arguments for the call,
	//	optional 'e', extracted 'formals' for the lambda abstraction,
	//	originally bound 'bindings', trailing 'body'.
	return LetCommon(term, ctx, no_lift, with_env);
}

// NOTE: Specialized %LetCore implementation for empty binding list.
ReductionStatus
LetEmpty(TermNode& term, ContextNode& ctx, bool no_lift, bool with_env)
{
	// NOTE: Subterms are the empty term, optional 'e',
	//	unused originally bound 'bindings', trailing 'body'.
	YAssert(term.size() >= (with_env ? 3U : 2U), "Invalid nested call found.");
	LetAddFormalsTerm(term, with_env);
	// NOTE: This is required by %LetCombinePrepare.
	term.begin()->Clear();
	// NOTE: Now subterms are unused arguments for the call (an empty list),
	//	optional 'e', unused 'formals' for the lambda abstraction
	//	(an empty list), originally bound 'bindings', trailing 'body'.
	return LetCombinePrepare([&, no_lift]{
		return LetCombineEmpty(term, ctx, no_lift);
	}, term, ctx, with_env);
}

void
LetExpire(TermNode& term, TermNode& nd,
	const EnvironmentReference& r_env, TermTags o_tags)
{
	YAssert(IsList(nd), "Invalid term found.");
	// XXX: As %ReduceToReferenceUList for lvalues in terms of
	//	%BindParameterObject in NPLA1.cpp.
	const auto a(term.get_allocator());
	TermNode::Container con(a);

	for(auto& o : nd)
	{
		if(const auto p = NPL::TryAccessLeaf<TermReference>(o))
			con.emplace_back(o.GetContainer(), ValueObject(
				std::allocator_arg, a, in_place_type<TermReference>, *p));
		else
			con.emplace_back(TermNode::Container(o.get_allocator()),
				ValueObject(std::allocator_arg, a, in_place_type<
				TermReference>, GetLValueTagsOf(o.Tags | o_tags)
				| TermTags::Unique, o, r_env));
	}
	con.swap(term.GetContainerRef());
	// NOTE: As %PrepareFoldRList.
	term.SetValue(TermRange(term, TermTags::Temporary));
}

//! \since build 918
void
LetExpireChecked(TermNode& term, TermNode& nd,
	const EnvironmentReference& r_env, TermTags o_tags)
{
	if(IsList(nd))
		LetExpire(term, nd, r_env, o_tags);
	else
		ThrowListTypeErrorForNonlist(nd, true);
}

ReductionStatus
LetAsteriskImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	CheckVariadicArity(term, 0);

	auto& con(term.GetContainerRef());
	auto i(con.begin());
	auto& bindings(*++i);

	// NOTE: Optimize by the fast path for cases of zero arguments.
	if(IsEmpty(bindings))
		return LetEmpty(term, ctx, no_lift, {});
	if(const auto p = NPL::TryAccessLeaf<TermReference>(bindings))
	{
		auto& nd(p->get());

		// NOTE: Optimize by the fast path for cases of zero arguments.
		if(IsEmpty(nd))
			return LetEmpty(term, ctx, no_lift, {});
		if(p->IsMovable())
			con.splice(con.begin(), nd.GetContainerRef(), nd.begin());
		// NOTE: As %PrepareFoldRList.
		else if(IsList(nd))
		{
			p->AddTags(TermTags::Nonmodifying);
			bindings.ClearContainer(),
			bindings.SetValue(TermRange(nd, p->GetTags()));
			con.push_front(nd.GetContainer().front());
			++bindings.Value.GetObject<TermRange>().First;
		}
		else
			ThrowInsufficientTermsError(nd, true);
	}
	else if(const auto p_tr = NPL::TryAccessLeaf<TermRange>(bindings))
	{
		if(p_tr->empty())
			return LetEmpty(term, ctx, no_lift, {});
		con.push_front(NPL::Deref(p_tr->First++));
	}
	else
		con.splice(con.begin(), bindings.GetContainerRef(),
			bindings.begin());

	// NOTE: This is the 1st binding extracted for the immediate '$let'
	//	combination. This may represents a reference value.
	auto& extracted(con.front());
#if true
	// NOTE: As the %LetExpireChecked call with a binding list prvalue
	//	containing 1 binding subterm (see below).
	auto ref([&]() -> TermReference{
		if(const auto p = NPL::TryAccessLeaf<TermReference>(extracted))
			return TermReference(GetLValueTagsOf(p->GetTags()), *p);
		return TermReference(GetLValueTagsOf(extracted.Tags)
			| TermTags::Unique, extracted, ctx.GetRecordPtr());
	}());
	auto& nd(ref.get());

	if(IsBranchedList(nd))
	{
		auto& formals(*con.emplace_front().emplace());
		TermReference tref(BindReferenceTags(ref.GetTags()), ref);

		RegularizeTerm(formals,
			ReduceToFirst(formals, AccessFirstSubterm(nd), &tref));
		ReduceToRestOrVal<LiftNoOp>(*con.emplace_front().emplace(), nd,
			&ref, [&](TermNode& dst, TermNode& tm, const TermReference&){
			LiftOtherOrCopy(dst, tm, ref.IsMovable());
		});
		// XXX: The original 'bindings' (i.e. %extract) is not a binding
		//	list. Nevertheless, %LetCommon would not touch the term.
		return LetCommon(term, ctx, no_lift, {});
	}
	ThrowInsufficientTermsError(nd, true);
#else

	// NOTE: Make %extracted a binding list prvalue to fit to the expect of
	//	%LetExpireChecked.
	extracted.emplace(
		ystdex::exchange(extracted, TermNode(extracted.get_allocator())));
	LetExpireChecked(con.emplace_front(), extracted,
		ctx.GetRecordPtr(), extracted.Tags);
	return LetCore(term, ctx, no_lift, {});
#endif
}

ReductionStatus
LetOrRecImpl(TermNode& term, ContextNode& ctx,
	ReductionStatus(&let_core)(TermNode&, ContextNode&, bool, bool),
	bool no_lift, bool with_env = {})
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
	if(const auto p = NPL::TryAccessLeaf<TermReference>(bindings))
	{
		auto& nd(p->get());

		// NOTE: Optimize by the fast path for cases of zero arguments.
		if(IsEmpty(nd))
			return LetEmpty(term, ctx, no_lift, with_env);
		if(p->IsMovable())
			LetExpireChecked(forwarded, nd, p->GetEnvironmentReference(),
				p->GetTags());
		else
		{
			p->AddTags(TermTags::Nonmodifying);
			// NOTE: As %PrepareFoldRList.
			if(IsList(nd))
				forwarded.SetValue(TermRange(nd, p->GetTags())),
					forwarded.Tags = TermTags::Nonmodifying;
			else
				ThrowInsufficientTermsError(nd, true);
		}
	}
	else
		LetExpireChecked(forwarded, bindings, ctx.GetRecordPtr(),
			bindings.Tags);
	return let_core(term, ctx, no_lift, with_env);
}

// XXX: Currently the object language has no feature requiring %with_env.
//	Nevertheless, keep it anyway for simplicity.
ReductionStatus
LetRecCore(TermNode& term, ContextNode& ctx, bool no_lift, bool with_env)
{
	// NOTE: Subterms are %operand (the term range of 'bindings' with optional
	//	temporary list), optional 'e', originally bound 'bindings',
	//	trailing 'body'.
	YAssert(term.size() >= (with_env ? 3U : 2U), "Invalid nested call found.");
	LetBindingsExtract(term, with_env);
	// NOTE: Now subterms are extracted initializers for the definition,
	//	optional 'e', extracted 'formals' for the definition,
	//	originally bound 'bindings', trailing 'body'.
	YAssert(term.size() >= (with_env ? 4U : 3U), "Invalid term found.");
	return LetCombinePrepare([&, no_lift]() -> ReductionStatus{
		// NOTE: Now subterms are extracted initializers for the definition plus
		//	the parent in %Value, extracted 'formals' for the definition,
		//	unused 'bindings', trailing 'body'.
		YAssert(term.size() >= 3, "Invalid term found.");
#if false
		// NOTE: There is no need to check the parameter since it would be
		//	checked in the call to %BindParameter later.
		CheckParameterTree(term.GetContainer().back());
#endif
		return LetCombineBody([&](TermNode& operand, TermNode& formals,
			TermNode& body, EnvironmentGuard& egd){
			operand.Value.Clear();
			return RelayApplicativeNext(ctx, operand,
				A1::NameTypedReducerHandler(
				// TODO: Blocked. Use C++14 lambda initializers to simplify the
				//	implementation.
				std::bind([&, no_lift](EnvironmentGuard& gd){
				BindParameter(ctx.GetRecordPtr(), formals, operand);
				return LetCallBody(term, ctx, body, gd, no_lift);
			}, std::move(egd)), "eval-letrec-bind"));
		}, term, ctx);
	}, term, ctx, with_env);
}
//@}

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
DoBindingsToEnvironment(TermNode& term, ContextNode& ctx,
	shared_ptr<Environment>& p_env, TNIter i, TermNode& bindings,
	TermNode& operand)
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
void
ForwardImports(TermNode& term)
{
	for(auto& x : term)
		// NOTE: As %MoveRValueToForward, but with same term to
		//	be lifted.
		if(const auto p = NPL::TryAccessLeaf<const TermReference>(x))
			if(!p->IsReferencedLValue())
				LiftOtherOrCopy(x, p->get(), p->IsModifiable());
}

ReductionStatus
SymbolsToImportsApp(TermNode& term)
{
	Retain(term);
	RemoveHead(term);
	ForwardImports(term);
	return ReductionStatus::Retained;
}

YB_ATTR_nodiscard YB_PURE inline
	PDefH(TokenValue, CopyImportName, const TokenValue& s)
	ImplRet(HasImportsSigil(s) ? TokenValue(s.substr(1)) : s)

#if false
// NOTE: Lock a weak environment reference to a strong environment reference
//	accepting modifications later.
//! \since build 920
shared_ptr<Environment>
LockEnvironmentToModify(const EnvironmentReference& r_env)
{
	auto p_env(r_env.Lock());

	// NOTE: This is like the type check in %SetWithNoRecursion. If %r_env is
	//	from the current environment saved previously, it only fails when the
	//	environment goes out of its lifetime.
	Environment::EnsureValid(p_env);
	// NOTE: Evaluate only once. Equivalent to %SymbolsToImports for more than
	//	one calls of assignment with less overhead.
	CheckFrozenEnvironment(p_env);
	return p_env;
}
#endif

//! \since build 920
inline void
ShareEnvironmentToModify(const shared_ptr<Environment>& p_env)
{
	// NOTE: This is like the type check in %SetWithNoRecursion, expecting
	//	%p_env nonempty.
	YAssert(p_env, "Invalid environment found.");
	// NOTE: Evaluate only once. Equivalent to %SymbolsToImports for more than
	//	one calls of assignment with less overhead.
	CheckFrozenEnvironment(p_env);
}

//! \since build 920
void
BindImports(const shared_ptr<Environment>& p_env, TermNode& term,
	ContextNode& ctx, const shared_ptr<Environment>& p_src,
	bool ref_symbols = {})
{
	YAssert(IsList(term), "Invalid symbols term found.");
	if(IsBranch(term))
	{
		ShareEnvironmentToModify(p_env);
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

				// XXX: As %EvaluateIdentifier and the loop body in
				//	%ForwardImports, with some optimizations (see below).
				// XXX: Different to %EvaluateIdentifier, identifiers here are
				//	not used of operator, so %SetupTailOperatorName is not
				//	called.
				if(const auto p
					= NPL::TryAccessLeaf<const TermReference>(bound))
				{
					if(p->IsTemporary())
						LiftOtherOrCopy(nterm, p->get(), p->IsModifiable());
					else
						SetEvaluatedReference(nterm, bound, *p);
				}
				else if(bool(bound.Tags & TermTags::Temporary))
					LiftOtherOrCopy(nterm, bound,
						!NPL::Deref(pr.second).Frozen);
				else
					SetEvaluatedValue(nterm, bound, pr.second);
				// XXX: Different to %EvaluateIdentifier, only
				//	first-class objects are expected here. Literal
				//	handlers are ignored.
				if(ref_symbols)
					n = Ensigil(n);
				BindSymbol(p_env, n, nterm);
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

void
ProvideLetCommon(TermNode& term, ContextNode& ctx)
{
	// NOTE: Subterms are extracted arguments for the call, 'symbols',
	//	extracted 'formals' for the lambda abstraction,
	//	originally bound 'bindings', trailing 'body'.
	YAssert(term.size() >= 4, "Invalid nested call found.");

	auto& con(term.GetContainerRef());
	const auto a(con.get_allocator());
	auto i(con.begin());
	auto i_symbols(++i);
	const auto i_body(con.emplace(++++++i));

	CollectSubterms(*i_body, term, i);
	con.splice(con.end(), con, i_symbols);
	// NOTE: Now subterms are extracted arguments for the call, extracted
	//	'formals' for the lambda abstraction, originally bound 'bindings',
	//	'body', 'symbols'.
	YAssert(term.size() == 5, "Invalid term found.");

	auto& nd(ReferenceTerm(con.back()));

	// XXX: Like %Forms::SymbolsToImports.
	if(IsList(nd))
	{
		// XXX: Checks are required before the evaluation of 'body'.
		for(const auto& x : nd)
			yunused(NPL::ResolveRegular<TokenValue>(x));
	}
	else
		ThrowListTypeErrorForNonlist(nd, true);
	// NOTE: Construct the delayed operative before the body. The applicative
	//	and other trailing subterms make up the constructed trailing body
	//	subterms, which will be evaluated later in the dynamic environment
	//	created immediately before the evaluation of the constructed body. The
	//	constructed body is the combination of the delayed operative and
	//	the operand, see below.
	// XXX: Here the form is used as %LiteralHandler is not necessarily
	//	supported in this context (by the NPLA1 canonical evaluation algorithm),
	//	although it can work with the current implementation of
	//	%SetupDefaultInterpretation.
	con.emplace(i_body, A1::AsForm(a, ystdex::bind1(
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		[&](TermNode& t, ContextNode& c, shared_ptr<Environment>& p_env){
		// NOTE: Subterms are the unused combiner, 'body', 'symbols'.
		YAssert(t.size() == 3, "Invalid term found.");
		RemoveHead(t);
		AssertNextTerm(c, t);
		// NOTE: Evaluate 'body', and then create the bindings based on the
		//	evaluated 'body'. The imports refer to some subobjects in the
		//	evaluated subterms; otherwise there will be an error during the
		//	creation.
		return ReduceSubsequent(*t.begin(), c, A1::NameTypedReducerHandler(
			std::bind([&](shared_ptr<Environment>& p_env_0){
			BindImports(p_env_0, nd, c, c.GetRecordPtr());
			t.SetValue(c.ShareRecord());
			return ReductionStatus::Clean;
		}, std::move(p_env)), "provide-let-return"));
	}, std::placeholders::_2, ctx.ShareRecord())));
	// XXX: As %LetCombinePrepare, except that non-empty %con.begin()->Value is
	//	allowed.
	con.begin()->Value.assign(std::allocator_arg, a, ctx.WeakenRecord());
	// NOTE: Subterms are extracted arguments for the call plus the parent in
	//	%Value, extracted 'formals' for the lambda abstraction,
	//	unused 'bindings', constructed trailing body subterms (delayed
	//	operative, 'body', 'symbols'). This layout is compatible to the
	//	precondition of %LetCombineApplicative or %LetCombineEmpty.
}
//@}

//! \since build 920
ReductionStatus
ImportImpl(TermNode& term, ContextNode& ctx, bool ref_symbols)
{
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

} // unnamed namespace;

bool
IsSymbol(const string& id) ynothrow
{
	return IsNPLASymbol(id);
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

	EqualSubterm(r, act, x.begin(), y.begin(), x.end());
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
		}, NPL::TryAccessReferencedTerm<Encapsulation>(tm)));
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
				if(const auto p
					= NPL::TryAccessLeaf<const TermReference>(tm))
					term.SetContent(tm.GetContainer(), *p);
				else
				{
					// XXX: Subterms cleanup is deferred.
					// XXX: Allocators are not used here for performance in
					//	most cases.
					term.SetValue(in_place_type<TermReference>, tm,
						p_ref->GetEnvironmentReference());
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
	return ReduceCond(term, ctx, term.begin());
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
Not(const TermNode& x)
{
	auto& tm(ReferenceTerm(x));

	return IsLeaf(tm) && tm.Value == false;
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
	ConsImpl(term);
	LiftSubtermsToReturn(term);
	return ReductionStatus::Retained;
}

ReductionStatus
ConsRef(TermNode& term)
{
	// XXX: %ConsRef is declared external by %YF_API. %ConsImpl is not allowed
	//	for ELF symbol interposition anyway.
	return ConsImpl(term);
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
		if(IsBranchedList(nd))
		{
			const auto assign_term([&] YB_LAMBDA_ANNOTATE(
				(TermNode::Container&& c, ValueObject&& vo) , , flatten){
				// XXX: The term %op is reused as a shared bound operand as
				//	the referent.
				op = TermNode(std::allocator_arg, term.get_allocator(),
					std::move(c), std::move(vo));
				return std::ref(op);
			});
			auto& x(AccessFirstSubterm(nd));

			// NOTE: As %BindParameterObject::Match in NPLA1.cpp, never bind a
			//	temporary to a glvalue list element. Thus, no tag is checked in
			//	the condition here.
			if(p_ref)
				BindLocalReference(p_ref->GetTags()
					& (TermTags::Unique | TermTags::Nonmodifying), x,
					assign_term, p_ref->GetEnvironmentReference());
			else
				BindMoveLocalObject(x, assign_term);
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
		LiftOtherOrCopyPropagate(term, tm, p_ref);
		return ReductionStatus::Retained;
	});
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
		DoFirstRestVal(term, tm, has_ref);
		return ReductionStatus::Retained;
	});
}

ReductionStatus
RestFwd(TermNode& term)
{
	return RestOrVal<LiftNoOp>(term, LiftOtherOrCopyPropagateRef);
}

ReductionStatus
RestRef(TermNode& term, ContextNode& ctx)
{
	return CallResolvedUnary([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		CheckResolvedListReference(nd, p_ref);

		const auto a(term.get_allocator());
		auto p_sub(YSLib::allocate_shared<TermNode>(a));
		auto& con(p_sub->GetContainerRef());

		for(auto i(std::next(nd.begin())); i != nd.end(); ++i)
			// XXX: Same to %FirstRef.
			ReduceToReference(con.emplace_back(), *i, p_ref);
		// NOTE: Irregular representation is constructed for the list subobject
		//	reference.
		return ReduceAsSubobjectReference(term, std::move(p_sub),
			p_ref ? p_ref->GetEnvironmentReference() : ctx.WeakenRecord());
	}, term);
}

ReductionStatus
RestVal(TermNode& term)
{
	return RestOrVal<LiftSubtermsToReturn>(term,
		[](TermNode& dst, TermNode& tm, const TermReference&){
		DoFirstRestVal(dst, tm, true);
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
	SetRestImpl<LiftSubtermsToReturn>(term);
}

void
SetRestRef(TermNode& term)
{
	SetRestImpl<LiftNoOp>(term);
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
	Retain(term);
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
		// XXX: Allocators are not used on %FormContextHandler for performance
		//	in most cases.
		return WrapH(term, MakeValueOrMove(p_ref, [&]{
			return FormContextHandler(h, 1);
		}, [&]{
			return FormContextHandler(std::move(h), 1);
		}));
	});
}

ReductionStatus
WrapRef(TermNode& term)
{
	return WrapOrRef<WrapRefN>(term,
		[&](ContextHandler& h, ResolvedTermReferencePtr p_ref){
		// XXX: Ditto.
		return p_ref ? ReduceForCombinerRef(term, *p_ref, h, 1)
			: WrapH(term, FormContextHandler(std::move(std::move(h)), 1));
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
	LiftOther(term, tm);
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
	LiftOther(term, tm);
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
		return ApplyImpl(term, ctx, FetchValidEnvironment(
			ResolveEnvironment(*std::next(term.begin(), 3)).first));
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
	TermNode::Container(std::move(ycon)).swap(term.GetContainerRef());
	return ReductionStatus::Retained;
}

ReductionStatus
Append(TermNode& term)
{
	Retain(term);
#if true
	// XXX: This is a bit more efficient.
	RemoveHead(term);
	term.emplace(ystdex::exchange(term.GetContainerRef(),
		TermNode::Container(term.get_allocator())));
#else

	auto i(term.begin());
	auto& tm(NPL::Deref(i));

	tm.Clear();
	tm.GetContainerRef().splice(tm.end(), term.GetContainerRef(), ++i,
		term.end());
#endif
	return ReduceAppend(term);
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
					= NPL::TryAccessLeaf<const TermReference>(x))
				{
					if(!p->IsReferencedLValue())
						LiftMovedOther(x, *p, p->IsMovable());
				}
				nd.GetContainerRef().splice(nd.begin(), term.GetContainerRef(),
					i);
			}
			else
				ThrowListTypeErrorForNonlist(nd, p_ref);
		}
		else
			throw TypeError("Modifiable object expected.");
	}, l);
	return ReduceReturnUnspecified(term);
}

ReductionStatus
Let(TermNode& term, ContextNode& ctx)
{
	return LetOrRecImpl(term, ctx, LetCore, {});
}

ReductionStatus
LetRef(TermNode& term, ContextNode& ctx)
{
	return LetOrRecImpl(term, ctx, LetCore, true);
}

ReductionStatus
LetWithEnvironment(TermNode& term, ContextNode& ctx)
{
	return LetOrRecImpl(term, ctx, LetCore, {}, true);
}

ReductionStatus
LetWithEnvironmentRef(TermNode& term, ContextNode& ctx)
{
	return LetOrRecImpl(term, ctx, LetCore, true, true);
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
	return LetOrRecImpl(term, ctx, LetRecCore, {});
}

ReductionStatus
LetRecRef(TermNode& term, ContextNode& ctx)
{
	return LetOrRecImpl(term, ctx, LetRecCore, {});
}

ReductionStatus
AsEnvironment(TermNode& term, ContextNode& ctx)
{
	Retain(term);
	RemoveHead(term);

	// NOTE: As %LetEmpty.
	auto p_parent(ctx.WeakenRecord());
	EnvironmentGuard gd(ctx, NPL::SwitchToFreshEnvironment(ctx));

	ctx.GetRecordRef().Parent.assign(std::allocator_arg,
		term.get_allocator(), std::move(p_parent));
	// XXX: As %LetCallBodyCurrent.
	AssertNextTerm(ctx, term);
	// NOTE: The lifting is not enabled because the result is directly
	//	specified as a prvalue in the implementation.
	return TailCall::RelayNextGuarded(ctx, term, std::move(gd),
		[&](TermNode&, ContextNode&){
		return A1::RelayCurrentNext(ctx, term,
			std::ref(ContextState::Access(ctx).ReduceOnce),
			// TODO: Blocked. Use C++14 lambda initializers to simplify the
			//	implementation.
			ystdex::bind1([&](ContextNode&, shared_ptr<Environment>& p_e){
			term.ClearContainer(),
			term.SetValue(std::move(p_e));
			return ReductionStatus::Regular;
		}, ctx.ShareRecord()));
	});
}

ReductionStatus
BindingsWithParentToEnvironment(TermNode& term, ContextNode& ctx)
{
	CheckVariadicArity(term, 0);

	auto i(term.begin());
	auto& eterm(*++i);

	if(IsList(eterm))
		return A1::RelayCurrentNext(ctx, eterm,
			// NOTE: Capture the term regardless of the next term because
			//	continuation capture here is unsupported.
			[&](TermNode&, ContextNode& c){
			ReduceChildren(eterm, c);
			return ReductionStatus::Partial;
		}, NPL::ToReducer(ctx.get_allocator(),
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
	Retain(term);

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
				con.front() = A1::AsForm(a, SymbolsToImportsApp, Strict);
				return ReductionStatus::Retained;
			}
			term.ClearContainer();
			YAssert(IsEmpty(term), "Invalid term found.");
			return ReductionStatus::Regular;
		}
		ThrowListTypeErrorForNonlist(nd, true);
	}, NPL::Deref(term.begin()));
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
	if(const auto p = NPL::TryAccessLeaf<TermReference>(bindings))
	{
		auto& nd(p->get());

		if(p->IsMovable())
			LetExpireChecked(forwarded, nd, p->GetEnvironmentReference(),
				p->GetTags());
		else
		{
			p->AddTags(TermTags::Nonmodifying);
			if(IsList(nd))
				forwarded.SetValue(TermRange(nd, p->GetTags())),
					forwarded.Tags = TermTags::Nonmodifying;
			else
				ThrowInsufficientTermsError(nd, true);
		}
	}
	else
		LetExpireChecked(forwarded, bindings, ctx.GetRecordPtr(),
			bindings.Tags);
	// XXX: As %LetCore.
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


void
CallSystem(TermNode& term)
{
	CallUnaryAs<const string>(
		ystdex::compose(YSLib::usystem, std::mem_fn(&string::c_str)), term);
}

} // namespace Forms;

} // namesapce A1;

} // namespace NPL;

