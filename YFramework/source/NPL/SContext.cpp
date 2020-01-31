/*
	© 2012-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SContext.cpp
\ingroup NPL
\brief S 表达式上下文。
\version r1688
\author FrankHB <frankhb1989@gmail.com>
\since build 329
\par 创建时间:
	2012-08-03 19:55:59 +0800
\par 修改时间:
	2020-01-30 22:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SContext
*/


#include "NPL/YModules.h"
#include YFM_NPL_SContext // for ystdex::ref_eq, std::allocator_arg,
//	YSLib::make_string_view;
#include <ystdex/scope_guard.hpp> // for ystdex::make_guard;

using namespace YSLib;

namespace NPL
{

// XXX: Simplify with %CreateRecursively?
TermNode::Container
TermNode::ConCons(const ValueNode::Container& con)
{
	Container res;

	for(const auto& item : con)
		res.emplace_back(ConCons(item.GetContainer()), item.Value);
	return res;
}
TermNode::Container
TermNode::ConCons(ValueNode::Container&& con)
{
	Container res;

	for(auto& item : con)
		res.emplace_back(ConCons(std::move(item.GetContainerRef())),
			std::move(item.Value));
	return res;
}
TermNode::Container
TermNode::ConCons(const ValueNode::Container& con, allocator_type a)
{
	Container res(a);

	for(const auto& item : con)
		res.emplace_back(ConCons(item.GetContainer()), item.Value);
	return res;
}
TermNode::Container
TermNode::ConCons(ValueNode::Container&& con, allocator_type a)
{
	Container res(a);

	for(auto& item : con)
		res.emplace_back(ConCons(std::move(item.GetContainerRef())),
			std::move(item.Value));
	return res;
}

void
TermNode::MoveContent(TermNode&& node)
{
	YAssert(!ystdex::ref_eq<>()(*this, node), "Invalid self move found.");

	// NOTE: Similar to %ValueNode::MoveContent.
	const auto t(std::move(GetContainerRef()));

	SetContent(std::move(node));
}

void
TermNode::SwapContent(TermNode& term) ynothrowv
{
	SwapContainer(term),
	swap(Value, term.Value);
}

void
swap(TermNode& x, TermNode& y) ynothrowv
{
	x.SwapContent(y);
}


void
Session::DefaultParseByte(LexicalAnalyzer& lexer, char c)
{
	lexer.ParseByte(c);
}

void
Session::DefaultParseQuoted(LexicalAnalyzer& lexer, char c)
{
	lexer.ParseQuoted(c);
}


namespace SContext
{

TLCIter
Validate(TLCIter b, TLCIter e)
{
	size_t left(0);

	for(; b != e; ++b)
		if(*b == "(")
			++left;
		else if(*b == ")")
		{
			if(left != 0)
				--left;
			else
				return b;
		}
	if(left == 0)
		return b;
	throw LoggedEvent("Redundant '(' found.", Alert);
}

TLCIter
Reduce(TermNode& term, TLCIter b, TLCIter e)
{
	const auto a(term.get_allocator());

	return Reduce(term, b, e, [&](const SmallString& str){
		return NPL::AsTermNode(a, std::allocator_arg, a,
			string(YSLib::make_string_view(str), a));
	});
}
TLCIter
Reduce(TermNode& term, TLCIter b, TLCIter e, Tokenizer tokenize)
{
	const auto a(term.get_allocator());
	stack<TermNode> tms(a);
	const auto gd(ystdex::make_guard([&]() ynothrowv{
		YAssert(!tms.empty(), "Invalid state found.");
		term = std::move(tms.top());
	}));

	tms.push(std::move(term));
	for(; b != e; ++b)
		if(*b == "(")
			tms.push(NPL::AsTermNode(a));
		else if(*b != ")")
		{
			YAssert(!tms.empty(), "Invalid state found.");
			tms.top().Add(tokenize(*b));
		}
		else if(tms.size() != 1)
		{
			auto tm(std::move(tms.top()));

			tms.pop();
			YAssert(!tms.empty(), "Invalid state found.");
			tms.top().Add(std::move(tm));
		}
		else
			return b;
	if(tms.size() == 1)
		return b;
	throw LoggedEvent("Redundant '(' found.", Alert);
}

void
Analyze(TermNode& root, const TokenList& token_list)
{
	if(Reduce(root, token_list.cbegin(), token_list.cend())
		!= token_list.cend())
		throw LoggedEvent("Redundant ')' found.", Alert);
}
void
Analyze(TermNode& root, const Session& session)
{
	Analyze(root, session.GetTokenList());
}
void
Analyze(TermNode& root, Tokenizer tokenizer, const TokenList& token_list)
{
	if(Reduce(root, token_list.cbegin(), token_list.cend(),
		std::move(tokenizer)) != token_list.cend())
		throw LoggedEvent("Redundant ')' found.", Alert);
}
void
Analyze(TermNode& root, Tokenizer tokenizer, const Session& session)
{
	Analyze(root, std::move(tokenizer), session.GetTokenList());
}

} // namespace SContext;

} // namespace NPL;

