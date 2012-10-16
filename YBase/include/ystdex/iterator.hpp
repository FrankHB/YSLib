/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file iterator.hpp
\ingroup YStandardEx
\brief 通用迭代器。
\version r1150
\author FrankHB<frankhb1989@gmail.com>
\since build 347
\par 创建时间:
	2011-01-27 23:01:00 +0800
\par 修改时间:
	2012-10-16 21:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Iterator
*/


#ifndef YB_INC_YSTDEX_ITERATOR_HPP_
#define YB_INC_YSTDEX_ITERATOR_HPP_ 1

#include "any.h"
#include <iterator>
#include <utility>
#include "memory.hpp" // for std::addressof, ystdex::is_dereferencable,
//	ystdex::is_undereferencable;
#include "type_op.hpp" // for *_tag, remove_reference;
#include <tuple>

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
\ingroup helper_functions
\brief 取转移迭代器对。
\since build 337
*/
template<typename _tIterator1, typename _tIterator2>
inline auto
make_move_iterator_pair(_tIterator1 it1, _tIterator2 it2) -> decltype(
	std::make_pair(std::make_move_iterator(it1), std::make_move_iterator(it2)))
{
	return std::make_pair(std::make_move_iterator(it1),
		std::make_move_iterator(it2));
}
/*!
\ingroup helper_functions
\brief 取容器的迭代器对。
\since build 337
*/
template<typename _tContainer>
inline auto
make_move_iterator_pair(_tContainer& c)
	-> decltype(ystdex::make_move_iterator_pair(c.begin(), c.end()))
{
	return ystdex::make_move_iterator_pair(c.begin(), c.end());
}


/*!
\ingroup iterator_adaptors
\brief 指针迭代器。
\since build 290

转换指针为类类型的随机迭代器。
*/
template<typename _type>
class pointer_iterator
	: private std::iterator<typename
	std::iterator_traits<_type*>::iterator_category, _type>
{
protected:
	typedef typename std::iterator<typename
		std::iterator_traits<_type*>::iterator_category, _type> base_type;

public:
	typedef typename base_type::difference_type difference_type;
	typedef typename base_type::value_type value_type;
	typedef typename base_type::pointer pointer;
	typedef typename base_type::reference reference;
	typedef typename base_type::iterator_category iterator_category;

protected:
	mutable pointer current;

public:
	yconstfn
	pointer_iterator(std::nullptr_t = {})
		: current()
	{}
	//! \since build 347
	template<typename _tPointer>
	explicit yconstfn
	pointer_iterator(_tPointer&& ptr)
		: current(yforward(ptr))
	{}
	inline
	pointer_iterator(const pointer_iterator&) = default;
	inline
	pointer_iterator(pointer_iterator&&) = default;

	yconstfn reference
	operator*() const
	{
		return *current;
	}

	yconstfn pointer
	operator->() const
	{
		return current;
	}

	inline pointer_iterator&
	operator++()
	{
		++current;
		return *this;
	}
	yconstfn pointer_iterator
	operator++(int)
	{
		return current++;
	}

	inline pointer_iterator&
	operator--()
	{
		--current;
		return *this;
	}
	yconstfn pointer_iterator
	operator--(int)
	{
		return current--;
	}

	yconstfn reference
	operator[](const difference_type& n) const
	{
		return current[n];
	}

	inline pointer_iterator&
	operator+=(const difference_type& n)
	{
		current += n;
		return *this;
	}

	yconstfn pointer_iterator
	operator+(const difference_type& n) const
	{
		return pointer_iterator(current + n);
	}

	inline pointer_iterator&
	operator-=(const difference_type& n)
	{
		current -= n;
		return *this;
	}

	yconstfn pointer_iterator
	operator-(const difference_type& n) const
	{
		return pointer_iterator(current - n);
	}

	yconstfn
	operator pointer() const
	{
		return current;
	}
};


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
	typedef _type type;
};

template<typename _type>
struct pointer_classify<_type*>
{
	typedef pointer_iterator<_type> type;
};
//@}


/*!
\ingroup iterator_adaptors
\brief 伪迭代器。

总是返回单一值的迭代器适配器。
*/
template<typename _type, typename _tIterator = _type*,
	typename _tTraits = std::iterator_traits<_tIterator>>
class pseudo_iterator
{
protected:
	typedef _tTraits traits_type;

public:
	typedef _tIterator iterator_type;
	typedef typename traits_type::iterator_category iterator_category;
	typedef typename traits_type::value_type value_type;
	typedef typename traits_type::difference_type difference_type;
	typedef typename traits_type::reference reference;
	typedef typename traits_type::pointer pointer;

	value_type value;

	yconstfn
	pseudo_iterator()
		: value()
	{}
	explicit yconstfn
	pseudo_iterator(value_type v)
		: value(v)
	{}
	yconstfn inline
	pseudo_iterator(const pseudo_iterator&) = default;
	yconstfn inline
	pseudo_iterator(pseudo_iterator&&) = default;

	pseudo_iterator&
	operator=(const pseudo_iterator&) = default;
	pseudo_iterator&
	operator=(pseudo_iterator&&) = default;

	//前向迭代器需求。
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

	//双向迭代器需求。
	yconstfn pseudo_iterator&
	operator--()
	{
		return *this;
	}

	yconstfn pseudo_iterator
	operator--(int)
	{
		return *this;
	}

	//随机访问迭代器需求。
	yconstfn reference
	operator[](const difference_type& _n) const
	{
		return this[_n];
	}

	pseudo_iterator&
	operator+=(const difference_type&)
	{
		return *this;
	}

	yconstfn pseudo_iterator
	operator+(const difference_type&) const
	{
		return *this;
	}

	yconstfn pseudo_iterator&
	operator-=(const difference_type&)
	{
		return *this;
	}

	yconstfn pseudo_iterator
	operator-(const difference_type&) const
	{
		return *this;
	}
};


/*!
\ingroup iterator_adaptors
\brief 转换迭代器。
\since build 288

使用指定参数隐藏指定迭代器的间接操作的迭代器适配器。
*/
template<typename _tIterator, typename _fTransformer>
class transformed_iterator : public pointer_classify<_tIterator>::type
{
public:
	/*!
	\brief 原迭代器类型。
	\since build 290
	*/
	typedef typename pointer_classify<typename
		remove_reference<_tIterator>::type>::type iterator_type;
	typedef _fTransformer transformer_type;
	typedef decltype(std::declval<_fTransformer>()(
		std::declval<_tIterator&>())) result;
	typedef decltype(std::declval<_fTransformer>()(std::declval<const
		_tIterator&>())) const_result;

protected:
	mutable transformer_type transformer;

public:
	//! \since build 347
	template<typename _tIter, typename _tTran>
	explicit yconstfn
	transformed_iterator(_tIter&& i, _tTran&& f = {})
		: iterator_type(yforward(i)), transformer(f)
	{}

	inline result
	operator*()
	{
		return transformer(get());
	}
	inline const_result
	operator*() const
	{
		return transformer(get());
	}

	inline result*
	operator->()
	{
		return std::addressof(operator*());
	}
	inline const_result*
	operator->() const
	{
		return std::addressof(operator*());
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
};


/*!
\ingroup helper_functions
\brief 创建转换迭代器。
\note 使用 ADL 。
\since build 288
*/
template<typename _tIterator, typename _fTransformer>
inline transformed_iterator<typename array_ref_decay<_tIterator>::type,
	_fTransformer>
make_transform(_tIterator&& i, _fTransformer&& f)
{
	return transformed_iterator<typename array_ref_decay<_tIterator>::type,
		_fTransformer>(yforward(i), f);
}


/*!
\brief 成对迭代操作。
\since build 288
*/
template<typename _tIterator>
struct pair_iterate
{
	typedef _tIterator iterator_type;
	typedef decltype(*std::declval<_tIterator>()) reference;
	typedef typename remove_reference<reference>::type pair_type;
	typedef typename pair_type::first_type first_type;
	typedef typename pair_type::second_type second_type;

	static yconstfn auto
	first(const _tIterator& i) -> decltype(i->first)
	{
		return i->first;
	}
	static yconstfn auto
	second(const _tIterator& i) -> decltype(i->second)
	{
		return i->second;
	}
};


/*!
\brief 操纵子。
\since build 288
*/
//@{
yconstexpr first_tag get_first = {}, get_key = {};
yconstexpr second_tag get_second = {}, get_value = {};
//@}


/*!
\brief 管道匹配操作符。
\since build 288
*/
template<typename _tIterator>
inline auto
operator|(_tIterator&& i, first_tag)
	-> decltype(make_transform(yforward(i), pair_iterate<typename
	decay<_tIterator>::type>::first))
{
	return make_transform(yforward(i), pair_iterate<typename
		decay<_tIterator>::type>::first);
}
template<typename _tIterator>
inline auto
operator|(_tIterator&& i, second_tag)
	-> decltype(make_transform(yforward(i), pair_iterate<typename
	decay<_tIterator>::type>::second))
{
	return make_transform(yforward(i), pair_iterate<typename
		decay<_tIterator>::type>::second);
}


/*!
\ingroup iterator_adaptors
\brief 成对迭代器。

拼接两个迭代器对得到的迭代器适配器，以第一个为主迭代器的迭代器适配器。
*/
template<typename _tMaster, typename _tSlave,
	class _tTraits = std::iterator_traits<_tMaster>>
class pair_iterator : private std::pair<_tMaster, _tSlave>
{
protected:
	typedef _tTraits traits_type;

public:
	typedef std::pair<_tMaster, _tSlave> pair_type;
	typedef _tMaster iterator_type;
	typedef typename traits_type::iterator_category iterator_category;
	typedef typename traits_type::value_type value_type;
	typedef typename traits_type::difference_type difference_type;
	typedef typename traits_type::reference reference;
	typedef typename traits_type::pointer pointer;

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
	operator=(pair_iterator&&) = default;
	// TODO: Allow iterator to const_iterator conversion.

	//前向迭代器需求。
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
		const auto i(*this);

		++*this;
		return i;
	}

	//双向迭代器需求。
	pair_iterator&
	operator--()
	{
		yunseq(--this->first, --this->second);
		return *this;
	}

	pair_iterator
	operator--(int)
	{
		const auto i(*this);

		--*this;
		return i;
	}

	//随机访问迭代器需求。
	yconstfn reference
	operator[](const difference_type& _n) const
	{
		return this->first[_n];
	}

	pair_iterator&
	operator+=(const difference_type& _n)
	{
		yunseq(this->first += _n, this->second += _n);
		return *this;
	}

	yconstfn pair_iterator
	operator+(const difference_type& _n) const
	{
		return pair_iterator(this->first + _n, this->second + _n);
	}

	pair_iterator&
	operator-=(const difference_type& _n)
	{
		yunseq(this->first -= _n, this->second -= _n);
		return *this;
	}

	yconstfn pair_iterator
	operator-(const difference_type& _n) const
	{
		return pair_iterator(this->first - _n, this->second - _n);
	}

	yconstfn const pair_type&
	base() const
	{
		return *this;
	}
};


/*!
\brief 迭代器操作静态模版。
\since build 347
*/
template<typename _tIterator, typename _tReference
	= typename std::iterator_traits<_tIterator>::reference>
class iterator_operations
{
public:
	typedef _tIterator iterator_type;
	typedef std::iterator_traits<iterator_type> traits_type;
	typedef typename traits_type::value_type value_type;
	typedef typename traits_type::pointer pointer;
	typedef _tReference reference;
	/*!
	\brief 公共迭代器类型。
	*/
	typedef void_ref common_iterator;
	typedef enum
	{
		deref = 0,
		inc = 1,
		is_deref = 2,
		is_underef = 3,
		eq = 4
	} operation_id;
	/*
	\brief 迭代器操作类型。
	\note 模版实参 0 ：类型 reference(*)(common_iterator) ，解引用；
		模版实参 1 ：类型 void(*)(common_iterator) ，自增；
		模版实参 2 ：类型 bool(*)(common_iterator) ，判断是否确定可解引用；
		模版实参 3 ：类型 bool(*)(common_iterator) ，判断是否确定不可解引用；
		模版实参 4 ：类型 bool(*)(common_iterator, common_iterator) ，比较相等性。
	*/
	typedef std::tuple<
		reference(*)(common_iterator),
		void(*)(common_iterator),
		bool(*)(common_iterator),
		bool(*)(common_iterator),
		bool(*)(common_iterator, common_iterator)
	> operation_list;

	//! \since build 347
	//@{
	static bool
	are_equal(const iterator_type& x, const iterator_type& y)
	{
		return x == y;
	}
	static bool
	are_equal(common_iterator x, common_iterator y)
	{
		return x.operator iterator_type&() == y.operator iterator_type&();
	}

	static reference
	dereference(const iterator_type& i)
	{
		return *i;
	}
	//@}
	static reference
	dereference(common_iterator i)
	{
		return *i.operator iterator_type&();
	}

	static void
	increase(iterator_type& i)
	{
		++i;
	}
	static void
	increase(common_iterator i)
	{
		++i.operator iterator_type&();
	}

	static yconstfn bool
	is_dereferencable(common_iterator i)
	{
		return ystdex::is_dereferencable(i.operator iterator_type&());
	}

	static yconstfn bool
	is_undereferencable(common_iterator i)
	{
		return ystdex::is_undereferencable(i.operator iterator_type&());
	}

	/*
	\brief 迭代器操作。
	*/
	static yconstexpr operation_list operations{dereference, increase,
		is_dereferencable, is_undereferencable, are_equal};

protected:
	/*!
	\brief 无参数构造： \c protected 默认实现。
	\since build 296
	*/
	yconstfn iterator_operations() = default;
	/*!
	\brief 复制构造： \c protected 默认实现。
	\since build 296
	*/
	yconstfn iterator_operations(const iterator_operations&) = default;
};

template<typename _tIterator, typename _tReference>
yconstexpr typename iterator_operations<_tIterator, _tReference>::operation_list
	iterator_operations<_tIterator, _tReference>::operations;


/*!
\ingroup iterator_adaptors
\brief 单态输入迭代器。
\since 347

非多态输入迭代器适配器。
*/
template<typename _type, typename _tPointer, typename _tReference>
class any_input_iterator : public std::iterator<
	std::input_iterator_tag, _type, std::ptrdiff_t, _tPointer, _tReference>
{
public:
	typedef std::iterator<std::input_iterator_tag, _type,
		std::ptrdiff_t, _tPointer, _tReference> iterator_type;
	typedef iterator_operations<iterator_type> operations_base;
	typedef typename operations_base::value_type value_type;
	typedef typename operations_base::pointer pointer;
	typedef typename operations_base::reference reference;
	typedef typename operations_base::common_iterator common_iterator;
	typedef typename operations_base::operation_list operations_type;

private:
	common_iterator obj;
	const operations_type* operations_ptr;

public:
	any_input_iterator() = delete;
	/*!
	\brief 显式构造：使用现有迭代器。
	\post operations_ptr != nullptr 。
	\since build 347
	*/
	template<typename _tIterator>
	explicit
	any_input_iterator(_tIterator&& i)
		: obj(i), operations_ptr(&iterator_operations<typename
		std::remove_reference<_tIterator>::type, reference>::operations)
	{}
	any_input_iterator(const any_input_iterator&) = delete;

	any_input_iterator&
	operator++()
	{
		std::get<operations_base::inc>(*operations_ptr)(obj);
		return *this;
	}

	reference
	operator*() const
	{
		return std::get<operations_base::deref>(*operations_ptr)(obj);
	}

	pointer
	operator->() const
	{
		return &**this;
	}

	common_iterator
	get() const
	{
		return obj;
	}
	const operations_type&
	get_operations() const
	{
		return *operations_ptr;
	}

	/*!
	\brief 比较底层迭代器的类型是否相同。
	\since build 347
	*/
	bool
	same_type(const any_input_iterator& i) const
	{
		// NOTE: In some bad conforming implementation the result may be wrong
		//	because of boundaries.
		return operations_ptr == i.operations_ptr;
	}
};

/*!
\brief 比较单态输入迭代器的相等性。
\param x 左操作数。
\param y 右操作数。
\pre 断言检查 <tt>x.same_type(y)</tt> 。
\since build 347
*/
template<typename _type, typename _tPointer, typename _tReference>
inline bool
operator==(const any_input_iterator<_type, _tPointer, _tReference>& x,
	const any_input_iterator<_type, _tPointer, _tReference>& y)
{
	yconstraint(x.same_type(y));

	//any_iterator_base<...>::eq
	return std::get<4>(x.get_operations())(x.get(), y.get());
}

/*!
\brief 比较单态输入迭代器的不等性。
\since build 347
*/
template<typename _type, typename _tPointer, typename _tReference>
inline bool
operator!=(const any_input_iterator<_type, _tPointer, _tReference>& x,
	const any_input_iterator<_type, _tPointer, _tReference>& y)
{
	return !(x == y);
}

//! \since build 347
template<typename _type, typename _tPointer, typename _tReference>
inline bool
is_dereferencable(const
	any_input_iterator<_type, _tPointer, _tReference>& i)
{
	//any_iterator_base<...>::is_deref
	return std::get<2>(i.get_operations())(i.get());
}

//! \since build 347
template<typename _type, typename _tPointer, typename _tReference>
inline bool
is_undereferencable(const
	any_input_iterator<_type, _tPointer, _tReference>& i)
{
	//any_iterator_base<...>::is_underef
	return std::get<3>(i.get_operations())(i.get());
}

//! \since build 347
typedef any_input_iterator<void_ref, void*, void_ref>
	input_monomorphic_iterator;

} // namespace ystdex;

#endif

