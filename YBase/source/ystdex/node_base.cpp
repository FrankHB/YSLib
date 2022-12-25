/*
	© 2019, 2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file node_base.cpp
\ingroup YStandardEx
\brief 作为节点序列容器实现的节点基础。
\version r176
\author FrankHB <frankhb1989@gmail.com>
\since build 865
\par 创建时间:
	2019-08-22 17:48:43 +0800
\par 修改时间:
	2022-11-28 19:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::NodeBase
*/


#include "ystdex/node_base.h"
#include "ystdex/expanded_function.hpp" // for yverify, ystdex::retry_on_cond;

namespace ystdex
{

namespace details
{

void
list_node_base::swap(list_node_base& x, list_node_base& y) ynothrow
{
	if(x.next != &x)
	{
		if(y.next != &y)
		{
			std::swap(x.next, y.next),
			std::swap(x.prev, y.prev);
			yunseq(
			x.next->prev = x.prev->next = &x,
			y.next->prev = y.prev->next = &y
			);
		}
		else
		{
			yunseq(
			y.next = x.next,
			y.prev = x.prev
			);
			yunseq(
			y.next->prev = y.prev->next = &y,
			x.next = x.prev = &x
			);
		}
	}
	else if(y.next != &y)
	{
		yunseq(
		x.next = y.next,
		x.prev = y.prev
		);
		yunseq(
		x.next->prev = x.prev->next = &x,
		y.next = y.prev = &y
		);
	}
}

void
list_node_base::transfer(list_node_base::base_ptr first,
	list_node_base::base_ptr last) ynothrow
{
	// NOTE: The destination node of the splice operations is specified by
	//	%this.
	yverify(last != this);
	// NOTE: Determine the next node of the back node of the range, the node
	//	before the range and the node before the destination node.
	last->prev->next = this;
	first->prev->next = last;
	prev->next = first;

	const auto tmp(prev);

	// NOTE: Determine the previous node of the node before the destination
	//	node, the next node of the back node of the range in the source and the
	//	front node of the range.
	prev = last->prev;
	last->prev = first->prev;
	first->prev = tmp;
}


void
list_node_base::reverse_nodes() ynothrow
{
	auto x(this);

	ystdex::retry_on_cond([&]() ynothrow{
		return x != this;
	}, [&]() ynothrow{
		std::swap(x->next, x->prev);
		x = x->prev;
	});
}

void
list_node_base::hook(base_ptr position) ynothrow
{
	yunseq(next = position, prev = position->prev);
	position->prev->next = this;
	position->prev = this;
}

void
list_node_base::unhook() ynothrow
{
	const auto saved_next(next);
	const auto saved_prev(prev);

	saved_prev->next = saved_next;
	saved_next->prev = saved_prev;
}


list_header::list_header(list_header&& x) ynothrow
	: list_node_base{x.next, x.prev}, node_count(x.node_count)
{
	if(x.next != &x.base())
	{
		next->prev = prev->next = &base();
		x.reset();
	}
	else
		reset_links();
}

void
list_header::move_nodes(list_header&& x)
{
	auto& xnode(x.base());

	if(xnode.next == &xnode)
		reset();
	else
	{
		const auto p_node(&base());

		yunseq(p_node->next = xnode.next, p_node->prev = xnode.prev);
		p_node->next->prev = p_node->prev->next = p_node;
		node_count = x.node_count;
		x.reset();
	}
}

} // namespace details;

} // namespace ystdex;

