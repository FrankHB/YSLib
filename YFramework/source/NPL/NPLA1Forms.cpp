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
\version r18132
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2014-02-15 11:19:51 +0800
\par 修改时间:
	2020-06-06 05:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1Forms
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1Forms // for YSLib, TermReference, in_place_type,
//	function, NPL::TryAccessReferencedTerm, ystdex::value_or, NPL::Deref,
//	RemoveHead, IsBranch, std::placeholders, ystdex::as_const, IsLeaf,
//	ystdex::ref_eq, ContextHandler, shared_ptr, string, unordered_map,
//	Environment, lref, list, IsBranchedList, TokenValue, NPL::TryAccessLeaf,
//	ValueObject, any_ops::use_holder, weak_ptr, ystdex::type_id,
//	YSLib::allocate_shared, InvalidReference, MoveFirstSubterm, ShareMoveTerm,
//	ystdex::get_less, map, set, std::make_move_iterator, ystdex::exists,
//	ystdex::retry_on_cond, ystdex::id, ystdex::ref, ystdex::id,
//	ystdex::expand_proxy, ystdex::cast_mutable, RelaySwitched,
//	ReduceCombinedBranch, ResolvedTermReferencePtr, LiftOtherOrCopy,
//	NPL::IsMovable, ResolveTerm, NPL::Access, ystdex::equality_comparable,
//	std::allocator_arg, NoContainer, ystdex::exchange,
//	NPL::SwitchToFreshEnvironment, TermTags, GetLValueTagsOf, NPL::AsTermNode,
//	AccessFirstSubterm, NPL::TryAccessReferencedLeaf, ystdex::invoke_value_or,
//	ystdex::call_value_or, LiftMovedOther, ystdex::make_transform,
//	NPL::TryAccessTerm, LiftCollapsed, NPL::AllocateEnvironment, std::mem_fn;
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
using Forms::ThrowInsufficientTermsError;
//! \since build 859
using Forms::ThrowInvalidSyntaxError;
using Forms::ThrowValueCategoryErrorForFirstArgument;
//@}


//! \since build 884
ReductionStatus
ReduceAgainCombined(TermNode& term, ContextNode& ctx)
{
#if NPL_Impl_NPLA1_Enable_TCO
	EnsureTCOAction(ctx, term).RequestRetrying();
	SetupNextTerm(ctx, term);
	return A1::RelayCurrentOrDirect(ctx,
		Continuation(std::ref(ReduceCombinedBranch), ctx), term);
#elif NPL_Impl_NPLA1_Enable_Thunked
	return ReduceSubsequentCombinedBranch(term, ctx, []() ynothrow{
		return ReductionStatus::Retrying;
	});
#else
	return ReduceNextCombinedBranch(term, ContextState::Access(ctx));
#endif
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
	// XXX: This is slightly more efficient than direct assignment with
	//	x86_64-pc-linux G++ 9.3 in this context.
	term.Value = ValueObject(std::allocator_arg, term.get_allocator(),
		ValueToken::Unspecified);
	return ReductionStatus::Clean;
}


//! \since build 881
using Action = function<void()>;


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
		return ReduceAgainCombined(term, ctx);
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
		return ReduceAgainCombined(term, ctx);
	}
	LiftOther(term, tm);
	return ReductionStatus::Retained;
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

//! \since build 888
YB_NORETURN inline void
ThrowInsufficientTermsErrorFor(InvalidSyntax&& e)
{
	TryExpr(ThrowInsufficientTermsError())
	CatchExpr(..., std::throw_with_nested(std::move(e)))
}

template<typename _func>
auto
DoDefineSet(TermNode& term, size_t n, _func f) -> decltype(f())
{
	Retain(term);
	if(term.size() > n)
		return f();
	ThrowInsufficientTermsErrorFor(
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
	//	inlining call of %ReduceCombinedBranch.
	return A1::RelayForEvalOrDirectNoLift(ctx, term, EnvironmentGuard(ctx,
		ctx.SwitchEnvironment(std::move(p_env))),
		std::ref(ReduceCombinedBranch));
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
	Session sess(ctx.get_allocator());

	SContext::Analyze(std::allocator_arg, ctx.get_allocator(), sess,
		sess.Process(NPL::ResolveRegular<const string>(expr)))
		.SwapContainer(expr);
	return EvalImplUnchecked(term, ctx, no_lift);
}
//@}


//! \since build 888
YB_ATTR_nodiscard YB_PURE inline
	PDefH(InvalidSyntax, MakeFunctionAbstractionError, )
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
		// NOTE: Since now binding does not rely on temporaries stored elsewhere
		//	(by using %TermTags::Temporary instead), this should be safe even
		//	with TCO.
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
		// NOTE: As %MarkTemporaryTerm in %NPLA1.
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
		//	should not be called later in %CombinerReturnThunk in %NPLA1 where
		//	the term is actually a combiner call.
		yunused(EnsureTCOActionUnchecked(ctx, term));
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
	ThrowInsufficientTermsErrorFor(MakeFunctionAbstractionError());
}

ReductionStatus
LambdaImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	return CreateFunction(term, [&, no_lift]{
		term.Value = CheckFunctionCreation([&]() -> ContextHandler{
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

		return ReduceSubsequent(tm, ctx, [&, i, no_lift]{
			term.Value = CheckFunctionCreation([&]{
				// XXX: List components are ignored.
				auto p_env_pr(ResolveEnvironment(tm));

				return CreateVau(term, no_lift, i,
					std::move(p_env_pr.first), p_env_pr.second);
			});
			return ReductionStatus::Clean;
		});
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
					// XXX: Allocators are not used here for performance in most
					//	cases.
					term.Value = TermReference(tm,
						FetchTailEnvironmentReference(*p_ref, ctx));
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
				return RelaySwitched(ctx, [&]{
					RelaySwitched(ctx, [&]{
						LiftOther(term, l);
						return ReductionStatus::Retained;
					});
#	if NPL_Impl_NPLA1_Enable_TCO
					// TODO: See %ReduceCallSubsequent.
					SetupTailTCOAction(ctx, l, {});
#	endif
					return RelayForCombine(ctx, l, env.shared_from_this());
				});
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


void
ThrowInsufficientTermsError()
{
	throw ParameterMismatch("Insufficient terms found for list parameter.");
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

