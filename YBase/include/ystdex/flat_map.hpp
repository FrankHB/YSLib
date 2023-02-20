/*
	© 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file flat_map.hpp
\ingroup YStandardEx
\brief 平坦映射容器。
\version r1067
\author FrankHB <frankhb1989@gmail.com>
\since build 968
\par 创建时间:
	2023-02-16 05:32:04 +0800
\par 修改时间:
	2023-02-20 17:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::FlatMap
\see WG21 N4928 [flat.map] 。
\see WG21 P0429R8 。
\see WG21 P0429R9 。

提供 ISO C++23 标准库头 \c \<flat_map> 兼容的替代接口和实现。
原始设计参见 WG21 P0429R9 ，除以下例外：
swap 使用 WG21 P0429R8 。
当前不支持依赖 ranges 的特性，包括 from_ranges 构造、insert_range 和其它操作。
包括以下缺陷修复：
LWG 3803 的一部分：构造函数的比较器默认参数（推导指引除外）。
LWG 3816 ：容器类型满足序列容器要求。
因为核心语言特性限制，不支持以下特性：
\<compare> 相关接口。
LWG 3803 的一部分：推导指引的修改。
LWG 3804 ：部分推导指引缺失。
*/


#ifndef YB_INC_ystdex_flat_map_hpp_
#define YB_INC_ystdex_flat_map_hpp_ 1

#include "range.hpp" // for "range.hpp", <initializer_list>, std::pair,
//	std::declval, std::iterator_traits, is_unqualified_object,
//	is_nothrow_swappable;
#if __cplusplus > 202002L && __has_include(<flat_map>)
#include <flat_map> // for <flat_map>, __cpp_lib_flat_map;
#	if __cpp_lib_flat_map >= 202207L
#		define YB_Has_Cpp23_flat_map true
#	else
#		define YB_Has_Cpp23_flat_map false
#	endif
#endif
#if !YB_Has_Cpp23_flat_map
#	include "iterator.hpp" // for pair_iterator, random_access_iteratable,
//	std::random_access_iterator_tag, indirection_proxy, std::get,
//	enable_if_convertible_t, enable_if_t;
#	include "swap.hpp" // for pair_proxy, is_nothrow_swappable;
#	include <vector> // for std::vector;
#	include "iterator_trait.hpp" // for has_iterator_category;
#	include "allocator.hpp" // for std::uses_allocator,
//	ystdex::make_obj_using_allocator, std::equal, std::lexicographical_compare,
//	std::sort, std::unique, std::min, std::partition_point;
#	include "container.hpp" // for ystdex::try_emplace,
//	ystdex::insert_or_assign, ystdex::emplace_hint_in_place, ystdex::search_map;
#endif

//! \since build 968
namespace ystdex
{

namespace details
{

//! \warning 非虚析构。
template<class _tKeyConIter, class _tMappedConIter>
class flat_map_iterator : public random_access_iteratable<flat_map_iterator<
	_tKeyConIter, _tMappedConIter>, typename std::iterator_traits<_tKeyConIter>
	::difference_type, std::pair<typename std::iterator_traits<_tKeyConIter>
	::reference, typename std::iterator_traits<_tMappedConIter>::reference>>
{
public:
	using iterator_category = std::random_access_iterator_tag;
	// XXX: This shall be same to the %value_type in container.
	using value_type
		= std::pair<typename std::iterator_traits<_tKeyConIter>::value_type,
		typename std::iterator_traits<_tMappedConIter>::value_type>;
	using difference_type
		= typename std::iterator_traits<_tKeyConIter>::difference_type;
	using reference = pair_proxy<typename std::iterator_traits<_tKeyConIter>
		::reference, typename std::iterator_traits<_tMappedConIter>::reference>;
	using pointer = indirection_proxy<reference>;

private:
	// XXX: Do not use base class to prevent ambiguity of %operator!= for
	//	%std::tuple instances find by %random_access_iteratable.
	pair_iterator<_tKeyConIter, _tMappedConIter> pr;

public:
	flat_map_iterator() = default;
	flat_map_iterator(_tKeyConIter i_k, _tMappedConIter i_m)
		: pr(i_k, i_m)
	{}
	// NOTE: This is for the conversion from %iterator to %const_iterator.
	template<typename _tOtherKeyConIter, typename _tOtherMappedConIter,
		yimpl(typename
		= enable_if_convertible_t<_tOtherKeyConIter, _tKeyConIter>, typename
		= enable_if_convertible_t<_tOtherMappedConIter, _tMappedConIter>)>
	flat_map_iterator(const flat_map_iterator<_tOtherKeyConIter,
		_tOtherMappedConIter>& other)
		: pr(other.get_key_iterator(), other.get_mapped_iterator())
	{}

	flat_map_iterator&
	operator+=(difference_type n)
	{
		pr += n;
		return *this;
	}

	flat_map_iterator&
	operator-=(difference_type n)
	{
		pr -= n;
		return *this;
	}

	YB_ATTR_nodiscard yconstfn reference
	operator*() const
	{
		return reference(*get_key_iterator(), *get_mapped_iterator());
	}

	YB_ATTR_nodiscard yconstfn pointer
	operator->() const
	{
		return pointer{operator*()};
	}

	flat_map_iterator&
	operator++()
	{
		++pr;
		return *this;
	}

	flat_map_iterator&
	operator--()
	{
		--pr;
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const flat_map_iterator& x, const flat_map_iterator& y)
	{
		return x.get_key_iterator() == y.get_key_iterator();
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator<(const flat_map_iterator& x, const flat_map_iterator& y)
	{
		return x.get_key_iterator() < y.get_key_iterator();
	}

	template<yimpl(typename = void)>
	friend yconstfn difference_type
	operator-(const flat_map_iterator& x, const flat_map_iterator& y)
	{
		return difference_type(x.get_key_iterator() - y.get_key_iterator());
	}

	YB_ATTR_nodiscard const _tKeyConIter&
	get_key_iterator() const ynothrow
	{
		return std::get<0>(pr.base());
	}

	YB_ATTR_nodiscard const _tMappedConIter&
	get_mapped_iterator() const ynothrow
	{
		return std::get<1>(pr.base());
	}
};
} // namespace details;


//! \ingroup tags
//!@{
#if YB_Has_Cpp23_flat_map
using std::sorted_unique_t;
using std::sorted_unique;
#else
struct sorted_unique_t
{
	explicit
	sorted_unique_t() = default;
};

yconstexpr_inline const sorted_unique_t sorted_unique{};
#endif
//!@}


//! \todo 在其它头文件中提供实现。
template<typename, typename, typename, class>
class flat_multimap;

/*!
\ingroup YBase_replacement_features
\brief 平坦映射容器。
\warning 非虚析构。
\see Documentation::YBase @2.1.4.1 。
\see Documentation::YBase @2.1.4.2 。

类似 ISO C++23 的 std::flat_map 的容器。
不完整类型的支持条件同 WG21 N4510 ，除对类型完整地要求扩展到键和映射的类型。
部分成员提供较 ISO C++23 更强的 noexcept 异常规范，除转移赋值略有不同。
*/
template<typename _tKey, typename _tMapped, typename _fComp = less<_tKey>,
	class _tKeyCon = std::vector<_tKey>,
	class _tMappedCon = std::vector<_tMapped>>
class flat_map : private totally_ordered<
	flat_map<_tKey, _tMapped, _fComp, _tKeyCon, _tMappedCon>>
{
	//! \see WG21 N4928 [flat.map.overview]/7 。
	//!@{
	static_assert(has_iterator_category<std::random_access_iterator_tag,
		typename _tKeyCon::iterator>(), "Invalid iterator found.");
	static_assert(has_iterator_category<std::random_access_iterator_tag,
		typename _tMappedCon::iterator>(), "Invalid iterator found.");
	//!@}
	//! \see WG21 N4928 [flat.map.overview]/8 。
	//!@{
	static_assert(is_same<_tKey, typename _tKeyCon::value_type>(),
		"Inconsistent key types found.");
	static_assert(is_same<_tMapped, typename _tMappedCon::value_type>(),
		"Inconsistent mapped types found.");
	//!@}

public:
	using key_type = _tKey;
	static_assert(is_unqualified_object<key_type>(),
		"The key type shall be an unqualified object type.");
	using mapped_type = _tMapped;
	static_assert(is_unqualified_object<mapped_type>(),
		"The mapped type shall be an unqualified object type.");
	using value_type = std::pair<key_type, mapped_type>;
	using key_compare = _fComp;
	static_assert(is_convertible<decltype(std::declval<const key_compare&>()(
		std::declval<const key_type&>(), std::declval<const key_type&>())),
		bool>(), "Invalid comparison object type found.");
	using reference = std::pair<const key_type&, mapped_type&>;
	using const_reference = std::pair<const key_type&, const mapped_type&>;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	//! \note 实现定义：符合要求的未指定类型。
	//!@{
	using iterator = details::flat_map_iterator<typename _tKeyCon::iterator,
		typename _tMappedCon::iterator>;
	using const_iterator = details::flat_map_iterator<typename
		_tKeyCon::const_iterator, typename _tMappedCon::const_iterator>;
	//!@}
	using reverse_iterator = ystdex::reverse_iterator<iterator>;
	using const_reverse_iterator = ystdex::reverse_iterator<const_iterator>;
	using key_container_type = _tKeyCon;
	static_assert(is_unqualified_object<key_container_type>(),
		"The key container shall be an unqualified object type.");
	static_assert(is_nothrow_swappable<key_container_type>(),
		"The key container shall be swappable without exception thrown.");
	using mapped_container_type = _tMappedCon;
	static_assert(is_unqualified_object<mapped_container_type>(),
		"The mapped container shall be an unqualified object type.");
	static_assert(is_nothrow_swappable<mapped_container_type>(),
		"The mapped container shall be swappable without exception thrown.");
	class value_compare
	{
		friend class flat_map;

	private:
		key_compare comp;

		value_compare(key_compare c)
			: comp(c)
		{}

	public:
		YB_ATTR_nodiscard YB_PURE bool
		operator()(const value_type& x, const value_type& y) const
		{
			return comp(x.first, y.first);
		}
	};
	struct containers
	{
		key_container_type keys;
		mapped_container_type values;
	};

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
			return !comp(x.first, y.first) && !comp(y.first, x.first);
		}
	};
	template<class _tAlloc>
	using enable_uses_alloc_t = enable_if_t<std::uses_allocator<
		key_container_type, _tAlloc>::value && std::uses_allocator<
		mapped_container_type, _tAlloc>::value, int>;

	containers c;
	key_compare compare{};

public:
	flat_map() yimpl(= default);
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_map(const _tAlloc& a)
		: c{ystdex::make_obj_using_allocator<key_container_type>(a),
		ystdex::make_obj_using_allocator<mapped_container_type>(a)}
	{}
	flat_map(key_container_type key_cont, mapped_container_type mapped_cont)
		: c{std::move(key_cont), std::move(mapped_cont)}
	{
		sort_and_unique();
	}
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_map(key_container_type key_cont, mapped_container_type mapped_cont,
		const _tAlloc& a)
		: flat_map(ystdex::make_obj_using_allocator<key_container_type>(a,
		std::move(key_cont)), ystdex::make_obj_using_allocator<
		mapped_container_type>(a, std::move(mapped_cont)))
	{}
	flat_map(sorted_unique_t, key_container_type key_cont,
		mapped_container_type mapped_cont)
		: c{std::move(key_cont), std::move(mapped_cont)}
	{}
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_map(sorted_unique_t s, key_container_type key_cont,
		mapped_container_type mapped_cont, const _tAlloc& a)
		: flat_map(s, ystdex::make_obj_using_allocator<key_container_type>(a,
		std::move(key_cont)), ystdex::make_obj_using_allocator<
		mapped_container_type>(a, std::move(mapped_cont)))
	{}
	explicit
	flat_map(const key_compare& comp)
		: c(), compare(comp)
	{}
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_map(const key_compare& comp, const _tAlloc& a)
		: c{ystdex::make_obj_using_allocator<key_container_type>(a),
		ystdex::make_obj_using_allocator<mapped_container_type>(a)},
		compare(comp)
	{}
	template<typename _tIn>
	inline
	flat_map(_tIn first, _tIn last, const key_compare& comp = key_compare())
		: flat_map(comp)
	{
		insert(first, last);
		sort_and_unique();
	}
	template<typename _tIn, class _tAlloc,
		yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_map(_tIn first, _tIn last, const key_compare& comp, const _tAlloc& a)
		: flat_map(comp, a)
	{
		insert(first, last);
		sort_and_unique();
	}
	template<typename _tIn, class _tAlloc,
		yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_map(_tIn first, _tIn last, const _tAlloc& a)
		: flat_map(a)
	{
		insert(first, last);
		sort_and_unique();
	}
	template<typename _tIn>
	inline
	flat_map(sorted_unique_t, _tIn first, _tIn last,
		const key_compare& comp = key_compare())
		: flat_map(comp)
	{
		insert(first, last);
	}
	template<typename _tIn, class _tAlloc,
		yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_map(sorted_unique_t, _tIn first, _tIn last, const key_compare& comp,
		const _tAlloc& a)
		: flat_map(comp, a)
	{
		insert(first, last);
	}
	template<typename _tIn, class _tAlloc,
		yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_map(sorted_unique_t, _tIn first, _tIn last, const _tAlloc& a)
		: flat_map(a)
	{
		insert(first, last);
	}
	inline
	flat_map(std::initializer_list<value_type> il,
		const key_compare& comp = key_compare())
		: flat_map(il.begin(), il.end(), comp)
	{}
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_map(std::initializer_list<value_type> il, const key_compare& comp,
		const _tAlloc& a)
		: flat_map(il.begin(), il.end(), comp, a)
	{}
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_map(std::initializer_list<value_type> il, const _tAlloc& a)
		: flat_map(il.begin(), il.end(), a)
	{}
	inline
	flat_map(sorted_unique_t s, std::initializer_list<value_type> il,
		const key_compare& comp = key_compare())
		: flat_map(s, il.begin(), il.end(), comp)
	{}
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_map(sorted_unique_t s, std::initializer_list<value_type> il,
		const key_compare& comp, const _tAlloc& a)
		: flat_map(s, il.begin(), il.end(), comp, a)
	{}
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	flat_map(sorted_unique_t s, std::initializer_list<value_type> il,
		const _tAlloc& a)
		: flat_map(s, il.begin(), il.end(), a)
	{}

	flat_map&
	operator=(std::initializer_list<value_type> il)
	{
		clear();
		insert(il);
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const flat_map& x, const flat_map& y)
	{
		return std::equal(x.begin(), x.end(), y.begin(), y.end());
	}

	YB_ATTR_nodiscard YB_PURE friend bool
	operator<(const flat_map& x, const flat_map& y)
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
		return iterator(c.keys.begin(), c.values.begin());
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	begin() const ynothrow
	{
		return const_iterator(c.keys.begin(), c.values.begin());
	}

	YB_ATTR_nodiscard YB_PURE iterator
	end() ynothrow
	{
		return iterator(c.keys.end(), c.values.end());
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	end() const ynothrow
	{
		return const_iterator(c.keys.end(), c.values.end());
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
		return c.keys.empty();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	size() const ynothrow
	{
		return c.keys.size();
	}

	YB_ATTR_nodiscard YB_PURE size_type
	max_size() const ynothrow
	{
		return std::min(c.keys.max_size(), c.values.max_size());
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
			throw std::out_of_range("flat_map::at");
		return (*i).second;
	}

public:
	template<typename... _tParams>
	std::pair<iterator, bool>
	emplace(_tParams&&... args)
	{
		value_type v(yforward(args)...);
		const auto i(lower_bound(v.first));

		if(i == end() || compare(v.first, i->first))
		{
			const auto i_k(c.keys.emplace(i.get_key_iterator(),
				std::move(v.first)));
			auto i_m(i.get_mapped_iterator());

			try
			{
				i_m = c.values.emplace(i_m, std::move(v.second));
			}
			catch(...)
			{
				c.keys.erase(i_k);
				throw;
			}
			return {iterator(i_k, i_m), true};
		}
		return {i, {}};
	}

	template<typename... _tParams>
	inline iterator
	emplace_hint(const_iterator, _tParams&&... args)
	{
		// XXX: The hint is ignored.
		return emplace(yforward(args)...).first;
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
		const auto mid(end() - difference_type(n));
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

			c.keys.insert(c.keys.end(), std::move(v.first));
			c.values.insert(c.values.end(), std::move(v.second));
			yunseq(++first, ++n);
		}
		return n;
	}

public:
	//! \post \c *this 被清空。
	containers
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

	/*!
	\post <tt>key_cont.size() == mapped_cont.size()</tt> 。
	\post \c key_cont 的元素在 key_compare 下有序且没有超过 1 个的等价元素。
	*/
	void
	replace(key_container_type&& key_cont, mapped_container_type&& mapped_cont)
	{
		try
		{
			c.keys = std::move(key_cont);
			c.values = std::move(mapped_cont);
		}
		catch(...)
		{
			clear();
			throw;
		}
	}

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
		return ystdex::try_emplace_hint(*this, hint, k, yforward(args)...);
	}
	template<typename... _tParams>
	inline iterator
	try_emplace(const_iterator hint, key_type&& k, _tParams&&... args)
	{
		return ystdex::try_emplace_hint(*this, hint, k, yforward(args)...);
	}

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
		return ystdex::insert_or_assign_hint(*this, hint, k, yforward(obj));
	}
	template<typename _tObj>
	inline iterator
	insert_or_assign(const_iterator hint, key_type&& k, _tObj&& obj)
	{
		return ystdex::insert_or_assign_hint(*this, hint, std::move(k),
			yforward(obj));
	}

	iterator
	erase(iterator position)
	{
		return erase_impl(position);
	}
	iterator
	erase(const_iterator position)
	{
		return erase_impl(position);
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
		// XXX: As %erase_impl.
		return iterator(c.keys.erase(first.get_key_iterator(),
			last.get_key_iterator()), c.values.erase(
			first.get_mapped_iterator(), last.get_mapped_iterator()));
	}

private:
	template<typename _tIter>
	iterator
	erase_impl(_tIter i)
	{
		// XXX: Assume the calls to %erase do not throw. This is guaranteed by
		//	the underlying requirements on the container types.
		return iterator(c.keys.erase(i.get_key_iterator()),
			c.values.erase(i.get_mapped_iterator()));
	}

public:
	// XXX: The change to remove the throwing exception specifiction does not
	//	resolve the possibility of throwring during swapping the comparison
	//	objects, which is changed by WG21 P0429R9. See also https://github.com/tzlaine/flat_map/commit/7384d91a665133a0eabf7bed20a8293473765c3b.
	void
	swap(flat_map& x) ynoexcept(yimpl(is_nothrow_swappable<_fComp>()))
	{
		ystdex::swap_dependent(compare, x.compare),
		ystdex::swap_dependent(c.keys, x.c.keys),
		ystdex::swap_dependent(c.values, x.c.values);
	}
	friend void
	swap(flat_map& x, flat_map& y) ynoexcept_spec(x.swap(y))
	{
		x.swap(y);
	}

	void
	clear() ynothrow
	{
		c.keys.clear(),
		c.values.clear();
	}

	YB_ATTR_nodiscard YB_PURE key_compare
	key_comp() const
	{
		return compare;
	}

	YB_ATTR_nodiscard YB_PURE value_compare
	value_comp() const
	{
		return value_compare(compare);
	}

	YB_ATTR_nodiscard YB_PURE const key_container_type&
	keys() const ynothrow
	{
		return c.keys;
	}

	YB_ATTR_nodiscard YB_PURE const mapped_container_type&
	values() const ynothrow
	{
		return c.values;
	}

#define YB_Impl_FlatMap_GenericLookupHead(_n, _r) \
	template<typename _tTransKey, \
		yimpl(typename = enable_if_transparent_t<_fComp, _tTransKey>)> \
	YB_ATTR_nodiscard YB_PURE inline _r \
	_n(const _tTransKey& x)

	YB_ATTR_nodiscard YB_PURE iterator
	find(const key_type& x)
	{
		return find_impl<iterator>(*this, x);
	}
	YB_Impl_FlatMap_GenericLookupHead(find, iterator)
	{
		return find_impl<iterator>(*this, x);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	find(const key_type& x) const
	{
		return find_impl<const_iterator>(*this, x);
	}
	YB_Impl_FlatMap_GenericLookupHead(find, const_iterator) const
	{
		return find_impl<const_iterator>(*this, x);
	}

private:
	template<typename _tIter, class _tMap, typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static inline _tIter
	find_impl(_tMap& m, const _tKeyOrTransKey& x)
	{
		const auto pr(ystdex::search_map(m, x));

		return pr.second ? m.end() : pr.first;
	}

public:
	YB_ATTR_nodiscard YB_PURE size_type
	count(const key_type& x) const
	{
		return contains(x) ? 1 : 0;
	}
	YB_Impl_FlatMap_GenericLookupHead(count, size_type) const
	{
		// XXX: This cannot just use %find instead. See https://github.com/cplusplus/papers/issues/1037#issuecomment-960066305.
		return std::count(c.keys.begin(), c.keys.end(), x);
	}

	YB_ATTR_nodiscard YB_PURE bool
	contains(const key_type& x) const
	{
		return find(x) != end();
	}
	YB_Impl_FlatMap_GenericLookupHead(contains, bool) const
	{
		return find(x) != end();
	}

	YB_ATTR_nodiscard YB_PURE iterator
	lower_bound(const key_type& x)
	{
		return lower_bound_impl<iterator>(*this, x);
	}
	YB_Impl_FlatMap_GenericLookupHead(lower_bound, iterator)
	{
		return lower_bound_impl<iterator>(*this, x);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	lower_bound(const key_type& x) const
	{
		return lower_bound_impl<const_iterator>(*this, x);
	}
	YB_Impl_FlatMap_GenericLookupHead(lower_bound, const_iterator) const
	{
		return lower_bound_impl<const_iterator>(*this, x);
	}

private:
	template<typename _tIter, class _tMap, typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static inline _tIter
	lower_bound_impl(_tMap& m, const _tKeyOrTransKey& x)
	{
		return step_next<_tIter>(m, std::partition_point(m.c.keys.begin(),
			m.c.keys.end(), [&](const decltype(*m.c.keys.begin())& k){
			return bool(m.compare(k, x));
		}));
	}

public:
	YB_ATTR_nodiscard YB_PURE iterator
	upper_bound(const key_type& x)
	{
		return upper_bound_impl<iterator>(*this, x);
	}
	YB_Impl_FlatMap_GenericLookupHead(upper_bound, iterator)
	{
		return upper_bound_impl<iterator>(*this, x);
	}
	YB_ATTR_nodiscard YB_PURE const_iterator
	upper_bound(const key_type& x) const
	{
		return upper_bound_impl<const_iterator>(*this, x);
	}
	YB_Impl_FlatMap_GenericLookupHead(upper_bound, const_iterator) const
	{
		return upper_bound_impl<const_iterator>(*this, x);
	}

private:
	template<typename _tIter, class _tMap, typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static inline _tIter
	upper_bound_impl(_tMap& m, const _tKeyOrTransKey& x)
	{
		return step_next<_tIter>(m, std::partition_point(m.c.keys.begin(),
			m.c.keys.end(), [&](const decltype(*m.c.keys.begin())& k){
			return !bool(m.compare(x, k));
		}));
	}

public:
	YB_ATTR_nodiscard YB_PURE std::pair<iterator, iterator>
	equal_range(const key_type& x)
	{
		return equal_range_impl<iterator>(*this, x);
	}
	YB_Impl_FlatMap_GenericLookupHead(equal_range,
		std::pair<iterator YPP_Comma iterator>)
	{
		return equal_range_impl<iterator>(*this, x);
	}
	YB_ATTR_nodiscard YB_PURE std::pair<const_iterator, const_iterator>
	equal_range(const key_type& x) const
	{
		return equal_range_impl<const_iterator>(*this, x);
	}
	YB_Impl_FlatMap_GenericLookupHead(equal_range,
		std::pair<const_iterator YPP_Comma const_iterator>) const
	{
		return equal_range_impl<const_iterator>(*this, x);
	}

private:
	template<typename _tIter, class _tMap, typename _tKeyOrTransKey>
	YB_ATTR_nodiscard YB_PURE static inline std::pair<_tIter, _tIter>
	equal_range_impl(_tMap& m, const _tKeyOrTransKey& x)
	{
		return {lower_bound_impl<_tIter>(m, x), upper_bound_impl<_tIter>(m, x)};
	}

#undef YB_Impl_FlatMap_GenericLookupHead

	template<typename _tIter, class _tMap, typename _tKeyIter>
	YB_ATTR_nodiscard YB_PURE static inline _tIter
	step_next(_tMap& m, _tKeyIter i_k)
	{
		return _tIter(i_k, m.c.values.begin() + (i_k - m.c.keys.begin()));
	}
};

} // namespace ystdex;

#endif

