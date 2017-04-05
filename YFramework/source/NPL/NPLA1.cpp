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
\version r3284
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 18:02:47 +0800
\par 修改时间:
	2017-04-05 13:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1 // for ystdex::bind1, ystdex::pvoid,
//	ystdex::call_value_or, std::mem_fn;
#include YFM_NPL_SContext
#include <ystdex/scope_guard.hpp> // for ystdex::unique_guard;

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

// TODO: Add check for reserved identifier clash?
yconstexpr const auto GuardName("__$!");
yconstexpr const auto LeafTermName("__$$@");
yconstexpr const auto ListTermName("__$$");
//! \since build 737
yconstexpr const auto LiteralTermName("__$$@_");
//! \since build 777
yconstexpr const auto ParentContextName("__$@_parent");

//! \since build 779
yconstfn_relaxed bool
IsReservedName(string_view id) ynothrowv
{
	YAssertNonnull(id.data());
	return ystdex::begins_with(id, "__");
}

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

//! \since build 753
ReductionStatus
AndOr(TermNode& term, ContextNode& ctx, bool is_and)
{
	Forms::Retain(term);

	auto i(term.begin());

	if(++i != term.end())
	{
		if(std::next(i) == term.end())
			LiftTerm(term, *i);
		else
		{
			ReduceChecked(*i, ctx);
			if(ystdex::value_or(i->Value.AccessPtr<bool>(), is_and) == is_and)
				term.Remove(i);
			else
			{
				if(is_and)
					term.Value = false;
				else
					LiftTerm(term, i->Value);
				term.Remove(i);
				return ReductionStatus::Clean;
			}
		}
		return ReductionStatus::Retrying;
	}
	term.Value = is_and;
	return ReductionStatus::Clean;
}

//! \since build 748
template<typename _func>
void
EqualTerm(TermNode& term, _func f)
{
	Forms::RetainN(term, 2);

	auto i(term.begin());
	const auto& x(Deref(++i));

	term.Value = f(x.Value, Deref(++i).Value);
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
	\since build 771
	*/
	shared_ptr<TermNode> p_formals;
	//! \brief 捕获静态上下文，包含引入抽象时的静态环境。
	shared_ptr<ContextNode> p_context;
	//! \brief 闭包对象。
	shared_ptr<TermNode> p_closure;

public:
	//! \since build 769
	VauHandler(TermNode& term, ContextNode& ctx, bool ignore)
		: p_formals([&]{
			using namespace Forms;

			Retain(term);
			if(term.size() > 2)
			{
				auto& con(term.GetContainerRef());
				auto i(con.begin());
				const auto& formals(Deref(++i));

				if(!ignore)
				{
					const auto& eterm(Deref(++i));

					if(const auto p = TermToNamePtr(eterm))
					{
						eformal = *p;
						YTraceDe(Debug, "Found context parameter name '%s'.",
							eformal.c_str());
						if(eformal == "#ignore")
							eformal.clear();
						else if(!IsNPLASymbol(eformal))
							throw InvalidSyntax("Symbol or '#ignore' expected"
								" for context parameter.");
					}
					else
						throw InvalidSyntax("Invalid context parameter found.");
				}
				YTraceDe(Debug, "Found operator with %zu parameter(s) to be"
					" bound.", formals.size());
				auto res(make_shared<TermNode>(std::move(formals)));

				con.erase(con.cbegin(), ++i);
				return res;
			}
			else
				throw InvalidSyntax(
					"Insufficient terms in function abstraction.");
		}()),
		// NOTE: Capturing by reference.
		// TODO: Optimize. This does not need to be shared, since it would
		//	always be copied, if used.
		// TODO: Region inference?
		// FIXME: This may be unsafe if the external owner is destroyed.
		p_context(make_shared<ContextNode>(ctx)),
		p_closure(make_shared<TermNode>(std::move(term)))
	{}

	//! \since build 772
	ReductionStatus
	operator()(TermNode& term, ContextNode& ctx) const
	{
		if(IsBranch(term))
		{
			// FIXME: Cyclic reference to context handler when the term value
			//	(i.e. the closure) is copied upward?
			using namespace Forms;
			const auto& formals(Deref(p_formals));
			// NOTE: Active record frame with outer scope bindings.
			// TODO: Optimize for performance.
			// NOTE: This is probably better to be copy-on-write. Since
			//	no immutable reference would be accessed before
			//	mutation, no care is needed for reference invalidation.
			// TODO: Optimize using initialization from iterator pair?
			// XXX: Referencing escaped variables (now only parameters need
			//	to be cared) form the context would cause undefined behavior
			//	(e.g. returning a reference to automatic object in the host
			//	language).
			// TODO: Reduce such undefined behavior resonably?
			ContextNode comp_ctx;

			// NOTE: Bind dynamic context.
			if(!eformal.empty())
				comp_ctx.AddValue(eformal, ValueObject(ctx, OwnershipTag<>()));
			// NOTE: Since first term is expected to be saved (e.g. by
			//	%ReduceCombined), it is safe to reduce directly.
			RemoveHead(term);
			BindParameter(comp_ctx, formals, term);
			YTraceDe(Debug, "Function called, with %ld shared term(s), %ld"
				" shared context(s), %zu parameter(s).", p_closure.use_count(),
				p_context.use_count(), formals.size());
			YAssert(&comp_ctx != &Deref(p_context),
				"Self reference of context found.");
			// NOTE: Static context is bound by setting parent context pointer.
			// NOTE: Shared ownership is necessary here to prevent the context
			//	disposed too early after the vau handler has been destroyed. The
			//	context has to live longer if there exists the child to capture
			//	the context and then return. And there cannot be cyclic
			//	reference.
			comp_ctx.AddValue(ParentContextName, make_weak(p_context));
			// NOTE: Beta reduction.
			// TODO: Implement accurate lifetime analysis rather than
			//	'p_closure.unique()'.
			ReduceCheckedClosure(term, comp_ctx, {}, *p_closure);
			return CheckNorm(term);
		}
		else
			throw LoggedEvent("Invalid composition found.", Alert);
	}
};

//! \since build 779
observer_ptr<ContextNode>
ResolveShared(string_view id, const shared_ptr<ContextNode>& p_shared)
{
	if(p_shared)
		return make_observer(get_raw(p_shared));
	// TODO: Use concrete semantic failure exception.
	throw NPLException(
		sfmt("Invalid reference found for%s name '%s', probably due to invalid"
			" context access by danling reference.", IsReservedName(id)
			? " reserved" : "", id.data()));
}

} // unnamed namespace;

GuardPasses&
AccessGuardPassesRef(ContextNode& ctx)
{
	return ctx.Place<GuardPasses>(GuardName);
}

EvaluationPasses&
AccessLeafPassesRef(ContextNode& ctx)
{
	return ctx.Place<EvaluationPasses>(LeafTermName);
}

EvaluationPasses&
AccessListPassesRef(ContextNode& ctx)
{
	return ctx.Place<EvaluationPasses>(ListTermName);
}

LiteralPasses&
AccessLiteralPassesRef(ContextNode& ctx)
{
	return ctx.Place<LiteralPasses>(LiteralTermName);
}

Guard
InvokeGuard(TermNode& term, ContextNode& ctx)
{
	return InvokePasses<GuardPasses>(ResolveName(ctx, GuardName), term, ctx);
}

ReductionStatus
InvokeLeaf(TermNode& term, ContextNode& ctx)
{
	return InvokePasses<EvaluationPasses>(ResolveName(ctx, LeafTermName), term,
		ctx);
}

ReductionStatus
InvokeList(TermNode& term, ContextNode& ctx)
{
	return InvokePasses<EvaluationPasses>(ResolveName(ctx, ListTermName), term,
		ctx);
}

ReductionStatus
InvokeLiteral(TermNode& term, ContextNode& ctx, string_view id)
{
	YAssertNonnull(id.data());
	return InvokePasses<LiteralPasses>(ResolveName(ctx, LiteralTermName), term,
		ctx, id);
}


ReductionStatus
Reduce(TermNode& term, ContextNode& ctx)
{
	const auto gd(InvokeGuard(term, ctx));

	// NOTE: Rewriting loop until the normal form is got.
	return ystdex::retry_on_cond(CheckReducible, [&]() -> ReductionStatus{
		if(IsBranch(term))
		{
			YAssert(term.size() != 0, "Invalid node found.");
			if(term.size() != 1)
				// NOTE: List evaluation.
				return InvokeList(term, ctx);
			else
			{
				// NOTE: List with single element shall be reduced as the
				//	element.
				LiftFirst(term);
				return Reduce(term, ctx);
			}
		}

		const auto& tp(term.Value.GetType());

		// NOTE: Empty list or special value token has no-op to do with.
		// TODO: Handle special value token?
		return tp != ystdex::type_id<void>() && tp != ystdex::type_id<
			ValueToken>() ? InvokeLeaf(term, ctx) : ReductionStatus::Clean;
	});
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
	CheckedReduceWith(Reduce, term, ctx);
}

void
ReduceCheckedClosure(TermNode& term, ContextNode& ctx, bool move,
	TermNode& closure)
{
	TermNode comp_term(NoContainer, term.GetName());

	if(move)
		LiftTerm(comp_term, closure);
	else
		comp_term.SetContent(closure);
	// TODO: Test for normal form?
	// XXX: Term reused.
	ReduceChecked(comp_term, ctx);
	term.SetContent(std::move(comp_term));
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
	return IsBranch(term) ? Reduce(Deref(term.begin()), ctx)
		: ReductionStatus::Clean;
}

ReductionStatus
ReduceOrdered(TermNode& term, ContextNode& ctx)
{
	const auto res(ReduceChildrenOrdered(term, ctx));

	if(IsBranch(term))
		LiftTerm(term, *term.rbegin());
	return res;
}

ReductionStatus
ReduceTail(TermNode& term, ContextNode& ctx, TNIter i)
{
	auto& con(term.GetContainerRef());

	con.erase(con.begin(), i);
	return Reduce(term, ctx);
}


void
SetupTraceDepth(ContextNode& root, const string& name)
{
	yunseq(
	root.Place<size_t>(name),
	AccessGuardPassesRef(root) = [name](TermNode& term, ContextNode& ctx){
		using ystdex::pvoid;
		auto& depth(AccessChild<size_t>(ctx, name));

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
	CatchThrow(ystdex::bad_any_cast& e, LoggedEvent(
		ystdex::sfmt("Mismatched types ('%s', '%s') found.",
		e.from(), e.to()), Warning))
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
	YAssertNonnull(id.data());
	if(const auto p = ResolveName(ctx, id))
	{
		// NOTE: The referenced term is lived through the envaluation, which is
		//	guaranteed by the context.
		if(p->empty())
			LiftTermRef(term, p->Value);
		else
			// XXX: Children are copied.
			term.SetContentIndirect(p->GetContainer(), p->Value);
		if(const auto p_handler = AccessPtr<LiteralHandler>(term))
			return (*p_handler)(ctx);
		// NOTE: Unevaluated term shall be detected and evaluated. See also
		//	$2017-02 @ %Documentation::Workflow::Annual2017.
		return IsLeaf(term) ? (term.Value.GetType()
			!= ystdex::type_id<TokenValue>() ? EvaluateDelayed(term)
			: ReductionStatus::Retrying) : ReductionStatus::Retained;
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
		//	Separators should have been handled in appropriate preprocess passes.
		const auto lcat(CategorizeBasicLexeme(id));

		switch(lcat)
		{
		case LexemeCategory::Code:
			// TODO: When do code literals need to be evaluated?
			id = DeliteralizeUnchecked(id);
			if(YB_UNLIKELY(id.empty()))
				break;
		case LexemeCategory::Symbol:
			return CheckReducible(InvokeLiteral(term, ctx, id))
				? EvaluateIdentifier(term, ctx, id) : ReductionStatus::Clean;
			// XXX: Empty token is ignored.
			// XXX: Remained reducible?
		case LexemeCategory::Data:
			// XXX: This should be prevented being passed to second pass in
			//	%TermToNamePtr normally. This is guarded by normal form handling
			//	in the loop in %Reduce.
			term.Value.emplace<string>(Deliteralize(id));
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

		if(const auto p_handler = AccessPtr<ContextHandler>(fm))
		{
			const auto handler(std::move(*p_handler));
			const auto res(handler(term, ctx));

			// NOTE: Normalization: Cleanup if necessary.
			if(res == ReductionStatus::Clean)
				term.ClearContainer();
			return res;
		}
		// TODO: Capture contextual information in error.
		// TODO: Extract general form information extractor function.
		throw ListReductionFailure(
			sfmt("No matching combiner '%s' for operand with %zu argument(s)"
				" found.", [&](observer_ptr<const string> p){
				return
					p ? *p : sfmt("#<unknown:%s>", fm.Value.GetType().name());
			}(TermToNamePtr(fm)).c_str(), FetchArgumentN(term)));
	}
	return ReductionStatus::Clean;
}

ReductionStatus
ReduceLeafToken(TermNode& term, ContextNode& ctx)
{
	return ystdex::call_value_or([&](string_view id){
		return EvaluateLeafToken(term, ctx, id);
	// XXX: A term without token is ignored.
	}, TermToNamePtr(term), ReductionStatus::Clean);
}


observer_ptr<const ValueNode>
ResolveName(const ContextNode& ctx, string_view id)
{
	YAssertNonnull(id.data());

	observer_ptr<const ValueNode> p;
	auto ctx_ref(ystdex::ref(ctx));

	ystdex::retry_on_cond(
		[&](observer_ptr<const ContextNode> p_ctx) ynothrow -> bool{
		if(p_ctx)
		{
			ctx_ref = ystdex::ref(Deref(p_ctx));
			return true;
		}
		return {};
	}, [&, id]() -> observer_ptr<const ContextNode>{
		if((p = LookupName(ctx_ref, id)))
			return {};
		if(const auto p_parent = FetchValuePtr(ctx_ref, ParentContextName))
		{
			const auto& tp(p_parent->GetType());

			if(tp == ystdex::type_id<observer_ptr<const ContextNode>>())
				return p_parent->GetObject<observer_ptr<const ContextNode>>();
			if(tp == ystdex::type_id<weak_ptr<ContextNode>>())
				return ResolveShared(id,
					p_parent->GetObject<weak_ptr<ContextNode>>().lock());
			if(tp == ystdex::type_id<shared_ptr<ContextNode>>())
				return ResolveShared(id,
					p_parent->GetObject<shared_ptr<ContextNode>>());
		}
		return {};
	});
	return p;
}


void
SetupDefaultInterpretation(ContextNode& root, EvaluationPasses passes)
{
	passes += ReduceHeadEmptyList;
	passes += ReduceFirst;
	// TODO: Insert more optional optimized lifted form evaluation passes:
	//	macro expansion, etc.
	passes += ReduceCombined;
	AccessListPassesRef(root) = std::move(passes);
	AccessLeafPassesRef(root) = ReduceLeafToken;
}


REPLContext::REPLContext(bool trace)
{
	using namespace std::placeholders;

	SetupDefaultInterpretation(Root,
		std::bind(std::ref(ListTermPreprocess), _1, _2));
	if(trace)
		SetupTraceDepth(Root);
}

void
REPLContext::LoadFrom(std::istream& is)
{
	if(is)
	{
		if(const auto p = is.rdbuf())
			LoadFrom(*p);
		else
			throw std::invalid_argument("Invalid stream buffer found.");
	}
	else
		throw std::invalid_argument("Invalid stream found.");
}
void
REPLContext::LoadFrom(std::streambuf& buf)
{
	using s_it_t = std::istreambuf_iterator<char>;

	Process(Session(s_it_t(&buf), s_it_t()));
}

TermNode
REPLContext::Perform(string_view unit)
{
	YAssertNonnull(unit.data());
	if(!unit.empty())
		return Process(Session(unit));
	throw LoggedEvent("Empty token list found.", Alert);
}

void
REPLContext::Process(TermNode& term)
{
	TokenizeTerm(term);
	Preprocess(term);
	Reduce(term, Root);
}
TermNode
REPLContext::Process(const TokenList& token_list)
{
	auto term(SContext::Analyze(token_list));

	Process(term);
	return term;
}
TermNode
REPLContext::Process(const Session& session)
{
	auto term(SContext::Analyze(session));

	Process(term);
	return term;
}


namespace Forms
{

size_t
RetainN(const TermNode& term, size_t m)
{
	const auto n(FetchArgumentN(term));

	if(n != m)
		throw ArityMismatch(m, n);
	return n;
}


void
BindParameter(ContextNode& e, const TermNode& t, TermNode& o)
{
	if(IsBranch(t))
	{
		if(IsBranch(o))
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
						if(*p == "...")
							--last;
				}
			}
			if(n_p == n_o || (last != t.end() && n_o >= n_p - 1))
			{
				auto j(o.begin());

				for(auto i(t.begin()); i != last; yunseq(++i, ++j))
				{
					YAssert(j != o.end(), "Invalid state of operand found.");
					BindParameter(e, Deref(i), Deref(j));
				}
				if(last != t.end())
				{
					TermNode::Container con;

					for(; j != o.end(); ++j)
					{
						auto& b(Deref(j));

						// TODO: Merge with static binding implementation?
						// TODO: How to reduce unnecessary copy of retained
						//	list?
						con.emplace(b.CreateWith(IValueHolder::Move),
							MakeIndex(con), b.Value.MakeIndirect());
					}
					e.emplace(std::move(con), "...");
					YAssert(++last == t.end(), "Invalid state found.");
				}
			}
			else if(last == t.end())
				throw ArityMismatch(n_p, n_o);
			else
				throw ParameterMismatch(
					"Insufficient term found for list parameter.");
		}
		else
			throw ParameterMismatch(
				"Invalid leaf term found for non-empty list parameter.");
	}
	else if(!t.Value)
	{
		if(o)
			throw ParameterMismatch(
				"Invalid branch term found for empty list parameter.");
	}
	else if(const auto p = AccessPtr<TokenValue>(t))
		BindParameterLeaf(e, *p, std::move(o));
	else
		throw ParameterMismatch("Invalid parameter value found.");
}

void
BindParameterLeaf(ContextNode& e, const TokenValue& n,
	TermNode::Container&& con, ValueObject&& vo)
{
	if(n != "#ignore")
	{
		if(!n.empty() && IsNPLASymbol(n))
			// NOTE: The symbol can be rebound.
			// NOTE: The operands should have been evaluated if this is
			//	in a lambda. Children nodes in arguments retained are
			//	also transferred.
			// XXX: Moved. This is copy elision in object language.
			e[n].SetContent(std::move(con), std::move(vo));
		else
			throw ParameterMismatch(
				"Invalid token found for symbol parameter.");
	}
}


bool
ExtractModifier(TermNode::Container& con, const ValueObject& mod)
{
	YAssert(!con.empty(), "Empty node container found.");
	if(con.size() > 1)
	{
		const auto i(std::next(con.cbegin()));

		// XXX: Modifier is treated as special name.
		if(const auto p = TermToNamePtr(Deref(i)))
			if(*p == mod)
			{
				con.erase(i);
				return true;
			}
	}
	return {};
}


void
DefineOrSet(TermNode& aterm, ContextNode& actx, bool define)
{
	ReduceWithModifier(aterm, actx,
		[=](TermNode& term, ContextNode& ctx, bool mod){
		auto& con(term.GetContainerRef());
		auto i(con.begin());

		++i;
		if(!i->empty())
		{
			const auto i_beg(i->begin());

			if(const auto p_id = TermToNamePtr(Deref(i_beg)))
			{
				const auto id(*p_id);

				i->GetContainerRef().erase(i_beg);
				Lambda(term, ctx);
				DefineOrSetFor(id, term, ctx, define, mod);
			}
			else
				throw InvalidSyntax("Invalid term found.");
		}
		else if(const auto p_id = TermToNamePtr(Deref(i)))
		{
			const auto id(*p_id);

			YTraceDe(Debug, "Found identifier '%s'.", id.c_str());
			if(++i != con.end())
			{
				CheckedReduceWith(ReduceTail, term, ctx, i);
				DefineOrSetFor(id, term, ctx, define, mod);
			}
			else if(define)
				RemoveIdentifier(ctx, id, mod);
			else
				throw InvalidSyntax("Source operand not found.");
		}
		else
			throw NPLException("Invalid node category found.");
		term.Value = ValueToken::Unspecified;
	});
}

void
DefineOrSetFor(string_view id, TermNode& term, ContextNode& ctx, bool define,
	bool mod)
{
	YAssertNonnull(id.data());
	if(!id.empty() && IsNPLASymbol(id))
		// XXX: Moved.
		// NOTE: Unevaluated term is directly saved.
		(define ? DefineValue : RedefineValue)
			(ctx, id, std::move(term.Value), mod);
	else
		throw InvalidSyntax(sfmt("Invalid token '%s' cannot be %s.", id.data(),
			define ? "defined" : "set"));
}

ReductionStatus
If(TermNode& term, ContextNode& ctx)
{
	const auto size(term.size());

	if(size == 3 || size == 4)
	{
		auto i(term.begin());

		ReduceChecked(Deref(++i), ctx);
		if(!ystdex::value_or(i->Value.AccessPtr<bool>()))
			++i;
		if(++i != term.end())
		{
			LiftTerm(term, *i);
			return ReductionStatus::Retrying;
		}
	}
	else
		throw InvalidSyntax("Syntax error in conditional form.");
	return ReductionStatus::Clean;
}

void
Lambda(TermNode& term, ContextNode& ctx)
{
	// NOTE: %ToContextHandler implies strict evaluation of arguments in
	//	%StrictContextHandler::operator().
	term.Value = ToContextHandler(VauHandler(term, ctx, true));
}

void
Vau(TermNode& term, ContextNode& ctx)
{
	term.Value = ContextHandler(FormContextHandler(VauHandler(term, ctx, {})));
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

	if(IsList(Deref(i)))
	{
		auto tail(std::move(Deref(i)));

		term.erase(i);
		for(auto& tm : tail)
			AppendTerm(term, tm);
		RemoveHead(term);
	}
	else
		throw InvalidSyntax("The tail argument shall be a list.");
	return ReductionStatus::Retained;
}

void
EqualReference(TermNode& term)
{
	EqualTerm(term, YSLib::HoldSame);
}

void
EqualValue(TermNode& term)
{
	EqualTerm(term, ystdex::equal_to<>());
}

ReductionStatus
Eval(TermNode& term)
{
	RetainN(term, 2);

	const auto i(std::next(term.begin()));
	auto& ctx(Access<ContextNode>(Deref(std::next(i))));

	LiftTerm(term, Deref(i));
	return Reduce(term, ctx);
}

void
EvaluateUnit(TermNode& term, const REPLContext& ctx)
{
	CallUnaryAs<const string>([ctx](const string& unit){
		REPLContext(ctx).Perform(unit);
	}, term);
}

bool
IsSymbol(const string& id) ynothrow
{
	return IsNPLASymbol(id);
}

ReductionStatus
ValueOf(TermNode& term, const ContextNode& ctx)
{
	RetainN(term);
	LiftTerm(term, Deref(std::next(term.begin())));
	if(const auto p_id = AccessPtr<string>(term))
		TryRet(EvaluateIdentifier(term, ctx, *p_id))
		CatchIgnore(BadIdentifier&)
	term.Value = ValueToken::Null;
	return ReductionStatus::Clean;
}

} // namespace Forms;

} // namesapce A1;

} // namespace NPL;

