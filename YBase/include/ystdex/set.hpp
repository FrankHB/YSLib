/*
	© 2016-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file set.hpp
\ingroup YStandardEx
\brief 集合容器。
\version r1072
\author FrankHB <frankhb1989@gmail.com>
\since build 665
\par 创建时间:
	2016-01-23 20:13:53 +0800
\par 修改时间:
	2017-03-28 00:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Set
*/


#ifndef YB_INC_ystdex_set_hpp_
#define YB_INC_ystdex_set_hpp_ 1

#include "functor.hpp" // for lref, std::move, less, std::allocator,
//	std::allocator_traits, std::piecewise_construct, enable_if_transparent,
//	std::count, std::lower_bound, std::upper_bound, std::equal_range;
#include "iterator.hpp" // for transformed_iterator,
//	iterator_transformation::second, ystdex::make_transform,
//	std::forward_as_tuple;
#include <map> // for std::map, std::initializer_list;

namespace ystdex
{

/*!
\brief 转移集合元素。
\post 转移后元素和参数比较相等。
\since build 673
*/
template<typename _type>
yconstfn const _type&
set_value_move(const _type& x) ynothrow
{
	return x;
}


// XXX: G++ 5.2.0 rejects generic associative lookup in debug mode of %std::map.
// TODO: Find precise version supporting debug mode.
#if (__cpp_lib_generic_associative_lookup >= 201304 || __cplusplus >= 201402L) \
	&& !(defined(__GLIBCXX__) && defined(_GLIBCXX_DEBUG))
#	define YB_Impl_Set_UseGenericLookup 1
#else
#	define YB_Impl_Set_UseGenericLookup 0
#endif

//! \since build 679
namespace details
{

//! \since build 680
template<typename _tWrappedKey, typename _fComp,
	bool = has_mem_is_transparent<_fComp>::value>
struct tcompare
{
	_fComp comp;

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
\brief 允许指定不同于值类型的键的集合。
\note 和 std::set 类似，但迭代器可修改，且插入操作要求参数满足 CopyInsertable 。
\see WG21 N3456 23.2.4 [associative.reqmts] 。
\since build 665
*/
template<typename _type, typename _fComp = less<_type>,
	class _tAlloc = std::allocator<_type>>
class mapped_set
{
public:
	using key_type = _type;
	using value_type = _type;
	using key_compare = _fComp;
	using value_compare = _fComp;
	using allocator_type = _tAlloc;
	using reference = value_type&;
	using const_reference =  const value_type&;

private:
	using mapped_key_type = details::wrapped_key<_type>;
	using mapped_key_compare = details::tcompare<mapped_key_type, _fComp>;
	using umap_type
		= std::map<mapped_key_type, value_type, mapped_key_compare, _tAlloc>;
	using umap_pair = typename umap_type::value_type;
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
	using pointer = typename std::allocator_traits<_tAlloc>::pointer;
	using const_pointer
		= typename std::allocator_traits<_tAlloc>::const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
	// XXX: It is undefined behavior when %value_type is incomplete, however
	//	some implementations actually support this.
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
		// NOTE: The underlying implementation may reuse node, e.g. libstdc++.
		//	So it is preferred to directly move and then to fix references.
		m_map = s.m_map;
		amend_all();
		return *this;
	}
	mapped_set&
	operator=(mapped_set&& s)
#if false
	// TODO: Blocked. Apply exception specification after published ISO C++17.
		ynoexcept(std::allocator_traits<_tAlloc>::is_always_equal()
		&& is_nothrow_move_assignable<_fComp>())
#endif
	{
		s.swap(*this);
		return *this;
	}
	mapped_set&
	operator=(std::initializer_list<value_type> il)
	{
		mapped_set(il).swap(*this);
		return *this;
	}

	allocator_type
	get_allocator() const ynothrow
	{
		return m_map.get_allocator();
	}

	iterator
	begin() ynothrow
	{
		return iterator(m_map.begin());
	}
	const_iterator
	begin() const ynothrow
	{
		return const_iterator(m_map.begin());
	}

	iterator
	end() ynothrow
	{
		return iterator(m_map.end());
	}
	const_iterator
	end() const ynothrow
	{
		return const_iterator(m_map.end());
	}

	reverse_iterator
	rbegin() ynothrow
	{
		return reverse_iterator(end());
	}
	const_reverse_iterator
	rbegin() const ynothrow
	{
		return const_reverse_iterator(end());
	}

	reverse_iterator
	rend() ynothrow
	{
		return reverse_iterator(begin());
	}
	const_reverse_iterator
	rend() const ynothrow
	{
		return const_reverse_iterator(begin());
	}

	const_iterator
	cbegin() const ynothrow
	{
		return const_iterator(m_map.cbegin());
	}

	const_iterator
	cend() const ynothrow
	{
		return const_iterator(m_map.cend());
	}

	const_reverse_iterator
	crbegin() const ynothrow
	{
		return const_reverse_iterator(end());
	}

	const_reverse_iterator
	crend() const ynothrow
	{
		return const_reverse_iterator(begin());
	}

	bool
	empty() const ynothrow
	{
		return m_map.empty();
	}

	size_type
	size() const ynothrow
	{
		return m_map.size();
	}

	size_type
	max_size() const ynothrow
	{
		return m_map.max_size();
	}

	template<typename... _tParams>
	std::pair<iterator, bool>
	emplace(_tParams&&... args)
	{
		// XXX: %value_type needs to be MoveConstructible, not
		//	EmplaceInsertable.
		return insert(value_type(yforward(args)...));
	}

	//! \note 使用 ADL set_value_move 转移元素。
	template<typename... _tParams>
	iterator
	emplace_hint(const_iterator position, _tParams&&... args)
	{
		// XXX: %value_type needs to be MoveConstructible, not
		//	EmplaceInsertable.
		return insert(position, value_type(yforward(args)...));
	}

	std::pair<iterator, bool>
	insert(const value_type& x)
	{
		// XXX: %value_type needs to be CopyConstructible rather than
		//	CopyInsertable into %mapped_set.
		// XXX: %mapped_key_type and %value_type need to be EmplaceConstructible
		//	into %umap_type.
		const auto res(m_map.emplace(std::piecewise_construct,
			std::forward_as_tuple(mapped_key_type(x)),
			std::forward_as_tuple(x)));

		if(res.second)
			amend_pair(*res.first);
		return {iterator(res.first), res.second};
	}
	//! \note 使用 ADL set_value_move 转移元素。
	std::pair<iterator, bool>
	insert(value_type&& x)
	{
		// XXX: %mapped_key_type and moved %value_type need to be
		//	EmplaceConstructible into %umap_type.
		const auto res(m_map.emplace(std::piecewise_construct,
			std::forward_as_tuple(mapped_key_type(x)),
			std::forward_as_tuple(set_value_move(x))));

		if(res.second)
			amend_pair(*res.first);
		return {iterator(res.first), res.second};
	}
	iterator
	insert(const_iterator position, const value_type& x)
	{
		// XXX: %mapped_key_type and %value_type need to be EmplaceConstructible
		//	into %umap_type.
		const auto res(m_map.emplace_hint(position.get(),
			std::piecewise_construct, std::forward_as_tuple(mapped_key_type(x)),
			std::forward_as_tuple(x)));

		// TODO: Blocked. Use C++1z %try_emplace result.
		amend_pair(*res);
		return iterator(res);
	}
	//! \note 使用 ADL set_value_move 转移元素。
	iterator
	insert(const_iterator position, value_type&& x)
	{
		// XXX: %mapped_key_type and moved %value_type need to be
		//	EmplaceConstructible into %umap_type.
		const auto res(m_map.emplace_hint(position.get(),
			std::piecewise_construct, std::forward_as_tuple(mapped_key_type(x)),
			std::forward_as_tuple(set_value_move(x))));

		// TODO: Blocked. Use C++1z %try_emplace result.
		amend_pair(*res);
		return iterator(res);
	}
	template<typename _tIn>
	void
	insert(_tIn first, _tIn last)
	{
		m_map.insert(ystdex::make_transform(first, iter_trans<_tIn>),
			ystdex::make_transform(last, iter_trans<_tIn>));
		// XXX: Time complexity.
		amend_all();
	}
	void
	insert(std::initializer_list<value_type> il)
	{
		insert(il.begin(), il.end());
	}

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

	void
	swap(mapped_set& s)
#if false
	// TODO: Blocked. Apply exception specification after published ISO C++17.
		ynoexcept(std::allocator_traits<_tAlloc>::is_always_equal()
		&& is_nothrow_swappable<_fComp>())
#endif
	{
		m_map.swap(s.m_map);
	}

	void
	clear() ynothrow
	{
		m_map.clear();
	}

	key_compare
	key_comp() const
	{
		return m_map.key_comp().comp;
	}

	value_compare
	value_comp() const
	{
		return m_map.key_comp().comp;
	}

#define YB_Impl_Set_GenericLookupHead(_n, _r) \
	template<typename _tKey, \
		yimpl(typename = enable_if_transparent_t<_fComp, _tKey>)> \
	_r \
	_n(const _tKey& x)

	iterator
	find(const key_type& x)
	{
		return iterator(m_map.find(mapped_key_type(x)));
	}
	const_iterator
	find(const key_type& x) const
	{
		return const_iterator(m_map.find(mapped_key_type(x)));
	}
	//! \since build 678
	YB_Impl_Set_GenericLookupHead(find, iterator)
	{
#if YB_Impl_Set_UseGenericLookup
		return iterator(m_map.find(x));
#else
		const auto i(lower_bound(x));

		return i != end() && !gcomp()(x, *i) ? i : end();
#endif
	}
	//! \since build 678
	YB_Impl_Set_GenericLookupHead(find, const_iterator) const
	{
#if YB_Impl_Set_UseGenericLookup
		return const_iterator(m_map.find(x));
#else
		const auto i(lower_bound(x));

		return i != cend() && !gcomp()(x, *i) ? i : cend();
#endif
	}

	size_type
	count(const key_type& x) const
	{
		return m_map.count(mapped_key_type(x));
	}
	YB_Impl_Set_GenericLookupHead(count, size_type) const
	{
#if YB_Impl_Set_UseGenericLookup
		return m_map.count(x);
#else
		const auto pr(equal_range(x));

		return std::count(pr.first, pr.second, gcomp());
#endif
	}

	iterator
	lower_bound(const key_type& x)
	{
		return iterator(m_map.lower_bound(mapped_key_type(x)));
	}
	const_iterator
	lower_bound(const key_type& x) const
	{
		return const_iterator(m_map.lower_bound(mapped_key_type(x)));
	}
	//! \since build 678
	YB_Impl_Set_GenericLookupHead(lower_bound, iterator)
	{
#if YB_Impl_Set_UseGenericLookup
		return iterator(m_map.lower_bound(x));
#else
		return std::lower_bound(begin(), end(), x, gcomp());
#endif
	}
	//! \since build 678
	YB_Impl_Set_GenericLookupHead(lower_bound, const_iterator) const
	{
#if YB_Impl_Set_UseGenericLookup
		return const_iterator(m_map.lower_bound(x));
#else
		return std::lower_bound(cbegin(), cend(), x, gcomp());
#endif
	}

	iterator
	upper_bound(const key_type& x)
	{
		return iterator(m_map.upper_bound(mapped_key_type(x)));
	}
	const_iterator
	upper_bound(const key_type& x) const
	{
		return const_iterator(m_map.upper_bound(mapped_key_type(x)));
	}
	//! \since build 678
	YB_Impl_Set_GenericLookupHead(upper_bound, iterator)
	{
#if YB_Impl_Set_UseGenericLookup
		return iterator(m_map.upper_bound(x));
#else
		return std::upper_bound(begin(), end(), x, gcomp());
#endif
	}
	//! \since build 678
	YB_Impl_Set_GenericLookupHead(upper_bound, const_iterator) const
	{
#if YB_Impl_Set_UseGenericLookup
		return const_iterator(m_map.upper_bound(x));
#else
		return std::upper_bound(cbegin(), cend(), x, gcomp());
#endif
	}

	std::pair<iterator, iterator>
	equal_range(const key_type& x)
	{
		const auto pr(m_map.equal_range(mapped_key_type(x)));

		return {iterator(pr.first), iterator(pr.second)};
	}
	std::pair<const_iterator, const_iterator>
	equal_range(const key_type& x) const
	{
		const auto pr(m_map.equal_range(mapped_key_type(x)));

		return {const_iterator(pr.first), const_iterator(pr.second)};
	}
	//! \since build 678
	YB_Impl_Set_GenericLookupHead(equal_range,
		std::pair<iterator YPP_Comma iterator>)
	{
#if YB_Impl_Set_UseGenericLookup
		const auto pr(m_map.equal_range(x));

		return {iterator(pr.first), iterator(pr.second)};
#else
		return std::equal_range(begin(), end(), x, gcomp());
#endif
	}
	//! \since build 678
	YB_Impl_Set_GenericLookupHead(equal_range,
		std::pair<const_iterator YPP_Comma const_iterator>) const
	{
#if YB_Impl_Set_UseGenericLookup
		const auto pr(m_map.equal_range(x));

		return {const_iterator(pr.first), const_iterator(pr.second)};
#else
		return std::equal_range(cbegin(), cend(), x, gcomp());
#endif
	}
#undef YB_Impl_Set_GenericLookupHead

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

#if !YB_Impl_Set_UseGenericLookup
	//! \since build 680
	mapped_key_compare
	gcomp() const
	{
		return m_map.key_comp();
	}
#endif

	template<typename _tIter>
	static umap_pair
	iter_trans(const _tIter& i)
	{
		return {mapped_key_type(*i), *i};
	}
};

#undef YB_Impl_Set_UseGenericLookup

} // namespace ystdex;

#endif

