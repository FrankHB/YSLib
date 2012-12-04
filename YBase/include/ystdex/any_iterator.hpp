/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any_iterator.hpp
\ingroup YStandardEx
\brief 动态泛型迭代器。
\version r582
\author FrankHB<frankhb1989@gmail.com>
\since build 355
\par 创建时间:
	2012-11-08 14:28:42 +0800
\par 修改时间:
	2012-12-04 13:06 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::AnyIterator
*/


#ifndef YB_INC_YSTDEX_ANY_ITERATOR_HPP_
#define YB_INC_YSTDEX_ANY_ITERATOR_HPP_ 1

#include "any.h"
#include "cast.hpp" // for ystdex::polymorphic_downcast;
#include "memory.hpp" // for ystdex::is_dereferencable,
//	ystdex::is_undereferencable;

namespace ystdex
{

namespace any_ops
{

//! \since build 355
//@{
enum iterator_op : op_code
{
	check_dereferencable = end_base_op,
	check_undereferencable,
	dereference,
	increase,
	end_iterator_op
};


enum input_iterator_op : op_code
{
	equals = end_iterator_op,
	end_input_iterator_op,
	end_output_iterator_op = end_input_iterator_op,
	end_forward_iterator_op = end_input_iterator_op
};


enum bidirectional_iteartor_op : op_code
{
	decrease = end_forward_iterator_op,
	end_bidirectional_iterator_op
};


enum random_access_iteartor_op : op_code
{
	advance = end_forward_iterator_op,
	distance,
	less_compare,
	end_random_access_iteartor_op
};


template<typename _type>
struct wrap_handler
{
	typedef typename wrapped_traits<_type>::type value_type;
	typedef typename std::conditional<wrapped_traits<_type>::value,
		ref_handler<value_type>, value_handler<value_type>>::type type;
};


template<typename _type>
class iterator_handler : public wrap_handler<_type>::type
{
public:
	typedef typename wrap_handler<_type>::type base;
	typedef typename base::value_type value_type;

	using base::get_reference;

	using base::init;

	static void
	manage(any_storage& d, const any_storage& s, any_ops::op_code op)
	{
		switch(op)
		{
		case check_dereferencable:
			d.access<bool>() = is_dereferencable(get_reference(s));
			break;
		case check_undereferencable:
			d.access<bool>() = is_undereferencable(get_reference(s));
			break;
		case dereference:
			d = void_ref(*get_reference(s));
			break;
		case increase:
			++get_reference(d);
			break;
		default:
			base::manage(d, s, op);
		}
	}
};


template<typename _type>
class input_iterator_handler : public iterator_handler<_type>
{
public:
	typedef iterator_handler<_type> base;
	typedef typename base::value_type value_type;

	using base::get_reference;

	using base::init;

	static void
	manage(any_storage& d, const any_storage& s, any_ops::op_code op)
	{
		switch(op)
		{
		case equals:
			{
				const auto p(d.access<any_storage*>());

				d.access<bool>() = get_reference(*p) == get_reference(s);
			}
			break;
		default:
			base::manage(d, s, op);
		}
	}
};
//@}

} // namespace any_ops;


/*!
\ingroup iterator_adaptors
\brief 动态泛型输入迭代器。
\since 347
*/
template<typename _type, typename _tPointer = _type*,
	typename _tReference = _type&>
class any_input_iterator : public std::iterator<std::input_iterator_tag, _type,
	std::ptrdiff_t, _tPointer, _tReference>, protected any
{
public:
	typedef std::iterator<std::input_iterator_tag, _type,
		std::ptrdiff_t, _tPointer, _tReference> iterator_type;
	//! \since build 349
	typedef std::iterator_traits<iterator_type> traits_type;
	typedef typename traits_type::value_type value_type;
	typedef typename traits_type::pointer pointer;
	typedef _tReference reference;

	//! \since build 357
	any_input_iterator() = default;
	/*!
	\brief 构造：使用现有迭代器。
	\since build 356
	*/
	template<typename _tIterator>
	any_input_iterator(_tIterator&& i)
		: any()
	{
		typedef typename std::remove_reference<_tIterator>::type param_obj_type;
		typedef any_ops::input_iterator_handler<param_obj_type> handler;

		static_assert(is_convertible<decltype(*std::declval<typename
			wrapped_traits<param_obj_type>::type&>()), reference>::value,
			"Wrong target iterator type found.");

		manager = handler::manage;
		handler::init(storage, yforward(i));
	}
	//! \since build 356
	any_input_iterator(const any_input_iterator&) = default;
	//! \since build 356
	any_input_iterator(any_input_iterator&&) = default;

	reference
	operator*() const
	{
		yassume(manager);

		any_ops::any_storage t;

		manager(t, storage, any_ops::dereference);
		return reference(t.access<void_ref>());
	}

	pointer
	operator->() const
	{
		return &**this;
	}

	any_input_iterator&
	operator++()
	{
		yassume(manager);

		manager(storage, storage, any_ops::increase);
		return *this;
	}

	//! \since build 349
	any
	get() const
	{
		return static_cast<const any&>(*this);
	}

	//! \since build 355
	//@{
	bool
	check_dereferencable() const
	{
		if(manager)
		{
			any_ops::any_storage t;

			manager(t, storage, any_ops::check_dereferencable);
			return t.access<bool>();
		}
		return false;
	}

	bool
	check_undereferencable() const
	{
		if(manager)
		{
			any_ops::any_storage t;

			manager(t, storage, any_ops::check_undereferencable);
			return t.access<bool>();
		}
		return true;
	}

	bool
	equals(const any_input_iterator& i) const
	{
		if(!*this && !i)
			return true;

		yassume(type() == i.type());

		any_ops::any_storage t(&storage);

		manager(t, i.storage, any_ops::equals);
		return t.access<bool>();
	}
	//@}

	//! \since build 355
	using any::type;
};

/*!
\brief 比较单态输入迭代器的相等性。
\param x 左操作数。
\param y 右操作数。
\pre 断言检查 <tt>x.get_holder().type() == y.get_holder().type()</tt> 。
\since build 347
*/
template<typename _type, typename _tPointer, typename _tReference>
inline bool
operator==(const any_input_iterator<_type, _tPointer, _tReference>& x,
	const any_input_iterator<_type, _tPointer, _tReference>& y)
{
	return x.equals(y);
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
	return i.check_dereferencable();
}

//! \since build 347
template<typename _type, typename _tPointer, typename _tReference>
inline bool
is_undereferencable(const
	any_input_iterator<_type, _tPointer, _tReference>& i)
{
	return i.check_undereferencable();
}

//! \since build 347
typedef any_input_iterator<void_ref, void*, void_ref>
	input_monomorphic_iterator;

} // namespace ystdex;

#endif

