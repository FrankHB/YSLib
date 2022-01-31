/*
	© 2019-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file list.hpp
\ingroup YStandardEx
\brief 列表容器。
\version r1693
\author FrankHB <frankhb1989@gmail.com>
\since build 864
\par 创建时间:
	2019-08-14 14:48:52 +0800
\par 修改时间:
	2022-01-26 06:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::List

提供 ISO C++17 标准库头 \c \<list> 兼容的替代接口和实现。
包括以下已有其它实现支持的 ISO C++17 后的修改：
WG21 P0084R2 ：返回类型增强。
包括以下已有其它实现支持的 ISO C++20 后的修改：
LWG 2839 ：允许自转移赋值。
*/


#ifndef YB_INC_ystdex_list_hpp_
#define YB_INC_ystdex_list_hpp_ 1

#include "node_base.h" // for "node_base.h", std::move, std::addressof;
#include <limits> // for std::numeric_limits;
#include <list> // for <list>, std::initializer_list;
#include "allocator.hpp" // for replace_storage_t, bidirectional_iteratable,
//	equality_comparable, totally_ordered, rebind_alloc_t, allocator_traits,
//	yverify, false_, true_, ystdex::make_move_if_noexcept_iterator,
//	std::advance, ystdex::alloc_on_move, conditional_t, allocator_guard,
//	ystdex::alloc_on_swap, ystdex::swap_dependent, std::allocator, is_object,
//	is_unqualified, and_, is_allocator_for, ystdex::reverse_iterator,
//	is_nothrow_constructible, less, ref_eq, equal_to, is_bitwise_swappable;
#include "base.h" // for noncopyable, nonmovable;
#include "iterator_trait.hpp" // for enable_for_input_iterator_t;
#include <algorithm> // for std::equal, std::lexicographical_compare;

namespace ystdex
{

#if false
// NOTE: For exposition only. Since %ystdex::list has some C++20 features
//	not proveded by ISO C++17, it should not be in %cpp2017 inline namespace.
#if (__cpp_lib_incomplete_container_elements >= 201505L \
	|| __cplusplus >= 201703L) && ((__cpp_lib_erase_if >= 201811L \
	&& __cpp_lib_list_remove_return_type >= 201806L) || __cplusplus >= 201811L)
#	define YB_Has_Cpp20_list true
#else
#	define YB_Has_Cpp20_list false
#endif
#undef YB_Has_Cpp20_list
#endif


//! \since build 864
namespace details
{

template<typename _type>
class list_node : public list_node_base
{
private:
	replace_storage_t<_type> storage;

public:
	//! \since build 855
	YB_ATTR_nodiscard YB_PURE yconstfn_relaxed _type*
	access_ptr() ynothrow
	{
		return static_cast<_type*>(storage.access());
	}
	//! \since build 855
	YB_ATTR_nodiscard YB_PURE yconstfn const _type*
	access_ptr() const ynothrow
	{
		return static_cast<const _type*>(storage.access());
	}
};


// XXX: See $2019-09 @ %Documentation::Workflow.
//! \since build 865
template<typename>
class list_const_iterator;


template<typename _type>
class list_iterator
	: public bidirectional_iteratable<list_iterator<_type>, _type&>
{
	//! \since build 866
	template<typename, class>
	friend class list_rep;
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
	//! \since build 866
	template<typename, class>
	friend class list_rep;

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


/*!
\brief 列表表示类型模板。
\invariant <tt>std::distance(begin(), end())
	== difference_type(objects.header.node_count)</tt>
\since build 866
*/
template<typename _type, class _tAlloc>
class list_rep
{
public:
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using allocator_type = _tAlloc;
	using iterator = list_iterator<_type>;
	using const_iterator = list_const_iterator<_type>;

protected:
	using base_ptr = list_node_base*;
	using const_base_ptr = const list_node_base*;

private:
	using value_node = list_node<_type>;

public:
	using link_type = value_node*;
	using const_link_type = const value_node*;

private:
	using node_allocator = rebind_alloc_t<_tAlloc, list_node<_type>>;
	using node_ator_traits = allocator_traits<node_allocator>;
	using equal_alloc_or_pocma = or_<typename node_ator_traits::is_always_equal,
		typename node_ator_traits::propagate_on_container_move_assignment>;

protected:
	// TODO: Resolve LWG 2112 with %is_final support.
	struct components : node_allocator
	{
		list_header header{};

		components() ynoexcept_spec(node_allocator())
			: node_allocator()
		{}
		//! \since build 867
		explicit
		components(node_allocator a) ynothrow
			: node_allocator(std::move(a))
		{}
		components(const components& x)
			: node_allocator(
			node_ator_traits::select_on_container_copy_construction(x))
		{}
		components(components&&) = default;
		//! \since build 867
		components(components&& x, node_allocator a) ynothrow
			: node_allocator(std::move(a)), header(std::move(x.header))
		{}
	};

private:
	struct node_remove : private noncopyable, private nonmovable
	{
		list_rep& list;
		base_ptr head;
		base_ptr* p_tail;

		explicit
		node_remove(list_rep& l) ynothrow
			: list(l), head(&list.objects.header), p_tail(std::addressof(head))
		{}
		~node_remove()
		{
			list.clear_nodes_from(head);
		}

		// NOTE: Track pending nodes in a singly linked list.
		void
		detach(iterator position) ynothrowv
		{
			const auto removed(position.p_node);

			yverify(list.objects.header != removed);
			--list.objects.header.node_count;
			removed->unhook();
			removed->next = &list.objects.header;
			*p_tail = removed;
			p_tail = &removed->next;
		}
	};

protected:
	components objects;

public:
	list_rep() = default;
	list_rep(const allocator_type& a) ynothrow
		: objects(node_allocator(a))
	{}
	list_rep(const list_rep& x)
		: objects(x.objects)
	{
		init_range(x.begin(), x.end());
	}
	list_rep(list_rep&&) = default;
	list_rep(list_rep&& x, const allocator_type& a)
		ynoexcept(node_ator_traits::is_always_equal::value)
		: list_rep(std::move(x), node_allocator(a),
		typename node_ator_traits::is_always_equal())
	{}

private:
	//! \since build 867
	list_rep(list_rep&& x, node_allocator a, true_) ynothrow
		: objects(std::move(x.objects), std::move(a))
	{}
	//! \since build 867
	list_rep(list_rep&& x, node_allocator a, false_)
		: objects(std::move(a))
	{
		if(get_node_allocator() == x.get_node_allocator())
			move_nodes(std::move(x));
		else
			insert_range(begin(),
				ystdex::make_move_if_noexcept_iterator(x.begin()),
				ystdex::make_move_if_noexcept_iterator(x.end()));
	}

public:
	~list_rep() ynothrow
	{
		clear_nodes();
	}

	list_rep&
	operator=(const list_rep& x)
	{
		if(std::addressof(x) != this)
		{
			if(typename
				node_ator_traits::propagate_on_container_copy_assignment())
			{
				auto& this_alloc(get_node_allocator());
				const auto& that_alloc(x.get_node_allocator());

				if(!typename node_ator_traits::is_always_equal()
					&& this_alloc != that_alloc)
					clear();
				ystdex::alloc_on_copy(this_alloc, that_alloc);
			}
			assign_range(x.begin(), x.end());
		}
		return *this;
	}
	list_rep&
	operator=(list_rep&& x) ynoexcept(yimpl(equal_alloc_or_pocma()))
	{
		move_assign_elements(x, equal_alloc_or_pocma());
		return *this;
	}

	template<typename _tIn>
	void
	init_range(_tIn first, _tIn last)
	{
		for(; first != last; ++first)
			insert_it(end(), *first);
	}

	template<typename _tIn>
	void
	assign_range(_tIn first, _tIn last)
	{
		auto i(begin());
		const auto j(end());

		// XXX: Excessive order refinment by ','?
		for(; i != j && first != last; ++i, static_cast<void>(++first))
			*i = *first;
		if(first == last)
			erase_range(i, j);
		else
			insert_range(j, first, last);
	}

	YB_ATTR_nodiscard YB_PURE allocator_type
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
		return iterator(objects.header.next);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	begin() const ynothrow
	{
		return const_iterator(objects.header.next);
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

	YB_ATTR_nodiscard YB_PURE bool
	empty() const ynothrowv
	{
		// XXX: This is not a narrow contract. It checks the sanity of the
		//	implementation.
		yverify((objects.header.next == &objects.header) == (size() == 0));
		return size() == 0;
	}

	YB_ATTR_nodiscard YB_PURE size_type
	size() const ynothrowv
	{
		// XXX: This is not a narrow contract. It checks the sanity of the
		//	implementation.
		yverify(std::distance(begin(), end())
			== difference_type(objects.header.node_count));
		return objects.header.node_count;
	}

	YB_ATTR_nodiscard YB_PURE size_type
	max_size() const ynothrow
	{
		// XXX: See $2022-01 @ %Documentation::Workflow.
		return std::numeric_limits<size_type>::max();
	}

	const_iterator
	resize_pos(size_type& sz) const
	{
		const_iterator i;
		const size_type len(size());

		if(sz < len)
		{
			if(sz <= len / 2)
			{
				i = begin();
				std::advance(i, sz);
			}
			else
			{
				const ptrdiff_t n_erase(len - sz);

				i = end();
				std::advance(i, -n_erase);
			}
			sz = 0;
			return i;
		}
		else
			i = end();
		sz -= len;
		return i;
	}

	void
	append_default(size_type n)
	{
		size_type i(0);

		try
		{
			for(; i < n; ++i)
				create_node()->hook(&objects.header);
		}
		catch(...)
		{
			auto cur(objects.header.prev);

			for(; i != 0; --i)
			{
				yverify(&objects.header.prev != &objects.header);

				const auto nd = link_type(cur);

				cur = nd->prev;
				drop_node(nd);
			}
			cur->next = &objects.header;
			throw;
		}
		objects.header.node_count += n;
	}

protected:
	void
	move_nodes(list_rep&& x) ynothrow
	{
		objects.header.move_nodes(std::move(x.objects.header));
	}

	/*!
	\brief 转移赋值可能不相等的不在转移赋值时传播的分配器的容器元素。
	\note 从不相等分配器转移元素结果是可能是复制而不是转移对象。
	*/
	void
	move_assign_elements(list_rep& x, false_)
	{
		if(get_node_allocator() == x.get_node_allocator())
			move_assign_elements(x, true_());
		else
			assign_range(ystdex::make_move_if_noexcept_iterator(x.begin()),
				ystdex::make_move_if_noexcept_iterator(x.end()));
	}
	//! \brief 转移赋值相等或在转移赋值时传播的分配器的容器元素。
	void
	move_assign_elements(list_rep& x, true_) ynothrow
	{
		// XXX: The resolution of LWG 2839 requires self-move to be
		//	well-defined.
		clear();
		move_nodes(std::move(x));
		ystdex::alloc_on_move(get_node_allocator(), x.get_node_allocator());
	}

public:
	template<typename... _tParams>
	void
	insert_it(iterator position, _tParams&&... args)
	{
		attach_node(create_node(yforward(args)...), position.p_node);
	}

	template<typename _tIn>
	iterator
	insert_range(iterator position, _tIn first, _tIn last)
	{
		if(first != last)
		{
			list_rep tmp(get_node_allocator());

			tmp.init_range(first, last);

			const auto i(tmp.begin());

			splice(position, tmp);
			return i;
		}
		return position.cast_mutable();
	}

	void
	erase_it(iterator position) ynothrow
	{
		drop_node(detach_node(position.p_node));
	}

	iterator
	erase_one(iterator position) ynothrow
	{
		const auto next(position.p_node->next);

		erase_it(position);
		return iterator(next);
	}

	void
	erase_range(iterator first, iterator last) ynothrow
	{
		// XXX: The naive implementation have good performance in general.
#if false
		if(first == begin() && last == end())
			clear();
		else
#endif
		{
#if false
			size_type n(0);
			const auto eptr(last.p_node);
			auto cur(first.p_node);
			const auto prev(cur->prev);

			while(cur != eptr)
			{
				const auto nd = link_type(cur);

				cur = nd->next;
				drop_node(nd);
				++n;
			}
			objects.header.node_count -= n;
			yunseq(prev->next = eptr, eptr->prev = prev);
#else
			while(first != last)
				first = erase_one(first);
#endif
		}
	}

	void
	pop_back() ynothrowv
	{
		yverify(!empty());
		erase_it(iterator(objects.header.prev));
	}

	template<typename... _tParams>
	iterator
	emplace(const_iterator position, _tParams&&... args)
	{
		const auto nd(create_node(yforward(args)...));

		attach_node(nd, position.cast_mutable().p_node);
		return iterator(nd);
	}

protected:
	YB_ATTR_nodiscard typename node_ator_traits::pointer
	allocate_node()
	{
		return node_ator_traits::allocate(objects, 1);
	}

	void
	deallocate_node(typename node_ator_traits::pointer p) ynothrow
	{
		node_ator_traits::deallocate(objects, p, 1);
	}

	template<typename... _tParams>
	void
	construct_node(link_type nd, _tParams&&... args)
	{
		// XXX: Ensure no need to use placment new for the node.
		static_assert(is_trivially_default_constructible<value_node>(),
			"Invalid node type found.");
		// XXX: See $2022-01 @ %Documentation::Workflow.
		using guard_alloc_t = conditional_t<sizeof(node_allocator)
			<= sizeof(void*), node_allocator, node_allocator&>;
		guard_alloc_t a(get_node_allocator());
		// NOTE: This should be same to %deallocate_node(nd) on exception
		//	thrown, except a copy of allocator may be used.
		allocator_guard<guard_alloc_t> gd(nd, a);

		node_ator_traits::construct(a, nd->access_ptr(), yforward(args)...);
		gd.release();
	}

	template<typename... _tParams>
	YB_ATTR_nodiscard YB_ATTR_returns_nonnull inline link_type
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
	void
	attach_node(link_type nd, base_ptr position) ynothrow
	{
		nd->hook(position);
		++objects.header.node_count;
	}

	YB_ATTR_nodiscard link_type
	detach_node(base_ptr cur)
	{
		const auto nd = link_type(cur);

		--objects.header.node_count;
		nd->unhook();
		return nd;
	}

public:
	void
	clear() ynothrow
	{
		clear_nodes();
		objects.header.reset();
	}

private:
	void
	clear_nodes() ynothrow
	{
		clear_nodes_from(objects.header.next);
	}

	void
	clear_nodes_from(base_ptr cur) ynothrow
	{
		while(cur != &objects.header)
		{
			const auto nd = link_type(cur);

			cur = nd->next;
			drop_node(nd);
		}
	}

public:
	friend void
	swap(list_rep& x, list_rep& y) ynothrow
	{
		list_node_base::swap(x.objects.header, y.objects.header);
		std::swap(x.objects.header.node_count, y.objects.header.node_count);
		ystdex::alloc_on_swap(x.get_node_allocator(), y.get_node_allocator());
	}

private:
	void
	expects_same_allocators(list_rep& x) ynothrowv
	{
		// NOTE: [list.ops] specifies the behavior is undefined in %splice if
		//	the allocators are not equal.
		expects_same_allocators(x,
			typename node_ator_traits::is_always_equal());
	}
	void
	expects_same_allocators(list_rep& x, false_) ynothrowv
	{
		yunused(x);
		// NOTE: [list.ops] specifies the behavior is undefined in %splice if
		//	the allocators are not equal.
		yverify(x.get_node_allocator() == get_node_allocator());
	}
	void
	expects_same_allocators(list_rep&, true_) ynothrow
	{}

	void
	transfer_range(iterator position, iterator first, iterator last)
	{
		position.p_node->transfer(first.p_node, last.p_node);
	}
	void
	transfer_range(iterator position, const_iterator first, const_iterator last)
	{
		transfer_range(position, first.cast_mutable(), last.cast_mutable());
	}

public:
	void
	splice(iterator position, list_rep& x) ynothrowv
	{
		expects_same_allocators(x);
		yverify(std::addressof(x) != this);
		if(!x.empty())
		{
			transfer_range(position, x.begin(), x.end());
			objects.header.node_count += x.objects.header.node_count;
			x.objects.header.node_count = 0;
		}
	}
	void
	splice(iterator position, list_rep& x, const_iterator i) ynothrowv
	{
		expects_same_allocators(x);
		// XXX: Use %YB_VerifyIterator?
		yverify(i != x.end());

		auto j(i);

		++j;
		if(!(position == i || position == j))
		{
			// XXX: The node count changes shall not be unsquenced if %*this is
			//	same to %x, so no %yunseq is used here.
			transfer_range(position, i, j),
			++objects.header.node_count,
			--x.objects.header.node_count;
		}
	}
	void
	splice(iterator position, list_rep& x, const_iterator first,
		const_iterator last) ynothrowv
	{
		expects_same_allocators(x);
		if(first != last)
		{
			// XXX: Use %YB_VerifyIterator?
			yverify(first != x.end());

			const auto n(size_t(std::distance(first, last)));

			if(position != last)
				// XXX: The node count changes shall not be unsquenced if %*this
				//	is same to %x, so no %yunseq is used here.
				transfer_range(position, first, last),
				objects.header.node_count += n,
				x.objects.header.node_count -= n;
		}
	}

private:
	template<typename _func>
	size_type
	remove_for(_func f)
	{
		const auto old_size(size());
		const auto last(end());
		auto first(begin());

		while(first != last)
		{
			auto next(first);

			++next;
			f(first);
			first = next;
		}
		return old_size - size();
	}

public:
	size_type
	remove(const _type& value)
	{
		auto extra(end());
		auto n_removed(remove_for([&, this](iterator i){
			if(*i == value)
			{
				if(!ref_eq<>()(*i, value))
					erase_it(i);
				else
					extra = i;
			}
		}));

		if(extra != end())
		{
			erase_it(extra);
			++n_removed;
		}
		return n_removed;
	}

	template<typename _fPred>
	size_type
	remove_if(_fPred& pred)
	{
		node_remove op(*this);

		return remove_for([&](iterator i){
			if(pred(*i))
				op.detach(i);
		});
	}

	template<typename _fBiPred>
	size_type
	unique(_fBiPred& binary_pred)
	{
		node_remove op(*this);
		const auto last(end());
		auto first(begin());

		if(first != last)
		{
			const auto old_size(size());
			auto next(first);

			while(++next != last)
			{
				if(binary_pred(*first, *next))
					op.detach(first);
				first = next;
			}
			return old_size - size();
		}
		return 0;
	}

	//! \see LWG 300 。
	template<typename _fComp>
	void
	merge(list_rep& x, _fComp& comp)
	{
		expects_same_allocators(x);
		if(std::addressof(x) != this)
		{
			auto first1(begin()), first2(x.begin()), last1(end()),
				last2(x.end());
			const size_t orig_size(x.size());

			try
			{
				while(first1 != last1 && first2 != last2)
					if(comp(*first2, *first1))
					{
						auto next(first2);

						transfer_range(first1, first2, ++next);
						first2 = next;
					}
					else
						++first1;
				if(first2 != last2)
					transfer_range(last1, first2, last2);
				objects.header.node_count += x.objects.header.node_count;
				x.objects.header.node_count = 0;
			}
			catch(...)
			{
				const size_t dist(std::distance(first2, last2));

				objects.header.node_count += orig_size - dist;
				x.objects.header.node_count = dist;
				throw;
			}
		}
	}

	// NOTE: This is basically a stable quick sort implementation for lists,
	//	returning the suggested pivot iterator as the new first iterator of a
	//	range possibly necessary to be sorted later.
	template<typename _fComp>
	iterator
	sort(iterator first, iterator last, size_type n, _fComp& comp)
	{
		yverify(std::distance(first, last) == n);
		if(n > 1)
		{
			const auto half(n / 2);
			auto mid(std::next(first, half));

			first = sort(first, mid, half, comp);
			mid = sort(mid, last, n - half, comp);

			auto next(first);

			for(bool init(true); ; init = {})
			{
				// NOTE: [first, mid) and [mid, last) are sorted and non-empty.
				// XXX: Use %YB_VerifyIterator?
				yverify(mid != end());
				yverify(first != end());
				// NOTE: This is stable as no equivalent elements will cause
				//	the reordering implemented as the splice operation.
				if(comp(*mid, *first))
				{
					// NOTE: Note %mid will be spliced before %first. As the
					//	result of %transfer_range, the range needing to be
					//	sorted (and guaranteed sorted after this call) is
					//	expanded. This only requires the update at the first
					//	iteration, since all other operations do not expand the
					//	range any more.
					if(init)
						next = mid;
					{
						auto new_mid(mid);
						auto j(mid);

						++j;
						if(!(first == mid || first == j))
							transfer_range(first, mid, j),
						mid = new_mid;
					}
					if(mid == last)
						return next;
				}
				else
				{
					++first;
					if(first == mid)
						return next;
				}
			}
		}
		return first;
	}

	void
	reverse() ynothrow
	{
		objects.header.reverse_nodes();
	}
};

} // namespace details;

//! \since build 927
//@{
//! \relates details::list_iterator
template<typename _type>
struct is_bitwise_swappable<details::list_iterator<_type>> : true_
{};

//! \relates details::list_const_iterator
template<typename _type>
struct is_bitwise_swappable<details::list_const_iterator<_type>> : true_
{};
//@}


/*!
\ingroup YBase_replacement_features
\brief 列表容器。
\note 和 ISO C++ 的 std::list 类似，但支持不完整类型。
\warning 非虚析构。
\see Documentation::YBase @2.1.4.1 。
\see Documentation::YBase @2.1.4.2 。
\see ISO C++17 [list.overview]/3 。
\see WG21 N4371
\since build 866

类似 ISO C++17 的 std::list 的容器，包括不完整类型作为值类型。
部分成员提供较 ISO C++17 更强的 noexcept 异常规范，除转移赋值略有不同。
*/
template<typename _type, class _tAlloc = std::allocator<_type>>
class list : private totally_ordered<list<_type, _tAlloc>>
{
public:
	using value_type = _type;
	/*!
	\see ISO C++17 [allocator.requirements] 。
	\see ISO C++17 [container.requirements.general]/15 。
	\see LWG 274 。
	\see LWG 2447 。
	*/
	static_assert(and_<is_object<_type>, is_unqualified<_type>>(),
		"The value type for allocator shall be an unqualified object type.");
	static_assert(is_allocator_for<_tAlloc, value_type>(),
		"Value type mismatched to the allocator found.");
	using allocator_type = _tAlloc;

private:
	using ator_traits = allocator_traits<allocator_type>;
	using rep_type = details::list_rep<_type, _tAlloc>;

public:
	using pointer = typename ator_traits::pointer;
	using const_pointer = typename ator_traits::const_pointer;
	using reference = value_type&;
	using const_reference = const value_type&;
	//! \note 实现定义：等价于 std::size_t 的类型。
	using size_type = typename rep_type::size_type;
	//! \note 实现定义：等价 std::ptrdiff_t 的类型。
	using difference_type = typename rep_type::difference_type;
	//! \note 实现定义：符合要求的未指定类型。
	//@{
	using iterator = yimpl(typename rep_type::iterator);
	using const_iterator = yimpl(typename rep_type::const_iterator);
	//@}
	using reverse_iterator = ystdex::reverse_iterator<iterator>;
	using const_reverse_iterator = ystdex::reverse_iterator<const_iterator>;

private:
	rep_type rep;

public:
	list() yimpl(= default);
	explicit
	list(const allocator_type& a) ynothrow
		: rep(a)
	{}
	explicit
	list(size_type n, const allocator_type& a = allocator_type())
		: rep(a)
	{
		for(; n != 0; --n)
			emplace_back();
	}
	list(size_type n, const value_type& value,
		const allocator_type& a = allocator_type())
		: rep(a)
	{
		init_fill(n, value);
	}
	// NOTE: The resolution has been implemented in libstdc++ since r184911,
	//	see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=43813.
	/*!
	\see ISO C++17 [sequences.reqmts]/13 。
	\see LWG 438 。
	\see LWG 1234 。
	*/
	template<typename _tIn,
		yimpl(typename = enable_for_input_iterator_t<_tIn>)>
	list(_tIn first, _tIn last, const allocator_type& a = allocator_type())
		: rep(a)
	{
		init_range(first, last);
	}
	list(std::initializer_list<value_type> il,
		const allocator_type& a = allocator_type())
		: rep(a)
	{
		init_range(il.begin(), il.end());
	}
	list(const list&) yimpl(= default);
	list(const list& x, const allocator_type& a)
		: rep(a)
	{
		init_range(x.begin(), x.end());
	}
	// XXX: The exception specification is strengthened to 'noexcept'.
	//	In ISO C++17 there is no explicit exception specification.
	list(list&&) yimpl(= default);
	// XXX: The exception specification is strengthened to noexcept
	//	specification having the operand equivalent to the conjunction of
	//	%std::allocator_traits::is_always_equal trait of the internal node. In
	//	ISO C++17 there is no explicit exception specification.
	list(list&& x, const allocator_type& a)
		yimpl(ynoexcept(is_nothrow_constructible<
		rep_type, rep_type&&, const allocator_type&>()))
		: rep(std::move(x.rep), a)
	{}

public:
	~list() yimpl(= default);

	list&
	operator=(const list&) yimpl(= default);
	// XXX: The exception specification is changed. ISO C++17 only requires
	//	conditional non-throwing exception specification when the allocator
	//	meets %std::allocator_traits<allocator_type>::is_always_equal, with
	//	regardless to %propagate_on_container_move_assignment of the node
	//	allocator. Here the %allocator_traits of the internal node is also used
	//	instead. See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91541 and LWG
	//	3267.
	list&
	operator=(list&&) yimpl(= default);
	list&
	operator=(std::initializer_list<value_type> il)
	{
		assign(il);
		return *this;
	}

private:
	void
	init_fill(size_type n, const value_type& x)
	{
		for(; n != 0; --n)
			push_back(x);
	}

	/*!
	\see ISO C++17 [sequences.reqmts]/13 。
	\see LWG 438 。
	\see LWG 1234 。
	*/
	template<typename _tIn>
	void
	init_range(_tIn first, _tIn last)
	{
		rep.init_range(first, last);
	}

	void
	assign_fill(size_type n, const value_type& val)
	{
		iterator i = begin();
		for(; i != end() && n > 0; ++i, --n)
			*i = val;
		if(n > 0)
			insert(end(), n, val);
		else
			erase(i, end());
	}

public:
	void
	assign(size_type n, const value_type& val)
	{
		assign_fill(n, val);
	}
	/*!
	\see ISO C++17 [sequences.reqmts]/13 。
	\see LWG 438 。
	\see LWG 1234 。
	*/
	template<typename _tIn, yimpl(typename = enable_for_input_iterator_t<_tIn>)>
	void
	assign(_tIn first, _tIn last)
	{
		rep.assign_range(first, last);
	}
	void
	assign(std::initializer_list<value_type> il)
	{
		rep.assign_range(il.begin(), il.end());
	}

	YB_ATTR_nodiscard YB_PURE allocator_type
	get_allocator() const ynothrow
	{
		return rep.get_allocator();
	}

	YB_ATTR_nodiscard YB_PURE iterator
	begin() ynothrow
	{
		return rep.begin();
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	begin() const ynothrow
	{
		return rep.begin();
	}

	YB_ATTR_nodiscard YB_PURE iterator
	end() ynothrow
	{
		return rep.end();
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	end() const ynothrow
	{
		return rep.end();
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

	YB_ATTR_nodiscard YB_PURE const_iterator
	cbegin() const ynothrow
	{
		return begin();
	}

	YB_ATTR_nodiscard YB_PURE const_iterator
	cend() const ynothrow
	{
		return end();
	}

	YB_ATTR_nodiscard YB_PURE const_reverse_iterator
	crbegin() const ynothrow
	{
		return const_reverse_iterator(end());
	}

	YB_ATTR_nodiscard YB_PURE const_reverse_iterator
	crend() const ynothrow
	{
		return const_reverse_iterator(begin());
	}

	YB_ATTR_nodiscard YB_PURE bool
	empty() const ynothrow
	{
		return rep.empty();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	size() const ynothrow
	{
		return rep.size();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	max_size() const ynothrow
	{
		return rep.max_size();
	}

	void
	resize(size_type sz)
	{
		const auto i(rep.resize_pos(sz));

		if(sz != 0)
			rep.append_default(sz);
		else
			erase(i, end());
	}
	void
	resize(size_type sz, const value_type& x)
	{
		const auto i(rep.resize_pos(sz));

		if(sz != 0)
			insert(end(), sz, x);
		else
			erase(i, end());
	}

	YB_ATTR_nodiscard YB_PURE reference
	front() ynothrow
	{
		return *begin();
	}
	YB_ATTR_nodiscard YB_PURE const_reference
	front() const ynothrow
	{
		return *begin();
	}

	YB_ATTR_nodiscard YB_PURE reference
	back() ynothrow
	{
		auto i(end());

		--i;
		return *i;
	}
	YB_ATTR_nodiscard YB_PURE const_reference
	back() const ynothrow
	{
		auto i(cend());

		--i;
		return *i;
	}

	//! \see WG21 P0084R2 。
	template<typename... _tParams>
	reference
	emplace_front(_tParams&&... args)
	{
		rep.insert_it(begin(), yforward(args)...);
		return front();
	}

	//! \see WG21 P0084R2 。
	template<typename... _tParams>
	reference
	emplace_back(_tParams&&... args)
	{
		rep.insert_it(end(), yforward(args)...);
		return back();
	}

	void
	push_front(const value_type& x)
	{
		rep.insert_it(begin(), x);
	}
	void
	push_front(value_type&& x)
	{
		rep.insert_it(begin(), std::move(x));
	}

	void
	pop_front() yimpl(ynothrowv)
	{
		yverify(!empty());
		rep.erase_it(begin());
	}

	void
	push_back(const value_type& x)
	{
		rep.insert_it(end(), x);
	}
	void
	push_back(value_type&& x)
	{
		rep.insert_it(end(), std::move(x));
	}

	void
	pop_back() yimpl(ynothrowv)
	{
		rep.pop_back();
	}

	template<typename... _tParams>
	iterator
	emplace(const_iterator position, _tParams&&... args)
	{
		return rep.emplace(position, yforward(args)...);
	}

	iterator
	insert(const_iterator position, const value_type& x)
	{
		return rep.emplace(position, x);
	}
	iterator
	insert(const_iterator position, value_type&& x)
	{
		return rep.emplace(position, std::move(x));
	}
	iterator
	insert(const_iterator p, std::initializer_list<value_type> il)
	{
		return insert(p, il.begin(), il.end());
	}
	iterator
	insert(const_iterator position, size_type n, const value_type& x)
	{
		if(n != 0)
		{
			list tmp(n, x, get_allocator());
			auto i(tmp.begin());

			splice(position, tmp);
			return i;
		}
		return position.cast_mutable();
	}
	/*!
	\see ISO C++17 [sequences.reqmts]/13 。
	\see LWG 438 。
	\see LWG 1234 。
	*/
	template<typename _tIn,
		yimpl(typename = enable_for_input_iterator_t<_tIn>)>
	iterator
	insert(const_iterator position, _tIn first, _tIn last)
	{
		return rep.insert_range(position, first, last);
	}

	iterator
	erase(const_iterator position) yimpl(ynothrow)
	{
		return rep.erase_one(position.cast_mutable());
	}
	iterator
	erase(const_iterator first, const_iterator last) yimpl(ynothrow)
	{
		auto res(last.cast_mutable());

		rep.erase_range(first.cast_mutable(), res);
		return res;
	}

	// XXX: The exception specification is strengthened. ISO C++17 only requires
	//	conditional non-throwing exception specification when the allocator
	//	meets %std::allocator_traits<allocator_type>::is_always_equal.
	void
	swap(list& x) ynothrow
	{
		ystdex::swap_dependent(rep, x.rep);
	}
	friend void
	swap(list<_type, _tAlloc>& x, list<_type, _tAlloc>& y)
		ynoexcept_spec(x.swap(y))
	{
		x.swap(y);
	}

	void
	clear() ynothrow
	{
		rep.clear();
	}

	void
	splice(const_iterator position, list& x) yimpl(ynothrowv)
	{
		rep.splice(position.cast_mutable(), x.rep);
	}
	void
	splice(const_iterator position, list&& x) yimpl(ynothrowv)
	{
		splice(position, x);
	}
	void
	splice(const_iterator position, list& x, const_iterator i) yimpl(ynothrowv)
	{
		rep.splice(position.cast_mutable(), x.rep, i);
	}
	void
	splice(const_iterator position, list&& x, const_iterator i) yimpl(ynothrowv)
	{
		splice(position, x, i);
	}
	void
	splice(const_iterator position, list& x, const_iterator first,
		const_iterator last) yimpl(ynothrowv)
	{
		rep.splice(position.cast_mutable(), x.rep, first, last);
	}
	void
	splice(const_iterator position, list&& x, const_iterator first,
		const_iterator last) yimpl(ynothrowv)
	{
		splice(position, x, first, last);
	}

	//! \see LWG 526 。
	//@{
	size_type
	remove(const _type& value)
	{
		return rep.remove(value);
	}

	template<typename _fPred>
	size_type
	remove_if(_fPred pred)
	{
		return rep.remove_if(pred);
	}

	size_type
	unique()
	{
		return unique(equal_to<>());
	}
	template<typename _fBiPred>
	size_type
	unique(_fBiPred binary_pred)
	{
		return rep.unique(binary_pred);
	}
	//@}

	void
	merge(list& x)
	{
		merge(std::move(x), less<>());
	}
	void
	merge(list&& x)
	{
		merge(x);
	}
	//! \see LWG 300 。
	template<typename _fComp>
	void
	merge(list& x, _fComp comp)
	{
		rep.merge(x.rep, comp);
	}
	template<typename _fComp>
	void
	merge(list&& x, _fComp comp)
	{
		merge(x, comp);
	}

	void
	sort()
	{
		sort(less<>());
	}
	template<typename _fComp>
	void
	sort(_fComp comp)
	{
		rep.sort(begin(), end(), size(), comp);
	}

	void
	reverse() ynothrow
	{
		rep.reverse();
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const list& x, const list& y)
	{
		return x.size() == y.size()
			&& std::equal(x.cbegin(), x.cend(), y.cbegin());
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator<(const list& x, const list& y)
	{
		return std::lexicographical_compare(x.cbegin(), x.cend(), y.cbegin(),
			y.cend());
	}
};

} // namespace ystdex;

#endif

