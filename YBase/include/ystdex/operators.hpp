/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file operators.hpp
\ingroup YStandardEx
\brief 重载操作符。
\version r1720
\author FrankHB <frankhb1989@gmail.com>
\since build 260
\par 创建时间:
	2011-11-13 14:58:05 +0800
\par 修改时间:
	2015-02-10 18:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Operators
\see http://www.boost.org/doc/libs/1_54_0/boost/operators.hpp 。
\see https://github.com/d-frey/operators/tree/master/include/df/operators 。

用法同 Boost.Operators ，但迭代器相关部分参数有所删减。
引入部分 df.operators 特性。
*/


#ifndef YB_INC_ystdex_operators_hpp_
#define YB_INC_ystdex_operators_hpp_ 1

#include "../ydef.h"
#include <memory> // for std::addressof;

namespace ystdex
{

#define YB_Impl_Operators_friend(_op, _tRet, _spec, _expr, ...) \
	friend yconstfn _tRet \
	operator _op (__VA_ARGS__) ynoexcept(_spec) \
	{ \
		return (_expr); \
	}
#define YB_Impl_Operators_friend_s(_op, _tRet, _expr, ...) \
	YB_Impl_Operators_friend(_op, _tRet, noexcept(_expr), _expr, __VA_ARGS__)
#define YB_Impl_Operators_friend_lr(_op, _type, _type2) \
	YB_Impl_Operators_friend_s(_op, _type, x _op##= y, _type x, \
		const _type2& y) \
	YB_Impl_Operators_friend_s(_op, _type, x _op##= std::move(y), _type x, \
		_type2&& y)
#define YB_Impl_Operators_TmplHead2(_name) \
	template<class _type, typename _type2, class _tBase = empty_base<_type>> \
	struct _name
#define YB_Impl_Operators_TmplHead1(_name) \
	template<class _type, class _tBase = empty_base<_type>> \
	struct _name


namespace details
{

#define YB_Impl_Operators_Compare2(_op, _expr, _param_type, _param_type2) \
	YB_Impl_Operators_friend_s(_op, bool, _expr, const _param_type& x, \
		const _param_type2& y)
#define YB_Impl_Operators_Compare1(_op, _expr, _param_type) \
	YB_Impl_Operators_friend_s(_op, bool, _expr, const _param_type& x, \
		const _param_type& y)


YB_Impl_Operators_TmplHead2(less_than_comparable2) : _tBase
{
	YB_Impl_Operators_Compare2(<=, !bool(x > y), _type, _type2)
	YB_Impl_Operators_Compare2(>=, !bool(x < y), _type, _type2)
	YB_Impl_Operators_Compare2(>, y < x, _type2, _type)
	YB_Impl_Operators_Compare2(<, y > x, _type2, _type)
	YB_Impl_Operators_Compare2(<=, !bool(y < x), _type2, _type)
	YB_Impl_Operators_Compare2(>=, !bool(y > x), _type2, _type)
};

YB_Impl_Operators_TmplHead1(less_than_comparable1) : _tBase
{
	YB_Impl_Operators_Compare1(>, y < x, _type)
	YB_Impl_Operators_Compare1(<=, !bool(y < x), _type)
	YB_Impl_Operators_Compare1(>=, !bool(x < y), _type)
};


YB_Impl_Operators_TmplHead2(equality_comparable2) : _tBase
{
	YB_Impl_Operators_Compare2(==, x == y, _type2, _type)
	YB_Impl_Operators_Compare2(!=, !bool(x == y), _type2, _type)
	YB_Impl_Operators_Compare2(!=, !bool(y == x), _type, _type2)
};

YB_Impl_Operators_TmplHead1(equality_comparable1) : _tBase
{
	YB_Impl_Operators_Compare1(!=, !bool(x == y), _type)
};


YB_Impl_Operators_TmplHead2(equivalent2) : _tBase
{
	YB_Impl_Operators_Compare2(!=, !bool(x < y) && !bool(x > y), _type, _type2)
};

YB_Impl_Operators_TmplHead1(equivalent1) : _tBase
{
	YB_Impl_Operators_Compare1(!=, !bool(x < y) && !bool(y < x), _type)
};


YB_Impl_Operators_TmplHead2(partially_ordered2) : _tBase
{
	YB_Impl_Operators_Compare2(<=, bool(x < y) || bool(x == y), _type, _type2)
	YB_Impl_Operators_Compare2(>=, bool(x > y) || bool(x == y), _type, _type2)
	YB_Impl_Operators_Compare2(>, y < x, _type2, _type)
	YB_Impl_Operators_Compare2(<, y > x, _type2, _type)
	YB_Impl_Operators_Compare2(<=, bool(y > x) || bool(y == x), _type2, _type)
	YB_Impl_Operators_Compare2(>=, bool(y < x) || bool(y == x), _type2, _type)
};

YB_Impl_Operators_TmplHead1(partially_ordered1) : _tBase
{
	YB_Impl_Operators_Compare1(>, y < x, _type)
	YB_Impl_Operators_Compare1(<=, bool(x < y) || bool(x == y), _type)
	YB_Impl_Operators_Compare1(>=, bool(y < x) || bool(x == y), _type)
};

#undef YB_Impl_Operators_Compare2
#undef YB_Impl_Operators_Compare1


#define YB_Impl_Operators_Commutative(_name, _op) \
	YB_Impl_Operators_TmplHead2(_name##2) : _tBase \
	{ \
		YB_Impl_Operators_friend_lr(_op, _type, _type2) \
		YB_Impl_Operators_friend_s(_op, _type, y _op##= x, const _type2& x, \
			_type y) \
		YB_Impl_Operators_friend_s(_op, _type, y _op##= std::move(x), \
			_type2&& x, _type y) \
	}; \
	YB_Impl_Operators_TmplHead1(_name##1) : _tBase \
	{ \
		YB_Impl_Operators_friend_lr(_op, _type, _type) \
	};

#define YB_Impl_Operators_NonCommutative(_name, _op) \
	YB_Impl_Operators_TmplHead2(_name##2) : _tBase \
	{ \
		YB_Impl_Operators_friend_lr(_op, _type, _type2) \
	}; \
	YB_Impl_Operators_TmplHead2(_name##2##_##left) : _tBase \
	{ \
		YB_Impl_Operators_friend_s(_op, _type, _type(x) _op##= y, \
			const _type2& x, const _type& y) \
		YB_Impl_Operators_friend_s(_op, _type, _type(x) _op##= std::move(y), \
			const _type2& x, _type&& y) \
	}; \
	YB_Impl_Operators_TmplHead1(_name##1) : _tBase \
	{ \
		YB_Impl_Operators_friend_lr(_op, _type, _type) \
	};

YB_Impl_Operators_Commutative(multipliable, *)
YB_Impl_Operators_Commutative(addable, +)
YB_Impl_Operators_NonCommutative(subtractable, -)
YB_Impl_Operators_NonCommutative(dividable, /)
YB_Impl_Operators_NonCommutative(modable, %)
YB_Impl_Operators_Commutative(xorable, ^)
YB_Impl_Operators_Commutative(andable, &)
YB_Impl_Operators_Commutative(orable, |)

#undef YB_Impl_Operators_NonCommutative
#undef YB_Impl_Operators_Commutative


#define YB_Impl_Operators_Binary(_name, _op) \
	YB_Impl_Operators_TmplHead2(_name##2) : _tBase \
	{ \
		YB_Impl_Operators_friend_lr(_op, _type, _type2) \
	}; \
	YB_Impl_Operators_TmplHead1(_name##1) : _tBase \
	{ \
		YB_Impl_Operators_friend_lr(_op, _type, _type) \
	};

	YB_Impl_Operators_Binary(left_shiftable, <<)
	YB_Impl_Operators_Binary(right_shiftable, >>)

#undef YB_Impl_Operators_Binary

YB_Impl_Operators_TmplHead1(incrementable) : _tBase
{
	//! \since build 576
	friend _type
	operator++(_type& x, int) ynoexcept(noexcept(_type(x)) && noexcept(++x)
		&& noexcept(_type(std::declval<_type>())))
	{
		_type t(x);

		++x;
		return t;
	}
};

YB_Impl_Operators_TmplHead1(decrementable) : _tBase
{
	//! \since build 576
	friend _type
	operator--(_type& x, int) ynoexcept(noexcept(_type(x)) && noexcept(--x)
		&& noexcept(_type(std::declval<_type>())))
	{
		_type t(x);

		++x;
		return t;
	}
};

//! \since build 576
YB_Impl_Operators_TmplHead2(dereferenceable) : _tBase
{
	yconstfn _type2
	operator->() const ynoexcept(noexcept(*std::declval<const _type&>()))
	{
		return std::addressof(*static_cast<const _type&>(*this));
	}
};

//! \since build 576
template<class _type, typename _type2, typename _tRet,
	class _tBase = empty_base<_type>>
struct indexable : _tBase
{
	yconstfn _tRet
	operator[](_type2 n) const
		ynoexcept(noexcept(*(std::declval<const _type&>() + n)))
	{
		return *(static_cast<const _type&>(*this) + n);
	}
};


YB_Impl_Operators_TmplHead2(totally_ordered2)
	: less_than_comparable2<_type, _type2,
	equality_comparable2<_type, _type2, _tBase>>
{};

YB_Impl_Operators_TmplHead1(totally_ordered1)
	: less_than_comparable1<_type, equality_comparable1<_type, _tBase>>
{};


YB_Impl_Operators_TmplHead2(additive2)
	: addable2<_type, _type2, subtractable2<_type, _type2, _tBase>>
{};

YB_Impl_Operators_TmplHead1(additive1)
	: addable1<_type, subtractable1<_type, _tBase>>
{};


YB_Impl_Operators_TmplHead2(multiplicative2)
	: multipliable2<_type, _type2, dividable2<_type, _type2, _tBase>>
{};

YB_Impl_Operators_TmplHead1(multiplicative1)
	: multipliable1<_type, dividable1<_type, _tBase>>
{};


YB_Impl_Operators_TmplHead2(integer_multiplicative2)
	: multiplicative2<_type, _type2, modable2<_type, _type2, _tBase>>
{};

YB_Impl_Operators_TmplHead1(integer_multiplicative1)
	: multiplicative1<_type, modable1<_type, _tBase>>
{};


YB_Impl_Operators_TmplHead2(arithmetic2)
	: additive2<_type, _type2, multiplicative2<_type, _type2, _tBase>>
{};

YB_Impl_Operators_TmplHead1(arithmetic1)
	: additive1<_type, multiplicative1<_type, _tBase>>
{};


YB_Impl_Operators_TmplHead2(integer_arithmetic2) : additive2<_type, _type2,
	integer_multiplicative2<_type, _type2, _tBase>>
{};

YB_Impl_Operators_TmplHead1(integer_arithmetic1)
	: additive1<_type, integer_multiplicative1<_type, _tBase>>
{};


YB_Impl_Operators_TmplHead2(bitwise2) : xorable2<_type, _type2,
	andable2<_type, _type2, orable2<_type, _type2, _tBase>>>
{};

YB_Impl_Operators_TmplHead1(bitwise1)
	: xorable1<_type, andable1<_type, orable1<_type, _tBase>>>
{};


YB_Impl_Operators_TmplHead1(unit_steppable)
	: incrementable<_type, decrementable<_type, _tBase>>
{};


YB_Impl_Operators_TmplHead2(shiftable2) : left_shiftable2<_type, _type2,
	right_shiftable2<_type, _type2, _tBase>>
{};

YB_Impl_Operators_TmplHead1(shiftable1)
	: left_shiftable1<_type, right_shiftable1<_type, _tBase>>
{};


YB_Impl_Operators_TmplHead2(ring_operators2) : additive2<_type, _type2,
	subtractable2_left<_type, _type2, multipliable2<_type, _type2, _tBase>>>
{};

YB_Impl_Operators_TmplHead1(ring_operators1)
	: additive1<_type, multipliable1<_type, _tBase>>
{};


YB_Impl_Operators_TmplHead2(ordered_ring_operators2)
	: ring_operators2<_type, _type2, totally_ordered2<_type, _type2, _tBase>>
{};

YB_Impl_Operators_TmplHead1(ordered_ring_operators1)
	: ring_operators1<_type, totally_ordered1<_type, _tBase>>
{};


YB_Impl_Operators_TmplHead2(field_operators2) : ring_operators2<_type, _type2,
	dividable2<_type, _type2, dividable2_left<_type, _type2, _tBase>>>
{};

YB_Impl_Operators_TmplHead1(field_operators1)
	: ring_operators1<_type, dividable1<_type, _tBase>>
{};


YB_Impl_Operators_TmplHead2(ordered_field_operators2)
	: field_operators2<_type, _type2, totally_ordered2<_type, _type2, _tBase>>
{};

YB_Impl_Operators_TmplHead1(ordered_field_operators1)
	: field_operators1<_type, totally_ordered1<_type, _tBase>>
{};


YB_Impl_Operators_TmplHead2(euclidean_ring_operators2)
	: ring_operators2<_type, _type2, dividable2<_type, _type2,
	dividable2_left<_type, _type2, modable2<_type, _type2,
	modable2_left<_type, _type2, _tBase>>>>>
{};

YB_Impl_Operators_TmplHead1(euclidean_ring_operators1)
	: ring_operators1<_type, dividable1<_type, modable1<_type, _tBase>>>
{};


YB_Impl_Operators_TmplHead2(ordered_euclidean_ring_operators2)
	: totally_ordered2<_type, _type2,
	euclidean_ring_operators2<_type, _type2, _tBase>>
{};

YB_Impl_Operators_TmplHead1(ordered_euclidean_ring_operators1)
	: totally_ordered1<_type, euclidean_ring_operators1<_type, _tBase>>
{};


//! \since build 576
YB_Impl_Operators_TmplHead2(input_iteratable) : equality_comparable1<_type,
	incrementable<_type, dereferenceable<_type, _type2, _tBase>>>
{};


YB_Impl_Operators_TmplHead1(output_iteratable)
	: incrementable<_type, _tBase>
{};


//! \since build 576
//@{
YB_Impl_Operators_TmplHead2(forward_iteratable)
	: input_iteratable<_type, _type2, _tBase>
{};


YB_Impl_Operators_TmplHead2(bidirectional_iteratable)
	: forward_iteratable<_type, _type2, decrementable<_type, _tBase>>
{};


template<class _type, typename _tDiff, typename _tRef,
	class _tBase = empty_base<_type>>
struct random_access_iteratable
	: bidirectional_iteratable<_type, _tRef, less_than_comparable1<_type,
	additive2<_type, _tDiff, indexable<_type, _tDiff, _type, _tBase>>>>
{};
//@}

} // namespace details;


/*!
\brief 实现命名空间：作为 ADL 边界。
\since build 576
*/
namespace operators_impl
{

template<class>
struct is_chained_base : false_type
{};


# define YB_Impl_Operators_Chain2(_name) \
	using ystdex::details::_name; \
	template<class _type, typename _type2, class _tBase> \
	struct is_chained_base<_name<_type, _type2, _tBase>> : true_type \
	{};

# define YB_Impl_Operators_Chain1(_name) \
	using ystdex::details::_name; \
	template<class _type, class _tBase> \
	struct is_chained_base<_name<_type, _tBase>> : true_type \
	{};

#define YB_Impl_Operators_Chain(_name) \
	using ystdex::details::_name##2; \
	template<class _type, typename _type2 = _type, class \
		_tBase = empty_base<_type>, bool _b = is_chained_base<_type2>::value> \
	struct _name : _name##2<_type, _type2, _tBase> \
	{}; \
	\
	using ystdex::details::_name##1; \
	template<class _type, typename _type2, class _tBase> \
	struct _name<_type, _type2, _tBase, true> : _name##1<_type, _type2> \
	{}; \
	\
	template<class _type, class _tBase> \
	struct _name<_type, _type, _tBase, false> : _name##1<_type, _tBase> \
	{}; \
	\
	template<class _type, typename _type2, class _tBase, bool _b> \
	struct is_chained_base<_name<_type, _type2, _tBase, _b>> \
		: true_type \
	{}; \
	\
	YB_Impl_Operators_Chain2(_name##2) \
	YB_Impl_Operators_Chain1(_name##1)


YB_Impl_Operators_Chain(less_than_comparable)
YB_Impl_Operators_Chain(equality_comparable)
YB_Impl_Operators_Chain(multipliable)
YB_Impl_Operators_Chain(addable)
YB_Impl_Operators_Chain(subtractable)
YB_Impl_Operators_Chain2(subtractable2_left)
YB_Impl_Operators_Chain(dividable)
YB_Impl_Operators_Chain2(dividable2_left)
YB_Impl_Operators_Chain(modable)
YB_Impl_Operators_Chain2(modable2_left)
YB_Impl_Operators_Chain(xorable)
YB_Impl_Operators_Chain(andable)
YB_Impl_Operators_Chain(orable)

YB_Impl_Operators_Chain1(incrementable)
YB_Impl_Operators_Chain1(decrementable)

YB_Impl_Operators_Chain1(dereferenceable)
YB_Impl_Operators_Chain2(indexable)

YB_Impl_Operators_Chain(left_shiftable)
YB_Impl_Operators_Chain(right_shiftable)
YB_Impl_Operators_Chain(equivalent)
YB_Impl_Operators_Chain(partially_ordered)

YB_Impl_Operators_Chain(totally_ordered)
YB_Impl_Operators_Chain(additive)
YB_Impl_Operators_Chain(multiplicative)
YB_Impl_Operators_Chain(integer_multiplicative)
YB_Impl_Operators_Chain(arithmetic)
YB_Impl_Operators_Chain(integer_arithmetic)
YB_Impl_Operators_Chain(bitwise)
YB_Impl_Operators_Chain1(unit_steppable)
YB_Impl_Operators_Chain(shiftable)
YB_Impl_Operators_Chain(ring_operators)
YB_Impl_Operators_Chain(ordered_ring_operators)
YB_Impl_Operators_Chain(field_operators)
YB_Impl_Operators_Chain(ordered_field_operators)
YB_Impl_Operators_Chain(euclidean_ring_operators)
YB_Impl_Operators_Chain(ordered_euclidean_ring_operators)
YB_Impl_Operators_Chain1(input_iteratable)
YB_Impl_Operators_Chain1(output_iteratable)
YB_Impl_Operators_Chain1(forward_iteratable)
YB_Impl_Operators_Chain1(bidirectional_iteratable)
YB_Impl_Operators_Chain2(random_access_iteratable)

#undef YB_Impl_Operators_Chain2
#undef YB_Impl_Operators_Chain1
#undef YB_Impl_Operators_Chain

//! \since build 439
//@{
template<class _type, typename _type2>
struct operators2 : public totally_ordered2<_type, _type2,
	integer_arithmetic2<_type, _type2, bitwise2<_type, _type2>>>
{};


template<class _type, typename _type2 = _type>
struct operators : public operators2<_type, _type2>
{};

template<class _type>
struct operators<_type, _type> : totally_ordered<_type,
	integer_arithmetic<_type, bitwise<_type, unit_steppable<_type>>>>
{};
//@}

#undef YB_Impl_Operators_TmplHead1
#undef YB_Impl_Operators_TmplHead2
#undef YB_Impl_Operators_friend_lr
#undef YB_Impl_Operators_friend_s
#undef YB_Impl_Operators_friend

} // namespace operators_impl;

//! \since build 576
using namespace operators_impl;

} // namespace ystdex;

#endif

