/*
	© 2018-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file tree.h
\ingroup YStandardEx
\brief 作为关联容器实现的树。
\version r4926
\author FrankHB <frankhb1989@gmail.com>
\since build 830
\par 创建时间:
	2018-07-06 21:15:48 +0800
\par 修改时间:
	2023-02-13 20:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Tree

关联容器的内部实现的搜索树接口。
设计和 libstdc++ 的 <bits/stl_tree.h> 中非公开接口类似，以红黑树作为内部数据结构，
	可同时兼容 std::map 、std::set 、std::multimap 和 std::multiset 的容器。
接口支持 ISO C++17 模式下的容器功能，且支持不完整类型作为关联容器的键。
实现语言的特性要求和 YBase.YStandardEx 要求一致。
为便于参照和测试，实现也和 libstdc++ 在 ISO C++17 模式下的相似，
	但充分利用了其它 YBase.YStandardEx 特性。
因为用于内部实现，接口主要在用于实现的特定命名空间内。
除另行指定的兼容性策略外，作为实现的接口不保证对外稳定。
*/


#ifndef YB_INC_ystdex_tree_h_
#define YB_INC_ystdex_tree_h_ 1

#include "node_handle.hpp" // for "node_handle.hpp" (implying "range.hpp"),
//	size_t, typed_storage, std::allocator, bidirectional_iteratable,
//	std::bidirectional_iterator_tag, ptrdiff_t, totally_ordered,
//	is_unqualified_object, is_allocator_for, ystdex::reverse_iterator,
//	rebind_alloc_t, node_handle, node_insert_return, cond_t, is_same, bool_,
//	conditional_t, allocator_traits, and_, is_nothrow_default_constructible,
//	is_nothrow_copy_constructible, false_, true_, std::addressof,
//	ystdex::alloc_on_copy, is_nothrow_move_assignable, std::equal,
//	std::lexicographical_compare, yassume, std::forward,
//	is_throwing_move_copyable, ystdex::alloc_on_move,
//	is_trivially_default_constructible, allocator_guard, ystdex::to_allocated,
//	is_trivially_destructible, std::pointer_traits, is_invocable, std::pair,
//	std::declval, enable_if_t, yconstraint, is_nothrow_swappable,
//	ystdex::alloc_on_swap, YAssert, is_bitwise_swappable;
#include "base.h" // for noncopyable;
#include "compressed_pair.hpp" // for compressed_pair, value_init;
#include "functor.hpp" // for enable_if_transparent_t, ystdex::invoke;
#include <limits> // for std::numeric_limits;
#include "iterator_trait.hpp" // for has_iterator_value_type;

namespace ystdex
{

//! \since build 830
//!@{
namespace details
{

inline namespace rb_tree
{

enum class tree_color : bool
{
	red = false,
	black = true
};


//! \warning 非虚析构。
//!@{
struct tree_node_base
{
	using base_ptr = tree_node_base*;
	using const_base_ptr = const tree_node_base*;

	// XXX: Uninitialized.
	tree_color color;
	base_ptr parent, left, right;

	YB_ATTR_nodiscard YB_PURE static base_ptr
	maximum(base_ptr x) ynothrow
	{
		while(x->right)
			x = x->right;
		return x;
	}
	YB_ATTR_nodiscard YB_PURE static const_base_ptr
	maximum(const_base_ptr x) ynothrow
	{
		while(x->right)
			x = x->right;
		return x;
	}

	YB_ATTR_nodiscard YB_PURE static base_ptr
	minimum(base_ptr x) ynothrow
	{
		while(x->left)
			x = x->left;
		return x;
	}
	YB_ATTR_nodiscard YB_PURE static const_base_ptr
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
class tree_node : public tree_node_base, private typed_storage<_type>
{
public:
	using typed_storage<_type>::access;

	using typed_storage<_type>::access_ptr;
};
//!@}


/*!
\pre 断言：参数非空。
\since build 966
*/
//!@{
YB_API YB_NONNULL(1) YB_PURE tree_node_base*
tree_increment(tree_node_base*) ynothrowv;
YB_API YB_NONNULL(1) YB_PURE const tree_node_base*
tree_increment(const tree_node_base*) ynothrowv;

YB_API YB_NONNULL(1) YB_PURE tree_node_base*
tree_decrement(tree_node_base*) ynothrowv;
YB_API YB_NONNULL(1) YB_PURE const tree_node_base*
tree_decrement(const tree_node_base*) ynothrowv;
//!@}


//! \warning 非虚析构。
//!@{
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
	using difference_type = ptrdiff_t;
	using pointer = _type*;
	using reference = _type&;

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

	//! \since build 966
	//!@{
	YB_ATTR_nodiscard YB_PURE reference
	operator*() const ynothrowv
	{
		return link_type(p_node)->access();
	}

	tree_iterator&
	operator++() ynothrowv
	{
		p_node = tree_increment(p_node);
		return *this;
	}

	tree_iterator&
	operator--() ynothrowv
	{
		p_node = tree_decrement(p_node);
		return *this;
	}
	//!@}

	//! \since build 958
	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const tree_iterator& x, const tree_iterator& y) ynothrow
	{
		return x.p_node == y.p_node;
	}
};


template<typename _type>
class tree_const_iterator
	: public bidirectional_iteratable<tree_const_iterator<_type>, const _type&>
{
	// XXX: Partial specialization is not allowed here by ISO C++.
	template<typename, typename, class, typename, class>
	friend class tree;

public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = _type;
	using difference_type = ptrdiff_t;
	using pointer = const _type*;
	using reference = const _type&;

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

	//! \since build 966
	YB_ATTR_nodiscard YB_PURE reference
	operator*() const ynothrowv
	{
		return link_type(p_node)->access();
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

	//! \since build 958
	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const tree_const_iterator& x, const tree_const_iterator& y)
		ynothrow
	{
		return x.p_node == y.p_node;
	}
};
//!@}


/*!
\pre 断言：指针参数非空。
\since build 966
*/
//!@{
YB_API YB_NONNULL(2, 3) void
tree_insert_and_rebalance(bool, tree_node_base*, tree_node_base*,
	tree_node_base&) ynothrowv;

YB_ATTR_nodiscard YB_API YB_ATTR_returns_nonnull YB_NONNULL(1) tree_node_base*
tree_rebalance_for_erase(tree_node_base*, tree_node_base&) ynothrowv;
//!@}

YB_ATTR_nodiscard YB_API YB_PURE size_t
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
	template<typename, typename>
	friend struct tree_merge_helper;

public:
	using key_type = _tKey;
	using value_type = _type;
	/*!
	\see ISO C++17 [allocator.requirements] 。
	\see LWG 274 。
	\see LWG 2447 。
	*/
	static_assert(is_unqualified_object<value_type>(),
		"The value type for allocator shall be an unqualified object type.");
	/*!
	\see ISO C++17 [container.requirements.general]/15 。
	\see WG21 P1463R1 。
	*/
	static_assert(is_allocator_for<_tAlloc, value_type>(),
		"Value type mismatched to the allocator found.");
	/*!
	\brief 分配器类型。
	\note 支持 uses-allocator 构造。
	*/
	using allocator_type = _tAlloc;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	/*!
	\warning 不检查 const 安全性。若修改键破坏类不变量，则行为未定义。
	\note 对集合容器实现可调整 iterator 成员为 const_iterator 避免。
	\see LWG 103 。
	*/
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
	//! \note 集合容器使用常量迭代器以避免修改键破坏类不变量。
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
	//! \since build 966
	using node_pointer = typename node_ator_traits::pointer;
	//! \since build 865
	using equal_alloc_or_pocma = bool_<node_ator_traits::is_always_equal::value
		|| node_ator_traits::propagate_on_container_move_assignment::value>;
	// XXX: See $2022-01 @ %Documentation::Workflow.
	using allocator_reference = conditional_t<sizeof(node_allocator)
		<= sizeof(void*), node_allocator, node_allocator&>;
	struct alloc_node
	{
		tree& tree_ref;

		alloc_node(tree& t)
			: tree_ref(t)
		{}

		template<typename _tParam>
		YB_ATTR_nodiscard YB_ATTR_returns_nonnull inline link_type
		operator()(_tParam&& arg) const
		{
			return tree_ref.create_node(yforward(arg));
		}

		template<typename _tParam>
		YB_ATTR_nodiscard link_type
		reconstruct(base_ptr p, _tParam&& arg) const
		{
			const auto nd = link_type(p);

			tree_ref.destroy_node(nd);
			tree_ref.construct_node(std::pointer_traits<typename
				node_ator_traits::pointer>::pointer_to(*nd), yforward(arg));
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
		YB_ATTR_nodiscard YB_ATTR_returns_nonnull link_type
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
	struct components : compressed_pair<node_allocator, _tKeyComp>
	{
		//! \since build 940
		using base = compressed_pair<node_allocator, _tKeyComp>;

		tree_header header{};

		//! \since build 966
		components()
			ynoexcept(and_<is_nothrow_default_constructible<node_allocator>,
			is_nothrow_default_constructible<_tKeyComp>>())
		{}
		//! \since build 867
		explicit
		components(node_allocator a) ynothrow
			: base(std::move(a), value_init)
		{}
		components(const _tKeyComp& comp, node_allocator&& a)
			: base(std::move(a), _tKeyComp(comp))
		{}
		components(const components& x)
			: base(node_allocator(
			node_ator_traits::select_on_container_copy_construction(x.first())),
			x.get_key_comp())
		{}
		// NOTE: As per ISO C++17 [associative.reqmts], the comparison object
		//	shall be copy constructible. However, whether it should be copy is
		//	questionable, see LWG 2227. At current, follow the common practice
		//	in libstdc++ and Microsoft VC++'s STL by copying, but not moving in
		//	libc++.
		// XXX: The comparion object is always copied even during moving.
		/*!
		\see LWG 2227 。
		\since build 958
		*/
		components(components&& x)
			ynoexcept(is_nothrow_copy_constructible<_tKeyComp>())
			: base(std::move(x.first()), x.second()),
			header(std::move(x.header))
		{}
		//! \since build 867
		components(components&& x, node_allocator a)
			: base(std::move(a), std::move(x)),
			header(std::move(x.header))
		{}

		//! \since build 865
		void
		move_data(components& from) ynothrow
		{
			header.move_data(from.header);
		}

		YB_ATTR_nodiscard YB_PURE _tKeyComp&
		get_key_comp() ynothrow
		{
			return base::second();
		}
		YB_ATTR_nodiscard YB_PURE const _tKeyComp&
		get_key_comp() const ynothrow
		{
			return base::second();
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
		: objects(x.objects.get_key_comp(), node_allocator(a))
	{
		if(x.root())
			root() = copy_node(x);
	}
	tree(tree&&) = default;
	tree(tree&& x, const allocator_type& a)
		: tree(std::move(x), node_allocator(a))
	{}
	//! \since build 866
	//!@{
	tree(tree&& x, node_allocator&& a)
		// XXX: %is_nothrow_constructible is not usable for the incomplete type.
		ynoexcept_spec(tree(std::move(x), std::move(a),
		typename node_ator_traits::is_always_equal()))
		: tree(std::move(x), std::move(a),
		typename node_ator_traits::is_always_equal())
	{}

private:
	//! \since build 867
	tree(tree&& x, node_allocator a, false_)
		: objects(x.objects.get_key_comp(), std::move(a))
	{
		if(x.root())
			move_data(x, false_());
	}
	//! \since build 867
	tree(tree&& x, node_allocator a, true_)
		ynoexcept(is_nothrow_default_constructible<_fComp>())
		: objects(std::move(x.objects), std::move(a))
	{}
	//!@}

public:
	~tree() ynothrow
	{
		clear_nodes();
	}

	tree&
	operator=(const tree& x)
	{
		if(std::addressof(x) != this)
		{
			if(node_ator_traits::propagate_on_container_copy_assignment::value)
			{
				auto& this_alloc(get_node_allocator());
				const auto& that_alloc(x.get_node_allocator());

				if(!node_ator_traits::is_always_equal::value
					&& this_alloc != that_alloc)
					clear();
				ystdex::alloc_on_copy(this_alloc, that_alloc);
			}

			reuse_or_alloc_node roan(*this);

			objects.header.reset();
			objects.get_key_comp() = x.objects.get_key_comp();
			if(x.root())
				root() = copy_node<false>(x, roan);
		}
		return *this;
	}
	/*!
	\see LWG 2839 。
	\since build 864
	*/
	tree&
	operator=(tree&& x) ynoexcept(
		and_<equal_alloc_or_pocma, is_nothrow_move_assignable<_fComp>>())
	{
		// NOTE: As ISO C++ [res.on.arguments], moving same object to itself is
		//	not supported to meet the postcondition of the move assignment until
		//	the resolution of LWG 2839, which makes it valid but unspecified
		//	after the move. See %move_assign_elements for details.
		// XXX: Moving from an ancestor to its subtree (composed by elements of
		//	recursive container type) is not supported and it would typically
		//	lead to resource leaks, however there is no check anyway. This is
		//	different to ISO C++ as associative containers do not support
		//	incomplete element types so the recursive container type itself
		//	causes undefined behavior in such cases. Use %swap if the operand
		//	can reference to the object itself or its subtrees thereby.
		objects.get_key_comp() = std::move(x.objects.get_key_comp());
		move_assign_elements(x, equal_alloc_or_pocma());
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const tree& x, const tree& y)
	{
		return x.size() == y.size()
			&& std::equal(x.cbegin(), x.cend(), y.cbegin());
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator<(const tree& x, const tree& y)
	{
		return std::lexicographical_compare(x.cbegin(), x.cend(), y.cbegin(),
			y.cend());
	}

	template<typename _tIter>
	void
	assign_unique(_tIter first, _tIter last)
	{
		reuse_or_alloc_node roan(*this);

		objects.header.reset();
		insert_range_unique(first, last, roan);
	}

	template<typename _tIter>
	void
	assign_equal(_tIter first, _tIter last)
	{
		reuse_or_alloc_node roan(*this);

		objects.header.reset();
		insert_range_equal(first, last, roan);
	}

private:
	//! \since build 958
	template<bool _bMove, class _tNodeGen>
	YB_ATTR_nodiscard link_type
	copy_node(link_type x, base_ptr p, const _tNodeGen& node_gen)
	{
		yassume(x), yassume(p);
		const auto clone_node([&]{
			link_type nd(node_gen(std::forward<conditional_t<_bMove,
				value_type&&, const value_type&>>(x->access())));

			yunseq(nd->color = x->color, nd->left = {}, nd->right = {});
			return nd;
		});
		const auto top(clone_node());

		top->parent = p;
		try
		{
			if(x->right)
				top->right = copy_node<_bMove>(get_right(x), top, node_gen);
			p = top;
			x = get_left(x);

			while(x)
			{
				const auto y(clone_node());

				p->left = y;
				y->parent = p;
				if(x->right)
					y->right = copy_node<_bMove>(get_right(x), y, node_gen);
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
	template<bool _bMove, class _tNodeGen>
	YB_ATTR_nodiscard link_type
	copy_node(const tree& x, const _tNodeGen& node_gen)
	{
		const auto root(copy_node<_bMove>(x.node_mbegin(), node_end(),
			node_gen));

		yunseq(leftmost() = minimum(root), rightmost() = maximum(root),
			objects.header.node_count = x.objects.header.node_count);
		return root;
	}
	YB_ATTR_nodiscard link_type
	copy_node(const tree& x)
	{
		alloc_node an(*this);

		return copy_node<false>(x, an);
	}

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
			// XXX: This requires %value_type complete.
			yconstexpr const bool
				move(!is_throwing_move_copyable<value_type>());
			alloc_node an(*this);

			root() = copy_node<move>(x, an);
			yconstexpr_if(move)
				x.clear();
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
	\note 从不相等分配器转移元素可能复制而不是转移对象。
	*/
	void
	move_assign_elements(tree& x, false_)
	{
		// NOTE: This just clears the elements for self-move. See also the
		//	comment in move %operator=.
		if(get_node_allocator() == x.get_node_allocator())
			move_assign_elements(x, true_());
		else
		{
			// NOTE: Try to move reusing existing nodes.
			reuse_or_alloc_node roan(*this);

			objects.header.reset();
			if(x.root())
			{
				root() = copy_node<true>(x, roan);
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
		// NOTE: Ditto.
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
		return objects.first();
	}
	YB_ATTR_nodiscard YB_PURE const node_allocator&
	get_node_allocator() const ynothrow
	{
		return objects.first();
	}

protected:
	//! \since build 865
	// XXX: This implies %YB_ALLOCATOR and %YB_returns_nonnull for non-fancy
	//	pointers.
	YB_ATTR_nodiscard node_pointer
	allocate_node()
	{
		allocator_reference a(get_node_allocator());

		return node_ator_traits::allocate(a, 1);
	}

	//! \since build 865
	void
	deallocate_node(node_pointer p) ynothrow
	{
		allocator_reference a(get_node_allocator());

		node_ator_traits::deallocate(a, p, 1);
	}

	//! \pre 断言：第一参数非空。
	template<typename... _tParams>
	void
	construct_node(node_pointer p, _tParams&&... args)
	{
		YAssertNonnull(p);

		// NOTE: Unintialized data members are delayed to be overwritten by
		//	%tree_insert_and_rebalance after creation.
		// XXX: Ensure no need to use placment new for the node, so there needs
		//	no try-catch block on allocation failure if WG21 P0593R6 is
		//	supported. This is a special case of the trait inWG21 P26740.
		static_assert(is_trivially_default_constructible<value_node>(),
			"Invalid node type found.");
		allocator_reference a(get_node_allocator());
		// NOTE: This should be same to %deallocate_node(p) on exception thrown.
		allocator_guard<allocator_reference> gd(p, a);
		const auto nd(ystdex::to_allocated(p));

#if __cplusplus < 202002L
		try
		{
			// XXX: Data members are not initialized here. See above.
			::new(nd) value_node;
			node_ator_traits::construct(a, nd->access_ptr(), yforward(args)...);
		}
		catch(...)
		{
			nd->~value_node();
		}
#else
		node_ator_traits::construct(a, nd->access_ptr(), yforward(args)...);
#endif
		gd.release();
	}

	template<typename... _tParams>
	YB_ATTR_nodiscard YB_ATTR_returns_nonnull inline link_type
	create_node(_tParams&&... args)
	{
		const auto p(allocate_node());

		construct_node(p, yforward(args)...);
		// XXX: Metadata in %p is dropped.
		return ystdex::to_allocated(p);
	}

	void
	destroy_node(link_type nd) ynothrow
	{
		// XXX: Ensure no need to use explicit destructor call for the node if
		//	WG21 P0593R6 is supported, although it is not acually used (see
		//	below). This is a special case of the trait inWG21 P26740.
		static_assert(is_trivially_destructible<value_node>(),
			"Invalid node type found.");
		allocator_reference a(get_node_allocator());

		node_ator_traits::destroy(a, nd->access_ptr());
#if __cplusplus < 202002L
		// XXX: This is actually compatible to ISO C++20 because there are not
		//	data member accesses to the object pointed by %nd. It can be even a
		//	potiential hint to optimizing implementations. Nevertheless, leave
		//	it away right now to be more consistent with %construct_node.
		nd->~value_node();
#endif
	}

	void
	drop_node(link_type nd) ynothrow
	{
		destroy_node(nd);
		deallocate_node(std::pointer_traits<typename
			node_ator_traits::pointer>::pointer_to(*nd));
	}

private:
	//! \pre 间接断言：指针参数非空。
	//!@{
	//! \since build 866
	YB_NONNULL(3) void
	attach_node(bool insert_left, base_ptr p, link_type nd)
	{
		tree_insert_and_rebalance(insert_left, ystdex::to_address(nd), p,
			objects.header);
		++objects.header.node_count;
	}

	//! \since build 866
	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(2) link_type
	detach_node(base_ptr p)
	{
		--objects.header.node_count;
		return link_type(tree_rebalance_for_erase(p, objects.header));
	}
	//!@}

public:
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
		// XXX: See $2022-01 @ %Documentation::Workflow.
		return std::numeric_limits<size_type>::max();
	}

protected:
	YB_ATTR_nodiscard YB_PURE base_ptr&
	root() ynothrow
	{
		return objects.header.parent;
	}
	YB_ATTR_nodiscard YB_PURE const_base_ptr
	root() const ynothrow
	{
		return objects.header.parent;
	}

	YB_ATTR_nodiscard YB_PURE base_ptr&
	leftmost() ynothrow
	{
		return objects.header.left;
	}
	YB_ATTR_nodiscard YB_PURE const_base_ptr
	leftmost() const ynothrow
	{
		return objects.header.left;
	}

	YB_ATTR_nodiscard YB_PURE base_ptr&
	rightmost() ynothrow
	{
		return objects.header.right;
	}
	YB_ATTR_nodiscard YB_PURE const_base_ptr
	rightmost() const ynothrow
	{
		return objects.header.right;
	}

	YB_ATTR_nodiscard YB_PURE link_type
	node_begin() ynothrow
	{
		return link_type(objects.header.parent);
	}
	YB_ATTR_nodiscard YB_PURE const_link_type
	node_begin() const ynothrow
	{
		return node_mbegin();
	}

	//! \since build 958
	YB_ATTR_nodiscard YB_PURE link_type
	node_mbegin() const ynothrow
	{
		return link_type(objects.header.parent);
	}

	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE base_ptr
	node_end() ynothrow
	{
		return &objects.header;
	}
	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE const_base_ptr
	node_end() const ynothrow
	{
		return &objects.header;
	}

	YB_ATTR_nodiscard YB_PURE static const _tKey&
	select_key(const_base_ptr x)
	{
		return select_key(const_link_type(x));
	}
	YB_ATTR_nodiscard YB_PURE static const _tKey&
	select_key(const_link_type x)
	{
		// NOTE: This function is usually called to ask the comparison object,
		//	so it is likely appropriate to have the checks here.
		static_assert(is_invocable<_fComp&, const _tKey&, const _tKey&>(),
			"Invalid comparison object type found.");
		// XXX: LWG 2542.
		static_assert(is_invocable<const _fComp&, const _tKey&, const _tKey&>(),
			"Invalid comparison object type found.");

		return _fKeyOfValue()(select_value(x));
	}

	/*!
	\pre 断言：参数非空。
	\since build 966
	*/
	//!@{
	YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE static const_reference
	select_value(const_base_ptr x) ynothrowv
	{
		YAssertNonnull(x);
		return const_link_type(x)->access();
	}
	YB_ATTR_nodiscard YB_PURE static const_reference
	select_value(const_link_type x) ynothrowv
	{
		YAssertNonnull(x);
		return x->access();
	}
	//!@}

	YB_ATTR_nodiscard YB_PURE static link_type
	get_left(base_ptr x) ynothrow
	{
		return link_type(x->left);
	}
	YB_ATTR_nodiscard YB_PURE static const_link_type
	get_left(const_base_ptr x) ynothrow
	{
		return const_link_type(x->left);
	}

	YB_ATTR_nodiscard YB_PURE static link_type
	get_right(base_ptr x) ynothrow
	{
		return link_type(x->right);
	}
	YB_ATTR_nodiscard YB_PURE static const_link_type
	get_right(const_base_ptr x) ynothrow
	{
		return const_link_type(x->right);
	}

	YB_ATTR_nodiscard YB_PURE static base_ptr
	maximum(base_ptr x) ynothrow
	{
		return tree_node_base::maximum(x);
	}
	YB_ATTR_nodiscard YB_PURE static const_base_ptr
	maximum(const_base_ptr x) ynothrow
	{
		return tree_node_base::maximum(x);
	}

	YB_ATTR_nodiscard YB_PURE static base_ptr
	minimum(base_ptr x) ynothrow
	{
		return tree_node_base::minimum(x);
	}
	YB_ATTR_nodiscard YB_PURE static const_base_ptr
	minimum(const_base_ptr x) ynothrow
	{
		return tree_node_base::minimum(x);
	}

private:
	//! \since build 840
	template<typename _tParam>
	YB_ATTR_nodiscard inline bool
	is_insert_left(base_ptr p, const _tParam& arg)
	{
		return p == node_end() || !objects.get_key_comp()(select_key(p), arg);
	}

	//! \since build 840
	template<typename _tParam>
	YB_ATTR_nodiscard inline bool
	is_insert_left_strict(base_ptr p, const _tParam& arg)
	{
		return p == node_end() || objects.get_key_comp()(arg, select_key(p));
	}

	/*
	\pre 间接断言：指针参数非空。
	\since build 866
	*/
	YB_NONNULL(3) iterator
	insert_attach(bool insert_left, base_ptr p, link_type nd)
	{
		attach_node(insert_left, p, nd);
		return iterator(nd);
	}

	//! \since build 866
	YB_ATTR_nodiscard bool
	insert_left_q(const std::pair<base_ptr, base_ptr>& pr, const key_type& k)
	{
		return bool(pr.first) || is_insert_left_strict(pr.second, k);
	}

	//! \pre 间接断言：指针参数非空。
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
			x = !objects.get_key_comp()(select_key(x), _fKeyOfValue()(v))
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
			x = !objects.get_key_comp()(select_key(x), select_key(nd))
				? get_left(x) : get_right(x);
		}
		return insert_attach(is_insert_left(y, select_key(nd)), y, nd);
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

public:
	YB_ATTR_nodiscard YB_PURE std::pair<base_ptr, base_ptr>
	get_insert_unique_pos(const key_type& k)
	{
		auto x(node_begin());
		auto y(node_end());
		bool r(true);

		while(x)
		{
			y = x;
			r = objects.get_key_comp()(k, select_key(x));
			x = r ? get_left(x) : get_right(x);
		}

		iterator i(y);

		if(r)
		{
			if(i == begin())
				return {x, y};
			--i;
		}
		if(objects.get_key_comp()(select_key(i.p_node), k))
			return {x, y};
		return {i.p_node, {}};
	}

	YB_ATTR_nodiscard YB_PURE std::pair<base_ptr, base_ptr>
	get_insert_equal_pos(const key_type& k)
	{
		auto x(node_begin());
		auto y(node_end());

		while(x)
		{
			y = x;
			x = objects.get_key_comp()(k, select_key(x)) ? get_left(x)
				: get_right(x);
		}
		return {x, y};
	}

	YB_ATTR_nodiscard YB_PURE std::pair<base_ptr, base_ptr>
	get_insert_hint_unique_pos(const_iterator position, const key_type& k)
	{
		const auto i(position.cast_mutable());

		if(i.p_node == node_end())
		{
			if(size() > 0 && objects.get_key_comp()(select_key(rightmost()), k))
				return {{}, rightmost()};
			return get_insert_unique_pos(k);
		}
		else if(objects.get_key_comp()(k, select_key(i.p_node)))
		{
			if(i.p_node == leftmost())
				return {leftmost(), leftmost()};

			auto before(i);

			if(objects.get_key_comp()(select_key((--before).p_node), k))
			{
				if(get_right(before.p_node))
					return {i.p_node, i.p_node};
				return {{}, before.p_node};
			}
			return get_insert_unique_pos(k);
		}
		else if(objects.get_key_comp()(select_key(i.p_node), k))
		{
			if(i.p_node == rightmost())
				return {{}, rightmost()};

			auto after(i);

			if(objects.get_key_comp()(k, select_key((++after).p_node)))
			{
				if(get_right(i.p_node))
					return {after.p_node, after.p_node};
				return {{}, i.p_node};
			}
			return get_insert_unique_pos(k);
		}
		return {i.p_node, {}};
	}

	YB_ATTR_nodiscard YB_PURE std::pair<base_ptr, base_ptr>
	get_insert_hint_equal_pos(const_iterator position, const key_type& k)
	{
		const auto i(position.cast_mutable());

		if(i.p_node == node_end())
		{
			if(size() > 0
				&& !objects.get_key_comp()(k, select_key(rightmost())))
				return {{}, rightmost()};
			return get_insert_equal_pos(k);
		}
		else if(!objects.get_key_comp()(select_key(i.p_node), k))
		{
			if(i.p_node == leftmost())
				return {leftmost(), leftmost()};

			auto before(i);

			if(!objects.get_key_comp()(k, select_key((--before).p_node)))
			{
				if(get_right(before.p_node))
					return {i.p_node, i.p_node};
				return {0, before.p_node};
			}
			return get_insert_equal_pos(k);
		}
		else
		{
			if(i.p_node == rightmost())
				return {{}, rightmost()};

			auto after(i);

			if(!objects.get_key_comp()(select_key((++after).p_node), k))
			{
				if(get_right(i.p_node))
					return {after.p_node, after.p_node};
				return {{}, i.p_node};
			}
			return {{}, {}};
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
		const auto& k(_fKeyOfValue()(v));
		const auto pr(get_insert_unique_pos(k));

		if(pr.second)
		{
			// NOTE: This is checked first to avoid stray node allocated when
			//	the key comparation throws.
			const bool insert_left(insert_left_q(pr, k));

			// XXX: Assume the key in the constructed node is equivalent to %k.
			return {insert_attach(insert_left, pr.second,
				link_type(node_gen(yforward(v)))), true};
		}
		return {iterator(pr.first), {}};
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
		const auto pr(get_insert_equal_pos(k));
		// NOTE: This is checked first to avoid stray node allocated when the
		//	key comparation throws.
		const bool insert_left(insert_left_q(pr, k));

		// XXX: Assume the key in the constructed node is equivalent to %k.
		return insert_attach(insert_left, pr.second,
			link_type(node_gen(yforward(v))));
	}

	template<typename _tParam>
	iterator
	insert_hint_unique(const_iterator position, _tParam&& v)
	{
		alloc_node an(*this);

		return insert_hint_unique(position, yforward(v), an);
	}
	template<typename _tParam, class _tNodeGen>
	iterator
	insert_hint_unique(const_iterator position, _tParam&& v,
		const _tNodeGen& node_gen)
	{
		const auto& k(_fKeyOfValue()(v));
		const auto pr(get_insert_hint_unique_pos(position, k));

		if(pr.second)
		{
			// NOTE: This is checked first to avoid stray node allocated when
			//	the key comparation throws.
			const bool insert_left(insert_left_q(pr, k));

			// XXX: Assume the key in the constructed node is equivalent to %k.
			return insert_attach(insert_left, pr.second,
				link_type(node_gen(yforward(v))));
		}
		return iterator(pr.first);
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
		const auto pr(get_insert_hint_equal_pos(position, k));

		if(pr.second)
		{
			// NOTE: This is checked first to avoid stray node allocated when
			//	the key comparation throws.
			const bool insert_left(insert_left_q(pr, k));

			// XXX: Assume the key in the constructed node is equivalent to %k.
			return insert_attach(insert_left, pr.second,
				link_type(node_gen(yforward(v))));
		}
		return insert_equal_lower(yforward(v));
	}

	template<typename... _tParams>
	inline std::pair<iterator, bool>
	emplace_unique(_tParams&&... args)
	{
		return insert_or_drop_node(
			[&](const key_type& k, link_type nd) -> std::pair<iterator, bool>{
			const auto pr(get_insert_unique_pos(k));

			if(pr.second)
				return {insert_node(pr.first, pr.second, nd), true};
			drop_node(nd);
			return {iterator(pr.first), {}};
		}, yforward(args)...);
	}

	template<typename... _tParams>
	inline iterator
	emplace_equal(_tParams&&... args)
	{
		return insert_or_drop_node([&](const key_type& k, link_type nd){
			const auto pr(get_insert_equal_pos(k));

			return insert_node(pr.first, pr.second, nd);
		}, yforward(args)...);
	}

	template<typename... _tParams>
	inline iterator
	emplace_hint_unique(const_iterator position, _tParams&&... args)
	{
		return insert_or_drop_node([&](const key_type& k, link_type nd){
			const auto pr(get_insert_hint_unique_pos(position, k));

			if(pr.second)
				return insert_node(pr.first, pr.second, nd);
			drop_node(nd);
			return iterator(pr.first);
		}, yforward(args)...);
	}

	template<typename... _tParams>
	inline iterator
	emplace_hint_equal(const_iterator position, _tParams&&... args)
	{
		return insert_or_drop_node([&](const key_type& k, link_type nd){
			const auto pr(get_insert_hint_equal_pos(position, k));

			return pr.second ? insert_node(pr.first, pr.second, nd)
				: insert_equal_lower_node(nd);
		}, yforward(args)...);
	}

private:
	// XXX: The return type can be incomplete before this template is
	//	instantiated.
	//! \since build 966
	template<typename _func, typename... _tParams>
	inline auto
	insert_or_drop_node(_func f, _tParams&&... args) -> decltype(std::declval<
		_func&>()(std::declval<const key_type&>(), std::declval<link_type>()))
	{
		const auto nd(create_node(yforward(args)...));

		try
		{
			return f(select_key(nd), nd);
		}
		catch(...)
		{
			drop_node(nd);
			throw;
		}
	}

public:
	//! \since build 866
	//!@{
	template<typename _tIn>
	enable_if_t<has_iterator_value_type<value_type, _tIn>{}>
	insert_range_unique(_tIn first, _tIn last)
	{
		alloc_node an(*this);

		insert_range_unique(first, last, an);
	}
	template<typename _tIn>
	enable_if_t<!has_iterator_value_type<value_type, _tIn>{}>
	insert_range_unique(_tIn first, _tIn last)
	{
		for(; first != last; ++first)
			emplace_hint_unique(end(), *first);
	}
	//! \since build 966
	template<typename _tIn, class _tNodeGen>
	void
	insert_range_unique(_tIn first, _tIn last, _tNodeGen& node_gen)
	{
		for(; first != last; ++first)
			insert_hint_unique(end(), *first, node_gen);
	}

	template<typename _tIn>
	enable_if_t<has_iterator_value_type<value_type, _tIn>{}>
	insert_range_equal(_tIn first, _tIn last)
	{
		alloc_node an(*this);

		insert_range_equal(first, last, an);
	}
	template<typename _tIn>
	enable_if_t<!has_iterator_value_type<value_type, _tIn>{}>
	insert_range_equal(_tIn first, _tIn last)
	{
		for(; first != last; ++first)
			emplace_hint_equal(end(), *first);
	}
	//! \since build 966
	template<typename _tIn, class _tNodeGen>
	void
	insert_range_equal(_tIn first, _tIn last, _tNodeGen& node_gen)
	{
		for(; first != last; ++first)
			insert_hint_equal(end(), *first, node_gen);
	}
	//!@}

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
	iterator
	erase(const_iterator position)
	{
		yconstraint(position != end());

		auto i(position.cast_mutable());

		++i;
		erase_it(position.cast_mutable());
		return i;
	}
	iterator
	erase(iterator position)
	{
		yconstraint(position != end());

		auto i(position);

		++i;
		erase_it(position);
		return i;
	}
	size_type
	erase(const key_type& x)
	{
		const auto pr(equal_range(x));
		const auto old_size(size());

		erase_it(pr.first, pr.second);
		return old_size - size();
	}
	iterator
	erase(const_iterator first, const_iterator last)
	{
		const auto i(last.cast_mutable());

		erase_it(first.cast_mutable(), i);
		return i;
	}
	void
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
			std::swap(x.root(), y.root()),
			std::swap(x.leftmost(), y.leftmost()),
			std::swap(x.rightmost(), y.rightmost());
			yunseq(x.root()->parent = x.node_end(),
				y.root()->parent = y.node_end());
			std::swap(x.objects.header.node_count, y.objects.header.node_count);
		}
		// NOTE: As libstdc++'s implementation, the header's color does not
		//	change.
		std::swap(x.objects.get_key_comp(), y.objects.get_key_comp());
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
	/*!
	\pre 间接断言：分配器和参数指定的容器分配器相等。
	\since build 966
	*/
	//!@{
	//! \brief 从兼容容器合并唯一的键。
	template<typename _fComp2>
	void
	merge_unique(compatible_tree_type<_fComp2>& src)
	{
		merge_impl(src, &tree::get_insert_unique_pos);
	}

	//! \brief 从兼容容器合并等价的键。
	template<typename _fComp2>
	void
	merge_equal(compatible_tree_type<_fComp2>& src)
	{
		merge_impl(src, &tree::get_insert_equal_pos);
	}

private:
	template<typename _fCallable, typename _fComp2>
	void
	merge_impl(compatible_tree_type<_fComp2>& src, _fCallable f)
	{
		ystdex::expects_equal_allocators(*this, src);

		auto i(src.begin());
		const auto last(src.end());

		while(i != last)
		{
			const auto j(i++);
			const auto pr(ystdex::invoke(f, *this, _fKeyOfValue()(*j)));

			if(pr.second)
			{
				auto& src_header(tree_merge_helper<tree,
					_fComp2>::get_components(src).header);
				const auto p(tree_rebalance_for_erase(j.p_node, src_header));

				--src_header.node_count;
				insert_node(pr.first, pr.second, link_type(p));
			}
		}
	}
	//!@}

public:
	YB_ATTR_nodiscard _fComp
	key_comp() const
	{
		return objects.get_key_comp();
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

	// XXX: For containers having unique keys, using %find should be more
	//	efficient.
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
		return equal_range_impl<const_iterator>(*this, k);
	}

#define YB_Impl_Tree_GenericLookupHead(_n, _r) \
	template<typename _tTransKey, \
		yimpl(typename = enable_if_transparent_t<_fComp, _tTransKey>)> \
	YB_ATTR_nodiscard YB_PURE inline _r \
	_n(const _tTransKey& k)

	//! \since build 844
	//!@{
	YB_Impl_Tree_GenericLookupHead(find_tr, iterator)
	{
		return find_impl<iterator>(*this, k);
	}
	YB_Impl_Tree_GenericLookupHead(find_tr, const_iterator) const
	{
		return find_impl<const_iterator>(*this, k);
	}

	YB_Impl_Tree_GenericLookupHead(count_tr, size_type) const
	{
		// XXX: This cannot just use %find_impl instead even for containers
		//	having unique key. See https://github.com/cplusplus/papers/issues/1037#issuecomment-960066305.
		const auto pr(equal_range_tr(k));

		return size_type(std::distance(pr.first, pr.second));
	}

	YB_Impl_Tree_GenericLookupHead(lower_bound_tr, iterator)
	{
		return lower_bound_impl<iterator>(*this, k);
	}
	YB_Impl_Tree_GenericLookupHead(lower_bound_tr, const_iterator) const
	{
		return lower_bound_impl<const_iterator>(*this, k);
	}

	YB_Impl_Tree_GenericLookupHead(upper_bound_tr, iterator)
	{
		return upper_bound_impl<iterator>(*this, k);
	}
	YB_Impl_Tree_GenericLookupHead(upper_bound_tr, const_iterator) const
	{
		return upper_bound_impl<const_iterator>(*this, k);
	}

	YB_Impl_Tree_GenericLookupHead(equal_range_tr,
		std::pair<iterator YPP_Comma iterator>)
	{
		return equal_range_impl<iterator>(*this, k);
	}
	YB_Impl_Tree_GenericLookupHead(equal_range_tr,
		std::pair<const_iterator YPP_Comma const_iterator>) const
	{
		return equal_range_impl<const_iterator>(*this, k);
	}
	//!@}

#undef YB_Impl_Tree_GenericLookupHead

private:
	template<typename _tIter, class _tTree, typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static _tIter
	find_impl(_tTree& t, const _tKeyOrTransKey& k)
	{
		const auto
			i(lower_bound_impl<_tIter>(t, t.node_begin(), t.node_end(), k));

		return (i == t.end()
			|| t.objects.get_key_comp()(k, select_key(i.p_node))) ? t.end() : i;
	}

	template<typename _tIter, class _tTree, typename _tLink, typename _tBasePtr,
		typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static _tIter
	lower_bound_impl(_tTree& t, _tLink x, _tBasePtr y, const _tKeyOrTransKey& k)
	{
		while(x)
			if(!t.objects.get_key_comp()(select_key(x), k))
			{
				y = x;
				x = get_left(x);
			}
			else
				x = get_right(x);
		return _tIter(y);
	}
	template<typename _tIter, class _tTree, typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static inline _tIter
	lower_bound_impl(_tTree& t, const _tKeyOrTransKey& k)
	{
		return lower_bound_impl<_tIter>(t, t.node_begin(), t.node_end(), k);
	}

	template<typename _tIter, class _tTree, typename _tLink, typename _tBasePtr,
		typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static _tIter
	upper_bound_impl(_tTree& t, _tLink x, _tBasePtr y, const _tKeyOrTransKey& k)
	{
		while(x)
			if(t.objects.get_key_comp()(k, select_key(x)))
			{
				y = x;
				x = get_left(x);
			}
			else
				x = get_right(x);
		return _tIter(y);
	}
	template<typename _tIter, class _tTree, typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static inline _tIter
	upper_bound_impl(_tTree& t, const _tKeyOrTransKey& k)
	{
		return upper_bound_impl<_tIter>(t, t.node_begin(), t.node_end(), k);
	}

	//! \since build 840
	template<typename _tIter, class _tTree, typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE std::pair<_tIter, _tIter>
	equal_range_impl(_tTree& t, const _tKeyOrTransKey& k)
	{
		auto x(t.node_begin());
		auto y(t.node_end());

		while(x)
		{
			const auto& t_comp(t.objects.get_key_comp());

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
				return {lower_bound_impl<_tIter>(t, x, y, k),
					upper_bound_impl<_tIter>(t, xu, yu, k)};
			}
		}
		return {_tIter(y), _tIter(y)};
	}

public:
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

		for(const_iterator i(begin()); i != end(); ++i)
		{
			const auto x(const_link_type(i.p_node));
			const auto l(get_left(x));
			const auto r(get_right(x));

			if(x->color == tree_color::red)
				if((l && l->color == tree_color::red)
					|| (r && r->color == tree_color::red))
					return {};
			if(l && objects.get_key_comp()(select_key(x), select_key(l)))
				return {};
			if(r && objects.get_key_comp()(select_key(r), select_key(x)))
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

	//! \pre 间接断言：若参数非空，分配器和参数指定的容器分配器相等。
	//!@{
	insert_return_type
	reinsert_node_unique(node_type&& nh)
	{
		insert_return_type ret;

		if(nh.empty())
			ret.position = end();
		else
		{
			ystdex::expects_equal_allocators(*this, nh);

			const auto pr(get_insert_unique_pos(nh.ckey()));

			if(pr.second)
			{
				ret.position = insert_node(pr.first, pr.second, nh.ptr);
				nh.ptr = {};
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
			ystdex::expects_equal_allocators(*this, nh);

			const auto pr(get_insert_equal_pos(nh.ckey()));

			ret = pr.second ? insert_node(pr.first, pr.second, nh.ptr)
				: insert_equal_lower_node(nh.ptr);
			nh.ptr = {};
		}
		return ret;
	}

	iterator
	reinsert_node_hint_unique(const_iterator hint, node_type&& nh)
	{
		if(nh.empty())
			return end();

		ystdex::expects_equal_allocators(*this, nh);

		const auto pr(get_insert_hint_unique_pos(hint, nh.ckey()));

		if(pr.second)
		{
			auto ret(insert_node(pr.first, pr.second, nh.ptr));

			nh.ptr = {};
			return ret;
		}
		return iterator(pr.first);
	}

	iterator
	reinsert_node_hint_equal(const_iterator hint, node_type&& nh)
	{
		if(nh.empty())
			return end();

		ystdex::expects_equal_allocators(*this, nh);

		const auto pr(get_insert_hint_equal_pos(hint, nh.ckey()));
		auto ret(pr.second ? insert_node(pr.first, pr.second, nh.ptr)
			: insert_equal_lower_node(nh.ptr));

		nh.ptr = {};
		return ret;
	}
	//!@}

	node_type
	extract(const_iterator position)
	{
		const auto p(tree_rebalance_for_erase(position.cast_mutable().p_node,
			objects.header));

		--objects.header.node_count;
		return {link_type(p), get_node_allocator()};
	}
	node_type
	extract(const key_type& k)
	{
		node_type nh;
		const auto position(find(k));

		if(position != end())
			nh = extract(const_iterator(position));
		return nh;
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

	//! \since build 966
	YB_ATTR_nodiscard YB_PURE static auto
	get_components(tree<_tKey, _type, _fKeyOfValue, _fComp2, _tAlloc>& tree)
		ynothrow -> decltype((tree.objects))
	{
		return tree.objects;
	}
};

} // inline namespace rb_tree;

} // namespace details;
//!@}

//! \since build 927
//!@{
//! \relates details::tree_iterator
template<typename _type>
struct is_bitwise_swappable<details::tree_iterator<_type>> : true_
{};

//! \relates details::tree_const_iterator
template<typename _type>
struct is_bitwise_swappable<details::tree_const_iterator<_type>> : true_
{};
//!@}

} // namespace ystdex;

#endif

