/*
	© 2016-2019, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file set.hpp
\ingroup YStandardEx
\brief 集合容器。
\version r1439
\author FrankHB <frankhb1989@gmail.com>
\since build 665
\par 创建时间:
	2016-01-23 20:13:53 +0800
\par 修改时间:
	2021-09-26 04:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Set
*/


#ifndef YB_INC_ystdex_set_hpp_
#define YB_INC_ystdex_set_hpp_ 1

#include "functor.hpp" // for lref, std::move, less, std::allocator,
//	std::piecewise_construct, enable_if_transparent,
//	std::count, std::lower_bound, std::upper_bound, std::equal_range;
#include "iterator.hpp" // for transformed_iterator,
//	iterator_transformation::second, ystdex::reverse_iterator,
//	ystdex::make_transform, index_sequence, std::tuple, std::forward_as_tuple,
//	std::get;
#include "map.hpp" // for "tree.h" (implying "range.hpp"), map,
//	rebind_alloc_t, std::initializer_list, ystdex::search_map_by,
//	ystdex::emplace_hint_in_place, ystdex::as_const;

namespace ystdex
{

// NOTE: This is always true now since %ystdex::map is used.
// XXX: G++ 5.2.0 rejects generic associative lookup in debug mode of %std::map.
// TODO: Find precise version supporting debug mode.
#if true \
	|| ((__cpp_lib_generic_associative_lookup >= 201304L \
	|| __cplusplus >= 201402L) && !(defined(__GLIBCXX__) \
	&& defined(_GLIBCXX_DEBUG)))
#	define YB_Impl_Set_UseGenericLookup true
#else
#	define YB_Impl_Set_UseGenericLookup false
#endif

//! \since build 679
namespace details
{

//! \since build 680
template<typename _tWrappedKey, typename _fComp,
	bool = has_mem_is_transparent<_fComp>::value>
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
	<tt>get_value_key<key_type>(yforward(args)...)</tt> 合式，
		其结果作为用于插入的键，是新创建的值或已有值的引用 \c k 。
	若需以键 \c k 插入值， <tt>extend_key(yforward(k), *this)</tt> 合式。
	对 key_type 类型的 get_value_key 实现应支持任意能被 emplace
		和 emplace_hint 的参数组合，并返回适当的蕴含 key_type 比较的类型。
用于 \c insert 的 ADL 的合式要求和作用包括：
	对被插入的元素 \c v ， <tt>set_value_move(v)</tt> 合式，
		以保持键等价关系的方式转移资源。
*/
template<typename _type>
struct mapped_set_traits
{
	//! \note 覆盖实现可直接返回右值。
	//@{
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
	//@{
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
	//@}
	//@}

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
	//@{
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
	//@{
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
	//@}

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
	//@{
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
	//@}

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

	//! \since build 830
	void
	swap(mapped_set& s)
		ynoexcept_spec(and_<typename allocator_traits<_tAlloc>::is_always_equal,
		is_nothrow_move_assignable<_fComp>>::value)
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

#define YB_Impl_Set_GenericLookupHead(_n, _r) \
	template<typename _tTransKey, \
		yimpl(typename = enable_if_transparent_t<_fComp, _tTransKey>)> \
	YB_ATTR_nodiscard YB_PURE _r \
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

	YB_ATTR_nodiscard YB_PURE size_type
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
	//@}

	//! \since build 844
	friend void
	swap(mapped_set& x, mapped_set& y) ynoexcept_spec(x.swap(y))
	{
		x.swap(y);
	}
};

#undef YB_Impl_Set_UseGenericLookup

} // namespace ystdex;

#endif

