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
\version r850
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 18:02:47 +0800
\par 修改时间:
	2016-01-22 15:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1 // for ystdex::unimplemented;
#include YFM_NPL_SContext
#include <ystdex/functional.hpp> // for ystdex::bind1;

using namespace YSLib;

namespace NPL
{

namespace A1
{

void
InsertChild(ValueNode&& node, ValueNode::Container& con)
{
	con.insert(node.GetName().empty() ? ValueNode(0, '$' + MakeIndex(con),
		std::move(node.Value)) : std::move(node));
}

void
InsertSequenceChild(ValueNode&& node, NodeSequence& con)
{
	con.emplace_back(std::move(node));
}

ValueNode
TransformNode(const ValueNode& node, NodeMapper mapper,
	NodeMapper map_leaf_node, NodeToString node_to_str,
	NodeInserter insert_child)
{
	auto s(node.size());

	if(s == 0)
		return map_leaf_node(node);

	auto i(node.begin());
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
			return {0, name, std::move(nd.Value)};
		return {ValueNode::Container{std::move(nd)}, name};
	}

	ValueNode::Container node_con;

	std::for_each(i, node.end(), [&](const ValueNode& nd){
		insert_child(mapper ? mapper(nd) : nested_call(nd), node_con);
	});
	return {std::move(node_con), name};
}

ValueNode
TransformNodeSequence(const ValueNode& node, NodeMapper mapper, NodeMapper
	map_leaf_node, NodeToString node_to_str, NodeSequenceInserter insert_child)
{
	auto s(node.size());

	if(s == 0)
		return map_leaf_node(node);

	auto i(node.begin());
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

		if(n.GetName().empty())
			return {0, name, std::move(n.Value)};
		return {0, name, NodeSequence{std::move(n)}};
	}

	NodeSequence node_con;

	std::for_each(i, node.end(), [&](const ValueNode& nd){
		insert_child(mapper ? mapper(nd) : nested_call(nd), node_con);
	});
	return {0, name, std::move(node_con)};
}

} // namesapce A1;

} // namespace NPL;

