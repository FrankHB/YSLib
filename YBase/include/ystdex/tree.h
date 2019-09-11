/*
	© 2018-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file tree.h
\ingroup YStandardEx
\brief 作为关联容器实现的树。
\version r3326
\author FrankHB <frankhb1989@gmail.com>
\since build 830
\par 创建时间:
	2018-07-06 21:15:48 +0800
\par 修改时间:
	2019-09-12 03:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Tree

关联容器的内部实现的搜索树接口。
设计和 libstdc++ 的 <bits/stl_tree.h> 中非公开接口类似，以红黑树作为内部数据结构，
	可同时兼容 std::map 、 std::set 、 std::multimap 和 std::multiset 的容器。
接口支持 ISO C++17 模式下的容器功能，且支持不完整类型作为关联容器的键。
同时，提供了 ISO C++17 节点句柄(node handle) 兼容接口的基本实现。
实现语言的特性要求和 YBase.YStandardEx 要求一致。
为便于参照和测试，实现也和 libstdc++ 在 ISO C++17 模式下的相似，
	但充分利用了其它 YBase.YStandardEx 特性。
因为用于内部实现，接口主要在用于实现的特定命名空间内。
除另行指定的兼容性策略外，作为实现的接口不保证对外稳定。
*/


#ifndef YB_INC_ystdex_tree_hpp_
#define YB_INC_ystdex_tree_hpp_ 1

#include "memory.hpp" // for allocator_traits, is_move_assignable,
//	ystdex::swap_dependent, YAssert, yassume, yconstraint, true_, false_,
//	std::pointer_traits, is_nothrow_copy_constructible, aligned_storage_t,
//	standard_layout_storage, std::allocator, std::bidirectional_iterator_tag,
//	rebind_alloc_t, ystdex::reverse_iterator, is_same, cond_t,
//	is_nothrow_move_assignable, or_, and_, is_invocable, std::declval,
//	ystdex::alloc_on_copy, std::move_if_noexcept,
//	is_trivially_default_constructible, allocator_guard_delete, allocator_guard,
//	is_trivially_destructible, std::pair, enable_if_t, is_nothrow_swappable,
//	ystdex::alloc_on_swap,  ystdex::alloc_on_move;
#include "optional.h" // for optional, bidirectional_iteratable,
//	equality_comparable, totally_ordered, has_mem_is_transparent;
#include "utility.hpp" // for noncopyable, ystdex::as_const;
#include "iterator_trait.hpp" // for has_iterator_value_type;
#include <algorithm> // for std::equal, std::lexicographical_compare;

namespace ystdex
{

//! \since build 830
//@{
namespace details
{

inline namespace rb_tree
{

template<typename, typename, typename, typename, class>
class tree;

} // unnamed namespace;

//! \warning 非虚析构。
//{@
//! \brief 节点句柄基类。
template<typename _type, class _tNodeAlloc>
class node_handle_base
{
	template<typename, typename, typename, typename, class>
	friend class tree;

public:
	using allocator_type = rebind_alloc_t<_tNodeAlloc, _type>;

private:
	//! \since build 864
	using node_ator_traits = allocator_traits<_tNodeAlloc>;

	optional<_tNodeAlloc> alloc{};

protected:
	typename node_ator_traits::pointer ptr{};

	yconstfn
	node_handle_base() ynothrow = default;
	~node_handle_base()
	{
		destroy();
	}

	node_handle_base(node_handle_base&& nh) ynothrow
		: alloc(std::move(nh.alloc)), ptr(nh.ptr)
	{
		nh.ptr = {};
		nh.alloc = nullopt;
	}
	node_handle_base(typename node_ator_traits::pointer p, const _tNodeAlloc& a)
		: ptr(p), alloc(a)
	{}

	node_handle_base&
	operator=(node_handle_base&& nh) ynothrow
	{
		destroy();
		ptr = nh.ptr;
		move_assign(nh, is_move_assignable<_tNodeAlloc>());
		yunseq(nh.ptr = {}, nh.alloc = nullopt);
		return *this;
	}

public:
	explicit
	operator bool() const ynothrow
	{
		return ptr;
	}

protected:
	void
	swap_base(node_handle_base& nh) ynothrow
	{
		ystdex::swap_dependent(ptr, nh.ptr);
		if(node_ator_traits::propagate_on_container_swap::value || !alloc
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
	allocator_type
	get_allocator() const ynothrow
	{
		yconstraint(!this->empty());
		return allocator_type(*alloc);
	}

	YB_ATTR_nodiscard bool
	empty() const ynothrow
	{
		return !ptr;
	}

	void
	move_assign(node_handle_base& nh, true_)
	{
		if(node_ator_traits::propagate_on_container_move_assignment() || !alloc)
			alloc = std::move(nh.alloc);
		else
			YAssert(alloc == nh.alloc, "Unequal allocators found.");
	}
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
	// TODO: Add unordered associative container support?

private:
	//! \since build 864
	using ator_traits = allocator_traits<_tNodeAlloc>;
	template<typename _tObj>
	using ptr_t = typename std::pointer_traits<typename
		ator_traits::pointer>::template rebind<remove_reference_t<_tObj>>;

	ptr_t<_tKey> p_key = {};
	ptr_t<typename _type::second_type> p_mapped = {};

public:
	yconstfn
	node_handle() ynothrow = default;

private:
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

	node_handle&
	operator=(node_handle&&) ynothrow = default;

	using key_type = _tKey;
	using mapped_type = typename _type::second_type;

	const key_type&
	ckey() const ynothrowv
	{
		return key();
	}

	key_type&
	key() const ynothrowv
	{
		yconstraint(!this->empty());
		return *p_key;
	}

	mapped_type&
	mapped() const ynothrowv
	{
		yconstraint(!this->empty());
		return *p_mapped;
	}

	void
	swap(node_handle& nh) ynothrow
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
	template<typename _tObj>
	inline ptr_t<_tObj>
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
	// TODO: Add unordered associative container support?

private:
	//! \since build 864
	using ator_traits = allocator_traits<_tNodeAlloc>;

public:
	using value_type = _type;

	yconstfn
	node_handle() ynothrow = default;

private:
	node_handle(typename ator_traits::pointer p, const _tNodeAlloc& a)
		: node_handle_base<_type, _tNodeAlloc>(p, a)
	{}

public:
	~node_handle() = default;
	node_handle(node_handle&&) ynothrow = default;

	node_handle&
	operator=(node_handle&&) ynothrow = default;

	value_type&
	value() const ynothrowv
	{
		yconstraint(!this->empty());
		return *this->ptr->access_ptr();
	}

	const value_type&
	ckey() const ynothrowv
	{
		return value();
	}

	friend void
	swap(node_handle& x, node_handle& y) ynothrow
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
//@}


inline namespace rb_tree
{

enum class tree_color : bool
{
	red = false,
	black = true
};


//! \warning 非虚析构。
//@{
struct tree_node_base
{
	using base_ptr = tree_node_base*;
	using const_base_ptr = const tree_node_base*;

	// XXX: Uninitialized.
	tree_color color;
	base_ptr parent, left, right;

	static base_ptr
	maximum(base_ptr x) ynothrow
	{
		while(x->right)
			x = x->right;
		return x;
	}

	static const_base_ptr
	maximum(const_base_ptr x) ynothrow
	{
		while(x->right)
			x = x->right;
		return x;
	}

	static base_ptr
	minimum(base_ptr x) ynothrow
	{
		while(x->left)
			x = x->left;
		return x;
	}

	static const_base_ptr
	minimum(const_base_ptr x) ynothrow
	{
		while(x->left)
			x = x->left;
		return x;
	}

	//! \since build 864
	void
	initialize() ynothrow
	{
		reset_links();
		color = tree_color::red;
	}

	void
	reset_links() ynothrow
	{
		parent = {};
		left = this;
		right = this;
	}
};


// NOTE: This should be optimized once %is_final is supported.
template<typename _tKeyComp>
struct tree_key_compare
{
	_tKeyComp key_compare{};

	//! \since build 864
	tree_key_compare() ynoexcept_spec(_tKeyComp())
	{}
	tree_key_compare(const _tKeyComp& comp)
		: key_compare(comp)
	{}
	tree_key_compare(const tree_key_compare&) = default;
	// NOTE: As per ISO C++17 [associative.reqmts], the comparison object shall
	//	be copy constructible.
	// XXX: This is copied.
	tree_key_compare(tree_key_compare&& x)
		ynoexcept(is_nothrow_copy_constructible<_tKeyComp>())
		: key_compare(x.key_compare)
	{}
};


struct tree_header : tree_node_base
{
	size_t node_count;

	tree_header() ynothrow
		: node_count(0)
	{
		initialize();
	}
	tree_header(tree_header&& x) ynothrow
	{
		if(x.parent)
			move_data(x);
		else
		{
			initialize();
			node_count = 0;
		}
	}

	//! \since build 865
	void
	move_data(tree_header& from) ynothrow
	{
		base() = from.base();
		yunseq(parent->parent = &base(), node_count = from.node_count);
		from.reset();
	}

	//! \since build 865
	void
	reset() ynothrow
	{
		reset_links();
		node_count = 0;
	}

	//! \since build 865
	YB_ATTR_nodiscard YB_PURE tree_node_base&
	base() ynothrow
	{
		return *this;
	}
};


template<typename _type>
class tree_node : public tree_node_base
{
private:
	standard_layout_storage<aligned_storage_t<sizeof(_type), yalignof(_type)>>
		storage;

public:
	YB_PURE yconstfn_relaxed YB_PURE _type*
	access_ptr()
	{
		return static_cast<_type*>(storage.access());
	}
	YB_PURE yconstfn YB_PURE const _type*
	access_ptr() const
	{
		return static_cast<const _type*>(storage.access());
	}
};
//@}


YB_API YB_PURE tree_node_base*
tree_increment(tree_node_base*) ynothrow;
YB_API YB_PURE const tree_node_base*
tree_increment(const tree_node_base*) ynothrow;

YB_API YB_PURE tree_node_base*
tree_decrement(tree_node_base*) ynothrow;
YB_API YB_PURE const tree_node_base*
tree_decrement(const tree_node_base*) ynothrow;


//! \warning 非虚析构。
//@{
template<typename, typename _type, class, typename,
	class = std::allocator<_type>>
class tree;

template<typename>
class tree_const_iterator;


template<typename _type>
class tree_iterator
	: public bidirectional_iteratable<tree_iterator<_type>, _type&>
{
	// XXX: Partial specialization is not allowed here by ISO C++.
	template<typename, typename, class, typename, class>
	friend class tree;
	template<typename>
	friend class tree_const_iterator;

public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = _type;
	using pointer = _type*;
	using reference = _type&;
	using difference_type = ptrdiff_t;

private:
	//! \since build 864
	using base_ptr = tree_node_base::base_ptr;
	//! \since build 864
	using link_type = tree_node<_type>*;

	base_ptr p_node = {};

public:
	tree_iterator() ynothrow = default;
	explicit
	tree_iterator(base_ptr x) ynothrow
		: p_node(x)
	{}

	YB_ATTR_nodiscard YB_PURE reference
	operator*() const ynothrow
	{
		return *link_type(p_node)->access_ptr();
	}

	tree_iterator&
	operator++() ynothrow
	{
		p_node = tree_increment(p_node);
		return *this;
	}

	tree_iterator&
	operator--() ynothrow
	{
		p_node = tree_decrement(p_node);
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE bool
	operator==(const tree_iterator& x) const ynothrow
	{
		return p_node == x.p_node;
	}
};


template<typename _type>
class tree_const_iterator
	: public bidirectional_iteratable<tree_const_iterator<_type>, const _type&>,
	public equality_comparable<tree_const_iterator<_type>, tree_iterator<_type>>
{
	// XXX: Partial specialization is not allowed here by ISO C++.
	template<typename, typename, class, typename, class>
	friend class tree;

public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = _type;
	using pointer = const _type*;
	using reference = const _type&;
	using difference_type = ptrdiff_t;

private:
	//! \since build 864
	using iterator = tree_iterator<_type>;
	//! \since build 864
	using base_ptr = tree_node_base::const_base_ptr;
	//! \since build 864
	using link_type = const tree_node<_type>*;

	base_ptr p_node = {};

public:
	tree_const_iterator() ynothrow = default;
	explicit
	tree_const_iterator(base_ptr x) ynothrow
		: p_node(x)
	{}
	tree_const_iterator(const iterator& it) ynothrow
		: p_node(it.p_node)
	{}

	YB_ATTR_nodiscard YB_PURE iterator
	cast_mutable() const ynothrow
	{
		return iterator(const_cast<typename iterator::base_ptr>(p_node));
	}

	YB_ATTR_nodiscard YB_PURE reference
	operator*() const ynothrow
	{
		return *link_type(p_node)->access_ptr();
	}

	tree_const_iterator&
	operator++() ynothrow
	{
		p_node = tree_increment(p_node);
		return *this;
	}

	tree_const_iterator&
	operator--() ynothrow
	{
		p_node = tree_decrement(p_node);
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE bool
	operator==(const tree_const_iterator& x) const ynothrow
	{
		return p_node == x.p_node;
	}
	YB_ATTR_nodiscard YB_PURE bool
	operator==(const iterator& x) const ynothrow
	{
		return p_node == x.p_node;
	}
};
//@}


YB_API void
tree_insert_and_rebalance(bool, tree_node_base*, tree_node_base*,
	tree_node_base&) ynothrow;

YB_API tree_node_base*
tree_rebalance_for_erase(tree_node_base*, tree_node_base&) ynothrow;

YB_API YB_PURE size_t
tree_black_count(const tree_node_base*, const tree_node_base*) ynothrow;


template<typename, typename>
struct tree_merge_helper
{
	~tree_merge_helper() = delete;
};


//! \warning 非虚析构。
template<typename _tKey, typename _type, typename _fKeyOfValue, typename _fComp,
	class _tAlloc>
class tree
	: private totally_ordered<tree<_tKey, _type, _fKeyOfValue, _fComp, _tAlloc>>
{
	static_assert(is_invocable<_fComp&, const _tKey&, const _tKey&>(),
		"Invalid comparison object type found.");
	// XXX: LWG 2542.
	static_assert(is_invocable<const _fComp&, const _tKey&, const _tKey&>(),
		"Invalid comparison object type found.");

	template<typename, typename>
	friend struct tree_merge_helper;

public:
	using key_type = _tKey;
	using value_type = _type;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	/*!
	\brief 分配器类型。
	\note 支持 uses-allocator 构造。
	*/
	using allocator_type = _tAlloc;
	using iterator = tree_iterator<value_type>;
	using const_iterator = tree_const_iterator<value_type>;
	using reverse_iterator = ystdex::reverse_iterator<iterator>;
	using const_reverse_iterator = ystdex::reverse_iterator<const_iterator>;

private:
	//! \since build 865
	using value_node = tree_node<value_type>;
	using node_allocator = rebind_alloc_t<_tAlloc, value_node>;

public:
	using node_type = node_handle<_tKey, _type, node_allocator>;
	using insert_return_type = node_insert_return<
		cond_t<is_same<_tKey, _type>, const_iterator, iterator>, node_type>;

protected:
	using base_ptr = tree_node_base*;
	using const_base_ptr = const tree_node_base*;
	using link_type = value_node*;
	using const_link_type = const value_node*;

private:
	//! \since build 864
	using node_ator_traits = allocator_traits<node_allocator>;
	//! \since build 865
	using equal_alloc_or_pocma = or_<typename node_ator_traits::is_always_equal,
		typename node_ator_traits::propagate_on_container_move_assignment>;
	struct alloc_node
	{
		tree& tree_ref;

		alloc_node(tree& t)
			: tree_ref(t)
		{}

		template<typename _tParam>
		inline link_type
		operator()(_tParam&& arg) const
		{
			return tree_ref.create_node(yforward(arg));
		}

		template<typename _tParam>
		link_type
		reconstruct(base_ptr p, _tParam&& arg) const
		{
			const auto nd = link_type(p);

			tree_ref.destroy_node(nd);
			tree_ref.construct_node(nd, yforward(arg));
			return nd;
		}
	};
	struct reuse_or_alloc_node : protected alloc_node, private noncopyable
	{
		mutable base_ptr root;
		mutable base_ptr nodes;

		reuse_or_alloc_node(tree& t)
			: alloc_node(t), root(t.root()), nodes(t.rightmost())
		{
			if(root)
			{
				root->parent = {};
				if(nodes->left)
					nodes = nodes->left;
			}
			else
				nodes = {};
		}
		~reuse_or_alloc_node()
		{
			alloc_node::tree_ref.erase_node(link_type(root));
		}

		template<typename _tParam>
		link_type
		operator()(_tParam&& arg) const
		{
			if(nodes)
				return alloc_node::reconstruct(adjust(), yforward(arg));
			return alloc_node::operator()(yforward(arg));
		}

		//! \since build 866
		base_ptr
		adjust() const
		{
			auto node(nodes);

			nodes = nodes->parent;
			if(nodes)
			{
				if(nodes->right == node)
				{
					nodes->right = {};
					if(nodes->left)
					{
						nodes = nodes->left;
						while(nodes->right)
							nodes = nodes->right;
						if(nodes->left)
							nodes = nodes->left;
					}
				}
				else
					nodes->left = {};
			}
			else
				root = {};
			return node;
		}
	};
	template<typename _fComp2>
	using compatible_tree_type
		= tree<_tKey, _type, _fKeyOfValue, _fComp2, _tAlloc>;

protected:
	template<typename _tKeyComp>
	struct components : node_allocator, tree_key_compare<_tKeyComp>
	{
		using base_key_compare = tree_key_compare<_tKeyComp>;

		tree_header header{};

		//! \since build 864
		components() ynoexcept(
			noexcept(node_allocator()) && noexcept(base_key_compare()))
			: node_allocator(), base_key_compare()
		{}
		//! \since build 866
		explicit
		components(node_allocator&& a) ynothrow
			: node_allocator(std::move(a))
		{}
		components(const _tKeyComp& comp, node_allocator&& a)
			: node_allocator(std::move(a)), base_key_compare(comp)
		{}
		components(const components& x)
			: node_allocator(
			node_ator_traits::select_on_container_copy_construction(x)),
			base_key_compare(x.key_compare)
		{}
		components(components&&) = default;
		//! \since build 866
		components(components&& x, node_allocator&& a)
			: node_allocator(std::move(a)), base_key_compare(std::move(x)),
			header(std::move(x.header))
		{}

		//! \since build 865
		void
		move_data(components& from) ynothrow
		{
			header.move_data(from.header);
		}
	};

	components<_fComp> objects;

public:
	tree() = default;
	tree(const allocator_type& a)
		: objects(node_allocator(a))
	{}
	tree(const _fComp& comp, const allocator_type& a = allocator_type())
		: objects(comp, node_allocator(a))
	{}
	tree(const tree& x)
		: objects(x.objects)
	{
		if(x.root())
			root() = copy_node(x);
	}
	tree(const tree& x, const allocator_type& a)
		: objects(x.objects.key_compare, node_allocator(a))
	{
		if(x.root())
			root() = copy_node(x);
	}
	tree(tree&&) = default;
	tree(tree&& x, const allocator_type& a)
		: tree(std::move(x), node_allocator(a))
	{}
	//! \since build 866
	//@{
	tree(tree&& x, node_allocator&& a)
		// XXX: %is_nothrow_constructible is not usable for the incomplete type.
		ynoexcept(noexcept(tree(std::move(x),
		std::move(a), typename node_ator_traits::is_always_equal())))
		: tree(std::move(x), std::move(a),
		typename node_ator_traits::is_always_equal())
	{}

private:
	tree(tree&& x, node_allocator&& a, true_)
		ynoexcept(is_nothrow_default_constructible<_fComp>())
		: objects(std::move(x.objects), std::move(a))
	{}
	tree(tree&& x, node_allocator&& a, false_)
		: objects(x.objects.key_compare, std::move(a))
	{
		if(x.root())
			move_data(x, false_());
	}
	//@}

public:
	~tree() ynothrow
	{
		clear_nodes();
	}

	tree&
	operator=(const tree& x)
	{
		if(&x != this)
		{
			if(typename
				node_ator_traits::propagate_on_container_copy_assignment())
			{
				auto& this_alloc(get_node_allocator());
				auto& that_alloc(x.get_node_allocator());

				if(!typename node_ator_traits::is_always_equal()
					&& this_alloc != that_alloc)
					clear();
				ystdex::alloc_on_copy(this_alloc, that_alloc);
			}

			reuse_or_alloc_node roan(*this);

			objects.header.reset();
			objects.key_compare = x.objects.key_compare;
			if(x.root())
				root() = copy_node(x, roan);
		}
		return *this;
	}
	//! \since build 864
	tree&
	operator=(tree&& x) ynoexcept(and_<equal_alloc_or_pocma,
		is_nothrow_move_assignable<_fComp>>())
	{
		// NOTE: Like ISO C++ [res.on.arguments], moving same object to itself
		//	is not supported to meet the postcondition of the move assignment,
		//	and the result is unspecified. Since it is still well-defined, there
		//	is no assertion here.
		// XXX: Moving from an ancestor to its subtree (composed by elements of
		//	recursive container type) is also not supported and it would
		//	typically lead to resource leaks, however there is no check anyway.
		//	This is different to ISO C++ as associative containers do not
		//	support incomplete element types so the recursive container type
		//	itself causes undefined behavior in such cases. Use %swap if the
		//	operand can reference to the object itself or its subtrees thereby.
		objects.key_compare = std::move(x.objects.key_compare);
		move_assign_elements(x, equal_alloc_or_pocma());
		return *this;
	}

private:
	/*!
	\brief 转移可能不相等分配器的容器元素。
	\note 从不相等分配器转移元素结果是复制而不是转移对象。
	*/
	void
	move_data(tree& x, false_)
	{
		if(get_node_allocator() == x.get_node_allocator())
			move_data(x, true_());
		else
		{
			alloc_node an(*this);

			root() = copy_node(x, [&an](const value_type& cval){
				return an(std::move_if_noexcept(const_cast<value_type&>(cval)));
			});
		}
	}
	/*!
	\brief 转移相等分配器的容器元素。
	\since build 865
	*/
	void
	move_data(tree& x, true_) ynothrow
	{
		objects.move_data(x.objects);
	}

	/*!
	\brief 转移赋值可能不相等的不在转移赋值时传播的分配器的容器元素。
	\note 从不相等分配器转移元素结果是可能是复制而不是转移对象。
	*/
	void
	move_assign_elements(tree& x, false_)
	{
		if(get_node_allocator() == x.get_node_allocator())
			move_assign_elements(x, true_());
		else
		{
			// NOTE: Try to move reusing existing nodes.
			reuse_or_alloc_node roan(*this);

			objects.header.reset();
			if(x.root())
			{
				root() = copy_node(x, [&roan](const value_type& cval){
					return roan(
						std::move_if_noexcept(const_cast<value_type&>(cval)));
				});
				x.clear();
			}
		}
	}
	/*!
	\brief 转移赋值相等或在转移赋值时传播的分配器的容器元素。
	\since build 865
	*/
	void
	move_assign_elements(tree& x, true_) ynothrow
	{
		clear_nodes();
		if(x.root())
			move_data(x, true_());
		else
			objects.header.reset();
		ystdex::alloc_on_move(get_node_allocator(), x.get_node_allocator());
	}

public:
	YB_ATTR_nodiscard allocator_type
	get_allocator() const ynothrow
	{
		return allocator_type(get_node_allocator());
	}

	YB_ATTR_nodiscard YB_PURE node_allocator&
	get_node_allocator() ynothrow
	{
		return objects;
	}
	YB_ATTR_nodiscard YB_PURE const node_allocator&
	get_node_allocator() const ynothrow
	{
		return objects;
	}

	YB_ATTR_nodiscard YB_PURE iterator
	begin() ynothrow
	{
		return iterator(objects.header.left);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	begin() const ynothrow
	{
		return const_iterator(objects.header.left);
	}

	YB_ATTR_nodiscard YB_PURE iterator
	end() ynothrow
	{
		return iterator(&objects.header);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	end() const ynothrow
	{
		return const_iterator(&objects.header);
	}

	YB_ATTR_nodiscard YB_PURE reverse_iterator
	rbegin() ynothrow
	{
		return reverse_iterator(end());
	}
	YB_ATTR_nodiscard YB_PURE const_reverse_iterator
	rbegin() const ynothrow
	{
		return const_reverse_iterator(end());
	}
	YB_ATTR_nodiscard YB_PURE reverse_iterator
	rend() ynothrow
	{
		return reverse_iterator(begin());
	}

	YB_ATTR_nodiscard YB_PURE const_reverse_iterator
	rend() const ynothrow
	{
		return const_reverse_iterator(begin());
	}

	YB_ATTR_nodiscard bool
	empty() const ynothrow
	{
		return objects.header.node_count == 0;
	}

	YB_ATTR_nodiscard size_type
	size() const ynothrow
	{
		return objects.header.node_count;
	}

	YB_ATTR_nodiscard size_type
	max_size() const ynothrow
	{
		return node_ator_traits::max_size(get_node_allocator());
	}

protected:
	//! \since build 865
	YB_ALLOCATOR YB_ATTR_returns_nonnull link_type
	allocate_node()
	{
		return node_ator_traits::allocate(get_node_allocator(), 1);
	}

	//! \since build 865
	void
	deallocate_node(link_type p) ynothrow
	{
		node_ator_traits::deallocate(get_node_allocator(), p, 1);
	}

	template<typename... _tParams>
	void
	construct_node(link_type nd, _tParams&&... args)
	{
		// XXX: Ensure no need to use placment new for the node.
		static_assert(is_trivially_default_constructible<value_node>(),
			"Invalid node type found.");
		auto& a(get_node_allocator());
		// NOTE: This should be same to %deallocate_node(nd) on exception
		//	thrown.
		allocator_guard<node_allocator>
			gd(nd, allocator_guard_delete<node_allocator>(a, 1));

		node_ator_traits::construct(a, nd->access_ptr(), yforward(args)...);
		gd.release();
	}

	template<typename... _tParams>
	YB_ATTR_nodiscard inline link_type
	create_node(_tParams&&... args)
	{
		const auto nd(allocate_node());

		construct_node(nd, yforward(args)...);
		return nd;
	}

	void
	destroy_node(link_type nd) ynothrow
	{
		// XXX: Ensure no need to use explicit destructor call for the node.
		static_assert(is_trivially_destructible<value_node>(),
			"Invalid node type found.");
		node_ator_traits::destroy(get_node_allocator(), nd->access_ptr());
	}

	void
	drop_node(link_type nd) ynothrow
	{
		destroy_node(nd);
		deallocate_node(nd);
	}

private:
	//! \since build 866
	void
	attach_node(bool insert_left, base_ptr p, link_type nd)
	{
		tree_insert_and_rebalance(insert_left, nd, p, objects.header);
		++objects.header.node_count;
	}

	//! \since build 866
	YB_ATTR_nodiscard link_type
	detach_node(base_ptr cur)
	{
		--objects.header.node_count;
		return link_type(tree_rebalance_for_erase(cur, objects.header));
	}

protected:
	base_ptr&
	root() ynothrow
	{
		return objects.header.parent;
	}
	const_base_ptr
	root() const ynothrow
	{
		return objects.header.parent;
	}

	base_ptr&
	leftmost() ynothrow
	{
		return objects.header.left;
	}
	const_base_ptr
	leftmost() const ynothrow
	{
		return objects.header.left;
	}

	base_ptr&
	rightmost() ynothrow
	{
		return objects.header.right;
	}
	const_base_ptr
	rightmost() const ynothrow
	{
		return objects.header.right;
	}

	link_type
	node_begin() ynothrow
	{
		return link_type(objects.header.parent);
	}
	const_link_type
	node_begin() const ynothrow
	{
		return const_link_type(objects.header.parent);
	}

	base_ptr
	node_end() ynothrow
	{
		return &objects.header;
	}
	const_base_ptr
	node_end() const ynothrow
	{
		return &objects.header;
	}

	static const _tKey&
	select_key(const_base_ptr x)
	{
		return _fKeyOfValue()(select_value(x));
	}
	static const _tKey&
	select_key(const_link_type x)
	{
		return _fKeyOfValue()(select_value(x));
	}

	static const_reference
	select_value(const_link_type x)
	{
		return *x->access_ptr();
	}
	static const_reference
	select_value(const_base_ptr x)
	{
		return *const_link_type(x)->access_ptr();
	}

	static link_type
	get_left(base_ptr x) ynothrow
	{
		return link_type(x->left);
	}
	static const_link_type
	get_left(const_base_ptr x) ynothrow
	{
		return const_link_type(x->left);
	}

	static link_type
	get_right(base_ptr x) ynothrow
	{
		return link_type(x->right);
	}
	static const_link_type
	get_right(const_base_ptr x) ynothrow
	{
		return const_link_type(x->right);
	}

	static base_ptr
	maximum(base_ptr x) ynothrow
	{
		return tree_node_base::maximum(x);
	}
	static const_base_ptr
	maximum(const_base_ptr x) ynothrow
	{
		return tree_node_base::maximum(x);
	}

	static base_ptr
	minimum(base_ptr x) ynothrow
	{
		return tree_node_base::minimum(x);
	}
	static const_base_ptr
	minimum(const_base_ptr x) ynothrow
	{
		return tree_node_base::minimum(x);
	}

private:
	//! \since build 840
	template<typename _tParam>
	inline bool
	is_insert_left(base_ptr p, const _tParam& arg)
	{
		return p == node_end() || !objects.key_compare(select_key(p), arg);
	}

	//! \since build 840
	template<typename _tParam>
	inline bool
	is_insert_left_strict(base_ptr p, const _tParam& arg)
	{
		return p == node_end() || objects.key_compare(arg, select_key(p));
	}

	//! \since build 866
	iterator
	insert_attach(bool insert_left, base_ptr p, link_type nd)
	{
		attach_node(insert_left, p, nd);
		return iterator(nd);
	}

	//! \since build 866
	bool
	insert_left_q(const std::pair<base_ptr, base_ptr>& pr, const key_type& k)
	{
		return bool(pr.first) || is_insert_left_strict(pr.second, k);
	}

	iterator
	insert_node(base_ptr x, base_ptr p, link_type nd)
	{
		return insert_attach(bool(x)
			|| is_insert_left_strict(p, select_key(nd)), p, nd);
	}

	template<typename _tParam>
	iterator
	insert_equal_lower(_tParam&& v)
	{
		auto x(node_begin());
		auto y(node_end());

		while(x)
		{
			y = x;
			x = !objects.key_compare(select_key(x), _fKeyOfValue()(v))
				? get_left(x) : get_right(x);
		}

		// NOTE: This is checked first to avoid stray node allocated when the
		//	key comparation throws.
		const bool insert_left(is_insert_left(y, _fKeyOfValue()(v)));

		return insert_attach(insert_left, y, create_node(yforward(v)));
	}

	iterator
	insert_equal_lower_node(link_type nd)
	{
		auto x(node_begin());
		auto y(node_end());

		while(x)
		{
			y = x;
			x = !objects.key_compare(select_key(x), select_key(nd))
				? get_left(x) : get_right(x);
		}
		return insert_attach(is_insert_left(y, select_key(nd)), y, nd);
	}

	template<class _tNodeGen>
	link_type
	copy_node(const_link_type x, base_ptr p, const _tNodeGen& node_gen)
	{
		yassume(x), yassume(p);

		const auto clone_node([&](const_link_type s){
			link_type res(node_gen(*s->access_ptr()));

			yunseq(res->color = s->color, res->left = {}, res->right = {});
			return res;
		});
		const auto top(clone_node(x));

		top->parent = p;
		try
		{
			if(x->right)
				top->right = copy_node(get_right(x), top, node_gen);
			p = top;
			x = get_left(x);

			while(x)
			{
				const auto y(clone_node(x));

				p->left = y;
				y->parent = p;
				if(x->right)
					y->right = copy_node(get_right(x), y, node_gen);
				p = y;
				x = get_left(x);
			}
		}
		catch(...)
		{
			erase_node(top);
			throw;
		}
		return top;
	}
	template<class _tNodeGen>
	link_type
	copy_node(const tree& x, const _tNodeGen& node_gen)
	{
		const auto root(copy_node(x.node_begin(), node_end(), node_gen));

		yunseq(leftmost() = minimum(root), rightmost() = maximum(root),
			objects.header.node_count = x.objects.header.node_count);
		return root;
	}
	link_type
	copy_node(const tree& x)
	{
		alloc_node an(*this);

		return copy_node(x, an);
	}

	void
	erase_node(link_type x) ynothrow
	{
		while(x)
		{
			erase_node(get_right(x));

			auto y(get_left(x));

			drop_node(x);
			x = y;
		}
	}

	template<typename _tIter, class _tTree, typename _tLink, typename _tBasePtr>
	static _tIter
	lower_bound_impl(_tTree& t, _tLink x, _tBasePtr y, const _tKey& k)
	{
		while(x)
			if(!t.objects.key_compare(select_key(x), k))
			{
				y = x;
				x = get_left(x);
			}
			else
				x = get_right(x);
		return _tIter(y);
	}
	template<typename _tIter, class _tTree>
	static inline _tIter
	lower_bound_impl(_tTree& t, const key_type& k)
	{
		return lower_bound_impl<_tIter>(t, t.node_begin(), t.node_end(), k);
	}

	template<typename _tIter, class _tTree, typename _tLink, typename _tBasePtr>
	static _tIter
	upper_bound_impl(_tTree& t, _tLink x, _tBasePtr y, const _tKey& k)
	{
		while(x)
			if(t.objects.key_compare(k, select_key(x)))
			{
				y = x;
				x = get_left(x);
			}
			else
				x = get_right(x);
		return _tIter(y);
	}
	template<typename _tIter, class _tTree>
	static inline _tIter
	upper_bound_impl(_tTree& t, const key_type& k)
	{
		return lower_bound_impl<_tIter>(t, t.node_begin(), t.node_end(), k);
	}

	//! \since build 840
	template<typename _tIter, class _tTree>
	std::pair<_tIter, _tIter>
	equal_range_impl(_tTree& t, const key_type& k)
	{
		using pr_t = std::pair<_tIter, _tIter>;
		auto x(t.node_begin());
		auto y(t.node_end());

		while(x)
		{
			const auto& t_comp(t.objects.key_compare);

			if(t_comp(select_key(x), k))
				x = get_right(x);
			else if(t_comp(k, select_key(x)))
			{
				y = x;
				x = get_left(x);
			}
			else
			{
				auto xu(x);
				auto yu(y);

				y = x;
				x = get_left(x);
				xu = get_right(xu);
				return pr_t(lower_bound_impl<_tIter>(t, x, y, k),
					upper_bound_impl<_tIter>(t, xu, yu, k));
			}
		}
		return pr_t(_tIter(y), _tIter(y));
	}

	template<typename _tIter, class _tTree>
	static _tIter
	find_impl(_tTree& t, const key_type& k)
	{
		const auto
			j(lower_bound_impl<_tIter>(t, t.node_begin(), t.node_end(), k));

		return (j == t.end()
			|| t.objects.key_compare(k, select_key(j.p_node))) ? t.end() : j;
	}

	iterator
	lower_bound_link(link_type x, base_ptr y, const _tKey& k)
	{
		return lower_bound_impl<iterator>(*this, x, y, k);
	}
	const_iterator
	lower_bound_link(const_link_type x, const_base_ptr y, const _tKey& k) const
	{
		return lower_bound_impl<const_iterator>(*this, x, y, k);
	}

	iterator
	upper_bound_link(link_type x, base_ptr y, const _tKey& k)
	{
		return upper_bound_impl<iterator>(*this, x, y, k);
	}
	const_iterator
	upper_bound_link(const_link_type x, const_base_ptr y, const _tKey& k) const
	{
		return upper_bound_impl<const_iterator>(*this, x, y, k);
	}

public:
	std::pair<base_ptr, base_ptr>
	get_insert_unique_pos(const key_type& k)
	{
		using res_t = std::pair<base_ptr, base_ptr>;
		auto x(node_begin());
		auto y(node_end());
		bool comp(true);

		while(x)
		{
			y = x;
			comp = objects.key_compare(k, select_key(x));
			x = comp ? get_left(x) : get_right(x);
		}

		auto j = iterator(y);

		if(comp)
		{
			if(j == begin())
				return res_t(x, y);
			else
				--j;
		}
		if(objects.key_compare(select_key(j.p_node), k))
			return res_t(x, y);
		return res_t(j.p_node, {});
	}

	std::pair<base_ptr, base_ptr>
	get_insert_equal_pos(const key_type& k)
	{
		auto x(node_begin());
		auto y(node_end());

		while(x)
		{
			y = x;
			x = objects.key_compare(k, select_key(x)) ? get_left(x)
				: get_right(x);
		}
		return std::pair<base_ptr, base_ptr>(x, y);
	}

	std::pair<base_ptr, base_ptr>
	get_insert_hint_unique_pos(const_iterator position, const key_type& k)
	{
		// TODO: Simplify.
		using res_t = std::pair<base_ptr, base_ptr>;
		auto mpos(position.cast_mutable());

		if(mpos.p_node == node_end())
		{
			if(size() > 0 && objects.key_compare(select_key(rightmost()), k))
				return res_t({}, rightmost());
			else
				return get_insert_unique_pos(k);
		}
		else if(objects.key_compare(k, select_key(mpos.p_node)))
		{
			auto before(mpos);

			if(mpos.p_node == leftmost())
				return res_t(leftmost(), leftmost());
			else if(objects.key_compare(select_key((--before).p_node), k))
				return !get_right(before.p_node) ? res_t({}, before.p_node)
					: res_t(mpos.p_node, mpos.p_node);
			else
				return get_insert_unique_pos(k);
		}
		else if(objects.key_compare(select_key(mpos.p_node), k))
		{
			auto after(mpos);

			if(mpos.p_node == rightmost())
				return res_t({}, rightmost());
			else if(objects.key_compare(k, select_key((++after).p_node)))
				return !get_right(mpos.p_node) ? res_t({}, mpos.p_node)
					: res_t(after.p_node, after.p_node);
			else
				return get_insert_unique_pos(k);
		}
		else
			return res_t(mpos.p_node, {});
	}

	std::pair<base_ptr, base_ptr>
	get_insert_hint_equal_pos(const_iterator position, const key_type& k)
	{
		using res_t = std::pair<base_ptr, base_ptr>;
		auto mpos(position.cast_mutable());

		if(mpos.p_node == node_end())
		{
			if(size() > 0 && !objects.key_compare(k, select_key(rightmost())))
				return res_t(0, rightmost());
			else
				return get_insert_equal_pos(k);
		}
		else if(!objects.key_compare(select_key(mpos.p_node), k))
		{
			auto before(mpos);

			if(mpos.p_node == leftmost())
				return res(leftmost(), leftmost());
			else if(!objects.key_compare(k, select_key((--before).p_node)))
			{
				if(!get_right(before.p_node))
					return res_t(0, before.p_node);
				else
					return res_t(mpos.p_node, mpos.p_node);
			}
			else
				return get_insert_equal_pos(k);
		}
		else
		{
			auto after(mpos);

			if(mpos.p_node == rightmost())
				return res_t(0, rightmost());
			else if(!objects.key_compare(select_key((++after).p_node), k))
			{
				if(!get_right(mpos.p_node))
					return res_t({}, mpos.p_node);
				else
					return res_t(after.p_node, after.p_node);
			}
			else
				return res_t({}, {});
		}
	}

	template<typename _tParam>
	std::pair<iterator, bool>
	insert_unique(_tParam&& v)
	{
		alloc_node an(*this);

		return insert_unique(yforward(v), an);
	}
	//! \since build 866
	template<typename _tParam, class _tNodeGen>
	std::pair<iterator, bool>
	insert_unique(_tParam&& v, const _tNodeGen& node_gen)
	{
		using res_t = std::pair<iterator, bool>;
		const auto& k(_fKeyOfValue()(v));
		const auto res(get_insert_unique_pos(k));

		if(res.second)
		{
			// NOTE: This is checked first to avoid stray node allocated when
			//	the key comparation throws.
			const bool insert_left(insert_left_q(res, k));

			// XXX: Assume the key in the constructed node is equivalent to %k.
			return res_t(insert_attach(insert_left, res.second,
				link_type(node_gen(yforward(v)))), true);
		}
		return res_t(iterator(res.first), {});
	}

	template<typename _tParam>
	iterator
	insert_equal(_tParam&& v)
	{
		alloc_node an(*this);

		return insert_equal(yforward(v), an);
	}
	//! \since build 866
	template<typename _tParam, class _tNodeGen>
	iterator
	insert_equal(_tParam&& v, const _tNodeGen& node_gen)
	{
		const auto& k(_fKeyOfValue()(v));
		const auto res(get_insert_equal_pos(k));
		// NOTE: This is checked first to avoid stray node allocated when the
		//	key comparation throws.
		const bool insert_left(insert_left_q(res, k));

		// XXX: Assume the key in the constructed node is equivalent to %k.
		return insert_attach(insert_left, res.second,
			link_type(node_gen(yforward(v))));
	}

	template<typename _tParam>
	iterator
	insert_hint_unique(const_iterator position, _tParam&& x)
	{
		alloc_node an(*this);

		return insert_hint_unique(position, yforward(x), an);
	}
	template<typename _tParam, class _tNodeGen>
	iterator
	insert_hint_unique(const_iterator position, _tParam&& v,
		const _tNodeGen& node_gen)
	{
		const auto& k(_fKeyOfValue()(v));
		const auto res(get_insert_hint_unique_pos(position, k));

		if(res.second)
		{
			// NOTE: This is checked first to avoid stray node allocated when
			//	the key comparation throws.
			const bool insert_left(insert_left_q(res, k));

			// XXX: Assume the key in the constructed node is equivalent to %k.
			return insert_attach(insert_left, res.second,
				link_type(node_gen(yforward(v))));
		}
		return iterator(res.first);
	}

	template<typename _tParam>
	inline iterator
	insert_hint_equal(const_iterator position, _tParam&& x)
	{
		alloc_node an(*this);

		return insert_hint_equal(position, yforward(x), an);
	}
	template<typename _tParam, class _tNodeGen>
	iterator
	insert_hint_equal(const_iterator position, _tParam&& v,
		const _tNodeGen& node_gen)
	{
		const auto& k(_fKeyOfValue()(v));
		const auto res(get_insert_hint_equal_pos(position, k));

		if(res.second)
		{
			// NOTE: This is checked first to avoid stray node allocated when
			//	the key comparation throws.
			const bool insert_left(insert_left_q(res, k));

			// XXX: Assume the key in the constructed node is equivalent to %k.
			return insert_attach(insert_left, res.second,
				link_type(node_gen(yforward(v))));
		}
		return insert_equal_lower(yforward(v));
	}

private:
	// XXX: The return type can be incomplete before this template is
	//	instantiated.
	//! \since build 864
	template<typename _func, yimpl(typename _tRes
		= decltype(std::declval<_func&>()(std::declval<const key_type&>())))>
	inline _tRes
	insert_or_drop(link_type nd, _func f)
	{
		try
		{
			return f(select_key(nd));
		}
		catch(...)
		{
			drop_node(nd);
			throw;
		}
	}

public:
	template<typename... _tParams>
	YB_FLATTEN inline std::pair<iterator, bool>
	emplace_unique(_tParams&&... args)
	{
		return emplace_unique_node(create_node(yforward(args)...));
	}

private:
	//! \since build 864
	std::pair<iterator, bool>
	emplace_unique_node(link_type nd)
	{
		return insert_or_drop(nd,
			[&](const key_type& k) -> std::pair<iterator, bool>{
			const auto pr(get_insert_unique_pos(k));
	
			if(pr.second)
				return {insert_node(pr.first, pr.second, nd), true};
			drop_node(nd);
			return {iterator(pr.first), {}};
		});
	}

public:
	template<typename... _tParams>
	YB_FLATTEN inline iterator
	emplace_equal(_tParams&&... args)
	{
		return emplace_equal_node(create_node(yforward(args)...));
	}

private:
	//! \since build 864
	iterator
	emplace_equal_node(link_type nd)
	{
		return insert_or_drop(nd, [&](const key_type& k){
			const auto pr(get_insert_equal_pos(k));

			return insert_node(pr.first, pr.second, nd);
		});
	}

public:
	template<typename... _tParams>
	YB_FLATTEN inline iterator
	emplace_hint_unique(const_iterator position, _tParams&&... args)
	{
		return
			emplace_hint_unique_node(position, create_node(yforward(args)...));
	}

private:
	//! \since build 864
	iterator
	emplace_hint_unique_node(const_iterator position, link_type nd)
	{
		return insert_or_drop(nd, [&](const key_type& k){
			const auto pr(get_insert_hint_unique_pos(position, k));

			if(pr.second)
				return insert_node(pr.first, pr.second, nd);
			drop_node(nd);
			return iterator(pr.first);
		});
	}

public:
	template<typename... _tParams>
	YB_FLATTEN inline iterator
	emplace_hint_equal(const_iterator position, _tParams&&... args)
	{
		return
			emplace_hint_equal_node(position, create_node(yforward(args)...));
	}

private:
	//! \since build 864
	iterator
	emplace_hint_equal_node(const_iterator position, link_type nd)
	{
		return insert_or_drop([&](const key_type& k){
			const auto pr(get_insert_hint_equal_pos(position, k));

			return pr.second ? insert_node(pr.first, pr.second, nd)
				: insert_equal_lower_node(nd);
		});
	}

public:
	//! \since build 866
	//@{
	template<typename _tIn>
	enable_if_t<has_iterator_value_type<value_type, _tIn>::value>
	insert_range_unique(_tIn first, _tIn last)
	{
		alloc_node an(*this);

		for(; first != last; ++first)
			insert_unique(*first, an);
	}
	template<typename _tIn>
	enable_if_t<!has_iterator_value_type<value_type, _tIn>::value>
	insert_range_unique(_tIn first, _tIn last)
	{
		for(; first != last; ++first)
			emplace_hint_unique(end(), *first);
	}

	template<typename _tIn>
	enable_if_t<has_iterator_value_type<value_type, _tIn>::value>
	insert_range_equal(_tIn first, _tIn last)
	{
		alloc_node an(*this);

		for(; first != last; ++first)
			insert_equal(*first, an);
	}
	template<typename _tIn>
	enable_if_t<!has_iterator_value_type<value_type, _tIn>::value>
	insert_range_equal(_tIn first, _tIn last)
	{
		alloc_node an(*this);

		for(; first != last; ++first)
			emplace_hint_equal(end(), *first);
	}
	//@}

private:
	//! \since build 866
	void
	erase_it(iterator position)
	{
		drop_node(detach_node(position.p_node));
	}
	//! \since build 866
	void
	erase_it(iterator first, iterator last)
	{
		if(first == begin() && last == end())
			clear();
		else
			while(first != last)
				erase_it(first++);
	}

public:
	YB_FLATTEN iterator
	erase(const_iterator position)
	{
		yconstraint(position != end());

		auto res(position.cast_mutable());

		++res;
		erase_it(position.cast_mutable());
		return res;
	}
	YB_FLATTEN iterator
	erase(iterator position)
	{
		yconstraint(position != end());

		auto res(position);

		++res;
		erase_it(position);
		return res;
	}
	YB_FLATTEN size_type
	erase(const key_type& x)
	{
		const auto pr(equal_range(x));
		const size_type old_size(size());
	
		erase_it(pr.first, pr.second);
		return old_size - size();
	}
	YB_FLATTEN iterator
	erase(const_iterator first, const_iterator last)
	{
		const auto res(last.cast_mutable());

		erase_it(first.cast_mutable(), res);
		return res;
	}
	YB_FLATTEN void
	erase(const key_type* first, const key_type* last)
	{
		while(first != last)
			erase(*first++);
	}

	//! \since build 864
	friend void
	swap(tree& x, tree& y) ynoexcept(is_nothrow_swappable<_fComp>())
	{
		if(!x.root())
		{
			if(y.root())
				x.objects.move_data(y.objects);
		}
		else if(!y.root())
			y.objects.move_data(x.objects);
		else
		{
			std::swap(x.root(), y.root());
			std::swap(x.leftmost(), y.leftmost());
			std::swap(x.rightmost(), y.rightmost());
			yunseq(x.root()->parent = x.node_end(),
				y.root()->parent = y.node_end());
			std::swap(x.objects.header.node_count, y.objects.header.node_count);
		}
		// NOTE: As libstdc++'s implementation, the header's color does not
		//	change.
		std::swap(x.objects.key_compare, y.objects.key_compare);
		ystdex::alloc_on_swap(x.get_node_allocator(), y.get_node_allocator());
	}

	void
	clear() ynothrow
	{
		clear_nodes();
		objects.header.reset();
	}

private:
	//! \since build 866
	void
	clear_nodes() ynothrow
	{
		erase_node(node_begin());
	}

public:
	YB_ATTR_nodiscard _fComp
	key_comp() const
	{
		return objects.key_compare;
	}

	YB_ATTR_nodiscard YB_PURE iterator
	find(const key_type& k)
	{
		return find_impl<iterator>(*this, k);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	find(const key_type& k) const
	{
		return find_impl<const_iterator>(*this, k);
	}

	YB_ATTR_nodiscard YB_PURE size_type
	count(const key_type& k) const
	{
		const auto pr(equal_range(k));

		return size_type(std::distance(pr.first, pr.second));
	}

	YB_ATTR_nodiscard YB_PURE iterator
	lower_bound(const key_type& k)
	{
		return lower_bound_impl<iterator>(*this, k);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	lower_bound(const key_type& k) const
	{
		return lower_bound_impl<const_iterator>(*this, k);
	}

	YB_ATTR_nodiscard YB_PURE iterator
	upper_bound(const key_type& k)
	{
		return upper_bound_impl<iterator>(*this, k);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	upper_bound(const key_type& k) const
	{
		return upper_bound_impl<const_iterator>(*this, k);
	}

	YB_ATTR_nodiscard YB_PURE std::pair<iterator, iterator>
	equal_range(const key_type& k)
	{
		return equal_range_impl<iterator>(*this, k);
	}
	YB_ATTR_nodiscard YB_PURE std::pair<const_iterator, const_iterator>
	equal_range(const key_type& k) const
	{
		return equal_range_impl<iterator>(*this, k);
	}

	//! \since build 844
	//@{
	template<typename _tTransKey, typename
		= enable_if_t<has_mem_is_transparent<_fComp, _tTransKey>::value>>
	YB_ATTR_nodiscard YB_PURE inline iterator
	find_tr(const _tTransKey& k)
	{
		return ystdex::as_const(*this).find_tr(k).cast_mutable();
	}
	template<typename _tTransKey, typename
		= enable_if_t<has_mem_is_transparent<_fComp, _tTransKey>::value>>
	YB_ATTR_nodiscard YB_PURE const_iterator
	find_tr(const _tTransKey& k) const
	{
		const auto j(lower_bound_tr(k));

		return j != end() && objects.key_compare(k, select_key(j.p_node))
			? end() : j;
	}

	template<typename _tTransKey, typename
		= enable_if_t<has_mem_is_transparent<_fComp, _tTransKey>::value>>
	YB_ATTR_nodiscard YB_PURE inline size_type
	count_tr(const _tTransKey& k) const
	{
		const auto pr(equal_range_tr(k));

		return std::distance(pr.first, pr.second);
	}

	template<typename _tTransKey, typename
		= enable_if_t<has_mem_is_transparent<_fComp, _tTransKey>::value>>
	YB_ATTR_nodiscard YB_PURE inline iterator
	lower_bound_tr(const _tTransKey& k)
	{
		return ystdex::as_const(*this).lower_bound_tr(k).cast_mutable();
	}
	template<typename _tTransKey, typename
		= enable_if_t<has_mem_is_transparent<_fComp, _tTransKey>::value>>
	YB_ATTR_nodiscard YB_PURE const_iterator
	lower_bound_tr(const _tTransKey& k) const
	{
		auto x(node_begin());
		auto y(node_end());

		while(x)
			if(!objects.key_compare(select_key(x), k))
			{
				y = x;
				x = get_left(x);
			}
			else
				x = get_right(x);
		return const_iterator(y);
	}

	template<typename _tTransKey, typename
		= enable_if_t<has_mem_is_transparent<_fComp, _tTransKey>::value>>
	YB_ATTR_nodiscard YB_PURE inline iterator
	upper_bound_tr(const _tTransKey& k)
	{
		return ystdex::as_const(*this).upper_bound_tr(k).cast_mutable();
	}
	template<typename _tTransKey, typename
		= enable_if_t<has_mem_is_transparent<_fComp, _tTransKey>::value>>
	YB_ATTR_nodiscard YB_PURE const_iterator
	upper_bound_tr(const _tTransKey& k) const
	{
		auto x(node_begin());
		auto y(node_end());

		while(x)
			if(objects.key_compare(k, select_key(x)))
			{
				y = x;
				x = get_left(x);
			}
			else
				x = get_right(x);
		return const_iterator(y);
	}

	template<typename _tTransKey, typename
		= enable_if_t<has_mem_is_transparent<_fComp, _tTransKey>::value>>
	YB_ATTR_nodiscard YB_PURE inline std::pair<iterator, iterator>
	equal_range_tr(const _tTransKey& k)
	{
		const auto pr(ystdex::as_const(*this).equal_range_tr(k));

		return {pr.first.cast_mutable(), pr.second.cast_mutable()};
	}
	template<typename _tTransKey, typename
		= enable_if_t<has_mem_is_transparent<_fComp, _tTransKey>::value>>
	YB_ATTR_nodiscard YB_PURE std::pair<const_iterator, const_iterator>
	equal_range_tr(const _tTransKey& k) const
	{
		auto low(lower_bound_tr(k));
		auto high(low);
		auto& t_cmp(objects.key_compare);

		while(high != end() && !t_cmp(k, select_key(high.p_node)))
			++high;
		return {low, high};
	}
	//@}

	/*!
	\brief 验证红黑树满足约束。
	\return 验证通过。
	\note 通常用于调试。
	*/
	YB_ATTR_nodiscard YB_PURE bool
	rb_verify() const
	{
		if(objects.header.node_count == 0 || begin() == end())
			return objects.header.node_count == 0 && begin() == end()
				&& objects.header.left == node_end()
				&& objects.header.right == node_end();

		const auto len(tree_black_count(leftmost(), root()));

		for(const_iterator it(begin()); it != end(); ++it)
		{
			const auto x(const_link_type(it.p_node));
			const auto l(get_left(x));
			const auto r(get_right(x));

			if(x->color == tree_color::red)
				if((l && l->color == tree_color::red)
					|| (r && r->color == tree_color::red))
					return {};
			if(l && objects.key_compare(select_key(x), select_key(l)))
				return {};
			if(r && objects.key_compare(select_key(r), select_key(x)))
				return {};
			if(!l && !r && tree_black_count(x, root()) != len)
				return {};
		}
		if(leftmost() != tree_node_base::minimum(root()))
			return {};
		if(rightmost() != tree_node_base::maximum(root()))
			return {};
		return true;
	}

	template<typename _tIter>
	void
	assign_unique(_tIter first, _tIter last)
	{
		reuse_or_alloc_node roan(*this);

		objects.header.reset();
		for(; first != last; ++first)
			insert_hint_unique(end(), *first, roan);
	}

	template<typename _tIter>
	void
	assign_equal(_tIter first, _tIter last)
	{
		reuse_or_alloc_node roan(*this);

		objects.header.reset();
		for(; first != last; ++first)
			insert_hint_equal(end(), *first, roan);
	}

	insert_return_type
	reinsert_node_unique(node_type&& nh)
	{
		insert_return_type ret;

		if(nh.empty())
			ret.position = end();
		else
		{
			YAssert(get_node_allocator() == *nh.alloc,
				"Mismatched allocators found.");

			const auto pr(get_insert_unique_pos(nh.ckey()));

			if(pr.second)
			{
				ret.position = insert_node(pr.first, pr.second, nh.m_ptr);
				nh.m_ptr = nullptr;
				ret.inserted = true;
			}
			else
			{
				ret.node = std::move(nh);
				ret.position = iterator(pr.first);
				ret.inserted = {};
			}
		}
		return ret;
	}

	iterator
	reinsert_node_equal(node_type&& nh)
	{
		iterator ret;

		if(nh.empty())
			ret = end();
		else
		{
			YAssert(get_node_allocator() == *nh.alloc,
				"Mismatched allocators found.");
			
			const auto res(get_insert_equal_pos(nh.ckey()));

			ret = res.second ? insert_node(res.first, res.second, nh.m_ptr)
				: insert_equal_lower_node(nh.m_ptr);
			nh.m_ptr = {};
		}
		return ret;
	}

	iterator
	reinsert_node_hint_unique(const_iterator hint, node_type&& nh)
	{
		iterator ret;
		if(nh.empty())
			ret = end();
		else
		{
			YAssert(get_node_allocator() == *nh.alloc,
				"Mismatched allocators found.");

			const auto res(get_insert_hint_unique_pos(hint, nh.ckey()));

			if(res.second)
			{
				ret = insert_node(res.first, res.second, nh.m_ptr);
				nh.m_ptr = {};
			}
			else
				ret = iterator(res.first);
		}
		return ret;
	}

	iterator
	reinsert_node_hint_equal(const_iterator hint, node_type&& nh)
	{
		iterator ret;
		if(nh.empty())
			ret = end();
		else
		{
			YAssert(get_node_allocator() == *nh.alloc,
				"Mismatched allocators found.");

			const auto res(get_insert_hint_equal_pos(hint, nh.ckey()));

			ret = res.second ? insert_node(res.first, res.second, nh.m_ptr)
				: insert_equal_lower_node(nh.m_ptr);
			nh.m_ptr = {};
		}
		return ret;
	}

	YB_ATTR_nodiscard node_type
	extract(const_iterator position)
	{
		const auto ptr(tree_rebalance_for_erase(position.cast_mutable().p_node,
			objects.header));

		--objects.header.node_count;
		return {link_type(ptr), get_node_allocator()};
	}
	YB_ATTR_nodiscard node_type
	extract(const key_type& k)
	{
		node_type nh;
		const auto position(find(k));

		if(position != end())
			nh = extract(const_iterator(position));
		return nh;
	}

	//! \brief 从兼容容器合并唯一的键。
	template<typename _fComp2>
	void
	merge_unique(compatible_tree_type<_fComp2>& src) ynothrow
	{
		auto i(src.begin());
		const auto end(src.end());

		while(i != end)
		{
			auto position = i++;
			auto res = get_insert_unique_pos(_fKeyOfValue()(*position));
			if(res.second)
			{
				auto& src_objs(tree_merge_helper<tree, _fComp2>::get_components(
					src));
				const auto ptr(tree_rebalance_for_erase(position.p_node,
					src_objs.header));

				--src_objs.node_count;
				insert_node(res.first, res.second, link_type(ptr));
			}
		}
	}

	//! \brief 从兼容容器合并等价的键。
	template<typename _fComp2>
	void
	merge_equal(compatible_tree_type<_fComp2>& src) ynothrow
	{
		auto i(src.begin());
		const auto end(src.end());

		while(i != end)
		{
			const auto position(i++);
			const auto pr(get_insert_equal_pos(_fKeyOfValue()(*position)));

			if(pr.second)
			{
				auto& src_objs(tree_merge_helper<tree, _fComp2>::get_components(
					src));
				const auto ptr(
					tree_rebalance_for_erase(position.p_node, src_objs.header));

				--src_objs.node_count;
				insert_node(pr.first, pr.second, link_type(ptr));
			}
		}
	}

	friend bool
	operator==(const tree& x, const tree& y)
	{
		return x.size() == y.size()
			&& std::equal(x.cbegin(), x.cend(), y.cbegin());
	}

	friend bool
	operator<(const tree& x, const tree& y)
	{
		return std::lexicographical_compare(x.cbegin(), x.cend(), y.cbegin(),
			y.cend());
	}
};

//! \relates tree
template<typename _tKey, typename _type, typename _fKeyOfValue,
		 typename _fComp1, class _tAlloc, typename _fComp2>
struct tree_merge_helper<
	tree<_tKey, _type, _fKeyOfValue, _fComp1, _tAlloc>, _fComp2>
{
private:
	friend class tree<_tKey, _type, _fKeyOfValue, _fComp1, _tAlloc>;

	static auto
	get_components(tree<_tKey, _type, _fKeyOfValue, _fComp2, _tAlloc>& tree)
		-> decltype((tree.objects))
	{
		return tree.objects;
	}
};

} // inline namespace rb_tree;

} // namespace details;
//@}

} // namespace ystdex;

#endif

