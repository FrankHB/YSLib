/*
	© 2018-2019, 2022-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file node_handle.hpp
\ingroup YStandardEx
\brief 作为关联容器和无序容器实现的节点句柄。
\version r5726
\author FrankHB <frankhb1989@gmail.com>
\since build 966
\par 创建时间:
	2023-01-27 13:30:58 +0800
\par 修改时间:
	2023-01-30 19:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::NodeHandle

关联容器和无序容器的内部实现的节点句柄接口。
设计和 libstdc++ 的 <bits/node_handle.h> 中非公开接口类似。
提供了 ISO C++17 节点句柄(node handle) 兼容接口的基本实现。
实现语言的特性要求和 YBase.YStandardEx 要求一致。
为便于参照和测试，实现也和 libstdc++ 在 ISO C++17 模式下的相似，
	但充分利用了其它 YBase.YStandardEx 特性。
因为用于内部实现，接口主要在用于实现的特定命名空间内。
除另行指定的兼容性策略外，作为实现的接口不保证对外稳定。
*/


#ifndef YB_INC_ystdex_node_handle_hpp_
#define YB_INC_ystdex_node_handle_hpp_ 1

#include "allocator.hpp" // for internal "allocator.hpp", allocator_traits,
//	is_move_assignable, ystdex::swap_dependent, YAssert, yconstraint, yassume,
//	true_, false_, std::pointer_traits, rebind_alloc_t, or_, YAssertNonnull,
//	std::declval, is_bitwise_swappable;
#include "optional.h" // for optional, nullopt;

namespace ystdex
{

//! \since build 830
//!@{
namespace details
{

inline namespace rb_tree
{

template<typename, typename, typename, typename, class>
class tree;

} // inline namespace rb_tree;

//! \since build 966
inline namespace hash_table
{

template<typename, typename, class, typename, typename, typename, class, class>
class table;

} // inline namespace hash_table;

//! \warning 非虚析构。
//{@
//! \brief 节点句柄基类。
template<typename _type, class _tNodeAlloc>
class node_handle_base
{
	template<typename, typename, typename, typename, class>
	friend class tree;
	template<typename, typename, class, typename, typename, typename, class,
		class>
	friend class table;

public:
	using allocator_type = rebind_alloc_t<_tNodeAlloc, _type>;

private:
	//! \since build 864
	using node_ator_traits = allocator_traits<_tNodeAlloc>;
	//! \since build 966
	using equal_alloc_or_pocs = or_<typename node_ator_traits::is_always_equal,
		typename node_ator_traits::propagate_on_container_swap>;

private:
	optional<_tNodeAlloc> alloc{};

protected:
	typename node_ator_traits::pointer ptr = {};

	yconstfn
	node_handle_base() ynothrow = default;
	node_handle_base(node_handle_base&& nh) ynothrow
		: alloc(std::move(nh.alloc)), ptr(nh.ptr)
	{
		nh.ptr = {};
		nh.alloc = nullopt;
	}
	//! \pre 间接断言：第一参数非空。
	node_handle_base(typename node_ator_traits::pointer p, const _tNodeAlloc& a)
		: alloc(a), ptr(p)
	{
		YAssertNonnull(p);
	}
	~node_handle_base()
	{
		destroy();
	}

	//! \since build 966
	node_handle_base&
	operator=(node_handle_base&& nh) ynothrowv
	{
		destroy();
		ptr = nh.ptr;
		move_assign(nh, is_move_assignable<_tNodeAlloc>());
		yunseq(nh.ptr = {}, nh.alloc = nullopt);
		return *this;
	}

public:
	YB_ATTR_nodiscard YB_PURE explicit
	operator bool() const ynothrow
	{
		return ptr;
	}

protected:
	//! \since build 966
	void
	swap_base(node_handle_base& nh) ynoexcept(equal_alloc_or_pocs())
	{
		ystdex::swap_dependent(ptr, nh.ptr);
		if(node_ator_traits::propagate_on_container_swap() || !alloc
			|| !nh.alloc)
			alloc.swap(nh.alloc);
		else
			YAssert(alloc == nh.alloc, "Unequal allocators found.");
	}

private:
	void
	destroy() ynothrow
	{
		if(ptr)
		{
			allocator_type a(*alloc);

			allocator_traits<allocator_type>::destroy(a, ptr->access_ptr());
			node_ator_traits::deallocate(*alloc, ptr, 1);
		}
	}

public:
	//! \since build 966
	YB_ATTR_nodiscard YB_PURE allocator_type
	get_allocator() const ynothrowv
	{
		yconstraint(!this->empty());
		return allocator_type(*alloc);
	}

	YB_ATTR_nodiscard YB_PURE bool
	empty() const ynothrow
	{
		return !ptr;
	}

private:
	//! \since build 966
	void
	move_assign(node_handle_base& nh, true_)
	{
		if(node_ator_traits::propagate_on_container_move_assignment() || !alloc)
			alloc = std::move(nh.alloc);
		else
			YAssert(alloc == nh.alloc, "Unequal allocators found.");
	}
	//! \since build 966
	void
	move_assign(false_)
	{
		yassume(alloc);
	}
};


//! \brief 映射容器的节点句柄。
template<typename _tKey, typename _type, typename _tNodeAlloc>
class node_handle : public node_handle_base<_type, _tNodeAlloc>
{
	template<typename, typename, typename, typename, class>
	friend class tree;
	template<typename, typename, class, typename, typename, typename, class,
		class>
	friend class table;

public:
	using key_type = _tKey;
	using mapped_type = typename _type::second_type;

private:
	//! \since build 864
	using ator_traits = allocator_traits<_tNodeAlloc>;
	template<typename _tObj>
	using ptr_t = typename std::pointer_traits<typename ator_traits::pointer
		>::template rebind<remove_reference_t<_tObj>>;

	ptr_t<_tKey> p_key = {};
	ptr_t<typename _type::second_type> p_mapped = {};

public:
	yconstfn
	node_handle() ynothrow = default;

private:
	//! \pre 间接断言：第一参数非空。
	node_handle(typename ator_traits::pointer p, const _tNodeAlloc& a)
		: node_handle_base<_type, _tNodeAlloc>(p, a)
	{
		if(p)
		{
			auto& k(const_cast<_tKey&>(p->access_ptr()->first));

			p_key = pointer_to(k);
			p_mapped = pointer_to(p->access_ptr()->second);
		}
		else
		{
			p_key = {};
			p_mapped = {};
		}
	}

public:
	node_handle(node_handle&&) ynothrow = default;
	~node_handle() = default;

	//! \since build 966
	node_handle&
	operator=(node_handle&&) ynothrowv = default;

	//! \note YStandardEx 扩展。
	YB_ATTR_nodiscard YB_PURE const key_type&
	ckey() const ynothrowv
	{
		return key();
	}

	YB_ATTR_nodiscard YB_PURE key_type&
	key() const ynothrowv
	{
		yconstraint(!this->empty());
		return *p_key;
	}

	YB_ATTR_nodiscard YB_PURE mapped_type&
	mapped() const ynothrowv
	{
		yconstraint(!this->empty());
		return *p_mapped;
	}

	//! \since build 966
	void
	swap(node_handle& nh)
		// XXX: To avoid relying on CWG 1207, do not use 'this'.
		ynoexcept_spec(std::declval<node_handle&>().swap_base(nh))
	{
		this->swap_base(nh);
		ystdex::swap_dependent(p_key, nh.p_key);
		ystdex::swap_dependent(p_mapped, nh.p_mapped);
	}
	friend void
	swap(node_handle& x, node_handle& y) ynoexcept_spec(x.swap(y))
	{
		x.swap(y);
	}

private:
	//! \since build 966
	template<typename _tObj>
	static inline ptr_t<_tObj>
	pointer_to(_tObj& obj)
	{
		return std::pointer_traits<ptr_t<_type>>::pointer_to(obj);
	}
};


//! \brief 集合容器的节点句柄。
template<typename _type, typename _tNodeAlloc>
class node_handle<_type, _type, _tNodeAlloc>
	: public node_handle_base<_type, _tNodeAlloc>
{
	template<typename, typename, typename, typename, class>
	friend class tree;
	template<typename, typename, class, typename, typename, typename, class,
		class>
	friend class table;

private:
	//! \since build 864
	using ator_traits = allocator_traits<_tNodeAlloc>;

public:
	using value_type = _type;

	yconstfn
	node_handle() ynothrow = default;

private:
	//! \pre 间接断言：第一参数非空。
	node_handle(typename ator_traits::pointer p, const _tNodeAlloc& a)
		: node_handle_base<_type, _tNodeAlloc>(p, a)
	{}

public:
	node_handle(node_handle&&) ynothrow = default;
	~node_handle() = default;

	//! \since build 966
	node_handle&
	operator=(node_handle&&) ynothrowv = default;

	//! \note YStandardEx 扩展。
	YB_ATTR_nodiscard YB_PURE const value_type&
	ckey() const ynothrowv
	{
		return value();
	}

	YB_ATTR_nodiscard YB_PURE value_type&
	value() const ynothrowv
	{
		yconstraint(!this->empty());
		return *this->ptr->access_ptr();
	}

	//! \since build 966
	void
	swap(node_handle& nh)
		// XXX: To avoid relying on CWG 1207, do not use 'this'.
		ynoexcept_spec(std::declval<node_handle&>().swap_base(nh))
	{
		this->swap_base(nh);
	}
	friend void
	swap(node_handle& x, node_handle& y) ynoexcept_spec(x.swap(y))
	{
		x->swap_base(y);
	}
};


//! \see ISO C++ [container.insert.return] 。
template<typename _tIter, typename _tNodeHandle>
struct node_insert_return
{
	_tIter position = _tIter();
	bool inserted = {};
	_tNodeHandle node;
};
//!@}

} // namespace details;
//!@}

//! \since build 966
//!@{
//! \relates details::node_handle_base
template<typename _type, class _tNodeAlloc>
struct is_bitwise_swappable<details::node_handle_base<_type, _tNodeAlloc>>
	: true_
{};

//! \relates details::node_handle
template<typename _tKey, typename _type, typename _tNodeAlloc>
struct is_bitwise_swappable<details::node_handle<_tKey, _type, _tNodeAlloc>>
	: true_
{};
//!@}

} // namespace ystdex;

#endif

