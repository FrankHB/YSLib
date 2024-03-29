﻿/*
	© 2012-2016, 2018-2019, 2021-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any_iterator.hpp
\ingroup YStandardEx
\brief 动态泛型迭代器。
\version r1549
\author FrankHB <frankhb1989@gmail.com>
\since build 355
\par 创建时间:
	2012-11-08 14:28:42 +0800
\par 修改时间:
	2022-02-14 07:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::AnyIterator
*/


#ifndef YB_INC_ystdex_any_iterator_hpp_
#define YB_INC_ystdex_any_iterator_hpp_ 1

#include "any.h" // for "any.h", any_ops, cond_t, wrap_handler, _t, decay_t,
//	ptrdiff_t, any, exclude_self_t, any_ops::with_handler_t, true_,
//	is_convertible, indirect_t, unwrap_ref_decay_t, remove_reference_t,
//	is_bitwise_swappable;
#include "iterator_op.hpp" // for is_undereferenceable, input_iteratable,
//	forward_iteratable, bidirectional_iteratable;

namespace ystdex
{

namespace any_ops
{

//! \since build 355
//@{
enum iterator_op : op_code
{
	/*!
	\note 要求已构造对象类型 bool 。
	\since build 400
	*/
	check_undereferenceable = end_base_op,
	//! \note 要求已构造对象类型 void_ref 。
	dereference,
	//! \note 要求已构造目标对象。
	increase,
	end_iterator_op
};


enum input_iterator_op : op_code
{
	//! \note 要求已构造有效的 any_storage* ；转换为 bool 类型的值为操作结果。
	equals = end_iterator_op,
	end_input_iterator_op,
	end_output_iterator_op = end_input_iterator_op,
	end_forward_iterator_op = end_input_iterator_op
};


enum bidirectional_iteartor_op : op_code
{
	//! \note 要求已构造目标对象。
	decrease = end_forward_iterator_op,
	end_bidirectional_iterator_op
};


enum random_access_iteartor_op : op_code
{
	//! \note 要求已构造目标对象。
	advance = end_forward_iterator_op,
	distance,
	less_compare,
	end_random_access_iteartor_op
};
//@}


//! \since build 927
//@{
//! \note 迭代器引用可能是引用类型，函数类型或可能有 cv 限定符的对象类型。
template<typename _type, typename _tReference, class _bStoredLocally
	= bool_<is_in_place_storable<_type>::value>>
class iterator_handler : public wrap_handler<_type, _bStoredLocally>::type
{
public:
	//! \since build 355
	//@{
	using base = _t<wrap_handler<_type>>;
	using value_type = typename base::value_type;

	using base::get_reference;

	using base::init;
	//@}

	//! \since build 692
	static void
	manage(any_storage& d, any_storage& s, any_ops::op_code op)
	{
		switch(op)
		{
		case check_undereferenceable:
			d.access<bool>() = is_undereferenceable(get_reference(s));
			break;
		case dereference:
			{
				// XXX: The type %_tReference can be cv-qualified.
				using val_t = cond_t<is_reference<_tReference>,
					lref<remove_reference_t<_tReference>>,
					remove_cv_t<_tReference>>;
				auto& p(d.access<any*>());

				yassume(p);
				// NOTE: This implies %val_t shall be a decayed type.
				*p = any(any_ops::with_handler_t<_t<any_ops::wrap_handler<
					val_t>>>(), val_t(*get_reference(s)));
			}
			break;
		case increase:
			++get_reference(d);
			break;
		default:
			base::manage(d, s, op);
		}
	}
};


template<typename _type, typename _tReference, class _bStoredLocally
	= bool_<is_in_place_storable<_type>::value>>
class input_iterator_handler
	: public iterator_handler<_type, _tReference, _bStoredLocally>
{
public:
	//! \since build 355
	//@{
	using base = iterator_handler<_type, _tReference>;
	using value_type = typename base::value_type;

private:
	using iteratore_value_type
		= typename std::iterator_traits<value_type>::value_type;
	// XXX: The requirement on %reference is relaxed intentionally. See also
	//	https://stackoverflow.com/questions/53800391/iso-c-input-iterator-with-an-abstract-class-as-the-value-type.
	//! \since build 848
	static_assert(or_<and_<is_reference<_tReference>, or_<is_convertible<
		_tReference, iteratore_value_type&>, is_convertible<_tReference,
		iteratore_value_type&&>>>, is_convertible<_tReference,
		iteratore_value_type>>(), "Invalid reference type not meeting relaxed"
		" InputIterator requirements found.");

public:
	using base::get_reference;

	using base::init;
	//@}

	//! \since build 692
	static void
	manage(any_storage& d, any_storage& s, any_ops::op_code op)
	{
		switch(op)
		{
		case equals:
			if(get_reference(*d.access<any_storage*>()) != get_reference(s))
				d = static_cast<any_storage*>(nullptr);
			break;
		default:
			base::manage(d, s, op);
		}
	}
};


template<typename _type, typename _tReference, class _bStoredLocally
	= bool_<is_in_place_storable<_type>::value>>
class forward_iterator_handler
	: public input_iterator_handler<_type, _tReference>
{
public:
	//! \since build 400
	//@{
	using base = input_iterator_handler<_type, _tReference>;
	using value_type = typename base::value_type;

	using base::get_reference;

	using base::init;

	using base::manage;
	//@}
};


template<typename _type, typename _tReference, class _bStoredLocally
	= bool_<is_in_place_storable<_type>::value>>
class bidirectional_iterator_handler
	: public forward_iterator_handler<_type, _tReference>
{
public:
	//! \since build 400
	//@{
	using base = forward_iterator_handler<_type, _tReference>;
	using value_type = typename base::value_type;

	using base::get_reference;

	using base::init;
	//@}

	//! \since build 692
	static void
	manage(any_storage& d, any_storage& s, any_ops::op_code op)
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
//@}

} // namespace any_ops;


//! \since build 400
//@{
/*!
\ingroup iterator_adaptors
\brief 动态泛型输入迭代器。
\warning 非虚析构。
*/
template<typename _type, typename _tDifference = ptrdiff_t,
	typename _tPointer = _type*, typename _tReference = _type&>
class any_input_iterator : protected any,
	public input_iteratable<any_input_iterator<_type, _tDifference, _tPointer,
	_tReference>, _tReference>
{
protected:
	//! \since build 686
	using base = any;

public:
	//! \since build 676
	//@{
	using iterator_category = std::input_iterator_tag;
	using value_type = _type;
	using difference_type = _tDifference;
	//@}
	using pointer = _tPointer;
	using reference = _tReference;

	//! \since build 357
	any_input_iterator() = default;
	//! \brief 构造：使用现有迭代器。
	//@{
	//! \since build 675
	template<typename _tIter,
		yimpl(typename = exclude_self_t<any_input_iterator, _tIter>)>
	any_input_iterator(_tIter&& i)
		: any_input_iterator(any_ops::with_handler_t<
		any_ops::input_iterator_handler<decay_t<_tIter>, reference>>(),
		yforward(i))
	{}
	//! \since build 927
	template<typename _tIter,
		yimpl(typename = exclude_self_t<any_input_iterator, _tIter>)>
	any_input_iterator(trivial_swap_t, _tIter&& i)
		: any_input_iterator(any_ops::with_handler_t<
		any_ops::input_iterator_handler<decay_t<_tIter>, reference, true_>>(),
		yforward(i))
	{}
	//@}

protected:
	//! \since build 686
	template<typename _tIter, typename _tHandler>
	any_input_iterator(any_ops::with_handler_t<_tHandler> t, _tIter&& i)
		: base(t, yforward(i))
	{
		static_assert(is_convertible<indirect_t<unwrap_ref_decay_t<_tIter>>,
			reference>(), "Wrong target iterator type found.");
	}

public:
	//! \since build 356
	any_input_iterator(const any_input_iterator&) = default;
	//! \since build 356
	any_input_iterator(any_input_iterator&&) = default;

	//! \since build 454
	//@{
	any_input_iterator&
	operator=(const any_input_iterator&) = default;
	any_input_iterator&
	operator=(any_input_iterator&&) = default;
	//@}

	YB_ATTR_nodiscard reference
	operator*() const
	{
		// TODO: More graceful implementation for unused value?
		any res;
		any_ops::any_storage t;
		const auto gd(t.pun<any*>(&res));

		call(t, any_ops::dereference);
		return reference(
			*ystdex::unchecked_any_cast<remove_reference_t<reference>>(&res));
	}

	YB_ATTR_nodiscard YB_PURE pointer
	operator->() const
	{
		return &**this;
	}

	any_input_iterator&
	operator++()
	{
		call(get_storage(), any_ops::increase);
		return *this;
	}

	//! \since build 676
	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const any_input_iterator& x, const any_input_iterator& y)
	{
		if(x.has_value() || y.has_value())
		{
			yconstraint(x.type() == y.type());
			return y.template unchecked_access<any_ops::any_storage*>(
				any_ops::equals, &x.get_storage());
		}
		return true;
	}

	//! \since build 615
	YB_ATTR_nodiscard YB_PURE any&
	get() ynothrow
	{
		return *this;
	}
	//! \since build 615
	YB_ATTR_nodiscard YB_PURE const any&
	get() const ynothrow
	{
		return *this;
	}

	//! \since build 854
	using any::get_object_ptr;

	/*!
	\ingroup is_undereferenceable
	\since build 676
	*/
	YB_ATTR_nodiscard YB_PURE friend bool
	is_undereferenceable(const any_input_iterator& i)
	{
		return i.has_value() ? i.template unchecked_access<bool>(
			any_ops::check_undereferenceable) : true;
	}

	//! \since build 854
	using any::try_get_object_ptr;

	//! \since build 355
	using any::type;
};

/*!
\relates any_input_iterator
\since build 926
*/
template<typename _type, typename _tDifference, typename _tPointer,
	typename _tReference>
struct is_bitwise_swappable<any_input_iterator<_type, _tDifference, _tPointer,
	_tReference>> : true_
{};


using input_monomorphic_iterator
	= any_input_iterator<void_ref, ptrdiff_t, void*, void_ref_any>;
//@}


/*!
\ingroup iterator_adaptors
\brief 动态泛型前向迭代器。
*/
template<typename _type, typename _tDifference = ptrdiff_t,
	typename _tPointer = _type*, typename _tReference = _type&>
class any_forward_iterator : public any_input_iterator<_type, _tDifference,
	_tPointer, _tReference>, public forward_iteratable<any_forward_iterator<
	_type, _tDifference, _tPointer, _tReference>, _tReference>
{
protected:
	//! \since build 686
	using base
		= any_input_iterator<_type, _tDifference, _tPointer, _tReference>;

public:
	using iterator_category = std::forward_iterator_tag;
	using pointer = _tPointer;
	using reference = _tReference;

	any_forward_iterator() = default;
	//! \brief 构造：使用现有迭代器。
	//@{
	//! \since build 686
	template<typename _tIter,
		yimpl(typename = exclude_self_t<any_forward_iterator, _tIter>)>
	any_forward_iterator(_tIter&& i)
		: any_forward_iterator(any_ops::with_handler_t<
		any_ops::forward_iterator_handler<decay_t<_tIter>, reference>>(),
		yforward(i))
	{}
	//! \since build 927
	template<typename _tIter,
		yimpl(typename = exclude_self_t<any_forward_iterator, _tIter>)>
	any_forward_iterator(trivial_swap_t, _tIter&& i)
		: any_forward_iterator(any_ops::with_handler_t<
		any_ops::forward_iterator_handler<decay_t<_tIter>, reference, true_>>(),
		yforward(i))
	{}
	//@}

protected:
	//! \since build 686
	template<typename _tIter, typename _tHandler>
	any_forward_iterator(any_ops::with_handler_t<_tHandler> t, _tIter&& i)
		: base(t, yforward(i))
	{}

public:
	any_forward_iterator(const any_forward_iterator&) = default;
	any_forward_iterator(any_forward_iterator&&) = default;

	//! \since build 454
	//@{
	any_forward_iterator&
	operator=(const any_forward_iterator&) = default;
	any_forward_iterator&
	operator=(any_forward_iterator&&) = default;
	//@}

	any_forward_iterator&
	operator++()
	{
		base::operator++();
		return *this;
	}

	//! \since build 676
	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const any_forward_iterator& x, const any_forward_iterator& y)
	{
		return static_cast<const base&>(x) == static_cast<const base&>(y);
	}
};

/*!
\relates any_forward_iterator
\since build 926
*/
template<typename _type, typename _tDifference, typename _tPointer,
	typename _tReference>
struct is_bitwise_swappable<any_forward_iterator<_type, _tDifference, _tPointer,
	_tReference>> : true_
{};


using forward_monomorphic_iterator
	= any_forward_iterator<void_ref, ptrdiff_t, void*, void_ref>;


/*!
\ingroup iterator_adaptors
\brief 动态泛型双向迭代器。
*/
template<typename _type, typename _tDifference = ptrdiff_t,
	typename _tPointer = _type*, typename _tReference = _type&>
class any_bidirectional_iterator : public any_forward_iterator<_type,
	_tDifference, _tPointer, _tReference>, public bidirectional_iteratable<
	any_bidirectional_iterator<_type, _tDifference, _tPointer, _tReference>,
	_tReference>
{
protected:
	//! \since build 686
	using base
		= any_forward_iterator<_type, _tDifference, _tPointer, _tReference>;

public:
	using iterator_category = std::bidirectional_iterator_tag;
	using pointer = _tPointer;
	using reference = _tReference;

	any_bidirectional_iterator() = default;
	//! \brief 构造：使用现有迭代器。
	//@{
	//! \since build 717
	template<typename _tIter,
		yimpl(typename = exclude_self_t<any_bidirectional_iterator, _tIter>)>
	any_bidirectional_iterator(_tIter&& i)
		: any_bidirectional_iterator(any_ops::with_handler_t<
		any_ops::bidirectional_iterator_handler<decay_t<_tIter>, reference>>(),
		yforward(i))
	{}
	//! \since build 927
	template<typename _tIter,
		yimpl(typename = exclude_self_t<any_bidirectional_iterator, _tIter>)>
	any_bidirectional_iterator(trivial_swap_t, _tIter&& i)
		: any_bidirectional_iterator(any_ops::with_handler_t<
		any_ops::bidirectional_iterator_handler<decay_t<_tIter>, reference,
		true_>>(), yforward(i))
	{}
	//@}

protected:
	//! \since build 686
	template<typename _tIter, typename _tHandler>
	any_bidirectional_iterator(any_ops::with_handler_t<_tHandler> t, _tIter&& i)
		: base(t, yforward(i))
	{}

public:
	any_bidirectional_iterator(const any_bidirectional_iterator&) = default;
	any_bidirectional_iterator(any_bidirectional_iterator&&) = default;

	//! \since build 454
	//@{
	any_bidirectional_iterator&
	operator=(const any_bidirectional_iterator&) = default;
	any_bidirectional_iterator&
	operator=(any_bidirectional_iterator&&) = default;
	//@}

	any_bidirectional_iterator&
	operator++()
	{
		base::operator++();
		return *this;
	}

	any_bidirectional_iterator&
	operator--()
	{
		this->call(this->get_storage(), any_ops::decrease);
		return *this;
	}

	//! \since build 676
	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const any_bidirectional_iterator& x,
		const any_bidirectional_iterator& y)
	{
		return static_cast<const base&>(x) == static_cast<const base&>(y);
	}
};

/*!
\relates any_bidirectional_iterator
\since build 926
*/
template<typename _type, typename _tDifference, typename _tPointer,
	typename _tReference>
struct is_bitwise_swappable<any_bidirectional_iterator<_type, _tDifference,
	_tPointer, _tReference>> : true_
{};


using bidirectional_monomorphic_iterator
	= any_bidirectional_iterator<void_ref, ptrdiff_t, void*, void_ref>;
//@}

} // namespace ystdex;

#endif

