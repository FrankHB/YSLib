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
\brief C++ 标准库迭代器扩展。
\version r1878;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 189 。
\par 创建时间:
	2011-01-27 23:01:00 +0800;
\par 修改时间:
	2012-08-04 00:28 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YStandardEx::Iterator;
*/


#ifndef YB_INC_YSTDEX_ITERATOR_HPP_
#define YB_INC_YSTDEX_ITERATOR_HPP_

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
\ingroup iterator_adaptors
\brief 指针迭代器。
\since build 290 。

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
	pointer_iterator(std::nullptr_t = nullptr)
		: current()
	{}
	template<typename _tPointer>
	yconstfn
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
\since build 290 。

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
class pseudo_iterator : private std::iterator<
	typename _tTraits::iterator_category,
	typename _tTraits::value_type,
	typename _tTraits::difference_type,
	typename _tTraits::pointer,
	typename _tTraits::reference>
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
\since build 288 。

使用指定参数隐藏指定迭代器的间接操作的迭代器适配器。
*/
template<typename _tIterator, typename _fTransformer>
class transformed_iterator : public pointer_classify<_tIterator>::type
{
public:
	/*!
	\brief 原迭代器类型。
	\since build 290 。
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
	template<typename _tIter, typename _tTran>
	yconstfn
	transformed_iterator(_tIter&& i, _tTran&& f = _tTran())
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
	\since build 290 。
	*/
	inline
	operator iterator_type&()
	{
		return *this;
	}

	/*!
	\brief 转换为原迭代器 const 引用。
	\since build 290 。
	*/
	yconstfn
	operator const iterator_type&() const
	{
		return *this;
	}

	/*!
	\brief 取原迭代器引用。
	\since build 290 。
	*/
	inline iterator_type&
	get()
	{
		return *this;
	}

	/*!
	\brief 取原迭代器 const 引用。
	\since build 290 。
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
\since build 288 。
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
\since build 288 。
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
\since build 288 。
*/
//@{
yconstexpr first_tag get_first = {}, get_key = {};
yconstexpr second_tag get_second = {}, get_value = {};
//@}


/*!
\brief 管道匹配操作符。
\since build 288 。
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
class pair_iterator : private std::iterator<
	typename _tTraits::iterator_category,
	typename _tTraits::value_type,
	typename _tTraits::difference_type,
	typename _tTraits::pointer,
	typename _tTraits::reference>,
	protected std::pair<_tMaster, _tSlave>
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
	// TODO: allow iterator to const_iterator conversion;

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
\brief 公共迭代器特征类。
*/
struct common_iterator_base
{
	/*!
	\brief 公共迭代器类型。
	*/
	typedef void_ref common_iterator;
	typedef void_ref value_type;
	typedef void* pointer;
	typedef void_ref reference;
	typedef enum
	{
		deref = 0,
		is_deref = 1,
		is_underef = 2,
		inc = 3
	} operation_t;
	/*
	\brief 迭代器操作类型。
	\note 模版实参 0 ：类型 reference(*)(common_iterator) ，解引用；
		模版实参 1 ：类型 bool(*)(common_iterator) ，判断是否确定可解引用；
		模版实参 2 ：类型 bool(*)(common_iterator) ，判断是否确定不可解引用；
		模版实参 3 ：类型 void(*)(common_iterator) ，自增。
	*/
	typedef std::tuple<
		reference(*)(common_iterator),
		bool(*)(common_iterator),
		bool(*)(common_iterator),
		void(*)(common_iterator)
	> operation_list;

protected:
	/*!
	\brief 无参数构造： \c protected 默认实现。
	\since build 296 。
	*/
	yconstfn common_iterator_base() = default;
	/*!
	\brief 复制构造： \c protected 默认实现。
	\since build 296 。
	*/
	yconstfn common_iterator_base(const common_iterator_base&) = default;
};


/*!
\brief 迭代器操作静态模版。
*/
template<typename _tIterator>
struct iterator_operations : public common_iterator_base
{
	typedef _tIterator iterator_type;

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

	static reference
	dereference(iterator_type& i)
	{
		return *i;
	}
	static reference
	dereference(common_iterator i)
	{
		return *i.operator iterator_type&();
	}

	/*
	\brief 迭代器操作。
	*/
	static yconstexpr operation_list operations{dereference, is_dereferencable,
		is_undereferencable, increase};

protected:
	/*!
	\brief 无参数构造： \c protected 默认实现。
	\since build 296 。
	*/
	yconstfn iterator_operations() = default;
	/*!
	\brief 复制构造： \c protected 默认实现。
	\since build 296 。
	*/
	yconstfn iterator_operations(const iterator_operations&) = default;
};

template<typename _tIterator>
yconstexpr typename common_iterator_base::operation_list
	iterator_operations<_tIterator>::operations;


/*!
\ingroup iterator_adaptors
\brief 单态输入迭代器。

非多态输入迭代器适配器。
*/
class input_monomorphic_iterator : private std::iterator<
	std::input_iterator_tag, void_ref, std::ptrdiff_t,
	void*, void_ref>
{
public:
	typedef common_iterator_base::common_iterator common_iterator;
	typedef common_iterator_base::operation_list operations_type;

private:
	common_iterator obj;
	const operations_type* operations_ptr;

public:
	input_monomorphic_iterator() = delete;
	/*!
	\brief 构造：使用现有迭代器。
	\post operations_ptr != nullptr 。
	*/
	template<typename _tIterator>
	input_monomorphic_iterator(_tIterator&& i)
		: obj(i), operations_ptr(&iterator_operations<typename
		std::remove_reference<_tIterator>::type>::operations)
	{}
	input_monomorphic_iterator(const input_monomorphic_iterator&) = delete;

	input_monomorphic_iterator&
	operator++()
	{
		std::get<common_iterator_base::inc>(*operations_ptr)(obj);
		return *this;
	}

	reference
	operator*() const
	{
		return std::get<common_iterator_base::deref>(*operations_ptr)(obj);
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
};

inline bool
is_dereferencable(const input_monomorphic_iterator& i)
{
	return std::get<common_iterator_base::is_deref>(i.get_operations())(
		i.get());
}

inline bool
is_undereferencable(const input_monomorphic_iterator& i)
{
	return std::get<common_iterator_base::is_underef>(i.get_operations())(
		i.get());
}

} // namespace ystdex;

#endif

