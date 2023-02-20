/*
	© 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file flat_set.hpp
\ingroup YStandardEx
\brief 平坦集合容器。
\version r763
\author FrankHB <frankhb1989@gmail.com>
\since build 968
\par 创建时间:
	2023-02-16 05:36:01 +0800
\par 修改时间:
	2023-02-20 17:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::FlatSet
\see WG21 N4928 [flat.set] 。
\see WG21 P1222R3 。
\see WG21 P1222R4 。

提供 ISO C++23 标准库头 \c \<flat_set> 兼容的替代接口和实现。
原始设计参见 WG21 P1222R4 ，除以下例外：
swap 使用 WG21 P1222R3 。
当前不支持依赖 ranges 的特性，包括 from_ranges 构造、insert_range 和其它操作。
包括以下缺陷修复：
LWG 3751 ：宏 __cpp_lib_flat_set 。
LWG 3803 的一部分：构造函数的比较器默认参数（推导指引除外）。
因为核心语言特性限制，不支持以下特性：
LWG 3774 ：\<compare> 相关接口。
LWG 3786 ：推导指引上的默认分配器参数。
LWG 3803 的一部分：推导指引的修改。
LWG 3804 ：部分推导指引缺失。
*/


#ifndef YB_INC_ystdex_flat_set_hpp_
#define YB_INC_ystdex_flat_set_hpp_ 1

#include "range.hpp" // for "range.hpp", <initializer_list>, std::pair,
//	std::declval, std::iterator_traits, is_unqualified_object,
//	is_nothrow_swappable;
#if __cplusplus > 202002L && __has_include(<flat_set>)
#include <flat_set> // for <flat_set>, __cpp_lib_flat_set;
#	if __cpp_lib_flat_set >= 202207L
#		define YB_Has_Cpp23_flat_set true
#	else
#		define YB_Has_Cpp23_flat_set false
#	endif
#endif
#if !YB_Has_Cpp23_flat_set
#	include "flat_map.hpp" // for sorted_unique_t, sorted_unique, enable_if_t;
#	include <vector> // for std::vector;
#	include "iterator_trait.hpp" // for has_iterator_category;
#	include "allocator.hpp" // for std::uses_allocator,
//	ystdex::make_obj_using_allocator, std::equal, std::lexicographical_compare,
//	std::sort, std::unique, std::partition_point;
#	include "swap.hpp" // for is_nothrow_swappable;
#	include "container.hpp" // for ystdex::search_map;
#endif

//! \since build 968
namespace ystdex
{

//! \ingroup tags
//!@{
#if YB_Has_Cpp23_flat_set
using std::sorted_unique_t;
using std::sorted_unique;
#elif false
// XXX: To prevent redefinitions, they are in %FlatMap.
struct sorted_unique_t
{
	explicit
	sorted_unique_t() = default;
};

yconstexpr_inline const sorted_unique_t sorted_unique{};
#endif
//!@}


//! \todo 在其它头文件中提供实现。
template<typename, typename, class>
class flat_multiset;

/*!
\ingroup YBase_replacement_features
\brief 平坦映射容器。
\warning 非虚析构。
\see Documentation::YBase @2.1.4.1 。
\see Documentation::YBase @2.1.4.2 。

类似 ISO C++23 的 std::flat_set 的容器。
不完整类型的支持条件同 WG21 N4510 ，除对类型完整地要求扩展到键和映射的类型。
部分成员提供较 ISO C++23 更强的 noexcept 异常规范，除转移赋值略有不同。
*/
template<typename _tKey, typename _fComp = less<_tKey>,
	class _tKeyCon = std::vector<_tKey>>
class flat_set : private totally_ordered<flat_set<_tKey, _fComp, _tKeyCon>>
{
	//! \see WG21 N4928 [flat.set.overview]/7 。
	static_assert(has_iterator_category<std::random_access_iterator_tag,
		typename _tKeyCon::iterator>(), "Invalid iterator found.");
	//! \see WG21 N4928 [flat.set.overview]/8 。
	static_assert(is_same<_tKey, typename _tKeyCon::value_type>(),
		"Inconsistent key types found.");

public:
	using key_type = _tKey;
	static_assert(is_unqualified_object<key_type>(),
		"The key type shall be an unqualified object type.");
	using value_type = _tKey;
	using key_compare = _fComp;
	static_assert(is_convertible<decltype(std::declval<const key_compare&>()(
		std::declval<const key_type&>(), std::declval<const key_type&>())),
		bool>(), "Invalid comparison object type found.");
	using value_compare = _fComp;
	using reference = value_type&;
	using const_reference = const value_type&;
	using size_type = typename _tKeyCon::size_type;
	using difference_type = typename _tKeyCon::difference_type;
	//! \note 实现定义：同 _tKeyCon 的迭代器。
	//!@{
	using iterator = typename _tKeyCon::iterator;
	using const_iterator = typename _tKeyCon::const_iterator;
	//!@}
	using reverse_iterator = ystdex::reverse_iterator<iterator>;
	using const_reverse_iterator = ystdex::reverse_iterator<const_iterator>;
	using container_type = _tKeyCon;
	static_assert(is_unqualified_object<container_type>(),
		"The key container shall be an unqualified object type.");
	static_assert(is_nothrow_swappable<container_type>(),
		"The key container shall be swappable without exception thrown.");

private:
	struct key_equiv
	{
		key_compare comp;

		key_equiv(key_compare c)
			: comp(c)
		{}

		YB_ATTR_nodiscard YB_PURE bool
		operator()(const_reference x, const_reference y) const
		{
			return !comp(x, y);
		}
	};
	template<class _tAlloc>
	using enable_uses_alloc_t = enable_if_t<std::uses_allocator<
		container_type, _tAlloc>::value, int>;

	container_type c;
	key_compare compare{};

public:
	flat_set() yimpl(= default);
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_set(const _tAlloc& a)
		: c(ystdex::make_obj_using_allocator<container_type>(a))
	{}
	explicit
	flat_set(container_type cont)
		: c(std::move(cont))
	{
		sort_and_unique();
	}
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_set(container_type cont, const _tAlloc& a)
		: flat_set(ystdex::make_obj_using_allocator<container_type>(a,
		std::move(cont)))
	{}
	flat_set(sorted_unique_t, container_type cont)
		: c(std::move(cont))
	{}
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_set(sorted_unique_t s, container_type cont, const _tAlloc& a)
		: flat_set(s, ystdex::make_obj_using_allocator<container_type>(a,
		std::move(cont)))
	{}
	explicit
	flat_set(const key_compare& comp)
		: c(), compare(comp)
	{}
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_set(const key_compare& comp, const _tAlloc& a)
		: c(ystdex::make_obj_using_allocator<container_type>(a)), compare(comp)
	{}
	template<typename _tIn>
	inline
	flat_set(_tIn first, _tIn last, const key_compare& comp = key_compare())
		: flat_set(comp)
	{
		insert(first, last);
		sort_and_unique();
	}
	template<typename _tIn, class _tAlloc,
		yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_set(_tIn first, _tIn last, const key_compare& comp, const _tAlloc& a)
		: flat_set(comp, a)
	{
		insert(first, last);
		sort_and_unique();
	}
	template<typename _tIn, class _tAlloc,
		yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_set(_tIn first, _tIn last, const _tAlloc& a)
		: flat_set(a)
	{
		insert(first, last);
		sort_and_unique();
	}
	template<typename _tIn>
	inline
	flat_set(sorted_unique_t, _tIn first, _tIn last,
		const key_compare& comp = key_compare())
		: flat_set(comp)
	{
		insert(first, last);
	}
	template<typename _tIn, class _tAlloc,
		yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_set(sorted_unique_t, _tIn first, _tIn last, const key_compare& comp,
		const _tAlloc& a)
		: flat_set(comp, a)
	{
		insert(first, last);
	}
	template<typename _tIn, class _tAlloc,
		yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_set(sorted_unique_t, _tIn first, _tIn last, const _tAlloc& a)
		: flat_set(a)
	{
		insert(first, last);
	}
	inline
	flat_set(std::initializer_list<value_type> il,
		const key_compare& comp = key_compare())
		: flat_set(il.begin(), il.end(), comp)
	{}
	template<class _tAlloc,
		yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_set(std::initializer_list<value_type> il, const key_compare& comp,
		const _tAlloc& a)
		: flat_set(il.begin(), il.end(), comp, a)
	{}
	template<class _tAlloc,
		yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_set(std::initializer_list<value_type> il, const _tAlloc& a)
		: flat_set(il.begin(), il.end(), a)
	{}
	inline
	flat_set(sorted_unique_t s, std::initializer_list<value_type> il,
		const key_compare& comp = key_compare())
		: flat_set(s, il.begin(), il.end(), comp)
	{}
	template<class _tAlloc,
		yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_set(sorted_unique_t s, std::initializer_list<value_type> il,
		const key_compare& comp, const _tAlloc& a)
		: flat_set(s, il.begin(), il.end(), comp, a)
	{}
	template<class _tAlloc,
		yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_set(sorted_unique_t s, std::initializer_list<value_type> il,
		const _tAlloc& a)
		: flat_set(s, il.begin(), il.end(), a)
	{}

	flat_set&
	operator=(std::initializer_list<value_type> il)
	{
		clear();
		insert(il);
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const flat_set& x, const flat_set& y)
	{
		return std::equal(x.begin(), x.end(), y.begin(), y.end());
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator<(const flat_set& x, const flat_set& y)
	{
		return std::lexicographical_compare(x.begin(), x.end(), y.begin(),
			y.end());
	}

private:
	void
	sort_and_unique()
	{
		// XXX: The specification has no guarantee on stability of sorted
		//	subrange.
		std::sort(begin(), end(), value_comp());
		erase(std::unique(begin(), end(), key_equiv(compare)), end());
	}

public:
	YB_ATTR_nodiscard YB_PURE iterator
	begin() ynothrow
	{
		return iterator(c.begin());
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	begin() const ynothrow
	{
		return const_iterator(c.begin());
	}

	YB_ATTR_nodiscard YB_PURE iterator
	end() ynothrow
	{
		return iterator(c.end());
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	end() const ynothrow
	{
		return const_iterator(c.end());
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
		return c.empty();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	size() const ynothrow
	{
		return c.size();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	max_size() const ynothrow
	{
		return c.max_size();
	}

	template<typename... _tParams>
	std::pair<iterator, bool>
	emplace(_tParams&&... args)
	{
		key_type k(yforward(args)...);
		const auto i(lower_bound(k));

		if(i == end() || compare(k, *i))
			return {iterator(c.emplace(i, std::move(k))), true};
		return {i, {}};
	}

	template<typename... _tParams>
	inline iterator
	emplace_hint(const_iterator, _tParams&&... args)
	{
		// XXX: The hint is ignored.
		return emplace(yforward(args)...);
	}

	std::pair<iterator, bool>
	insert(const value_type& x)
	{
		return emplace(x);
	}
	std::pair<iterator, bool>
	insert(value_type&& x)
	{
		return emplace(std::move(x));
	}
	template<typename _tPair>
	inline yimpl(enable_if_constructible_r_t)<
		std::pair<iterator YPP_Comma bool>, value_type, _tPair>
	insert(_tPair&& x)
	{
		return emplace(yforward(x));
	}
	iterator
	insert(const_iterator position, const value_type& x)
	{
		return emplace_hint(position, x);
	}
	iterator
	insert(const_iterator position, value_type&& x)
	{
		return emplace_hint(position, std::move(x));
	}
	template<typename _tPair>
	inline yimpl(enable_if_constructible_r_t)<iterator, value_type, _tPair>
	insert(const_iterator position, _tPair&& x)
	{
		return emplace_hint(position, yforward(x));
	}
	template<typename _tIn>
	void
	insert(_tIn first, _tIn last)
	{
		const auto n(insert_append(first, last));
		const auto mid(end() - n);
		const auto vcomp(value_comp());

		// XXX: As %sort_and_unique.
		std::sort(mid, end(), vcomp);
		std::inplace_merge(begin(), mid, end(), vcomp);
		erase(std::unique(begin(), end(), key_equiv(compare)), end());
	}
	template<typename _tIn>
	void
	insert(sorted_unique_t, _tIn first, _tIn last)
	{
		const auto n(insert_append(first, last));
		const auto mid(end() - n);
		const auto vcomp(value_comp());

		std::inplace_merge(begin(), mid, end(), vcomp);
		erase(std::unique(begin(), end(), key_equiv(compare)), end());
	}
	void
	insert(std::initializer_list<value_type> il)
	{
		insert(il.begin(), il.end());
	}
	void
	insert(sorted_unique_t s, std::initializer_list<value_type> il)
	{
		insert(s, il.begin(), il.end());
	}

private:
	template<typename _tIn>
	YB_ATTR_nodiscard size_type
	insert_append(_tIn first, _tIn last)
	{
		size_type n(0);

		while(first != last)
		{
			value_type v(*first);

			c.insert(c.end(), std::move(v));
			yunseq(++first, ++n);
		}
		return n;
	}

public:
	//! \post \c *this 被清空。
	container_type
	extract() &&
	{
		try
		{
			auto res(std::move(c));

			clear();
			return res;
		}
		catch(...)
		{
			clear();
			throw;
		}
	}

	//! \post \c cont 的元素在 key_compare 下有序且没有超过 1 个的等价元素。
	void
	replace(container_type&& cont)
	{
		try
		{
			c = std::move(cont);
		}
		catch(...)
		{
			clear();
			throw;
		}
	}

	iterator
	erase(iterator position)
	{
		return c.erase(position);
	}
	iterator
	erase(const_iterator position)
	{
		return c.erase(position);
	}
	size_type
	erase(const key_type& x)
	{
		const auto i(find(x));

		if(i != end())
		{
			erase(i);
			return 1;
		}
		return 0;
	}
	iterator
	erase(const_iterator first, const_iterator last)
	{
		return c.erase(first, last);
	}

	// XXX: The change to remove the throwing exception specifiction does not
	//	resolve the possibility of throwring during swapping the comparison
	//	objects, which is changed by WG21 P0429R9. See also https://github.com/tzlaine/flat_set/commit/7384d91a665133a0eabf7bed20a8293473765c3b.
	void
	swap(flat_set& x) ynoexcept(yimpl(is_nothrow_swappable<_fComp>()))
	{
		ystdex::swap_dependent(compare, x.compare),
		ystdex::swap_dependent(c, x.c);
	}
	friend void
	swap(flat_set& x, flat_set& y) ynoexcept_spec(x.swap(y))
	{
		x.swap(y);
	}

	void
	clear() ynothrow
	{
		c.clear();
	}

	YB_ATTR_nodiscard YB_PURE key_compare
	key_comp() const
	{
		return compare;
	}

	YB_ATTR_nodiscard YB_PURE value_compare
	value_comp() const
	{
		return compare;
	}

#define YB_Impl_FlatSet_GenericLookupHead(_n, _r) \
	template<typename _tTransKey, \
		yimpl(typename = enable_if_transparent_t<_fComp, _tTransKey>)> \
	YB_ATTR_nodiscard YB_PURE inline _r \
	_n(const _tTransKey& x)

	YB_ATTR_nodiscard YB_PURE iterator
	find(const key_type& x)
	{
		return find_impl<iterator>(*this, x);
	}
	YB_Impl_FlatSet_GenericLookupHead(find, iterator)
	{
		return find_impl<iterator>(*this, x);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	find(const key_type& x) const
	{
		return find_impl<const_iterator>(*this, x);
	}
	YB_Impl_FlatSet_GenericLookupHead(find, const_iterator) const
	{
		return find_impl<const_iterator>(*this, x);
	}

private:
	template<typename _tIter, class _tSet, typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static inline _tIter
	find_impl(_tSet& s, const _tKeyOrTransKey& x)
	{
		const auto pr(ystdex::search_map(s, x));

		return pr.second ? s.end() : pr.first;
	}

public:
	YB_ATTR_nodiscard YB_PURE size_type
	count(const key_type& x) const
	{
		return contains(x) ? 1 : 0;
	}
	YB_Impl_FlatSet_GenericLookupHead(count, size_type) const
	{
		// XXX: This cannot just use %find instead. See https://github.com/cplusplus/papers/issues/1037#issuecomment-960066305.
		return std::count(c.begin(), c.end(), x);
	}

	YB_ATTR_nodiscard YB_PURE bool
	contains(const key_type& x) const
	{
		return find(x) != end();
	}
	YB_Impl_FlatSet_GenericLookupHead(contains, bool) const
	{
		return find(x) != end();
	}

	YB_ATTR_nodiscard YB_PURE iterator
	lower_bound(const key_type& x)
	{
		return lower_bound_impl<iterator>(*this, x);
	}
	YB_Impl_FlatSet_GenericLookupHead(lower_bound, iterator)
	{
		return lower_bound_impl<iterator>(*this, x);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	lower_bound(const key_type& x) const
	{
		return lower_bound_impl<const_iterator>(*this, x);
	}
	YB_Impl_FlatSet_GenericLookupHead(lower_bound, const_iterator) const
	{
		return lower_bound_impl<const_iterator>(*this, x);
	}

private:
	template<typename _tIter, class _tSet, typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static inline _tIter
	lower_bound_impl(_tSet& s, const _tKeyOrTransKey& x)
	{
		return std::partition_point(s.c.begin(), s.c.end(),
			[&](const decltype(*s.c.begin())& k){
			return bool(s.compare(k, x));
		});
	}

public:
	YB_ATTR_nodiscard YB_PURE iterator
	upper_bound(const key_type& x)
	{
		return upper_bound_impl<iterator>(*this, x);
	}
	YB_Impl_FlatSet_GenericLookupHead(upper_bound, iterator)
	{
		return upper_bound_impl<iterator>(*this, x);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	upper_bound(const key_type& x) const
	{
		return upper_bound_impl<const_iterator>(*this, x);
	}
	YB_Impl_FlatSet_GenericLookupHead(upper_bound, const_iterator) const
	{
		return upper_bound_impl<const_iterator>(*this, x);
	}

private:
	template<typename _tIter, class _tSet, typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static inline _tIter
	upper_bound_impl(_tSet& s, const _tKeyOrTransKey& x)
	{
		return std::partition_point(s.c.begin(), s.c.end(),
			[&](const decltype(*s.c.begin())& k){
			return !bool(s.compare(x, k));
		});
	}

public:
	YB_ATTR_nodiscard YB_PURE std::pair<iterator, iterator>
	equal_range(const key_type& x)
	{
		return equal_range_impl<iterator>(*this, x);
	}
	YB_Impl_FlatSet_GenericLookupHead(equal_range,
		std::pair<iterator YPP_Comma iterator>)
	{
		return equal_range_impl<iterator>(*this, x);
	}
	YB_ATTR_nodiscard YB_PURE std::pair<const_iterator, const_iterator>
	equal_range(const key_type& x) const
	{
		return equal_range_impl<const_iterator>(*this, x);
	}
	YB_Impl_FlatSet_GenericLookupHead(equal_range,
		std::pair<const_iterator YPP_Comma const_iterator>) const
	{
		return equal_range_impl<const_iterator>(*this, x);
	}

private:
	template<typename _tIter, class _tSet, typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static inline std::pair<_tIter, _tIter>
	equal_range_impl(_tSet& s, const _tKeyOrTransKey& x)
	{
		return {lower_bound_impl<_tIter>(s, x), upper_bound_impl<_tIter>(s, x)};
	}

#undef YB_Impl_FlatSet_GenericLookupHead
};

} // namespace ystdex;

#endif

