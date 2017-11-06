/*
	© 2014-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.cpp
\ingroup NPL
\brief NPLA1 公共接口。
\version r5030
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-02 18:02:47 +0800
\par 修改时间:
	2017-11-06 21:44 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1 // for ystdex::bind1, unordered_map, ystdex::pvoid,
//	ystdex::call_value_or, ystdex::as_const, ystdex::ref;
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;
#include <ystdex/scope_guard.hpp> // for ystdex::swap_guard,
//	ystdex::unique_guard;
#include YFM_NPL_SContext // for Session;

using namespace YSLib;

namespace NPL
{

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
//@{
bool
ExtractBool(TermNode& term, bool is_and) ynothrow
{
	return ystdex::value_or(AccessTermPtr<bool>(term), is_and) == is_and;
}

void
ExtractTail(ContextNode& ctx, ContextNode::Reducer& acts)
{
	ctx.SetupTail(std::move(acts));
}

template<typename _func, typename _type>
ReductionStatus
ReduceWithCheckedNestedNextActions(TermNode& term, ContextNode& ctx,
	_func extr_tail, ContextNode::Reducer&& cur, _type&& next)
{
	// XXX: Assume it is always reducing the same term and the next actions are
	//	safe to be dropped.
	return NPL::ReduceWithNextActions(term, ctx,
		[&, extr_tail](ContextNode& c, _type& acts, ReductionStatus res){
		if(!CheckReducible(res))
			ystdex::expand_proxy<void(ContextNode&, _type&,
				const ReductionStatus&)>::call(extr_tail, c, acts, res);
		else
			c.SetupBoundedTail(ReduceOnce, term, ctx);
	}, std::move(cur), std::move(next));
}

ReductionStatus
ReduceCheckedNested(TermNode& term, ContextNode& ctx)
{
	return ReduceWithCheckedNestedNextActions(term, ctx, ExtractTail,
		ReduceOnce, ctx.Switch());
}
//@}

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

		const auto and_or_next([&, i, is_and]{
			if(ExtractBool(*i, is_and))
				term.Remove(i);
			else
			{
				term.Value = !is_and;
				return ReductionStatus::Clean;
			}
			return ReduceAgain(term, ctx);
		});

#if true
		return ReduceWithNextActions(term, ctx, ExtractTail,
			std::bind(ReduceCheckedNested, std::ref(*i), std::ref(ctx)),
			ContextNode::Reducer(and_or_next));
#else
		// NOTE: This does not support proper tail calls.
		ReduceChecked(*i, ctx);
		return and_or_next();
#endif
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
		throw NPLException("Invalid cyclic reference found.");
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
	\since build 790
	\todo 优化。考虑使用区域推断代替。
	*/
	//@{
	//! \brief 捕获静态环境作为父环境的指针，包含引入抽象时的静态环境。
	weak_ptr<Environment> p_parent;
	//! \brief 可选保持所有权的静态环境指针。
	shared_ptr<Environment> p_static;
	//@}
	//! \brief 闭包求值构造。
	shared_ptr<TermNode> p_closure;

public:
	/*!
	\pre 形式参数对象指针非空。
	\since build 790
	*/
	VauHandler(string&& ename, shared_ptr<TermNode>&& p_fm,
		shared_ptr<Environment>&& p_env, bool owning,
		const ContextNode& ctx, TermNode& term)
		: eformal(std::move(ename)), p_formals((LiftToSelf(Deref(p_fm)),
		std::move(p_fm))),
		local_prototype(ctx, make_shared<Environment>()), p_parent(p_env),
		p_static(owning ? std::move(p_env) : nullptr),
		p_closure(share_move(term))
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
			const auto& formals(Deref(p_formals));
			// NOTE: Local context: activation record frame with outer scope
			//	bindings.
			// XXX: Referencing escaped variables (now only parameters need
			//	to be cared) form the context would cause undefined behavior
			//	(e.g. returning a reference to automatic object in the host
			//	language). See %BindParameter.
			ContextNode local(local_prototype, make_shared<Environment>());

			// NOTE: Bound dynamic context.
			if(!eformal.empty())
				local.GetBindingsRef().AddValue(eformal,
					ValueObject(ctx.WeakenRecord()));
			// NOTE: Since first term is expected to be saved (e.g. by
			//	%ReduceCombined), it is safe to reduce directly.
			RemoveHead(term);
			// NOTE: Forming beta using parameter binding, to substitute them
			//	as arguments for later closure reducation.
			BindParameter(local, formals, term);
			local.Trace.Log(Debug, [&]{
				return sfmt("Function called, with %ld shared term(s), %ld %s"
					" shared static environment(s), %zu parameter(s).",
					p_closure.use_count(), p_parent.use_count(),
					p_static.use_count() != 0 ? "owning" : "nonowning",
					formals.size());
			});
			// NOTE: Static environment is bound as base of local context by
			//	setting parent environment pointer.
			local.GetRecordRef().Parent = p_parent;
			// TODO: Implement accurate lifetime analysis
			//	depending on 'p_closure.unique()'?
			return ReduceCheckedClosure(term, local, {}, *p_closure);
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

} // unnamed namespace;


ReductionStatus
Reduce(TermNode& term, ContextNode& ctx)
{
	return ctx.RewriteGuarded(term, ReduceOnce);
}

ReductionStatus
ReduceAgain(TermNode& term, ContextNode& ctx)
{
	return ctx.SetupBoundedTail(ReduceOnce, term, ctx);
}

void
ReduceArguments(TNIter first, TNIter last, ContextNode& ctx)
{
	if(first != last)
		// NOTE: The order of evaluation is unspecified by the language
		//	specification. It should not be depended on.
		ReduceChildren(++first, last, ctx);
	else
		throw InvalidSyntax("Invalid function application found.");
}

void
ReduceChecked(TermNode& term, ContextNode& ctx)
{
	// TODO: Replace implementation with CPS'd implementation.
#if false
	ReduceCheckedNested(term, ctx, ReduceOnce, ctx.Switch());
#else
	// NOTE: This does not support proper tail calls.
	CheckedReduceWith(Reduce, term, ctx);
#endif
}

ReductionStatus
ReduceCheckedClosure(TermNode& term, ContextNode& ctx, bool move,
	TermNode& closure)
{
	if(move)
		LiftTerm(term, closure);
	else
		term.SetContent(closure);
	// XXX: Term reused.
	ReduceChecked(term, ctx);
	// TODO: Detect lifetime escape to perform copy elision.
	// NOTE: To keep lifetime of objects referenced by references introduced in
	//	%EvaluateIdentifier sane, %ValueObject::MakeMoveCopy is not enough
	//	because it will not copy objects referenced in holders of
	//	%YSLib::RefHolder instances). On the other hand, the references captured
	//	by vau handlers (which requries recursive copy of vau handler members if
	//	forced) are not blessed here to avoid leak abstraction of detailed
	//	implementation of vau handlers; it can be checked by the vau handler
	//	itself, if necessary.
	LiftToSelf(term);
	// NOTE: The no-op returning of term is the copy elision (unconditionally).
	//	It is equivalent to returning by reference, which can be dangerous.
	term.SetContent(term.CreateWith(&ValueObject::MakeMoveCopy),
		term.Value.MakeMoveCopy());
	return CheckNorm(term);
}

void
ReduceChildren(TNIter first, TNIter last, ContextNode& ctx)
{
	// NOTE: Separators or other sequence constructs are not handled here. The
	//	evaluation can be potentionally parallel, though the simplest one is
	//	left-to-right.
	// TODO: Use excetion policies to be evaluated concurrently?
	std::for_each(first, last, ystdex::bind1(Reduce, std::ref(ctx)));
}

ReductionStatus
ReduceChildrenOrdered(TNIter first, TNIter last, ContextNode& ctx)
{
	const auto tr([&](TNIter iter){
		return ystdex::make_transform(iter, [&](TNIter i){
			return Reduce(*i, ctx);
		});
	});

	return ystdex::default_last_value<ReductionStatus>()(tr(first), tr(last),
		ReductionStatus::Clean);
}

ReductionStatus
ReduceFirst(TermNode& term, ContextNode& ctx)
{
	return IsBranch(term) ? ReduceNested(Deref(term.begin()), ctx)
		: ReductionStatus::Clean;
}

ReductionStatus
ReduceNested(TermNode& term, ContextNode& ctx)
{
#if true
	return ReduceWithNestedNextActions(term, ctx, ExtractTail, ReduceOnce,
		ctx.Switch());
#elif false
	// NOTE: This is not full capable for proper tail calls.
	return ctx.SetupTail(std::bind([&](ContextNode::Reducer& act){
		auto gd(ystdex::unique_guard([&]() ynothrow{
			// XXX: Failure is ignored.
			FilterExceptions([&]{
				ctx.SetupTail(std::move(act));
			});
		}));
		const auto res(Reduce(term, ctx));

		if(CheckReducible(res))
			ystdex::dismiss(gd);
		return res;
	}, ctx.Switch()));
#else
	// NOTE: This does not support proper tail calls.
	ystdex::swap_guard<ContextNode::Reducer> gd(true, ctx.TailAction);
	const auto res(Reduce(term, ctx));

	if(CheckReducible(res))
		gd.dismiss();
	return res;
#endif
}

ReductionStatus
ReduceOnce(TermNode& term, ContextNode& ctx)
{
	if(IsBranch(term))
	{
		YAssert(term.size() != 0, "Invalid node found.");
		if(term.size() != 1)
			// NOTE: List evaluation.
			return ctx.Push(ctx.EvaluateList, term, ctx);
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
	return tp != ystdex::type_id<void>() && tp != ystdex::type_id<ValueToken>()
		? ctx.Push(ctx.EvaluateLeaf, term, ctx) : ReductionStatus::Clean;
}

ReductionStatus
ReduceOrdered(TermNode& term, ContextNode& ctx)
{
	const auto res(ReduceChildrenOrdered(term, ctx));

	if(IsBranch(term))
	{
		if(term.size() > 1)
			LiftTerm(term, *term.rbegin());
		else
			term.Value = ValueToken::Unspecified;
	}
	return res;
}

ReductionStatus
ReduceTail(TermNode& term, ContextNode& ctx, TNIter i)
{
	auto& con(term.GetContainerRef());

	con.erase(con.begin(), i);
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
ReplaceSeparatedChildren(TermNode& term, const ValueObject& name,
	const ValueObject& delim)
{
	if(std::find_if(term.begin(), term.end(),
		ystdex::bind1(HasValue<ValueObject>, std::ref(delim))) != term.end())
		term = TransformForSeparator(term, name, delim,
			TokenValue(term.GetName()));
	return ReductionStatus::Clean;
}


ReductionStatus
FormContextHandler::operator()(TermNode& term, ContextNode& ctx) const
{
	// TODO: Is it worth matching specific builtin special forms here?
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
	ReduceArguments(term, ctx);
	YAssert(IsBranch(term), "Invalid state found.");
	// NOTE: Matching function calls.
	return Handler(term, ctx);
#if false
	// TODO: Use other exception type with more precise information for this
	//	error? Also consider capture of contextual information in error.
	throw ListReductionFailure(ystdex::sfmt("Invalid list form with"
		" %zu term(s) not reduced found.", n), YSLib::Warning);
#endif
}


void
RegisterSequenceContextTransformer(EvaluationPasses& passes, ContextNode& node,
	const TokenValue& name, const ValueObject& delim, bool ordered)
{
	passes += ystdex::bind1(ReplaceSeparatedChildren, name, delim);
	RegisterForm(node, name,
		ordered ? ReduceOrdered : [](TermNode& term, ContextNode& ctx){
		ReduceChildren(term, ctx);
		return ReductionStatus::Retained;
	});
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
	// NOTE: The referenced term is lived through the envaluation, which is
	//	guaranteed by the evaluated parent term.
	LiftDelayed(term, delayed);
	// NOTE: To make it work with %DetectReducible.
	return ReductionStatus::Retrying;
}

ReductionStatus
EvaluateIdentifier(TermNode& term, const ContextNode& ctx, string_view id)
{
	if(const auto p = ResolveName(ctx, id))
	{
		// NOTE: This is conversion of lvalue in object to value of expression.
		//	The referenced term is lived through the evaluation, which is
		//	guaranteed by the context. This is necessary since the ownership of
		//	objects which are not temporaries in evaluated terms needs to be
		//	always in the environment, rather than in the tree. It would be safe
		//	if not passed directly and without rebinding. Note access of objects
		//	denoted by invalid reference after rebinding would cause undefined
		//	behavior in the object language.
		// NOTE: %Reference term is necessary here to implement reference
		//	collapsing.
		term.Value = TermReference(ReferenceTerm(*p));
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
		const auto& fm(Deref(ystdex::as_const(term).begin()));
		const auto ret([&](ReductionStatus res){
			// NOTE: Normalization: Cleanup if necessary.
			if(res == ReductionStatus::Clean)
				term.ClearContainer();
			return res;
		});
#if true
		const auto cleanup_and_extract(
			[ret](ContextNode& c, ContextNode::Reducer& acts,
				ReductionStatus res){
			ret(res);
			ExtractTail(c, acts);
		});

		if(const auto p_handler = AccessPtr<ContextHandler>(fm))
			return ReduceWithNextActions(term, ctx, cleanup_and_extract,
				[&, p_handler]{
				const auto handler(std::move(*p_handler));

				return handler(term, ctx);
			}, ctx.Switch());
		if(const auto p_handler = AccessTermPtr<ContextHandler>(fm))
			return ReduceWithNextActions(term, ctx, cleanup_and_extract,
				[&, p_handler]{
				return (*p_handler)(term, ctx);
			}, ctx.Switch());
#else
		// NOTE: This does not support proper tail calls.
		if(const auto p_handler = AccessPtr<ContextHandler>(fm))
		{
			const auto handler(std::move(*p_handler));

			return ret(handler(term, ctx));
		}
		if(const auto p_handler = AccessTermPtr<ContextHandler>(fm))
			return ret((*p_handler)(term, ctx));
#endif
		else
			// TODO: Capture contextual information in error.
			// TODO: Extract general form information extractor function.
			throw ListReductionFailure(
				ystdex::sfmt("No matching combiner %s for operand with %zu"
					" argument(s) found.", TermToValueString(fm).c_str(),
					FetchArgumentN(term)));
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


observer_ptr<ValueNode>
ResolveName(const ContextNode& ctx, string_view id)
{
	YAssertNonnull(id.data());
	return ctx.GetRecordRef().Resolve(id);
}

pair<shared_ptr<Environment>, bool>
ResolveEnvironment(ValueObject& vo)
{
	if(const auto p = vo.AccessPtr<weak_ptr<Environment>>())
		return {p->lock(), {}};
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

	// TODO: Binding of reference support here, or in %MatchParameter?
	MatchParameter(t, o, [&](TNIter first, TNIter last, string_view id){
		YAssert(ystdex::begins_with(id, "."), "Invalid symbol found.");
		id.remove_prefix(1);
		if(!id.empty())
		{
			TermNode::Container con;

			for(; first != last; ++first)
			{
				auto& b(Deref(first));

				LiftTermRefToSelf(b);
#if true
				con.emplace(b.CreateWith(&ValueObject::MakeMoveCopy),
					MakeIndex(con), b.Value.MakeMoveCopy());
#else
				// XXX: Moved. This is copy elision in object language.
				// NOTE: This is not supported since it should be moved instead.
				con.emplace(std::move(b.GetContainerRef()), MakeIndex(con),
					std::move(b.Value));
#endif
			}
			m[id].SetContent(ValueNode(std::move(con)));
		}
	}, [&](const TokenValue& n, TermNode&& b){
		CheckParameterLeafToken(n, [&]{
			LiftTermRefToSelf(b);
#if true
			// NOTE: The operands should have been evaluated. Children nodes in
			//	arguments retained are also transferred.
			m[n].SetContent(b.CreateWith(&ValueObject::MakeMoveCopy),
				b.Value.MakeMoveCopy());
#else
			// NOTE: The symbol can be rebound.
			// FIXME: Correct key of node when not bound?
			// XXX: Moved. This is copy elision in object language.
			// NOTE: This is not supported since it should be moved instead.
			m[n].SetContent(std::move(b.GetContainerRef()), std::move(b.Value));
#endif
		});
	});
}

void
MatchParameter(const TermNode& t, TermNode& o,
	std::function<void(TNIter, TNIter, const TokenValue&)> bind_trailing_seq,
	std::function<void(const TokenValue&, TermNode&&)> bind_value)
{
	// TODO: Binding of reference support?
	LiftTermRefToSelf(o);
	if(IsBranch(t))
	{
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
					YAssert(!p->empty(), "Invalid token value found.");
					if(p->front() == '.')
						--last;
				}
				else
					// TODO: Merge with %CheckParameterLeafToken?
					throw ParameterMismatch(
						"Invalid token found for symbol parameter.");
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
		ReduceChecked(term, ctx);
		BindParameter(ctx, formals, term);
	});
}

void
DefineWithRecursion(TermNode& term, ContextNode& ctx)
{
	DoDefine(term, [&](TermNode& formals){
		RecursiveThunk gd(ctx.GetRecordRef(), formals);

		ReduceChecked(term, ctx);
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
		const auto if_next([&, i]() -> ReductionStatus{
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

#if true
		return ReduceWithNextActions(term, ctx, ExtractTail,
			std::bind(ReduceCheckedNested, std::ref(*i), std::ref(ctx)),
			ContextNode::Reducer(if_next));
#else
		// NOTE: This does not support proper tail calls.
		ReduceChecked(*i, ctx);
		return if_next();
#endif
	}
	else
		throw InvalidSyntax("Syntax error in conditional form.");
}

void
Lambda(TermNode& term, ContextNode& ctx)
{
	CreateFunction(term, [&](TermNode::Container& con){
		auto p_env(ctx.ShareRecord());
		auto i(con.begin());
		auto formals(share_move(Deref(++i)));

		con.erase(con.cbegin(), ++i);
		// NOTE: %ToContextHandler implies strict evaluation of arguments in
		//	%StrictContextHandler::operator().
		return ToContextHandler(VauHandler({}, std::move(formals),
			std::move(p_env), {}, ctx, term));
	}, 1);
}

void
Vau(TermNode& term, ContextNode& ctx)
{
	CreateFunction(term, [&](TermNode::Container& con){
		auto p_env(ctx.ShareRecord());
		auto i(con.begin());
		auto formals(share_move(Deref(++i)));
		string eformal(CheckEnvFormal(Deref(++i)));

		con.erase(con.cbegin(), ++i);
		return FormContextHandler(VauHandler(std::move(eformal),
			std::move(formals), std::move(p_env), {}, ctx, term));
	}, 2);
}

void
VauWithEnvironment(TermNode& term, ContextNode& ctx)
{
	CreateFunction(term, [&](TermNode::Container& con){
		auto i(con.begin());

		ReduceChecked(Deref(++i), ctx);

		// XXX: List components are ignored.
		auto p_env_pr(ResolveEnvironment(Deref(i)));
		auto formals(share_move(Deref(++i)));
		string eformal(CheckEnvFormal(Deref(++i)));

		con.erase(con.cbegin(), ++i);
		return FormContextHandler(VauHandler(std::move(eformal), std::move(
			formals), std::move(p_env_pr.first), p_env_pr.second, ctx, term));
	}, 3);
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
	RetainN(term, 2);

	auto i(std::next(term.begin(), 2));
	const auto get_ins_idx([&]{
		term.erase(i);
		return GetLastIndexOf(term);
	});
	const auto ret([&]{
		RemoveHead(term);
		return ReductionStatus::Retained;
	});

	// TODO: Simplify?
	if(const auto p = AccessPtr<const TermReference>(Deref(i)))
	{
		if(IsList(*p))
		{
			const auto& tail(p->get());
			auto idx(get_ins_idx());

			for(auto& tm : tail)
				term.AddChild(MakeIndex(++idx), tm);
			return ret();
		}
	}
	else if(IsList(Deref(i)))
	{
		auto tail(std::move(Deref(i)));
		auto idx(get_ins_idx());

		for(auto& tm : tail)
			term.AddChild(MakeIndex(++idx), std::move(tm));
		return ret();
	}
	throw InvalidSyntax("The tail argument shall be a list.");
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
	RetainN(term, 2);

	const auto i(std::next(term.begin()));
	// TODO: Support more environment types?
	ContextNode c(ctx, ResolveEnvironment(Deref(std::next(i))).first);
	auto& closure(Deref(i));

	if(const auto p = AccessPtr<const TermReference>(closure))
		return ReduceCheckedClosure(term, c, {}, *p);
	return ReduceCheckedClosure(term, c, true, closure);
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

