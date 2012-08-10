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
\version r1602;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2012-08-09 03:57 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YObject;
*/


#include "YSLib/Core/yobject.h"

YSL_BEGIN

ValueObject::ValueObject(const ValueObject& c)
	: manager(c.manager), obj_ptr(nullptr)
{
	if(manager && c.obj_ptr)
		manager(obj_ptr, c.obj_ptr, Clone);
}
ValueObject::ValueObject(ValueObject&& c) ynothrow
	: manager(c.manager), obj_ptr(c.obj_ptr)
{
	c.obj_ptr = nullptr;
}

bool
ValueObject::operator==(const ValueObject& rhs) const
{
	return (!manager && !rhs.manager) || (manager && rhs.manager
		&& manager == rhs.manager && obj_ptr && rhs.obj_ptr
		&& manager(obj_ptr, rhs.obj_ptr, Equality));
}

ValueObject&
ValueObject::operator=(ValueObject&& c) ynothrow
{
	if(&c != this)
	{
		Clear();
		manager = c.manager;
		std::swap(obj_ptr, c.obj_ptr);
	}
	return *this;
}

void
ValueObject::Clear() ynothrow
{
	if(manager)
		manager(obj_ptr, obj_ptr, Destroy);
}

void
ValueObject::Swap(ValueObject& c) ynothrow
{
	std::swap(manager, c.manager);
	std::swap(obj_ptr, c.obj_ptr);
}


namespace
{

//! \since build 330 。
ValueNode::Container*
CloneNodeContainer(const ValueNode::Container& cont)
{
	const auto p(new ValueNode::Container());

	for(const auto& pr : cont)
		p->insert(pr);
	return p;
}

} // unnamed namespace;

ValueNode::ValueNode(const ValueNode& n)
	: ValueObject(static_cast<const ValueObject&>(n)), name(n),
	pNodes(pNodes ? CloneNodeContainer(*pNodes) : nullptr)
{}

YSL_END

