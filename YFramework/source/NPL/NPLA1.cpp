/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.cpp
\ingroup NPL
\brief 配置设置。
\version r123
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 18:02:47 +0800
\par 修改时间:
	2014-06-02 15:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1
#include YFM_NPL_SContext

using namespace YSLib;

namespace NPL
{

ValueNode
MapNPLA1Node(const ValueNode& node)
{
	return node.GetName().empty() ? ValueNode(0, "", std::move(node.Value))
		: std::move(node);
}

ValueNode
TransformNPLA1(const ValueNode& node, std::function<NodeMapper> mapper)
{
	auto s(node.GetSize());

	if(s == 0)
		return {0, "", node ? Deliteralize(Access<string>(node)) : string()};

	auto i(node.begin());

	if(s == 1)
		return TransformNPLA1(*i, mapper);

	const auto& new_name([&]()->string{
		try
		{
			const auto& str(Access<string>(*i));

			yunseq(++i, --s);
			return str;
		}
		catch(ystdex::bad_any_cast&)
		{}
		return string();
	}());

	if(s == 1)
	{
		auto&& n(TransformNPLA1(*i, mapper));

		if(n.GetName().empty())
			return {0, new_name, std::move(n.Value)};
		return {ValueNode::Container{std::move(n)}, new_name};
	}

	auto p_node_con(make_unique<ValueNode::Container>());

	std::for_each(i, node.end(), [&](const ValueNode& nd){
		auto&& n(mapper(TransformNPLA1(nd, mapper)));

		p_node_con->insert(n.GetName().empty() ? ValueNode(0,
			'$' + std::to_string(p_node_con->size()), std::move(n.Value))
			: std::move(n));
	});
	return {std::move(p_node_con), new_name};
}


ValueNode
LoadNPLA1(ValueNode&& tree, std::function<NodeMapper> mapper)
{
	ValueNode root;

	try
	{
		root = TransformNPLA1(tree, mapper);
	}
	catch(ystdex::bad_any_cast& e)
	{
		// TODO: Avoid memory allocation.
		throw LoggedEvent(ystdex::sfmt(
			"Bad configuration found: cast failed from [%s] to [%s] .",
			e.from(), e.to()), Warning);
	}
	return root;
}

} // namespace NPL;

