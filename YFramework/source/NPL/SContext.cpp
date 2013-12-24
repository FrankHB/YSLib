/*
	© 2012-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SContext.cpp
\ingroup NPL
\brief S 表达式上下文。
\version r1456
\author FrankHB <frankhb1989@gmail.com>
\since build 329
\par 创建时间:
	2012-08-03 19:55:59 +0800
\par 修改时间:
	2013-12-24 00:40 +0800
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

Session::Session(const string& line)
	: llex()
{
	for(const auto& c : line)
		llex.ParseByte(c);
}
Session::Session(const TextFile& tf)
{
	ystdex::ifile_iterator i(tf.GetPtr());

	while(!tf.CheckEOF())
	{
		if(YB_UNLIKELY(is_undereferenceable(i)))
			throw LoggedEvent("Bad Source!", Critical);
		llex.ParseByte(*i);
		++i;
	}
}


namespace SContext
{

TLCIter
Validate(TLCIter b, TLCIter e)
{
	while(b != e && *b != ")")
		if(*b == "(")
		{
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
			auto nd(MakeNode(to_string(node.GetSize())));
			auto res(Reduce(nd, ++b, e));

			if(res == e || *res != ")")
				throw LoggedEvent("Redundant '(' found.", Alert);
			node += std::move(nd);
			b = ++res;
		}
		else
			node += {0, to_string(node.GetSize()), *b++};
	return b;
}

void
Analyze(ValueNode& root, const TokenList& token_list)
{
#if 0
	if(token_list.empty())
		throw LoggedEvent("Empty token list found;", Alert);
#endif
	if(Validate(token_list.begin(), token_list.end()) != token_list.end())
		throw LoggedEvent("Redundant ')' found.", Alert);

	const auto res(Reduce(root, token_list.begin(), token_list.end()));

	yassume(res == token_list.end());
}
void
Analyze(ValueNode& root, const Session& session)
{
	Analyze(root, session.GetTokenList());
}
void
Analyze(ValueNode& root, const string& unit)
{
	Analyze(root, Session(unit).GetTokenList());
}

} // namespace SContext;

} // namespace NPL;

