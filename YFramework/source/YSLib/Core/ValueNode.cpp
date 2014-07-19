/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ValueNode.cpp
\ingroup Core
\brief 值类型节点。
\version r390
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:04:03 +0800;
\par 修改时间:
	2014-07-14 14:53 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::ValueNode
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_ValueNode

namespace YSLib
{

ValueNode::ValueNode(const ValueNode& node)
	: name(node.name), p_container(node.p_container ?
	CloneNonpolymorphic(node.p_container) : nullptr), Value(node.Value)
{}

const ValueNode&
ValueNode::operator%=(const ValueNode& node) const
{
	const auto& n((*this)[node.name]);

	n.Value = node.Value;
	return n;
}
const ValueNode&
ValueNode::operator%=(const ValueNode&& node) const
{
	const auto& n((*this)[node.name]);

	n.Value = std::move(node.Value);
	return n;
}

const ValueNode&
ValueNode::operator[](const string& name) const
{
	auto& con(CheckNodes());
	auto i(con.lower_bound({0, name}));

	if(i == con.end() || con.key_comp()({0, name}, *i))
		i = con.emplace_hint(i, 0, name);
	return *i;
}

ValueNode::Container&
ValueNode::GetContainerRef() const
{
	if(p_container)
		return *p_container;
	throw ystdex::bad_any_cast();
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
	if(!p_container)
		p_container.reset(new Container());
	return GetContainerRef();
}

void
ValueNode::SetChildren(Container con) const
{
	SetChildren(make_unique<Container>(std::move(con)));
}
void
ValueNode::SetChildren(unique_ptr<Container> p_con) const
{
	p_container = std::move(p_con);
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
	return AccessNode(GetContainerRef(), name);
}

void
ValueNode::swap(ValueNode& node) ynothrow
{
	std::swap(name, node.name),
	std::swap(p_container, node.p_container),
	Value.swap(node.Value);
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


bool
IsPrefixedIndex(const string& name, char prefix)
{
	if(name.length() > 1 && name[0] == prefix)
		try
		{
			const string ss(&name[1]);

			return std::to_string(std::stoul(ss)) == ss;
		}
		catch(std::invalid_argument&)
		{}
	return {};
}

} // namespace YSLib;

