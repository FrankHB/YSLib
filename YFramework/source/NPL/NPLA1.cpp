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
\version r1113
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 18:02:47 +0800
\par 修改时间:
	2016-05-30 10:27 +0800
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


ValueObject
FetchValue(const ContextNode& ctx, const string& name)
{
	return ystdex::call_value_or<ValueObject>(
		std::mem_fn(&ValueNode::Value), LookupName(ctx, name));
}

observer_ptr<const ValueNode>
LookupName(const ContextNode& ctx, const string& id) ynothrow
{
	return AccessNodePtr(ctx, id);
}


bool
DetectReducible(TermNode& term, bool reducible)
{
	// TODO: Use explicit continuation parameters?
//	if(reducible)
	//	k(term);
	YSLib::RemoveEmptyChildren(term.GetContainerRef());
	// NOTE: Only stopping on getting a normal form.
	return reducible && !term.empty();
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
EvaluateGuard(TermNode& term, ContextNode& ctx)
{
	return InvokePasses<GuardPasses>(term, ctx, GuardName);
}

bool
EvaluateLeafPasses(TermNode& term, ContextNode& ctx)
{
	return InvokePasses<EvaluationPasses>(term, ctx, LeafTermName);
}

bool
EvaluateListPasses(TermNode& term, ContextNode& ctx)
{
	return InvokePasses<EvaluationPasses>(term, ctx, ListTermName);
}


bool
Reduce(TermNode& term, ContextNode& ctx)
{
	const auto gd(EvaluateGuard(term, ctx));

	// NOTE: Rewriting loop until the normal form is got.
	return ystdex::retry_on_cond(std::bind(DetectReducible, std::ref(term),
		std::placeholders::_1), [&]() -> bool{
		if(!term.empty())
		{
			YAssert(term.size() != 0, "Invalid node found.");
			if(term.size() != 1)
				// NOTE: List evaluation.
				return EvaluateListPasses(term, ctx);
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
			return EvaluateLeafPasses(term, ctx);
		// NOTE: Exited loop has produced normal form by default.
		return {};
	});
}

void
ReduceArguments(TermNode::Container& con, ContextNode& ctx)
{
	if(con.size() > 1)
		// NOTE: The order of evaluation is unspecified by the language
		//	specification. However here it can only be either left-to-right
		//	or right-to-left unless the separators has been predicted.
		std::for_each(std::next(con.begin()), con.end(),
			[&](decltype(*con.end())& sub_term){
			Reduce(sub_term, ctx);
		});
	else
		throw LoggedEvent("Invalid term to handle found.", Err);
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
TransformForSeperator(const TermNode& term, const ValueObject& pfx,
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
TransformForSeperatorRecursive(const TermNode& term, const ValueObject& pfx,
	const ValueObject& delim, const string& name)
{
	auto res(AsNode(name, term.Value));

	if(!term.empty())
	{
		res += AsIndexNode(res, pfx);
		ystdex::split(term.begin(), term.end(), std::bind(HasValue,
			std::placeholders::_1, std::ref(delim)), [&](TNCIter b, TNCIter e){
			while(b != e)
				res += TransformForSeperatorRecursive(*b++, pfx, delim,
					MakeIndex(res));
		});
	}
	return res;
}

bool
ReplaceTermForSeperator(TermNode& term, const ValueObject& pfx,
	const ValueObject& delim)
{
	if(std::find_if(term.begin(), term.end(), std::bind(HasValue,
		std::placeholders::_1, std::ref(delim))) != term.end())
		term = TransformForSeperator(term, pfx, delim);
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
	CatchThrow(ystdex::bad_any_cast& e, LoggedEvent(
		ystdex::sfmt("Mismatched types ('%s', '%s') found.",
		e.from(), e.to()), Warning))
	// TODO: Use nest exceptions?
	CatchThrow(std::exception& e, LoggedEvent(e.what(), Err))
}


void
FunctionContextHandler::operator()(TermNode& term, ContextNode& ctx) const
{
	auto& con(term.GetContainerRef());

	// NOTE: Arguments evaluation: applicative order.
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
	// TODO: Unreduced form check.
#if false
	if(n == 0)
		YTraceDe(Warning, "Empty reduced form found.");
	else
		YTraceDe(Warning, "%zu term(s) not reduced found.", n);
#endif
}

} // namesapce A1;

} // namespace NPL;

