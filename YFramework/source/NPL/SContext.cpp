/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SContext.cpp
\ingroup NPL
\brief S 表达式上下文。
\version r1426;
\author FrankHB<frankhb1989@gmail.com>
\since build 329 。
\par 创建时间:
	2012-08-03 19:55:59 +0800;
\par 修改时间:
	2012-09-02 20:35 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	NPL::SContext;
*/


#include "NPL/SContext.h"

using namespace YSLib;

YSL_BEGIN_NAMESPACE(NPL)

Session::Session(const string& line)
	: llex()
{
	for(const auto& c : line)
		llex.ParseByte(c);
}
Session::Session(const TextFile& tf)
{
	ystdex::ifile_iterator i(*tf.GetPtr());

	while(!tf.CheckEOF())
	{
		if(YB_UNLIKELY(is_undereferencable(i)))
			throw LoggedEvent("Bad Source!", 0x40);
		llex.ParseByte(*i);
		++i;
	}
}


YSL_BEGIN_NAMESPACE(SContext)

TLCIter
Validate(TLCIter b, TLCIter e)
{
	while(b != e && *b != ")")
		if(*b == "(")
		{
			auto res(Validate(++b, e));

			if(res == e || *res != ")")
				throw LoggedEvent("Redundant '(' found.", 0x20);
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
			ValueNode nd(to_string(node.GetSize()));
			auto res(Reduce(nd, ++b, e));

			if(res == e || *res != ")")
				throw LoggedEvent("Redundant '(' found.", 0x20);
			node.Add(std::move(nd));
			b = ++res;
		}
		else
			node.Add(ValueNode(to_string(node.GetSize()), *b++));
	return b;
}

void
Analyze(ValueNode& root, const TokenList& token_list)
{
#if 0
	if(token_list.empty())
		throw LoggedEvent("Empty token list found;", 0x20);
#endif
	if(Validate(token_list.begin(), token_list.end()) != token_list.end())
		throw LoggedEvent("Redundant ')' found.", 0x20);

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

YSL_END_NAMESPACE(SContext)

YSL_END_NAMESPACE(NPL)

