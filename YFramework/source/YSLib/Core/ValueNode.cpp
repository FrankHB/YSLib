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
\version r213
\author FrankHB<frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:04:03 +0800;
\par 修改时间:
	2012-09-17 21:35 +0800;
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::ValueNode
*/


#include "YSLib/Core/ValueNode.h"

YSL_BEGIN

ValueNode&
ValueNode::operator[](const string& name)
{
	auto& cont(CheckNodes());
	auto i(cont.lower_bound(name));

	if(i == cont.end() || cont.key_comp()(name, *i))
		// TODO: Use %emplace_hint.
		i = cont.insert(i, name);
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
	const auto i(cont.find(name));

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

bool
ValueNode::Add(const ValueNode& n)
{
	return CheckNodes().insert(n).second;
}
bool
ValueNode::Add(ValueNode&& n)
{
	// TODO: Use %emplace.
	return CheckNodes().insert(std::move(n)).second;
}

ValueNode::Container&
ValueNode::CheckNodes()
{
	if(!value)
		value = Container();
	return GetContainer();
}

bool
ValueNode::Remove(const ValueNode& node)
{
	try
	{
		return GetContainer().erase(node.name) != 0;
	}
	catch(ystdex::bad_any_cast&)
	{}
	return false;
}

YSL_END

