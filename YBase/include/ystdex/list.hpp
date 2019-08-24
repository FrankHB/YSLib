/*
	© 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file list.hpp
\ingroup YStandardEx
\brief 列表容器。
\version r320
\author FrankHB <frankhb1989@gmail.com>
\since build 864
\par 创建时间:
	2019-08-14 14:48:52 +0800
\par 修改时间:
	2019-08-24 20:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::List
*/


#ifndef YB_INC_ystdex_list_hpp_
#define YB_INC_ystdex_list_hpp_ 1

#include "node_base.h" // for "node_base.h";
#include "placement.hpp" // for aligned_storage_t, standard_layout_storage;
#include "operators.hpp" // for bidirectional_iteratable, equality_comparable;

namespace ystdex
{

//! \since build 864
namespace details
{

template<typename _type>
class list_node : public list_node_base
{
public:
	using link_type = tree_node<_type>*;

private:
	standard_layout_storage<aligned_storage_t<sizeof(_type),
		yalignof(_type)>> storage;

public:
	YB_ATTR_nodiscard YB_PURE yconstfn_relaxed _type*
	access_ptr()
	{
		return static_cast<_type*>(storage.access());
	}
	YB_ATTR_nodiscard YB_PURE yconstfn const _type*
	access_ptr() const
	{
		return static_cast<const _type*>(storage.access());
	}
};


//! \since build 865
template<typename>
class list_const_iterator;


template<typename _type>
class list_iterator
	: public bidirectional_iteratable<list_iterator<_type>, _type&>
{
	//! \since build 865
	template<typename>
	friend class list_const_iterator;

public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = _type;
	using pointer = _type*;
	using reference = _type&;
	using difference_type = ptrdiff_t;

private:
	using base_ptr = list_node_base::base_ptr;

	base_ptr p_node{};

public:
	list_iterator() ynothrow = default;
	explicit
	list_iterator(base_ptr x) ynothrow
		: p_node(x)
	{}

	YB_ATTR_nodiscard YB_PURE list_iterator
	cast_mutable() const ynothrow
	{
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE reference
	operator*() const ynothrow
	{
		return *static_cast<list_node<_type>*>(p_node)->access_ptr();
	}

	list_iterator&
	operator++() ynothrow
	{
		p_node = p_node->next;
		return *this;
	}

	list_iterator&
	operator--() ynothrow
	{
		p_node = p_node->prev;
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE bool
	operator==(const list_iterator& x) const ynothrow
	{
		return p_node == x.p_node;
	}
};


template<typename _type>
class list_const_iterator
	: public bidirectional_iteratable<list_const_iterator<_type>, const _type&>,
	public equality_comparable<list_const_iterator<_type>, list_iterator<_type>>
{
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = _type;
	//! \since build 865
	using pointer = const _type*;
	//! \since build 865
	using reference = const _type&;
	using difference_type = ptrdiff_t;

private:
	using base_ptr = list_node_base::const_base_ptr;
	using iterator = list_iterator<_type>;

	base_ptr p_node = {};

public:
	list_const_iterator() ynothrow = default;
	explicit
	list_const_iterator(base_ptr x) ynothrow
		: p_node(x)
	{}
	list_const_iterator(const iterator& x) ynothrow
		: p_node(x.p_node)
	{}

	YB_ATTR_nodiscard YB_PURE iterator
	cast_mutable() const ynothrow
	{
		return iterator(const_cast<typename iterator::base_ptr>(p_node));
	}

	YB_ATTR_nodiscard YB_PURE reference
	operator*() const ynothrow
	{
		return *static_cast<const list_node<_type>*>(p_node)->access_ptr();
	}

	list_const_iterator&
	operator++() ynothrow
	{
		p_node = p_node->next;
		return *this;
	}

	list_const_iterator&
	operator--() ynothrow
	{
		p_node = p_node->prev;
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE bool
	operator==(const list_const_iterator& x) const ynothrow
	{
		return p_node == x.p_node;
	}
	YB_ATTR_nodiscard YB_PURE bool
	operator==(const iterator& x) const ynothrow
	{
		return p_node == x.p_node;
	}
};

} // namespace details;

} // namespace ystdex;

#endif

