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
\version r675;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2012-08-27 17:26 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YObject;
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

ValueNode::ValueNode(const ValueNode& node)
	: ValueObject(static_cast<const ValueObject&>(node)), name(node),
	pNodes(pNodes ? CloneNodeContainer(*pNodes) : nullptr)
{}

void
ValueNode::CheckNodes()
{
	if(!pNodes)
		pNodes.reset(new Container);
}

YSL_END

