/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file iterator.hpp
\ingroup YStandardEx
\brief 通用迭代器。
\version r3645
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 189
\par 创建时间:
	2011-01-27 23:01:00 +0800
\par 修改时间:
	2014-09-03 13:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Iterator
*/


#ifndef YB_INC_ystdex_iterator_hpp_
#define YB_INC_ystdex_iterator_hpp_ 1

#include "type_op.hpp" // for ystdex::remove_reference_t, ystdex::*_tag;
#include "cassert.h"
#include <iterator> // for std::make_move_iterator, std::iterator,
//	std::iterator_traits;
#include <utility> // for std::make_pair;
#include <memory> // for std::addressof;

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
\ingroup metafunctions
\brief 选择迭代器类型的特定重载避免和其它类型冲突。
\sa enable_if_t
\since build 483
*/
template<typename _tParam, typename = yimpl(std::pair<
	decltype(*std::declval<_tParam&>()), decltype(++std::declval<_tParam&>())>)>
using enable_for_iterator_t = enable_if_t<
	is_same<decltype(++std::declval<_tParam&>()), _tParam&>::value, int>;


/*!	\defgroup iterator_operations Iterator Operations
\ingroup iterators
\brief 迭代器操作。
\since build 375
*/
//@{

/*!	\defgroup is_undereferenceable Is Undereferenceable Iterator
\brief 判断迭代器实例是否不可解引用。
\tparam _tIter 迭代器类型。
\note 注意返回 \c false 不表示参数实际可解引用。
\since build 461
*/
//@{
template<typename _tIter>
yconstfn bool
is_undereferenceable(const _tIter&) ynothrow
{
	return {};
}
template<typename _type>
yconstfn bool
is_undereferenceable(_type* p) ynothrow
{
	return !p;
}
//@}

/*!
\brief 迭代器指向的值满足条件时取邻接迭代器，否则取原值。
\param i 指定的迭代器。
\pre 迭代器可解引用，蕴含断言：<tt>!is_undereferenceable(i)</tt> 。
*/
//@{
template<typename _tIn, typename _fPred>
_tIn
next_if(_tIn i, _fPred f,
	typename std::iterator_traits<_tIn>::difference_type n = 1)
{
	yconstraint(!is_undereferenceable(i));
	return f(*i) ? std::next(i, n) : i;
}
template<typename _tIn, typename _type>
_tIn
next_if_eq(_tIn i, const _type& val,
	typename std::iterator_traits<_tIn>::difference_type n = 1)
{
	yconstraint(!is_undereferenceable(i));
	return *i == val ? std::next(i, n) : i;
}
//@}

/*!
\brief 迭代器指向的值满足条件时取反向邻接迭代器，否则取原值。
\pre 迭代器可解引用。
*/
//@{
template<typename _tBi, typename _fPred>
_tBi
prev_if(_tBi i, _fPred f,
	typename std::iterator_traits<_tBi>::difference_type n = 1)
{
	return f(*i) ? std::prev(i, n) : i;
}
template<typename _tBi, typename _type>
_tBi
prev_if_eq(_tBi i, const _type& val,
	typename std::iterator_traits<_tBi>::difference_type n = 1)
{
	return *i == val ? std::prev(i, n) : i;
}
//@}

//@}


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
class pointer_iterator
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

	//! \since build 356
	pointer_iterator&
	operator+=(difference_type n)
	{
		yconstraint(raw);
		raw += n;
		return *this;
	}

	//! \since build 356
	pointer_iterator&
	operator-=(difference_type n)
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

	yconstfn pointer
	operator->() const
	{
		return raw;
	}

	inline pointer_iterator&
	operator++()
	{
		yconstraint(raw);
		++raw;
		return *this;
	}
	//! \since build 415
	pointer_iterator
	operator++(int)
	{
		return raw++;
	}

	inline pointer_iterator&
	operator--()
	{
		--raw;
		return *this;
	}
	//! \since build 415
	pointer_iterator
	operator--(int)
	{
		return raw--;
	}

	//! \since build 356
	reference
	operator[](difference_type n) const
	{
		yconstraint(raw);

		return raw[n];
	}

	//! \since build 356
	yconstfn pointer_iterator
	operator+(difference_type n) const
	{
		return pointer_iterator(raw + n);
	}

	//! \since build 356
	yconstfn pointer_iterator
	operator-(difference_type n) const
	{
		return pointer_iterator(raw - n);
	}

	yconstfn
	operator pointer() const
	{
		return raw;
	}
};

/*!
\relates pointer_iterator
\since build 356
*/
//@{
template<typename _type>
inline bool
operator==(const pointer_iterator<_type>& x, const pointer_iterator<_type>& y)
{
	using pointer = typename pointer_iterator<_type>::pointer;

	return pointer(x) == pointer(y);
}

template<typename _type>
inline bool
operator!=(const pointer_iterator<_type>& x, const pointer_iterator<_type>& y)
{
	return !(x == y);
}
//@}


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

总是返回单一值的迭代器适配器。
*/
template<typename _type, typename _tIter = _type*,
	typename _tTraits = std::iterator_traits<_tIter>>
class pseudo_iterator
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
	pseudo_iterator()
		: value()
	{}
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

	//! \since build 356
	pseudo_iterator&
	operator+=(difference_type)
	{
		return *this;
	}

	//! \since build 356
	pseudo_iterator&
	operator-=(difference_type)
	{
		return *this;
	}

	//! \brief 满足前向迭代器要求。
	//@{
	yconstfn reference
	operator*() const
	{
		return value;
	}

	yconstfn pointer
	operator->() const
	{
		return this;
	}

	pseudo_iterator&
	operator++()
	{
		return *this;
	}
	pseudo_iterator
	operator++(int)
	{
		return *this;
	}
	//@}

	//! \brief 满足双向迭代器要求。
	//@{
	yconstfn pseudo_iterator&
	operator--() const
	{
		return *this;
	}
	yconstfn pseudo_iterator
	operator--(int) const
	{
		return *this;
	}
	//@}

	/*!
	\brief 满足随机访问迭代器要求。
	\since build 356
	*/
	//@{
	yconstfn reference
	operator[](difference_type n) const
	{
		return this[n];
	}

	yconstfn pseudo_iterator
	operator+(difference_type) const
	{
		return *this;
	}

	yconstfn pseudo_iterator
	operator-(difference_type) const
	{
		return *this;
	}
	//@}
};

/*!
\brief 满足输入迭代器要求。
\relates pseudo_iterator
\since build 356
*/
//@{
template<typename _type, typename _tIter, typename _tTraits>
inline bool
operator==(const pseudo_iterator<_type, _tIter, _tTraits>& x,
	const pseudo_iterator<_type, _tIter, _tTraits>& y)
{
	return x.value == y.value;
}

template<typename _type, typename _tIter, typename _tTraits>
inline bool
operator!=(const pseudo_iterator<_type, _tIter, _tTraits>& x,
	const pseudo_iterator<_type, _tIter, _tTraits>& y)
{
	return !(x == y);
}
//@}


/*!
\ingroup iterator_adaptors
\brief 转换迭代器。
\pre 转换器必须满足 DefaultConstructible ，否则不保证可默认构造。
\note 对返回新值的二元操作复制的转换器基于第一操作数。
\warning 非虚析构。
\since build 532

使用指定参数转换得到新迭代器的间接操作替代指定原始类型的间接操作的迭代器适配器。
被替代的原始类型是迭代器类型，或除间接操作（可以不存在）外符合迭代器要求的类型。
*/
template<typename _tIter, typename _fTransformer, typename _tReference = void>
class transformed_iterator : public pointer_classify<decay_t<_tIter>>::type
{
	//! \since build 529
	static_assert(is_decayed<_tIter>::value, "Invalid type found.");
	//! \since build 529
	static_assert(is_decayed<_fTransformer>::value, "Invalid type found.");

public:
	/*!
	\brief 原迭代器类型。
	\since build 290
	*/
	using iterator_type = typename pointer_classify<_tIter>::type;
	using transformer_type = _fTransformer;
	//! \since build 439
	using transformed_type = result_of_t<transformer_type&(_tIter&)>;
	//! \since build 415
	using difference_type
		= typename std::iterator_traits<iterator_type>::difference_type;
	/*!
	\note 仅当参数 _tReference 为引用时可符合 ISO C++ 对 ForwardIterator 的要求。
	\since build 357
	*/
	using reference = conditional_t<is_same<_tReference, void>::value,
		transformed_type, _tReference>;

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

	//! \since build 357
	inline reference
	operator*() const
	{
		return transformer(get());
	}

	//! \since build 496
	template<typename _tDiff>
	enable_if_t<is_convertible<decltype(
		std::declval<iterator_type&>()[_tDiff()]), reference>::value, reference>
	operator[](_tDiff n)
	{
		return get()[difference_type(n)];
	}

	/*!
	\brief 转换为原迭代器引用。
	\since build 290
	*/
	inline
	operator iterator_type&()
	{
		return *this;
	}

	/*!
	\brief 转换为原迭代器 const 引用。
	\since build 290
	*/
	yconstfn
	operator const iterator_type&() const
	{
		return *this;
	}

	/*!
	\brief 取原迭代器引用。
	\since build 290
	*/
	inline iterator_type&
	get()
	{
		return *this;
	}

	/*!
	\brief 取原迭代器 const 引用。
	\since build 290
	*/
	yconstfn const iterator_type&
	get() const
	{
		return *this;
	}

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

/*!
\brief 满足输入迭代器要求。
\relates transformed_iterator
\since build 356
*/
//@{
template<typename _type, typename _fTransformer>
inline bool
operator==(const transformed_iterator<_type, _fTransformer>& x,
	const transformed_iterator<_type, _fTransformer>& y)
{
	return x.get() == y.get();
}

template<typename _type, typename _fTransformer>
inline bool
operator!=(const transformed_iterator<_type, _fTransformer>& x,
	const transformed_iterator<_type, _fTransformer>& y)
{
	return !(x == y);
}
//@}

/*!
\brief 满足随机迭代器要求。
\relates transformed_iterator
\since build 496
*/
//@{
template<typename _type, typename _fTransformer>
inline transformed_iterator<_type, _fTransformer>&
operator+=(transformed_iterator<_type, _fTransformer>& i,
	typename transformed_iterator<_type, _fTransformer>::difference_type n)
{
	i.get() += n;
	return i;
}

template<typename _type, typename _fTransformer>
inline transformed_iterator<_type, _fTransformer>&
operator-=(transformed_iterator<_type, _fTransformer>& i,
	typename transformed_iterator<_type, _fTransformer>::difference_type n)
{
	i.get() -= n;
	return i;
}

template<typename _type, typename _fTransformer>
transformed_iterator<_type, _fTransformer>
operator+(const transformed_iterator<_type, _fTransformer>& i,
	typename transformed_iterator<_type, _fTransformer>::difference_type n)
{
	auto it(i);

	it += n;
	return it;
}

template<typename _type, typename _fTransformer>
transformed_iterator<_type, _fTransformer>
operator-(const transformed_iterator<_type, _fTransformer>& i,
	typename transformed_iterator<_type, _fTransformer>::difference_type n)
{
	auto it(i);

	it -= n;
	return it;
}
template<typename _type, typename _fTransformer>
typename transformed_iterator<_type, _fTransformer>::difference_type
operator-(const transformed_iterator<_type, _fTransformer>& x,
	const transformed_iterator<_type, _fTransformer>& y)
{
	return x.get() - y.get();
}

template<typename _type, typename _fTransformer>
inline bool
operator<(const transformed_iterator<_type, _fTransformer>& x,
	const transformed_iterator<_type, _fTransformer>& y)
{
	return bool(x.get() < y.get());
}

template<typename _type, typename _fTransformer>
inline bool
operator<=(const transformed_iterator<_type, _fTransformer>& x,
	const transformed_iterator<_type, _fTransformer>& y)
{
	return !(y < x);
}

template<typename _type, typename _fTransformer>
inline bool
operator>(const transformed_iterator<_type, _fTransformer>& x,
	const transformed_iterator<_type, _fTransformer>& y)
{
	return y < x;
}

template<typename _type, typename _fTransformer>
inline bool
operator>=(const transformed_iterator<_type, _fTransformer>& x,
	const transformed_iterator<_type, _fTransformer>& y)
{
	return !(x < y);
}
//@}

/*!
\ingroup helper_functions
\brief 创建转换迭代器。
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
	template<typename _tIter>
	static yconstfn auto
	second(const _tIter& i) -> decltype((i->second))
	{
		return i->second;
	}
	template<typename _tIter>
	static yconstfn auto
	indirect(const _tIter& i) -> decltype((**i))
	{
		return **i;
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


/*!
\ingroup iterator_adaptors
\brief 成对迭代器。
\note 成员迭代器需可复制构造，满足随机迭代器要求（但不需要 + 和 - ）。
\warning 非虚析构。

拼接两个迭代器对得到的迭代器适配器，以第一个为主迭代器的迭代器适配器。
*/
template<typename _tMaster, typename _tSlave,
	class _tTraits = std::iterator_traits<_tMaster>>
class pair_iterator : private std::pair<_tMaster, _tSlave>
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
	pair_iterator()
		: std::pair<_tMaster, _tSlave>(_tMaster(), _tSlave())
	{}
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

	//! \since build 356
	pair_iterator&
	operator+=(difference_type n)
	{
		yunseq(this->first += n, this->second += n);
		return *this;
	}

	//! \since build 356
	pair_iterator&
	operator-=(difference_type n)
	{
		yunseq(this->first -= n, this->second -= n);
		return *this;
	}

	//! \brief 满足前向迭代器要求。
	//@{
	yconstfn reference
	operator*() const
	{
		return *this->first;
	}

	yconstfn pointer
	operator->() const
	{
		return this->first;
	}

	pair_iterator&
	operator++()
	{
		yunseq(++this->first, ++this->second);
		return *this;
	}
	pair_iterator
	operator++(int)
	{
		auto i(*this);

		++*this;
		return i;
	}
	//@}

	//! \brief 满足双向迭代器要求。
	//@{
	pair_iterator&
	operator--()
	{
		yunseq(--this->first, --this->second);
		return *this;
	}
	pair_iterator
	operator--(int)
	{
		auto i(*this);

		--*this;
		return i;
	}
	//@}

	/*!
	\brief 满足随机访问迭代器要求。
	\since build 356
	*/
	//@{
	yconstfn reference
	operator[](difference_type n) const
	{
		return this->first[n];
	}

	pair_iterator
	operator+(difference_type n) const
	{
		auto i(*this);

		yunseq(i.first += n, i.second += n);
		return i;
	}

	pair_iterator
	operator-(difference_type n) const
	{
		auto i(*this);

		yunseq(i.first -= n, i.second -= n);
		return i;
	}
	//@}

	//! \since build 439
	template<typename _tFirst, typename _tSecond,
		typename = enable_if_t<is_convertible<_tMaster, _tFirst>::value
		&& is_convertible<_tSlave, _tSecond>::value, int>>
	operator std::pair<_tFirst, _tSecond>()
	{
		return std::pair<_tFirst, _tSecond>(this->first, this->second);
	}

	yconstfn const pair_type&
	base() const
	{
		return *this;
	}
};

/*!
\relates pair_iterator
\since build 356
*/
//@{
template<typename _tMaster, typename _tSlave>
bool
operator==(const pair_iterator<_tMaster, _tSlave>& x,
	const pair_iterator<_tMaster, _tSlave>& y)
{
	return x.base().first == y.base().first
		&& x.base().second == y.base().second();
}

template<typename _tMaster, typename _tSlave>
inline bool
operator!=(const pair_iterator<_tMaster, _tSlave>& x,
	const pair_iterator<_tMaster, _tSlave>& y)
{
	return !(x == y);
}
//@}


/*!
\ingroup iterator_adaptors
\brief 间接输入迭代器。
\note 向指定类型的迭代器传递输入迭代器操作。
\since build 412
*/
template<typename _tIter>
class indirect_input_iterator
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

	pointer
	operator->() const
	{
		return (*iter).operator->();
	}

	//! \since build 439
	template<typename = enable_if_t<is_constructible<bool,
		decltype(*std::declval<iterator_type&>())>::value, int>>
	explicit
	operator bool() const
//	operator bool() const ynoexcept((!is_undereferenceable(std::declval<
//		iterator_type&>()) && bool(*std::declval<iterator_type&>())))
	{
		return !is_undereferenceable(iter) && bool(*iter);
	}

	/*!
	\brief 间接操作。
	\pre 断言：<tt>!is_undereferenceable(iter)</tt> 。
	\since build 461
	*/
	reference
	operator*() const
	{
		yconstraint(!is_undereferenceable(iter));
		return **iter;
	}

	/*!
	\brief 迭代：向后遍历。
	\pre 断言：<tt>!is_undereferenceable(iter)</tt> 。
	*/
	indirect_input_iterator&
	operator++()
	{
		yconstraint(!is_undereferenceable(iter));
		++*iter;
		return *this;
	}
	indirect_input_iterator
	operator++(int)
	{
		const auto i(*this);

		++*this;
		return i;
	}

	friend bool
	operator==(const indirect_input_iterator& x, const indirect_input_iterator& y)
	{
		return (!bool(x) && !bool(y)) || x.iter == y.iter;
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
\relates indirect_input_iterator
\since build 412
*/
template<typename _tIter>
inline bool
operator!=(const indirect_input_iterator<_tIter>& x,
	const indirect_input_iterator<_tIter>& y)
{
	return !(x == y);
}


/*!
\brief 原型迭代器：共享对象引用，在解引用时进行指定的更新操作。
\since build 522
*/
template<typename _type, typename _fUpdater>
class prototyped_iterator
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

	pointer
	operator->() const
	{
		return std::addressof(**this);
	}

	prototyped_iterator&
	operator++() ynothrow
	{
		++idx;
		return *this;
	}
	prototyped_iterator
	operator++(int) ynothrow
	{
		auto i(*this);

		++*this;
		return i;
	}

	prototyped_iterator
	operator--() ynothrow
	{
		--idx;
		return *this;
	}
	prototyped_iterator
	operator--(int) ynothrow
	{
		auto i(*this);

		--*this;
		return i;
	}

	reference
	operator[](difference_type n) const
	{
		yassume(!(idx + n < 0));
		updater(proto_ref.get(), idx + n);
		return proto_ref;
	}

	prototyped_iterator
	operator+(difference_type n) const
	{
		yassume(!(idx + n < 0));
		return prototyped_iterator(proto_ref, idx + n, updater);
	}

	prototyped_iterator
	operator-(difference_type n) const
	{
		yassume(!(idx + n < 0));
		return prototyped_iterator(proto_ref, idx - n, updater);
	}

	bool
	equals(const prototyped_iterator& i) const ynothrow
	{
		return std::addressof(proto_ref.get())
			== std::addressof(i.proto_ref.get()) && idx == i.idx;
	}
};

/*!
\relates prototyped_iterator
\since build 522
*/
//@{
//! \brief 比较原型迭代器的相等性。
template<typename _type, typename _fUpdater>
bool
operator==(const prototyped_iterator<_type, _fUpdater>& x,
	const prototyped_iterator<_type, _fUpdater>& y) ynothrow
{
	return x.equals(y);
}

//! \brief 比较原型迭代器的不等性。
template<typename _type, typename _fUpdater>
bool
operator!=(const prototyped_iterator<_type, _fUpdater>& x,
	const prototyped_iterator<_type, _fUpdater>& y) ynothrow
{
	return !(x == y);
}

/*!
\ingroup helper_functions
\brief 创建原型迭代器。
*/
template<typename _type, typename _fUpdater>
yconstfn prototyped_iterator<_type, _fUpdater>
make_prototyped_iterator(_type& proto, size_t i, _fUpdater f)
{
	return prototyped_iterator<_type, _fUpdater>(proto, i, f);
}
//@}


/*!
\ingroup iterators
\brief 成员下标迭代器。
\warning 非虚析构。
\since build 356 。

根据指定类型提供的下标操作枚举其成员的随机访问迭代器。
*/
template<class _tCon, typename _type>
class subscriptive_iterator
{
public:
	using container_type = _tCon;
	using iterator_category = std::random_access_iterator_tag;
	using value_type = _type;
	using difference_type = ptrdiff_t;
	using pointer = _type*;
	using reference = _type&;

protected:
	//! \since build 433
	_tCon* con_ptr;
	size_t idx;

public:
	//! \since build 522
	yconstfn
	subscriptive_iterator(container_type& c, size_t i)
		: con_ptr(std::addressof(c)), idx(i)
	{}

	subscriptive_iterator&
	operator+=(difference_type n)
	{
		idx += n;
		return *this;
	}

	subscriptive_iterator&
	operator-=(difference_type n)
	{
		yassume(!(idx < n));
		idx -= n;
		return *this;
	}

	//! \since build 461
	//@{
	reference
	operator*() const
	{
		return (*con_ptr)[idx];
	}

	pointer
	operator->() const
	{
		return std::addressof(**this);
	}

	subscriptive_iterator&
	operator++() ynothrow
	{
		++idx;
		return *this;
	}
	subscriptive_iterator
	operator++(int) ynothrow
	{
		auto i(*this);

		++*this;
		return i;
	}

	subscriptive_iterator
	operator--() ynothrow
	{
		--idx;
		return *this;
	}
	subscriptive_iterator
	operator--(int) ynothrow
	{
		auto i(*this);

		--*this;
		return i;
	}
	//@}

	reference
	operator[](difference_type n) const
	{
		yassume(!(idx + n < 0));
		return (*con_ptr)[idx + n];
	}

	subscriptive_iterator
	operator+(difference_type n) const
	{
		yassume(!(idx + n < 0));
		return subscriptive_iterator(*con_ptr, idx + n);
	}

	subscriptive_iterator
	operator-(difference_type n) const
	{
		yassume(!(idx + n < 0));
		return subscriptive_iterator(*con_ptr, idx - n);
	}

	//! \since build 461
	_tCon*
	container() const ynothrow
	{
		return con_ptr;
	}

	//! \since build 461
	bool
	equals(const subscriptive_iterator& i) const ynothrow
	{
		return con_ptr == i.con_ptr && idx == i.idx;
	}

	//! \since build 461
	size_t
	index() const ynothrow
	{
		return idx;
	}
};

/*!
\relates subscriptive_iterator
\since build 461
*/
//@{
//! \brief 比较成员下标迭代器的相等性。
template<class _tCon, typename _type>
bool
operator==(const subscriptive_iterator<_tCon, _type>& x,
	const subscriptive_iterator<_tCon, _type>& y) ynothrow
{
	return x.equals(y);
}

//! \brief 比较成员下标迭代器的不等性。
template<class _tCon, typename _type>
bool
operator!=(const subscriptive_iterator<_tCon, _type>& x,
	const subscriptive_iterator<_tCon, _type>& y) ynothrow
{
	return !(x == y);
}
//@}

} // namespace ystdex;

#endif

