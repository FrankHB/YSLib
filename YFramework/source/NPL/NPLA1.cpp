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
\version r1244
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 18:02:47 +0800
\par 修改时间:
	2016-09-14 09:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1 // for ystdex::bind1, YSLib::RemoveEmptyChildren,
//	ystdex::pvoid;
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

Guard
InvokeGuard(TermNode& term, ContextNode& ctx)
{
	return InvokePasses<GuardPasses>(term, ctx, GuardName);
}

bool
InvokeLeaf(TermNode& term, ContextNode& ctx)
{
	return InvokePasses<EvaluationPasses>(term, ctx, LeafTermName);
}

bool
InvokeList(TermNode& term, ContextNode& ctx)
{
	return InvokePasses<EvaluationPasses>(term, ctx, ListTermName);
}


bool
Reduce(TermNode& term, ContextNode& ctx)
{
	const auto gd(InvokeGuard(term, ctx));

	// NOTE: Rewriting loop until the normal form is got.
	return ystdex::retry_on_cond(ystdex::bind1(DetectReducible, std::ref(term)),
		[&]() -> bool{
		if(!term.empty())
		{
			YAssert(term.size() != 0, "Invalid node found.");
			if(term.size() != 1)
				// NOTE: List evaluation.
				return InvokeList(term, ctx);
			else
			{
				// NOTE: List with single element shall be reduced to its value.
				LiftFirst(term);
				return Reduce(term, ctx);
			}
		}
		else if(!term.Value)
			// NOTE: Empty list.
			term.Value = ValueToken::Null;
		else if(AccessPtr<ValueToken>(term))
			// TODO: Handle special value token.
			;
		else
			return InvokeLeaf(term, ctx);
		// NOTE: Exited loop has produced normal form by default.
		return {};
	});
}

void
ReduceChildren(TermNode::iterator first, TermNode::iterator last,
	ContextNode& ctx)
{
	// NOTE: Separators or other sequence constructs are not handled here. The
	//	evaluation can be potentionally parallel, though the simplest one is
	//	left-to-right.
	// TODO: Use %ExecutionPolicy?
	std::for_each(first, last, ystdex::bind1(Reduce, std::ref(ctx)));
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
	const ValueObject& delim, const string& name)
{
	auto res(AsNode(name, term.Value));

	if(!term.empty())
	{
		res += AsIndexNode(res, pfx);
		ystdex::split(term.begin(), term.end(), std::bind(HasValue,
			std::placeholders::_1, std::ref(delim)), [&](TNCIter b, TNCIter e){
			auto child(AsIndexNode(res));

			while(b != e)
			{
				child += {b->GetContainer(), MakeIndex(child), b->Value};
				++b;
			}
			res += std::move(child);
		});
	}
	return res;
}

TermNode
TransformForSeparatorRecursive(const TermNode& term, const ValueObject& pfx,
	const ValueObject& delim, const string& name)
{
	auto res(AsNode(name, term.Value));

	if(!term.empty())
	{
		res += AsIndexNode(res, pfx);
		ystdex::split(term.begin(), term.end(), std::bind(HasValue,
			std::placeholders::_1, std::ref(delim)), [&](TNCIter b, TNCIter e){
			while(b != e)
				res += TransformForSeparatorRecursive(*b++, pfx, delim,
					MakeIndex(res));
		});
	}
	return res;
}

bool
ReplaceSeparatedChildren(TermNode& term, const ValueObject& name,
	const ValueObject& delim)
{
	if(std::find_if(term.begin(), term.end(),
		ystdex::bind1(HasValue, std::ref(delim))) != term.end())
		term = TransformForSeparator(term, name, delim, term.GetName());
	return {};
}


void
FormContextHandler::operator()(TermNode& term, ContextNode& ctx) const
{
	// TODO: Is it worth matching specific builtin special forms here?
	try
	{
		if(!term.empty())
			Handler(term, ctx);
		else
			// TODO: Use more specific exceptions.
			throw std::invalid_argument("Empty term found.");
	}
	CatchExpr(NPLException&, throw)
	// TODO: Use semantic exceptions.
	CatchThrow(ystdex::bad_any_cast& e, LoggedEvent(
		ystdex::sfmt("Mismatched types ('%s', '%s') found.",
		e.from(), e.to()), Warning))
	// TODO: Use nested exceptions?
	CatchThrow(std::exception& e, LoggedEvent(e.what(), Err))
}


void
FunctionContextHandler::operator()(TermNode& term, ContextNode& ctx) const
{
	auto& con(term.GetContainerRef());

	// NOTE: This implementes arguments evaluation in applicative order.
	ReduceArguments(con, ctx);

	const auto n(con.size());

	if(n > 1)
	{
		// NOTE: Matching function calls.
		auto i(con.begin());

		// NOTE: Adjust null list argument application
		//	to function call without arguments.
		// TODO: Improve performance of comparison?
		if(n == 2 && Deref(++i).Value == ValueToken::Null)
			con.erase(i);
		Handler(term, ctx);
	}
	else
		// TODO: Use other exception type for this type of error?
		// TODO: Capture contextual information in error.
		throw ListReductionFailure(ystdex::sfmt("Invalid list form with"
			" %zu term(s) not reduced found.", n), YSLib::Warning);
	// TODO: Add unreduced form check? Is this better to be inserted in other
	//	passes?
#if false
	if(con.empty())
		YTraceDe(Warning, "Empty reduced form found.");
	else
		YTraceDe(Warning, "%zu term(s) not reduced found.", n);
#endif
}


void
RegisterSequenceContextTransformer(EvaluationPasses& passes, ContextNode& node,
	const string& name, const ValueObject& delim)
{
	passes += ystdex::bind1(ReplaceSeparatedChildren, name, delim);
	NPL::RegisterContextHandler(node, name,
		FormContextHandler([](TermNode& term, ContextNode& ctx){
		RemoveHead(term);
		ReduceChildren(term, ctx);
	}));
}


bool
EvaluateContextFirst(TermNode& term, ContextNode& ctx)
{
	if(!term.empty())
	{
		const auto& fm(Deref(ystdex::as_const(term).begin()));

		if(const auto p_handler = AccessPtr<ContextHandler>(fm))
			(*p_handler)(term, ctx);
		else
		{
			const auto p(AccessPtr<string>(fm));

			// TODO: Capture contextual information in error.
			throw ListReductionFailure(ystdex::sfmt("No matching form '%s'"
				" with %zu argument(s) found.", p ? p->c_str()
				: "#<unknown>", term.size()));
		}
	}
	return {};
}

bool
EvaluateIdentifier(TermNode& term, ContextNode& ctx, const string& id)
{
	if(auto v = FetchValue(ctx, id))
	{
		term.Value = std::move(v);
		if(const auto p_handler = AccessPtr<LiteralHandler>(term))
			return (*p_handler)(ctx);
	}
	else
		throw BadIdentifier(id);
	return {};
}

} // namesapce A1;

} // namespace NPL;

