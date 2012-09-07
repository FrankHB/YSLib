/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yobject.cpp
\ingroup Core
\brief 平台无关的基础对象。
\version r733
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2012-09-06 19:29 +0800
\par 字符集:
	UTF-8
\par 模块名称:
	YSLib::Core::YObject
*/


#include "YSLib/Core/yobject.h"

YSL_BEGIN

bool
ValueObject::operator==(const ValueObject& vo) const
{
	const auto x(content.get_holder());
	const auto y(vo.content.get_holder());

	return x && y && x->type() == y->type() ? static_cast<const IValueHolder&>(
		*x) == static_cast<const IValueHolder&>(*y) : false;
}


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

