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
\version r1839
\author FrankHB <frankhb1989@gmail.com>
\since build 329
\par 创建时间:
	2012-08-03 19:55:59 +0800
\par 修改时间:
	2020-05-24 10:55 +0800
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
ByteParser::Update(bool got_delim)
{
	auto& lexer(GetLexerRef());
	const auto& cbuf(GetBuffer());
	const auto len(cbuf.length());

	if(old_length < len)
	{
		if(!lexer.GetUnescapeContext().IsHandling())
		{
			if(YB_UNLIKELY(!update_current))
			{
				lexemes.emplace_back();
				update_current = true;
			}

			auto& cur(lexemes.back());

			if(old_length + 1 == len)
			{
				const char c(cbuf.back());

 				if(got_delim)
				{
					cur += c;
					if(lexer.GetDelimiter() == char())
						update_current = {};
				}
				else if(lexer.GetDelimiter() == char() && IsDelimiter(c))
				{
					if(IsGraphicalDelimiter(c))
					{
						if(cur.empty())
						{
							cur = string({c}, cur.get_allocator());
							update_current = {};
						}
						else
							lexemes.push_back(string({c}, cur.get_allocator()));
					}
					else if(!cur.empty())
						update_current = {};
				}
				else
					cur += c;
			}
			else
				cur += cbuf.substr(old_length, len - old_length);
			old_length = len;
		}
	}
	else if(len + 1 == old_length)
	{
		if(!lexemes.empty())
		{
			auto& cur(lexemes.back());

			if(!cur.empty())
				cur.pop_back();
			else
				lexemes.pop_back();
			--old_length;
		}
		// XXX: Invalid popping back is ignored.
	}
	else if(old_length != len)
		throw LoggedEvent("Invalid parser modified the internal buffer"
			" with other than 0, -1 or 1 character(s).", Alert);
}


LexemeList
DelimitedByteParser::GetTokenList() const
{
	const auto& cbuf(GetBuffer());
	const auto a(cbuf.get_allocator());
	LexemeList res(a);
	size_t i(0);

	std::for_each(qlist.cbegin(), qlist.cend(), [&](size_t s){
		if(s != i)
		{
			DecomposeString(res, string_view(&cbuf[i], s - i));
			i = s;
		}
	});
	if(cbuf.length() != i)
		DecomposeString(res, string_view(&cbuf[i], cbuf.length() - i));
	return res;
}

void
DelimitedByteParser::DecomposeString(LexemeList& lst, bool not_quoted,
	string_view sv)
{
	if(not_quoted)
		lst.splice(lst.end(), Decompose(sv, lst.get_allocator()));
	else
		lst.push_back(LexemeList::value_type(sv.begin(), sv.end()));
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
Analyze(TermNode& root, const LexemeList& lexeme_list)
{
	if(Reduce(root, lexeme_list.cbegin(), lexeme_list.cend())
		!= lexeme_list.cend())
		throw LoggedEvent("Redundant ')' found.", Alert);
}
void
Analyze(TermNode& root, Tokenizer tokenize, const LexemeList& lexeme_list)
{
	if(Reduce(root, lexeme_list.cbegin(), lexeme_list.cend(),
		std::move(tokenize)) != lexeme_list.cend())
		throw LoggedEvent("Redundant ')' found.", Alert);
}

} // namespace SContext;

} // namespace NPL;

