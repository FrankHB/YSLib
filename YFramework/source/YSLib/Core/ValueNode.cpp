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
\version r324
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:04:03 +0800;
\par 修改时间:
	2013-08-05 21:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::ValueNode
*/


#include "YSLib/Core/ValueNode.h"

namespace YSLib
{

const ValueNode&
ValueNode::operator[](const string& name) const
{
	auto& con(CheckNodes());
	auto i(con.lower_bound({0, name}));

	if(i == con.end() || con.key_comp()({0, name}, *i))
		// TODO: Use %emplace_hint.
		i = con.insert(i, {0, name});
	return *i;
}

size_t
ValueNode::GetSize() const ynothrow
{
	const auto p_con(GetContainerPtr());

	return p_con ? p_con->size() : 0;
}

ValueNode::Container&
ValueNode::CheckNodes() const
{
	if(!Value)
		Value = Container();
	return GetContainer();
}

bool
ValueNode::Add(const ValueNode& node) const
{
	return CheckNodes().insert(node).second;
}
bool
ValueNode::Add(ValueNode&& node) const
{
	// TODO: Use %emplace.
	return CheckNodes().insert(std::move(node)).second;
}

bool
ValueNode::Remove(const ValueNode& node) const
{
	const auto p_con(GetContainerPtr());

	return p_con ? p_con->erase({0, node.name}) != 0 : false;
}

const ValueNode&
ValueNode::at(const string& name) const
{
	return AccessNode(GetContainer(), name);
}


const ValueNode&
AccessNode(const ValueNode::Container* p_con, const string& name)
{
	if(const auto p = AccessNodePtr(p_con, name))
		return *p;
	throw std::out_of_range("Wrong name found.");
}

const ValueNode*
AccessNodePtr(const ValueNode::Container& con, const string& name)
{
	const auto i(con.find(ValueNode(0, name)));

	return i != end(con) ? &*i : nullptr;
}

} // namespace YSLib;

