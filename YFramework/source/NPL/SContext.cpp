/*
	© 2012-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SContext.cpp
\ingroup NPL
\brief S 表达式上下文。
\version r2120
\author FrankHB <frankhb1989@gmail.com>
\since build 329
\par 创建时间:
	2012-08-03 19:55:59 +0800
\par 修改时间:
	2022-03-14 18:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SContext
*/


#include "NPL/YModules.h"
#include YFM_NPL_SContext // for ystdex::ref_eq;

namespace NPL
{

void
TermNode::ClearContainer() ynothrow
{
	// NOTE: See %~TermNode.
#if true
	while(!container.empty())
	{
		const auto i(container.begin());

		// NOTE: Flatten the subterms and insert them before the leftmost
		//	recursive subterm.
		container.splice(i, std::move(NPL::Deref(i).container));
		container.erase(i);
	}
#else
	// NOTE: The alternative implementation may be more efficient, but only
	//	for cases with sufficient large amount of subterms.
	for(auto i(container.begin()); i != container.end(); ++i)
		// NOTE: The loop invariant is that no subterms left to 'i' have not
		//	finished the container cleanup (with moved-after state) and the
		//	%Value in these terms are kept in a prefix DFS traverse order in
		//	the resulted sequence. Flatten the subterms and insert them after
		//	'*i' to keep the loop invariant at first.
		container.splice(std::next(i), std::move(i->container));
		// NOTE: Subterms are not erased at once in the loop to prevent the
		//	overhead of internal housekeeping (for the size) in the %container.
	// NOTE: All flattened subterms and remained %Value objects are cleanup.
	container.clear();
#endif
	// XXX: Assertion failure may be caused by undefined behavior or wrong
	//	implmenetation.
	YAssert(IsLeaf(*this), "Failed clearing the container of a term.");
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

