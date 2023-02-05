/*
	© 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file unordered_map.hpp
\ingroup YStandardEx
\brief 无序映射容器。
\version r763
\author FrankHB <frankhb1989@gmail.com>
\since build 966
\par 创建时间:
	2023-01-26 14:18:09 +0800
\par 修改时间:
	2023-02-05 22:25 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::UnorderedMap

提供 ISO C++20 标准库头 \c \<unordered_map> 兼容的替代接口和实现。
包括以下已有其它实现支持的 ISO C++20 后的修改：
LWG 2839 ：允许自转移赋值。
包括以下扩展：
允许不完整类型作为 unordered_map 的模板参数。
*/


#ifndef YB_INC_ystdex_unordered_map_hpp_
#define YB_INC_ystdex_unordered_map_hpp_ 1

#include "hash_table.h" // for "hash_table.h" (implying "range.hpp"),
//	__cpp_lib_allocator_traits_is_always_equal, std::hash, equal_to, std::pair,
//	std::allocator, equality_comparable, allocator_traits, first_of, YAssert,
//	enable_if_constructible_r_t, and_, is_nothrow_swappable,
//	ystdex::swap_dependent;
#include <unordered_map> // for <unordered_map>,
//	__cpp_lib_generic_unordered_lookup, __cpp_lib_unordered_map_try_emplace,
//	__cpp_lib_node_extract, std::initializer_list;

namespace ystdex
{

//! \since build 966
//!@{
#if false
// NOTE: For exposition only. Since %ystdex::unordered_map has incomplete type
//	support which is not guaranteed by ISO C++20 (even if it works with some
//	implementations, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=92770), it
//	should not be in %cpp2020 inline namespace.
#if (__cpp_lib_generic_unordered_lookup >= 201811L \
	|| __cplusplus >= 201402L) && ((__cpp_lib_allocator_traits_is_always_equal
	>= 201411L && __cpp_lib_unordered_map_try_emplace >= 201411L \
	&& __cpp_lib_node_extract >= 201606L) || __cplusplus >= 201606L)
#	define YB_Has_Cpp20_unordered_map true
#else
#	define YB_Has_Cpp20_unordered_map false
#endif
#undef YB_Has_Cpp20_unordered_map
#endif

//! \todo 提供实现。
template<typename, typename, typename, class, class>
class unordered_multimap;

/*!
\ingroup YBase_replacement_features
\brief 无序映射容器。
\warning 非虚析构。
\see Documentation::YBase @2.1.4.1 。
\see Documentation::YBase @2.1.4.2 。

类似 ISO C++20 的 std::unordered_map 的容器，但支持不完整类型作为键和被映射的类型。
支持 WG21 P0458R2 。
不完整类型的支持条件同 WG21 N4510 ，除对类型完整地要求扩展到键和映射的类型。
部分成员提供较 ISO C++20 更强的 noexcept 异常规范，除转移赋值略有不同。
*/
template<typename _tKey, typename _tMapped, typename _fHash = std::hash<_tKey>,
	typename _fPred = equal_to<_tKey>,
	class _tAlloc = std::allocator<std::pair<const _tKey, _tMapped>>>
class unordered_map : private equality_comparable<unordered_map<_tKey,
	_tMapped, _fHash, _fPred, _tAlloc>>
{
	template<typename, typename, typename>
	friend class details::hash_table::table_merge_helper;

public:
	using key_type = _tKey;
	using mapped_type = _tMapped;
	using value_type = std::pair<const _tKey, _tMapped>;
	// NOTE: Checks for %value_type and %_tAlloc is in instantiation of
	//	%rep_type below.
	using hasher = _fHash;
	using key_equal = _fPred;
	using allocator_type = _tAlloc;

private:
	using ator_traits = allocator_traits<allocator_type>;
	template<typename _tTraits = details::hash_table::table_traits<
		details::hash_table::cache_default<_tKey, _fHash>{}, true>>
	using umap_table = details::hash_table::table<_tKey, value_type, _tAlloc,
		first_of<>, _fPred, _fHash, details::hash_table::default_hashing,
		_tTraits>;
	using rep_type = umap_table<>;

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
	using local_iterator = typename rep_type::local_iterator;
	using const_local_iterator = typename rep_type::const_local_iterator;
	//!@}
	using node_type = typename rep_type::node_type;
	using insert_return_type = typename rep_type::insert_return_type;

private:
	rep_type table;

public:
	//! \note 实现定义：隐含的桶数为 0 或参数 n 的默认值为 0 。
	//!@{
	//! \see LWG 2193 。
	unordered_map() yimpl(= default);
	explicit
	unordered_map(const allocator_type& a)
		: table(a)
	{}
	explicit
	unordered_map(size_type n, const hasher& hf = hasher(), const key_equal& eql
		= key_equal(), const allocator_type& a = allocator_type())
		: table(n, hf, eql, a)
	{}
	template<typename _tIn>
	inline
	unordered_map(_tIn first, _tIn last, size_type n = yimpl(0),
		const hasher& hf = hasher(), const key_equal& eql = key_equal(),
		const allocator_type& a = allocator_type())
		: table(n, hf, eql, a)
	{
		table.insert_range_unique(first, last);
	}
	unordered_map(std::initializer_list<value_type> il, size_type n = yimpl(0),
		const hasher& hf = hasher(), const key_equal& eql = key_equal(),
		const allocator_type& a = allocator_type())
		: table(n, hf, eql, a)
	{
		table.insert_range_unique(il.begin(), il.end());
	}
	unordered_map(size_type n, const allocator_type& a)
		: unordered_map(n, hasher(), key_equal(), a)
	{}
	unordered_map(size_type n, const hasher& hf, const allocator_type& a)
		: unordered_map(n, hf, key_equal(), a)
	{}
	template<typename _tIn>
	inline
	unordered_map(_tIn first, _tIn last, size_type n, const allocator_type& a)
		: unordered_map(first, last, n, hasher(), key_equal(), a)
	{}
	template<typename _tIn>
	inline
	unordered_map(_tIn first, _tIn last, size_type n, const hasher& hf,
		const allocator_type& a)
		: unordered_map(first, last, n, hf, key_equal(), a)
	{}
	unordered_map(std::initializer_list<value_type> il, size_type n,
		const allocator_type& a)
		: unordered_map(il, n, hasher(), key_equal(), a)
	{}
	unordered_map(std::initializer_list<value_type> il, size_type n,
		const hasher& hf, const allocator_type& a)
		: unordered_map(il, n, hf, key_equal(), a)
	{}
	//!@}
	unordered_map(const unordered_map&) = default;
	unordered_map(const unordered_map& um, const allocator_type& a)
		: table(um.table, a)
	{}
	unordered_map(unordered_map&&) = default;
	unordered_map(unordered_map&& um, const allocator_type& a)
		ynoexcept_spec(rep_type(std::move(um.table), a))
		: table(std::move(um.table), a)
	{}

	unordered_map&
	operator=(const unordered_map&) = default;
	// XXX: The exception specification is changed. Since ISO C++17 (by adoption
	//	of WG21 N4258) the standard only requires conditional non-throwing
	//	exception specification when the allocator meets
	//	%std::allocator_traits<allocator_type>::is_always_equal and both the
	//	comparison object and the hash object types meet
	//	%std::is_nothrow_move_assignable, with regardless to
	//	%propagate_on_container_move_assignment of the node allocator. Here the
	//	%allocator_traits of the internal node is also used instead. See
	//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91541 and LWG 3267.
	//! \see WG21 N4258 。
	unordered_map&
	operator=(unordered_map&&) yimpl(= default);
	unordered_map&
	operator=(std::initializer_list<value_type> il)
	{
		table.assign_unique(il.begin(), il.end(), il.size());
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const unordered_map& x, const unordered_map& y)
	{
		return x.table == y.table;
	}

	YB_ATTR_nodiscard YB_PURE allocator_type
	get_allocator() const ynothrow
	{
		return table.get_allocator();
	}

	YB_ATTR_nodiscard YB_PURE iterator
	begin() ynothrow
	{
		return table.begin();
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	begin() const ynothrow
	{
		return table.begin();
	}

	YB_ATTR_nodiscard YB_PURE iterator
	end() ynothrow
	{
		return table.end();
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	end() const ynothrow
	{
		return table.end();
	}

	YB_ATTR_nodiscard YB_PURE const_iterator
	cbegin() const ynothrow
	{
		return table.begin();
	}

	YB_ATTR_nodiscard YB_PURE const_iterator
	cend() const ynothrow
	{
		return table.end();
	}

	YB_ATTR_nodiscard YB_PURE bool
	empty() const ynothrow
	{
		return table.empty();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	size() const ynothrow
	{
		return table.size();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	max_size() const ynothrow
	{
		return table.max_size();
	}

	//! \see WG21 N4279 。
	//!@{
	// NOTE: Like %try_emplace with hint in %map, dedicated code instead of
	//	%ystdex::(try_emplace, try_emplace_hint) is used to make the internal
	//	implementation efficient. At current, %hash_table::table does not expose
	//	sufficient interface to reduce the unneeded hashing. See also https://gcc.gnu.org/bugzilla/show_bug.cgi?id=95079.
	template<typename... _tParams>
	std::pair<iterator, bool>
	emplace(_tParams&&... args)
	{
		return table.emplace_unique(yforward(args)...);
	}

	template<typename... _tParams>
	iterator
	emplace_hint(const_iterator position, _tParams&&... args)
	{
		return table.emplace_hint_unique(position, yforward(args)...);
	}

	std::pair<iterator, bool>
	insert(const value_type& x)
	{
		return table.insert_unique(x);
	}
	//! \see LWG 2354 。
	std::pair<iterator, bool>
	insert(value_type&& x)
	{
		return table.insert_unique(std::move(x));
	}
	template<typename _tPair>
	inline yimpl(enable_if_constructible_r_t)<
		std::pair<iterator YPP_Comma bool>, value_type, _tPair>
	insert(_tPair&& x)
	{
		return table.emplace_unique(yforward(x));
	}
	iterator
	insert(const_iterator position, const value_type& x)
	{
		return table.insert_hint_unique(position, x);
	}
	//! \see LWG 2354 。
	iterator
	insert(const_iterator position, value_type&& x)
	{
		return table.insert_hint_unique(position, std::move(x));
	}
	template<typename _tPair>
	inline yimpl(enable_if_constructible_r_t)<iterator, value_type, _tPair>
	insert(const_iterator hint, _tPair&& x)
	{
		return table.emplace_hint_unique(hint, yforward(x));
	}
	template<typename _tIn>
	void
	insert(_tIn first, _tIn last)
	{
		table.insert_range_unique(first, last);
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
		return table.extract(position);
	}
	node_type
	extract(const key_type& x)
	{
		return table.extract(x);
	}

	//! \pre 间接断言：若最后一个参数非空，分配器和参数指定的容器分配器相等。
	//!@{
	insert_return_type
	insert(node_type&& nh)
	{
		return table.reinsert_node_unique(std::move(nh));
	}
	iterator
	insert(const_iterator, node_type&& nh)
	{
		// XXX: The hint is ignored.
		return table.reinsert_node_unique(std::move(nh)).position;
	}
	//!@}

	template<typename... _tParams>
	std::pair<iterator, bool>
	try_emplace(const key_type& k, _tParams&&... args)
	{
		return table.try_emplace(cend(), k, yforward(args)...);
	}
	template<typename... _tParams>
	std::pair<iterator, bool>
	try_emplace(key_type&& k, _tParams&&... args)
	{
		return table.try_emplace(cend(), std::move(k), yforward(args)...);
	}
	template<typename... _tParams>
	iterator
	try_emplace(const_iterator hint, const key_type& k, _tParams&&... args)
	{
		return table.try_emplace(hint, k, yforward(args)...).first;
	}
	template<typename... _tParams>
	iterator
	try_emplace(const_iterator hint, key_type&& k, _tParams&&... args)
	{
		return table.try_emplace(hint, std::move(k), yforward(args)...).first;
	}

	template<typename _tObj>
	std::pair<iterator, bool>
	insert_or_assign(const key_type& k, _tObj&& obj)
	{
		auto ret(table.try_emplace(cend(), k, yforward(obj)));

		if(!ret.second)
			ret.first->second = yforward(obj);
		return ret;
	}
	template<typename _tObj>
	std::pair<iterator, bool>
	insert_or_assign(key_type&& k, _tObj&& obj)
	{
		auto ret(table.try_emplace(cend(), std::move(k), yforward(obj)));

		if(!ret.second)
			ret.first->second = yforward(obj);
		return ret;
	}
	template<typename _tObj>
	iterator
	insert_or_assign(const_iterator hint, const key_type& k, _tObj&& obj)
	{
		auto ret(table.try_emplace(hint, k, yforward(obj)));

		if(!ret.second)
			ret.first->second = yforward(obj);
		return ret.first;
	}
	template<typename _tObj>
	iterator
	insert_or_assign(const_iterator hint, key_type&& k, _tObj&& obj)
	{
		auto ret(table.try_emplace(hint, std::move(k), yforward(obj)));

		if(!ret.second)
			ret.first->second = yforward(obj);
		return ret.first;
	}
	//!@}

	//! \see LWG 2059 。
	iterator
	erase(iterator position)
	{
		return table.erase(position);
	}
	iterator
	erase(const_iterator position)
	{
		return table.erase(position);
	}
	size_type
	erase(const key_type& x)
	{
		return table.erase(x);
	}
	iterator
	erase(const_iterator first, const_iterator last)
	{
		return table.erase(first, last);
	}

	// XXX: The exception specification is strengthened. Since ISO C++17 (by
	//	adoption of WG21 N4258) the standard only requires conditional
	//	non-throwing exception specification when the allocator also meets
	//	%std::allocator_traits<allocator_type>::is_always_equal.
	//! \see WG21 N4258 。
	void
	swap(unordered_map& x) ynoexcept(yimpl(
		and_<is_nothrow_swappable<_fHash>, is_nothrow_swappable<_fPred>>()))
	{
		ystdex::swap_dependent(table, x.table);
	}
	friend void
	swap(unordered_map& x, unordered_map& y) ynoexcept_spec(x.swap(y))
	{
		x.swap(y);
	}

	void
	clear() ynothrow
	{
		table.clear();
	}

	//! \pre 间接断言：分配器和参数指定的容器分配器相等。
	//!@{
	template<typename _fHash2, typename _fPred2>
	inline void
	merge(unordered_map<_tKey, _tMapped, _fHash2, _fPred2, _tAlloc>& source)
	{
		table.merge_unique(details::table_merge_helper<unordered_map, _fHash2,
			_fPred2>::get_table(source));
	}
	template<typename _fHash2, typename _fPred2>
	inline void
	merge(unordered_map<_tKey, _tMapped, _fHash2, _fPred2, _tAlloc>&& source)
	{
		merge(source);
	}
	template<typename _fHash2, typename _fPred2>
	inline void
	merge(unordered_multimap<_tKey, _tMapped, _fHash2, _fPred2, _tAlloc>&
		source)
	{
		table.merge_unique(details::table_merge_helper<unordered_map, _fHash2,
			_fPred2>::get_table(source));
	}
	template<typename _fHash2, typename _fPred2>
	inline void
	merge(unordered_multimap<_tKey, _tMapped, _fHash2, _fPred2, _tAlloc>&&
		source)
	{
		merge(source);
	}
	//!@}

	YB_ATTR_nodiscard YB_PURE hasher
	hash_function() const
	{
		return table.hash_function();
	}

	YB_ATTR_nodiscard YB_PURE key_equal
	key_eq() const
	{
		return table.key_eq();
	}

#define YB_Impl_UnorderedMap_GenericLookupHead(_n) \
	template<typename _tTransKey> \
	YB_ATTR_nodiscard YB_PURE inline auto \
	_n(const _tTransKey& x)

	YB_ATTR_nodiscard YB_PURE iterator
	find(const key_type& x)
	{
		return table.find(x);
	}
	YB_Impl_UnorderedMap_GenericLookupHead(find) -> decltype(table.find_tr(x))
	{
		return table.find_tr(x);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	find(const key_type& x) const
	{
		return table.find(x);
	}
	YB_Impl_UnorderedMap_GenericLookupHead(find) const
		-> decltype(table.find_tr(x))
	{
		return table.find_tr(x);
	}

	YB_ATTR_nodiscard YB_PURE size_type
	count(const key_type& x) const
	{
		return table.count(x);
	}
	YB_Impl_UnorderedMap_GenericLookupHead(count) const
		-> decltype(table.count_tr(x))
	{
		return table.count_tr(x);
	}

	YB_ATTR_nodiscard YB_PURE bool
	contains(const key_type& x) const
	{
		return table.find(x) != table.end();
	}
	YB_Impl_UnorderedMap_GenericLookupHead(contains) const
		-> decltype(table.find_tr(x), void(), true)
	{
		return table.find_tr(x) != table.end();
	}

	YB_ATTR_nodiscard YB_PURE std::pair<iterator, iterator>
	equal_range(const key_type& x)
	{
		return table.equal_range(x);
	}
	YB_Impl_UnorderedMap_GenericLookupHead(equal_range)
		-> decltype(table.equal_range_tr(x))
	{
		return table.equal_range_tr(x);
	}
	YB_ATTR_nodiscard YB_PURE std::pair<const_iterator, const_iterator>
	equal_range(const key_type& x) const
	{
		return table.equal_range(x);
	}
	YB_Impl_UnorderedMap_GenericLookupHead(equal_range) const
		-> decltype(table.equal_range_tr(x))
	{
		return table.equal_range_tr(x);
	}

#undef YB_Impl_UnorderedMap_GenericLookupHead

	mapped_type&
	operator[](const key_type& k)
	{
		return try_emplace(k).first->second;
	}
	mapped_type&
	operator[](key_type&& k)
	{
		return try_emplace(std::move(k)).first->second;
	}

	YB_ATTR_nodiscard YB_PURE mapped_type&
	at(const key_type& k)
	{
		return at_impl(*this, k);
	}
	YB_ATTR_nodiscard YB_PURE const mapped_type&
	at(const key_type& k) const
	{
		return at_impl(*this, k);
	}

private:
	template<class _tClass>
	YB_ATTR_nodiscard YB_PURE static auto
	at_impl(_tClass&& m, const key_type& k) -> decltype(((*m.find(k)).second))
	{
		const auto i(m.find(k));

		if(i == m.end())
			// XXX: Is it needed to extract and put into a function?
			throw std::out_of_range("unordered_map::at");
		return (*i).second;
	}

public:
	YB_ATTR_nodiscard YB_PURE size_type
	bucket_count() const ynothrow
	{
		return table.bucket_count();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	max_bucket_count() const ynothrow
	{
		return table.max_bucket_count();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	bucket_size(size_type n) const
	{
		return table.bucket_size(n);
	}

	YB_ATTR_nodiscard YB_PURE size_type
	bucket(const key_type& key) const
	{
		return table.bucket(key);
	}

	YB_ATTR_nodiscard YB_PURE local_iterator
	begin(size_type n)
	{
		return table.begin(n);
	}
	YB_ATTR_nodiscard YB_PURE const_local_iterator
	begin(size_type n) const
	{
		return table.begin(n);
	}

	YB_ATTR_nodiscard YB_PURE const_local_iterator
	cbegin(size_type n) const
	{
		return table.cbegin(n);
	}

	YB_ATTR_nodiscard YB_PURE local_iterator
	end(size_type n)
	{
		return table.end(n);
	}
	YB_ATTR_nodiscard YB_PURE const_local_iterator
	end(size_type n) const
	{
		return table.end(n);
	}

	YB_ATTR_nodiscard YB_PURE const_local_iterator
	cend(size_type n) const
	{
		return table.cend(n);
	}

	YB_ATTR_nodiscard YB_PURE float
	load_factor() const ynothrow
	{
		return table.load_factor();
	}

	YB_ATTR_nodiscard YB_PURE float
	max_load_factor() const ynothrow
	{
		return table.max_load_factor();
	}
	void
	max_load_factor(float z)
	{
		table.max_load_factor(z);
	}

	void
	rehash(size_type n)
	{
		table.rehash(n);
	}

	void
	reserve(size_type n)
	{
		table.reserve(n);
	}
};


namespace details
{

template<typename _tKey, typename _tMapped, typename _fHash1, typename _fPred1,
	class _tAlloc, typename _fHash2, typename _fPred2>
struct table_merge_helper<unordered_map<_tKey, _tMapped, _fHash1, _fPred1,
	_tAlloc>, _fHash2, _fPred2>
{
private:
	friend unordered_map<_tKey, _tMapped, _fHash1, _fPred1, _tAlloc>;

	YB_ATTR_nodiscard YB_PURE static auto
	get_table(unordered_map<_tKey, _tMapped, _fHash2, _fPred2, _tAlloc>& m)
		ynothrow -> decltype((m.table))
	{
		return m.table;
	}
	// XXX: Disabled to prevent ill-formed instantiation of incomplete types in
	//	the trailing return type.
	// TODO: Use complete %unordered_multimap implementation.
#if false
	YB_ATTR_nodiscard YB_PURE static auto
	get_table(unordered_multimap<_tKey, _tMapped, _fHash2, _fPred2, _tAlloc>& m)
		ynothrow -> decltype((m.table))
	{
		return m.table;
	}
#endif
};

} // namespace details;

} // namespace ystdex;

#endif

