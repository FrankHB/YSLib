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
\version r642
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:04:03 +0800
\par 修改时间:
	2016-03-15 12:30 +0800
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
	// TODO: Blocked. Use %string as argument using C++14 heterogeneous
	//	%lower_bound template.
	return *ystdex::search_map(container, AsNode(n), [&](Container::iterator i){
		return EmplaceValueWithHintTo(container, i, n);
	});
}

bool
ValueNode::Remove(const ValueNode& node)
{
	return container.erase(AsNode(node.name)) != 0;
}

void
ValueNode::SwapContent(ValueNode& node) ynothrow
{
	SwapContainer(node),
	Value.swap(node.Value);
}

void
ValueNode::swap(ValueNode& node) ynothrow
{
	std::swap(name, node.name),
	SwapContent(node);
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
ValueNode&
AccessNode(ValueNode& node, size_t n)
{
	const auto p(AccessNodePtr(node, n));

	if(p)
		return *p;
	throw std::out_of_range("Index is out of range.");
}
const ValueNode&
AccessNode(const ValueNode& node, size_t n)
{
	const auto p(AccessNodePtr(node, n));

	if(p)
		return *p;
	throw std::out_of_range("Index is out of range.");
}

observer_ptr<ValueNode>
AccessNodePtr(ValueNode::Container& con, const string& name) ynothrow
{
	// TODO: Blocked. Use %string as argument using C++14 heterogeneous %find
	//	template.
	return make_observer(ystdex::call_value_or<ValueNode*>(ystdex::addrof<>(),
		con.find(AsNode(name)), {}, end(con)));
}
observer_ptr<const ValueNode>
AccessNodePtr(const ValueNode::Container& con, const string& name) ynothrow
{
	// TODO: Blocked. Use %string as argument using C++14 heterogeneous %find
	//	template.
	return make_observer(ystdex::call_value_or<const ValueNode*>(
		ystdex::addrof<>(), con.find(AsNode(name)), {}, end(con)));
}
observer_ptr<ValueNode>
AccessNodePtr(ValueNode& node, size_t n)
{
	auto& con(node.GetContainerRef());

	// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
	return n < con.size() ? make_observer(&*std::next(con.begin(), ptrdiff_t(n)))
		: nullptr;
}
observer_ptr<const ValueNode>
AccessNodePtr(const ValueNode& node, size_t n)
{
	auto& con(node.GetContainer());

	// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
	return n < con.size() ? make_observer(&*std::next(con.cbegin(), ptrdiff_t(n)))
		: nullptr;
}


void
RemoveEmptyChildren(ValueNode::Container& con) ynothrow
{
	ystdex::erase_all_if(con, [](const ValueNode& term) ynothrow{
		return !term;
	});
}

void
RemoveHead(ValueNode::Container& con) ynothrowv
{
	YAssert(!con.empty(), "Empty node container found.");
	con.erase(con.cbegin());
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

