/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ValueNode.cpp
\ingroup Core
\brief 值类型节点。
\version r478
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:04:03 +0800;
\par 修改时间:
	2015-05-16 08:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::ValueNode
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_ValueNode

namespace YSLib
{

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
ValueNode::operator[](const string& n) const
{
	auto i(container.lower_bound({0, n}));

	if(i == container.end() || container.key_comp()({0, n}, *i))
		i = container.emplace_hint(i, 0, n);
	return *i;
}

bool
ValueNode::Remove(const ValueNode& node) const
{
	return container.erase({0, node.name}) != 0;
}

void
ValueNode::SwapContent(const ValueNode& node) const ynothrow
{
	SwapContainer(node),
	Value.swap(node.Value);
}

const ValueNode&
ValueNode::at(const string& n) const
{
	return AccessNode(GetContainerRef(), n);
}

void
ValueNode::swap(ValueNode& node) ynothrow
{
	std::swap(name, node.name),
	SwapContent(node);
}

const ValueNode&
at(const ValueNode& node, size_t n)
{
	auto& con(node.GetContainerRef());

	if(n < con.size())
		// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
		return *std::next(con.cbegin(), ptrdiff_t(n));
	throw std::out_of_range("Index is out of range.");
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

			return to_string(std::stoul(ss)) == ss;
		}
		CatchIgnore(std::invalid_argument&)
	return {};
}

} // namespace YSLib;

