﻿/*
	© 2018-2019, 2021-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file map.hpp
\ingroup YStandardEx
\brief 映射容器。
\version r1454
\author FrankHB <frankhb1989@gmail.com>
\since build 830
\par 创建时间:
	2018-07-06 21:12:51 +0800
\par 修改时间:
	2023-02-02 03:18 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Map

提供 ISO C++17 标准库头 \c \<map> 兼容的替代接口和实现。
包括以下已有其它实现支持的 ISO C++20 后的修改：
LWG 2839 ：允许自转移赋值。
包括以下扩展：
允许不完整类型作为 map 的模板参数。
*/


#ifndef YB_INC_ystdex_map_hpp_
#define YB_INC_ystdex_map_hpp_ 1

#include "tree.h" // for "tree.h" (implying "range.hpp"),
//	__cpp_lib_allocator_traits_is_always_equal, less, std::pair, std::allocator,
//	totally_ordered, allocator_traits, first_of, YAssert, is_constructible,
//	enable_if_t, enable_if_constructible_r_t, ystdex::swap_dependent;
#include <map> // for <map>, __cpp_lib_generic_associative_lookup,
//	__cpp_lib_map_try_emplace, __cpp_lib_node_extract, std::initializer_list;
#include <tuple> // for std::piecewise_construct, std::tuple;
#include "container.hpp" // for ystdex::try_emplace, ystdex::insert_or_assign,
//	ystdex::emplace_hint_in_place;

namespace ystdex
{

//! \since build 830
//!@{
#if false
// NOTE: For exposition only. Since %ystdex::map has incomplete type support
//	which is not guaranteed by ISO C++17 (even if guaranteed by libstdc++), it
//	should not be in %cpp2017 inline namespace.
#if (__cpp_lib_generic_associative_lookup >= 201304L \
	|| __cplusplus >= 201402L) && ((__cpp_lib_allocator_traits_is_always_equal
	>= 201411L && __cpp_lib_map_try_emplace >= 201411L \
	&& __cpp_lib_node_extract >= 201606L) || __cplusplus >= 201606L)
#	define YB_Has_Cpp17_map true
#else
#	define YB_Has_Cpp17_map false
#endif
#undef YB_Has_Cpp17_map
#endif

//! \todo 在其它头文件中提供实现。
template<typename, typename, typename, class>
class multimap;

/*!
\ingroup YBase_replacement_features
\brief 映射容器。
\warning 非虚析构。
\see Documentation::YBase @2.1.4.1 。
\see Documentation::YBase @2.1.4.2 。

类似 ISO C++17 的 std::map 的容器，但支持不完整类型作为键和被映射的类型。
支持 WG21 P0458R2 。
不完整类型的支持条件同 WG21 N4510 ，除对类型完整地要求扩展到键和映射的类型。
部分成员提供较 ISO C++17 更强的 noexcept 异常规范，除转移赋值略有不同。
*/
template<typename _tKey, typename _tMapped, typename _fComp = less<_tKey>,
	class _tAlloc = std::allocator<std::pair<const _tKey, _tMapped>>>
class map : private totally_ordered<map<_tKey, _tMapped, _fComp, _tAlloc>>
{
	template<typename, typename>
	friend struct details::rb_tree::tree_merge_helper;

public:
	using key_type = _tKey;
	using mapped_type = _tMapped;
	using value_type = std::pair<const _tKey, _tMapped>;
	// NOTE: Checks for %value_type and %_tAlloc is in instantiation of
	//	%rep_type below.
	using key_compare = _fComp;
	using allocator_type = _tAlloc;
	class value_compare
	{
		friend class map<_tKey, _tMapped, _fComp, _tAlloc>;

	protected:
		_fComp comp;

		value_compare(_fComp c) : comp(c)
		{}

	public:
		YB_ATTR_nodiscard YB_PURE bool
		operator()(const value_type& x, const value_type& y) const
		{
			return comp(x.first, y.first);
		}
	};

private:
	//! \since build 864
	using ator_traits = allocator_traits<allocator_type>;
	using rep_type = details::rb_tree::tree<key_type, value_type, first_of<>,
		key_compare, allocator_type>;

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
	//!@{
	using iterator = typename rep_type::iterator;
	using const_iterator = typename rep_type::const_iterator;
	//!@}
	using reverse_iterator = typename rep_type::reverse_iterator;
	using const_reverse_iterator = typename rep_type::const_reverse_iterator;
	using node_type = typename rep_type::node_type;
	using insert_return_type = typename rep_type::insert_return_type;

private:
	rep_type tree;

public:
	//! \see LWG 2193 。
	map() yimpl(= default);
	explicit
	map(const allocator_type& a)
		: tree(a)
	{}
	explicit
	map(const _fComp& comp, const allocator_type& a = allocator_type())
		: tree(comp, a)
	{}
	template<typename _tIn>
	inline
	map(_tIn first, _tIn last)
		: tree()
	{
		tree.insert_range_unique(first, last);
	}
	template<typename _tIn>
	inline
	map(_tIn first, _tIn last, const allocator_type& a)
		: tree(_fComp(), a)
	{
		tree.insert_range_unique(first, last);
	}
	template<typename _tIn>
	inline
	map(_tIn first, _tIn last, const _fComp& comp,
		const allocator_type& a = allocator_type())
		: tree(comp, a)
	{
		tree.insert_range_unique(first, last);
	}
	map(std::initializer_list<value_type> il, const _fComp& comp = _fComp(),
		const allocator_type& a = allocator_type())
		: tree(comp, allocator_type(a))
	{
		tree.insert_range_unique(il.begin(), il.end());
	}
	map(std::initializer_list<value_type> il, const allocator_type& a)
		: tree(_fComp(), a)
	{
		tree.insert_range_unique(il.begin(), il.end());
	}
	map(const map&) yimpl(= default);
	map(const map& m, const allocator_type& a)
		: tree(m.tree, a)
	{}
	// XXX: The exception specification is strengthened to the noexcept
	//	specification of the copy constructor of the comparison object. In ISO
	//	C++17 there is no explicit exception specification.
	map(map&&) yimpl(= default);
	// XXX: The exception specification is strengthened to noexcept
	//	specification having the operand equivalent to the conjunction of
	//	%std::allocator_traits::is_always_equal trait of the internal node and
	//	'is_nothrow_copy_constructible<_fComp>'. In ISO C++17 there is
	//	no explicit exception specification.
	map(map&& m, const allocator_type& a)
		yimpl(ynoexcept(is_nothrow_constructible<
		rep_type, rep_type&&, const allocator_type&>()))
		: tree(std::move(m.tree), a)
	{}
	~map() yimpl(= default);

	map&
	operator=(const map&) yimpl(= default);
	// XXX: The exception specification is changed. Since ISO C++17 (by adoption
	//	of WG21 N4258) the standardonly requires conditional non-throwing
	//	exception specification when the allocator meets
	//	%std::allocator_traits<allocator_type>::is_always_equal and the
	//	comparison object type meets %std::is_nothrow_move_assignable, with
	//	regardless to %propagate_on_container_move_assignment of the node
	//	allocator. Here the %allocator_traits of the internal node is also used
	//	instead. See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91541 and LWG
	//	3267.
	//! \see WG21 N4258 。
	map&
	operator=(map&&) yimpl(= default);
	map&
	operator=(std::initializer_list<value_type> il)
	{
		tree.assign_unique(il.begin(), il.end());
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const map& x, const map& y)
	{
		return x.tree == y.tree;
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator<(const map& x, const map& y)
	{
		return x.tree < y.tree;
	}

	YB_ATTR_nodiscard YB_PURE allocator_type
	get_allocator() const ynothrow
	{
		return tree.get_allocator();
	}

	YB_ATTR_nodiscard YB_PURE iterator
	begin() ynothrow
	{
		return tree.begin();
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	begin() const ynothrow
	{
		return tree.begin();
	}

	YB_ATTR_nodiscard YB_PURE iterator
	end() ynothrow
	{
		return tree.end();
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	end() const ynothrow
	{
		return tree.end();
	}

	YB_ATTR_nodiscard YB_PURE reverse_iterator
	rbegin() ynothrow
	{
		return tree.rbegin();
	}
	YB_ATTR_nodiscard YB_PURE const_reverse_iterator
	rbegin() const ynothrow
	{
		return tree.rbegin();
	}

	YB_ATTR_nodiscard YB_PURE reverse_iterator
	rend() ynothrow
	{
		return tree.rend();
	}
	YB_ATTR_nodiscard YB_PURE const_reverse_iterator
	rend() const ynothrow
	{
		return tree.rend();
	}

	YB_ATTR_nodiscard YB_PURE const_iterator
	cbegin() const ynothrow
	{
		return tree.begin();
	}

	YB_ATTR_nodiscard YB_PURE const_iterator
	cend() const ynothrow
	{
		return tree.end();
	}

	YB_ATTR_nodiscard YB_PURE const_reverse_iterator
	crbegin() const ynothrow
	{
		return tree.rbegin();
	}

	YB_ATTR_nodiscard YB_PURE const_reverse_iterator
	crend() const ynothrow
	{
		return tree.rend();
	}

	YB_ATTR_nodiscard YB_PURE bool
	empty() const ynothrow
	{
		return tree.empty();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	size() const ynothrow
	{
		return tree.size();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	max_size() const ynothrow
	{
		return tree.max_size();
	}

	mapped_type&
	operator[](const key_type& x)
	{
		return try_emplace(x).first->second;
	}
	mapped_type&
	operator[](key_type&& x)
	{
		return try_emplace(std::move(x)).first->second;
	}

	YB_ATTR_nodiscard YB_PURE mapped_type&
	at(const key_type& x)
	{
		return at_impl(*this, x, lower_bound(x));
	}
	YB_ATTR_nodiscard YB_PURE const mapped_type&
	at(const key_type& x) const
	{
		return at_impl(*this, x, lower_bound(x));
	}

private:
	template<class _tClass, typename _tFwd>
	YB_ATTR_nodiscard YB_PURE static auto
	at_impl(_tClass&& m, const key_type& k, _tFwd i) -> decltype(((*i).second))
	{
		if(i == m.end() || m.key_comp()(k, (*i).first))
			// XXX: Is it needed to extract and put into a function?
			throw std::out_of_range("map::at");
		return (*i).second;
	}

public:
	template<typename... _tParams>
	inline std::pair<iterator, bool>
	emplace(_tParams&&... args)
	{
		return tree.emplace_unique(yforward(args)...);
	}

	template<typename... _tParams>
	inline iterator
	emplace_hint(const_iterator position, _tParams&&... args)
	{
		return tree.emplace_hint_unique(position, yforward(args)...);
	}

	std::pair<iterator, bool>
	insert(const value_type& x)
	{
		return tree.insert_unique(x);
	}
	//! \see LWG 2354 。
	std::pair<iterator, bool>
	insert(value_type&& x)
	{
		return tree.insert_unique(std::move(x));
	}
	//! \see LWG 2005 。
	template<typename _tPair>
	inline yimpl(enable_if_constructible_r_t)<
		std::pair<iterator YPP_Comma bool>, value_type, _tPair>
	insert(_tPair&& x)
	{
		return tree.emplace_unique(yforward(x));
	}
	iterator
	insert(const_iterator position, const value_type& x)
	{
		return tree.insert_hint_unique(position, x);
	}
	//! \see LWG 2354 。
	iterator
	insert(const_iterator position, value_type&& x)
	{
		return tree.insert_hint_unique(position, std::move(x));
	}
	//! \see LWG 2005 。
	template<typename _tPair>
	inline yimpl(enable_if_constructible_r_t)<iterator, value_type, _tPair>
	insert(const_iterator position, _tPair&& x)
	{
		return tree.emplace_hint_unique(position, yforward(x));
	}
	//! \see LWG 2571 。
	template<typename _tIn>
	void
	insert(_tIn first, _tIn last)
	{
		tree.insert_range_unique(first, last);
	}
	void
	insert(std::initializer_list<value_type> il)
	{
		insert(il.begin(), il.end());
	}

	node_type
	extract(const_iterator position)
	{
		YAssert(position != end(), "Invalid iterator value found.");
		return tree.extract(position);
	}
	node_type
	extract(const key_type& x)
	{
		return tree.extract(x);
	}

	//! \pre 间接断言：若最后一个参数非空，分配器和参数指定的容器分配器相等。
	//!@{
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
	//!@}

	//! \see WG21 N4279 。
	//!@{
	template<typename... _tParams>
	inline std::pair<iterator, bool>
	try_emplace(const key_type& k, _tParams&&... args)
	{
		return ystdex::try_emplace(*this, k, yforward(args)...);
	}
	template<typename... _tParams>
	inline std::pair<iterator, bool>
	try_emplace(key_type&& k, _tParams&&... args)
	{
		return ystdex::try_emplace(*this, k, yforward(args)...);
	}
	template<typename... _tParams>
	inline iterator
	try_emplace(const_iterator hint, const key_type& k, _tParams&&... args)
	{
		return try_emplace_hint_impl(hint, k, yforward(args)...);
	}
	template<typename... _tParams>
	inline iterator
	try_emplace(const_iterator hint, key_type&& k, _tParams&&... args)
	{
		return try_emplace_hint_impl(hint, std::move(k), yforward(args)...);
	}

private:
	//! \since build 843
	template<typename _tParam, typename... _tParams>
	iterator
	try_emplace_hint_impl(const_iterator hint, _tParam&& k, _tParams&&... args)
	{
		// NOTE: The following code with %ystdex::try_emplace_hint is not used
		//	because there is more specific internal method to deal with internal
		//	knowledge of node pointers of underlying tree and case for %cend(),
		//	i.e. in the case where %hint equals to %cend(), the rightmost node
		//	of %rb_tree::tree can be get without a tree decrement on the
		//	iterator.
#if false
		return
			ystdex::try_emplace_hint(*this, hint, k, yforward(args)...).first;
#else
		const auto pr(tree.get_insert_hint_unique_pos(hint, k));

		return pr.second ? emplace_hint(iterator(pr.second),
			std::piecewise_construct, std::forward_as_tuple(yforward(k)),
			std::forward_as_tuple(yforward(args)...))
			: iterator(pr.first);
#endif
	}

public:
	template<typename _tObj>
	inline std::pair<iterator, bool>
	insert_or_assign(const key_type& k, _tObj&& obj)
	{
		return ystdex::insert_or_assign(*this, k, yforward(obj));
	}
	template<typename _tObj>
	inline std::pair<iterator, bool>
	insert_or_assign(key_type&& k, _tObj&& obj)
	{
		return ystdex::insert_or_assign(*this, std::move(k), yforward(obj));
	}
	template<typename _tObj>
	inline iterator
	insert_or_assign(const_iterator hint, const key_type& k, _tObj&& obj)
	{
		return insert_or_assign_hint_impl(hint, k, yforward(obj));
	}
	template<typename _tObj>
	inline iterator
	insert_or_assign(const_iterator hint, key_type&& k, _tObj&& obj)
	{
		return insert_or_assign_hint_impl(hint, std::move(k), yforward(obj));
	}

private:
	//! \since build 843
	template<typename _tParam, typename _tObj>
	iterator
	insert_or_assign_hint_impl(const_iterator hint, _tParam&& k, _tObj&& obj)
	{
		// NOTE: Similar to %try_emplace_hint_impl.
		const auto pr(tree.get_insert_hint_unique_pos(hint, k));

		if(pr.second)
			return ystdex::emplace_hint_in_place(*this, iterator(pr.second),
				yforward(k), yforward(obj));

		const iterator i(pr.first);

		(*i).second = yforward(obj);
		return i;
	}
	//!@}

public:
	//! \see LWG 2059 。
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

	// XXX: The exception specification is strengthened. Since ISO C++17 (by
	//	adoption of WG21 N4258) the standard only requires conditional
	//	non-throwing exception specification when the allocator also meets
	//	%std::allocator_traits<allocator_type>::is_always_equal.
	/*!
	\see WG21 N4258 。
	\since build 864
	*/
	void
	swap(map& x) ynoexcept(yimpl(is_nothrow_swappable<_fComp>()))
	{
		ystdex::swap_dependent(tree, x.tree);
	}
	//! \since build 864
	friend void
	swap(map& x, map& y) ynoexcept_spec(x.swap(y))
	{
		x.swap(y);
	}

	void
	clear() ynothrow
	{
		tree.clear();
	}

	//! \pre 间接断言：分配器和参数指定的容器分配器相等。
	//!@{
	template<typename _tCon2>
	inline void
	merge(map<_tKey, _tMapped, _tCon2, _tAlloc>& source)
	{
		tree.merge_unique(
			details::rb_tree::tree_merge_helper<map, _tCon2>::get_tree(source));
	}
	template<typename _tCon2>
	inline void
	merge(map<_tKey, _tMapped, _tCon2, _tAlloc>&& source)
	{
		merge(source);
	}
	template<typename _tCon2>
	inline void
	merge(multimap<_tKey, _tMapped, _tCon2, _tAlloc>& source)
	{
		tree.merge_unique(
			details::rb_tree::tree_merge_helper<map, _tCon2>::get_tree(source));
	}
	template<typename _tCon2>
	inline void
	merge(multimap<_tKey, _tMapped, _tCon2, _tAlloc>&& source)
	{
		merge(source);
	}
	//!@}

	YB_ATTR_nodiscard YB_PURE key_compare
	key_comp() const
	{
		return tree.key_comp();
	}

	YB_ATTR_nodiscard YB_PURE value_compare
	value_comp() const
	{
		return value_compare(tree.key_comp());
	}

#define YB_Impl_Map_GenericLookupHead(_n) \
	template<typename _tTransKey> \
	YB_ATTR_nodiscard YB_PURE inline auto \
	_n(const _tTransKey& x)

	YB_ATTR_nodiscard YB_PURE iterator
	find(const key_type& x)
	{
		return tree.find(x);
	}
	YB_Impl_Map_GenericLookupHead(find) -> decltype(tree.find_tr(x))
	{
		return tree.find_tr(x);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	find(const key_type& x) const
	{
		return tree.find(x);
	}
	YB_Impl_Map_GenericLookupHead(find) const -> decltype(tree.find_tr(x))
	{
		return tree.find_tr(x);
	}

	YB_ATTR_nodiscard YB_PURE size_type
	count(const key_type& x) const
	{
		return contains(x) ? 1 : 0;
	}
	YB_Impl_Map_GenericLookupHead(count) const -> decltype(tree.count_tr(x))
	{
		// XXX: This cannot just use %find instead. See https://github.com/cplusplus/papers/issues/1037#issuecomment-960066305.
		return tree.count_tr(x);
	}

	/*!
	\see WG21 P0458R2 。
	\since build 866
	*/
	//!@{
	YB_ATTR_nodiscard YB_PURE bool
	contains(const key_type& x) const
	{
		return tree.find(x) != tree.end();
	}
	YB_Impl_Map_GenericLookupHead(contains) const
		-> decltype(void(tree.find_tr(x)), true)
	{
		return tree.find_tr(x) != tree.end();
	}
	//!@}

	YB_ATTR_nodiscard YB_PURE iterator
	lower_bound(const key_type& x)
	{
		return tree.lower_bound(x);
	}
	YB_Impl_Map_GenericLookupHead(lower_bound)
		-> decltype(iterator(tree.lower_bound_tr(x)))
	{
		return iterator(tree.lower_bound_tr(x));
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	lower_bound(const key_type& x) const
	{
		return tree.lower_bound(x);
	}
	YB_Impl_Map_GenericLookupHead(lower_bound) const
		-> decltype(const_iterator(tree.lower_bound_tr(x)))
	{
		return const_iterator(tree.lower_bound_tr(x));
	}

	YB_ATTR_nodiscard YB_PURE iterator
	upper_bound(const key_type& x)
	{
		return tree.upper_bound(x);
	}
	YB_Impl_Map_GenericLookupHead(upper_bound)
		-> decltype(iterator(tree.upper_bound_tr(x)))
	{
		return iterator(tree.upper_bound_tr(x));
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	upper_bound(const key_type& x) const
	{
		return tree.upper_bound(x);
	}
	YB_Impl_Map_GenericLookupHead(upper_bound) const
		-> decltype(const_iterator(tree.upper_bound_tr(x)))
	{
		return const_iterator(tree.upper_bound_tr(x));
	}

	YB_ATTR_nodiscard YB_PURE std::pair<iterator, iterator>
	equal_range(const key_type& x)
	{
		return tree.equal_range(x);
	}
	YB_Impl_Map_GenericLookupHead(equal_range)
		-> decltype(std::pair<iterator, iterator>(tree.equal_range_tr(x)))
	{
		return std::pair<iterator, iterator>(tree.equal_range_tr(x));
	}
	YB_ATTR_nodiscard YB_PURE std::pair<const_iterator, const_iterator>
	equal_range(const key_type& x) const
	{
		return tree.equal_range(x);
	}
	YB_Impl_Map_GenericLookupHead(equal_range) const -> decltype(
		std::pair<const_iterator, const_iterator>(tree.equal_range_tr(x)))
	{
		return
			std::pair<const_iterator, const_iterator>(tree.equal_range_tr(x));
	}

#undef YB_Impl_Map_GenericLookupHead
};

namespace details
{

inline namespace rb_tree
{

//! \relates map
template<typename _tKey, typename _tMapped, typename _fComp1, typename _tAlloc,
	typename _fComp2>
struct
	tree_merge_helper<ystdex::map<_tKey, _tMapped, _fComp1, _tAlloc>, _fComp2>
{
private:
	friend class ystdex::map<_tKey, _tMapped, _fComp1, _tAlloc>;

	YB_ATTR_nodiscard YB_PURE static auto
	get_tree(map<_tKey, _tMapped, _fComp2, _tAlloc>& m) ynothrow
		-> decltype((m.tree))
	{
		return m.tree;
	}
	// XXX: Disabled to prevent ill-formed instantiation of incomplete types in
	//	the trailing return type.
	// TODO: Use complete %multimap implementation.
#if false
	YB_ATTR_nodiscard YB_PURE static auto
	get_tree(multimap<_tKey, _tMapped, _fComp2, _tAlloc>& m) ynothrow
		-> decltype((m.tree))
	{
		return m.tree;
	}
#endif
};

} // inline namespace rb_tree;

} // namespace details;
//!@}

} // namespace ystdex;

#endif

