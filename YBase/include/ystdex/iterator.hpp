/*
	© 2011-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file iterator.hpp
\ingroup YStandardEx
\brief 通用迭代器。
\version r6023
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 189
\par 创建时间:
	2011-01-27 23:01:00 +0800
\par 修改时间:
	2017-05-27 00:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Iterator
*/


#ifndef YB_INC_ystdex_iterator_hpp_
#define YB_INC_ystdex_iterator_hpp_ 1

#include "pointer.hpp" // for "iterator_op.hpp", iterator_operators_t,
//	std::iterator_traits, _t, pointer_classify, cond_t, and_,
//	exclude_self_t, enable_if_inconvertible_t, *_tag, yassume,
//	is_undereferenceable, yconstraint, random_access_iteratable;
#include "type_op.hpp" // for first_tag, second_tag, std::tuple,
//	make_index_sequence, index_sequence, std::get;
#include "ref.hpp" // for lref;

namespace ystdex
{

/*!	\defgroup iterators Iterators
\brief 迭代器。
*/

/*!	\defgroup iterator_adaptors Iterator Adaptors
\ingroup iterators
\brief 迭代器适配器。
*/


/*!
\ingroup iterator_adaptors
\brief 伪迭代器。
\warning 非虚析构。
\since build 189

总是返回单一值的迭代器适配器。
*/
template<typename _type, typename _tIter = const _type*,
	typename _tTraits = std::iterator_traits<_tIter>>
class pseudo_iterator : public iterator_operators_t<
	pseudo_iterator<_type, _tIter, _tTraits>, _tTraits>
{
public:
	using iterator_type = _tIter;
	//! \since build 400
	using traits_type = _tTraits;
	using iterator_category = typename traits_type::iterator_category;
	using value_type = typename traits_type::value_type;
	using difference_type = typename traits_type::difference_type;
	using pointer = typename traits_type::pointer;
	using reference = typename traits_type::reference;

	value_type value;

	yconstfn
	pseudo_iterator() = default;
	explicit yconstfn
	pseudo_iterator(value_type v)
		: value(v)
	{}
	yconstfn
	pseudo_iterator(const pseudo_iterator&) = default;
	yconstfn
#if YB_IMPL_MSCPP
	//! \since build 458 as workaround for Visual C++ 2013
	pseudo_iterator(pseudo_iterator&& i)
		: value(std::move(i.value))
	{}
#else
	pseudo_iterator(pseudo_iterator&&) = default;
#endif

	pseudo_iterator&
	operator=(const pseudo_iterator&) = default;
	pseudo_iterator&
#if YB_IMPL_MSCPP
	//! \since build 458 as workaround for Visual C++ 2013
	operator=(pseudo_iterator&& i)
	{
		value = std::move(i.value);
		return *this;
	}
#else
	operator=(pseudo_iterator&&) = default;
#endif

	//! \since build 585
	//@{
	pseudo_iterator&
	operator+=(difference_type) ynothrow
	{
		return *this;
	}

	pseudo_iterator&
	operator-=(difference_type) ynothrow
	{
		return *this;
	}

	//! \brief 满足前向迭代器要求。
	//@{
	//! \since build 583
	reference
	operator*() ynothrow
	{
		return value;
	}
	yconstfn reference
	operator*() const ynothrow
	{
		return value;
	}

	yconstfn_relaxed pseudo_iterator&
	operator++() ynothrow
	{
		return *this;
	}

	/*!
	\brief 满足双向迭代器要求。
	\since build 665
	*/
	yconstfn_relaxed pseudo_iterator&
	operator--() ynothrow
	{
		return *this;
	}
	//@}

	//! \since build 600
	//@{
	friend yconstfn bool
	operator==(const pseudo_iterator& x, const pseudo_iterator& y)
		ynoexcept_spec(bool(x.value == y.value))
	{
		return x.value == y.value;
	}

	friend yconstfn bool
	operator<(const pseudo_iterator& x, const pseudo_iterator& y)
		ynoexcept_spec(bool(x.value < y.value))
	{
		return x.value < y.value;
	}

	friend yconstfn difference_type
	operator-(const pseudo_iterator&, const pseudo_iterator&) ynothrow
	{
		return 0;
	}
	//@}
};


namespace details
{

//! \since build 576
template<typename _tIter, typename _fTrans, typename _tReference>
struct transit_traits
{
	using iterator_type = _t<pointer_classify<_tIter>>;
	using iterator_category
		= typename std::iterator_traits<iterator_type>::iterator_category;
	using transformed_type = result_of_t<_fTrans&(_tIter&)>;
	using difference_type
		= typename std::iterator_traits<iterator_type>::difference_type;
	using reference
		= cond_t<is_same<_tReference, void>, transformed_type, _tReference>;
	/*!
	\since build 667
	\todo 测试 operator-> 并支持代理指针。
	*/
	using pointer = remove_reference_t<reference>*;
};

} // namespace details;


/*!
\ingroup iterator_adaptors
\brief 变换迭代器。
\pre 变换器必须满足 DefaultConstructible ，否则不保证可默认构造。
\note 对返回新值的二元操作复制的转换器基于第一操作数。
\warning 非虚析构。
\since build 532

使用指定参数转换得到新迭代器的间接操作替代指定原始类型的间接操作的迭代器适配器。
被替代的原始类型是迭代器类型，或除间接操作（可以不存在）外符合迭代器要求的类型。
*/
template<typename _tIter, typename _fTrans, typename _tReference = void>
class transformed_iterator : public iterator_operators_t<transformed_iterator<
	_tIter, _fTrans, _tReference>, details::transit_traits<_tIter, _fTrans,
	_tReference>>, private totally_ordered<transformed_iterator<_tIter, _fTrans,
	_tReference>, typename
	details::transit_traits<_tIter, _fTrans, _tReference>::iterator_type>
{
	//! \since build 529
	static_assert(is_decayed<_tIter>(), "Invalid type found.");
	//! \since build 529
	static_assert(is_decayed<_fTrans>(), "Invalid type found.");

	//! \since build 665
	template<typename, typename, typename>
	friend class transformed_iterator;

private:
	//! \since build 576
	using impl_traits = details::transit_traits<_tIter, _fTrans, _tReference>;

public:
	/*!
	\brief 原迭代器类型。
	\since build 290
	*/
	using iterator_type = typename impl_traits::iterator_type;
	using transformer_type = _fTrans;
	//! \since build 667
	//@{
	using iterator_category = typename impl_traits::iterator_category;
	using value_type = typename impl_traits::transformed_type;
	//@}
	//! \since build 439
	using transformed_type = typename impl_traits::transformed_type;
	//! \since build 415
	using difference_type = typename impl_traits::difference_type;
	//! \since build 667
	using pointer = typename impl_traits::pointer;
	/*!
	\note 仅当参数 _tReference 为引用时可符合 ISO C++ 对 ForwardIterator 的要求。
	\since build 357
	*/
	using reference = typename impl_traits::reference;

protected:
	//! \note 当为空类时作为第一个成员可启用空基类优化。
	mutable transformer_type transformer;

private:
	//! \since build 667
	iterator_type transformed;

public:
	//! \since build 496
	transformed_iterator() = default;
	// XXX: This is not actually related to substitution order. For other cases,
	//	use of multiple template parameters to simplify after CWG 1227 resolved
	//	by C++14 may be necessary.
	//! \since build 679
	template<typename _tIter2,
		yimpl(typename = exclude_self_t<transformed_iterator, _tIter2>,
		typename = enable_if_convertible_t<_tIter2&&, iterator_type>)>
	explicit yconstfn
	transformed_iterator(_tIter2&& i)
		: transformer(), transformed(yforward(i))
	{}
	//! \since build 678
	template<typename _tIter2, typename _fTrans2 = _fTrans,
		yimpl(typename = exclude_self_t<transformed_iterator, _tIter2>)>
	explicit yconstfn
	transformed_iterator(_tIter2&& i, _fTrans2 f)
		: transformer(f), transformed(yforward(i))
	{}
	//! \since build 665
	template<typename _tIter2, typename _fTrans2, typename _tRef2,
		yimpl(typename = enable_if_t<and_<not_<is_convertible<const
		transformed_iterator<_tIter2, _fTrans2, _tRef2>&, iterator_type>>,
		is_constructible<_tIter, const _tIter2&>,
		is_constructible<_fTrans, const _fTrans2&>>::value>)>
	yconstfn
	transformed_iterator(
		const transformed_iterator<_tIter2, _fTrans2, _tRef2>& i)
		: transformer(i.transformer), transformed(i.get())
	{}
	//! \since build 665
	template<typename _tIter2, typename _fTrans2, typename _tRef2,
		yimpl(typename = enable_if_t<and_<not_<is_convertible<
		transformed_iterator<_tIter2, _fTrans2, _tRef2>&&, iterator_type>>,
		is_constructible<_tIter, _tIter2&&>,
		is_constructible<_fTrans, _fTrans2&&>>::value>)>
	yconstfn
	transformed_iterator(transformed_iterator<_tIter2, _fTrans2, _tRef2>&& i)
		: transformer(std::move(i.transformer)), transformed(i.get())
	{}
	//! \since build 415
	//@{
	transformed_iterator(const transformed_iterator&) = default;
#if YB_IMPL_MSCPP
	//! \since build 503 as workaround for Visual C++ 2013
	transformed_iterator(transformed_iterator&& i)
		: transformer(std::move(i.transformer))
	{}
#else
	transformed_iterator(transformed_iterator&&) = default;
#endif
	//@}

	//! \since build 529
	transformed_iterator&
	operator=(const transformed_iterator&) = default;
	//! \since build 529
	transformed_iterator&
	operator=(transformed_iterator&&) = default;

	//! \since build 600
	friend transformed_iterator&
	operator+=(transformed_iterator& i, difference_type n)
		ynoexcept(noexcept(decltype(i)(i)) && noexcept(i.get() += n))
	{
		i.get() += n;
		return i;
	}

	//! \since build 600
	friend transformed_iterator&
	operator-=(transformed_iterator& i, difference_type n)
		ynoexcept(noexcept(decltype(i)(i)) && noexcept(i.get() -= n))
	{
		i.get() -= n;
		return i;
	}

	//! \since build 585
	reference
	operator*() const
		ynoexcept_spec(reference(std::declval<transformed_iterator&>().
		transformer(std::declval<transformed_iterator&>().get())))
	{
		return transformer(get());
	}

	//! \since build 665
	transformed_iterator&
	operator++()
	{
		++get();
		return *this;
	}

	//! \since build 665
	transformed_iterator&
	operator--()
	{
		--get();
		return *this;
	}

	//! \since build 685
	//@{
	template<yimpl(typename = void)>
	friend yconstfn bool
	operator==(const transformed_iterator& x, const transformed_iterator& y)
		ynoexcept_spec(bool(x.get() == y.get()))
	{
		return x.get() == y.get();
	}
	template<yimpl(typename = void)>
	friend yconstfn bool
	operator==(const transformed_iterator& x, const iterator_type& y)
		ynoexcept_spec(bool(x.get() == y))
	{
		return x.get() == y;
	}

	template<yimpl(typename = void)>
	friend yconstfn bool
	operator<(const transformed_iterator& x, const transformed_iterator& y)
		ynoexcept_spec(bool(x.get() < y.get()))
	{
		return bool(x.get() < y.get());
	}
	template<yimpl(typename = void)>
	friend yconstfn bool
	operator<(const transformed_iterator& x, const iterator_type& y)
		ynoexcept_spec(bool(x.get() < y))
	{
		return bool(x.get() < y);
	}

	template<yimpl(typename = void)>
	friend yconstfn difference_type
	operator-(const transformed_iterator& x, const transformed_iterator& y)
		ynoexcept_spec(difference_type(x.get() - y.get()))
	{
		return difference_type(x.get() - y.get());
	}
	template<yimpl(typename = void)>
	friend yconstfn difference_type
	operator-(const transformed_iterator& x, const iterator_type& y)
		ynoexcept_spec(difference_type(x.get() - y))
	{
		return difference_type(x.get() - y);
	}
	template<yimpl(typename = void)>
	friend yconstfn difference_type
	operator-(const iterator_type& x, const transformed_iterator& y)
		ynoexcept_spec(difference_type(x - y.get()))
	{
		return difference_type(x - y.get());
	}
	//@}

	//! \since build 585
	//@{
	//! \brief 取原迭代器引用。
	yconstfn_relaxed iterator_type&
	get() ynothrow
	{
		return transformed;
	}

	//! \brief 取原迭代器 const 引用。
	yconstfn const iterator_type&
	get() const ynothrow
	{
		return transformed;
	}
	//@}

	//! \since build 412
	//@{
	yconstfn_relaxed transformer_type&
	get_transformer() ynothrow
	{
		return transformer;
	}
	yconstfn const transformer_type&
	get_transformer() const ynothrow
	{
		return transformer;
	}
	//@}
};

/*!
\ingroup helper_functions
\brief 创建变换迭代器。
\relates transformed_iterator
\since build 529
*/
template<typename _tIter, typename _fTrans>
inline transformed_iterator<decay_t<_tIter>, _fTrans>
make_transform(_tIter&& i, _fTrans f)
{
	return transformed_iterator<decay_t<_tIter>, _fTrans>(yforward(i), f);
}


/*!
\brief 迭代变换操作。
\since build 358
*/
namespace iterator_transformation
{

//! \since build 665
//@{
template<typename _tIter = void>
struct first
{
	//! \since build 723
	yconstfn auto
	operator()(const _tIter& i) const ynoexcept_spec(i->first)
		-> decltype((i->first))
	{
		return i->first;
	}
};

template<>
struct first<void>
{
	//! \since build 723
	template<typename _tIter>
	yconstfn auto
	operator()(const _tIter& i) const ynoexcept_spec(i->first)
		-> decltype((i->first))
	{
		return i->first;
	}
};


template<typename _tIter = void>
struct get
{
	//! \since build 723
	yconstfn auto
	operator()(const _tIter& i) const ynoexcept_spec((*i).get())
		-> decltype((*i).get())
	{
		return (*i).get();
	}
};

template<>
struct get<void>
{
	//! \since build 723
	template<typename _tIter>
	yconstfn auto
	operator()(const _tIter& i) const ynoexcept_spec((*i).get())
		-> decltype((*i).get())
	{
		return (*i).get();
	}
};


template<typename _tIter = void>
struct indirect
{
	//! \since build 723
	yconstfn auto
	operator()(const _tIter& i) const ynoexcept_spec(**i) -> decltype(**i)
	{
		return **i;
	}
};

template<>
struct indirect<void>
{
	//! \since build 723
	template<typename _tIter>
	yconstfn auto
	operator()(const _tIter& i) const ynoexcept_spec(**i) -> decltype(**i)
	{
		return **i;
	}
};


template<typename _tIter = void>
struct second
{
	//! \since build 723
	yconstfn auto
	operator()(const _tIter& i) const ynoexcept_spec(i->second)
		-> decltype((i->second))
	{
		return i->second;
	}
};

template<>
struct second<void>
{
	//! \since build 723
	template<typename _tIter>
	yconstfn auto
	operator()(const _tIter& i) const ynoexcept_spec(i->second)
		-> decltype((i->second))
	{
		return i->second;
	}
};
//@}

} // namespace iterator_transformation;


/*!
\brief 操纵子。
\since build 288
*/
//@{
yconstexpr const first_tag get_first{}, get_key{};
yconstexpr const second_tag get_second{}, get_value{};
//! \since build 680
yconstexpr const struct get_indirect_t{} get_indirect{};
//! \since build 680
yconstexpr const struct get_get_t{} get_get{};
//@}


/*!
\brief 管道匹配操作符。
\note 使用 ADL \c make_transform 。
\since build 650
*/
//@{
template<typename _tIter>
inline auto
operator|(_tIter&& i, first_tag) -> decltype(make_transform(yforward(i),
	iterator_transformation::first<>()))
{
	return make_transform(yforward(i), iterator_transformation::first<>());
}
template<typename _tIter>
inline auto
operator|(_tIter&& i, second_tag) -> decltype(make_transform(yforward(i),
	iterator_transformation::second<>()))
{
	return make_transform(yforward(i), iterator_transformation::second<>());
}
//! \since build 680
template<typename _tIter>
inline auto
operator|(_tIter&& i, get_indirect_t) -> decltype(make_transform(yforward(i),
	iterator_transformation::indirect<>()))
{
	return make_transform(yforward(i), iterator_transformation::indirect<>());
}
//! \since build 680
template<typename _tIter>
inline auto
operator|(_tIter&& i, get_get_t)
	-> decltype(make_transform(yforward(i), iterator_transformation::get<>()))
{
	return make_transform(yforward(i), iterator_transformation::get<>());
}
//@}


/*!
\ingroup iterator_adaptors
\brief 元组迭代器。
\note 成员迭代器需可复制构造，满足随机迭代器要求（但不需要 + 和 - ）。
\warning 非虚析构。
\since build 595

拼接迭代器和其它对象类型对得到的迭代器适配器。
*/
template<typename _tIter, class _tTraits = std::iterator_traits<_tIter>,
	typename... _tSlaves>
class tuple_iterator : private std::tuple<_tIter, _tSlaves...>, public
	iterator_operators_t<tuple_iterator<_tIter, _tTraits, _tSlaves...>,
	_tTraits>
{
public:
	using base_type = std::tuple<_tIter, _tSlaves...>;
	using iterator_type = _tIter;
	using traits_type = _tTraits;
	using iterator_category = typename traits_type::iterator_category;
	using value_type = typename traits_type::value_type;
	using difference_type = typename traits_type::difference_type;
	using pointer = typename traits_type::pointer;
	using reference = typename traits_type::reference;

private:
	using seq_type = make_index_sequence<std::tuple_size<base_type>::value>;

public:
	yconstfn
	tuple_iterator() = default;
	explicit yconstfn
	tuple_iterator(const _tIter& i)
		: base_type(i, _tSlaves()...)
	{}
	template<typename... _tParams>
	yconstfn
	tuple_iterator(const _tIter& i, _tParams&&... args)
		: base_type(i, yforward(args)...)
	{}
	yconstfn
	tuple_iterator(const tuple_iterator&) = default;
	yconstfn
	tuple_iterator(tuple_iterator&&) = default;

	tuple_iterator&
	operator=(const tuple_iterator&) = default;
	tuple_iterator&
	operator=(tuple_iterator&&) = default;

	tuple_iterator&
	operator+=(difference_type n) ynoexcept_spec(
		yimpl(std::declval<tuple_iterator&>().add(n, seq_type())))
	{
		add(n, seq_type());
		return *this;
	}

	tuple_iterator&
	operator-=(difference_type n) ynoexcept_spec(
		yimpl(std::declval<tuple_iterator&>().subtract(n, seq_type())))
	{
		subtract(n, seq_type());
		return *this;
	}

	//! \brief 满足输入迭代器要求。
	//@{
	yconstfn reference
	operator*() const
		ynoexcept_spec(reference(*std::declval<tuple_iterator&>().get()))
	{
		return *get();
	}

	tuple_iterator&
	operator++()
		ynoexcept_spec(yimpl(std::declval<tuple_iterator&>().inc(seq_type())))
	{
		inc(seq_type());
		return *this;
	}
	//@}

	//! \brief 满足双向迭代器要求。
	tuple_iterator&
	operator--()
		ynoexcept_spec(yimpl(std::declval<tuple_iterator&>().dec(seq_type())))
	{
		dec(seq_type());
		return *this;
	}

	friend bool
	operator==(const tuple_iterator& x, const tuple_iterator& y)
		ynoexcept_spec(bool(x.get() == y.get()))
	{
		return x.get() == y.get();
	}

private:
	template<size_t... _vSeq>
	void
	add(difference_type n, index_sequence<_vSeq...>)
		ynoexcept_spec(yimpl(yunseq((void(
		std::get<_vSeq>(std::declval<tuple_iterator&>().base()) += n), 0)...)))
	{
		yunseq((void(std::get<_vSeq>(base()) += n), 0)...);
	}

public:
	yconstfn_relaxed base_type&
	base() ynothrow
	{
		return *this;
	}
	yconstfn const base_type&
	base() const ynothrow
	{
		return *this;
	}

private:
	template<size_t... _vSeq>
	void
	dec(index_sequence<_vSeq...>) ynoexcept_spec(yimpl(yunseq((void(
		--std::get<_vSeq>(std::declval<tuple_iterator&>().base())), 0)...)))
	{
		yunseq((void(--std::get<_vSeq>(base())), 0)...);
	}

	template<size_t... _vSeq>
	void
	inc(index_sequence<_vSeq...>) ynoexcept_spec(yimpl(yunseq((void(
		++std::get<_vSeq>(std::declval<tuple_iterator&>().base())), 0)...)))
	{
		yunseq((void(++std::get<_vSeq>(base())), 0)...);
	}

public:
	yconstfn_relaxed iterator_type&
	get() ynothrow
	{
		return std::get<0>(base());
	}
	yconstfn const iterator_type&
	get() const ynothrow
	{
		return std::get<0>(base());
	}

private:
	template<size_t... _vSeq>
	void
	subtract(difference_type n, index_sequence<_vSeq...>)
		ynoexcept_spec(yimpl(yunseq((void(
		std::get<_vSeq>(std::declval<tuple_iterator&>().base()) -= n), 0)...)))
	{
		yunseq((void(std::get<_vSeq>(base()) -= n), 0)...);
	}
};


/*!
\ingroup iterator_adaptors
\brief 成对迭代器。
\warning 非虚析构。
\sa tuple_iterator
\since build 595
*/
template<typename _tIter, typename _tSlave,
	class _tTraits = std::iterator_traits<_tIter>>
using pair_iterator = tuple_iterator<_tIter, _tTraits, _tSlave>;


/*!
\ingroup iterator_adaptors
\brief 间接输入迭代器。
\note 向指定类型的迭代器传递输入迭代器操作。
\since build 412
*/
template<typename _tIter>
class indirect_input_iterator : public iterator_operators_t<
	indirect_input_iterator<_tIter>, std::iterator_traits<_tIter>>
{
public:
	using iterator_type = _tIter;
	using iterator_category = std::input_iterator_tag;
	using value_type = typename std::iterator_traits<iterator_type>::value_type;
	using difference_type
		= typename std::iterator_traits<iterator_type>::difference_type;
	using pointer = typename std::iterator_traits<iterator_type>::pointer;
	using reference = typename std::iterator_traits<iterator_type>::reference;

private:
	iterator_type iter;

public:
	indirect_input_iterator()
		: iter()
	{}
	indirect_input_iterator(iterator_type i)
		: iter(i)
	{
		++*this;
	}
	indirect_input_iterator(const indirect_input_iterator&) = default;
	//! \note 使用 std::swap 和 ADL 。
	indirect_input_iterator(indirect_input_iterator&& i) ynothrow
		: iter()
	{
		using std::swap;

		swap(iter, i.iter);
	}

	indirect_input_iterator&
	operator=(const indirect_input_iterator&) = default;
	indirect_input_iterator&
#if YB_IMPL_MSCPP
	//! \since build 458 as workaround for Visual C++ 2013
	operator=(indirect_input_iterator&& i)
	{
		iter = std::move(i.iter);
		return *this;
	}
#else
	operator=(indirect_input_iterator&&) = default;
#endif

	/*!
	\brief 间接操作。
	\pre 断言： <tt>!is_undereferenceable(iter)</tt> 。
	\since build 461
	*/
	reference
	operator*() const
	{
		yassume(!is_undereferenceable(iter));
		return **iter;
	}

	/*!
	\brief 迭代：向后遍历。
	\pre 断言： <tt>!is_undereferenceable(iter)</tt> 。
	*/
	indirect_input_iterator&
	operator++()
	{
		yassume(!is_undereferenceable(iter));
		++*iter;
		return *this;
	}

	friend bool
	operator==(const indirect_input_iterator& x,
		const indirect_input_iterator& y)
	{
		return (!bool(x) && !bool(y)) || x.iter == y.iter;
	}

	//! \since build 561
	explicit
	operator bool() const
	{
		return !is_undereferenceable(iter) && !is_undereferenceable(*iter);
	}

	iterator_type&
	get() ynothrow
	{
		return iter;
	}
	const iterator_type&
	get() const ynothrow
	{
		return iter;
	}
};


/*!
\ingroup iterator_adaptors
\brief 下标转置迭代器适配器。
\warning 非虚析构。
\since build 575

对指定行宽的线性访问元素的行优先矩阵元素迭代器进行转置变换的随机访问迭代器适配器。
对 w × h 矩阵的迭代器 \c i 和转置后的迭代器 \c j 以及无符号数 \c x 和 \c y ，满足：
<tt>i[x * w + y] = j[y * w + x]</tt> ，其中 <tt>x < w && y < h</tt>。
*/
template<typename _tIter>
class transposed_iterator : public iterator_operators_t<
	transposed_iterator<_tIter>, std::iterator_traits<_tIter>>
{
public:
	using iterator_type = _tIter;
	using iterator_category = std::random_access_iterator_tag;
	using value_type = typename std::iterator_traits<iterator_type>::value_type;
	using difference_type
		= typename std::iterator_traits<iterator_type>::difference_type;
	using pointer = typename std::iterator_traits<iterator_type>::pointer;
	using reference = typename std::iterator_traits<iterator_type>::reference;
	using size_type = make_unsigned_t<difference_type>;

private:
	iterator_type iter;
	size_type width, height, row, col;

public:
	transposed_iterator() = default;
	//! \since build 586
	//@{
	yconstfn
	transposed_iterator(iterator_type i, size_type w, size_type h)
		ynoexcept(and_<is_nothrow_copy_constructible<iterator_type>,
		is_nothrow_copy_constructible<size_type>>())
		: iter(i), width((yconstraint(w != 0), w)),
		height((yconstraint(h != 0), h)), row(), col(w)
	{}
	yconstfn
	transposed_iterator(iterator_type i, size_type w, size_type h,
		size_type idx)
		ynoexcept(and_<is_nothrow_copy_constructible<iterator_type>,
		is_nothrow_copy_constructible<size_type>>())
		: iter(i), width((yconstraint(w != 0), w)),
		height((yconstraint(h != 0), h)),
		row((yconstraint(!(w * h < idx)), idx % h)), col(idx / h)
	{}
	//@}
	transposed_iterator(const transposed_iterator&) = default;
#if YB_IMPL_MSCPP
	//! \since build 575 as workaround for Visual C++ 2013
	transposed_iterator(transposed_iterator&& i)
		: iter(std::move(i.iter)), width(std::move(i.width)),
		height(std::move(i.height)), row(std::move(i.row)),
		col(std::move(i.col))
	{}
#else
	transposed_iterator(transposed_iterator&&) = default;
#endif
	//@}

	transposed_iterator&
	operator=(const transposed_iterator&) = default;
	transposed_iterator&
	operator=(transposed_iterator&&) = default;

	transposed_iterator&
	operator+=(difference_type n)
	{
		auto idx(col * height + row);

		// NOTE: See $2015-02 @ %Documentation::Workflow::Annual2015.
		yassume(!(difference_type(idx) < -n)),
		yassume(!(difference_type(width * height) < difference_type(idx) + n));
		idx += size_type(n);
		yunseq(row = idx % height, col = idx / height);
		return *this;
	}

	transposed_iterator&
	operator-=(difference_type n)
	{
		return *this += -n;
	}

	reference
	operator*() const
	{
		yconstraint(!is_undereferenceable(*this));
		return *get();
	}

	transposed_iterator&
	operator++()
	{
		yconstraint(!is_undereferenceable(*this));
		if(++row == height)
			yunseq(row = 0, ++col);
		return *this;
	}

	transposed_iterator&
	operator--()
	{
		yconstraint(row != 0 || col != 0);
		if(row-- == 0)
			yunseq(row = height - 1, --col);
		return *this;
	}

	//! \since build 577
	friend bool
	operator==(const transposed_iterator& x, const transposed_iterator& y)
	{
		yconstraint(x.share_sequence(y));

		return x.iter == y.iter && x.get_index() == y.get_index();
	}

	friend bool
	operator<(const transposed_iterator& x, const transposed_iterator& y)
	{
		yconstraint(x.share_sequence(y));
		return x.row < y.row || (x.row == y.row && x.col < y.col);
	}

	friend difference_type
	operator-(const transposed_iterator& x, const transposed_iterator& y)
	{
		yconstraint(x.share_sequence(y));
		// XXX: Conversion to 'difference_type' might be implementation-defined.
		return difference_type((x.col - y.col) * x.height + x.row - y.row);
	}

	yconstfn const iterator_type&
	base() const ynothrow
	{
		return iter;
	}

	iterator_type
	get() const ynothrowv
	{
		ynoexcept_assert("Invalid type found.", iter + row * width + col);

		// XXX: Conversion to 'difference_type' might be implementation-defined.
		return iter + difference_type(get_index());
	}

	friend bool
	is_undereferenceable(const transposed_iterator& i) ynothrow
	{
		using ystdex::is_undereferenceable;

		return is_undereferenceable(i.iter)
			|| !(i.row < i.height && i.col < i.width);
	}

	yconstfn size_type
	get_column() const ynothrow
	{
		return col;
	}

	yconstfn size_type
	get_height() const ynothrow
	{
		return height;
	}

	yconstfn size_type
	get_index() const ynothrowv
	{
		ynoexcept_assert("Invalid type found.", row * width + col);

		return row * width + col;
	}

	yconstfn size_type
	get_row() const ynothrow
	{
		return row;
	}

	yconstfn size_type
	get_width() const ynothrow
	{
		return width;
	}

	bool
	share_sequence(const transposed_iterator& i) const ynothrow
	{
		yassume(!is_undereferenceable(*this)
			|| (row == 0 && col == width)),
		yassume(!is_undereferenceable(i)
			|| (i.row == 0 && i.col == i.width));
		return iter == i.iter && width == i.width && height == i.height;
	}
};

/*!
\ingroup helper_functions
\relates transposed_iterator
\since build 575
*/
//@{
template<typename _tIter, typename _tSize = make_unsigned_t<
	typename std::iterator_traits<decay_t<_tIter>>::difference_type>>
inline transposed_iterator<decay_t<_tIter>>
make_transposed(_tIter&& i, _tSize w, _tSize h)
{
	return transposed_iterator<decay_t<_tIter>>(yforward(i), w, h);
}
template<typename _tIter, typename _tSize = make_unsigned_t<
	typename std::iterator_traits<decay_t<_tIter>>::difference_type>,
	typename _tIndex = _tSize>
inline transposed_iterator<decay_t<_tIter>>
make_transposed(_tIter&& i, _tSize w, _tSize h, _tIndex idx)
{
	return transposed_iterator<decay_t<_tIter>>(yforward(i), w, h, idx);
}
//@}


/*!
\ingroup iterators
\brief 原型迭代器：共享对象引用，在解引用时进行指定的更新操作的随机访问迭代器。
\since build 522
*/
template<typename _type, typename _fUpdater>
class prototyped_iterator : public iterator_operators_t<
	prototyped_iterator<_type, _fUpdater>, std::iterator_traits<yimpl(_type*)>>
{
public:
	using updater_type = _fUpdater;
	using iterator_category = std::random_access_iterator_tag;
	using value_type = _type;
	using difference_type = ptrdiff_t;
	using pointer = _type*;
	using reference = _type&;

protected:
	//! \since build 595
	lref<value_type> proto_ref;
	size_t idx;
	updater_type updater;

public:
	template<typename _fCallable>
	yconstfn
	prototyped_iterator(value_type& proto, size_t i, _fCallable&& f)
		: proto_ref(proto), idx(i), updater(yforward(f))
	{}

	prototyped_iterator&
	operator+=(difference_type n)
	{
		idx += n;
		return *this;
	}

	prototyped_iterator&
	operator-=(difference_type n)
	{
		yassume(!(idx < n));
		idx -= n;
		return *this;
	}

	reference
	operator*() const
	{
		updater(proto_ref.get(), idx);
		return proto_ref;
	}

	prototyped_iterator&
	operator++() ynothrow
	{
		++idx;
		return *this;
	}

	prototyped_iterator
	operator--() ynothrow
	{
		--idx;
		return *this;
	}

	//! \since build 577
	friend bool
	operator==(const prototyped_iterator& x, const prototyped_iterator& y)
		ynothrow
	{
		return std::addressof(x.proto_ref.get())
			== std::addressof(y.proto_ref.get()) && x.idx == y.idx;
	}
};

/*!
\ingroup helper_functions
\brief 创建原型迭代器。
\relates prototyped_iterator
\since build 522
*/
template<typename _type, typename _fUpdater>
yconstfn prototyped_iterator<_type, _fUpdater>
make_prototyped_iterator(_type& proto, size_t i, _fUpdater f)
{
	return prototyped_iterator<_type, _fUpdater>(proto, i, f);
}


/*!
\ingroup iterators
\brief 成员下标迭代器。
\warning 非虚析构。
\since build 556

根据指定类型提供的下标操作枚举其成员的随机访问迭代器。
*/
template<class _tCon, typename _type, typename _tDifference = ptrdiff_t,
	typename _tPointer = _type*, typename _tReference = _type&>
class subscriptive_iterator : public random_access_iteratable<
	subscriptive_iterator<_tCon, _type, _tDifference, _tPointer, _tReference>,
	_tDifference, _tReference>
{
public:
	//! \since build 356
	//@{
	using container_type = _tCon;
	using iterator_category = std::random_access_iterator_tag;
	using value_type = _type;
	using difference_type = _tDifference;
	using pointer = _tPointer;
	using reference = _tReference;
	//@}

protected:
	_tCon* container_ptr = {};
	size_t index = size_t(-1);

public:
	/*!
	\post <tt>!is_undereferenceable(*this)</tt> 。
	\since build 566
	*/
	subscriptive_iterator() = default;
	//! \since build 522
	yconstfn
	subscriptive_iterator(container_type& c, size_t i) ynothrowv
		: container_ptr(std::addressof(c)), index(i)
	{}

	//! \since build 356
	//@{
	subscriptive_iterator&
	operator+=(difference_type n) ynothrowv
	{
		index += n;
		return *this;
	}

	subscriptive_iterator&
	operator-=(difference_type n) ynothrowv
	{
		yassume(!(index < n));
		index -= n;
		return *this;
	}
	//@}

	//! \since build 461
	//@{
	reference
	operator*() const ynothrowv
	{
		yassume(!is_undereferenceable(*this));
		return (*container_ptr)[index];
	}

	subscriptive_iterator&
	operator++() ynothrow
	{
		++index;
		return *this;
	}

	subscriptive_iterator
	operator--() ynothrow
	{
		--index;
		return *this;
	}
	//@}

	//! \since build 577
	friend bool
	operator==(const subscriptive_iterator& x, const subscriptive_iterator& y)
		ynothrow
	{
		yconstraint(x.container() == y.container());
		return is_undereferenceable(y) || x.index == y.index;
	}

	//! \since build 577
	friend bool
	operator<(const subscriptive_iterator& x, const subscriptive_iterator& y)
		ynothrow
	{
		yconstraint(x.container() == y.container());
		return x.index < y.index;
	}

	//! \since build 600
	friend _tDifference
	operator-(const subscriptive_iterator& x, const subscriptive_iterator& y)
		ynothrow
	{
		yconstraint(x.container_ptr == y.container_ptr);
		return _tDifference(x.index - y.index);
	}

	//! \since build 461
	yconstfn _tCon*
	container() const ynothrow
	{
		return container_ptr;
	}

	yconstfn size_t
	get_index() const ynothrow
	{
		return index;
	}

	//! \since build 556
	friend bool
	is_undereferenceable(const subscriptive_iterator& i) ynothrow
	{
		return !i.container_ptr;
	}
};

} // namespace ystdex;

#endif

