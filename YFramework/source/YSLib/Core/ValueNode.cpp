/*
	© 2012-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ValueNode.cpp
\ingroup Core
\brief 值类型节点。
\version r794
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:04:03 +0800
\par 修改时间:
	2019-01-01 10:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::ValueNode
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_ValueNode // for ystdex::call_value_or, ystdex::addrof,
//	ystdex::compose, std::mem_fn, ystdex::bind1;
#include <cstdio> // for std::snprintf;

namespace YSLib
{

ValueNode&
ValueNode::operator%=(const ValueNode& node)
{
	return Deref(insert_or_assign(node.name, node).first);
}
ValueNode&
ValueNode::operator%=(ValueNode&& node)
{
	return Deref(insert_or_assign(node.name, std::move(node)).first);
}

ValueNode::Container
ValueNode::CreateRecursively(const Container& con, IValueHolder::Creation c)
{
	return CreateRecursively(con, ystdex::bind1(&ValueObject::Create, c));
}

void
ValueNode::MoveContent(ValueNode&& node)
{
	// NOTE: This is required for the case when the moved-to object (referenced
	//	by '*this') is an ancestor of the moved-from node (referenced by the
	//	parameter). In such cases, an object moved from the moved-to object
	//	container is needed to preserve the content of the moved-from node
	//	living sufficient long, since the move %operator= of %Container does not
	//	guarantee the old content of the container (as it can clear the old
	//	content in the container before moving elements). This also avoids
	//	cyclic references even when the move %operator= of %Container does
	//	preserved the old content of the container longer (e.g. by copy and
	//	swap), which would leak resources instead of d.
	const auto t(std::move(GetContainerRef()));

	SetContent(std::move(node));
}

void
ValueNode::SwapContent(ValueNode& node) ynothrowv
{
	SwapContainer(node),
	swap(Value, node.Value);
}

void
ValueNode::ThrowIndexOutOfRange(size_t idx)
{
	throw std::out_of_range(ystdex::sfmt("Index '%zu' is out of range.", idx));
}

void
ValueNode::ThrowWrongNameFound(string_view name)
{
	throw std::out_of_range(ystdex::sfmt("Wrong name '%s' found.",
		Nonnull(name.data())));
}

void
swap(ValueNode& x, ValueNode& y) ynothrowv
{
	std::swap(x.name, y.name),
	x.SwapContent(y);
}

ValueNode&
AccessNode(ValueNode::Container* p_con, const string& name)
{
	if(const auto p = AccessNodePtr(p_con, name))
		return *p;
	ValueNode::ThrowWrongNameFound(name);
}
const ValueNode&
AccessNode(const ValueNode::Container* p_con, const string& name)
{
	if(const auto p = AccessNodePtr(p_con, name))
		return *p;
	ValueNode::ThrowWrongNameFound(name);
}
ValueNode&
AccessNode(ValueNode& node, size_t n)
{
	const auto p(AccessNodePtr(node, n));

	if(p)
		return *p;
	ValueNode::ThrowIndexOutOfRange(n);
}
const ValueNode&
AccessNode(const ValueNode& node, size_t n)
{
	const auto p(AccessNodePtr(node, n));

	if(p)
		return *p;
	ValueNode::ThrowIndexOutOfRange(n);
}

observer_ptr<ValueNode>
AccessNodePtr(ValueNode::Container& con, const string& name) ynothrow
{
	return make_observer(ystdex::call_value_or<ValueNode*>(ystdex::addrof<>(),
		con.find(name), {}, ystdex::end(con)));
}
observer_ptr<const ValueNode>
AccessNodePtr(const ValueNode::Container& con, const string& name) ynothrow
{
	return make_observer(ystdex::call_value_or<const ValueNode*>(
		ystdex::addrof<>(), con.find(name), {}, ystdex::end(con)));
}
observer_ptr<ValueNode>
AccessNodePtr(ValueNode& node, size_t n)
{
	auto& con(node.GetContainerRef());

	// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
	return n < con.size()
		? make_observer(&*std::next(con.begin(), ptrdiff_t(n))) : nullptr;
}
observer_ptr<const ValueNode>
AccessNodePtr(const ValueNode& node, size_t n)
{
	auto& con(node.GetContainer());

	// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
	return n < con.size()
		? make_observer(&*std::next(con.cbegin(), ptrdiff_t(n))) : nullptr;
}

ValueObject
GetValueOf(observer_ptr<const ValueNode> p_node)
{
	return ystdex::call_value_or(std::mem_fn(&ValueNode::Value), p_node);
}

observer_ptr<const ValueObject>
GetValuePtrOf(observer_ptr<const ValueNode> p_node)
{
	return ystdex::call_value_or(ystdex::compose(make_observer<const ValueObject
		>, ystdex::addrof<>(), std::mem_fn(&ValueNode::Value)), p_node);
}


void
RemoveEmptyChildren(ValueNode::Container& con) ynothrow
{
	ystdex::erase_all_if(con, std::mem_fn(&ValueNode::operator!));
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

size_t
GetLastIndexOf(const ValueNode::Container& con)
{
	return con.empty() ? size_t(-1)
		: size_t(std::stoul(con.rbegin()->GetName()));
}

} // namespace YSLib;

