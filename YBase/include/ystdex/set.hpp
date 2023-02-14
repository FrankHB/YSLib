/*
	© 2016-2019, 2021, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file set.hpp
\ingroup YStandardEx
\brief 集合容器。
\version r2073
\author FrankHB <frankhb1989@gmail.com>
\since build 665
\par 创建时间:
	2016-01-23 20:13:53 +0800
\par 修改时间:
	2023-02-13 20:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Set

提供 ISO C++17 标准库头 \c \<set> 兼容的替代接口和实现。
包括以下已有其它实现支持的 ISO C++20 后的修改：
LWG 2839 ：允许自转移赋值。
包括以下扩展：
允许不完整类型作为 map 的模板参数。
同时提供基于 map 的 set 扩展。
*/


#ifndef YB_INC_ystdex_set_hpp_
#define YB_INC_ystdex_set_hpp_ 1

#include "map.hpp" // for "tree.h" (implying "range.hpp"),
//	__cpp_lib_allocator_traits_is_always_equal, less, std::allocator,
//	allocator_traits, id, std::move, lref, map, rebind_alloc_t,
//	ystdex::emplace_hint_in_place, ystdex::as_const, ystdex::search_map_by,
//	enable_if_transparent_t;
#include <set> // for <set>, __cpp_lib_generic_associative_lookup,
//	__cpp_lib_node_extract, std::initializer_list;
#include "iterator.hpp" // for transformed_iterator,
//	iterator_transformation::second, ystdex::reverse_iterator,
//	ystdex::make_transform, index_sequence, std::tuple, std::forward_as_tuple,
//	std::get;

namespace ystdex
{

//! \since build 967
//!@{
#if false
// NOTE: For exposition only. Since %ystdex::set has incomplete type support
//	which is not guaranteed by ISO C++17 (even if guaranteed by libstdc++), it
//	should not be in %cpp2017 inline namespace.
#if (__cpp_lib_generic_associative_lookup >= 201304L \
	|| __cplusplus >= 201402L) && ((__cpp_lib_allocator_traits_is_always_equal
	>= 201411L \
	&& __cpp_lib_node_extract >= 201606L) || __cplusplus >= 201606L)
#	define YB_Has_Cpp17_set true
#else
#	define YB_Has_Cpp17_set false
#endif
#undef YB_Has_Cpp17_set
#endif

//! \todo 在其它头文件中提供实现。
template<typename, typename, class>
class multiset;

/*!
\ingroup YBase_replacement_features
\brief 集合容器。
\warning 非虚析构。
\see Documentation::YBase @2.1.4.1 。
\see Documentation::YBase @2.1.4.2 。

类似 ISO C++17 的 std::set 的容器，但支持不完整类型作为键的类型。
支持 WG21 P0458R2 。
不完整类型的支持条件同 WG21 N4510 ，除对类型完整地要求扩展到键的类型。
部分成员提供较 ISO C++17 更强的 noexcept 异常规范，除转移赋值略有不同。
*/
template<typename _tKey, typename _fComp = less<_tKey>,
	class _tAlloc = std::allocator<_tKey>>
class set : private totally_ordered<set<_tKey, _fComp, _tAlloc>>
{
	template<typename, typename>
	friend struct details::rb_tree::tree_merge_helper;

public:
	using key_type = _tKey;
	using key_compare = _fComp;
	using value_type = _tKey;
	using value_compare = _fComp;
	// NOTE: Checks for %value_type and %_tAlloc is in instantiation of
	//	%rep_type below.
	using allocator_type = _tAlloc;

private:
	//! \since build 864
	using ator_traits = allocator_traits<allocator_type>;
	using rep_type = details::rb_tree::tree<key_type, value_type, id<>,
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
	set() yimpl(= default);
	explicit
	set(const allocator_type& a)
		: tree(a)
	{}
	explicit
	set(const _fComp& comp, const allocator_type& a = allocator_type())
		: tree(comp, a)
	{}
	template<typename _tIn>
	inline
	set(_tIn first, _tIn last)
		: tree()
	{
		tree.insert_range_unique(first, last);
	}
	template<typename _tIn>
	inline
	set(_tIn first, _tIn last, const allocator_type& a)
		: tree(_fComp(), a)
	{
		tree.insert_range_unique(first, last);
	}
	template<typename _tIn>
	inline
	set(_tIn first, _tIn last, const _fComp& comp,
		const allocator_type& a = allocator_type())
		: tree(comp, a)
	{
		tree.insert_range_unique(first, last);
	}
	set(std::initializer_list<value_type> il, const _fComp& comp = _fComp(),
		const allocator_type& a = allocator_type())
		: tree(comp, allocator_type(a))
	{
		tree.insert_range_unique(il.begin(), il.end());
	}
	set(std::initializer_list<value_type> il, const allocator_type& a)
		: tree(_fComp(), a)
	{
		tree.insert_range_unique(il.begin(), il.end());
	}
	set(const set&) yimpl(= default);
	set(const set& s, const allocator_type& a)
		: tree(s.tree, a)
	{}
	// XXX: The exception specification is strengthened. See the comment for the
	//	move constructor of %ystdex::map.
	set(set&&) yimpl(= default);
	set(set&& s, const allocator_type& a)
		yimpl(ynoexcept(is_nothrow_constructible<
		rep_type, rep_type&&, const allocator_type&>()))
		: tree(std::move(s.tree), a)
	{}
	~set() yimpl(= default);

	set&
	operator=(const set&) yimpl(= default);
	// XXX: The exception specification is changed. See the comment for the move
	//	assignment operator of %ystdex::map.
	//! \see WG21 N4258 。
	set&
	operator=(set&&) yimpl(= default);
	set&
	operator=(std::initializer_list<value_type> il)
	{
		tree.assign_unique(il.begin(), il.end());
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const set& x, const set& y)
	{
		return x.tree == y.tree;
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator<(const set& x, const set& y)
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

	// XXX: The exception specification is strengthened. See the comment for the
	//	member function %swap of %ystdex::map.
	//! \see WG21 N4258 。
	void
	swap(set& x) ynoexcept(yimpl(is_nothrow_swappable<_fComp>()))
	{
		ystdex::swap_dependent(tree, x.tree);
	}
	friend void
	swap(set& x, set& y) ynoexcept_spec(x.swap(y))
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
	merge(set<_tKey, _tCon2, _tAlloc>& source)
	{
		tree.merge_unique(
			details::rb_tree::tree_merge_helper<set, _tCon2>::get_tree(source));
	}
	template<typename _tCon2>
	inline void
	merge(set<_tKey, _tCon2, _tAlloc>&& source)
	{
		merge(source);
	}
	template<typename _tCon2>
	inline void
	merge(multiset<_tKey, _tCon2, _tAlloc>& source)
	{
		tree.merge_unique(
			details::rb_tree::tree_merge_helper<set, _tCon2>::get_tree(source));
	}
	template<typename _tCon2>
	inline void
	merge(multiset<_tKey, _tCon2, _tAlloc>&& source)
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

#define YB_Impl_Set_GenericLookupHead(_n) \
	template<typename _tTransKey> \
	YB_ATTR_nodiscard YB_PURE inline auto \
	_n(const _tTransKey& x)

	YB_ATTR_nodiscard YB_PURE iterator
	find(const key_type& x)
	{
		return tree.find(x);
	}
	YB_Impl_Set_GenericLookupHead(find) -> decltype(tree.find_tr(x))
	{
		return tree.find_tr(x);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	find(const key_type& x) const
	{
		return tree.find(x);
	}
	YB_Impl_Set_GenericLookupHead(find) const -> decltype(tree.find_tr(x))
	{
		return tree.find_tr(x);
	}

	YB_ATTR_nodiscard YB_PURE size_type
	count(const key_type& x) const
	{
		return contains(x) ? 1 : 0;
	}
	YB_Impl_Set_GenericLookupHead(count) const -> decltype(tree.count_tr(x))
	{
		// XXX: This cannot just use %find instead. See https://github.com/cplusplus/papers/issues/1037#issuecomment-960066305.
		return tree.count_tr(x);
	}

	//! \see WG21 P0458R2 。
	//!@{
	YB_ATTR_nodiscard YB_PURE bool
	contains(const key_type& x) const
	{
		return tree.find(x) != tree.end();
	}
	YB_Impl_Set_GenericLookupHead(contains) const
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
	YB_Impl_Set_GenericLookupHead(lower_bound)
		-> decltype(iterator(tree.lower_bound_tr(x)))
	{
		return iterator(tree.lower_bound_tr(x));
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	lower_bound(const key_type& x) const
	{
		return tree.lower_bound(x);
	}
	YB_Impl_Set_GenericLookupHead(lower_bound) const
		-> decltype(const_iterator(tree.lower_bound_tr(x)))
	{
		return const_iterator(tree.lower_bound_tr(x));
	}

	YB_ATTR_nodiscard YB_PURE iterator
	upper_bound(const key_type& x)
	{
		return tree.upper_bound(x);
	}
	YB_Impl_Set_GenericLookupHead(upper_bound)
		-> decltype(iterator(tree.upper_bound_tr(x)))
	{
		return iterator(tree.upper_bound_tr(x));
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	upper_bound(const key_type& x) const
	{
		return tree.upper_bound(x);
	}
	YB_Impl_Set_GenericLookupHead(upper_bound) const
		-> decltype(const_iterator(tree.upper_bound_tr(x)))
	{
		return const_iterator(tree.upper_bound_tr(x));
	}

	YB_ATTR_nodiscard YB_PURE std::pair<iterator, iterator>
	equal_range(const key_type& x)
	{
		return tree.equal_range(x);
	}
	YB_Impl_Set_GenericLookupHead(equal_range)
		-> decltype(std::pair<iterator, iterator>(tree.equal_range_tr(x)))
	{
		return std::pair<iterator, iterator>(tree.equal_range_tr(x));
	}
	YB_ATTR_nodiscard YB_PURE std::pair<const_iterator, const_iterator>
	equal_range(const key_type& x) const
	{
		return tree.equal_range(x);
	}
	YB_Impl_Set_GenericLookupHead(equal_range) const -> decltype(
		std::pair<const_iterator, const_iterator>(tree.equal_range_tr(x)))
	{
		return
			std::pair<const_iterator, const_iterator>(tree.equal_range_tr(x));
	}

#undef YB_Impl_Set_GenericLookupHead
};

namespace details
{

inline namespace rb_tree
{

//! \relates set
template<typename _tKey, typename _fComp1, typename _tAlloc, typename _fComp2>
struct
	tree_merge_helper<ystdex::set<_tKey, _fComp1, _tAlloc>, _fComp2>
{
private:
	friend class ystdex::set<_tKey, _fComp1, _tAlloc>;

	YB_ATTR_nodiscard YB_PURE static auto
	get_tree(set<_tKey, _fComp2, _tAlloc>& s) ynothrow -> decltype((s.tree))
	{
		return s.tree;
	}
	// XXX: Disabled to prevent ill-formed instantiation of incomplete types in
	//	the trailing return type.
	// TODO: Use complete %multiset implementation.
#if false
	YB_ATTR_nodiscard YB_PURE static auto
	get_tree(multiset<_tKey, _fComp2, _tAlloc>& s) ynothrow
		-> decltype((s.tree))
	{
		return s.tree;
	}
#endif
};

} // inline namespace rb_tree;

} // namespace details;
//!@}


#if false
// NOTE: For exposition effectively. This is always true now since %ystdex::map
//	is used. See the old revision for fallback implementations which is cleanup
//	away now (with %YB_Impl_Set_UseGenericLookup).
// XXX: G++ 5.2.0 rejects generic associative lookup in debug mode of %std::map.
// TODO: Find precise version supporting debug mode.
#if ((__cpp_lib_generic_associative_lookup >= 201304L \
	|| __cplusplus >= 201402L) && !(defined(__GLIBCXX__) \
	&& defined(_GLIBCXX_DEBUG)))
#	define YB_Impl_MappedSet_UseGenericLookup true
#else
#	define YB_Impl_MappedSet_UseGenericLookup false
#endif
#endif

//! \since build 679
namespace details
{

//! \since build 680
template<typename _tWrappedKey, typename _fComp,
	bool = has_mem_is_transparent<_fComp>{}>
struct tcompare
{
	_fComp comp{};

	//! \since build 843
	tcompare() = default;
	tcompare(_fComp c)
		: comp(std::move(c))
	{}

	bool
	operator()(const _tWrappedKey& x, const _tWrappedKey& y) const
	{
		return bool(comp(x.get(), y.get()));
	}
};

//! \since build 680
template<typename _tWrappedKey, typename _fComp>
struct tcompare<_tWrappedKey, _fComp, true>
	: private tcompare<_tWrappedKey, _fComp, false>
{
private:
	using base = tcompare<_tWrappedKey, _fComp, false>;

public:
	using is_transparent = yimpl(void);

	using base::comp;

#if YB_IMPL_CLANGPP
	// NOTE: See https://llvm.org/bugs/show_bug.cgi?id=27504.
	using tcompare<_tWrappedKey, _fComp, false>::tcompare;
#else
	using base::tcompare;
#endif

	using base::operator();
	template<typename _tKey>
	bool
	operator()(const _tWrappedKey& x, const _tKey& y) const
	{
		return bool(comp(x.get(), y));
	}
	template<typename _tKey>
	bool
	operator()(const _tKey& x, const _tWrappedKey& y) const
	{
		return bool(comp(x, y.get()));
	}
};


template<typename _type>
class wrapped_key
{
public:
	using key_type = _type;
	using wrapper = lref<const key_type>;

private:
	mutable wrapper k;

public:
	wrapped_key(const key_type& key)
		: k(key)
	{}

	void
	amend(const _type& key) const ynothrow
	{
		k = wrapper(key);
	}

	const _type&
	get() const ynothrow
	{
		return k;
	}
};

} // namespace details;


/*!
\ingroup customization_points
\brief 映射值集合特征。
\since build 844

集合类型底层使用 map 容器，并在修改容器时同时修改键但始终保持键的等价关系。
为有效使用这些操作而不受 const 键的限制，使用特征扩展部分操作。
用于 emplace 和 emplace_hint 的合式要求和作用包括：
	\c get_value_key<key_type>(yforward(args)...) 合式，
		其结果作为用于插入的键，是新创建的值或已有值的引用 \c k 。
	若需以键 \c k 插入值，<tt>extend_key(yforward(k), *this)</tt> 合式。
	对 key_type 类型的 \c get_value_key 实现应支持任意能被 \c emplace
		和 \c emplace_hint 的参数组合，并返回适当的蕴含 key_type 比较的类型。
用于 \c insert 的 ADL 的合式要求和作用包括：
	对被插入的元素 \c v ，\c set_value_move(v) 合式，
		以保持键等价关系的方式转移资源。
*/
template<typename _type>
struct mapped_set_traits
{
	//! \note 覆盖实现可直接返回右值。
	//!@{
	/*!
	\brief 从参数指定的透明比较的键转换为用于插入容器值的键的值。
	\return 键的引用。
	*/
	template<typename _tKey, class _tCon>
	static yconstfn _tKey&&
	extend_key(_tKey&& k, _tCon&&) ynothrow
	{
		return yforward(k);
	}

	//! \brief 从可选的参数中取键的值。
	//!@{
	static yconstfn auto
	get_value_key() ynothrow -> decltype(_type())
	{
		return _type();
	}
	template<typename _tKey, typename... _tParams>
	static yconstfn const _tKey&
	get_value_key(const _tKey& x, _tParams&&...) ynothrow
	{
		return x;
	}
	//!@}
	//!@}

	/*!
	\brief 恢复元素的键。
	\since build 845
	*/
	template<typename _tParam>
	static yconstfn_relaxed void
	restore_key(_type&, _tParam&&) ynothrow
	{}

	/*!
	\brief 转移集合元素。
	\post 转移后元素和参数比较相等。
	*/
	template<typename _tParam>
	static yconstfn const _tParam&
	set_value_move(const _tParam& x) ynothrow
	{
		return x;
	}
};


/*!
\brief 允许指定不同于值类型的键的集合。
\warning 若修改的值使键不等价，行为未定义。
\sa mapped_set_traits
\sa ystdex::try_emplace
\see WG21 N3456 23.2.4 [associative.reqmts] 。
\see Documentation::YBase @2.1.4.1 。
\see Documentation::YBase @2.1.4.2 。
\since build 844

类似 ISO C++ 的 std::set 的容器，但基于 ystdex::map ，元素和指向元素的迭代器可修改。
容器的 emplace 操作和 insert 操作基于不同的 mapped_set_traits 操作。
容器的 emplace 操作提供比标准库容器更多的保证，而不依赖 LWG 2362 的解决。
因为接口类似 std::set 而不是 std::map ，也不存在一般地更高效的其它选项，
	不提供 try_emplace 和 insert_or_move 操作。基于 ystdex::try_emplace
	和 ystdex::insert_or_move 插入元素可提供相对标准库容器更多的不同的（透明）键类型
	的类似操作，但仍不避免键可能被至少查找两次。
*/
template<typename _type, typename _fComp = less<_type>, class _tTraits
	= mapped_set_traits<_type>, class _tAlloc = std::allocator<_type>>
class mapped_set
{
public:
	using traits_type = _tTraits;
	//! \since build 665
	//!@{
	using key_type = _type;
	using value_type = _type;
	// NOTE: Checks for %value_type and %_tAlloc is in instantiation of
	//	%umap_type below.
	using key_compare = _fComp;
	using value_compare = _fComp;
	using allocator_type = _tAlloc;
	using reference = value_type&;
	using const_reference =  const value_type&;

private:
	using mapped_key_type = details::wrapped_key<_type>;
	using mapped_key_compare = details::tcompare<mapped_key_type, _fComp>;
	using umap_pair = std::pair<const mapped_key_type, value_type>;
	using umap_type = map<mapped_key_type, value_type, mapped_key_compare,
		rebind_alloc_t<_tAlloc, umap_pair>>;
	// NOTE: Here %get_second cannot be used directory due to possible
	//	incomplete value type and requirement on conversion between
	//	'const_iterator' and 'iterator'.
	template<typename _tIter>
	using wrap_iter
		= transformed_iterator<_tIter, iterator_transformation::second<>>;

public:
	using iterator = yimpl(wrap_iter<typename umap_type::iterator>);
	using const_iterator = yimpl(wrap_iter<typename umap_type::const_iterator>);
	using size_type = yimpl(typename umap_type::size_type);
	using difference_type = yimpl(typename umap_type::difference_type);
	using pointer = typename allocator_traits<_tAlloc>::pointer;
	using const_pointer = typename allocator_traits<_tAlloc>::const_pointer;
	using reverse_iterator = ystdex::reverse_iterator<iterator>;
	using const_reverse_iterator = ystdex::reverse_iterator<const_iterator>;

private:
	// XXX: It is undefined behavior when %value_type is incomplete if it is of
	//	an instance of %std::map, however some implementations actually support
	//	this.
	umap_type m_map;

public:
	mapped_set()
		: mapped_set(_fComp())
	{}
	explicit
	mapped_set(const _fComp& comp, const _tAlloc& a = _tAlloc())
		: m_map(comp, a)
	{}
	template<typename _tIn>
	mapped_set(_tIn first, _tIn last, const _fComp& comp = _fComp(),
		const _tAlloc& a = _tAlloc())
		: m_map(ystdex::make_transform(first, iter_trans<_tIn>),
		ystdex::make_transform(last, iter_trans<_tIn>), comp, a)
	{
		amend_all();
	}
	mapped_set(const mapped_set& s)
		: m_map(s.m_map)
	{
		amend_all();
	}
	mapped_set(mapped_set&& s)
		// NOTE: ISO C++ allows the underlying associative container to throw on
		//	move.
		yimpl(ynoexcept(is_nothrow_move_constructible<umap_type>()))
		: m_map(std::move(s.m_map))
	{
		amend_all();
	}
	explicit
	mapped_set(const _tAlloc& a)
		: m_map(a)
	{}
	mapped_set(const mapped_set& s, const _tAlloc& a)
		: m_map(s.m_map, a)
	{
		amend_all();
	}
	mapped_set(mapped_set&& s, const _tAlloc& a)
		: m_map(std::move(s.m_map), a)
	{
		amend_all();
	}
	mapped_set(std::initializer_list<value_type> il,
		const _fComp& comp = _fComp(), const _tAlloc& a = _tAlloc())
		: mapped_set(il.begin(), il.end(), comp, a)
	{}
	template<typename _tIn>
	mapped_set(_tIn first, _tIn last, const _tAlloc& a)
		: mapped_set(first, last, _fComp(), a)
	{}
	mapped_set(std::initializer_list<value_type> il, const _tAlloc& a)
		: mapped_set(il, _fComp(), a)
	{}
	~mapped_set() yimpl(= default);

	mapped_set&
	operator=(const mapped_set& s)
	{
		// NOTE: The underlying implementation of %std::map may reuse node, e.g.
		//	in libstdc++. So it is preferred to directly move and then to fix
		//	references.
		m_map = s.m_map;
		amend_all();
		return *this;
	}
	// NOTE: Since the resolution of LWG 2321 is respected in the underlying
	//	container, no %amend_all call is needed.
	// XXX: The exception specification is changed. See the comment for the move
	//	assignment operator of %ystdex::map.
	//! \since build 866
	mapped_set&
	operator=(mapped_set&&) yimpl(= default);
	mapped_set&
	operator=(std::initializer_list<value_type> il)
	{
		return *this = mapped_set(il);
	}

	YB_ATTR_nodiscard YB_PURE allocator_type
	get_allocator() const ynothrow
	{
		return m_map.get_allocator();
	}

	YB_ATTR_nodiscard YB_PURE iterator
	begin() ynothrow
	{
		return iterator(m_map.begin());
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	begin() const ynothrow
	{
		return const_iterator(m_map.begin());
	}

	YB_ATTR_nodiscard YB_PURE iterator
	end() ynothrow
	{
		return iterator(m_map.end());
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	end() const ynothrow
	{
		return const_iterator(m_map.end());
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
		return const_iterator(m_map.cbegin());
	}

	YB_ATTR_nodiscard YB_PURE const_iterator
	cend() const ynothrow
	{
		return const_iterator(m_map.cend());
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
		return m_map.empty();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	size() const ynothrow
	{
		return m_map.size();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	max_size() const ynothrow
	{
		return m_map.max_size();
	}

	/*!
	\pre 满足 ADL 合式要求。
	\note 使用 ADL get_value_key 取键的值。
	\note 使用 ADL extend_key 作为内部的键，仅在插入值时使用，可能被参数之一所有。
	\note 使用 ADL recover_key 恢复 extend_key 的键的值到构造后的元素。
	\note 保证当存在等价的键的值而不需要插入时，不复制和转移用于构造值的参数。
	\see LWG 2362 。

	使用指定参数构造元素插入容器。
	*/
	//!@{
	template<typename... _tParams>
	inline std::pair<iterator, bool>
	emplace(_tParams&&... args)
	{
		return emplace_hint_impl(index_sequence<>(), std::tuple<>(),
			yforward(args)...);
	}

	template<typename... _tParams>
	inline iterator
	emplace_hint(const_iterator position, _tParams&&... args)
	{
		return emplace_hint_impl(index_sequence<0>(),
			std::forward_as_tuple(position.get()), yforward(args)...).first;
	}

private:
	//! \since build 845
	template<size_t... _vSeq, typename... _tSeq, typename... _tParams>
	std::pair<iterator, bool>
	emplace_hint_impl(index_sequence<_vSeq...> seq, std::tuple<_tSeq...> pos,
		_tParams&&... args)
	{
		auto&& k(traits_type::get_value_key(yforward(args)...));

		return emplace_res_conv(emplace_search(
			[&](typename umap_type::const_iterator i){
			auto&& ek(traits_type::extend_key(std::move(k), *this));
			// XXX: Blocked. 'yforward' cause G++ 5.3 crash: internal compiler
			//	error: Segmentation fault.
			const auto j(ystdex::emplace_hint_in_place(m_map, i,
				mapped_key_type(ystdex::as_const(ek)),
				std::forward<_tParams>(args)...));

			traits_type::restore_key(j->second, std::move(ek));
			return j;
		}, ystdex::as_const(k), seq, pos));
	}
	//!@}

	//! \since build 849
	template<typename _func, typename _tKey, size_t... _vSeq, typename... _tSeq>
	std::pair<typename umap_type::iterator, bool>
	emplace_search(_func f, const _tKey& k, index_sequence<_vSeq...>,
		std::tuple<_tSeq...> pos)
	{
		yunused(pos);
		return ystdex::search_map_by([&](typename umap_type::const_iterator i){
			const auto j(f(i));

			amend_pair(*j);
			return j;
		}, m_map, std::get<_vSeq>(pos)..., k);
	}

	//! \since build 845
	static std::pair<iterator, bool>
	emplace_res_conv(const std::pair<typename umap_type::iterator, bool>& res)
	{
		return {iterator(res.first), res.second};
	}

public:
	std::pair<iterator, bool>
	insert(const value_type& x)
	{
		return insert_unique(x);
	}
	std::pair<iterator, bool>
	insert(value_type&& x)
	{
		return insert_unique(std::move(x));
	}
	iterator
	insert(const_iterator position, const value_type& x)
	{
		return insert_unique_hint(position, x);
	}
	iterator
	insert(const_iterator position, value_type&& x)
	{
		return insert_unique_hint(position, std::move(x));
	}
	template<typename _tIn>
	void
	insert(_tIn first, _tIn last)
	{
		m_map.insert(ystdex::make_transform(first, iter_trans<_tIn>),
			ystdex::make_transform(last, iter_trans<_tIn>));
		amend_all();
	}
	void
	insert(std::initializer_list<value_type> il)
	{
		insert(il.begin(), il.end());
	}

private:
	//! \since build 845
	//!@{
	static value_type
	insert_forward(value_type& x)
	{
		return traits_type::set_value_move(x);
	}
	static const value_type&
	insert_forward(const value_type& x)
	{
		return x;
	}

	template<typename _tParam>
	inline std::pair<iterator, bool>
	insert_unique(_tParam&& x)
	{
		return emplace_res_conv(insert_unique_impl(index_sequence<>(),
			std::tuple<>(), yforward(x)));
	}

	template<typename _tParam>
	inline iterator
	insert_unique_hint(iterator position, _tParam&& x)
	{
		return iterator(insert_unique_impl(index_sequence<0>(),
			std::forward_as_tuple(position.get()), yforward(x)).first);
	}

	template<size_t... _vSeq, typename... _tSeq, typename _tParam>
	std::pair<typename umap_type::iterator, bool>
	insert_unique_impl(index_sequence<_vSeq...> seq, std::tuple<_tSeq...> pos,
		_tParam&& x)
	{
		return emplace_search([&](typename umap_type::const_iterator i){
			// XXX: Result of %traits_type::set_value_move call need to be
			//	EmplaceConstructible into %mapped_type.
			return ystdex::emplace_hint_in_place(m_map, i, mapped_key_type(x),
				insert_forward(x));
		}, x, seq, pos);
	}
	//!@}

public:
	iterator
	erase(iterator position)
	{
		return iterator(m_map.erase(position.get()));
	}
	iterator
	erase(const_iterator position)
	{
		return iterator(m_map.erase(position.get()));
	}
	size_type
	erase(const key_type& x)
	{
		return m_map.erase(mapped_key_type(x));
	}
	iterator
	erase(const_iterator first, const_iterator last)
	{
		return iterator(m_map.erase(first.get(), last.get()));
	}

	// XXX: The exception specification is changed. See the comment for the
	//	function %swap of %ystdex::map.
	//! \since build 967
	void
	swap(mapped_set& s) ynoexcept_spec(is_nothrow_move_assignable<_fComp>())
	{
		m_map.swap(s.m_map);
	}

	void
	clear() ynothrow
	{
		m_map.clear();
	}

	YB_ATTR_nodiscard YB_PURE key_compare
	key_comp() const
	{
		return m_map.key_comp().comp;
	}

	YB_ATTR_nodiscard YB_PURE value_compare
	value_comp() const
	{
		return m_map.key_comp().comp;
	}

#define YB_Impl_MappedSet_GenericLookupHead(_n, _r) \
	template<typename _tTransKey, \
		yimpl(typename = enable_if_transparent_t<_fComp, _tTransKey>)> \
	YB_ATTR_nodiscard YB_PURE inline _r \
	_n(const _tTransKey& x)

	YB_ATTR_nodiscard YB_PURE iterator
	find(const key_type& x)
	{
		return iterator(m_map.find(mapped_key_type(x)));
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	find(const key_type& x) const
	{
		return const_iterator(m_map.find(mapped_key_type(x)));
	}
	//! \since build 678
	YB_Impl_MappedSet_GenericLookupHead(find, iterator)
	{
		return iterator(m_map.find(x));
	}
	//! \since build 678
	YB_Impl_MappedSet_GenericLookupHead(find, const_iterator) const
	{
		return const_iterator(m_map.find(x));
	}

	YB_ATTR_nodiscard YB_PURE size_type
	count(const key_type& x) const
	{
		return m_map.count(mapped_key_type(x));
	}
	YB_Impl_MappedSet_GenericLookupHead(count, size_type) const
	{
		return m_map.count(x);
	}

	/*!
	\see WG21 P0458R2 。
	\since build 967
	*/
	//!@{
	YB_ATTR_nodiscard YB_PURE bool
	contains(const key_type& x) const
	{
		return find(x) != end();
	}
	YB_Impl_MappedSet_GenericLookupHead(contains, bool) const
	{
		return find(x) != end();
	}
	//!@}

	YB_ATTR_nodiscard YB_PURE iterator
	lower_bound(const key_type& x)
	{
		return iterator(m_map.lower_bound(mapped_key_type(x)));
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	lower_bound(const key_type& x) const
	{
		return const_iterator(m_map.lower_bound(mapped_key_type(x)));
	}
	//! \since build 678
	YB_Impl_MappedSet_GenericLookupHead(lower_bound, iterator)
	{
		return iterator(m_map.lower_bound(x));
	}
	//! \since build 678
	YB_Impl_MappedSet_GenericLookupHead(lower_bound, const_iterator) const
	{
		return const_iterator(m_map.lower_bound(x));
	}

	YB_ATTR_nodiscard YB_PURE iterator
	upper_bound(const key_type& x)
	{
		return iterator(m_map.upper_bound(mapped_key_type(x)));
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	upper_bound(const key_type& x) const
	{
		return const_iterator(m_map.upper_bound(mapped_key_type(x)));
	}
	//! \since build 678
	YB_Impl_MappedSet_GenericLookupHead(upper_bound, iterator)
	{
		return iterator(m_map.upper_bound(x));
	}
	//! \since build 678
	YB_Impl_MappedSet_GenericLookupHead(upper_bound, const_iterator) const
	{
		return const_iterator(m_map.upper_bound(x));
	}

	YB_ATTR_nodiscard YB_PURE std::pair<iterator, iterator>
	equal_range(const key_type& x)
	{
		const auto pr(m_map.equal_range(mapped_key_type(x)));

		return {iterator(pr.first), iterator(pr.second)};
	}
	YB_ATTR_nodiscard YB_PURE std::pair<const_iterator, const_iterator>
	equal_range(const key_type& x) const
	{
		const auto pr(m_map.equal_range(mapped_key_type(x)));

		return {const_iterator(pr.first), const_iterator(pr.second)};
	}
	//! \since build 678
	YB_Impl_MappedSet_GenericLookupHead(equal_range,
		std::pair<iterator YPP_Comma iterator>)
	{
		const auto pr(m_map.equal_range(x));

		return {iterator(pr.first), iterator(pr.second)};
	}
	//! \since build 678
	YB_Impl_MappedSet_GenericLookupHead(equal_range,
		std::pair<const_iterator YPP_Comma const_iterator>) const
	{
		const auto pr(m_map.equal_range(x));

		return {const_iterator(pr.first), const_iterator(pr.second)};
	}

#undef YB_Impl_MappedSet_GenericLookupHead

private:
	//! \since build 742
	void
	amend_all() const ynothrow
	{
		for(const auto& pr : m_map)
			amend_pair(pr);
	}

	static void
	amend_pair(const umap_pair& pr) ynothrow
	{
		pr.first.amend(pr.second);
	}

	template<typename _tIter>
	static umap_pair
	iter_trans(const _tIter& i)
	{
		return {mapped_key_type(*i), *i};
	}
	//!@}

	//! \since build 844
	friend void
	swap(mapped_set& x, mapped_set& y) ynoexcept_spec(x.swap(y))
	{
		x.swap(y);
	}
};

} // namespace ystdex;

#endif

