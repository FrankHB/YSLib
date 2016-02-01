/*
	© 2012-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ValueNode.cpp
\ingroup Core
\brief 值类型节点。
\version r533
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:04:03 +0800;
\par 修改时间:
	2016-01-28 12:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::ValueNode
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_ValueNode
#include <cstdio> // for std::snprintf;

namespace YSLib
{

const ValueNode&
ValueNode::operator%=(const ValueNode& node)
{
	auto& n((*this)[node.name]);

	n.Value = node.Value;
	return n;
}
const ValueNode&
ValueNode::operator%=(const ValueNode&& node)
{
	auto& n((*this)[node.name]);

	n.Value = std::move(node.Value);
	return n;
}

ValueNode&
ValueNode::operator[](const string& n)
{
	auto i(container.lower_bound({0, n}));

	if(i == container.end() || container.key_comp()({0, n}, *i))
		i = container.emplace_hint(i, 0, n);
	return *i;
}

bool
ValueNode::Remove(const ValueNode& node)
{
	return container.erase(ValueNode(0, node.name)) != 0;
}

void
ValueNode::SwapContent(ValueNode& node) ynothrow
{
	SwapContainer(node),
	Value.swap(node.Value);
}

ValueNode&
ValueNode::at(const string& n)
{
	return AccessNode(GetContainerRef(), n);
}
const ValueNode&
ValueNode::at(const string& n) const
{
	return AccessNode(GetContainer(), n);
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
	auto& con(node.GetContainer());

	if(n < con.size())
		// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
		return *std::next(con.cbegin(), ptrdiff_t(n));
	throw std::out_of_range("Index is out of range.");
}


ValueNode&
AccessNode(ValueNode::Container* p_con, const string& name)
{
	if(const auto p = AccessNodePtr(p_con, name))
		return *p;
	throw std::out_of_range("Wrong name found.");
}
const ValueNode&
AccessNode(const ValueNode::Container* p_con, const string& name)
{
	if(const auto p = AccessNodePtr(p_con, name))
		return *p;
	throw std::out_of_range("Wrong name found.");
}

ValueNode*
AccessNodePtr(ValueNode::Container& con, const string& name)
{
	const auto i(con.find(ValueNode(0, name)));

	return i != end(con) ? &*i : nullptr;
}
const ValueNode*
AccessNodePtr(const ValueNode::Container& con, const string& name)
{
	const auto i(con.find(ValueNode(0, name)));

	return i != end(con) ? &*i : nullptr;
}


bool
IsPrefixedIndex(string_view name, char prefix)
{
	YAssertNonnull(name.data());
	if(name.length() > 1 && name.front() == prefix)
		try
		{
			const auto ss(name.substr(1));

			return MakeIndex(std::stoul(string(ss))) == ss;
		}
		CatchIgnore(std::invalid_argument&)
	return {};
}

string
MakeIndex(size_t n)
{
	char str[5]{};

	if(n < 10000)
		std::snprintf(str, 5, "%04u", unsigned(n));
	else
		throw std::invalid_argument("Argument is too large.");
	return str;
}

} // namespace YSLib;

