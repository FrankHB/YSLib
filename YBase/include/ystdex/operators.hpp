/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file operators.hpp
\ingroup YStandardEx
\brief 重载运算符。
\version r2096;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-11-13 14:58:05 +0800;
\par 修改时间:
	2011-11-14 12:25 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YStandardEx::Operators;
*/


#ifndef YCL_INC_YSTDEX_OPERATORS_HPP_
#define YCL_INC_YSTDEX_OPERATORS_HPP_

#include "../ydef.h"

namespace ystdex
{
	template <typename...>
	struct empty_base
	{};


#define YCL_OP_FRIEND(_op, _tRet, _expr, ...) \
	friend yconstexprf _tRet operator _op (__VA_ARGS__) {return (_expr);}
#define YCL_OP_TEMPLATE_HEADER2(_name) \
	template<class _type, class _type2, class _tBase = empty_base<_type>> \
	struct _name
#define YCL_OP_TEMPLATE_HEADER1(_name) \
	template<class _type, class _tBase = empty_base<_type>> \
	struct _name


	namespace _impl
	{

	#define YCL_OP_COMPARE2(_op, _expr, _param_type, _param_type2) \
		YCL_OP_FRIEND(_op, bool, _expr, const _param_type& x, \
		const _param_type2& y)
	#define YCL_OP_COMPARE1(_op, _expr, _param_type) \
		YCL_OP_FRIEND(_op, bool, _expr, const _param_type& x, \
		const _param_type& y)


		YCL_OP_TEMPLATE_HEADER2(less_than_comparable2) : _tBase
		{
			YCL_OP_COMPARE2(<=, !bool(x > y), _type, _type2)
			YCL_OP_COMPARE2(>=, !bool(x < y), _type, _type2)
			YCL_OP_COMPARE2(>, y < x, _type2, _type)
			YCL_OP_COMPARE2(<, y > x, _type2, _type)
			YCL_OP_COMPARE2(<=, !bool(y < x), _type2, _type)
			YCL_OP_COMPARE2(>=, !bool(y > x), _type2, _type)
		};

		YCL_OP_TEMPLATE_HEADER1(less_than_comparable1) : _tBase
		{
			YCL_OP_COMPARE1(>, y < x, _type)
			YCL_OP_COMPARE1(<=, !bool(y < x), _type)
			YCL_OP_COMPARE1(>=, !bool(x < y), _type)
		};


		YCL_OP_TEMPLATE_HEADER2(equality_comparable2) : _tBase
		{
			YCL_OP_COMPARE2(==, x == y,  _type2, _type)
			YCL_OP_COMPARE2(!=, !bool(x == y), _type2, _type)
			YCL_OP_COMPARE2(!=, !bool(y == x), _type, _type2)
		};

		YCL_OP_TEMPLATE_HEADER1(equality_comparable1) : _tBase
		{
			YCL_OP_COMPARE1(!=, !bool(x == y), _type)
		};


		YCL_OP_TEMPLATE_HEADER2(equivalent2) : _tBase
		{
			YCL_OP_COMPARE2(!=, !bool(x < y) && !bool(x > y), _type, _type2)
		};

		YCL_OP_TEMPLATE_HEADER1(equivalent1) : _tBase
		{
			YCL_OP_COMPARE1(!=, !bool(x < y) && !bool(y < x), _type)
		};


		YCL_OP_TEMPLATE_HEADER2(partially_ordered2) : _tBase
		{
			YCL_OP_COMPARE2(<=, bool(x < y) || bool(x == y), _type, _type2)
			YCL_OP_COMPARE2(>=, bool(x > y) || bool(x == y), _type, _type2)
			YCL_OP_COMPARE2(>, y < x, _type2, _type)
			YCL_OP_COMPARE2(<, y > x, _type2, _type)
			YCL_OP_COMPARE2(<=, bool(y > x) || bool(y == x), _type2, _type)
			YCL_OP_COMPARE2(>=, bool(y < x) || bool(y == x), _type2, _type)
		};

		YCL_OP_TEMPLATE_HEADER1(partially_ordered1) : _tBase
		{
			YCL_OP_COMPARE1(>, y < x, _type)
			YCL_OP_COMPARE1(<=, bool(x < y) || bool(x == y), _type)
			YCL_OP_COMPARE1(>=, bool(y < x) || bool(x == y), _type)
		};

	#undef YCL_OP_COMPARE2
	#undef YCL_OP_COMPARE1


	#define YCL_OP_COMMUTATIVE(_name, _op) \
		YCL_OP_TEMPLATE_HEADER2(_name##2) : _tBase \
		{ \
			YCL_OP_FRIEND(_op, _type, x _op##= y, _type x, const _type2& y) \
			YCL_OP_FRIEND(_op, _type, y _op##= x, const _type2& y, _type x) \
		}; \
		YCL_OP_TEMPLATE_HEADER1(_name##1) : _tBase \
		{ \
			YCL_OP_FRIEND(_op, _type, x _op##= y, _type x, const _type& y) \
		};

	#define YCL_OP_NON_COMMUTATIVE(_name, _op) \
		YCL_OP_TEMPLATE_HEADER2(_name##2) : _tBase \
		{ \
			YCL_OP_FRIEND(_op, _type, x _op##= y, _type x, const _type2& y) \
		}; \
		YCL_OP_TEMPLATE_HEADER2(_name##2##_##left) : _tBase \
		{ \
			YCL_OP_FRIEND(_op, _type, _type(x) _op##= y, const _type2& x, \
				const _type& y) \
		}; \
		YCL_OP_TEMPLATE_HEADER1(_name##1) : _tBase \
		{ \
			YCL_OP_FRIEND(_op, _type, x _op##= y, _type x, const _type& y) \
		};

		YCL_OP_COMMUTATIVE(multipliable, *)
		YCL_OP_COMMUTATIVE(addable, +)
		YCL_OP_NON_COMMUTATIVE(subtractable, -)
		YCL_OP_NON_COMMUTATIVE(dividable, /)
		YCL_OP_NON_COMMUTATIVE(modable, %)
		YCL_OP_COMMUTATIVE(xorable, ^)
		YCL_OP_COMMUTATIVE(andable, &)
		YCL_OP_COMMUTATIVE(orable, |)

	#undef YCL_OP_NON_COMMUTATIVE
	#undef YCL_OP_COMMUTATIVE


	#define YCL_OP_BINARY(_name, _op) \
		YCL_OP_TEMPLATE_HEADER2(_name##2) : _tBase \
		{ \
			YCL_OP_FRIEND(_op, _type, x _op##= y, _type x, const _type2& y) \
		}; \
		YCL_OP_TEMPLATE_HEADER1(_name##1) : _tBase \
		{ \
			YCL_OP_FRIEND(_op, _type, x _op##= y, _type x, const _type& y) \
		};

		YCL_OP_BINARY(left_shiftable, <<)
		YCL_OP_BINARY(right_shiftable, >>)

	#undef YCL_OP_BINARY

		YCL_OP_TEMPLATE_HEADER1(incrementable) : _tBase
		{
			friend _type
			operator++(_type& x, int)
			{
				_type t(x);

				++x;
				return t;
			}
		};

		YCL_OP_TEMPLATE_HEADER1(decrementable) : _tBase
		{
			friend _type
			operator--(_type& x, int)
			{
				_type t(x);

				++x;
				return t;
			}
		};

		YCL_OP_TEMPLATE_HEADER1(dereferenceable) : _tBase
		{
			auto
			operator->() const -> decltype(&*std::declval<const _type&>())
			{ 
				return &*static_cast<const _type&>(*this); 
			}
		};

		YCL_OP_TEMPLATE_HEADER2(indexable) : _tBase
		{
			auto
			operator[](_type2 n) const
				-> decltype(*(std::declval<const _type&>() + n))
			{
				return *(static_cast<const _type&>(*this) + n);
			}
		};


		YCL_OP_TEMPLATE_HEADER2(totally_ordered2)
			: less_than_comparable2<_type, _type2,
				equality_comparable2<_type, _type2, _tBase>
			>
		{};

		YCL_OP_TEMPLATE_HEADER1(totally_ordered1)
			: less_than_comparable1<_type, equality_comparable1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(additive2)
			: addable2<_type, _type2, subtractable2<_type, _type2, _tBase>>
		{};

		YCL_OP_TEMPLATE_HEADER1(additive1)
			: addable1<_type, subtractable1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(multiplicative2)
			: multipliable2<_type, _type2, dividable2<_type, _type2, _tBase>>
		{};

		YCL_OP_TEMPLATE_HEADER1(multiplicative1)
			: multipliable1<_type, dividable1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(integer_multiplicative2)
			: multiplicative2<_type, _type2, modable2<_type, _type2, _tBase>>
		{};

		YCL_OP_TEMPLATE_HEADER1(integer_multiplicative1)
			: multiplicative1<_type, modable1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(arithmetic2)
			: additive2<_type, _type2, multiplicative2<_type, _type2, _tBase>>
		{};

		YCL_OP_TEMPLATE_HEADER1(arithmetic1)
			: additive1<_type, multiplicative1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(integer_arithmetic2)
			: additive2<_type, _type2,
				integer_multiplicative2<_type, _type2, _tBase>
			>
		{};

		YCL_OP_TEMPLATE_HEADER1(integer_arithmetic1)
			: additive1<_type, integer_multiplicative1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(bitwise2)
			: xorable2<_type, _type2, andable2<_type, _type2,
				orable2<_type, _type2, _tBase>>
			>
		{};

		YCL_OP_TEMPLATE_HEADER1(bitwise1)
			: xorable1<_type, andable1<_type, orable1<_type, _tBase>>>
		{};


		YCL_OP_TEMPLATE_HEADER1(unit_steppable)
			: incrementable<_type, decrementable<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(shiftable2)
			: left_shiftable2<_type, _type2,
				right_shiftable2<_type, _type2, _tBase>
			>
		{};

		YCL_OP_TEMPLATE_HEADER1(shiftable1)
			: left_shiftable1<_type, right_shiftable1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(ring_operators2)
			: additive2<_type, _type2, subtractable2_left<_type, _type2,
				multipliable2<_type, _type2, _tBase>>
			>
		{};

		YCL_OP_TEMPLATE_HEADER1(ring_operators1)
			: additive1<_type, multipliable1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(ordered_ring_operators2)
			: ring_operators2<_type, _type2,
				totally_ordered2<_type, _type2, _tBase>
			>
		{};

		YCL_OP_TEMPLATE_HEADER1(ordered_ring_operators1)
			: ring_operators1<_type, totally_ordered1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(field_operators2)
			: ring_operators2<_type, _type2, dividable2<_type, _type2,
				dividable2_left<_type, _type2, _tBase>>
			>
		{};

		YCL_OP_TEMPLATE_HEADER1(field_operators1)
			: ring_operators1<_type, dividable1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(ordered_field_operators2)
			: field_operators2<_type, _type2,
				totally_ordered2<_type, _type2, _tBase>
			>
		{};

		YCL_OP_TEMPLATE_HEADER1(ordered_field_operators1)
			: field_operators1<_type, totally_ordered1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(euclidian_ring_operators2)
			: ring_operators2<_type, _type2, dividable2<_type, _type2,
				dividable2_left<_type, _type2, modable2<_type, _type2,
					modable2_left<_type, _type2, _tBase>>>
				>
			>
		{};

		YCL_OP_TEMPLATE_HEADER1(euclidian_ring_operators1)
			: ring_operators1<_type, dividable1<_type, modable1<_type, _tBase>>>
		{};


		YCL_OP_TEMPLATE_HEADER2(ordered_euclidian_ring_operators2)
			: totally_ordered2<_type, _type2,
				euclidian_ring_operators2<_type, _type2, _tBase>
			>
		{};

		YCL_OP_TEMPLATE_HEADER1(ordered_euclidian_ring_operators1)
			: totally_ordered1<_type, euclidian_ring_operators1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(euclidean_ring_operators2)
			: ring_operators2<_type, _type2, dividable2<_type, _type2,
				dividable2_left<_type, _type2, modable2<_type, _type2,
					modable2_left<_type, _type2, _tBase>>>
				>
			>
		{};

		YCL_OP_TEMPLATE_HEADER1(euclidean_ring_operators1)
			: ring_operators1<_type, dividable1<_type, modable1<_type, _tBase>>>
		{};


		YCL_OP_TEMPLATE_HEADER2(ordered_euclidean_ring_operators2)
			: totally_ordered2<_type, _type2,
				euclidean_ring_operators2<_type, _type2, _tBase>
			>
		{};

		YCL_OP_TEMPLATE_HEADER1(ordered_euclidean_ring_operators1)
			: totally_ordered1<_type, euclidean_ring_operators1<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER1(input_iteratable)
			: equality_comparable1<_type, incrementable<_type,
				dereferenceable<_type, _tBase>>
			>
		{};


		YCL_OP_TEMPLATE_HEADER1(output_iteratable)
			: incrementable<_type, _tBase>
		{};


		YCL_OP_TEMPLATE_HEADER1(forward_iteratable)
			: input_iteratable<_type, _tBase>
		{};


		YCL_OP_TEMPLATE_HEADER1(bidirectional_iteratable)
			: forward_iteratable<_type, decrementable<_type, _tBase>>
		{};


		YCL_OP_TEMPLATE_HEADER2(random_access_iteratable)
			: bidirectional_iteratable<_type, less_than_comparable1<_type,
				additive2<_type, _type2, indexable<_type, _type2, _tBase>>>
			>
		{};
	}


	/*!
	\note 用法同 boost.operators ，但迭代器相关部分参数有所删减。
	\note 参见 http://www.boost.org/doc/libs/1_47_0/boost/operators.hpp 。
	*/
	namespace operators
	{

		template<class> 
		struct is_chained_base
		{
			static yconstexpr bool value = false;
		};


	# define YCL_OP_CHAIN2(_name) \
		using ystdex::_impl::_name; \
		template<class _type, class _type2, class _tBase> \
		struct is_chained_base<_name<_type, _type2, _tBase>> \
		{ \
			static yconstexpr bool value = true; \
		};

	# define YCL_OP_CHAIN1(_name) \
		using ystdex::_impl::_name; \
		template<class _type, class _tBase> \
		struct is_chained_base<_name<_type, _tBase>> \
		{ \
			static yconstexpr bool value = true; \
		};

	#define YCL_OP_CHAIN(_name) \
		using ystdex::_impl::_name##2; \
		template<class _type, class _type2 = _type, \
			class _tBase = empty_base<_type>, \
			bool _v = is_chained_base<_type2>::value \
		> \
		struct _name \
			: _name##2<_type, _type2, _tBase> \
		{}; \
		\
		using ystdex::_impl::_name##1; \
		template<class _type, class _type2, class _tBase> \
		struct _name<_type, _type2, _tBase, true> \
			: _name##1<_type, _type2> \
		{}; \
		\
		template <class _type, class _tBase> \
		struct _name<_type, _type, _tBase, false> \
			: _name##1<_type, _tBase> \
		{}; \
		\
		template<class _type, class _type2, class _tBase, bool _v> \
		struct is_chained_base<_name<_type, _type2, _tBase, _v>> \
		{ \
			static yconstexpr bool value = true; \
		}; \
		\
		YCL_OP_CHAIN2(_name##2) \
		YCL_OP_CHAIN1(_name##1)


		YCL_OP_CHAIN(less_than_comparable)
		YCL_OP_CHAIN(equality_comparable)
		YCL_OP_CHAIN(multipliable)
		YCL_OP_CHAIN(addable)
		YCL_OP_CHAIN(subtractable)
		YCL_OP_CHAIN2(subtractable2_left)
		YCL_OP_CHAIN(dividable)
		YCL_OP_CHAIN2(dividable2_left)
		YCL_OP_CHAIN(modable)
		YCL_OP_CHAIN2(modable2_left)
		YCL_OP_CHAIN(xorable)
		YCL_OP_CHAIN(andable)
		YCL_OP_CHAIN(orable)

		YCL_OP_CHAIN1(incrementable)
		YCL_OP_CHAIN1(decrementable)

		YCL_OP_CHAIN1(dereferenceable)
		YCL_OP_CHAIN2(indexable)

		YCL_OP_CHAIN(left_shiftable)
		YCL_OP_CHAIN(right_shiftable)
		YCL_OP_CHAIN(equivalent)
		YCL_OP_CHAIN(partially_ordered)

		YCL_OP_CHAIN(totally_ordered)
		YCL_OP_CHAIN(additive)
		YCL_OP_CHAIN(multiplicative)
		YCL_OP_CHAIN(integer_multiplicative)
		YCL_OP_CHAIN(arithmetic)
		YCL_OP_CHAIN(integer_arithmetic)
		YCL_OP_CHAIN(bitwise)
		YCL_OP_CHAIN1(unit_steppable)
		YCL_OP_CHAIN(shiftable)
		YCL_OP_CHAIN(ring_operators)
		YCL_OP_CHAIN(ordered_ring_operators)
		YCL_OP_CHAIN(field_operators)
		YCL_OP_CHAIN(ordered_field_operators)
		YCL_OP_CHAIN(euclidian_ring_operators)
		YCL_OP_CHAIN(ordered_euclidian_ring_operators)
		YCL_OP_CHAIN(euclidean_ring_operators)
		YCL_OP_CHAIN(ordered_euclidean_ring_operators)
		YCL_OP_CHAIN1(input_iteratable)
		YCL_OP_CHAIN1(output_iteratable)
		YCL_OP_CHAIN1(forward_iteratable)
		YCL_OP_CHAIN1(bidirectional_iteratable)
		YCL_OP_CHAIN2(random_access_iteratable)

	#undef YCL_OP_CHAIN2
	#undef YCL_OP_CHAIN1
	#undef YCL_OP_CHAIN

	}

#undef YCL_OP_TEMPLATE_HEADER1
#undef YCL_OP_TEMPLATE_HEADER2
#undef YCL_OP_FRIEND

}

#endif

