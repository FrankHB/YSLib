/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ValueNode.cpp
\ingroup Core
\brief 值类型节点。
\version r246
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:04:03 +0800;
\par 修改时间:
	2013-04-08 21:14 +0800;
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::ValueNode
*/


#include "YSLib/Core/ValueNode.h"

YSL_BEGIN

bool
ValueNode::operator+=(const ValueNode& node)
{
	return CheckNodes().insert(node).second;
}
bool
ValueNode::operator+=(ValueNode&& node)
{
	// TODO: Use %emplace.
	return CheckNodes().insert(std::move(node)).second;
}

bool
ValueNode::operator-=(const ValueNode& node)
{
	try
	{
		return GetContainer().erase({0, node.name}) != 0;
	}
	catch(ystdex::bad_any_cast&)
	{}
	return false;
}

ValueNode&
ValueNode::operator[](const string& name)
{
	auto& cont(CheckNodes());
	auto i(cont.lower_bound({0, name}));

	if(i == cont.end() || cont.key_comp()({0, name}, *i))
		// TODO: Use %emplace_hint.
		i = cont.insert(i, {0, name});
	return const_cast<ValueNode&>(*i);
}

ValueNode::Container&
ValueNode::GetContainer() const
{
	return value.Access<Container>();
}
const ValueNode&
ValueNode::GetNode(const string& name) const
{
	auto& cont(GetContainer());
	const auto i(cont.find({0, name}));

	if(i != cont.end())
		return *i;
	throw std::out_of_range(name);
}
size_t
ValueNode::GetSize() const ynothrow
{
	try
	{
		return GetContainer().size();
	}
	catch(...)
	{}
	return 0;
}

ValueNode::Container&
ValueNode::CheckNodes()
{
	if(!value)
		value = Container();
	return GetContainer();
}

YSL_END

