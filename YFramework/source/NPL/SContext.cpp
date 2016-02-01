/*
	© 2012-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SContext.cpp
\ingroup NPL
\brief S 表达式上下文。
\version r1527
\author FrankHB <frankhb1989@gmail.com>
\since build 329
\par 创建时间:
	2012-08-03 19:55:59 +0800
\par 修改时间:
	2016-01-28 10:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SContext
*/


#include "NPL/YModules.h"
#include YFM_NPL_SContext

using namespace YSLib;

namespace NPL
{

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
Reduce(ValueNode& node, TLCIter b, TLCIter e)
{
	while(b != e && *b != ")")
		if(*b == "(")
		{
			// FIXME: Potential overflow.
			auto nd(MakeNode(MakeIndex(node)));
			auto res(Reduce(nd, ++b, e));

			if(res == e || *res != ")")
				throw LoggedEvent("Redundant '(' found.", Alert);
			node += std::move(nd);
			b = ++res;
		}
		else
			node += {0, MakeIndex(node), *b++};
	return b;
}

void
Analyze(ValueNode& root, const TokenList& token_list)
{
	if(Validate(token_list.cbegin(), token_list.cend()) != token_list.cend())
		throw LoggedEvent("Redundant ')' found.", Alert);

	const auto res(Reduce(root, token_list.cbegin(), token_list.cend()));

	yassume(res == token_list.end());
}
void
Analyze(ValueNode& root, const Session& session)
{
	Analyze(root, session.GetTokenList());
}
void
Analyze(ValueNode& root, string_view unit)
{
	YAssertNonnull(unit.data());
	Analyze(root, Session(unit).GetTokenList());
}

} // namespace SContext;

} // namespace NPL;

