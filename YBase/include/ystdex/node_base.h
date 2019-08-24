/*
	© 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file node_base.h
\ingroup YStandardEx
\brief 作为节点序列容器实现的节点基础。
\version r437
\author FrankHB <frankhb1989@gmail.com>
\since build 865
\par 创建时间:
	2019-08-22 17:48:43 +0800
\par 修改时间:
	2019-08-24 14:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::NodeBase

基于节点的序列容器的内部实现的节点接口。
设计和 libstdc++ 的 <bits/stl_list.h> 中非公开接口类似，以双向链表作为内部数据结构，
	兼容 std::list 的容器实现。
*/


#ifndef YB_INC_ystdex_node_base_h_
#define YB_INC_ystdex_node_base_h_ 1

#include "../ydef.h"

namespace ystdex
{

//! \since build 865
namespace details
{

//! \since build 864
//@{
struct YB_API list_node_base
{
	using base_ptr = list_node_base*;
	using const_base_ptr = const list_node_base*;

	// XXX: Uninitialized.
	base_ptr next;
	base_ptr prev;

	//! \since build 865
	//@{
	void
	reset_links() ynothrow
	{
		next = prev = this;
	}

	static void
	swap(list_node_base&, list_node_base&) ynothrow;

	//! \pre 断言：第二参数不等于 this 。
	void
	transfer(base_ptr, base_ptr) ynothrow;

	void
	reverse_nodes() ynothrow;

	void
	hook(base_ptr) ynothrow;

	void
	unhook() ynothrow;
	//@}
};


//! \since build 865
struct YB_API list_header : list_node_base
{
	//! \since build 864
	size_t node_count;

	list_header() ynothrow
		: node_count(0)
	{
		reset_links();
	}
	list_header(list_header&&) ynothrow;

	void
	move_nodes(list_header&&);

	//! \since build 865
	void
	reset() ynothrow
	{
		reset_links();
		node_count = 0;
	}

	//! \since build 864
	YB_ATTR_nodiscard YB_PURE list_node_base&
	base()
	{
		return *this;
	}
};
//@}

} // namespace details;

} // namespace ystdex;

#endif

