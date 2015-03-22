/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file iterator.hpp
\ingroup YStandardEx
\brief 通用迭代器。
\version r5080
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 189
\par 创建时间:
	2011-01-27 23:01:00 +0800
\par 修改时间:
	2015-03-21 09:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Iterator
*/


#ifndef YB_INC_ystdex_iterator_hpp_
#define YB_INC_ystdex_iterator_hpp_ 1

#include "iterator_op.hpp" // for std::reverse_iterator, std::pair,
//	std::make_move_iterator, std::iterator_traits, yconstraint, yassume,
//	ystdex::*_tag, ystdex::is_undereferenceable;
#include "operators.hpp" // for ystdex::random_access_iteratable,
//	std::addressof;

namespace ystdex
{

/*!	\defgroup iterators Iterators
\brief 迭代器。
*/

/*!	\defgroup iterator_adaptors Iterator Adaptors
\ingroup iterators
\brief 迭代器适配器。
*/


//! \ingroup helper_functions
//@{
/*!
\brief 构造反向迭代器。
\see WG21/N3936 24.5.1.3.21[reverse.iter.make] 。
\see http://wg21.cmeerw.net/lwg/issue2285 。
\since build 531
*/
template<typename _tIter>
std::reverse_iterator<_tIter>
make_reverse_iterator(_tIter i)
{
	return std::reverse_iterator<_tIter>(i);
}

/*!
\brief 构造转移迭代器对。
\since build 337
*/
template<typename _tIter1, typename _tIter2>
inline auto
make_move_iterator_pair(_tIter1 it1, _tIter2 it2) -> decltype(
	std::make_pair(std::make_move_iterator(it1), std::make_move_iterator(it2)))
{
	return std::make_pair(std::make_move_iterator(it1),
		std::make_move_iterator(it2));
}
/*!
\brief 构造指定序列范围（包含序列容器及内建数组等）的转移迭代器对。
\note 使用 ADL <tt>begin</tt> 和 <tt>end</tt> 指定范围迭代器。
\bug decltype 指定的返回类型不能使用 ADL 。
\since build 337
*/
template<typename _tRange>
inline auto
make_move_iterator_pair(_tRange& c)
	-> decltype(ystdex::make_move_iterator_pair(c.begin(), c.end()))
{
	using std::begin;
	using std::end;

	return ystdex::make_move_iterator_pair(begin(c), end(c));
}
//@}


//! \since build 576
namespace details
{

template<typename _tIter, typename _tTraits,
	typename _tTag = typename _tTraits::iterator_category>
struct iterator_operators;

template<typename _tIter, typename _tTraits>
struct iterator_operators<_tIter, _tTraits, std::input_iterator_tag>
{
	using type = input_iteratable<_tIter, typename _tTraits::reference>;
};

template<typename _tIter, typename _tTraits>
struct iterator_operators<_tIter, _tTraits, std::output_iterator_tag>
{
	using type = output_iteratable<_tIter, typename _tTraits::reference>;
};

template<typename _tIter, typename _tTraits>
struct iterator_operators<_tIter, _tTraits, std::forward_iterator_tag>
{
	using type = forward_iteratable<_tIter, typename _tTraits::reference>;
};

template<typename _tIter, typename _tTraits>
struct iterator_operators<_tIter, _tTraits, std::bidirectional_iterator_tag>
{
	using type = bidirectional_iteratable<_tIter, typename _tTraits::reference>;
};

template<typename _tIter, typename _tTraits>
struct iterator_operators<_tIter, _tTraits, std::random_access_iterator_tag>
{
	using type = random_access_iteratable<_tIter,
		typename _tTraits::difference_type, typename _tTraits::reference>;
};

} // namespace details;


/*!
\ingroup metafunctions
\brief 按迭代器类别取可实现迭代器的重载操作符集合的实现。
\note 仅使用第二参数的特定成员，可以是兼容 std::iterator 的实例的类型。
\warning 实例作为实现迭代器的基类时不应使用默认参数，因为此时无法访问成员类型。
\since build 576
*/
template<typename _tIter, typename _tTraits = std::iterator_traits<_tIter>>
using iterator_operators_t
	= typename details::iterator_operators<_tIter, _tTraits>::type;


/*!
\ingroup iterator_adaptors
\brief 指针迭代器。
\note 转换为 bool 、有序比较等操作使用转换为对应指针实现。
\warning 非虚析构。
\since build 290

转换指针为类类型的随机访问迭代器。
\todo 和 std::pointer_traits 交互。
*/
template<typename _type>
class pointer_iterator : public iterator_operators_t<pointer_iterator<_type>,
	std::iterator_traits<_type*>>
{
public:
	using iterator_type = _type*;
	using iterator_category
		= typename std::iterator_traits<iterator_type>::iterator_category;
	using value_type = typename std::iterator_traits<iterator_type>::value_type;
	using difference_type
		= typename std::iterator_traits<iterator_type>::difference_type;
	using pointer = typename std::iterator_traits<iterator_type>::pointer;
	using reference = typename std::iterator_traits<iterator_type>::reference;

protected:
	//! \since build 415
	pointer raw;

public:
	yconstfn
	pointer_iterator(nullptr_t = {})
		: raw()
	{}
	//! \since build 347
	template<typename _tPointer>
	explicit yconstfn
	pointer_iterator(_tPointer&& ptr)
		: raw(yforward(ptr))
	{}
	inline
	pointer_iterator(const pointer_iterator&) = default;

	//! \since build 585
	//@{
	pointer_iterator&
	operator+=(difference_type n) ynothrowv
	{
		yconstraint(raw);
		raw += n;
		return *this;
	}

	pointer_iterator&
	operator-=(difference_type n) ynothrowv
	{
		yconstraint(raw);
		raw -= n;
		return *this;
	}

	//! \since build 461
	reference
	operator*() const ynothrowv
	{
		yconstraint(raw);
		return *raw;
	}

	inline pointer_iterator&
	operator++() ynothrowv
	{
		yconstraint(raw);
		++raw;
		return *this;
	}

	inline pointer_iterator&
	operator--() ynothrowv
	{
		--raw;
		return *this;
	}

	yconstfn
	operator pointer() const ynothrow
	{
		return raw;
	}
	//@}
};

/*!
\relates pointer_iterator
\since build 585
*/
template<typename _type>
inline bool
operator==(const pointer_iterator<_type>& x, const pointer_iterator<_type>& y)
	ynothrow
{
	using pointer = typename pointer_iterator<_type>::pointer;

	return pointer(x) == pointer(y);
}


/*!
\ingroup meta_operations
\brief 指针包装为类类型迭代器。
\since build 290

若参数是指针类型则包装为 pointer_iterator 。
*/
//@{
template<typename _type>
struct pointer_classify
{
	using type = _type;
};

template<typename _type>
struct pointer_classify<_type*>
{
	using type = pointer_iterator<_type>;
};
//@}


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

	pseudo_iterator&
	operator++() ynothrow
	{
		return *this;
	}

	//! \brief 满足双向迭代器要求。
	yconstfn pseudo_iterator&
	operator--() const ynothrow
	{
		return *this;
	}
	//@}
};

//! \relates pseudo_iterator
//@{
/*!
\brief 满足输入迭代器要求。
\since build 585
*/
template<typename _type, typename _tIter, typename _tTraits>
yconstfn bool
operator==(const pseudo_iterator<_type, _tIter, _tTraits>& x,
	const pseudo_iterator<_type, _tIter, _tTraits>& y)
	ynoexcept_spec(bool(x.value == y.value))
{
	return x.value == y.value;
}

template<typename _type, typename _tIter, typename _tTraits>
yconstfn bool
operator<(const pseudo_iterator<_type, _tIter, _tTraits>& x,
	const pseudo_iterator<_type, _tIter, _tTraits>& y)
	ynoexcept_spec(bool(x.value < y.value))
{
	return x.value < y.value;
}

template<typename _type, typename _tIter, typename _tTraits>
inline yconstfn
	typename pseudo_iterator<_type, _tIter, _tTraits>::difference_type
operator-(const pseudo_iterator<_type, _tIter, _tTraits>&,
	const pseudo_iterator<_type, _tIter, _tTraits>&) ynothrow
{
	return 0;
}
//@}


namespace details
{

//! \since build 576
template<typename _tIter, typename _fTransformer, typename _tReference>
struct transit_traits
{
	using iterator_type = typename pointer_classify<_tIter>::type;
	using iterator_category
		= typename std::iterator_traits<iterator_type>::iterator_category;
	using transformed_type = result_of_t<_fTransformer&(_tIter&)>;
	using difference_type
		= typename std::iterator_traits<iterator_type>::difference_type;
	using reference = conditional_t<is_same<_tReference, void>::value,
		transformed_type, _tReference>;
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
template<typename _tIter, typename _fTransformer, typename _tReference = void>
class transformed_iterator : public pointer_classify<_tIter>::type,
	public iterator_operators_t<transformed_iterator<_tIter, _fTransformer,
	_tReference>,
	yimpl(details::transit_traits<_tIter, _fTransformer, _tReference>)>
{
	//! \since build 529
	static_assert(is_decayed<_tIter>::value, "Invalid type found.");
	//! \since build 529
	static_assert(is_decayed<_fTransformer>::value, "Invalid type found.");

private:
	//! \since build 576
	using impl_traits
		= details::transit_traits<_tIter, _fTransformer, _tReference>;
public:
	/*!
	\brief 原迭代器类型。
	\since build 290
	*/
	using iterator_type = typename impl_traits::iterator_type;
	using transformer_type = _fTransformer;
	//! \since build 439
	using transformed_type = typename impl_traits::transformed_type;
	//! \since build 415
	using difference_type = typename impl_traits::difference_type;
	/*!
	\note 仅当参数 _tReference 为引用时可符合 ISO C++ 对 ForwardIterator 的要求。
	\since build 357
	*/
	using reference = typename impl_traits::reference;

protected:
	//! \note 当为空类时作为第一个成员可启用空基类优化。
	mutable transformer_type transformer;

public:
	//! \since build 496
	transformed_iterator() = default;
	//! \since build 494
	template<typename _tIterOrig, typename _tTran,
		yimpl(typename = exclude_self_ctor_t<transformed_iterator, _tIterOrig>)>
	explicit yconstfn
	transformed_iterator(_tIterOrig&& i, _tTran f = {})
		: iterator_type(yforward(i)), transformer(f)
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

	//! \since build 585
	//@{
	reference
	operator*() const
		ynoexcept_spec(reference(std::declval<transformed_iterator&>().
		transformer(std::declval<transformed_iterator&>().get())))
	{
		return transformer(get());
	}

	/*!
	\brief 转换为原迭代器引用。
	*/
	inline
	operator iterator_type&() ynothrow
	{
		return *this;
	}

	/*!
	\brief 转换为原迭代器 const 引用。
	*/
	yconstfn
	operator const iterator_type&() const ynothrow
	{
		return *this;
	}

	/*!
	\brief 取原迭代器引用。
	*/
	iterator_type&
	get() ynothrow
	{
		return *this;
	}

	/*!
	\brief 取原迭代器 const 引用。
	*/
	yconstfn const iterator_type&
	get() const ynothrow
	{
		return *this;
	}
	//@}

	//! \since build 412
	//@{
	transformer_type&
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

//! \since build 585
//@{
/*!
\brief 满足输入迭代器要求。
\relates transformed_iterator
*/
template<typename _type, typename _fTransformer>
inline bool
operator==(const transformed_iterator<_type, _fTransformer>& x,
	const transformed_iterator<_type, _fTransformer>& y)
	ynoexcept_spec(bool(x.get() == y.get()))
{
	return x.get() == y.get();
}

/*!
\brief 满足随机迭代器要求。
\relates transformed_iterator
*/
//@{
template<typename _type, typename _fTransformer>
inline transformed_iterator<_type, _fTransformer>&
operator+=(transformed_iterator<_type, _fTransformer>& i,
	typename transformed_iterator<_type, _fTransformer>::difference_type n)
	ynoexcept(noexcept(decltype(i)(i)) && noexcept(i.get() += n))
{
	i.get() += n;
	return i;
}

template<typename _type, typename _fTransformer>
inline transformed_iterator<_type, _fTransformer>&
operator-=(transformed_iterator<_type, _fTransformer>& i,
	typename transformed_iterator<_type, _fTransformer>::difference_type n)
	ynoexcept(noexcept(decltype(i)(i)) && noexcept(i.get() -= n))
{
	i.get() -= n;
	return i;
}

template<typename _type, typename _fTransformer>
inline bool
operator<(const transformed_iterator<_type, _fTransformer>& x,
	const transformed_iterator<_type, _fTransformer>& y)
	ynoexcept_spec(bool(x.get() < y.get()))
{
	return bool(x.get() < y.get());
}
//@}

/*!
\ingroup helper_functions
\brief 创建变换迭代器。
\relates transformed_iterator
\since build 529
*/
template<typename _tIter, typename _fTransformer>
inline transformed_iterator<decay_t<_tIter>, _fTransformer>
make_transform(_tIter&& i, _fTransformer f)
{
	return transformed_iterator<decay_t<_tIter>, _fTransformer>(yforward(i), f);
}


/*!
\brief 迭代转换操作。
\since build 358
*/
namespace iterator_transformation
{

//! \since build 412
//@{
template<typename _tIter>
static yconstfn auto
first(const _tIter& i) -> decltype((i->first))
{
	return i->first;
}

//! \since build 536
template<typename _tIter>
static yconstfn auto
get(const _tIter& i) -> decltype(((*i).get()))
{
	return (*i).get();
}

template<typename _tIter>
static yconstfn auto
indirect(const _tIter& i) -> decltype((**i))
{
	return **i;
}

template<typename _tIter>
static yconstfn auto
second(const _tIter& i) -> decltype((i->second))
{
	return i->second;
}
//@}

} // namespace iterator_transformation;


/*!
\brief 操纵子。
\since build 288
*/
//@{
yconstexpr first_tag get_first{}, get_key{};
yconstexpr second_tag get_second{}, get_value{};
//! \since build 358
yconstexpr struct indirect_tag{} get_indirect{};
//! \since build 536
yconstexpr struct get_tag{} get_get{};
//@}


/*!
\brief 管道匹配操作符。
\since build 288
*/
template<typename _tIter>
inline auto
operator|(_tIter&& i, first_tag)
	-> decltype(make_transform(yforward(i), iterator_transformation::first<
	typename array_ref_decay<_tIter>::type>))
{
	return make_transform(yforward(i), iterator_transformation::first<
		typename array_ref_decay<_tIter>::type>);
}
template<typename _tIter>
inline auto
operator|(_tIter&& i, second_tag)
	-> decltype(make_transform(yforward(i), iterator_transformation::second<
	typename array_ref_decay<_tIter>::type>))
{
	return make_transform(yforward(i), iterator_transformation::second<
		typename array_ref_decay<_tIter>::type>);
}
//! \since build 358
template<typename _tIter>
inline auto
operator|(_tIter&& i, indirect_tag)
	-> decltype(make_transform(yforward(i), iterator_transformation::indirect<
	typename array_ref_decay<_tIter>::type>))
{
	return make_transform(yforward(i), iterator_transformation::indirect<
		typename array_ref_decay<_tIter>::type>);
}
//! \since build 536
template<typename _tIter>
inline auto
operator|(_tIter&& i, get_tag)
	-> decltype(make_transform(yforward(i), iterator_transformation::get<
	typename array_ref_decay<_tIter>::type>))
{
	return make_transform(yforward(i), iterator_transformation::get<
		typename array_ref_decay<_tIter>::type>);
}


/*!
\ingroup iterator_adaptors
\brief 成对迭代器。
\note 成员迭代器需可复制构造，满足随机迭代器要求（但不需要 + 和 - ）。
\warning 非虚析构。

拼接两个迭代器对得到的迭代器适配器，以第一个为主迭代器的迭代器适配器。
*/
template<typename _tMaster, typename _tSlave,
	class _tTraits = std::iterator_traits<_tMaster>>
class pair_iterator : private std::pair<_tMaster, _tSlave>, public
	iterator_operators_t<pair_iterator<_tMaster, _tSlave, _tTraits>, _tTraits>
{
public:
	using pair_type = std::pair<_tMaster, _tSlave>;
	using iterator_type = _tMaster;
	//! \since build 400
	using traits_type = _tTraits;
	using iterator_category = typename traits_type::iterator_category;
	using value_type = typename traits_type::value_type;
	using difference_type = typename traits_type::difference_type;
	using pointer = typename traits_type::pointer;
	using reference = typename traits_type::reference;

	yconstfn
	pair_iterator() = default;
	explicit yconstfn
	pair_iterator(const _tMaster& _i)
		: std::pair<_tMaster, _tSlave>(_i, _tSlave())
	{}
	yconstfn
	pair_iterator(const _tMaster& _i, const _tSlave& _s)
		: std::pair<_tMaster, _tSlave>(_i, _s)
	{}
	yconstfn
	pair_iterator(const pair_iterator&) = default;
	yconstfn
	pair_iterator(pair_iterator&& _r)
		: std::pair<_tMaster, _tSlave>(std::move(_r))
	{}

	inline pair_iterator&
	operator=(const pair_iterator&) = default;
	inline pair_iterator&
#if YB_IMPL_MSCPP
	//! \since build 458 as workaround for Visual C++ 2013
	operator=(pair_iterator&& i)
	{
		static_cast<std::pair<_tMaster, _tSlave>&>(*this)
			= static_cast<std::pair<_tMaster, _tSlave>&&>(i);
		return *this;
	}
#else
	operator=(pair_iterator&&) = default;
#endif

	//! \since build 585
	//@{
	pair_iterator&
	operator+=(difference_type n)
		ynoexcept_spec(yunseq(std::declval<pair_iterator&>().first += n,
		std::declval<pair_iterator&>().second += n))
	{
		yunseq(this->first += n, this->second += n);
		return *this;
	}

	pair_iterator&
	operator-=(difference_type n)
		ynoexcept_spec(yunseq(std::declval<pair_iterator&>().first -= n,
		std::declval<pair_iterator&>().second -= n))
	{
		yunseq(this->first -= n, this->second -= n);
		return *this;
	}

	//! \brief 满足输入迭代器要求。
	//@{
	yconstfn reference
	operator*() const
		ynoexcept_spec(reference(*std::declval<pair_iterator&>().first))
	{
		return *this->first;
	}

	pair_iterator&
	operator++() ynoexcept_spec(yunseq(++std::declval<pair_iterator&>().first,
		++std::declval<pair_iterator&>().second))
	{
		yunseq(++this->first, ++this->second);
		return *this;
	}
	//@}

	//! \brief 满足双向迭代器要求。
	pair_iterator&
	operator--() ynoexcept_spec(yunseq(--std::declval<pair_iterator&>().first,
		--std::declval<pair_iterator&>().second))
	{
		yunseq(--this->first, --this->second);
		return *this;
	}

	friend bool
	operator==(const pair_iterator& x, const pair_iterator& y)
		ynoexcept_spec(bool(x.first == y.first && x.second == y.second()))
	{
		return x.first == y.first && x.second == y.second();
	}

	template<typename _tFirst, typename _tSecond,
		yimpl(typename = enable_if_convertible_t<_tMaster, _tFirst>,
		typename = enable_if_convertible_t<_tSlave, _tSecond>)>
	operator std::pair<_tFirst, _tSecond>() ynoexcept(
		std::is_nothrow_copy_constructible<std::pair<_tFirst, _tSecond>()>
		::value && noexcept(std::pair<_tFirst, _tSecond>(std::declval<
		pair_iterator&>().first, std::declval<pair_iterator&>().second)))
	{
		return std::pair<_tFirst, _tSecond>(this->first, this->second);
	}

	yconstfn const pair_type&
	base() const ynothrow
	{
		return *this;
	}
	//@}
};


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
	\pre 断言：<tt>!is_undereferenceable(iter)</tt> 。
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
	\pre 断言：<tt>!is_undereferenceable(iter)</tt> 。
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
		ynoexcept(is_nothrow_copy_constructible<iterator_type>::value
		&& is_nothrow_copy_constructible<size_type>::value)
		: iter(i), width((yconstraint(w != 0), w)),
		height((yconstraint(h != 0), h)), row(), col(w)
	{}
	yconstfn
	transposed_iterator(iterator_type i, size_type w, size_type h,
		size_type idx)
		ynoexcept(is_nothrow_copy_constructible<iterator_type>::value
		&& is_nothrow_copy_constructible<size_type>::value)
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

		// See $2015-02 @ %Documentation::Workflow::Annual2015.
		yassume(!(difference_type(idx) < -n)),
		yassume(!(difference_type(width * height) < difference_type(idx) + n));
		idx += n;
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
		return (x.col - y.col) * x.height + x.row - y.row;
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

		return iter + get_index();
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
	std::reference_wrapper<value_type> proto_ref;
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
class subscriptive_iterator : public iterator_operators_t<
	subscriptive_iterator<_tCon, _type, _tDifference, _tPointer, _type>,
	yimpl(std::iterator<std::random_access_iterator_tag, _type, _tDifference,
	_tPointer, _tReference>)>
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

/*!
\relates subscriptive_iterator
\since build 556
*/
template<class _tCon, typename _type, typename _tDifference, typename _tPointer,
	typename _tReference>
inline _tDifference
operator-(const subscriptive_iterator<_tCon, _type, _tDifference, _tPointer,
	_tReference>& x, const subscriptive_iterator<_tCon, _type, _tDifference,
	_tPointer, _tReference>& y) ynothrow
{
	yconstraint(x.container() == y.container());
	return x.get_index() - y.get_index();
}

} // namespace ystdex;

#endif

