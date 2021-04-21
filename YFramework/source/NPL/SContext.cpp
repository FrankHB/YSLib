/*
	© 2012-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SContext.cpp
\ingroup NPL
\brief S 表达式上下文。
\version r2061
\author FrankHB <frankhb1989@gmail.com>
\since build 329
\par 创建时间:
	2012-08-03 19:55:59 +0800
\par 修改时间:
	2021-04-15 22:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SContext
*/


#include "NPL/YModules.h"
#include YFM_NPL_SContext // for ystdex::ref_eq, std::allocator_arg,
//	YSLib::make_string_view;

using namespace YSLib;

namespace NPL
{

void
TermNode::ClearContainer() ynothrow
{
	// NOTE: See %~TermNode.
	while(!container.empty())
	{
		const auto i(container.begin());

		// NOTE: Flatten the subterms and insert them before the leftmost
		//	recursive subterm.
		container.splice(i, std::move(NPL::Deref(i).container));
		container.erase(i);
	}
}

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
TermNode::MoveContainer(TermNode&& nd)
{
	YAssert(!ystdex::ref_eq<>()(*this, nd), "Invalid self move found.");

	// NOTE: Similar to %ValueNode::MoveContainer.
	const auto t(std::move(GetContainerRef()));

	SwapContainer(nd);
}

void
TermNode::MoveContent(TermNode&& nd)
{
	YAssert(!ystdex::ref_eq<>()(*this, nd), "Invalid self move found.");

	// NOTE: Similar to %ValueNode::MoveContent.
	const auto t(std::move(*this));

	SetContent(std::move(nd));
}

void
TermNode::MoveValue(TermNode&& nd)
{
	YAssert(!ystdex::ref_eq<>()(*this, nd), "Invalid self move found.");

	// NOTE: Similar to %ValueNode::MoveValue.
	const auto t(std::move(Value));

	Value = std::move(nd.Value);
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
	x.SwapContent(y),
	std::swap(x.Tags, y.Tags);
}


void
ReaderState::UpdateLexeme(const string& lexeme)
{
	if(lexeme.length() == 1)
	{
		const char c(lexeme.front());

		if(c == LeftDelimiter)
			++LeftDelimiterCount;
		else if(c == RightDelimiter)
			++RightDelimiterCount;
	}
}

} // namespace NPL;

