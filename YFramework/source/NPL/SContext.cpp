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
\version r1739
\author FrankHB <frankhb1989@gmail.com>
\since build 329
\par 创建时间:
	2012-08-03 19:55:59 +0800
\par 修改时间:
	2020-05-13 13:05 +0800
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


LexemeList
Session::GetTokenList(const ByteParser& parse) const
{
	const auto& cbuf(Lexer.GetBuffer());
	const auto a(cbuf.get_allocator());
	LexemeList res(a);
	const auto decomp([&](string_view sv){
		if(sv.front() != '\'' && sv.front() != '"')
			res.splice(res.end(), Decompose(sv, a));
		else
			res.push_back(LexemeList::value_type(sv.begin(), sv.end()));
	});
	size_t i(0);
	const auto& qlist(parse.GetQuotes());

	std::for_each(qlist.cbegin(), qlist.cend(), [&](size_t s){
		if(s != i)
		{
			decomp(string_view(&cbuf[i], s - i));
			i = s;
		}
	});
	if(cbuf.size() != i)
		decomp(string_view(&cbuf[i], cbuf.size() - i));
	return res;
}


namespace SContext
{

LLCIter
Validate(LLCIter b, LLCIter e)
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

LLCIter
Reduce(TermNode& term, LLCIter b, LLCIter e)
{
	const auto a(term.get_allocator());

	return Reduce(term, b, e, [&](const LexemeList::value_type& str){
		return NPL::AsTermNode(a, std::allocator_arg, a,
			string(YSLib::make_string_view(str), a));
	});
}
LLCIter
Reduce(TermNode& term, LLCIter b, LLCIter e, Tokenizer tokenize)
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
Analyze(TermNode& root, const LexemeList& token_list)
{
	if(Reduce(root, token_list.cbegin(), token_list.cend())
		!= token_list.cend())
		throw LoggedEvent("Redundant ')' found.", Alert);
}
void
Analyze(TermNode& root, const Session& sess, const ByteParser& parse)
{
	Analyze(root, sess.GetTokenList(parse));
}
void
Analyze(TermNode& root, Tokenizer tokenize, const LexemeList& token_list)
{
	if(Reduce(root, token_list.cbegin(), token_list.cend(),
		std::move(tokenize)) != token_list.cend())
		throw LoggedEvent("Redundant ')' found.", Alert);
}
void
Analyze(TermNode& root, Tokenizer tokenize, const Session& sess,
	const ByteParser& parse)
{
	Analyze(root, std::move(tokenize), sess.GetTokenList(parse));
}

} // namespace SContext;

} // namespace NPL;

