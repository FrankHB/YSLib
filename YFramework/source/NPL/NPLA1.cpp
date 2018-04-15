/*
	© 2014-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.cpp
\ingroup NPL
\brief NPLA1 公共接口。
\version r6893
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-02 18:02:47 +0800
\par 修改时间:
	2018-04-15 23:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1 // for ystdex::bind1, shared_ptr, lref, unordered_map,
//	ystdex::pvoid, ystdex::call_value_or, ystdex::as_const, pair;
#include <ystdex/scope_guard.hpp> // for ystdex::guard, ystdex::dismiss,
//	ystdex::swap_guard, ystdex::unique_guard;
#include YFM_NPL_SContext // for Session;

using namespace YSLib;

namespace NPL
{

// NOTE: The following options provide documented alternative implementations.
//	They are for exposition only. The code without TCO or thunk works without
//	several guarantees in the specification, so it is not conforming.

#define YF_Impl_NPLA1_Enable_TCO true
#define YF_Impl_NPLA1_Enable_TailRewriting true
#define YF_Impl_NPLA1_Enable_Thunked true

// NOTE: Use %RelaySwitched instead of %RelayNext as possible to be more
//	friendly to TCO.

//! \since build 820
static_assert(!YF_Impl_NPLA1_Enable_TCO || YF_Impl_NPLA1_Enable_Thunked,
	"Invalid combination of build options found.");

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

void
InsertChild(TermNode&& term, TermNode::Container& con)
{
	con.insert(term.GetName().empty() ? AsNode('$' + MakeIndex(con),
		std::move(term.Value)) : std::move(MapToValueNode(term)));
}

void
InsertSequenceChild(TermNode&& term, NodeSequence& con)
{
	con.emplace_back(std::move(MapToValueNode(term)));
}

ValueNode
TransformNode(const TermNode& term, NodeMapper mapper, NodeMapper map_leaf_node,
	NodeToString node_to_str, NodeInserter insert_child)
{
	auto s(term.size());

	if(s == 0)
		return map_leaf_node(term);

	auto i(term.begin());
	const auto nested_call(ystdex::bind1(TransformNode, mapper, map_leaf_node,
		node_to_str, insert_child));

	if(s == 1)
		return nested_call(*i);

	const auto& name(node_to_str(*i));

	if(!name.empty())
		yunseq(++i, --s);
	if(s == 1)
	{
		auto&& nd(nested_call(*i));

		if(nd.GetName().empty())
			return AsNode(name, std::move(nd.Value));
		return {ValueNode::Container{std::move(nd)}, name};
	}

	ValueNode::Container node_con;

	std::for_each(i, term.end(), [&](const TermNode& tm){
		insert_child(mapper ? mapper(tm) : nested_call(tm), node_con);
	});
	return {std::move(node_con), name};
}

ValueNode
TransformNodeSequence(const TermNode& term, NodeMapper mapper, NodeMapper
	map_leaf_node, NodeToString node_to_str, NodeSequenceInserter insert_child)
{
	auto s(term.size());

	if(s == 0)
		return map_leaf_node(term);

	auto i(term.begin());
	auto nested_call(ystdex::bind1(TransformNodeSequence, mapper,
		map_leaf_node, node_to_str, insert_child));

	if(s == 1)
		return nested_call(*i);

	const auto& name(node_to_str(*i));

	if(!name.empty())
		yunseq(++i, --s);
	if(s == 1)
	{
		auto&& n(nested_call(*i));

		return AsNode(name, n.GetName().empty() ? std::move(n.Value)
			: ValueObject(NodeSequence{std::move(n)}));
	}

	NodeSequence node_con;

	std::for_each(i, term.end(), [&](const TermNode& tm){
		insert_child(mapper ? mapper(tm) : nested_call(tm), node_con);
	});
	return AsNode(name, std::move(node_con));
}


//! \since build 685
namespace
{

//! \since build 736
template<typename _func>
TermNode
TransformForSeparatorTmpl(_func f, const TermNode& term, const ValueObject& pfx,
	const ValueObject& delim, const string& name)
{
	using namespace std::placeholders;
	// NOTE: Explicit type 'TermNode' is intended.
	TermNode res(AsNode(name, term.Value));

	if(IsBranch(term))
	{
		// NOTE: Explicit type 'TermNode' is intended.
		res += TermNode(AsIndexNode(res, pfx));
		ystdex::split(term.begin(), term.end(),
			ystdex::bind1(HasValue<ValueObject>, std::ref(delim)),
			std::bind(f, std::ref(res), _1, _2));
	}
	return res;
}

//! \since build 808
bool
ExtractBool(TermNode& term, bool is_and) ynothrow
{
	return ystdex::value_or(AccessTermPtr<bool>(term), is_and) == is_and;
}

//! \since build 820
ReductionStatus
NormalizeTerm(TermNode& term, ReductionStatus res)
{
	// NOTE: Cleanup if and only if necessary.
	if(res == ReductionStatus::Clean)
		term.ClearContainer();
	return res;
}

//! \since build 820
ReductionStatus
ReduceForClosureResult(TermNode& term, const ContextNode& ctx)
{
	NormalizeTerm(term, ctx.LastStatus);
	LiftToSelfSafe(term);
	return CheckNorm(term);
}

//! \since build 820
using EnvironmentGuard = ystdex::guard<EnvironmentSwitcher>;

#if YF_Impl_NPLA1_Enable_Thunked
//! \since build 821
inline /*[[nodiscard]]*/
#if true
PDefH(ReductionStatus, RelayTail, ContextNode& ctx, Reducer& cur)
	ImplRet(RelaySwitched(ctx, std::move(cur)))
#else
// NOTE: For exposition only. This does not hold guarantee of TCO in unbounded
//	recursive cases.
PDefH(ReductionStatus, RelayTail, ContextNode&, const Reducer& cur)
	ImplRet(cur())
#endif

//! \since build 814
void
PushActionsRange(EvaluationPasses::const_iterator first,
	EvaluationPasses::const_iterator last, TermNode& term, ContextNode& ctx,
	shared_ptr<ReductionStatus> p_res)
{
	if(first != last && !ctx.SkipToNextEvaluation)
	{
		const auto& f(first->second);

		++first;
		// NOTE: The returning value would inform propably existed enclosing
		//	caller to retry a new turn of reduction if the current action is
		//	cleared, otherwise it is to be ignored as per the condition of
		//	enclosing rewriting loop. Like synchronous case, the returning
		//	value cannot be handled just (as the enclosed operation) by
		//	unconditionally retrying some specific operation.
		// NOTE: The skip mark is requested by some other action, e.g. in
		//	%ReduceAgain. The next tail actions would be dropped by reducer
		//	which requests retrying, which assumes it is always reducing the
		//	same term and the next actions are safe to be dropped. It should
		//	be treated as attempt to switch to new action, which should be
		//	in turn already properly set or cleared as the current action.
		RelaySwitched(ctx, [=, &f, &term, &ctx]{
			PushActionsRange(first, last, term, ctx, p_res);
			// NOTE: If reducible, the current action should have been properly
			//	set or cleared. It cannot be cleared here because there is no
			//	simple way to guarantee the action is the last one for
			//	evaluation of specified term merely by last reduction status,
			//	and %ContextNode::Current needs to be exposed to allow capture
			//	of current continuation.
			return *p_res
				= CombineSequenceReductionResult(*p_res, f(term, ctx));
		});
	}
	else
		ctx.SkipToNextEvaluation = {};
}

//! \since build 812
ReductionStatus
DelimitActions(const EvaluationPasses& passes, TermNode& term, ContextNode& ctx)
{
	// NOTE: Now both outermost call and inner ones need to support continuation
	//	capture. The difference is that the boundary is implied in the outermost
	//	case, which is addressed by the separation of current action and
	//	delimited actions in the context. The implementation here does not use
	//	deleimited actions and they are reserved to external control primitive
	//	like shift/reset operations, to avoid need of unwinding which introduce
	//	the necessary of delimited frame mark and frame walking in delimieted
	//	actions. (But be cautious with overflow risks in call of
	//	%ContextNode::ApplyTail.)
	ctx.SkipToNextEvaluation = {};
	PushActionsRange(passes.cbegin(), passes.cend(), term, ctx,
		make_shared<ReductionStatus>(ReductionStatus::Clean));
	return ReductionStatus::Retrying;
}

#	if YF_Impl_NPLA1_Enable_TCO
//! \since build 818
struct TCOAction final
{
	lref<TermNode> Term;
	//! \since build 821
	mutable Reducer Next;
	//! \since build 820
	//@{
	bool ReduceNestedAsync = {};
	bool LiftCallResult = {};
	mutable stack<ystdex::any> XGuards{};
	mutable EnvironmentGuard EnvGuard;
	mutable list<shared_ptr<Environment>> EnvPtrList{};
	bool ReduceCombined = {};
	//@}
	//! \since build 823
	vector<shared_ptr<const void>> DynamicAnchors{};

	//! \since build 819
	TCOAction(ContextNode& ctx, TermNode& term, bool lift)
		: Term(term), Next(ctx.Switch()), LiftCallResult(lift), EnvGuard(ctx)
	{}
	// XXX: Not used, but provided for well-formness.
	//! \since build 819
	TCOAction(const TCOAction& a)
		// XXX: %EnvGuard is moved. This is only safe when newly constructed
		//	object always live longer than the older one.
		: Term(a.Term), Next(a.Next), ReduceNestedAsync(a.ReduceNestedAsync),
		LiftCallResult(a.LiftCallResult), XGuards(std::move(a.XGuards)),
		EnvGuard(std::move(a.EnvGuard)), ReduceCombined(a.ReduceCombined)
	{}
	DefDeMoveCtor(TCOAction)

	DefDeMoveAssignment(TCOAction)

	ReductionStatus
	operator()() const
	{
		auto& ctx(EnvGuard.func.Context.get());

		RelaySwitched(ctx, std::move(Next));

		// TODO: Support conditional term lifting in user code. See $2018-02
		//	@ %Documentation::Workflow::Annual2018.
		const auto res(LiftCallResult ? ReduceForClosureResult(Term, ctx)
			: ctx.LastStatus);

		// NOTE: The order here is significant. The environment in the guards
		//	should be hold until lifting is completed.
		while(!EnvPtrList.empty())
			EnvPtrList.pop_front();
		{
			const auto egd(std::move(EnvGuard));
		}
		while(!XGuards.empty())
			XGuards.pop();
		if(ReduceCombined)
			NormalizeTerm(Term, res);
		if(ReduceNestedAsync)
			ctx.SkipToNextEvaluation = true;
		return res;
	}
};
#	endif

//! \since build 817
ReductionStatus
ReduceCheckedAsync(TermNode& term, ContextNode& ctx)
{
	// XXX: Assume it is always reducing the same term and the next actions are
	//	safe to be dropped.
	ctx.LastStatus = ReductionStatus::Retrying;
	return RelaySwitched(ctx, [&]{
		if(CheckReducible(ctx.LastStatus))
		{
			RelaySwitched(ctx, std::bind(ReduceOnce, std::ref(term),
				std::ref(ctx)));
			ctx.SkipToNextEvaluation = true;
		}
		return ctx.LastStatus;
	});
}

//! \since build 810
ReductionStatus
ReduceChildrenOrderedAsync(TNIter first, TNIter last, ContextNode& ctx)
{
	if(first != last)
	{
		auto& subterm(*first);

		++first;
		return RelaySwitched(ctx, [&, first, last]{
			RelaySwitched(ctx, std::bind(ReduceChildrenOrderedAsync, first,
				last, std::ref(ctx)));
			return ReduceCheckedAsync(subterm, ctx);
		});
	}
	return ReductionStatus::Clean;
}
#endif

//! \since build 817
void
ReduceCheckedSync(TermNode& term, ContextNode& ctx)
{
	CheckedReduceWith(Reduce, term, ctx);
}

//! \since build 821
ReductionStatus
ReduceSubsequent(TermNode& term, ContextNode& ctx, Reducer&& next)
{
#if YF_Impl_NPLA1_Enable_Thunked
	return RelayNext(ctx, std::bind(ReduceCheckedAsync, std::ref(term),
		std::ref(ctx)), CombineActions(ctx, std::move(next), ctx.Switch()));
#else
	// NOTE: This does not support PTC.
	ReduceCheckedSync(term, ctx);
	return next();
#endif
}

#if YF_Impl_NPLA1_Enable_Thunked
#	if YF_Impl_NPLA1_Enable_TailRewriting
//! \since build 823
ReductionStatus
ReduceSequenceOrderedAsync(TermNode& term, ContextNode& ctx, TNIter i)
{
	YAssert(i != term.end(), "Invalid iterator found for sequence reduction.");
	if(std::next(i) == term.end())
	{
		LiftTerm(term, *i);
		return ReduceAgain(term, ctx);
	}
	return ReduceSubsequent(*i, ctx, [&, i]{
		return ReduceSequenceOrderedAsync(term, ctx, term.erase(i));
	});
}
#	endif
#endif


//! \since build 753
ReductionStatus
AndOr(TermNode& term, ContextNode& ctx, bool is_and)
{
	Forms::Retain(term);

	auto i(term.begin());

	if(++i != term.end())
	{
		if(std::next(i) == term.end())
		{
			LiftTerm(term, *i);
			return ReduceAgain(term, ctx);
		}
		return ReduceSubsequent(*i, ctx, [&, i, is_and]{
			if(ExtractBool(*i, is_and))
				term.Remove(i);
			else
			{
				term.Value = !is_and;
				return ReductionStatus::Clean;
			}
			return ReduceAgain(term, ctx);
		});
	}
	term.Value = is_and;
	return ReductionStatus::Clean;
}

//! \since build 804
//@{
template<typename _fComp, typename _func>
void
EqualTerm(TermNode& term, _fComp f, _func g)
{
	Forms::RetainN(term, 2);

	auto i(term.begin());
	const auto& x(Deref(++i));

	term.Value = f(g(x), g(ystdex::as_const(Deref(++i))));
}

template<typename _func>
void
EqualTermValue(TermNode& term, _func f)
{
	EqualTerm(term, f, [](const TermNode& node) -> const ValueObject&{
		return ReferenceTerm(node).Value;
	});
}

template<typename _func>
void
EqualTermReference(TermNode& term, _func f)
{
	EqualTerm(term, f, [](const TermNode& node) -> const TermNode&{
		return ReferenceTerm(node);
	});
}
//@}


//! \since build 782
class RecursiveThunk final
	: private yimpl(noncopyable), private yimpl(nonmovable)
{
private:
	//! \since build 784
	//@{
	using shared_ptr_t = shared_ptr<ContextHandler>;
	unordered_map<string, shared_ptr_t> store{};
	shared_ptr_t
		p_defualt{make_shared<ContextHandler>(ThrowInvalidCyclicReference)};
	//@}

public:
	//! \since build 787
	Environment& Record;
	//! \since build 780
	const TermNode& Term;

	//! \since build 787
	//@{
	RecursiveThunk(Environment& env, const TermNode& t)
		: Record(env), Term(t)
	{
		Fix(Record, Term, p_defualt);
	}

private:
	void
	Fix(Environment& env, const TermNode& t, const shared_ptr_t& p_d)
	{
		if(IsBranch(t))
			for(const auto& tm : t)
				Fix(env, tm, p_d);
		else if(const auto p = AccessPtr<TokenValue>(t))
		{
			const auto& n(*p);

			// XXX: This is served as addtional static environment.
			Forms::CheckParameterLeafToken(n, [&]{
				// TODO: The symbol can be rebound?
				env.GetMapRef()[n].SetContent(TermNode::Container(),
					ValueObject(ystdex::any_ops::use_holder, ystdex::in_place<
					HolderFromPointer<weak_ptr<ContextHandler>>>,
					store[n] = p_d));
			});
		}
	}

	void
	Restore(Environment& env, const TermNode& t)
	{
		if(IsBranch(t))
			for(const auto& tm : t)
				Restore(env, tm);
		else if(const auto p = AccessPtr<TokenValue>(t))
			FilterExceptions([&]{
				const auto& n(*p);
				auto& v(env.GetMapRef()[n].Value);

				if(v.type() == ystdex::type_id<ContextHandler>())
				{
					// XXX: The element should exist.
					auto& p_strong(store.at(n));

					Deref(p_strong) = std::move(v.GetObject<ContextHandler>());
					v = ValueObject(ystdex::any_ops::use_holder,
						ystdex::in_place<HolderFromPointer<shared_ptr_t>>,
						std::move(p_strong));
				}
			});
	}
	//@}

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


//! \since build 780
template<typename _func>
void
DoDefine(TermNode& term, _func f)
{
	Forms::Retain(term);
	if(term.size() > 2)
	{
		RemoveHead(term);

		auto formals(std::move(Deref(term.begin())));

		RemoveHead(term);
		LiftToSelf(formals);
		f(formals);
	}
	else
		throw InvalidSyntax("Insufficient terms in definition.");
	term.Value = ValueToken::Unspecified;
}


//! \since build 799
YB_NONNULL(2) void
CheckVauSymbol(const TokenValue& s, const char* target)
{
	if(s != "#ignore" && !IsNPLASymbol(s))
		throw InvalidSyntax(ystdex::sfmt("Token '%s' is not a symbol or"
			" '#ignore' expected for %s.", s.data(), target));
}

//! \since build 799
YB_NORETURN YB_NONNULL(2) void
ThrowInvalidSymbolType(const TermNode& term, const char* n)
{
	throw InvalidSyntax(ystdex::sfmt("Invalid %s type '%s' found.", n,
		term.Value.type().name()));
}

//! \since build 781
string
CheckEnvFormal(const TermNode& eterm)
{
	const auto& term(ReferenceTerm(eterm));

	if(const auto p = TermToNamePtr(term))
	{
		CheckVauSymbol(*p, "environment formal parameter");
		return *p;
	}
	else
		ThrowInvalidSymbolType(term, "environment formal parameter");
	return {};
}

#if YF_Impl_NPLA1_Enable_TCO
//! \since build 821
bool
MergeTCOFrames(Environment& parent, Environment& saved)
{
	// TODO: Make the frames reused as possible.
	// TODO: Allow not pinned objects?
	// XXX: Dynamic environment can be lost as lifetime
	//	of weak pointer is not guaranteed.
	for(const auto& binding : saved.GetMapRef())
		// XXX: Non-trivially destructible objects is treated same.
		// NOTE: Redirection is not needed here.
		parent.Remove(binding.GetName(), true);
	return parent.GetMapRef().empty();
}
#endif

//! \since build 822
//@{
ReductionStatus
ReduceCheckedClosureImpl(TermNode& term, ContextNode& ctx, bool move,
	TermNode& closure, bool lift_result)
{
	if(move)
		LiftTerm(term, closure);
	else
		term.SetContent(closure);
	// XXX: Term reused.
#if YF_Impl_NPLA1_Enable_TCO

	Reducer next(std::bind(ReduceCheckedAsync, std::ref(term), std::ref(ctx)));

	if(const auto p = ctx.Current.target<TCOAction>())
	// XXX: It should be same to saved enclosing term currently.
//	if(&p->Term.get() == &term)
	{
		if(lift_result)
		{
			if(p->LiftCallResult)
				next = std::bind([&, p](const Reducer& act){
					ReduceForClosureResult(term, ctx);
					return act();
				}, std::move(next));
			else
				p->LiftCallResult = true;
		}
		return RelaySwitchedUnchecked(ctx, std::move(next));
	}
	return RelayNext(ctx, std::move(next), TCOAction(ctx, term, lift_result));
#else
	if(lift_result)
		return ReduceSubsequent(term, ctx,
			std::bind(ReduceForClosureResult, std::ref(term), std::ref(ctx)));
	// TODO: Optimize.
	return ReduceSubsequent(term, ctx, [&]{
		return ctx.LastStatus;
	});
#endif
}

ReductionStatus
RelayOnNextEnvironment(ContextNode& ctx, TermNode& term, bool move,
	TermNode& closure, EnvironmentGuard&& gd, bool no_lift)
{
	auto next_action(std::bind(ReduceCheckedClosureImpl, std::ref(term),
		std::ref(ctx), move, std::ref(closure), !no_lift));

	// TODO: Merge guard in TCO action.
#if YF_Impl_NPLA1_Enable_TCO
	const auto update_fused_act([&](TCOAction& fused_act){
		fused_act.EnvGuard = std::move(gd);
	});

	if(const auto p = ctx.Current.target<TCOAction>())
	{
		// TODO: Remove variables in parent environments which are also in the
		//	current tail environment. If the resulted parent environment is
		//	empty, relink the parent environment of the tail environment and
		//	remove the old parent environment from the chain to reclaim the
		//	inactive record space.
		if(p->EnvGuard.func.SavedPtr)
		{
			auto& p_saved(gd.func.SavedPtr);

			if(p_saved)
			{
				auto& env_list(p->EnvPtrList);
				auto i(env_list.begin());

				while(i != env_list.end())
				{
					const auto& p_frame(*i);

					// NOTE: If the frame is hold elsewhere, it does not need
					//	to be owned here.
					if(!p_frame.unique())
						i = env_list.erase(i);
					else if(p_frame->IsNotReferenced()
						&& MergeTCOFrames(Deref(p_frame), *p_saved))
					{
						auto& saved_parent(p_saved->Parent);
						const auto compress([&](Environment* p_e) noexcept{
							if(p_e == p_frame.get())
							{
								saved_parent = std::move(p_frame->Parent);
								i = env_list.erase(i);
							}
						});

						if(p_frame->Parent == saved_parent)
							compress(p_frame.get());
						else if(const auto p_wenv
							= saved_parent.AccessPtr<EnvironmentReference>())
							compress(p_wenv->Lock().get());
						else if(const auto p_env
							= saved_parent.AccessPtr<shared_ptr<Environment>>())
							compress(p_env->get());
						// TODO: Compress parent environment for more cases.
						else
							++i;
					}
					else
						++i;
				}
				env_list.push_front(std::move(p_saved));
			}
		}
		else
			update_fused_act(*p);
		return RelaySwitchedUnchecked(ctx, std::move(next_action));
	}

	TCOAction fused_act(ctx, term, {});

	update_fused_act(fused_act);
	return RelayNext(ctx, std::move(next_action), std::move(fused_act));
#elif YF_Impl_NPLA1_Enable_Thunked
	// NOTE: Lambda is not used to avoid unspecified destruction order of
	//	captured component and better performance (compared to the case of
	//	%pair used to keep the order and %shared_ptr to hold the guard).
	struct Action
	{
		// NOTE: The destruction order of captured component is significant.
		//! \since build 821
		mutable Reducer Next;
		mutable EnvironmentGuard Guard;
		Action(ContextNode& ctx, EnvironmentGuard& egd)
			// XXX: Several members are moved. It is only safe when newly
			//	constructed object always live longer than the older one.
			: Next(ctx.Switch()), Guard(std::move(egd))
		{}
		Action(const Action& a)
			: Next(std::move(a.Next)), Guard(std::move(Guard))
		{}
		DefDeMoveCtor(Action)

		DefDeMoveAssignment(Action)

		ReductionStatus
		operator()() const
		{
			{
				const auto gd(std::move(Guard));
			}
			return Next();
		}
	};

	return RelayNext(ctx, std::move(next_action), Action(ctx, gd));
#else
	yunused(gd);
	// NOTE: This does not support PTC.
	return next_action();
#endif
}

ReductionStatus
EvalImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	Forms::RetainN(term, 2);

	const auto i(std::next(term.begin()));
	const auto term_args([](TermNode& expr) -> pair<bool, lref<TermNode>>{
		if(const auto p = AccessPtr<const TermReference>(expr))
			return {{}, p->get()};
		return {true, expr};
	}(Deref(i)));

	// TODO: Support more environment types?
	return RelayOnNextEnvironment(ctx, term, term_args.first, term_args.second,
		EnvironmentGuard(ctx, ctx.SwitchEnvironment(
		ResolveEnvironment(Deref(std::next(i))).first)), no_lift);
}

//@}


//! \since build 767
class VauHandler final
{
private:
	/*!
	\brief 动态上下文名称。
	\since build 769
	*/
	string eformal{};
	/*!
	\brief 形式参数对象。
	\invariant \t bool(p_formals) 。
	\since build 771
	*/
	shared_ptr<TermNode> p_formals;
	/*!
	\brief 局部上下文原型。
	\since build 790
	\todo 移除不使用的环境。
	*/
	ContextNode local_prototype;
	/*!
	\note 共享所有权用于检查循环引用。
	\since build 823
	\todo 优化。考虑使用区域推断代替。
	*/
	//@{
	//! \brief 捕获静态环境作为父环境的引用，包含引入抽象时的静态环境。
	EnvironmentReference parent;
	bool owning_static;
	//! \brief 可选保持所有权的静态环境指针或父环境锚对象指针。
	shared_ptr<const void> p_static;
	//@}
	//! \brief 闭包求值构造。
	shared_ptr<TermNode> p_closure;

public:
	//! \brief 返回时不提升项以允许返回引用。
	bool NoLifting = {};

	/*!
	\pre 形式参数对象指针非空。
	\since build 822
	*/
	VauHandler(string&& ename, shared_ptr<TermNode>&& p_fm,
		shared_ptr<Environment>&& p_env, bool owning,
		const ContextNode& ctx, TermNode& term, bool no_lift)
		: eformal(std::move(ename)), p_formals((LiftToSelf(Deref(p_fm)),
		std::move(p_fm))),
		local_prototype(ctx, make_shared<Environment>()), parent(p_env),
		owning_static(owning), p_static(owning ? std::move(p_env)
		: p_env->Anchor()), p_closure(share_move(term)), NoLifting(no_lift)
	{
		CheckParameterTree(*p_formals);
	}

	//! \since build 772
	ReductionStatus
	operator()(TermNode& term, ContextNode& ctx) const
	{
		if(IsBranch(term))
		{
			using namespace Forms;
			// NOTE: Local context: activation record frame with outer scope
			//	bindings.
			// XXX: Referencing escaped variables (now only parameters need to
			//	be cared) form the context would cause undefined behavior (e.g.
			//	returning a reference to automatic object in the host language).
			//	See %BindParameter.
			auto wenv(ctx.WeakenRecord());
			// XXX: Reuse of frame cannot be done here unless it can be proved
			//	all bindings would behave as in the old environment, which is
			//	too expensive for direct execution of programs with first-class
			//	environments.
			EnvironmentGuard
				gd(ctx, ctx.SwitchEnvironment(make_shared<Environment>()));

			// NOTE: Bound dynamic context.
			if(!eformal.empty())
			{
#if YF_Impl_NPLA1_Enable_TCO
				if(const auto p = ctx.Current.target<TCOAction>())
					if(const auto p_env = wenv.Lock())
						p->DynamicAnchors.push_back(p_env->Anchor());
#endif
				ctx.GetBindingsRef().AddValue(eformal,
					ValueObject(std::move(wenv)));
			}
			// NOTE: Since first term is expected to be saved (e.g. by
			//	%ReduceCombined), it is safe to reduce directly.
			RemoveHead(term);
			// NOTE: Forming beta using parameter binding, to substitute them as
			//	arguments for later closure reducation.
			// TODO: Do not lift terms if provable to safe?
			BindParameter(ctx, Deref(p_formals), term);
			ctx.Trace.Log(Debug, [&]{
				return sfmt("Function called, with %ld shared term(s), %ld"
					" %s shared static environment(s), %zu parameter(s).",
					p_closure.use_count(), parent.GetPtr().use_count(),
					owning_static ? "owning" : "nonowning", p_formals->size());
			});
			// NOTE: Static environment is bound as base of local context by
			//	setting parent environment pointer.
			ctx.GetRecordRef().Parent = parent;
			// TODO: Implement accurate lifetime analysis depending on
			//	'p_closure.unique()'?
			return RelayOnNextEnvironment(ctx, term, {}, *p_closure,
				std::move(gd), NoLifting);
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
				CheckVauSymbol(*p, "parameter in a parameter tree");
			else
				ThrowInvalidSymbolType(term, "parameter tree node");
		}
	}
};


//! \since build 781
template<typename _func>
void
CreateFunction(TermNode& term, _func f, size_t n)
{
	Forms::Retain(term);
	if(term.size() > n)
		term.Value = ContextHandler(f(term.GetContainerRef()));
	else
		throw InvalidSyntax("Insufficient terms in function abstraction.");
}

//! \since build 801
std::string
TermToValueString(const TermNode& term)
{
	return ystdex::quote(std::string(TermToString(ReferenceTerm(term))), '\''); 
}

//! \since build 821
template<typename... _tParams>
ReductionStatus
CombinerReturnThunk(const ContextHandler& h, TermNode& term, ContextNode& ctx,
	_tParams&&... args)
{
#if YF_Impl_NPLA1_Enable_Thunked
	auto call(std::bind(std::ref(h), std::ref(term), std::ref(ctx)));
#	if YF_Impl_NPLA1_Enable_TCO
	const auto update_fused_act([&](TCOAction& fused_act){
		// XXX: Blocked. 'yforward' cause G++ 5.3 crash: internal compiler
		//	error: Segmentation fault.
		yunseq(0, (fused_act.XGuards.push(std::forward<_tParams>(args)), 0)...);
		fused_act.ReduceCombined = true;
	});

	if(const auto p = ctx.Current.target<TCOAction>())
	// XXX: See %ReduceCheckedClosureImpl.
//	if(&p->Term.get() == &term)
	{
		update_fused_act(*p);
		return RelaySwitchedUnchecked(ctx, std::move(call));
	}

	TCOAction fused_act(ctx, term, {});

	update_fused_act(fused_act);
	return RelayNext(ctx, std::move(call), std::move(fused_act));
#	else
	return RelayNext(ctx, std::move(call),
		std::bind([&](Reducer& act, const _tParams&...){
		// NOTE: Captured argument pack is only needed when %h actually shares.
		RelaySwitched(ctx, std::move(act));
		return NormalizeTerm(term, ctx.LastStatus);
	}, ctx.Switch(), std::move(args)...));
#	endif
#else
	yunseq(0, args...);
	// NOTE: This does not support PTC.
	return NormalizeTerm(term, h(term, ctx));
#endif
}

//! \since build 822
//@{
ReductionStatus
ConsImpl(TermNode& term, bool by_val)
{
	using namespace Forms;

	RetainN(term, 2);

	const auto i(std::next(term.begin(), 2));
	const auto get_ins_idx([&, i]{
		term.erase(i);
		return GetLastIndexOf(term);
	});
	const auto ret([&]{
		RemoveHead(term);
		if(by_val)
			LiftSubtermsToSelfSafe(term);
		return ReductionStatus::Retained;
	});
	auto& item(Deref(i));

	// TODO: Simplify?
	if(const auto p = AccessPtr<const TermReference>(item))
	{
		if(IsList(*p))
		{
			const auto& tail(p->get());
			auto idx(get_ins_idx());

			for(const auto& tm : tail)
				term.AddChild(MakeIndex(++idx), tm);
			return ret();
		}
	}
	else if(IsList(item))
	{
		auto tail(std::move(item));
		auto idx(get_ins_idx());

		for(auto& tm : tail)
			term.AddChild(MakeIndex(++idx), std::move(tm));
		return ret();
	}
	throw InvalidSyntax("The tail argument shall be a list.");
}

void
LambdaImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	CreateFunction(term, [&, no_lift](TermNode::Container& con){
		auto p_env(ctx.ShareRecord());
		auto i(con.begin());
		auto formals(share_move(Deref(++i)));

		con.erase(con.cbegin(), ++i);
		// NOTE: %ToContextHandler implies strict evaluation of arguments in
		//	%StrictContextHandler::operator().
		return ToContextHandler(VauHandler({}, std::move(formals),
			std::move(p_env), {}, ctx, term, no_lift));
	}, 1);
}

void
VauImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	CreateFunction(term, [&, no_lift](TermNode::Container& con){
		auto p_env(ctx.ShareRecord());
		auto i(con.begin());
		auto formals(share_move(Deref(++i)));
		string eformal(CheckEnvFormal(Deref(++i)));

		con.erase(con.cbegin(), ++i);
		return FormContextHandler(VauHandler(std::move(eformal),
			std::move(formals), std::move(p_env), {}, ctx, term, no_lift));
	}, 2);
}

void
VauWithEnvironmentImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	CreateFunction(term, [&, no_lift](TermNode::Container& con){
		auto i(con.begin());

		ReduceCheckedSync(Deref(++i), ctx);

		// XXX: List components are ignored.
		auto p_env_pr(ResolveEnvironment(Deref(i)));
		auto formals(share_move(Deref(++i)));
		string eformal(CheckEnvFormal(Deref(++i)));

		con.erase(con.cbegin(), ++i);
		return FormContextHandler(VauHandler(std::move(eformal), std::move(
			formals), std::move(p_env_pr.first), p_env_pr.second, ctx, term,
			no_lift));
	}, 3);
}
//@}

} // unnamed namespace;


ReductionStatus
Reduce(TermNode& term, ContextNode& ctx)
{
#if YF_Impl_NPLA1_Enable_Thunked
	// TODO: Support other states?
	ystdex::swap_guard<Reducer> gd(true, ctx.Current);
	ystdex::swap_guard<bool> gd_skip(true, ctx.SkipToNextEvaluation);

#endif
	return ctx.RewriteGuarded(term,
		std::bind(ReduceOnce, std::ref(term), std::ref(ctx)));
}

ReductionStatus
ReduceAgain(TermNode& term, ContextNode& ctx)
{
#if YF_Impl_NPLA1_Enable_Thunked
	auto reduce_again(std::bind(ReduceOnce, std::ref(term), std::ref(ctx)));
#	if YF_Impl_NPLA1_Enable_TCO
	const auto update_fused_act([&](TCOAction& fused_act){
		fused_act.ReduceNestedAsync = true;
	});

	if(const auto p = ctx.Current.target<TCOAction>())
	// XXX: See %ReduceCheckedClosureImpl.
//	if(&p->Term.get() == &term)
	{
		update_fused_act(*p);
		return RelaySwitchedUnchecked(ctx, std::move(reduce_again));
	}

	TCOAction fused_act(ctx, term, {});

	update_fused_act(fused_act);
	return RelayNext(ctx, std::move(reduce_again), std::move(fused_act));
#	else
	return RelayNext(ctx, std::move(reduce_again), CombineActions(ctx, [&]{
		ctx.SkipToNextEvaluation = true;
		return ctx.LastStatus;
	}, ctx.Switch()));
#	endif
#else
	return Reduce(term, ctx);
#endif
}

void
ReduceArguments(TNIter first, TNIter last, ContextNode& ctx)
{
	if(first != last)
		// NOTE: This is neutral to thunks.
		// NOTE: The order of evaluation is unspecified by the language
		//	specification. It should not be depended on.
		ReduceChildren(++first, last, ctx);
	else
		throw InvalidSyntax("Invalid function application found.");
}

ReductionStatus
ReduceChecked(TermNode& term, ContextNode& ctx)
{
#if YF_Impl_NPLA1_Enable_Thunked
	return ReduceCheckedAsync(term, ctx);
#else
	// NOTE: This does not support PTC.
	ReduceCheckedSync(term, ctx);
	return CheckNorm(term);
#endif
}

ReductionStatus
ReduceCheckedClosure(TermNode& term, ContextNode& ctx, bool move,
	TermNode& closure)
{
	return ReduceCheckedClosureImpl(term, ctx, move, closure, {});
}

void
ReduceChildren(TNIter first, TNIter last, ContextNode& ctx)
{
	// NOTE: Separators or other sequence constructs are not handled here. The
	//	evaluation can be potentionally parallel, though the simplest one is
	//	left-to-right.
	// TODO: Use excetion policies to be evaluated concurrently?
	// NOTE: This does not support PTC, but allow it to be called
	//	in routines which expect proper tail actions, given the guarnatee that
	//	the precondition of %Reduce is not violated.
	// XXX: The remained tail action would be dropped.
#if YF_Impl_NPLA1_Enable_Thunked
	ReduceChildrenOrderedAsync(first, last, ctx);
#else
	std::for_each(first, last, ystdex::bind1(ReduceCheckedSync, std::ref(ctx)));
#endif
}

ReductionStatus
ReduceChildrenOrdered(TNIter first, TNIter last, ContextNode& ctx)
{
#if YF_Impl_NPLA1_Enable_Thunked
	return ReduceChildrenOrderedAsync(first, last, ctx);
#else
	// NOTE: This does not support PTC.
	const auto tr([&](TNIter iter){
		return ystdex::make_transform(iter, [&](TNIter i){
			ReduceCheckedSync(*i, ctx);
			// TODO: Simplify?
			return CheckNorm(*i);
		});
	});

	return ystdex::default_last_value<ReductionStatus>()(tr(first), tr(last),
		ReductionStatus::Clean);
#endif
}

ReductionStatus
ReduceFirst(TermNode& term, ContextNode& ctx)
{
	// NOTE: This is neutral to thunks.
	return IsBranch(term) ? ReduceOnce(Deref(term.begin()), ctx)
		: ReductionStatus::Clean;
}

ReductionStatus
ReduceOnce(TermNode& term, ContextNode& ctx)
{
	if(IsBranch(term))
	{
		YAssert(term.size() != 0, "Invalid node found.");
		if(term.size() != 1)
		{
			// NOTE: List evaluation.
#if YF_Impl_NPLA1_Enable_Thunked
			return DelimitActions(ctx.EvaluateList, term, ctx);
#else
			return ctx.EvaluateList(term, ctx);
#endif
		}
		else
		{
			// NOTE: List with single element shall be reduced as the
			//	element.
			LiftFirst(term);
			return ReduceAgain(term, ctx);
		}
	}

	const auto& tp(term.Value.type());

	// NOTE: Empty list or special value token has no-op to do with.
	// TODO: Handle special value token?
#if YF_Impl_NPLA1_Enable_Thunked
	// NOTE: The reduction relies on proper handling of reduction status.
	return tp != ystdex::type_id<void>() && tp != ystdex::type_id<ValueToken>()
		? DelimitActions(ctx.EvaluateLeaf, term, ctx) : ReductionStatus::Clean;
#else
	// NOTE: The reduction relies on proper tail action.
	return tp != ystdex::type_id<void>() && tp != ystdex::type_id<ValueToken>()
		? ctx.EvaluateLeaf(term, ctx) : ReductionStatus::Clean;
#endif
}

ReductionStatus
ReduceOrdered(TermNode& term, ContextNode& ctx)
{
#if YF_Impl_NPLA1_Enable_Thunked
#	if YF_Impl_NPLA1_Enable_TailRewriting
	if(!term.empty())
		return ReduceSequenceOrderedAsync(term, ctx, term.begin());
	term.Value = ValueToken::Unspecified;
	return ReductionStatus::Retained;
#	else
	return RelayNext(ctx, [&]{
		return ReduceChildrenOrdered(term, ctx);
	}, std::bind([&](Reducer& act){
		RelaySwitched(ctx, std::move(act));
		if(!term.empty())
			LiftTerm(term, *term.rbegin());
		else
			term.Value = ValueToken::Unspecified;
		return CheckNorm(term);
	}, ctx.Switch()));
#	endif
#else
	// NOTE: This does not support PTC.
	const auto res(ReduceChildrenOrdered(term, ctx));

	if(!term.empty())
		LiftTerm(term, *term.rbegin());
	else
		term.Value = ValueToken::Unspecified;
	return res;
#endif
}

ReductionStatus
ReduceTail(TermNode& term, ContextNode& ctx, TNIter i)
{
	auto& con(term.GetContainerRef());

	con.erase(con.begin(), i);
	// NOTE: This is neutral to thunks.
	return ReduceAgain(term, ctx);
}


void
SetupTraceDepth(ContextNode& root, const string& name)
{
	yunseq(
	root.GetBindingsRef().Place<size_t>(name),
	root.Guard = [name](TermNode& term, ContextNode& ctx){
		using ystdex::pvoid;
		auto& depth(AccessChild<size_t>(ctx.GetBindingsRef(), name));

		YTraceDe(Informative, "Depth = %zu, context = %p, semantics = %p.",
			depth, pvoid(&ctx), pvoid(&term));
		++depth;
		return ystdex::unique_guard([&]() ynothrow{
			--depth;
		});
	}
	);
}


TermNode
TransformForSeparator(const TermNode& term, const ValueObject& pfx,
	const ValueObject& delim, const TokenValue& name)
{
	return TransformForSeparatorTmpl([&](TermNode& res, TNCIter b, TNCIter e){
		// NOTE: Explicit type 'TermNode' is intended.
		TermNode child(AsIndexNode(res));

		while(b != e)
		{
			child.emplace(b->GetContainer(), MakeIndex(child), b->Value);
			++b;
		}
		res += std::move(child);
	}, term, pfx, delim, name);
}

TermNode
TransformForSeparatorRecursive(const TermNode& term, const ValueObject& pfx,
	const ValueObject& delim, const TokenValue& name)
{
	return TransformForSeparatorTmpl([&](TermNode& res, TNCIter b, TNCIter e){
		while(b != e)
			res += TransformForSeparatorRecursive(*b++, pfx, delim,
				MakeIndex(res));
	}, term, pfx, delim, name);
}

ReductionStatus
ReplaceSeparatedChildren(TermNode& term, const ValueObject& pfx,
	const ValueObject& delim)
{
	if(std::find_if(term.begin(), term.end(),
		ystdex::bind1(HasValue<ValueObject>, std::ref(delim))) != term.end())
		term = TransformForSeparator(term, pfx, delim,
			TokenValue(term.GetName()));
	return ReductionStatus::Clean;
}


ReductionStatus
FormContextHandler::operator()(TermNode& term, ContextNode& ctx) const
{
	// TODO: Is it worth matching specific builtin special forms here?
	// FIXME: Exception filtering does not work well with thunks.
	try
	{
		if(!Check || Check(term))
			return Handler(term, ctx);
		// TODO: Use more specific exception type?
		throw std::invalid_argument("Term check failed.");
	}
	CatchExpr(NPLException&, throw)
	// TODO: Use semantic exceptions.
	CatchThrow(ystdex::bad_any_cast& e, LoggedEvent(ystdex::sfmt(
		"Mismatched types ('%s', '%s') found.", e.from(), e.to()), Warning))
	// TODO: Use nested exceptions?
	CatchThrow(std::exception& e, LoggedEvent(e.what(), Err))
	// XXX: Use distinct status for failure?
	return ReductionStatus::Clean;
}


ReductionStatus
StrictContextHandler::operator()(TermNode& term, ContextNode& ctx) const
{
	// NOTE: This implementes arguments evaluation in applicative order.
#if YF_Impl_NPLA1_Enable_Thunked
	return RelayNext(ctx, [&]{
		ReduceArguments(term, ctx);
		return ReductionStatus::Retrying;
	}, std::bind([&](Reducer& act){
		RelaySwitched(ctx, std::move(act));
		AssertBranch(term, "Invalid state found.");
		// NOTE: Matching function calls.
		// FIXME: Multiple wrapped handlers are not sufficiently thunked.
		return Handler(term, ctx);
	}, ctx.Switch()));
#else
	// NOTE: This does not support PTC.
	ReduceArguments(term, ctx);
	AssertBranch(term, "Invalid state found.");
	// NOTE: Matching function calls.
	return Handler(term, ctx);
#endif
}


void
RegisterSequenceContextTransformer(EvaluationPasses& passes,
	const ValueObject& delim, bool ordered)
{
	passes += ystdex::bind1(ReplaceSeparatedChildren,
		ordered ? ContextHandler(Forms::Sequence)
		: ContextHandler(StrictContextHandler(ReduceBranchToList)), delim);
}


ReductionStatus
EvaluateDelayed(TermNode& term)
{
	return ystdex::call_value_or([&](DelayedTerm& delayed){
		return EvaluateDelayed(term, delayed);
	}, AccessPtr<DelayedTerm>(term), ReductionStatus::Clean);
}
ReductionStatus
EvaluateDelayed(TermNode& term, DelayedTerm& delayed)
{
	// NOTE: This does not rely on tail action.
	// NOTE: The referenced term is lived through the envaluation, which is
	//	guaranteed by the evaluated parent term.
	LiftDelayed(term, delayed);
	// NOTE: To make it work with %DetectReducible.
	return ReductionStatus::Retrying;
}

ReductionStatus
EvaluateIdentifier(TermNode& term, const ContextNode& ctx, string_view id)
{
	auto pr(ResolveName(ctx, id));
	const auto p_node(pr.first);

	if(p_node)
	{
		auto& node(*p_node);

		// NOTE: This is conversion of lvalue in object to value of expression.
		//	The referenced term is lived through the evaluation, which is
		//	guaranteed by the context. This is necessary since the ownership of
		//	objects which are not temporaries in evaluated terms needs to be
		//	always in the environment, rather than in the tree. It would be safe
		//	if not passed directly and without rebinding. Note access of objects
		//	denoted by invalid reference after rebinding would cause undefined
		//	behavior in the object language.
		// NOTE: Reference collapsed.
		if(const auto p_tref = AccessPtr<const TermReference>(node))
			term.Value = *p_tref;
		else
			term.Value = TermReference(node, pr.second.get().Anchor());
		// NOTE: This is not guaranteed to be saved as %ContextHandler in
		//	%ReduceCombined.
		if(const auto p_handler = AccessTermPtr<LiteralHandler>(term))
			return (*p_handler)(ctx);
		// NOTE: Unevaluated term shall be detected and evaluated. See also
		//	$2017-05 @ %Documentation::Workflow::Annual2017.
		return IsLeaf(term) ? (term.Value.type()
			!= ystdex::type_id<TokenValue>() ? EvaluateDelayed(term)
			: ReductionStatus::Clean) : ReductionStatus::Retained;
	}
	throw BadIdentifier(id);
}

ReductionStatus
EvaluateLeafToken(TermNode& term, ContextNode& ctx, string_view id)
{
	YAssertNonnull(id.data());
	// NOTE: Only string node of identifier is tested.
	if(!id.empty())
	{
		// NOTE: The term would be normalized by %ReduceCombined.
		//	If necessary, there can be inserted some additional cleanup to
		//	remove empty tokens, returning %ReductionStatus::Retrying.
		//	Separators should have been handled in appropriate preprocessing
		//	passes.
		const auto lcat(CategorizeBasicLexeme(id));

		switch(lcat)
		{
		case LexemeCategory::Code:
			// TODO: When do code literals need to be evaluated?
			id = DeliteralizeUnchecked(id);
			if(YB_UNLIKELY(id.empty()))
				break;
			YB_ATTR_fallthrough;
		case LexemeCategory::Symbol:
			YAssertNonnull(id.data());
			return CheckReducible(ctx.EvaluateLiteral(term, ctx, id))
				? EvaluateIdentifier(term, ctx, id) : ReductionStatus::Clean;
			// XXX: Empty token is ignored.
			// XXX: Remained reducible?
		case LexemeCategory::Data:
			// XXX: This should be prevented being passed to second pass in
			//	%TermToNamePtr normally. This is guarded by normal form handling
			//	in the loop in %ContextNode::Rewrite with %ReduceOnce.
			term.Value.emplace<string>(Deliteralize(id));
			YB_ATTR_fallthrough;
		default:
			break;
			// TODO: Handle other categories of literal.
		}
	}
	return ReductionStatus::Clean;
}

ReductionStatus
ReduceCombined(TermNode& term, ContextNode& ctx)
{
	if(IsBranch(term))
	{
		auto& fm(Deref(term.begin()));

		// NOTE: This is neutral to thunks.
		if(const auto p_handler = AccessPtr<ContextHandler>(fm))
#if YF_Impl_NPLA1_Enable_Thunked
		{
#	if YF_Impl_NPLA1_Enable_TCO
			// TODO: Optimize for performance using context-dependent store?
			// TODO: This should ideally be a member of handler. However, it
			//	makes no sense before %ContextHandler overloads for
			//	ref-qualifier '&&'.
			auto p(make_unique<ContextHandler>(std::move(*p_handler)));
#	else
			auto p(share_move(*p_handler));
#	endif

			return CombinerReturnThunk(*p, term, ctx, std::move(p));
		}
#else
			return CombinerReturnThunk(ContextHandler(std::move(*p_handler)),
				term, ctx);
#endif
		if(const auto p_handler = AccessTermPtr<ContextHandler>(fm))
			return CombinerReturnThunk(*p_handler, term, ctx);
		// TODO: Capture contextual information in error.
		// TODO: Extract general form information extractor function.
		throw ListReductionFailure(ystdex::sfmt("No matching combiner %s for"
			" operand with %zu argument(s) found.",
			TermToValueString(fm).c_str(), FetchArgumentN(term)));
	}
	return ReductionStatus::Clean;
}

ReductionStatus
ReduceLeafToken(TermNode& term, ContextNode& ctx)
{
	const auto res(ystdex::call_value_or([&](string_view id){
		return EvaluateLeafToken(term, ctx, id);
	// XXX: A term without token is ignored.
	}, TermToNamePtr(term), ReductionStatus::Clean));

	return CheckReducible(res) ? ReduceAgain(term, ctx) : res;
}


Environment::NameResolution
ResolveName(const ContextNode& ctx, string_view id)
{
	YAssertNonnull(id.data());
	return ctx.GetRecordRef().Resolve(id);
}

pair<shared_ptr<Environment>, bool>
ResolveEnvironment(ValueObject& vo)
{
	if(const auto p = vo.AccessPtr<EnvironmentReference>())
		return {p->Lock(), {}};
	if(const auto p = vo.AccessPtr<shared_ptr<Environment>>())
		return {*p, true};
	// TODO: Merge with %Environment::CheckParent?
	Environment::ThrowForInvalidType(vo.type());
}


void
SetupDefaultInterpretation(ContextNode& root, EvaluationPasses passes)
{
	passes += ReduceHeadEmptyList;
	passes += ReduceFirst;
	// TODO: Insert more optional optimized lifted form evaluation passes:
	//	macro expansion, etc.
	// NOTE: This should be the last of list pass for current TCO
	//	implementation.
	passes += ReduceCombined;
	root.EvaluateList = std::move(passes);
	root.EvaluateLeaf = ReduceLeafToken;
}


REPLContext::REPLContext(bool trace)
{
	using namespace std::placeholders;

	SetupDefaultInterpretation(Root,
		std::bind(std::ref(ListTermPreprocess), _1, _2));
	if(trace)
		SetupTraceDepth(Root);
}

TermNode
REPLContext::Perform(string_view unit, ContextNode& ctx)
{
	YAssertNonnull(unit.data());
	if(!unit.empty())
		return Process(Session(unit), ctx);
	throw LoggedEvent("Empty token list found.", Alert);
}

void
REPLContext::Prepare(TermNode& term) const
{
	TokenizeTerm(term);
	Preprocess(term);
}
TermNode
REPLContext::Prepare(const TokenList& token_list) const
{
	auto term(SContext::Analyze(token_list));

	Prepare(term);
	return term;
}
TermNode
REPLContext::Prepare(const Session& session) const
{
	auto term(SContext::Analyze(session));

	Prepare(term);
	return term;
}

void
REPLContext::Process(TermNode& term, ContextNode& ctx) const
{
	Prepare(term);
	Reduce(term, ctx);
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
	return s;
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
BindParameter(ContextNode& ctx, const TermNode& t, TermNode& o)
{
	auto& m(ctx.GetBindingsRef());

	// NOTE: The symbol can be rebound.
	MatchParameter(t, o, [&](TNIter first, TNIter last, string_view id){
		YAssert(ystdex::begins_with(id, "."), "Invalid symbol found.");
		id.remove_prefix(1);
		if(!id.empty())
		{
			const bool by_val(id.front() != '&');
			TermNode::Container con;

			if(!by_val)
				id.remove_prefix(1);
			for(; first != last; ++first)
			{
				auto& b(Deref(first));

				if(by_val)
				{
					LiftToSelf(b);
					con.emplace(b.CreateWith(&ValueObject::MakeMoveCopy),
						MakeIndex(con), b.Value.MakeMoveCopy());
				}
				// TODO: Support xvalue?
				// TODO: Check value ownership?
				// XXX: Moved. This is copy elision in object language.
				else
					// TODO: Other value ownership checks?
					// XXX: Moved. This is copy elision in object language.
					con.emplace(std::move(b.GetContainerRef()), MakeIndex(con),
						std::move(b.Value));
			}
			m[id].SetContent(ValueNode(std::move(con)));
		}
	}, [&](const TokenValue& n, TermNode&& b){
		CheckParameterLeafToken(n, [&]{
			if(!n.empty())
			{
				// NOTE: No duplication check here. The symbol can be rebound.

				const bool by_val(n.front() != '&');

				// NOTE: The operand should have been evaluated. Subnodes in
				//	arguments retained are also transferred.
				if(by_val)
				{
					LiftToSelf(b);
					LiftTermIndirection(m[n], b);
				}
				else
				{
					string_view id(n);

					id.remove_prefix(1);
					// TODO: Support xvalue?
					// TODO: Check value ownership?
					// XXX: Moved. This is copy elision in object language.
					m[id].SetContent(std::move(b.GetContainerRef()),
						std::move(b.Value));
				}
			}
		});
	});
}

void
MatchParameter(const TermNode& t, TermNode& o,
	std::function<void(TNIter, TNIter, const TokenValue&)> bind_trailing_seq,
	std::function<void(const TokenValue&, TermNode&&)> bind_value)
{
	if(IsBranch(t))
	{
		LiftTermRefToSelf(o);

		const auto n_p(t.size());
		const auto n_o(o.size());
		auto last(t.end());

		if(n_p > 0)
		{
			const auto& back(Deref(std::prev(last)));

			if(IsLeaf(back))
			{
				if(const auto p = AccessPtr<TokenValue>(back))
				{
					if(!p->empty() && p->front() == '.')
						--last;
				}
				else
					throw ParameterMismatch(ystdex::sfmt(
						"Invalid term %s found for symbol parameter.",
						TermToValueString(back).c_str()));
			}
		}
		if(n_p == n_o || (last != t.end() && n_o >= n_p - 1))
		{
			auto j(o.begin());

			for(auto i(t.begin()); i != last; yunseq(++i, ++j))
			{
				YAssert(j != o.end(), "Invalid state of operand found.");
				MatchParameter(Deref(i), Deref(j), bind_trailing_seq,
					bind_value);
			}
			if(last != t.end())
			{
				const auto& lastv(Deref(last).Value);

				YAssert(lastv.type() == ystdex::type_id<TokenValue>(),
					"Invalid ellipsis sequence token found.");
				bind_trailing_seq(j, o.end(), lastv.GetObject<TokenValue>());
				YAssert(++last == t.end(), "Invalid state found.");
			}
		}
		else if(last == t.end())
			throw ArityMismatch(n_p, n_o);
		else
			throw ParameterMismatch(
				"Insufficient term found for list parameter.");
	}
	else if(!t.Value)
	{
		if(o)
			throw ParameterMismatch(ystdex::sfmt(
				"Invalid nonempty operand found for empty list parameter,"
					" with value %s.", TermToValueString(o).c_str()));
	}
	else if(const auto p = AccessPtr<TokenValue>(t))
		bind_value(*p, std::move(o));
	else
		throw ParameterMismatch(ystdex::sfmt("Invalid parameter value found"
			" with value %s.", TermToValueString(t).c_str()));
}


void
DefineLazy(TermNode& term, ContextNode& ctx)
{
	DoDefine(term, [&](TermNode& formals){
		BindParameter(ctx, formals, term);
	});
}

void
DefineWithNoRecursion(TermNode& term, ContextNode& ctx)
{
	DoDefine(term, [&](TermNode& formals){
		ReduceCheckedSync(term, ctx);
		BindParameter(ctx, formals, term);
	});
}

void
DefineWithRecursion(TermNode& term, ContextNode& ctx)
{
	DoDefine(term, [&](TermNode& formals){
		RecursiveThunk gd(ctx.GetRecordRef(), formals);

		ReduceCheckedSync(term, ctx);
		BindParameter(ctx, formals, term);
		gd.Commit();
	});
}

void
Undefine(TermNode& term, ContextNode& ctx, bool forced)
{
	Retain(term);
	if(term.size() == 2)
	{
		const auto& n(Access<TokenValue>(Deref(std::next(term.begin()))));

		if(IsNPLASymbol(n))
			term.Value = ctx.GetRecordRef().Remove(n, forced);
		else
			throw InvalidSyntax(ystdex::sfmt("Invalid token '%s' found as name"
				" to be undefined.", n.c_str()));
	}
	else
		throw
			InvalidSyntax("Expected exact one term as name to be undefined.");
}


ReductionStatus
If(TermNode& term, ContextNode& ctx)
{
	Retain(term);

	const auto size(term.size());

	if(size == 3 || size == 4)
	{
		auto i(std::next(term.begin()));

		return ReduceSubsequent(*i, ctx, [&, i]() -> ReductionStatus{
			auto j(i);

			if(!ExtractBool(*j, true))
				++j;
			if(++j != term.end())
			{
				LiftTerm(term, *j);
				return ReduceAgain(term, ctx);
			}
			return ReductionStatus::Clean;
		});
	}
	else
		throw InvalidSyntax("Syntax error in conditional form.");
}

void
Lambda(TermNode& term, ContextNode& ctx)
{
	LambdaImpl(term, ctx, {});
}

void
LambdaRef(TermNode& term, ContextNode& ctx)
{
	LambdaImpl(term, ctx, true);
}

void
Vau(TermNode& term, ContextNode& ctx)
{
	VauImpl(term, ctx, {});
}

void
VauRef(TermNode& term, ContextNode& ctx)
{
	VauImpl(term, ctx, true);
}

void
VauWithEnvironment(TermNode& term, ContextNode& ctx)
{
	VauWithEnvironmentImpl(term, ctx, {});
}

void
VauWithEnvironmentRef(TermNode& term, ContextNode& ctx)
{
	VauWithEnvironmentImpl(term, ctx, true);
}


ReductionStatus
Sequence(TermNode& term, ContextNode& ctx)
{
	Retain(term);
	RemoveHead(term);
	return ReduceOrdered(term, ctx);
}


ReductionStatus
And(TermNode& term, ContextNode& ctx)
{
	return AndOr(term, ctx, true);
}

ReductionStatus
Or(TermNode& term, ContextNode& ctx)
{
	return AndOr(term, ctx, {});
}


void
CallSystem(TermNode& term)
{
	CallUnaryAs<const string>(
		ystdex::compose(usystem, std::mem_fn(&string::c_str)), term);
}

ReductionStatus
Cons(TermNode& term)
{
	return ConsImpl(term, true);
}

ReductionStatus
ConsRef(TermNode& term)
{
	return ConsImpl(term, {});
}

void
Equal(TermNode& term)
{
	EqualTermReference(term, [](const TermNode& x, const TermNode& y){
		const bool lx(IsLeaf(x)), ly(IsLeaf(y));

		return lx && ly ? YSLib::HoldSame(x.Value, y.Value) : &x == &y;
	});
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
	EqualTermReference(term, [](const TermNode& x, const TermNode& y){
		const bool lx(IsLeaf(x)), ly(IsLeaf(y));

		return lx && ly ? x.Value == y.Value : &x == &y;
	});
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

void
EvaluateUnit(TermNode& term, const REPLContext& ctx)
{
	CallUnaryAs<const string>([ctx](const string& unit){
		REPLContext(ctx).Perform(unit);
	}, term);
}

void
MakeEnvironment(TermNode& term)
{
	Retain(term);
	if(term.size() > 1)
	{
		ValueObject parent;
		const auto tr([&](TNCIter iter){
			return ystdex::make_transform(iter, [&](TNCIter i){
				return ReferenceTerm(*i).Value;
			});
		});

		parent.emplace<EnvironmentList>(tr(std::next(term.begin())),
			tr(term.end()));
		term.Value = make_shared<Environment>(std::move(parent));
	}
	else
		term.Value = make_shared<Environment>();
}

void
GetCurrentEnvironment(TermNode& term, ContextNode& ctx)
{
	RetainN(term, 0);
	term.Value = ValueObject(ctx.WeakenRecord());
}

ReductionStatus
ValueOf(TermNode& term, const ContextNode& ctx)
{
	RetainN(term);
	LiftToOther(term, Deref(std::next(term.begin())));
	if(const auto p_id = AccessPtr<string>(term))
		TryRet(EvaluateIdentifier(term, ctx, *p_id))
		CatchIgnore(BadIdentifier&)
	term.Value = ValueToken::Null;
	return ReductionStatus::Clean;
}


ContextHandler
Wrap(const ContextHandler& h)
{
	return ToContextHandler(h);
}

ContextHandler
WrapOnce(const ContextHandler& h)
{
	if(const auto p = h.target<FormContextHandler>())
		return ToContextHandler(*p);
	throw NPLException(ystdex::sfmt("Wrapping failed with type '%s'.",
		h.target_type().name()));
}

ContextHandler
Unwrap(const ContextHandler& h)
{
	if(const auto p = h.target<StrictContextHandler>())
		return ContextHandler(p->Handler);
	throw NPLException(ystdex::sfmt("Unwrapping failed with type '%s'.",
		h.target_type().name()));
}

} // namespace Forms;

} // namesapce A1;

} // namespace NPL;

