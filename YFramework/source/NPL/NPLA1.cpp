/*
	© 2014-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.cpp
\ingroup NPL
\brief NPLA1 公共接口。
\version r1954
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 18:02:47 +0800
\par 修改时间:
	2016-12-27 21:03 +0800
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

yconstexpr const auto GuardName("__$!");
yconstexpr const auto LeafTermName("__$$@");
yconstexpr const auto ListTermName("__$$");
//! \since build 737
yconstexpr const auto LiteralTermName("__$$@_");

// TODO: Use %ReductionStatus as pass invocation result directly instead of this
//	wrapper. This is not transformed yet to avoid G++ internal compiler error:
//	"error reporting routines re-entered".
//! \since build 730
yconstfn PDefH(ReductionStatus, ToStatus, bool res) ynothrow
	ImplRet(res ? ReductionStatus::NeedRetry : ReductionStatus::Success)

//! \since build 736
yconstfn PDefH(bool, NeedsRetry, ReductionStatus res) ynothrow
	ImplRet(res != ReductionStatus::Success)

//! \since build 736
template<typename _func>
TermNode
TransformForSeparatorTmpl(_func f, const TermNode& term, const ValueObject& pfx,
	const ValueObject& delim, const string& name)
{
	using namespace std::placeholders;
	auto res(AsNode(name, term.Value));

	if(IsBranch(term))
	{
		res += AsIndexNode(res, pfx);
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
	Forms::Quote(term);

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
				return ReductionStatus::Success;
			}
		}
		return ReductionStatus::NeedRetry;
	}
	term.Value = is_and;
	return ReductionStatus::Success;
}

//! \since build 748
template<typename _func>
void
EqualTerm(TermNode& term, _func f)
{
	Forms::QuoteN(term, 2);

	auto i(term.begin());
	const auto& x(Deref(++i));

	term.Value = f(x.Value, Deref(++i).Value);
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
	return InvokePasses<GuardPasses>(GuardName, term, ctx);
}

ReductionStatus
InvokeLeaf(TermNode& term, ContextNode& ctx)
{
	return ToStatus(InvokePasses<EvaluationPasses>(LeafTermName, term, ctx));
}

ReductionStatus
InvokeList(TermNode& term, ContextNode& ctx)
{
	return ToStatus(InvokePasses<EvaluationPasses>(ListTermName, term, ctx));
}

ReductionStatus
InvokeLiteral(TermNode& term, ContextNode& ctx, string_view id)
{
	YAssertNonnull(id.data());
	return
		ToStatus(InvokePasses<LiteralPasses>(LiteralTermName, term, ctx, id));
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
			ValueToken>() ? InvokeLeaf(term, ctx) : ReductionStatus::Success;
	});
}

void
ReduceArguments(TermNode::Container& con, ContextNode& ctx)
{
	if(con.size() > 1)
		// NOTE: The order of evaluation is unspecified by the language
		//	specification. It should not be depended on.
		ReduceChildren(std::next(con.begin()), con.end(), ctx);
	else
		throw InvalidSyntax("Argument not found.");
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
	TermNode app_term(NoContainer, term.GetName());

	if(move)
		LiftTerm(app_term, closure);
	else
		app_term.SetContent(closure);
	// TODO: Test for normal form?
	// XXX: Term reused.
	ReduceChecked(app_term, ctx);
	term.SetContent(std::move(app_term));
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

void
ReduceOrdered(TermNode& term, ContextNode& ctx)
{
	RemoveHead(term);
	// XXX: This relies on current implementation;
	yimpl(ReduceChildren)(term, ctx);
	if(!term.empty())
		LiftTerm(term, term.rbegin()->Value);
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
		auto child(AsIndexNode(res));

		while(b != e)
		{
			child += {b->GetContainer(), MakeIndex(child), b->Value};
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
	return ReductionStatus::Success;
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
	return ReductionStatus::Success;
}


ReductionStatus
StrictContextHandler::operator()(TermNode& term, ContextNode& ctx) const
{
	auto& con(term.GetContainerRef());

	// NOTE: This implementes arguments evaluation in applicative order.
	ReduceArguments(con, ctx);

	const auto n(con.size());

	if(n > 1)
	{
		// NOTE: Matching function calls.
		auto i(con.begin());

		// NOTE: Adjust null list argument application to function call without
		//	arguments.
		if(n == 2 && !Deref(++i))
			con.erase(i);
		// TODO: Add unreduced form check? Is this better to be inserted in
		//	other passes?
#if false
		if(con.empty())
			YTraceDe(Warning, "Empty reduced form found.");
		else
			YTraceDe(Warning, "%zu term(s) not reduced found.", n);
#endif
		return Handler(term, ctx);
	}
	// TODO: Use other exception type for this type of error?
	// TODO: Capture contextual information in error.
	throw ListReductionFailure(ystdex::sfmt("Invalid list form with"
		" %zu term(s) not reduced found.", n), YSLib::Warning);
}


void
RegisterSequenceContextTransformer(EvaluationPasses& passes, ContextNode& node,
	const TokenValue& name, const ValueObject& delim, bool ordered)
{
	// TODO: Simplify by using %ReductionStatus as invocation result directly.
//	passes += ystdex::bind1(ReplaceSeparatedChildren, name, delim);
	passes += ystdex::compose(NeedsRetry,
		ystdex::bind1(ReplaceSeparatedChildren, name, delim));
	RegisterFormContextHandler(node, name, ordered ? ReduceOrdered
		: static_cast<void(&)(TermNode&, ContextNode&)>(ReduceChildren),
		IsBranch);
}


ReductionStatus
EvaluateContextFirst(TermNode& term, ContextNode& ctx)
{
	if(IsBranch(term))
	{
		const auto& fm(Deref(ystdex::as_const(term).begin()));

		if(const auto p_handler = AccessPtr<ContextHandler>(fm))
			return (*p_handler)(term, ctx);
		// TODO: Capture contextual information in error.
		// TODO: Extract general form information extractor function.
		throw ListReductionFailure(
			ystdex::sfmt("No matching form '%s' with %zu argument(s) found.",
			ystdex::call_value_or(std::mem_fn(&string::c_str),
			ystdex::nonnull_or(TermToName(fm), AccessPtr<string>(fm)),
			"#<unknown>"), term.size()));
	}
	return ReductionStatus::Success;
}

ReductionStatus
EvaluateIdentifier(TermNode& term, const ContextNode& ctx, string_view id)
{
	YAssertNonnull(id.data());

	if(const auto p = FetchValuePtr(ctx, id))
	{
		// NOTE: The referenced term is lived through the envaluation, which is
		//	guaranteed by the context.
		LiftTermRef(term, *p);
		if(const auto p_handler = AccessPtr<LiteralHandler>(term))
			return ToStatus((*p_handler)(ctx));
	}
	else
		throw BadIdentifier(id);
	return EvaluateDelayed(term);
}

ReductionStatus
EvaluateLeafToken(TermNode& term, ContextNode& ctx, string_view id)
{
	YAssertNonnull(id.data());
	// NOTE: Only string node of identifier is tested.
	if(!id.empty())
	{
		// NOTE: If necessary, there can be inserted some cleanup to remove
		//	empty tokens, returning %ReductionStatus::NeedRetr. Separators
		//	should have been handled in appropriate preprocess passes.
		const auto lcat(CategorizeLiteral(id));

		switch(lcat)
		{
		case LiteralCategory::Code:
			// TODO: When do code literals need to be evaluated?
			id = DeliteralizeUnchecked(id);
			if(YB_UNLIKELY(id.empty()))
				break;
		case LiteralCategory::None:
			return InvokeLiteral(term, ctx, id) == ReductionStatus::Success
				? ReductionStatus::Success : EvaluateIdentifier(term, ctx, id);
			// XXX: Empty token is ignored.
			// XXX: Remained reducible?
		case LiteralCategory::Data:
			// XXX: This should be prevented being passed to second pass in
			//	%TermToName normally. This is guarded by normal form handling
			//	in the loop in %Reduce.
			term.Value = Deliteralize(id).to_string();
		default:
			break;
			// TODO: Handle other categories of literal.
		}
	}
	return ReductionStatus::Success;
}

ReductionStatus
EvaluateDelayed(TermNode& term)
{
	if(const auto p = AccessPtr<DelayedTerm>(term))
	{
		// NOTE: The referenced term is lived through the envaluation, which is
		//	guaranteed by the evaluated parent term.
		LiftDelayed(term, *p);
		// NOTE: To make it work with %DetectReducible.
		return ReductionStatus::NeedRetry;
	}
	return ReductionStatus::Success;
}

ReductionStatus
ReduceLeafToken(TermNode& term, ContextNode& ctx)
{
	return ystdex::call_value_or([&](string_view id) -> ReductionStatus{
		return EvaluateLeafToken(term, ctx, id);
	// FIXME: Success on node conversion failure?
	}, TermToName(term), ReductionStatus::Success);
}

void
SetupDefaultInterpretation(ContextNode& root, EvaluationPasses passes)
{
	// TODO: Simplify by using %ReductionStatus as invocation result directly
	//	in YSLib. Otherwise current versions of G++ would crash here as internal
	//	compiler error: "error reporting routines re-entered".
//	passes += ReduceFirst;
	passes += ystdex::compose(NeedsRetry, ReduceFirst);
	// TODO: Insert more form evaluation passes: macro expansion, etc.
//	passes += EvaluateContextFirst;
	passes += ystdex::compose(NeedsRetry, EvaluateContextFirst);
	AccessListPassesRef(root) = std::move(passes);
//	AccessLeafPassesRef(root) = ReduceLeafToken;
	AccessLeafPassesRef(root) = ystdex::compose(NeedsRetry, ReduceLeafToken);
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
QuoteN(const TermNode& term, size_t m)
{
	const auto n(FetchArgumentN(term));

	if(n != m)
		throw ArityMismatch(m, n);
	return n;
}


bool
ExtractModifier(TermNode::Container& con, const ValueObject& mod)
{
	YAssert(!con.empty(), "Empty node container found.");
	if(con.size() > 1)
	{
		const auto i(std::next(con.cbegin()));

		// XXX: Modifier is treated as special name.
		if(const auto p = TermToName(Deref(i)))
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

			if(const auto p_id = TermToName(Deref(i_beg)))
			{
				const auto id(*p_id);

				i->GetContainerRef().erase(i_beg);
				Lambda(term, ctx);
				DefineOrSetFor(id, term, ctx, define, mod);
			}
			else
				throw NPLException("Invalid node category found.");
		}
		else if(const auto p_id = TermToName(Deref(i)))
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
			term.ClearTo(ValueToken::Unspecified);
		}
		else
			throw NPLException("Invalid node category found.");
	});
}

void
DefineOrSetFor(const string& id, TermNode& term, ContextNode& ctx, bool define,
	bool mod)
{
	if(CategorizeLiteral(id) == LiteralCategory::None)
		// XXX: Moved.
		// NOTE: Unevaluated term is directly saved.
		(define ? DefineValue : RedefineValue)
			(ctx, id, std::move(term), mod);
	else
		throw InvalidSyntax(define ? "Literal cannot be defined."
			: "Literal cannot be set.");
}

shared_ptr<vector<string>>
ExtractLambdaParameters(const TermNode::Container& con)
{
	YTraceDe(Debug, "Found lambda abstraction form with %zu"
		" parameter(s) to be bound.", con.size());

	// TODO: Blocked. Use C++14 lambda initializers to reduce
	//	initialization cost by directly moving.
	auto p_params(make_shared<vector<string>>());
	set<string_view> svs;

	// TODO: Simplify?
	std::for_each(con.begin(), con.end(), [&](decltype(*con.begin()) pv){
		const auto& name(Access<TokenValue>(pv));

		// FIXME: Missing identifier syntax check.
		// TODO: Throw %InvalidSyntax for invalid syntax.
		if(svs.insert(name).second)
			p_params->push_back(name);
		else
			throw InvalidSyntax(
				sfmt("Duplicate parameter name '%s' found.", name.c_str()));
	});
	return p_params;
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
			LiftTerm(term, *i);
		return ReductionStatus::NeedRetry;
	}
	else
		throw InvalidSyntax("Syntax error in conditional form.");
}

void
Lambda(TermNode& term, ContextNode& ctx)
{
	auto& con(term.GetContainerRef());
	auto size(con.size());

	Quote(term);
	if(size > 1)
	{
		auto i(con.begin());
		const auto p_params(ExtractLambdaParameters((++i)->GetContainer()));

		con.erase(con.cbegin(), ++i);

		// TODO: Optimize. This does not need to be shared, since it would
		//	always be copied, if used.
		const auto p_ctx(make_shared<ContextNode>(ctx));
		const auto p_closure(make_shared<TermNode>(std::move(con),
			term.GetName(), std::move(term.Value)));

		// FIXME: Cyclic reference to '$lambda' context handler when the
		//	term value (i.e. the closure) is copied upward?
		term.Value = ToContextHandler([=](TermNode& app_term){
			auto& params(Deref(p_params));
			const auto n_params(params.size());
			const auto n_terms(app_term.size());

			YTraceDe(Debug, "Lambda called, with %ld shared term(s), %ld shared"
				" context(s), %zu parameter(s).", p_closure.use_count(),
				p_ctx.use_count(), n_params);
			if(n_terms != 0)
			{
				const auto n_args(n_terms - 1);

				if(n_args == n_params)
				{
					auto j(app_term.begin());
					// TODO: Optimize for performance.
					// NOTE: This is probably better to be copy-on-write. Since
					//	no immutable reference would be accessed before
					//	mutation, no care is needed for reference invalidation.
					auto app_ctx(Deref(p_ctx));

					++j;
					// NOTE: Introduce parameters as per lexical scoping rules.
					for(const auto& param : params)
					{
						// XXX: Moved.
						// NOTE: Unevaluated operands are directly saved.
						app_ctx[param].Value = std::move(*j);
						++j;
					}
					YAssert(j == app_term.end(),
						"Invalid state found on passing arguments.");
					// NOTE: Beta reduction.
					// TODO: Implement accurate lifetime analysis rather than
					//	'p_closure.unique()'.
					ReduceCheckedClosure(app_term, app_ctx, {}, *p_closure);
				}
				else
					throw ArityMismatch(n_params, n_args);
			}
			else
				throw LoggedEvent("Invalid application found.", Alert);
		});
		con.clear();
	}
	else
		throw InvalidSyntax("Syntax error in lambda abstraction.");
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
	Forms::CallUnaryAs<const string>([&](const string& cmd){
		term.Value = usystem(cmd.c_str());
	}, term);
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

void
Eval(TermNode& term, const REPLContext& ctx)
{
	Forms::CallUnaryAs<const string>([ctx](const string& unit){
		REPLContext(ctx).Perform(unit);
	}, term);
}

} // namespace Forms;

} // namesapce A1;

} // namespace NPL;

