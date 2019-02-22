/*
	© 2012-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SContext.cpp
\ingroup NPL
\brief S 表达式上下文。
\version r1620
\author FrankHB <frankhb1989@gmail.com>
\since build 329
\par 创建时间:
	2012-08-03 19:55:59 +0800
\par 修改时间:
	2019-02-14 14:51 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SContext
*/


#include "NPL/YModules.h"
#include YFM_NPL_SContext // for std::allocator_arg;

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
	while(b != e && *b != ")")
		if(*b == "(")
		{
			// FIXME: Potential overflow.
			auto res(Validate(++b, e));

			if(res == e || *res != ")")
				throw LoggedEvent("Redundant '(' found.", Alert);
			b = ++res;
		}
		else
			++b;
	return b;
}

TLCIter
Reduce(TermNode& term, TLCIter b, TLCIter e)
{
	const auto a(term.get_allocator());

	while(b != e && *b != ")")
		if(*b == "(")
		{
			// FIXME: Potential overflow.
			auto tm(NPL::AsTermNode(a));
			auto res(Reduce(tm, ++b, e));

			if(res == e || *res != ")")
				throw LoggedEvent("Redundant '(' found.", Alert);
			term.Add(std::move(tm));
			b = ++res;
		}
		else
			// TODO: More specific pool for %ValueObject?
			term.Add(NPL::AsTermNode(a, std::allocator_arg, a, *b++));
	return b;
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

} // namespace SContext;

} // namespace NPL;

