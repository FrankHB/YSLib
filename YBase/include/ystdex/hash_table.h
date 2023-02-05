/*
	© 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file hash_table.h
\ingroup YStandardEx
\brief 作为无序容器实现的散列表。
\version r3164
\author FrankHB <frankhb1989@gmail.com>
\since build 966
\par 创建时间:
	2023-01-26 19:03:06 +0800
\par 修改时间:
	2023-02-06 02:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::HashTable

无序关联容器的内部实现的散列表接口。
设计和 libstdc++ 的 <bits/hashtable_policy.h> 和 <bits/hashtable.h>
	中非公开接口类似，提供散列表内部数据结构，可同时兼容 std::unordered_map 、
	std::unordered_set 、std::unordered_multimap 和 std::unordered_multiset
	的容器。
接口支持 ISO C++20 模式下的容器功能，且支持不完整类型作为无序关联容器的键。
实现语言的特性要求和 YBase.YStandardEx 要求一致。
为便于参照和测试，实现也和 libstdc++ 在 ISO C++20 模式下的相似，
	但充分利用了其它 YBase.YStandardEx 特性。
因为用于内部实现，接口主要在用于实现的特定命名空间内。
除另行指定的兼容性策略外，作为实现的接口不保证对外稳定。
*/


#ifndef YB_INC_ystdex_hash_table_h_
#define YB_INC_ystdex_hash_table_h_ 1

#include "node_handle.hpp" // for "node_handle.hpp" (implying "range.hpp"),
//	std::iterator_traits, std::input_iterator_tag, std::forward_iterator_tag,
//	std::distance, std::pair, std::move, bool_, size_t, typed_storage,
//	ptrdiff_t, forward_iteratable, true_, std::min, CHAR_BIT, std::max, cond_t,
//	is_const, false_, detected_or_t, is_invocable, std::declval, is_empty,
//	replace_storage_t, is_nothrow_copy_constructible, not_, and_,
//	is_nothrow_invocable, rebind_alloc_t, is_unqualified_object,
//	is_allocator_for, is_nothrow_default_constructible, allocator_traits,
//	node_handle, node_insert_return, conditional_t, std::pointer_traits,
//	is_throwing_move_copyable, ystdex::alloc_on_copy,
//	is_nothrow_move_assignable, is_lvalue_reference, std::addressof,
//	ystdex::alloc_on_move, std::is_permutation, ystdex::to_address, enable_if_t,
//	is_nothrow_swappable, ystdex::alloc_on_swap, is_same,
//	is_nothrow_copy_assignable, YAssert, is_bitwise_swappable;
#include "hash.hpp" // for "hash.hpp", std::hash, std::__is_fast_hash
//	conditionally;
#include "base.h" // for noncopyable;
#include "functor.hpp" // for id, wrap_call_nothrow, modulus,
//	enable_if_transparent_t;
#include <cmath> // for std::ceil, std::floor;
#include "bit.hpp" // for ystdex::countl_zero;
#include <limits> // for std::numeric_limits;
#include "compressed_pair.hpp" // for compressed_pair_element;
#include <cstring> // for std::memset;
#include "iterator_trait.hpp" // for has_iterator_value_type;

namespace ystdex
{

//! \since build 966
//!@{
namespace details
{

// NOTE: Use the documented extension of libstdc++ if possible.
// XXX: See https://gcc.gnu.org/git/?p=gcc.git;a=commit;h=4df047dd3494ad17122ea46134a951a319a81b27.
#if defined(__GLIBCXX__) && __GLIBCXX__ > 20130205
template<typename _fHash>
using is_fast_hash = std::__is_fast_hash<_fHash>;
#else
// XXX: This is same to libstdc++ by default.
template<typename _fHash>
using is_fast_hash = not_<is_same<_fHash, std::hash<long double>>>;
#endif


template<typename _tIn>
inline typename std::iterator_traits<_tIn>::difference_type
distance_fw(_tIn first, _tIn last, std::input_iterator_tag)
{
	return first != last ? 1 : 0;
}
template<typename _tFwd>
inline typename std::iterator_traits<_tFwd>::difference_type
distance_fw(_tFwd first, _tFwd last, std::forward_iterator_tag)
{
	return std::distance(first, last);
}
template<typename _tIter>
inline typename std::iterator_traits<_tIter>::difference_type
distance_fw(_tIter first, _tIter last)
{
	return details::distance_fw(first, last,
		typename std::iterator_traits<_tIter>::iterator_category());
}

//! \warning 非虚析构。
//!@{
inline namespace hash_table
{

template<typename, typename, class, typename, typename, typename, class, class>
class table;

template<typename, typename, typename, typename, typename, typename, class>
class table_base;

template<class>
class table_alloc;


//! \ingroup traits
template<bool _bCached, bool _bUnique>
struct table_traits
{
	using hash_cached = bool_<_bCached>;
	using unique_keys = bool_<_bUnique>;
};


//! \ingroup traits
template<typename _fHash>
struct table_hash_traits
{
	YB_ATTR_nodiscard YB_STATELESS static yconstfn size_t
	small_size_threshold() ynothrow
	{
		// NOTE: As libstdc++. See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68303.
		return is_fast_hash<_fHash>() ? 0 : yimpl(20);
	}
};

struct hash_node_base
{
	hash_node_base* p_next = {};

	hash_node_base() ynothrow = default;
	hash_node_base(hash_node_base* p_nd) ynothrow
		: p_next(p_nd)
	{}
};


template<typename _type>
using hash_node_value_base = typed_storage<_type>;


template<bool>
struct hash_node_code_cache
{};

template<>
struct hash_node_code_cache<true>
{
	size_t cached_hash_code;
};

template<typename _type, bool _bCached>
struct hash_node_value
	: hash_node_value_base<_type>, hash_node_code_cache<_bCached>
{};


template<typename _type, bool _bCached>
struct hash_node : hash_node_base, hash_node_value<_type, _bCached>
{
	YB_ATTR_nodiscard YB_PURE hash_node*
	next() const ynothrow
	{
		return static_cast<hash_node*>(this->p_next);
	}
};


template<typename _type, bool _bCached>
class table_iterator_base
{
protected:
	using link_type = hash_node<_type, _bCached>*;

public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = _type;
	using difference_type = ptrdiff_t;

	link_type p_node = {};

protected:
	table_iterator_base() ynothrow = default;
	table_iterator_base(link_type p) ynothrow
		: p_node(p)
	{}

	void
	increment() ynothrowv
	{
		p_node = p_node->next();
	}

	YB_ATTR_nodiscard YB_PURE bool
	equals(const table_iterator_base& x) const ynothrow
	{
		return p_node == x.p_node;
	}
};


// XXX: Different to libstdc++, constness is not saved in the iterator types. If
//	needed for set containers, always use constant iterators (as
//	%rb_tree::tree_const_iterator).
template<typename _type, bool _bCached>
class table_iterator : public table_iterator_base<_type, _bCached>,
	public forward_iteratable<table_iterator<_type, _bCached>, _type&>
{
	template<typename, typename, class, typename, typename, typename, class,
		class>
	friend class table;
	template<typename, bool>
	friend class table_const_iterator;

private:
	using base_type = table_iterator_base<_type, _bCached>;
	using link_type = typename base_type::link_type;

public:
	using pointer = _type*;
	using reference = _type&;

	table_iterator() ynothrow = default;
	explicit
	table_iterator(link_type p) ynothrow
		: base_type(p)
	{}

	YB_ATTR_nodiscard YB_PURE reference
	operator*() const ynothrowv
	{
		return this->p_node->access();
	}

	table_iterator&
	operator++() ynothrowv
	{
		this->increment();
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const table_iterator& x, const table_iterator& y) ynothrow
	{
		return x.equals(y);
	}
};


template<typename _type, bool _bCached>
class table_const_iterator : public table_iterator_base<_type, _bCached>,
	public forward_iteratable<table_const_iterator<_type, _bCached>,
	const _type&>
{
	template<typename, typename, class, typename, typename, typename, class,
		class>
	friend class table;
	template<typename, bool>
	friend class table_const_iterator;

private:
	using base_type = table_iterator_base<_type, _bCached>;
	using link_type = typename base_type::link_type;

public:
	using pointer = const _type*;
	using reference = const _type&;

	table_const_iterator() ynothrow = default;
	explicit
	table_const_iterator(link_type p) ynothrow
		: base_type(p)
	{}
	table_const_iterator(const table_iterator<_type, _bCached>& x)
		ynothrow
		: base_type(x.p_node)
	{}

	reference
	operator*() const ynothrowv
	{
		return this->p_node->access();
	}

	table_const_iterator&
	operator++() ynothrowv
	{
		this->increment();
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const table_const_iterator& x, const table_const_iterator& y)
		ynothrow
	{
		return x.equals(y);
	}
};


//! \ingroup functors
using mod_range_hashing = wrap_call_nothrow<modulus<size_t>, size_t, size_t>;


// NOTE: The load factor is of %float exposed in ISO C++. Internally %double is
//	used to preserve precision, although it is not strictly necessary. For
//	efficiency reason, floating-point values with higher precision are not used.
//	See also https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96958 with similar
//	reasoning.
struct YB_API prime_rehash_policy
{
	using has_load_factor = true_;
	using state_t = size_t;

	static const size_t growth_factor = 2;
	float saved_max_load_factor;
	mutable size_t next_resize = 0;

	prime_rehash_policy(float z = 1.0) ynothrow
		: saved_max_load_factor(z)
	{}

	YB_ATTR_nodiscard YB_PURE float
	max_load_factor() const ynothrow
	{
		return saved_max_load_factor;
	}

	YB_ATTR_nodiscard YB_PURE double
	max_load_factor_d() const ynothrow
	{
		return double(max_load_factor());
	}

	YB_ATTR_nodiscard size_t
	get_next_bucket(size_t n) const ynothrow;

	YB_ATTR_nodiscard YB_PURE size_t
	get_bucket_for_elements(size_t n) const
	{
		return size_t(std::ceil(n / max_load_factor_d()));
	}

	YB_ATTR_nodiscard YB_PURE state_t
	get_state() const ynothrow
	{
		return next_resize;
	}

	YB_ATTR_nodiscard std::pair<bool, size_t>
	need_rehash(size_t, size_t, size_t) const ynothrow;

	void
	reset_state() ynothrow
	{
		next_resize = 0;
	}
	void
	reset_state(state_t state) ynothrow
	{
		next_resize = state;
	}
};
//!@}


//! \ingroup functors
struct mask_range_hashing
{
	YB_ATTR_nodiscard YB_PURE size_t
	operator()(size_t num, size_t den) const ynothrow
	{
		return num & (den - 1);
	}
};


YB_ATTR_nodiscard YB_STATELESS inline size_t
clp2(size_t n) ynothrow
{
	// NOTE: This is equivalent to return 'n != 0 ? std::bit_ceil(n) : 0';
	if(n >= 2)
	{
		const auto lz(ystdex::countl_zero(n - size_t(1)));

		return
			(size_t(1) << (std::numeric_limits<size_t>::digits - lz - 1)) << 1;
	}
	return n;
}


//! \warning 非虚析构。
//!@{
struct power2_rehash_policy
{
	using has_load_factor = true_;
	using state_t = size_t;

	static const size_t growth_factor = 2;
	float saved_max_load_factor;
	size_t next_resize;

	power2_rehash_policy(float z = 1.0) ynothrow
		: saved_max_load_factor(z), next_resize(0)
	{}

	YB_ATTR_nodiscard YB_PURE float
	max_load_factor() const ynothrow
	{
		return saved_max_load_factor;
	}

	YB_ATTR_nodiscard YB_PURE double
	max_load_factor_d() const ynothrow
	{
		return double(saved_max_load_factor);
	}

	YB_ATTR_nodiscard size_t
	get_next_bucket(size_t n) ynothrow
	{
		if(n != 0)
		{
			const auto max_width(std::min<size_t>(sizeof(size_t), 8));
			const auto max_bkt(size_t(1) << (max_width * CHAR_BIT - 1));
			auto res(clp2(n));

			if(res == 0)
				res = max_bkt;
			else if(res == 1)
				res = 2;
			next_resize = res == max_bkt ? size_t(-1)
				: size_t(std::floor(res * max_load_factor_d()));
			return res;
		}
		return 1;
	}

	YB_ATTR_nodiscard YB_PURE size_t
	get_bucket_for_elements(size_t n) const ynothrow
	{
		return size_t(std::ceil(n / max_load_factor_d()));
	}

	YB_ATTR_nodiscard std::pair<bool, size_t>
	need_rehash(size_t n, size_t x, size_t addend) ynothrow
	{
		x += addend;
		if(x > next_resize)
		{
			const double min_bkts(std::max<size_t>(x, next_resize ? 0 : 11)
				/ max_load_factor_d());

			if(min_bkts >= n)
				return {true, get_next_bucket(std::max<size_t>(
					size_t(std::floor(min_bkts) + 1), n * growth_factor))};
			next_resize = size_t(std::floor(n * max_load_factor_d()));
		}
		return {{}, 0};
	}

	YB_ATTR_nodiscard YB_PURE state_t
	get_state() const ynothrow
	{
		return next_resize;
	}

	void
	reset_state() ynothrow
	{
		next_resize = 0;
	}

	void
	reset_state(state_t state) ynothrow
	{
		next_resize = state;
	}
};


// NOTE: Different to libstdc++, types related to the underlying are packed to a
//	trait instead of type parameters in the class templates implementing the
//	table. This is good for shorter mangled names. The members provide a set of
//	the default implementation and implying no explicit composed hashing
//	(identified as "default range hashing" for an "unused" type parameter in
//	libstdc++) is used.
//! \ingroup traits
struct default_hashing
{
	using range_hash = yimpl(mod_range_hashing);
	using rehash_policy = yimpl(prime_rehash_policy);
};


template<class _tPolicy>
using has_mem_load_factor_t = typename _tPolicy::has_load_factor;


template<typename _tKey, typename _type, class _tAlloc, typename _fKeyOfValue,
	typename _fPred, typename _fHash, class _tHashingTraits, class _tTraits,
	bool = detected_or_t<false_, has_mem_load_factor_t,
	typename _tHashingTraits::rehash_policy>::value>
struct rehash_base;

template<typename _tKey, typename _type, class _tAlloc, typename _fKeyOfValue,
	typename _fPred, typename _fHash, class _tHashingTraits, class _tTraits>
struct rehash_base<_tKey, _type, _tAlloc, _fKeyOfValue, _fPred, _fHash,
	_tHashingTraits, _tTraits, false>
{};

template<typename _tKey, typename _type, class _tAlloc, typename _fKeyOfValue,
	typename _fPred, typename _fHash, class _tHashingTraits, class _tTraits>
struct rehash_base<_tKey, _type, _tAlloc, _fKeyOfValue, _fPred, _fHash,
	_tHashingTraits, _tTraits, true>
{
private:
	using table_t = table<_tKey, _type, _tAlloc, _fKeyOfValue, _fPred, _fHash,
		_tHashingTraits, _tTraits>;

public:
	YB_ATTR_nodiscard YB_PURE float
	max_load_factor() const ynothrow
	{
		return static_cast<const table_t*>(this)->rehash_policy()
			.max_load_factor();
	}
	void
	max_load_factor(float z)
	{
		static_cast<table_t*>(this)->rehash_policy(
			typename _tHashingTraits::hashing_policy(z));
	}

protected:
	YB_ATTR_nodiscard YB_PURE double
	max_load_factor_d() const ynothrow
	{
		return double(max_load_factor());
	}

public:
	// \see LWG 1189 。
	void
	reserve(size_t n)
	{
		const auto thiz(static_cast<table_t*>(this));

		thiz->rehash(thiz->rehash_policy().get_bucket_for_elements(n));
	}
};


//! \brief 散列表局部迭代器基类。
template<typename, typename, typename, typename, typename, bool>
struct table_local_iterator_base;


//! \brief 散列计算结果的混入基类。
template<typename _tKey, typename _type, typename _fKeyOfValue, typename _fHash,
	typename _fRangeHash, bool _bCached>
class hash_code_base : private compressed_pair_element<_fHash, 1>
{
	friend struct table_local_iterator_base<_tKey, _type, _fKeyOfValue, _fHash,
		_fRangeHash, false>;

public:
	using hasher = _fHash;

private:
	using base = compressed_pair_element<_fHash, 1>;

protected:
	using hash_code = size_t;

	hash_code_base() = default;
	hash_code_base(const _fHash& hf)
		: base(hf)
	{}
	// NOTE: By deliberately adding the explicitly defalted copy constructor,
	//	the implicitly declared move constructor is suppressed. Like the
	//	components implementation %rb_tree::tree in %Tree, this is consistent to
	//	the current practice. See LWG 2227 for more details.
	// XXX: The comparion object is always copied even during moving.
	hash_code_base(const hash_code_base&) = default;

	template<typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE inline hash_code
	get_hash_code(const _tKeyOrTransKey& k) const
	{
		static_assert(is_invocable<const _fHash&, const _tKeyOrTransKey&>(),
			"Hash function shall be invocable with an argument of key type.");

		return get_hash_function()(k);
	}
	YB_ATTR_nodiscard YB_PURE hash_code
	get_hash_code(const _fHash&, const hash_node_value<_type, true>& nd) const
		ynothrow
	{
		return nd.cached_hash_code;
	}
	template<typename _fHash2>
	YB_ATTR_nodiscard YB_PURE inline hash_code
	get_hash_code(const _fHash2&, const hash_node_value<_type, _bCached>& nd)
		const
	{
		return get_hash_code(_fKeyOfValue()(nd.access()));
	}
	YB_ATTR_nodiscard YB_PURE hash_code
	get_hash_code(const hash_node_value<_type, false>& nd) const
	{
		return get_hash_code(_fKeyOfValue()(nd.access()));
	}
	YB_ATTR_nodiscard YB_PURE hash_code
	get_hash_code(const hash_node_value<_type, true>& nd) const ynothrow
	{
		return nd.cached_hash_code;
	}

	YB_ATTR_nodiscard YB_PURE size_t
	get_bucket_index(hash_code c, size_t n) const ynothrow
	{
		return _fRangeHash()(c, n);
	}
	YB_ATTR_nodiscard YB_PURE size_t
	get_bucket_index(const hash_node_value<_type, false>& nd, size_t n) const
		ynoexcept(noexcept(std::declval<const _fHash&>()
		(std::declval<const _tKey&>()))
		&& noexcept(std::declval<const _fRangeHash&>()(hash_code(), size_t())))
	{
		return _fRangeHash()(get_hash_code(_fKeyOfValue()(nd.access())), n);
	}
	YB_ATTR_nodiscard YB_PURE size_t
	get_bucket_index(const hash_node_value<_type, true>& nd, size_t n) const
		ynoexcept(
		noexcept(std::declval<const _fRangeHash&>()(hash_code(), size_t())))
	{
		return _fRangeHash()(nd.cached_hash_code, n);
	}

	void
	copy_code(hash_node_code_cache<false>&,
		const hash_node_code_cache<false>&) const ynothrow
	{}
	void
	copy_code(hash_node_code_cache<true>& to,
		const hash_node_code_cache<true>& from) const ynothrow
	{
		to.cached_hash_code = from.cached_hash_code;
	}

	void
	store_code(hash_node_code_cache<false>&, hash_code) const ynothrow
	{}
	void
	store_code(hash_node_code_cache<true>& nd, hash_code c) const ynothrow
	{
		nd.cached_hash_code = c;
	}

	YB_ATTR_nodiscard YB_PURE const _fHash&
	get_hash_function() const ynothrow
	{
		return base::get();
	}

public:
	YB_ATTR_nodiscard YB_PURE hasher
	hash_function() const
	{
		return get_hash_function();
	}

	friend void
	swap(hash_code_base& x, hash_code_base& y) ynothrow
	{
		std::swap(x.base::get(), y.base::get());
	}
};


//! \brief 缓存散列结果的特化散列表局部迭代器基类。
template<typename _tKey, typename _type, typename _fKeyOfValue, typename _fHash,
	typename _fRangeHash>
class table_local_iterator_base<_tKey, _type, _fKeyOfValue, _fHash, _fRangeHash,
	true> : public table_iterator_base<_type, true>
{
protected:
	using base_node_iter = table_iterator_base<_type, true>;
	using hash_code_mixin = hash_code_base<_tKey, _type, _fKeyOfValue,
		_fHash, _fRangeHash, true>;

	size_t bucket_id;
	size_t n_buckets;

	table_local_iterator_base() ynothrow = default;
	table_local_iterator_base(const hash_code_mixin&, hash_node<_type, true>*
		p, size_t bkt, size_t n) ynothrow
		: base_node_iter(p),
		bucket_id(bkt), n_buckets(n)
	{}

	/*!
	\brief 自增：自增基类迭代器，并判断是否到达局部范围的结束位置。
	\note 以缓存的散列值为参数调用 _fRangeHash 默认初始化对象判断位置。
	*/
	void
	increment()
	{
		base_node_iter::increment();
		if(this->p_node)
		{
			if(_fRangeHash()(this->p_node->cached_hash_code, n_buckets)
				!= bucket_id)
				this->p_node = {};
		}
	}

public:
	YB_ATTR_nodiscard YB_PURE size_t
	get_bucket() const ynothrow
	{
		return bucket_id;
	}
};


template<typename _type, bool = is_empty<_type>{}>
struct hash_code_storage
{
	replace_storage_t<_type> storage;

	YB_ATTR_nodiscard YB_PURE yconstfn_relaxed _type*
	get_table_ptr() ynothrow
	{
		return static_cast<_type*>(storage.access());
	}
	YB_ATTR_nodiscard YB_PURE yconstfn const _type*
	get_table_ptr() const ynothrow
	{
		return static_cast<const _type*>(storage.access());
	}
};

template<typename _type>
struct hash_code_storage<_type, true>
{
	static_assert(is_empty<_type>(), "Type shall be empty.");

	YB_ATTR_nodiscard YB_PURE yconstfn_relaxed _type*
	get_table_ptr() ynothrow
	{
		return reinterpret_cast<_type*>(this);
	}
	YB_ATTR_nodiscard YB_PURE yconstfn const _type*
	get_table_ptr() const ynothrow
	{
		return reinterpret_cast<const _type*>(this);
	}
};


template<typename _tKey, typename _type, typename _fKeyOfValue,
	typename _fHash, typename _fRangeHash>
using hash_code_for_local_iter = hash_code_storage<hash_code_base<_tKey,
	_type, _fKeyOfValue, _fHash, _fRangeHash, false>>;


//! \brief 不缓存散列结果的特化散列表局部迭代器基类。
template<typename _tKey, typename _type, typename _fKeyOfValue,
	typename _fHash, typename _fRangeHash>
class table_local_iterator_base<_tKey, _type, _fKeyOfValue, _fHash, _fRangeHash,
	false> : public hash_code_for_local_iter<_tKey, _type, _fKeyOfValue, _fHash,
	_fRangeHash>, public table_iterator_base<_type, false>
{
protected:
	using hash_code_mixin = hash_code_base<_tKey, _type, _fKeyOfValue, _fHash,
		_fRangeHash, false>;
	using node_iter_base = table_iterator_base<_type, false>;

	size_t bucket_id;
	size_t n_buckets = size_t(-1);

	table_local_iterator_base() ynothrow = default;
	table_local_iterator_base(const hash_code_mixin& base, hash_node<_type,
		false>* p, size_t bkt, size_t n) ynoexcept_spec(init(base))
		: node_iter_base(p),
		bucket_id(bkt), n_buckets(n)
	{
		init(base);
	}
	table_local_iterator_base(const table_local_iterator_base& i)
		 ynoexcept_spec(*i.get_table_ptr())
		: node_iter_base(i.p_node), bucket_id(i.bucket_id),
		n_buckets(i.n_buckets)
	{
		if(n_buckets != size_t(-1))
			init(*i.get_table_ptr());
	}
	~table_local_iterator_base()
	{
		if(n_buckets != size_t(-1))
			destroy();
	}

	table_local_iterator_base&
	operator=(const table_local_iterator_base& i)
	{
		if(n_buckets != -1)
			destroy();
		yunseq(this->p_node = i.p_node, bucket_id = i.bucket_id,
			n_buckets = i.n_buckets);
		if(n_buckets != -1)
			init(*i.get_table_ptr());
		return *this;
	}

	/*!
	\brief 自增：自增基类迭代器，并判断是否到达局部范围的结束位置。
	\note 调用散列表中的散列函数重新计算散列值判断位置。
	*/
	void
	increment()
	{
		node_iter_base::increment();
		if(this->p_node)
		{
			if(this->get_table_ptr()->get_bucket_index(*this->p_node, n_buckets)
				!= bucket_id)
				this->p_node = {};
		}
	}

	void
	init(const hash_code_mixin& base)
		ynoexcept(is_nothrow_copy_constructible<hash_code_mixin>())
	{
		::new(this->get_table_ptr()) hash_code_mixin(base);
	}

	void
	destroy() ynothrow
	{
		this->get_table_ptr()->~hash_code_mixin();
	}

public:
	YB_ATTR_nodiscard YB_PURE size_t
	get_bucket() const ynothrow
	{
		return bucket_id;
	}
};


template<typename _tKey, typename _type, typename _fKeyOfValue,
	typename _fHash, typename _fRangeHash, bool _bCached>
class table_local_iterator : public table_local_iterator_base<_tKey, _type,
	_fKeyOfValue, _fHash, _fRangeHash, _bCached>,
	public forward_iteratable<table_local_iterator<_tKey, _type, _fKeyOfValue,
	_fHash, _fRangeHash, _bCached>, _type&>
{
private:
	using base_type = table_local_iterator_base<_tKey, _type, _fKeyOfValue,
		_fHash, _fRangeHash, _bCached>;
	using hash_code_base = typename base_type::hash_code_base;

public:
	using pointer = _type*;
	using reference = _type&;

	table_local_iterator() ynothrow = default;
	table_local_iterator(const hash_code_base& base,
		hash_node<_type, _bCached>* p_nd, size_t bkt, size_t n) ynothrow
		: base_type(base, p_nd, bkt, n)
	{}

	YB_ATTR_nodiscard YB_PURE reference
	operator*() const ynothrowv
	{
		return this->p_node->access();
	}

	table_local_iterator&
	operator++() ynothrow
	{
		this->increment();
		return *this;
	}

public:
	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const table_local_iterator& x, const table_local_iterator& y)
		ynothrow
	{
		return x.p_node == y.p_node;
	}
};


template<typename _tKey, typename _type, typename _fKeyOfValue, typename _fHash,
	typename _fRangeHash, bool _bCached>
class table_local_const_iterator : public table_local_iterator_base<_tKey,
	_type, _fKeyOfValue, _fHash, _fRangeHash, _bCached>,
	public forward_iteratable<table_local_const_iterator<_tKey, _type,
	_fKeyOfValue, _fHash, _fRangeHash, _bCached>, const _type&>
{
private:
	using base_type = table_local_iterator_base<_tKey, _type, _fKeyOfValue,
		_fHash, _fRangeHash, _bCached>;
	using hash_code_base = typename base_type::hash_code_base;

public:
	using iterator_category = std::forward_iterator_tag;
	using pointer = const _type*;
	using reference = const _type&;

	table_local_const_iterator() ynothrow = default;
	table_local_const_iterator(const hash_code_base& base,
		hash_node<_type, _bCached>* p_nd, size_t bkt, size_t n) ynothrow
		: base_type(base, p_nd, bkt, n)
	{}
	table_local_const_iterator(const table_local_iterator<_tKey,
		_type, _fKeyOfValue, _fHash, _fRangeHash, _bCached>& x) ynothrow
		: base_type(x)
	{}

	YB_ATTR_nodiscard YB_PURE reference
	operator*() const ynothrowv
	{
		return this->p_node->access();
	}

	table_local_const_iterator&
	operator++() ynothrow
	{
		this->increment();
		return *this;
	}

public:
	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const table_local_const_iterator& x,
		const table_local_const_iterator& y) ynothrow
	{
		return x.p_node == y.p_node;
	}
};


template<typename _tKey, typename _type, typename _fKeyOfValue,
	typename _fPred, typename _fHash, typename _fRangeHash, class _tTraits>
class table_base
	: public hash_code_base<_tKey, _type, _fKeyOfValue, _fHash, _fRangeHash,
	_tTraits::hash_cached::value>, private compressed_pair_element<_fPred, 0>
{
public:
	using key_equal = _fPred;

private:
	using equal_base = compressed_pair_element<_fPred, 0>;
	using hash_cached = typename _tTraits::hash_cached;

protected:
	using hash_code_mixin = hash_code_base<_tKey, _type, _fKeyOfValue,
		_fHash, _fRangeHash, hash_cached{}>;
	using hash_code = typename hash_code_mixin::hash_code;

	table_base() = default;
	table_base(const _fHash& hf, const _fPred& eql)
		: hash_code_mixin(hf), equal_base(eql)
	{}

private:
	YB_ATTR_nodiscard YB_STATELESS static bool
	code_equals(hash_code, const hash_node_code_cache<false>&) ynothrow
	{
		return true;
	}
	YB_ATTR_nodiscard YB_PURE static bool
	code_equals(hash_code c, const hash_node_code_cache<true>& nd) ynothrow
	{
		return c == nd.cached_hash_code;
	}

protected:
	template<typename _tKeyOrTransKey>
	YB_ATTR_nodiscard bool
	code_key_equals(const _tKeyOrTransKey& k, hash_code c,
		const hash_node_value<_type, hash_cached{}>& nd) const
	{
		return code_equals(c, nd) && key_equals(k, nd);
	}

	template<typename _tKeyOrTransKey>
	YB_ATTR_nodiscard bool
	key_equals(const _tKeyOrTransKey& k, const _type& v) const
	{
		static_assert(is_invocable<const _fPred&, const _tKeyOrTransKey&,
			const _tKey&>(), "Key equality predicate shall be invocable with 2"
			" arguments of key type.");

		return equal_base::get()(k, _fKeyOfValue()(v));
	}
	template<typename _tKeyOrTransKey>
	YB_ATTR_nodiscard bool
	key_equals(const _tKeyOrTransKey& k,
		const hash_node_value<_type, hash_cached{}>& nd) const
	{
		return key_equals(k, nd.access());
	}

protected:
	YB_ATTR_nodiscard bool
	node_equals(const hash_node_value<_type, hash_cached{}>& x,
		const hash_node_value<_type, hash_cached{}>& y) const
	{
		return node_cache_equals(x, y)
			&& key_equals(_fKeyOfValue()(x.access()), y);
	}

private:
	YB_ATTR_nodiscard YB_STATELESS static bool
	node_cache_equals(const hash_node_code_cache<false>&,
		const hash_node_code_cache<false>&) ynothrow
	{
		return true;
	}
	YB_ATTR_nodiscard YB_PURE static bool
	node_cache_equals(const hash_node_code_cache<true>& x,
		const hash_node_code_cache<true>& y) ynothrow
	{
		return x.cached_hash_code == y.cached_hash_code;
	}

public:
	YB_ATTR_nodiscard YB_PURE key_equal
	key_eq() const
	{
		return equal_base::get();
	}

	friend void
	swap(table_base& x, table_base& y) ynothrow
	{
		swap(static_cast<hash_code_mixin&>(x),
			static_cast<hash_code_mixin&>(y));
		std::swap(x.equal_base::get(), y.equal_base::get());
	}
};


template<class _tNodeAlloc>
class table_alloc : private compressed_pair_element<_tNodeAlloc, 0>
{
private:
	using node_alloc_base = compressed_pair_element<_tNodeAlloc, 0>;

protected:
	table_alloc() = default;
	table_alloc(const table_alloc&) = default;
	table_alloc(table_alloc&&) = default;
	template<class _tAlloc>
	table_alloc(_tAlloc&& a)
		: node_alloc_base(yforward(a))
	{}

public:
	YB_ATTR_nodiscard YB_PURE _tNodeAlloc&
	get_node_allocator() ynothrow
	{
		return node_alloc_base::get();
	}
	YB_ATTR_nodiscard YB_PURE const _tNodeAlloc&
	get_node_allocator() const ynothrow
	{
		return node_alloc_base::get();
	}
};


template<typename _tKey, typename _fHash>
using cache_default = not_<and_<is_fast_hash<_fHash>,
	is_nothrow_invocable<const _fHash&, const _tKey&>>>;


// NOTE: Like %rb_tree::tree, member functions with
//	%std::initializer_list<value_type> are not supported in principle. However,
//	there is the exception when the size can help efficient implementation.
template<typename _tKey, typename _type, class _tAlloc, typename _fKeyOfValue,
	typename _fPred, typename _fHash, class _tHashingTraits, class _tTraits>
class table : public table_base<_tKey, _type, _fKeyOfValue, _fPred, _fHash,
	typename _tHashingTraits::range_hash, _tTraits>, public rehash_base<_tKey,
	_type, _tAlloc, _fKeyOfValue, _fPred, _fHash, _tHashingTraits, _tTraits>,
	private table_alloc<rebind_alloc_t<_tAlloc,
	hash_node<_type, _tTraits::hash_cached::value>>>
{
public:
	using key_type = _tKey;
	using value_type = _type;
	/*!
	\see ISO C++20 [allocator.requirements] 。
	\see LWG 274 。
	\see LWG 2447 。
	*/
	static_assert(is_unqualified_object<value_type>(),
		"The value type for allocator shall be an unqualified object type.");
	/*!
	\see ISO C++20 [container.alloc.reqmts]/2 。
	\see WG21 P1463R1 。
	*/
	static_assert(is_allocator_for<_tAlloc, value_type>(),
		"Value type mismatched to the allocator found.");
	// NOTE: The member %hasher comes from %hash_code_mixin implied by
	//	%table_mixin.
	// NOTE: The member %key_equal comes from %table_mixin.
	using allocator_type = _tAlloc;

private:
	static_assert(is_nothrow_default_constructible<_fKeyOfValue>(),
		"Key extraction functor shall be nothrow default constructible");
	ynoexcept_assert("Key extraction functor shall be noexcept callable.",
		std::declval<const _fKeyOfValue&>()(std::declval<_type>()));
	using range_hash = typename _tHashingTraits::range_hash;
	static_assert(is_nothrow_default_constructible<range_hash>(),
		"Functor used to map hash code to bucket index shall be nothrow default"
		" constructible");
	ynoexcept_assert("Functor used to map hash code to bucket index shall be"
		" noexcept.", std::declval<const range_hash&>()(size_t(), size_t()));
	using table_mixin = table_base<_tKey, _type, _fKeyOfValue, _fPred, _fHash,
		range_hash, _tTraits>;
	using hash_code_mixin = typename table_mixin::hash_code_mixin;
	using rehash_mixin = rehash_base<_tKey, _type, _tAlloc, _fKeyOfValue,
		_fPred, _fHash, _tHashingTraits, _tTraits>;
	using traits_type = _tTraits;
	using hash_cached = typename traits_type::hash_cached;
	using value_node = hash_node<_type, hash_cached{}>;
	using node_allocator = rebind_alloc_t<_tAlloc, value_node>;
	using table_alloc_mixin = table_alloc<node_allocator>;
	using ator_traits = allocator_traits<allocator_type>;

public:
	using pointer = typename ator_traits::pointer;
	using const_pointer = typename ator_traits::const_pointer;
	using reference = value_type&;
	using const_reference = const value_type&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	/*!
	\warning 不检查 const 安全性。若修改键破坏类不变量，则行为未定义。
	\note 对集合容器实现可调整 iterator 成员为 const_iterator 避免。
	\see LWG 103 。
	*/
	using iterator = table_iterator<_type, hash_cached{}>;
	using const_iterator = table_const_iterator<_type, hash_cached{}>;
	/*!
	\warning 不检查 const 安全性。若修改键破坏类不变量，则行为未定义。
	\note 对集合容器实现可调整 local_iterator 成员为 const_local_iterator 避免。
	\see LWG 103 。
	*/
	using local_iterator = table_local_iterator<_tKey, _type, _fKeyOfValue,
		_fHash, range_hash, hash_cached{}>;
	using const_local_iterator = table_local_const_iterator<_tKey, _type,
		_fKeyOfValue, _fHash, range_hash, hash_cached{}>;
	using node_type = node_handle<_tKey, _type, node_allocator>;
	//! \note 集合容器使用常量迭代器以避免修改键破坏类不变量。
	using insert_return_type = node_insert_return<
		cond_t<is_same<_tKey, _type>, const_iterator, iterator>, node_type>;

protected:
	using base_ptr = hash_node_base*;
	using link_type = value_node*;

private:
	using node_ator_traits = allocator_traits<node_allocator>;
	using node_pointer = typename node_ator_traits::pointer;
	using equal_alloc_or_pocma = bool_<node_ator_traits::is_always_equal::value
		|| node_ator_traits::propagate_on_container_move_assignment::value>;
	template<bool _bNoRealloc = true>
	using comp_nothrow_move = and_<bool_<_bNoRealloc>,
		is_nothrow_copy_constructible<_fHash>,
		is_nothrow_copy_constructible<_fPred>>;
	// XXX: See $2022-01 @ %Documentation::Workflow.
	using allocator_reference = conditional_t<sizeof(node_allocator)
		<= sizeof(void*), node_allocator, node_allocator&>;
	using node_value_type = hash_node_value<_type, hash_cached{}>;
	using buckets_allocator = rebind_alloc_t<node_allocator, base_ptr>;
	using bucket_ator_tratis = std::allocator_traits<buckets_allocator>;
	using buckets_ptr = base_ptr*;
	using rehash_type = typename _tHashingTraits::rehash_policy;
	using rehash_state = typename rehash_type::state_t;
	using unique_keys = typename traits_type::unique_keys;
	using hash_code = typename table_mixin::hash_code;
	using ireturn_type
		= cond_t<unique_keys, std::pair<iterator, bool>, iterator>;
	struct alloc_node
	{
		table& table_ref;

		alloc_node(table& tbl) ynothrow
			: table_ref(tbl)
		{}

		template<typename _tParam>
		YB_ATTR_nodiscard YB_ATTR_returns_nonnull link_type
		operator()(_tParam&& arg) const
		{
			return table_ref.create_node(yforward(arg));
		}

		template<typename _tParam>
		YB_ATTR_nodiscard link_type
		reconstruct(link_type nd, _tParam&& arg) const
		{
#if false
			allocator_reference a(table_ref.get_node_allocator());

			// XXX: This is no more effcient by skipping node reconstruction.
			node_ator_traits::destroy(a, nd->access_ptr());

			allocator_guard<allocator_reference> gd(std::pointer_traits<
				typename node_ator_traits::pointer>::pointer_to(nd), a);

			node_ator_traits::construct(a, nd->access_ptr(), yforward(arg));
			gd.release();
#else
			table_ref.destroy_node(nd);
			table_ref.construct_node(std::pointer_traits<typename
				node_ator_traits::pointer>::pointer_to(*nd), yforward(arg));
#endif
			return nd;
		}
	};
	struct reuse_or_alloc_node : protected alloc_node, private noncopyable
	{
		mutable link_type nodes;

		reuse_or_alloc_node(table& tbl) ynothrow
			: reuse_or_alloc_node(tbl, tbl.node_begin())
		{}
		reuse_or_alloc_node(table& tbl, link_type nd) ynothrow
			: alloc_node(tbl), nodes(nd)
		{}
		~reuse_or_alloc_node()
		{
			alloc_node::table_ref.erase_node(nodes);
		}

		template<typename _tParam>
		YB_ATTR_nodiscard YB_ATTR_returns_nonnull link_type
		operator()(_tParam&& arg) const
		{
			if(nodes)
			{
				auto nd(nodes);

				nodes = nodes->next();
				nd->p_next = {};
				return alloc_node::reconstruct(nd, yforward(arg));
			}
			return alloc_node::operator()(yforward(arg));
		}
	};
	struct scoped_node final : private noncopyable
	{
		table& table_ref;
		link_type p_saved_node;

		scoped_node(link_type p_nd, table& tbl) ynothrow
			: table_ref(tbl), p_saved_node(p_nd)
		{}
		template<typename... _tParams>
		scoped_node(table& tbl, _tParams&&... args)
			: table_ref(tbl), p_saved_node(tbl.create_node(yforward(args)...))
		{}
		~scoped_node()
		{
			if(p_saved_node)
				table_ref.drop_node(p_saved_node);
		}
	};
	struct hash_code_base_access final : hash_code_mixin
	{
		using hash_code_mixin::get_bucket_index;
	};

	buckets_ptr p_buckets = &p_single_bucket;
	size_type n_buckets = 1;
	hash_node_base before_begin_node;
	size_type n_elements = 0;
	rehash_type current_rehash;
	base_ptr p_single_bucket = {};

public:
	table() = default;
	explicit
	table(const allocator_type& a)
		: table_alloc_mixin(node_allocator(a))
	{}

private:
	table(const _fHash& hf, const _fPred& eql, const allocator_type& a)
		: table_mixin(hf, eql), table_alloc_mixin(node_allocator(a))
	{}

public:
	// NOTE: This is specific to euqal keys (indicated by the last %false_
	//	parameter). Constructor is not provided here for uniques because it can
	//	be constructed like 'table(n, hf, eql, a)' followed by a call to
	//	%insert_range_unique.
	template<typename _tIn>
	table(_tIn first, _tIn last, size_type n, const _fHash& hf,
		const _fPred& eql, const allocator_type& a, false_)
		: table(hf, eql, a)
	{
		const auto count(details::distance_fw(first, last));
		const auto n_bkt(current_rehash.get_next_bucket(
			std::max(current_rehash.get_bucket_for_elements(count), n)));

		if(n_bkt > n_buckets)
			yunseq(p_buckets = allocate_buckets(n_bkt), n_buckets = n_bkt);

		alloc_node node_gen(*this);

		for(; first != last; ++first)
			insert_equal(*first, node_gen);
	}

public:
	table(std::initializer_list<value_type> il, size_type n = 0,
		const _fHash& hf = _fHash(), const _fPred& eql = _fPred(),
		const _tAlloc& a = _tAlloc())
		: table(il.begin(), il.end(), n, hf, eql, a, unique_keys())
	{}
	table(const table& x)
		: table_mixin(x), rehash_mixin(x), table_alloc_mixin(
		node_ator_traits::select_on_container_copy_construction(
		x.get_node_allocator())),
		p_buckets(), n_buckets(x.n_buckets), n_elements(x.n_elements),
		current_rehash(x.current_rehash)
	{
		alloc_node alloc_node_gen(*this);

		assign(x, alloc_node_gen);
	}
	table(const table& x, const allocator_type& a)
		: table_mixin(x), rehash_mixin(x), table_alloc_mixin(node_allocator(a)),
		p_buckets(), n_buckets(x.n_buckets), n_elements(x.n_elements),
		current_rehash(x.current_rehash)
	{
		alloc_node alloc_node_gen(*this);

		assign(x, alloc_node_gen);
	}
	explicit
	table(size_type n, const _fHash& hf = _fHash(),
		const _fPred& eql = _fPred(), const _tAlloc& a = _tAlloc())
		: table(hf, eql, a)
	{
		const auto n_bkt(current_rehash.get_next_bucket(n));

		if(n_bkt > n_buckets)
		{
			p_buckets = allocate_buckets(n_bkt);
			n_buckets = n_bkt;
		}
	}
	table(table&& x) ynoexcept(comp_nothrow_move<>())
		: table(std::move(x), std::move(x.get_node_allocator()),
		true_())
	{}
	table(table&& x, const allocator_type& a) ynoexcept(
		comp_nothrow_move<node_ator_traits::is_always_equal::value>())
		: table(std::move(x), node_allocator(a),
		typename node_ator_traits::is_always_equal())
	{}

private:
	table(table&& x, node_allocator&& a, false_)
		: table_mixin(x), rehash_mixin(x), table_alloc_mixin(std::move(a)),
		p_buckets(), n_buckets(x.n_buckets), n_elements(x.n_elements),
		current_rehash(x.current_rehash)
	{
		if(x.get_node_allocator() == get_node_allocator())
		{
			if(x.uses_single_bucket())
			{
				p_buckets = &p_single_bucket;
				p_single_bucket = x.p_single_bucket;
			}
			else
				p_buckets = x.p_buckets;
			update_bbegin(x.node_begin());
			x.reset_state();
		}
		else
		{
			alloc_node alloc_gen(*this);

			assign(cond_t<is_throwing_move_copyable<value_type>,
				const table&, table&&>(x), alloc_gen);
			x.clear();
		}
	}
	table(table&& x, node_allocator&& a, true_) ynoexcept(comp_nothrow_move<>())
		: table_mixin(x), rehash_mixin(x), table_alloc_mixin(std::move(a)),
		p_buckets(x.p_buckets), n_buckets(x.n_buckets), before_begin_node(
		x.before_begin_node.p_next), n_elements(x.n_elements),
		current_rehash(x.current_rehash)
	{
		if(x.uses_single_bucket())
		{
			p_buckets = &p_single_bucket;
			p_single_bucket = x.p_single_bucket;
		}
		update_bbegin();
		x.reset_state();
	}

public:
	~table() ynothrow
	{
		ynoexcept_assert(
			"Non-cached hashing requires noexcept in computing the hash code.",
			std::declval<const hash_code_base_access&>().get_bucket_index(
			std::declval<const node_value_type&>(), size_t()));

		clear();
		deallocate_buckets();
	}

	table&
	operator=(const table& x)
	{
		if(&x != this)
		{
			if(node_ator_traits::propagate_on_container_copy_assignment::value)
			{
				auto& this_alloc(get_node_allocator());
				auto& that_alloc(x.get_node_allocator());

				if(!node_ator_traits::is_always_equal::value
					&& this_alloc != that_alloc)
				{
					erase_node(node_begin());
					before_begin_node.p_next = {};
					deallocate_buckets();
					p_buckets = {};
					ystdex::alloc_on_copy(this_alloc, that_alloc);
					table_mixin::operator=(x);
					yunseq(n_buckets = x.n_buckets,
						n_elements = x.n_elements);
					current_rehash = x.current_rehash;

					alloc_node alloc_node_gen(*this);

					try
					{
						assign(x, alloc_node_gen);
					}
					catch(...)
					{
						reset_state();
						throw;
					}
					return *this;
				}
				ystdex::alloc_on_copy(this_alloc, that_alloc);
			}
			assign_elements(x);
		}
		return *this;
	}
	//! \see LWG 2839 。
	table&
	operator=(table&& x)
		ynoexcept(and_<equal_alloc_or_pocma, is_nothrow_move_assignable<_fHash>,
		is_nothrow_move_assignable<_fPred>>())
	{
		// NOTE: Self-move is respected, see the comment in move %operator= in
		//	%rb_tree::tree for rationale. However, the concrete implementation
		//	may differ in the state after move. See %move_assign_elements for
		//	details.
		move_assign_elements(x, equal_alloc_or_pocma());
		return *this;
	}

	//! \see WG21 N3068 。
	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const table& x, const table& y)
	{
		return x.size() == y.size() && x.equals_same_size(unique_keys(), y);
	}

	template<class _tHashTable, class _tNodeGen>
	void
	assign(_tHashTable&& x, const _tNodeGen& node_gen)
	{
		buckets_ptr p = {};

		if(!p_buckets)
			p_buckets = p = allocate_buckets(n_buckets);
		if(x.before_begin_node.p_next)
			try
			{
				using fwd_val_t = cond_t<is_lvalue_reference<_tHashTable>,
					const value_type&, value_type&&>;
				auto p_nd(x.node_begin());
				link_type p_this_nd(node_gen(fwd_val_t(p_nd->access())));

				this->copy_code(*p_this_nd, *p_nd);
				update_bbegin(p_this_nd);

				auto p_prev(p_this_nd);

				for(p_nd = p_nd->next(); p_nd; p_nd = p_nd->next())
				{
					p_this_nd = node_gen(fwd_val_t(p_nd->access()));
					p_prev->p_next = p_this_nd;
					this->copy_code(*p_this_nd, *p_nd);

					const auto bkt(get_bucket_index(*p_this_nd));

					if(!p_buckets[bkt])
						p_buckets[bkt] = p_prev;
					p_prev = p_this_nd;
				}
			}
			catch(...)
			{
				clear();
				if(p)
					deallocate_buckets();
				throw;
			}
	}

	template<class _tHashTable>
	void
	assign_elements(_tHashTable&& x)
	{
		buckets_ptr former_buckets = {};
		const auto former_bucket_count(n_buckets);
		const auto& former_state(current_rehash.get_state());

		if(n_buckets != x.n_buckets)
		{
			former_buckets = p_buckets;
			p_buckets = allocate_buckets(x.n_buckets);
			n_buckets = x.n_buckets;
		}
		else
			clear_bucket_vec();
		try
		{
			table_mixin::operator=(yforward(x));
			n_elements = x.n_elements;
			current_rehash = x.current_rehash;

			reuse_or_alloc_node roan(*this);

			before_begin_node.p_next = {};
			assign(yforward(x), roan);
			if(former_buckets)
				deallocate_buckets(former_buckets, former_bucket_count);
		}
		catch(...)
		{
			if(former_buckets)
			{
				deallocate_buckets();
				current_rehash.reset_state(former_state);
				yunseq(p_buckets = former_buckets,
					n_buckets = former_bucket_count);
			}
			clear_bucket_vec();
			throw;
		}
	}

	template<typename _tIn>
	void
	assign_unique(_tIn first, _tIn last, size_t size)
	{
		reuse_or_alloc_node roan(*this);

		assign_range_prepare(first, last, size);
		insert_range_unique(first, last, roan);
	}

	template<typename _tIn>
	void
	assign_equal(_tIn first, _tIn last, size_t size)
	{
		reuse_or_alloc_node roan(*this);

		assign_range_prepare(first, last, size);
		insert_range_equal(first, last, roan);
	}

private:
	template<typename _tIn>
	void
	assign_range_prepare(_tIn first, _tIn last, size_t size)
	{
		YAssert(size_t(std::distance(first, last)) == size,
			"Invalid range size found.");
		yunused(first), yunused(last);
		before_begin_node.p_next = {};
		clear();

		const auto n(current_rehash.get_bucket_for_elements(size));

		if(n_buckets < n)
			rehash(n);
	}

	void
	clear_bucket_vec() ynothrowv
	{
		std::memset(p_buckets, 0, n_buckets * sizeof(base_ptr));
	}

	/*!
	\brief 转移赋值可能不相等的不在转移赋值时传播的分配器的容器元素。
	\note 从不相等分配器转移元素可能复制而不是转移对象。
	*/
	void
	move_assign_elements(table& x, false_)
	{
		// NOTE: As libstdc++. See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85828.
		//	This does not touch the elements for self-move if the allocators are
		//	equal, otherwise the elements are finally cleared.
		if(get_node_allocator() == x.get_node_allocator())
			move_assign_elements(x, true_());
		else
		{
			assign_elements(x);
			x.clear();
		}
	}
	//! \brief 转移赋值相等或在转移赋值时传播的分配器的容器元素。
	void
	move_assign_elements(table& x, true_)
	{
		// NOTE: This does not touch the elements for self-move. See also the
		//	comment in move %operator=.
		if(YB_LIKELY(std::addressof(x) != this))
		{
			erase_node(node_begin());
			deallocate_buckets();
			table_mixin::operator=(std::move(x));
			current_rehash = x.current_rehash;
			if(!x.uses_single_bucket())
				p_buckets = x.p_buckets;
			else
			{
				p_buckets = &p_single_bucket;
				p_single_bucket = x.p_single_bucket;
			}
			n_buckets = x.n_buckets;
			before_begin_node.p_next = x.before_begin_node.p_next;
			n_elements = x.n_elements;
			ystdex::alloc_on_move(get_node_allocator(),
				x.get_node_allocator());
			update_bbegin();
			x.reset_state();
		}
	}

	YB_ATTR_nodiscard bool
	equals_same_size(false_, const table& y) const
	{
		yconstraint(size() == y.size());
		for(auto i_x = begin(); i_x != end();)
		{
			size_t x_count(1);
			auto i_x_end(i_x);

			for(++i_x_end; i_x_end != end() && this->key_eq()(
				_fKeyOfValue()(*i_x), _fKeyOfValue()(*i_x_end));
				++i_x_end)
				++x_count;

			const auto ybkt(y.get_bucket_index(*i_x.p_node));
			auto p_y_prev(y.p_buckets[ybkt]);

			if(!p_y_prev)
				return {};

			auto p_nd_y(static_cast<link_type>(p_y_prev->p_next));

			while(!this->key_eq()(_fKeyOfValue()(p_nd_y->access()),
				_fKeyOfValue()(*i_x)))
			{
				auto y_ref_n = p_nd_y;

				for(p_nd_y = p_nd_y->next(); p_nd_y; p_nd_y = p_nd_y->next())
					if(!y.node_equals(*y_ref_n, *p_nd_y))
						break;
				if(!p_nd_y || y.get_bucket_index(*p_nd_y) != ybkt)
					return {};
			}

			const_iterator i_y(p_nd_y);

			for(auto i_y_end = i_y; i_y_end != y.end(); ++i_y_end)
				if(--x_count == 0)
					break;
			if(x_count != 0)
				return {};
			if(!std::is_permutation(i_x, i_x_end, i_y))
				return {};
			i_x = i_x_end;
		}
		return true;
	}
	YB_ATTR_nodiscard bool
	equals_same_size(true_, const table& y) const
	{
		yconstraint(size() == y.size());
		for(auto i_x = begin(); i_x != end(); ++i_x)
		{
			const auto ybkt(y.get_bucket_index(*i_x.p_node));

			if(const auto p_prev = y.p_buckets[ybkt])
			{
				for(auto p_nd = static_cast<link_type>(p_prev->p_next);
					p_nd->access() != *i_x; p_nd = p_nd->next())
					if(!p_nd->p_next
						|| y.get_bucket_index(*p_nd->next()) != ybkt)
						return {};
			}
			else
				return {};
		}
		return true;
	}

public:
	YB_ATTR_nodiscard YB_PURE allocator_type
	get_allocator() const ynothrow
	{
		return allocator_type(get_node_allocator());
	}

	using table_alloc_mixin::get_node_allocator;

protected:
	// XXX: This implies %YB_ALLOCATOR and %YB_returns_nonnull for non-fancy
	//	pointers.
	YB_ATTR_nodiscard node_pointer
	allocate_node()
	{
		allocator_reference a(get_node_allocator());

		return node_ator_traits::allocate(a, 1);
	}

	void
	deallocate_node(node_pointer p) ynothrow
	{
		allocator_reference a(get_node_allocator());

		node_ator_traits::deallocate(a, p, 1);
	}

	void
	deallocate_node_link(link_type nd) ynothrow
	{
		deallocate_node(std::pointer_traits<
			typename node_ator_traits::pointer>::pointer_to(*nd));
	}

	//! \pre 断言：第一参数非空。
	template<typename... _tParams>
	void
	construct_node(node_pointer p, _tParams&&... args)
	{
		YAssertNonnull(p);

		allocator_reference a(get_node_allocator());
		// NOTE: This should be same to %deallocate_node(p) on exception thrown.
		allocator_guard<allocator_reference> gd(p, a);
		const auto nd(ystdex::to_allocated(p));

		try
		{
			::new(nd) value_node;
			node_ator_traits::construct(a, nd->access_ptr(), yforward(args)...);
		}
		catch(...)
		{
			nd->~value_node();
		}
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
		allocator_reference a(get_node_allocator());

		node_ator_traits::destroy(a, nd->access_ptr());
		nd->~value_node();
	}

	void
	drop_node(link_type nd) ynothrow
	{
		destroy_node(nd);
		deallocate_node_link(nd);
	}

	YB_ATTR_nodiscard YB_ALLOCATOR YB_ATTR_returns_nonnull buckets_ptr
	allocate_buckets_base(size_t n)
	{
		buckets_allocator alloc(get_node_allocator());
		const buckets_ptr
			p(ystdex::to_address(bucket_ator_tratis::allocate(alloc, n)));

		std::memset(p, 0, n * sizeof(base_ptr));
		return p;
	}

public:
	YB_ATTR_nodiscard YB_PURE iterator
	begin() ynothrow
	{
		return iterator(node_begin());
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	begin() const ynothrow
	{
		return const_iterator(node_begin());
	}
	YB_ATTR_nodiscard YB_PURE local_iterator
	begin(size_type bkt)
	{
		return local_iterator(*this, bucket_id_begin(bkt), bkt, n_buckets);
	}
	YB_ATTR_nodiscard YB_PURE const_local_iterator
	begin(size_type bkt) const
	{
		return
			const_local_iterator(*this, bucket_id_begin(bkt), bkt, n_buckets);
	}

	YB_ATTR_nodiscard YB_PURE iterator
	end() ynothrow
	{
		return iterator();
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	end() const ynothrow
	{
		return const_iterator();
	}
	YB_ATTR_nodiscard YB_PURE local_iterator
	end(size_type bkt)
	{
		return local_iterator(*this, {}, bkt, n_buckets);
	}
	YB_ATTR_nodiscard YB_PURE const_local_iterator
	end(size_type bkt) const
	{
		return const_local_iterator(*this, {}, bkt, n_buckets);
	}

	YB_ATTR_nodiscard YB_PURE const_iterator
	cbegin() const ynothrow
	{
		return const_iterator(node_begin());
	}
	//! \see LWG 691 。
	YB_ATTR_nodiscard YB_PURE const_local_iterator
	cbegin(size_type bkt) const ynothrow
	{
		return
			const_local_iterator(*this, bucket_id_begin(bkt), bkt, n_buckets);
	}

	YB_ATTR_nodiscard YB_PURE const_iterator
	cend() const ynothrow
	{
		return const_iterator();
	}
	YB_ATTR_nodiscard YB_PURE const_local_iterator
	cend(size_type bkt) const ynothrow
	{
		return const_local_iterator(*this, {}, bkt, n_buckets);
	}

	YB_ATTR_nodiscard YB_PURE size_type
	size() const ynothrow
	{
		return n_elements;
	}

	YB_ATTR_nodiscard YB_PURE bool
	empty() const ynothrow
	{
		return size() == 0;
	}

	YB_ATTR_nodiscard YB_PURE size_type
	max_size() const ynothrow
	{
		return node_ator_traits::max_size(get_node_allocator());
	}

	node_type
	extract(const_iterator position)
	{
		const auto bkt(get_bucket_index(*position.p_node));

		return extract_node(bkt, get_previous_node(bkt, position.p_node));
	}
	node_type
	extract(const _tKey& k)
	{
		node_type nh;
		const auto code(this->get_hash_code(k));
		size_t bkt(get_bucket_index(code));

		if(base_ptr p_prev = find_before_node(bkt, k, code))
			nh = extract_node(bkt, p_prev);
		return nh;
	}

private:
	node_type
	extract_node(size_t bkt, base_ptr p_prev)
	{
		auto nd(link_type(p_prev->p_next));

		if(p_prev == p_buckets[bkt])
			remove_bucket_begin(bkt, nd->next(),
				nd->p_next ? get_bucket_index(*nd->next()) : 0);
		else if(nd->p_next)
		{
			const auto next_bkt(get_bucket_index(*nd->next()));

			if(next_bkt != bkt)
				p_buckets[next_bkt] = p_prev;
		}
		p_prev->p_next = nd->p_next;
		nd->p_next = {};
		--n_elements;
		return {nd, get_node_allocator()};
	}

public:
	insert_return_type
	reinsert_node_unique(node_type&& nh)
	{
		if(nh.empty())
			return end();

		YAssert(get_allocator() == nh.get_allocator(),
			"Mismatched allocators found.");

		const auto& k(nh.p_key());
		const auto c(this->get_hash_code(k));
		const auto bkt(get_bucket_index(c));
		insert_return_type ret;

		if(const auto p_nd = find_node(bkt, k, c))
			yunseq(ret.node = std::move(nh), ret.position = iterator(p_nd));
		else
		{
			ret.position = insert_unique_node(bkt, c, nh.ptr);
			yunseq(nh.ptr = {}, ret.inserted = true);
		}
		return ret;
	}

	iterator
	reinsert_node_equal(const_iterator hint, node_type&& nh)
	{
		if(nh.empty())
			return end();

		YAssert(get_allocator() == nh.get_allocator(),
			"Mismatched allocators found.");

		auto ret(insert_equal_node(hint.p_node, this->get_hash_code(nh.p_key()),
			nh.ptr));

		nh.ptr = {};
		return ret;
	}

private:
	YB_ATTR_nodiscard YB_PURE base_ptr
	find_before_node(const key_type& k)
	{
		auto p_prev(&before_begin_node);

		if(p_prev->p_next)
			for(auto p = link_type(p_prev->p_next); p; p = p->next())
			{
				if(this->key_equals(k, *p))
					return p_prev;
				p_prev = p;
			}
		return {};
	}
	template<typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE base_ptr
	find_before_node(size_t bkt, const _tKeyOrTransKey& k, hash_code c) const
	{
		if(auto p_prev = p_buckets[bkt])
			for(auto p = link_type(p_prev->p_next); ; p = p->next())
			{
				if(this->code_key_equals(k, c, *p))
					return p_prev;
				if(!p->p_next || get_bucket_index(*p->next()) != bkt)
					break;
				p_prev = p;
			}
		return {};
	}

	template<typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE link_type
	find_node(size_type bkt, const _tKeyOrTransKey& key, hash_code c) const
	{
		if(const auto p_prev = find_before_node(bkt, key, c))
			return link_type(p_prev->p_next);
		return {};
	}

	YB_NONNULL(3) void
	insert_bucket_begin(size_t bkt, link_type p) ynothrowv
	{
		if(p_buckets[bkt])
			insert_bucket_hook(*p_buckets[bkt], *p);
		else
		{
			insert_bucket_hook(before_begin_node, *p);
			if(p->p_next)
				p_buckets[get_bucket_index(*p->next())] = p;
			p_buckets[bkt] = &before_begin_node;
		}
	}

	static void
	insert_bucket_hook(hash_node_base& prev_nd, hash_node_base& nd) ynothrow
	{
		nd.p_next = prev_nd.p_next;
		prev_nd.p_next = &nd;
	}

	void
	remove_bucket_begin(size_type bkt, link_type next, size_type next_bkt)
		ynothrowv
	{
		if(!next || next_bkt != bkt)
		{
			if(next)
				p_buckets[next_bkt] = p_buckets[bkt];
			if(&before_begin_node == p_buckets[bkt])
				before_begin_node.p_next = next;
			p_buckets[bkt] = {};
		}
	}

	YB_ATTR_nodiscard YB_PURE base_ptr
	get_previous_node(size_type bkt, link_type p_nd) ynothrowv
	{
		auto p_prev(p_buckets[bkt]);

		while(p_prev->p_next != p_nd)
			p_prev = p_prev->p_next;
		return p_prev;
	}

	YB_ATTR_nodiscard YB_PURE std::pair<const_iterator, hash_code>
	compute_hash_code(const_iterator hint, const key_type& k) const
	{
		if(in_small_size_threshold())
		{
			if(hint != cend())
			{
				for(auto i = hint; i != cend(); ++i)
					if(this->key_equals(k, *i.p_node))
						return {i, this->get_hash_code(*i.p_node)};
			}
			for(auto i = cbegin(); i != hint; ++i)
				if(this->key_equals(k, *i.p_node))
					return {i, this->get_hash_code(*i.p_node)};
		}
		return {hint, this->get_hash_code(k)};
	}

	iterator
	insert_unique_node(size_type bkt, hash_code c, link_type nd,
		size_type n = 1)
	{
		const auto& saved_state(current_rehash.get_state());
		const auto pr(current_rehash.need_rehash(n_buckets, n_elements, n));

		if(pr.first)
		{
			rehash_with_state(pr.second, saved_state);
			bkt = get_bucket_index(c);
		}
		this->store_code(*nd, c);
		insert_bucket_begin(bkt, nd);
		++n_elements;
		return iterator(nd);
	}

	iterator
	insert_equal_node(link_type hint, hash_code c, link_type nd)
	{
		const auto& saved_state(current_rehash.get_state());
		const auto pr(current_rehash.need_rehash(n_buckets, n_elements, 1));

		if(pr.first)
			rehash_with_state(pr.second, saved_state);
		this->store_code(*nd, c);

		const auto& k(_fKeyOfValue()(nd->access()));
		const auto bkt(get_bucket_index(c));

		if(const auto p_prev = YB_UNLIKELY(hint) && this->code_key_equals(k, c,
			*hint) ? hint : find_before_node(bkt, k, c))
		{
			insert_bucket_hook(*p_prev, *nd);
			if(YB_UNLIKELY(p_prev == hint))
				if(nd->p_next && !this->code_key_equals(k, c, *nd->next()))
				{
					const auto next_bkt(get_bucket_index(*nd->next()));

					if(next_bkt != bkt)
						p_buckets[next_bkt] = nd;
				}
		}
		else
			insert_bucket_begin(bkt, nd);
		++n_elements;
		return iterator(nd);
	}

public:
	template<typename _tParam>
	std::pair<iterator, bool>
	insert_unique(_tParam&& v)
	{
		alloc_node an(*this);

		return insert_unique(yforward(v), an);
	}
	template<typename _tParam, class _tNodeGen>
	std::pair<iterator, bool>
	insert_unique(_tParam&& v, const _tNodeGen& node_gen)
	{
		const auto& k(_fKeyOfValue()(v));

		if(in_small_size_threshold())
			for(auto i = begin(); i != end(); ++i)
				if(this->key_equals(k, *i.p_node))
					return {i, {}};

		const auto c(this->get_hash_code(k));
		const auto bkt(get_bucket_index(c));

		if(!in_small_size_threshold())
			if(link_type nd = find_node(bkt, k, c))
				return {iterator(nd), {}};

		scoped_node nd(node_gen(yforward(v)), *this);
		const auto i(insert_unique_node(bkt, c, nd.p_saved_node));

		nd.p_saved_node = {};
		return {i, true};
	}

	template<typename _tParam>
	iterator
	insert_equal(_tParam&& v)
	{
		alloc_node an(*this);

		return insert_equal(yforward(v), an);
	}
	template<typename _tParam, class _tNodeGen>
	inline iterator
	insert_equal(_tParam&& v, const _tNodeGen& node_gen)
	{
		return insert_hint_equal(cend(), yforward(v), node_gen);
	}

	template<typename _tParam>
	iterator
	insert_hint_unique(const_iterator position, _tParam&& v)
	{
		alloc_node an(*this);

		return insert_hint_unique(position, yforward(v), an);
	}
	template<typename _tParam, class _tNodeGen>
	inline iterator
	insert_hint_unique(const_iterator, _tParam&& arg, const _tNodeGen& node_gen)
	{
		// XXX: The hint is ignored.
		return insert_unique(yforward(arg), node_gen).first;
	}

	template<typename _tParam>
	iterator
	insert_hint_equal(const_iterator position, _tParam&& v)
	{
		alloc_node an(*this);

		return insert_hint_equal(position, yforward(v), an);
	}
	template<typename _tParam, class _tNodeGen>
	iterator
	insert_hint_equal(const_iterator position, _tParam&& v,
		const _tNodeGen& node_gen)
	{
		scoped_node nd(node_gen(yforward(v)), *this);
		const auto pr(this->compute_hash_code(
			position, _fKeyOfValue()(nd.p_saved_node->access())));
		const auto
			i(insert_equal_node(pr.first.p_node, pr.second, nd.p_saved_node));

		nd.p_saved_node = {};
		return i;
	}

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
			// XXX: As %emplace_hint_unique, the hint is ignored.
			emplace_unique(*first);
	}
	template<typename _tIn, class _tNodeGen>
	void
	insert_range_unique(_tIn first, _tIn last, const _tNodeGen& node_gen)
	{
		for(; first != last; ++first)
			// XXX: As %insert_hint_unique, the hint is ignored.
			insert_unique(*first, node_gen);
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
	// XXX: Ditto.
	template<typename _tIn, class _tNodeGen>
	void
	insert_range_equal(_tIn first, _tIn last, const _tNodeGen& node_gen)
	{
		size_type n(details::distance_fw(first, last));

		if(n != 0)
		{
			const rehash_state& saved_state(current_rehash.get_state());
			const auto pr(current_rehash.need_rehash(n_buckets,
				n_elements, n));

			if(pr.first)
				rehash_with_state(pr.second, saved_state);
			for(; first != last; ++first)
				insert_equal(*first, node_gen);
		}
	}

	template<typename... _tParams>
	inline std::pair<iterator, bool>
	emplace_unique(_tParams&&... args)
	{
		scoped_node nd(*this, yforward(args)...);
		const auto& k(_fKeyOfValue()(nd.p_saved_node->access()));

		if(in_small_size_threshold())
		{
			for(auto i = begin(); i != end(); ++i)
				if(this->key_equals(k, *i.p_node))
					return {i, {}};
		}

		const auto c(this->get_hash_code(k));
		const auto bkt(get_bucket_index(c));

		if(!in_small_size_threshold())
			if(const auto p = find_node(bkt, k, c))
				return {iterator(p), {}};

		const auto i(insert_unique_node(bkt, c, nd.p_saved_node));

		nd.p_saved_node = {};
		return {i, true};
	}

	template<typename... _tParams>
	inline iterator
	emplace_euqal(_tParams&&... args)
	{
		return emplace_hint_euqal(cend(), yforward(args)...);
	}

	template<typename... _tParams>
	iterator
	emplace_hint_unique(const_iterator, _tParams&&... args)
	{
		// XXX: The hint is ignored.
		return emplace_unique(yforward(args)...).first;
	}

	template<typename... _tParams>
	iterator
	emplace_hint_equal(const_iterator position, _tParams&&... args)
	{
		scoped_node nd(*this, yforward(args)...);
		const key_type& k(_fKeyOfValue()(nd.p_saved_node->access()));
		const auto res(this->compute_hash_code(position, k));
		const auto
			i(insert_equal_node(res.first.p_node, res.second, nd.p_saved_node));

		nd.p_saved_node = {};
		return i;
	}

	template<typename _tKeyOrTransKey, typename... _tParams>
	std::pair<iterator, bool>
	try_emplace(const_iterator, _tKeyOrTransKey&& k, _tParams&&... args)
	{
		const auto c(this->get_hash_code(k));
		const auto bkt(get_bucket_index(c));

		if(const auto nd = find_node(bkt, k, c))
			return {iterator(nd), {}};

		scoped_node nd(*this, std::piecewise_construct, std::forward_as_tuple(
			yforward(k)), std::forward_as_tuple(yforward(args)...));
		const auto i(insert_unique_node(bkt, c, nd.p_saved_node));

		nd.p_saved_node = {};
		return {i, true};
	}

	iterator
	erase(const_iterator position)
	{
		yconstraint(position != end());

		const auto p_node(position.p_node);
		const auto bkt(get_bucket_index(*p_node));

		return erase_at(bkt, get_previous_node(bkt, p_node), p_node);
	}
	//! \see LWG 2059 。
	iterator
	erase(iterator position)
	{
		return erase(const_iterator(position));
	}
	size_type
	erase(const key_type& k)
	{
		return erase_at(unique_keys(), k);
	}
	iterator
	erase(const_iterator first, const_iterator last)
	{
		auto p_nd(first.p_node);
		const auto p_last_nd(last.p_node);

		if(p_nd == p_last_nd)
			return iterator(p_nd);

		auto bkt(get_bucket_index(*p_nd));
		const auto p_prev(get_previous_node(bkt, p_nd));
		bool is_bucket_begin(p_nd == bucket_id_begin(bkt));
		auto i_bkt(bkt);

		while(true)
		{
			do
			{
				auto p(p_nd);

				p_nd = p_nd->next();
				--n_elements;
				drop_node(p);
				if(!p_nd)
					break;
				i_bkt = get_bucket_index(*p_nd);
			}while(p_nd != p_last_nd && i_bkt == bkt);
			if(is_bucket_begin)
				remove_bucket_begin(bkt, p_nd, i_bkt);
			if(p_nd == p_last_nd)
				break;
			is_bucket_begin = true;
			bkt = i_bkt;
		}
		if(p_nd && (i_bkt != bkt || is_bucket_begin))
			p_buckets[i_bkt] = p_prev;
		p_prev->p_next = p_nd;
		return iterator(p_nd);
	}

private:
	size_type
	erase_at(false_, const key_type& k)
	{
		size_t bkt;
		base_ptr p_prev;
		link_type p_nd;

		if(in_small_size_threshold())
		{
			p_prev = find_before_node(k);
			if(!p_prev)
				return 0;
			p_nd = link_type(p_prev->p_next);
			bkt = get_bucket_index(*p_nd);
		}
		else
		{
			const auto code(this->get_hash_code(k));

			bkt = get_bucket_index(code);
			p_prev = find_before_node(bkt, k, code);
			if(!p_prev)
				return 0;
			p_nd = link_type(p_prev->p_next);
		}

		// XXX: LWG 526.
		auto p_last_nd(p_nd->next());

		while(p_last_nd && this->node_equals(*p_nd, *p_last_nd))
			p_last_nd = p_last_nd->next();

		const auto i_last_bkt(p_last_nd ? get_bucket_index(*p_last_nd) : bkt);
		size_type n(0);

		do
		{
			const auto p(p_nd->next());

			drop_node(p_nd);
			p_nd = p;
			++n;
		}while(p_nd != p_last_nd);
		n_elements -= n;
		if(p_prev == p_buckets[bkt])
			remove_bucket_begin(bkt, p_last_nd, i_last_bkt);
		else if(i_last_bkt != bkt)
			p_buckets[i_last_bkt] = p_prev;
		p_prev->p_next = p_last_nd;
		return n;
	}
	size_type
	erase_at(true_, const key_type& k)
	{
		base_ptr p_prev;
		link_type p_nd;
		size_t bkt;

		if(in_small_size_threshold())
		{
			p_prev = find_before_node(k);
			if(!p_prev)
				return 0;
			p_nd = link_type(p_prev->p_next);
			bkt = get_bucket_index(*p_nd);
		}
		else
		{
			const auto code(this->get_hash_code(k));

			bkt = get_bucket_index(code);
			p_prev = find_before_node(bkt, k, code);
			if(!p_prev)
				return 0;
			p_nd = link_type(p_prev->p_next);
		}
		erase_at(bkt, p_prev, p_nd);
		return 1;
	}
	iterator
	erase_at(size_type bkt, base_ptr p_prev, link_type p_nd)
	{
		if(p_prev == p_buckets[bkt])
			remove_bucket_begin(bkt, p_nd->next(),
				p_nd->p_next ? get_bucket_index(*p_nd->next()) : 0);
		else if(p_nd->p_next)
		{
			const auto next_bkt(get_bucket_index(*p_nd->next()));

			if(next_bkt != bkt)
				p_buckets[next_bkt] = p_prev;
		}
		p_prev->p_next = p_nd->p_next;

		iterator i(p_nd->next());

		drop_node(p_nd);
		--n_elements;
		return i;
	}

	void
	erase_node(link_type x) ynothrow
	{
		while(x)
		{
			link_type y(x);

			x = x->next();
			drop_node(y);
		}
	}

public:
	friend void
	swap(table& x, table& y) ynoexcept(
		and_<is_nothrow_swappable<_fHash>, is_nothrow_swappable<_fPred>>())
	{
		swap(static_cast<table_mixin&>(x), static_cast<table_mixin&>(y));
		ystdex::alloc_on_swap(x.get_node_allocator(), y.get_node_allocator());
		std::swap(x.current_rehash, y.current_rehash);
		if(x.uses_single_bucket())
		{
			if(!y.uses_single_bucket())
			{
				x.p_buckets = y.p_buckets;
				y.p_buckets = &y.p_single_bucket;
			}
		}
		else if(y.uses_single_bucket())
		{
			y.p_buckets = x.p_buckets;
			x.p_buckets = &x.p_single_bucket;
		}
		else
			std::swap(x.p_buckets, y.p_buckets);
		std::swap(x.n_buckets, y.n_buckets),
		std::swap(x.before_begin_node.p_next, y.before_begin_node.p_next),
		std::swap(x.n_elements, y.n_elements),
		std::swap(x.p_single_bucket, y.p_single_bucket),
		x.update_bbegin(), y.update_bbegin();
	}

	void
	clear() ynothrow
	{
		erase_node(node_begin());
		std::memset(p_buckets, 0, n_buckets * sizeof(base_ptr));
		yunseq(n_elements = 0, before_begin_node.p_next = {});
	}

	//! \brief 从兼容容器合并唯一的键。
	template<typename _tHashTableCompat>
	void
	merge_unique(_tHashTableCompat& src)
	{
		static_assert(is_same<typename _tHashTableCompat::value_node,
			value_node>(), "Node types are compatible");

		YAssert(get_allocator() == src.get_allocator(),
			"Mismatched allocators found.");

		auto n(src.size());

		for(auto i = src.cbegin(), last = src.cend(); i != last;)
		{
			const auto j(i++);
			const auto& k(_fKeyOfValue()(*j));
			const auto c(this->get_hash_code(src.hash_function(), *j.p_node));
			const auto bkt(get_bucket_index(c));

			if(!find_node(bkt, k, c))
			{
				auto nh(src.extract(j));

				insert_unique_node(bkt, c, nh.ptr, n);
				nh.ptr = {};
				n = 1;
			}
			else if(n != 1)
				--n;
		}
	}

	//! \brief 从兼容容器合并等价的键。
	template<typename _tHashTableCompat>
	void
	merge_equal(_tHashTableCompat& src)
	{
		static_assert(is_same<typename _tHashTableCompat::node_type,
			node_type>(), "Node types are compatible.");

		YAssert(get_allocator() == src.get_allocator(),
			"Mismatched allocators found.");

		link_type hint = {};

		this->reserve(size() + src.size());
		for(auto i = src.cbegin(), last = src.cend(); i != last;)
		{
			const auto j(i++);
			const auto c(this->get_hash_code(src.hash_function(), *j.p_node));
			auto nh(src.extract(j));

			hint = insert_equal_node(hint, c, nh.ptr).p_node;
			nh.ptr = {};
		}
	}

	// NOTE: The member %hash_function comes from %hash_code_mixin implied by
	//	%table_mixin.

	// NOTE: The member %key_eq comes from %table_mixin.

	YB_ATTR_nodiscard YB_PURE iterator
	find(const key_type& k)
	{
		if(in_small_size_threshold())
		{
			for(auto i = begin(); i != end(); ++i)
				if(this->key_equals(k, *i.p_node))
					return i;
			return end();
		}

		const auto code(this->get_hash_code(k));

		return iterator(find_node(get_bucket_index(code), k, code));
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	find(const key_type& k) const
	{
		if(in_small_size_threshold())
		{
			for(auto i = begin(); i != end(); ++i)
				if(this->key_equals(k, *i.p_node))
					return i;
			return end();
		}

		const auto code(this->get_hash_code(k));

		return const_iterator(find_node(get_bucket_index(code), k, code));
	}

	YB_ATTR_nodiscard YB_PURE size_type
	count(const key_type& k) const
	{
		auto i = find(k);

		if(!i.p_node)
			return 0;
		if(unique_keys())
			return 1;

		size_type n(1);

		for(auto ref = i++;
			i.p_node && this->node_equals(*ref.p_node, *i.p_node); ++i)
			++n;
		return n;
	}

private:
	template<typename _tIter, class _tTree>
	YB_ATTR_nodiscard YB_PURE std::pair<_tIter, _tIter>
	equal_range_impl(_tTree& t, const key_type& k)
	{
		auto i(t.find(k));

		if(!i.p_node)
			return {i, i};

		const auto b(i++);

		if(unique_keys())
			return {b, i};
		while(i.p_node && t.node_equals(*b.p_node, *i.p_node))
			++i;
		return {b, i};
	}

public:
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

#define YB_Impl_HashTable_GenericLookupHead(_n, _r) \
	template<typename _tTransKey, \
		yimpl(typename = enable_if_transparent_t<_fHash, _tTransKey>, \
		typename = enable_if_transparent_t<_fPred, _tTransKey>)> \
	YB_ATTR_nodiscard YB_PURE _r \
	_n(const _tTransKey& k)

	YB_Impl_HashTable_GenericLookupHead(find_tr, iterator)
	{
		const auto code(this->get_hash_code(k));

		return iterator(find_node(get_bucket_index(code), k, code));
	}
	YB_Impl_HashTable_GenericLookupHead(find_tr, const_iterator) const
	{
		const auto code(this->get_hash_code(k));

		return const_iterator(find_node(get_bucket_index(code), k, code));
	}

	YB_Impl_HashTable_GenericLookupHead(count_tr, size_type) const
	{
		const hash_code c(this->get_hash_code(k));

		if(auto p_nd = find_node(get_bucket_index(c), k, c))
		{
			iterator i(p_nd);
			size_type n(1);

			for(++i; i.p_node && this->code_key_equals(k, c, *i.p_node); ++i)
				++n;
			return n;
		}
		return 0;
	}

	YB_Impl_HashTable_GenericLookupHead(equal_range_tr,
		std::pair<iterator YPP_Comma iterator>)
	{
		const hash_code c(this->get_hash_code(k));

		if(const auto p_nd = find_node(get_bucket_index(c), k, c))
		{
			iterator i(p_nd);
			const auto b(i++);

			while(i.p_node && this->code_key_equals(k, c, *i.p_node))
				++i;
			return {b, i};
		}
		return {iterator(), iterator()};
	}
	YB_Impl_HashTable_GenericLookupHead(equal_range_tr,
		std::pair<const_iterator YPP_Comma const_iterator>) const
	{
		const hash_code c(this->get_hash_code(k));

		if(const auto p_nd = find_node(get_bucket_index(c), k, c))
		{
			const_iterator i(p_nd);
			const auto b(i++);

			while(i.p_node && this->code_key_equals(k, c, *i.p_node))
				++i;
			return {b, i};
		}
		return {const_iterator(), const_iterator()};
	}

#undef YB_Impl_HashTable_GenericLookupHead

	// NOTE: The functions %(operator[], at) are not provided here. If needed,
	//	they can be implemented using functions templates %(try_emplace, find).

	YB_ATTR_nodiscard YB_PURE size_type
	bucket_count() const ynothrow
	{
		return n_buckets;
	}

	YB_ATTR_nodiscard YB_PURE size_type
	max_bucket_count() const ynothrow
	{
		return max_size();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	bucket(const key_type& k) const
	{
		return get_bucket_index(this->get_hash_code(k));
	}

	YB_ATTR_nodiscard YB_PURE size_type
	bucket_size(size_type bkt) const
	{
		return std::distance(begin(bkt), end(bkt));
	}

protected:
	YB_ATTR_nodiscard YB_PURE link_type
	node_begin() const
	{
		return link_type(before_begin_node.p_next);
	}

private:
	void
	update_bbegin()
	{
		if(node_begin())
			p_buckets[get_bucket_index(*node_begin())] = &before_begin_node;
	}

	void
	update_bbegin(link_type x)
	{
		before_begin_node.p_next = x;
		update_bbegin();
	}

	bool
	uses_single_bucket() const
	{
		return uses_single_bucket(p_buckets);
	}
	bool
	uses_single_bucket(buckets_ptr bkts) const
	{
		return YB_UNLIKELY(bkts == &p_single_bucket);
	}

	YB_ATTR_nodiscard YB_STATELESS yconstfn size_t
	in_small_size_threshold() const ynothrow
	{
		return size() <= table_hash_traits<_fHash>::small_size_threshold();
	}

	YB_ATTR_nodiscard YB_ALLOCATOR YB_ATTR_returns_nonnull buckets_ptr
	allocate_buckets(size_type n)
	{
		if(YB_UNLIKELY(n == 1))
		{
			p_single_bucket = {};
			return &p_single_bucket;
		}

		buckets_allocator a(get_node_allocator());
		const buckets_ptr
			p(ystdex::to_address(bucket_ator_tratis::allocate(a, n)));

		std::memset(p, 0, n * sizeof(base_ptr));
		return p;
	}

	void
	deallocate_buckets()
	{
		deallocate_buckets(p_buckets, n_buckets);
	}
	void
	deallocate_buckets(buckets_ptr bkts, size_type n)
	{
		if(!uses_single_bucket(bkts))
		{
			buckets_allocator a(get_node_allocator());

			bucket_ator_tratis::deallocate(a, std::pointer_traits<typename
				bucket_ator_tratis::pointer>::pointer_to(*bkts), n);
		}
	}

	YB_ATTR_nodiscard YB_PURE link_type
	bucket_id_begin(size_type bkt) const
	{
		const base_ptr p_nd(p_buckets[bkt]);

		return p_nd ? link_type(p_nd->p_next) : link_type();
	}

	void
	reset_state() ynothrow
	{
		current_rehash.reset_state(),
		yunseq(
		n_buckets = 1,
		p_single_bucket = {}
		);
		// XXX: G++ 12.1 has false positive on [-Wsequence-point].
		yunseq(
		p_buckets = &p_single_bucket,
		before_begin_node.p_next = {},
		n_elements = 0
		);
	}

	YB_ATTR_nodiscard YB_PURE size_type
	get_bucket_index(const node_value_type& nd) const ynothrow
	{
		return hash_code_mixin::get_bucket_index(nd, n_buckets);
	}
	YB_ATTR_nodiscard YB_PURE size_type
	get_bucket_index(hash_code c) const
	{
		return hash_code_mixin::get_bucket_index(c, n_buckets);
	}

public:
	YB_ATTR_nodiscard YB_PURE float
	load_factor() const ynothrow
	{
		return float(size()) / float(bucket_count());
	}

	// NOTE: The functions %max_load_factor come from instance of %rehash_base.

	YB_ATTR_nodiscard YB_PURE const rehash_type&
	rehash_policy() const ynothrow
	{
		return current_rehash;
	}
	void
	rehash_policy(const rehash_type& pol)
		ynoexcept(is_nothrow_copy_assignable<rehash_type>())
	{
		current_rehash = pol;
	}

	void
	rehash(size_type n)
	{
		static_assert(is_same<size_type, size_t>{},
			"Unsupported size type found.");
		const auto& saved_state(current_rehash.get_state());

		n = current_rehash.get_next_bucket(std::max(
			current_rehash.get_bucket_for_elements(n_elements + 1), n));
		if(n != n_buckets)
			rehash_with_state(n, saved_state);
		else
			current_rehash.reset_state(saved_state);
	}

private:
	void
	rehash_impl(size_t n, false_)
	{
		rehash_impl([&](link_type& p, buckets_ptr new_buckets) ynothrowv{
			size_t bbegin_bkt(0), prev_bkt(0);
			link_type p_prev = {};
			bool check_bucket = {};

			while(p)
			{
				auto next(p->next());
				const auto bkt(hash_code_mixin::get_bucket_index(*p, n));

				if(p_prev && prev_bkt == bkt)
				{
					p->p_next = p_prev->p_next;
					p_prev->p_next = p;
					check_bucket = true;
				}
				else
				{
					if(check_bucket)
					{
						if(p_prev->p_next)
						{
							const auto next_bkt(hash_code_mixin
								::get_bucket_index(*p_prev->next(), n));

							if(next_bkt != prev_bkt)
								new_buckets[next_bkt] = p_prev;
						}
						check_bucket = {};
					}
					rehash_iterate(bbegin_bkt, new_buckets, bkt, p);
				}
				yunseq(p_prev = p, prev_bkt = bkt);
				p = next;
			}
			if(check_bucket && p_prev->p_next)
			{
				const auto next_bkt(
					hash_code_mixin::get_bucket_index(*p_prev->next(), n));

				if(next_bkt != prev_bkt)
					new_buckets[next_bkt] = p_prev;
			}
		}, n);
	}
	void
	rehash_impl(size_t n, true_)
	{
		rehash_impl([&](link_type& p, buckets_ptr new_buckets) ynothrowv{
			size_t bbegin_bkt(0);

			while(p)
			{
				const auto next(p->next());

				rehash_iterate(bbegin_bkt, new_buckets,
					hash_code_mixin::get_bucket_index(*p, n), p);
				p = next;
			}
		}, n);
	}
	template<typename _func>
	void
	rehash_impl(_func f, size_t n)
	{
		const auto new_buckets(allocate_buckets(n));
		auto p(node_begin());

		before_begin_node.p_next = {};
		// NOTE: Call to %f shall not throw, so it is safe to call
		//	%(allocate_buckets, deallocate_buckets) out of a try-catch block.
		f(p, new_buckets);
		deallocate_buckets();
		yunseq(n_buckets = n, p_buckets = new_buckets);
	}

	YB_NONNULL(5) void
	rehash_iterate(size_t& bbegin_bkt, buckets_ptr new_buckets, size_t bkt,
		base_ptr p)
	{
		if(!new_buckets[bkt])
		{
			insert_bucket_hook(before_begin_node, *p);
			new_buckets[bkt] = &before_begin_node;
			if(p->p_next)
				new_buckets[bbegin_bkt] = p;
			bbegin_bkt = bkt;
		}
		else
			insert_bucket_hook(*new_buckets[bkt], *p);
	}

	void
	rehash_with_state(size_t n, const rehash_state& state)
	{
		try
		{
			rehash_impl(n, unique_keys());
		}
		catch(...)
		{
			current_rehash.reset_state(state);
			throw;
		}
	}

	// NOTE: The function %reserve, if present, comes from instance of
	//	%rehash_base. The parameter type shall be same to %size_type, conforming
	//	to unordered associative container requirements in ISO C++. This is also
	//	checked by the static assertion in %rehash.
};


template<typename, typename, typename>
class table_merge_helper
{};
//!@}

} // inline namespace hash_table;

} // namespace details;
//!@}

//! \relates details::table_iterator_base
template<typename _type, bool _bCached>
struct is_bitwise_swappable<
	details::table_iterator_base<_type, _bCached>> : true_
{};

//! \relates details::table_iterator
template<typename _type, bool _bCached>
struct is_bitwise_swappable<details::table_iterator<_type, _bCached>> : true_
{};

//! \relates details::table_const_iterator
template<typename _type, bool _bCached>
struct is_bitwise_swappable<details::table_const_iterator<_type, _bCached>>
	: true_
{};

//! \relates details::table_local_iterator_base
template<typename _tKey, typename _type, typename _fKeyOfValue,
	typename _fHash, typename _fRangeHash, bool _bCached>
struct is_bitwise_swappable<details::table_local_iterator_base<_tKey, _type,
	_fKeyOfValue, _fHash, _fRangeHash, _bCached>> : true_
{};

//! \relates details::table_local_iterator
template<typename _tKey, typename _type, typename _fKeyOfValue, typename _fHash,
	typename _fRangeHash, bool _bCached>
struct is_bitwise_swappable<details::table_local_iterator<_tKey, _type,
	_fKeyOfValue, _fHash, _fRangeHash, _bCached>> : true_
{};

//! \relates details::table_local_const_iterator
template<typename _tKey, typename _type, typename _fKeyOfValue,
	typename _fHash, typename _fRangeHash, bool _bCached>
struct is_bitwise_swappable<details::table_local_const_iterator<_tKey, _type,
	_fKeyOfValue, _fHash, _fRangeHash, _bCached>> : true_
{};

} // namespace ystdex;

#endif

