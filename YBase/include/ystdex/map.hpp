/*
	© 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file map.hpp
\ingroup YStandardEx
\brief 映射容器。
\version r787
\author FrankHB <frankhb1989@gmail.com>
\since build 830
\par 创建时间:
	2018-07-06 21:12:51 +0800
\par 修改时间:
	2018-07-11 15:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Map
*/


#ifndef YB_INC_ystdex_map_hpp_
#define YB_INC_ystdex_map_hpp_ 1

#include "tree.h" // for "tree.h", less, std::pair, std::allocator, std::pair,
//	totally_ordered, std::is_same, allocator_traits, first_of,
//	is_nothrow_copy_constructible, and_, is_constructible, enable_if_t,
//	ystdex::swap_dependent;
#include <map> // for <map>, std::initializer_list;
#include <tuple> // for std::piecewise_construct, std::tuple;

namespace ystdex
{

//! \since build 930
//@{
#if false
// NOTE: For exposition only. Since %ystdex::map has incomplete type support
//	which is not guaranteed by ISO C++17 (even if guaranteed by libstdc++), it
//	should not be in %cpp2017 inline namespace.
#if (__cpp_lib_generic_associative_lookup >= 201304 || __cplusplus >= 201402L) \
	&& ((__cpp_lib_allocator_traits_is_always_equal >= 201411 \
	&& __cpp_lib_map_try_emplace >= 201411 \
	&& __cpp_lib_node_extract >= 201606 \
	&& __cpp_lib_nonmember_container_access >= 201411) \
	|| __cplusplus >= 201703L)
#	define YB_Impl_Has_Cpp17_map true
#else
#	define YB_Impl_Has_Cpp17_map false
#endif
#endif

//! \todo 在其它头文件中提供实现。
template<typename, typename, typename, class>
class multimap;

/*!
\brief 映射容器。
\note 和 ISO C++ 的 std::map 类似，但支持不完整类型。
\warning 非虚析构。
*/
template<typename _tKey, typename _type, typename _fComp = less<_tKey>,
	class _tAlloc = std::allocator<std::pair<const _tKey, _type>>>
class map : private totally_ordered<map<_tKey, _type, _fComp, _tAlloc>>
{
	template<typename, typename>
	friend class details::rb_tree::tree_merge_helper;

public:
	using key_type = _tKey;
	using mapped_type = _type;
	using value_type = std::pair<const _tKey, _type>;
	static_assert(std::is_same<typename allocator_traits<_tAlloc>::value_type,
		value_type>(), "Value type mismatched to the allocator found.");
	using key_compare = _fComp;
	using allocator_type = _tAlloc;
	class value_compare
	{
		friend class map<_tKey, _type, _fComp, _tAlloc>;

	protected:
		_fComp comp;

		value_compare(_fComp c) : comp(c)
		{}

	public:
		bool
		operator()(const value_type& x, const value_type& y) const
		{
			return comp(x.first, y.first);
		}
	};

private:
	using pair_alloc_type = typename allocator_traits<_tAlloc>::template
		rebind_alloc<value_type>;
	using alloc_traits = allocator_traits<pair_alloc_type>;
	using rep_type = details::rb_tree::tree<key_type, value_type, first_of<>,
		key_compare, pair_alloc_type>;

	rep_type tree;

public:
	using pointer = typename alloc_traits::pointer;
	using const_pointer = typename alloc_traits::const_pointer;
	using reference = value_type&;
	using const_reference = const value_type&;
	using iterator = typename rep_type::iterator;
	using const_iterator = typename rep_type::const_iterator;
	using size_type = typename rep_type::size_type;
	using difference_type = typename rep_type::difference_type;
	using reverse_iterator = typename rep_type::reverse_iterator;
	using const_reverse_iterator = typename rep_type::const_reverse_iterator;
	using node_type = typename rep_type::node_type;
	using insert_return_type = typename rep_type::insert_return_type;

	map() = default;
	explicit map(const _fComp& comp, const allocator_type& a = allocator_type())
		: tree(comp, pair_alloc_type(a))
	{}

	explicit map(const allocator_type& a)
		: tree(_fComp(), pair_alloc_type(a))
	{}
	template<typename _tIn>
	map(_tIn first, _tIn last)
		: tree()
	{
		tree.insert_unique(first, last);
	}
	template<typename _tIn>
	map(_tIn first, _tIn last, const allocator_type& a)
		: tree(_fComp(), pair_alloc_type(a))
	{
		tree.insert_unique(first, last);
	}
	template<typename _tIn>
	map(_tIn first, _tIn last, const _fComp& comp,
		const allocator_type& a = allocator_type())
		: tree(comp, pair_alloc_type(a))
	{
		tree.insert_unique(first, last);
	}
	map(std::initializer_list<value_type> il, const _fComp& comp = _fComp(),
		const allocator_type& a = allocator_type())
		: tree(comp, pair_alloc_type(a))
	{
		tree.insert_unique(il.begin(), il.end());
	}
	map(std::initializer_list<value_type> il, const allocator_type& a)
		: tree(_fComp(), pair_alloc_type(a))
	{
		tree.insert_unique(il.begin(), il.end());
	}
	map(const map&) = default;
	map(const map& m, const allocator_type& a)
		: tree(m.tree, pair_alloc_type(a))
	{}
	map(map&&) = default;
	map(map&& m, const allocator_type& a)
		ynoexcept_spec(and_<is_nothrow_copy_constructible<_fComp>,
		typename alloc_traits::is_always_equal>::value)
		: tree(std::move(m.tree), pair_alloc_type(a))
	{}
	~map() = default;

	map&
	operator=(const map&) = default;
	map&
	operator=(map&&) = default;
	map&
	operator=(std::initializer_list<value_type> il)
	{
		tree.assign_unique(il.begin(), il.end());
		return *this;
	}

	allocator_type
	get_allocator() const ynothrow
	{
		return allocator_type(tree.get_allocator());
	}

	iterator
	begin() ynothrow
	{
		return tree.begin();
	}

	const_iterator
	begin() const ynothrow
	{
		return tree.begin();
	}

	iterator
	end() ynothrow
	{
		return tree.end();
	}

	const_iterator
	end() const ynothrow
	{
		return tree.end();
	}

	reverse_iterator
	rbegin() ynothrow
	{
		return tree.rbegin();
	}

	const_reverse_iterator
	rbegin() const ynothrow
	{
		return tree.rbegin();
	}

	reverse_iterator
	rend() ynothrow
	{
		return tree.rend();
	}

	const_reverse_iterator
	rend() const ynothrow
	{
		return tree.rend();
	}

	const_iterator
	cbegin() const noexcept
	{
		return tree.begin();
	}

	const_iterator
	cend() const noexcept
	{
		return tree.end();
	}

	const_reverse_iterator
	crbegin() const noexcept
	{
		return tree.rbegin();
	}

	const_reverse_iterator
	crend() const noexcept
	{
		return tree.rend();
	}

	bool
	empty() const ynothrow
	{
		return tree.empty();
	}

	size_type
	size() const ynothrow
	{
		return tree.size();
	}

	size_type
	max_size() const ynothrow
	{
		return tree.max_size();
	}

	mapped_type&
	operator[](const key_type& k)
	{
		auto i(lower_bound(k));

		if(i == end() || key_comp()(k, (*i).first))
			i = tree.emplace_hint_unique(i, std::piecewise_construct,
				std::tuple<const key_type&>(k), std::tuple<>());
		return (*i).second;
	}

	mapped_type&
	operator[](key_type&& k)
	{
		auto i(lower_bound(k));

		if(i == end() || key_comp()(k, (*i).first))
			i = tree.emplace_hint_unique(i, std::piecewise_construct,
				std::forward_as_tuple(std::move(k)), std::tuple<>());
		return (*i).second;
	}

	mapped_type&
	at(const key_type& k)
	{
		return at_impl(*this, k, lower_bound(k));
	}
	const mapped_type&
	at(const key_type& k) const
	{
		return at_impl(*this, k, lower_bound(k));
	}

private:
	template<class _tClass, typename _tFwd>
	static auto
	at_impl(_tClass&& m, const key_type& k, _tFwd i) -> decltype(((*i).second))
	{
		if(i == m.end() || m.key_comp()(k, (*i).first))
			// XXX: Is it needed to extract and put into a function?
			throw std::out_of_range("map::at");
		return (*i).second;
	}

public:
	template<typename... _tParams>
	std::pair<iterator, bool>
	emplace(_tParams&&... args)
	{
		return tree.emplace_unique(yforward(args)...);
	}

	template<typename... _tParams>
	iterator
	emplace_hint(const_iterator position, _tParams&&... args)
	{
		return tree.emplace_hint_unique(position, yforward(args)...);
	}

	node_type
	extract(const_iterator pos)
	{
		yassume(pos != end());
		return tree.extract(pos);
	}
	node_type
	extract(const key_type& x)
	{
		return tree.extract(x);
	}

	insert_return_type
	insert(node_type&& nh)
	{
		return tree.reinsert_node_unique(std::move(nh));
	}
	iterator
	insert(const_iterator hint, node_type&& nh)
	{
		return tree.reinsert_node_hint_unique(hint, std::move(nh));
	}

	template<typename _tCon2>
	void
	merge(map<_tKey, _type, _tCon2, _tAlloc>& src)
	{
		tree.merge_unique(
			details::rb_tree::tree_merge_helper<map, _tCon2>::get_tree(src));
	}
	template<typename _tCon2>
	void
	merge(map<_tKey, _type, _tCon2, _tAlloc>&& src)
	{
		merge(src);
	}
	template<typename _tCon2>
	void
	merge(multimap<_tKey, _type, _tCon2, _tAlloc>& src)
	{
		tree.merge_unique(
			details::rb_tree::tree_merge_helper<map, _tCon2>::get_tree(src));
	}
	template<typename _tCon2>
	void
	merge(multimap<_tKey, _type, _tCon2, _tAlloc>&& src)
	{
		merge(src);
	}

	template<typename... _tParams>
	std::pair<iterator, bool>
	try_emplace(const key_type& k, _tParams&&... args)
	{
		const auto i(lower_bound(k));

		if(i == end() || key_comp()(k, (*i).first))
		{
			i = emplace_hint(i, std::piecewise_construct, std::forward_as_tuple(
				k), std::forward_as_tuple(yforward(args)...));
			return {i, true};
		}
		return {i, {}};
	}
	template<typename... _tParams>
	std::pair<iterator, bool>
	try_emplace(key_type&& k, _tParams&&... args)
	{
		const auto i(lower_bound(k));

		if(i == end() || key_comp()(k, (*i).first))
		{
			i = emplace_hint(i, std::piecewise_construct, std::forward_as_tuple(
				std::move(k)), std::forward_as_tuple(yforward(args)...));
			return {i, true};
		}
		return {i, {}};
	}

	template<typename... _tParams>
	iterator
	try_emplace(const_iterator hint, const key_type& k, _tParams&&... args)
	{
		iterator i;
		auto true_hint = tree.get_insert_hint_unique_pos(hint, k);
		if(true_hint.second)
			i = emplace_hint(iterator(true_hint.second),
				std::piecewise_construct,
				std::forward_as_tuple(k),
				std::forward_as_tuple(yforward(args)...));
		else
			i = iterator(true_hint.first);
		return i;
	}
	template<typename... _tParams>
	iterator
	try_emplace(const_iterator hint, key_type&& k, _tParams&&... args)
	{
		iterator i;
		auto true_hint = tree.get_insert_hint_unique_pos(hint, k);
		if(true_hint.second)
			i = emplace_hint(iterator(true_hint.second),
				std::piecewise_construct, std::forward_as_tuple(std::move(k)),
				std::forward_as_tuple(yforward(args)...));
		else
			i = iterator(true_hint.first);
		return i;
	}

	std::pair<iterator, bool>
	insert(const value_type& x)
	{
		return tree.insert_unique(x);
	}
	std::pair<iterator, bool>
	insert(value_type&& x)
	{
		return tree.insert_unique(std::move(x));
	}
	template<typename _tPair,
		typename = enable_if_t<is_constructible<value_type, _tPair&&>::value>>
	std::pair<iterator, bool>
	insert(_tPair&& x)
	{
		return tree.insert_unique(std::forward<_tPair>(x));
	}
	void
	insert(std::initializer_list<value_type> il)
	{
		insert(il.begin(), il.end());
	}
	iterator
	insert(const_iterator position, const value_type& x)
	{
		return tree.insert_hint_unique(position, x);
	}
	iterator
	insert(const_iterator position, value_type&& x)
	{
		return tree.insert_hint_unique(position, std::move(x));
	}

	template<typename _tPair,
		typename = enable_if_t<is_constructible<value_type, _tPair&&>::value>>
	iterator
	insert(const_iterator position, _tPair&& x)
	{
		return tree.insert_hint_unique(position, std::forward<_tPair>(x));
	}

	template<typename _tIn>
	void
	insert(_tIn first, _tIn last)
	{
		tree.insert_unique(first, last);
	}

	template<typename _tObj>
	std::pair<iterator, bool>
	insert_or_assign(const key_type& k, _tObj&& obj)
	{
		iterator i = lower_bound(k);
		if(i == end() || key_comp()(k, (*i).first))
		{
			i = emplace_hint(i,
				std::piecewise_construct,
				std::forward_as_tuple(k),
				std::forward_as_tuple(std::forward<_tObj>(obj)));
			return {i, true};
		}
		(*i).second = std::forward<_tObj>(obj);
		return {i, {}};
	}

	template<typename _tObj>
	std::pair<iterator, bool>
	insert_or_assign(key_type&& k, _tObj&& obj)
	{
		iterator i = lower_bound(k);
		if(i == end() || key_comp()(k, (*i).first))
		{
			i = emplace_hint(i,
				std::piecewise_construct,
				std::forward_as_tuple(std::move(k)),
				std::forward_as_tuple(std::forward<_tObj>(obj)));
			return {i, true};
		}
		(*i).second = std::forward<_tObj>(obj);
		return {i, {}};
	}

	template<typename _tObj>
	iterator
	insert_or_assign(const_iterator hint, const key_type& k, _tObj&& obj)
	{
		iterator i;
		const auto true_hint(tree.get_insert_hint_unique_pos(hint, k));

		if(true_hint.second)
		{
			return emplace_hint(iterator(true_hint.second),
				std::piecewise_construct,
				std::forward_as_tuple(k),
				std::forward_as_tuple(std::forward<_tObj>(obj)));
		}
		i = iterator(true_hint.first);
		(*i).second = std::forward<_tObj>(obj);
		return i;
	}
	template<typename _tObj>
	iterator
	insert_or_assign(const_iterator hint, key_type&& k, _tObj&& obj)
	{
		iterator i;
		auto true_hint(tree.get_insert_hint_unique_pos(hint, k));

		if(true_hint.second)
		{
			return emplace_hint(iterator(true_hint.second),
				std::piecewise_construct,
				std::forward_as_tuple(std::move(k)),
				std::forward_as_tuple(std::forward<_tObj>(obj)));
		}
		i = iterator(true_hint.first);
		(*i).second = std::forward<_tObj>(obj);
		return i;
	}

	iterator
	erase(iterator position)
	{
		return tree.erase(position);
	}
	iterator
	erase(const_iterator position)
	{
		return tree.erase(position);
	}
	size_type
	erase(const key_type& x)
	{
		return tree.erase(x);
	}
	iterator
	erase(const_iterator first, const_iterator last)
	{
		return tree.erase(first, last);
	}

	void
	swap(map& x) ynoexcept_spec(is_nothrow_swappable<_fComp>::value)
	{
		ystdex::swap_dependent(tree, x.tree);
	}

	void
	clear() ynothrow
	{
		tree.clear();
	}

	key_compare
	key_comp() const
	{
		return tree.key_comp();
	}

	value_compare
	value_comp() const
	{
		return value_compare(tree.key_comp());
	}

	iterator
	find(const key_type& x)
	{
		return tree.find(x);
	}

	template<typename _tTransKey>
	auto
	find(const _tTransKey& x) -> decltype(tree.find_tr(x))
	{
		return tree.find_tr(x);
	}

	const_iterator
	find(const key_type& x) const
	{
		return tree.find(x);
	}

	template<typename _tTransKey>
	auto
	find(const _tTransKey& x) const -> decltype(tree.find_tr(x))
	{
		return tree.find_tr(x);
	}

	size_type
	count(const key_type& x) const
	{
		return tree.find(x) == tree.end() ? 0 : 1;
	}
	template<typename _tTransKey>
	auto
	count(const _tTransKey& x) const -> decltype(tree.count_tr(x))
	{
		return tree.count_tr(x);
	}

	iterator
	lower_bound(const key_type& x)
	{
		return tree.lower_bound(x);
	}
	template<typename _tTransKey>
	auto
	lower_bound(const _tTransKey& x)
		-> decltype(iterator(tree.lower_bound_tr(x)))
	{
		return iterator(tree.lower_bound_tr(x));
	}

	const_iterator
	lower_bound(const key_type& x) const
	{
		return tree.lower_bound(x);
	}
	template<typename _tTransKey>
	auto
	lower_bound(const _tTransKey& x) const
		-> decltype(const_iterator(tree.lower_bound_tr(x)))
	{
		return const_iterator(tree.lower_bound_tr(x));
	}

	iterator
	upper_bound(const key_type& x)
	{
		return tree.upper_bound(x);
	}
	template<typename _tTransKey>
	auto
	upper_bound(const _tTransKey& x)
		-> decltype(iterator(tree.upper_bound_tr(x)))
	{
		return iterator(tree.upper_bound_tr(x));
	}

	const_iterator
	upper_bound(const key_type& x) const
	{
		return tree.upper_bound(x);
	}

	template<typename _tTransKey>
	auto
	upper_bound(const _tTransKey& x) const
		-> decltype(const_iterator(tree.upper_bound_tr(x)))
	{
		return const_iterator(tree.upper_bound_tr(x));
	}

	std::pair<iterator, iterator>
	equal_range(const key_type& x)
	{
		return tree.equal_range(x);
	}
	template<typename _tTransKey>
	auto
	equal_range(const _tTransKey& x)
		-> decltype(std::pair<iterator, iterator>(tree.equal_range_tr(x)))
	{
		return std::pair<iterator, iterator>(tree.equal_range_tr(x));
	}
	std::pair<const_iterator, const_iterator>
	equal_range(const key_type& x) const
	{
		return tree.equal_range(x);
	}
	template<typename _tTransKey>
	auto
	equal_range(const _tTransKey& x) const -> decltype(
		std::pair<const_iterator, const_iterator>(tree.equal_range_tr(x)))
	{
		return std::pair<const_iterator, const_iterator>(
			tree.equal_range_tr(x));
	}

	friend bool
	operator==(const map& x, const map& y)
	{
		return x.tree == y.tree;
	}

	friend bool
	operator<(const map& x, const map& y)
	{
		return x.tree < y.tree;
	}
};

//! \relates map
template<typename _tKey, typename _type, typename _fComp, typename _tAlloc>
inline void
swap(map<_tKey, _type, _fComp, _tAlloc>& x,
	map<_tKey, _type, _fComp, _tAlloc>& y) ynoexcept_spec(noexcept(x.swap(y)))
{
	x.swap(y);
}

namespace details
{

inline namespace rb_tree
{

//! \relates map
template<typename _tKey, typename _type, typename _fComp1, typename _tAlloc,
	typename _fComp2>
struct tree_merge_helper<ystdex::map<_tKey, _type, _fComp1, _tAlloc>, _fComp2>
{
private:
	friend class ystdex::map<_tKey, _type, _fComp1, _tAlloc>;

	static auto
	get_tree(ystdex::map<_tKey, _type, _fComp2, _tAlloc>& m)
		-> decltype((m.tree))
	{
		return m.tree;
	}
	static auto
	get_tree(ystdex::multimap<_tKey, _type, _fComp2, _tAlloc>& m)
		-> decltype((m.tree))
	{
		return m.tree;
	}
};

} // inline namespace rb_tree;

} // namespace details;
//@}

} // namespace ystdex;

#endif

