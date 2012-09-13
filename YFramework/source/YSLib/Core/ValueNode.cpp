/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ValueNode.cpp
\ingroup Core
\brief 值类型节点。
\version r132
\author FrankHB<frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:04:03 +0800;
\par 修改时间:
	2012-09-13 12:35 +0800;
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::ValueNode
*/


#include "YSLib/Core/ValueNode.h"

YSL_BEGIN

namespace
{

//! \since build 330
ValueNode::Container*
CloneNodeContainer(const ValueNode::Container& cont)
{
	const auto p(new ValueNode::Container());

	for(const auto& pr : cont)
		p->insert(pr);
	return p;
}

} // unnamed namespace;

ValueNode::ValueNode(const ValueNode& node)
	: name(node), value(node.GetValue()),
	p_nodes(p_nodes ? CloneNodeContainer(*p_nodes) : nullptr)
{}

ValueNode&
ValueNode::operator[](const string& name)
{
	CheckNodes();

	auto i(p_nodes->lower_bound(name));

	if(i == p_nodes->end() || p_nodes->key_comp()(name, *i))
		// TODO: Use %emplace_hint.
		i = p_nodes->insert(i, name);
	return const_cast<ValueNode&>(*i);
}

const ValueNode&
ValueNode::GetNode(const string& name) const
{
	if(p_nodes)
	{
		const auto i(p_nodes->find(name));

		if(i != p_nodes->end())
			return *i;
	}
	throw std::out_of_range(name);
}

bool
ValueNode::Add(const ValueNode& n)
{
	CheckNodes();
	return p_nodes->insert(n).second;
}
bool
ValueNode::Add(ValueNode&& n)
{
	CheckNodes();
	// TODO: Use %emplace.
	return p_nodes->insert(std::move(n)).second;
}

void
ValueNode::CheckNodes()
{
	if(!p_nodes)
		p_nodes.reset(new Container());
}

YSL_END

