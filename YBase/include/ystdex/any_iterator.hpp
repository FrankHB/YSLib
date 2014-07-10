/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any_iterator.hpp
\ingroup YStandardEx
\brief 动态泛型迭代器。
\version r956
\author FrankHB <frankhb1989@gmail.com>
\since build 355
\par 创建时间:
	2012-11-08 14:28:42 +0800
\par 修改时间:
	2014-07-10 05:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::AnyIterator
*/


#ifndef YB_INC_ystdex_any_iterator_hpp_
#define YB_INC_ystdex_any_iterator_hpp_ 1

#include "any.h"
#include "functional.hpp" // for ystdex::wrapped_traits;
#include "iterator.hpp" // for ystdex::is_undereferenceable;

namespace ystdex
{

namespace any_ops
{

//! \since build 355
//@{
enum iterator_op : op_code
{
	//! \since build 400
	check_undereferenceable = end_base_op,
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
	using value_type = typename wrapped_traits<_type>::type;
	using type = conditional_t<wrapped_traits<_type>::value,
		ref_handler<value_type>, value_handler<value_type>>;
};


template<typename _type>
class iterator_handler : public wrap_handler<_type>::type
{
public:
	using base = typename wrap_handler<_type>::type;
	using value_type = typename base::value_type;

	using base::get_reference;

	using base::init;

	static void
	manage(any_storage& d, const any_storage& s, any_ops::op_code op)
	{
		switch(op)
		{
		case check_undereferenceable:
			d.access<bool>() = is_undereferenceable(get_reference(s));
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
	using base = iterator_handler<_type>;
	using value_type = typename base::value_type;

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


//! \since build 400
template<typename _type>
class forward_iterator_handler : public input_iterator_handler<_type>
{
public:
	using base = input_iterator_handler<_type>;
	using value_type = typename base::value_type;

	using base::get_reference;

	using base::init;

	using base::manage;
};


//! \since build 400
template<typename _type>
class bidirectional_iterator_handler : public forward_iterator_handler<_type>
{
public:
	using base = forward_iterator_handler<_type>;
	using value_type = typename base::value_type;

	using base::get_reference;

	using base::init;

	static void
	manage(any_storage& d, const any_storage& s, any_ops::op_code op)
	{
		switch(op)
		{
		case decrease:
			--get_reference(d);
			break;
		default:
			base::manage(d, s, op);
		}
	}
};

} // namespace any_ops;


//! \since build 459
//@{
#define YB_IterOp1(_n, _t, _it, _e) \
	template<typename _type, typename _tDifference, typename _tPointer, \
		typename _tReference> \
	inline _t \
	_n(const _it<_type, _tDifference, _tPointer, _tReference>& i) \
	{ \
		return _e; \
	}

#define YB_IterOp2(_n, _t, _it, _e) \
	template<typename _type, typename _tDifference, typename _tPointer, \
		typename _tReference> \
	inline _t \
	_n(const _it<_type, _tDifference, _tPointer, _tReference>& x, \
		const _it<_type, _tDifference, _tPointer, _tReference>& y) \
	{ \
		return _e; \
	}

#define YB_IterOpPost(_op, _it) \
	_it \
	operator _op(int) \
	{ \
		auto tmp = *this; \
	\
		_op *this; \
		return tmp; \
	}
//@}

//! \since build 400
//@{
/*!
\ingroup iterator_adaptors
\brief 动态泛型输入迭代器。
\warning 非虚析构。
*/
template<typename _type, typename _tDifference = ptrdiff_t,
	typename _tPointer = _type*, typename _tReference = _type&>
class any_input_iterator : public std::iterator<std::input_iterator_tag, _type,
	_tDifference, _tPointer, _tReference>, protected any
{
public:
	using pointer = _tPointer;
	using reference = _tReference;

	//! \since build 357
	any_input_iterator() = default;
	/*!
	\brief 构造：使用现有迭代器。
	\since build 356
	*/
	template<typename _tIter>
	any_input_iterator(_tIter&& i)
		: any()
	{
		using param_obj_type = typename remove_rcv<_tIter>::type;
		using handler = any_ops::input_iterator_handler<param_obj_type>;

		static_assert(is_convertible<decltype(*std::declval<typename
			wrapped_traits<param_obj_type>::type&>()), reference>::value,
			"Wrong target iterator type found.");

		manager = handler::manage;
		handler::init(storage, yforward(i));
	}
	//! \since build 356
	any_input_iterator(const any_input_iterator&) = default;
#if YB_IMPL_MSCPP
	//! \since build 454 as workaround for Visual C++ 2013
	any_input_iterator(any_input_iterator&& i)
		: any(static_cast<any&&>(i))
	{}
#else
	//! \since build 356
	any_input_iterator(any_input_iterator&&) = default;
#endif

	//! \since build 454
	//@{
	any_input_iterator&
	operator=(const any_input_iterator&) = default;
	any_input_iterator&
#if YB_IMPL_MSCPP
	//! \since build 454 as workaround for Visual C++ 2013
	operator=(any_input_iterator&& i)
	{
		static_cast<any&>(*this) = static_cast<any&&>(i);
		return *this;
	}
#else
	operator=(any_input_iterator&&) = default;
#endif
	//@}

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

	//! \since build 400
	bool
	check_undereferenceable() const
	{
		if(manager)
		{
			any_ops::any_storage t;

			manager(t, storage, any_ops::check_undereferenceable);
			return t.access<bool>();
		}
		return true;
	}

	//! \since build 355
	//@{
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

	using any::type;
	//@}
};

//! \relates any_input_iterator
//@{
YB_IterOp2(operator==, bool, any_input_iterator, x.equals(y))

YB_IterOp2(operator!=, bool, any_input_iterator, !(x == y))

YB_IterOp1(is_undereferenceable, bool, any_input_iterator,
	i.check_undereferenceable())
//@}

using input_monomorphic_iterator
	= any_input_iterator<void_ref, ptrdiff_t, void*, void_ref>;
//@}


/*!
\ingroup iterator_adaptors
\brief 动态泛型前向迭代器。
*/
template<typename _type, typename _tDifference = ptrdiff_t,
	typename _tPointer = _type*, typename _tReference = _type&>
class any_forward_iterator
	: public any_input_iterator<_type, _tDifference, _tPointer, _tReference>
{
public:
	using iterator_category = std::forward_iterator_tag;
	using pointer = _tPointer;
	using reference = _tReference;

	any_forward_iterator() = default;
	template<typename _tIter>
	any_forward_iterator(_tIter&& i)
		: any_input_iterator<_type, _tPointer, _tReference>(yforward(i))
	{}
	any_forward_iterator(const any_forward_iterator&) = default;
#if YB_IMPL_MSCPP
	//! \since build 454 as workaround for Visual C++ 2013
	any_forward_iterator(any_forward_iterator&& i)
		: any_input_iterator(static_cast<any_input_iterator&&>(i))
	{}
#else
	any_forward_iterator(any_forward_iterator&&) = default;
#endif

	//! \since build 454
	//@{
	any_forward_iterator&
	operator=(const any_forward_iterator&) = default;
	any_forward_iterator&
#if YB_IMPL_MSCPP
	//! \since build 454 as workaround for Visual C++ 2013
	operator=(any_forward_iterator&& i)
	{
		static_cast<any_input_iterator&>(*this)
			= static_cast<any_input_iterator&&>(i);
		return *this;
	}
#else
	operator=(any_forward_iterator&&) = default;
#endif
	//@}

	any_forward_iterator&
	operator++()
	{
		any_input_iterator<_type, _tPointer, _tReference>::operator++();
		return *this;
	}
	YB_IterOpPost(++, any_forward_iterator)
};

//! \relates any_forward_iterator
//@{
YB_IterOp2(operator==, bool, any_forward_iterator, x.equals(y))

YB_IterOp2(operator!=, bool, any_forward_iterator, !(x == y))

YB_IterOp1(is_undereferenceable, bool, any_forward_iterator,
	i.check_undereferenceable())
//@}

using forward_monomorphic_iterator
	= any_forward_iterator<void_ref, ptrdiff_t, void*, void_ref>;


/*!
\ingroup iterator_adaptors
\brief 动态泛型双向迭代器。
*/
template<typename _type, typename _tDifference = ptrdiff_t,
	typename _tPointer = _type*, typename _tReference = _type&>
class any_bidirectional_iterator
	: public any_forward_iterator<_type, _tDifference, _tPointer, _tReference>
{
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using pointer = _tPointer;
	using reference = _tReference;

	any_bidirectional_iterator() = default;
	template<typename _tIter>
	any_bidirectional_iterator(_tIter&& i)
		: any_input_iterator<_type, _tPointer, _tReference>(yforward(i))
	{}
	any_bidirectional_iterator(const any_bidirectional_iterator&) = default;
#if YB_IMPL_MSCPP
	//! \since build 454 as workaround for Visual C++ 2013
	any_bidirectional_iterator(any_bidirectional_iterator&& i)
		: any_forward_iterator(static_cast<any_forward_iterator&&>(i))
	{}
#else
	any_bidirectional_iterator(any_bidirectional_iterator&&) = default;
#endif

	//! \since build 454
	//@{
	any_bidirectional_iterator&
	operator=(const any_bidirectional_iterator&) = default;
	any_bidirectional_iterator&
#if YB_IMPL_MSCPP
	//! \since build 454 as workaround for Visual C++ 2013
	operator=(any_bidirectional_iterator&& i)
	{
		static_cast<any_forward_iterator&>(*this)
			= static_cast<any_forward_iterator&&>(i);
		return *this;
	}
#else
	operator=(any_bidirectional_iterator&&) = default;
#endif
	//@}

	any_bidirectional_iterator&
	operator++()
	{
		any_forward_iterator<_type, _tPointer, _tReference>::operator++();
		return *this;
	}
	YB_IterOpPost(++, any_bidirectional_iterator)

	any_bidirectional_iterator&
	operator--()
	{
		yassume(this->manager);
		this->manager(this->storage, this->storage, any_ops::decrease);
		return *this;
	}
	YB_IterOpPost(--, any_bidirectional_iterator)
};

//! \relates any_bidirectional_iterator
//@{
YB_IterOp2(operator==, bool, any_bidirectional_iterator, x.equals(y))

YB_IterOp2(operator!=, bool, any_bidirectional_iterator, !(x == y))

YB_IterOp1(is_undereferenceable, bool, any_bidirectional_iterator,
	i.check_undereferenceable())
//@}

using bidirectional_monomorphic_iterator
	= any_bidirectional_iterator<void_ref, ptrdiff_t, void*, void_ref>;


#undef YB_IterOp1
#undef YB_IterOp2
#undef YB_IterOpPost

//@}

} // namespace ystdex;

#endif

