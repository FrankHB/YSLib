/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file operators.hpp
\ingroup YStandardEx
\brief 重载操作符。
\version r2347
\author FrankHB <frankhb1989@gmail.com>
\since build 260
\par 创建时间:
	2011-11-13 14:58:05 +0800
\par 修改时间:
	2016-03-28 12:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Operators
\see http://www.boost.org/doc/libs/1_54_0/boost/operators.hpp 。
\see https://github.com/taocpp/operators 。

用法同 Boost.Operators ，但不公开带数字后缀的接口。
*/


#ifndef YB_INC_ystdex_operators_hpp_
#define YB_INC_ystdex_operators_hpp_ 1

#include "addressof.hpp" // for _t, false_type, true_type,
//	ystdex::constfn_addressof;

namespace ystdex
{

/*!
\brief 依赖名称查找操作：作为 ADL 的边界。
\since build 586
*/
namespace dependent_operators
{

//! \since build 682
using use_constfn = yimpl(true_type);

} // namespace dependent_operators;

#define YB_Impl_Operators_friend(_c, _op, _tRet, _spec, _expr, ...) \
	friend _c _tRet \
	operator _op(__VA_ARGS__) ynoexcept(_spec) \
	{ \
		return (_expr); \
	}
#define YB_Impl_Operators_friend_s(_c, _op, _tRet, _expr, ...) \
	YB_Impl_Operators_friend(_c, _op, _tRet, noexcept(_expr), _expr, __VA_ARGS__)
#define YB_Impl_Operators_friend_l(_c, _op, _type, _type2) \
	YB_Impl_Operators_friend_s(_c, _op, _type, x _op##= y, _type x, \
		const _type2& y)
#define YB_Impl_Operators_friend_r(_c, _op, _type, _type2) \
	YB_Impl_Operators_friend_s(_c, _op, _type, x _op##= std::move(y), _type x, \
		_type2&& y)
#define YB_Impl_Operators_Head1(_name) \
	template<class _type, class _tOpt = yimpl(use_constfn)> \
	struct _name
#define YB_Impl_Operators_Head2(_name) \
	template<class _type, typename _type2, \
		class _tOpt = yimpl(true_type)> \
	struct _name
#define YB_Impl_Operators_Head2_de(_name) \
	template<class _type, typename _type2 = _type, \
		class _tOpt = yimpl(use_constfn)> \
	struct _name
#define YB_Impl_Operators_Head2_spec(_name) \
	template<class _type, class _tOpt> \
	struct _name<_type, _type, _tOpt>
#define YB_Impl_Operators_Head3(_name) \
	template<class _type, typename _type2, typename _type3, \
		class _tOpt = yimpl(use_constfn)> \
	struct _name

namespace details
{

#define YB_Impl_Operators_SHead2(_name) \
	template<class _type, typename _type2, class _tOpt> \
	struct _name
#define YB_Impl_Operators_SHead1(_name) \
	template<class _type, class _tOpt> \
	struct _name


//! \since build 640
//@{
template<typename _type = void>
struct op_idt
{
	using type = _type;
};

//! \since build 655
template<>
struct op_idt<void>;

template<typename _type = void>
using op_idt_t = _t<op_idt<_type>>;
//@}


// NOTE: The trunk libstdc++ std::experimental::string_view comparison should
//	depend on the same technique.
// TODO: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52072. It is strange
//	to still have this bug. Not fully tested for G++ 5. See also https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67426.
#if true // YB_IMPL_GNUCPP
#	define YB_Impl_Operators_Compare2(_c, _op, _expr, _ptp, _ptp2) \
	YB_Impl_Operators_friend_s(_c, _op, bool, _expr, const _ptp& x, \
		const _ptp2& y)
#else
#	define YB_Impl_Operators_Compare2(_c, _op, _expr, _ptp, _ptp2) \
	YB_Impl_Operators_friend_s(_c, _op, bool, _expr, const _ptp& x, \
		const _ptp2& y) \
	YB_Impl_Operators_friend_s(_c, _op, bool, _expr, \
		const details::op_idt_t<_ptp>& x, const _ptp2& y) \
	YB_Impl_Operators_friend_s(_c, _op, bool, _expr, \
		const _ptp& x, const details::op_idt_t<_ptp2>& y)
#endif
#define YB_Impl_Operators_Compare1(_c, _op, _expr, _ptp) \
	YB_Impl_Operators_friend_s(_c, _op, bool, _expr, const _ptp& x, \
		const _ptp& y)

//! \since build 681
//@{
// TODO: Replace 'yconstfn' to deduced '_tOpt' result.
#define YB_Impl_Operators_Scmp2(_name, _op, _expr, _ptp, _ptp2) \
	YB_Impl_Operators_SHead2(_name) \
	{ \
		YB_Impl_Operators_Compare2(yconstfn, _op, _expr, _ptp, _ptp2) \
	};
#define YB_Impl_Operators_Scmp1(_name, _op, _expr, _ptp) \
	YB_Impl_Operators_SHead1(_name) \
	{ \
		YB_Impl_Operators_Compare1(yconstfn, _op, _expr, _ptp) \
	};

// TODO: Add strictly partial order comparison support to reduce duplicate code
//	between 'lt_*' and 'po_*'.
YB_Impl_Operators_Scmp2(lt_1, <=, !bool(x > y), _type, _type2)
YB_Impl_Operators_Scmp2(lt_2, >=, !bool(x < y), _type, _type2)
YB_Impl_Operators_Scmp2(lt_3, >, y < x, _type2, _type)
YB_Impl_Operators_Scmp2(lt_4, <, y > x, _type2, _type)
YB_Impl_Operators_Scmp2(lt_5, <=, !bool(y < x), _type2, _type)
YB_Impl_Operators_Scmp2(lt_6, >=, !bool(y > x), _type2, _type)

YB_Impl_Operators_Scmp1(lts_1, >, y < x, _type)
YB_Impl_Operators_Scmp1(lts_2, <=, !bool(y < x), _type)
YB_Impl_Operators_Scmp1(lts_3, >=, !bool(x < y), _type)

YB_Impl_Operators_Scmp2(ec_1, ==, y == x, _type2, _type)
YB_Impl_Operators_Scmp2(ec_2, !=, !bool(x == y), _type2, _type)
YB_Impl_Operators_Scmp2(ec_3, !=, !bool(x == y), _type, _type2)

YB_Impl_Operators_Scmp1(ecs_1, !=, !bool(x == y), _type)

YB_Impl_Operators_Scmp2(eq_1, !=, !bool(x < y) && !bool(x > y), _type, _type2)

YB_Impl_Operators_Scmp1(eqs_1, !=, !bool(x < y) && !bool(y < x), _type)

YB_Impl_Operators_Scmp2(po_1, <=, bool(x < y) || bool(x == y), _type, _type2)
YB_Impl_Operators_Scmp2(po_2, >=, bool(x > y) || bool(x == y), _type, _type2)
YB_Impl_Operators_Scmp2(po_5, <=, bool(y > x) || bool(y == x), _type2, _type)
YB_Impl_Operators_Scmp2(po_6, >=, bool(y < x) || bool(y == x), _type2, _type)

YB_Impl_Operators_Scmp1(pos_1, >, y < x, _type)
YB_Impl_Operators_Scmp1(pos_2, <=, !bool(y < x), _type)
YB_Impl_Operators_Scmp1(pos_3, >=, !bool(x < y), _type)

#undef YB_Impl_Operators_Scmp1
#undef YB_Impl_Operators_Scmp2

#undef YB_Impl_Operators_Compare2
#undef YB_Impl_Operators_Compare1

// TODO: Replace 'yconstfn' to deduced '_tOpt' result.
#define YB_Impl_Operators_Scon_or_ncon(_name, _op) \
	YB_Impl_Operators_SHead1(_name##_l) \
	{ \
		YB_Impl_Operators_friend_l(yconstfn, _op, _type, _type) \
	}; \
	\
	YB_Impl_Operators_SHead1(_name##_r) \
	{ \
		YB_Impl_Operators_friend_r(yconstfn, _op, _type, _type) \
	};
#define YB_Impl_Operators_Scon_or_ncon_lr(_name, _op) \
	YB_Impl_Operators_SHead2(_name##_lr_l) \
	{ \
		YB_Impl_Operators_friend_l(yconstfn, _op, _type, _type2) \
	}; \
	\
	YB_Impl_Operators_SHead2(_name##_lr_r) \
	{ \
		YB_Impl_Operators_friend_r(yconstfn, _op, _type, _type2) \
	};
#define YB_Impl_Operators_Scon2(_name, _op) \
	YB_Impl_Operators_SHead2(_name##_s1) \
	{ \
		YB_Impl_Operators_friend_s(yconstfn, _op, _type, y _op##= x, \
			const _type2& x, _type y) \
	}; \
	\
	YB_Impl_Operators_SHead2(_name##_s2) \
	{ \
		YB_Impl_Operators_friend_s(yconstfn, _op, _type, \
			y _op##= std::move(x), _type2&& x, _type y) \
	};
#define YB_Impl_Operators_Sncon2(_name, _op) \
	YB_Impl_Operators_SHead2(_name##_left_s1) \
	{ \
		YB_Impl_Operators_friend_s(yconstfn, _op, _type, _type(x) _op##= y, \
			const _type2& x, const _type& y) \
	}; \
	\
	YB_Impl_Operators_SHead2(_name##_left_s2) \
	{ \
		YB_Impl_Operators_friend_s(yconstfn, _op, _type, \
			_type(x) _op##= std::move(y), const _type2& x, _type&& y) \
	};
#define YB_Impl_Operators_Scon(_name, _op) \
	YB_Impl_Operators_Scon_or_ncon_lr(_name, _op) \
	YB_Impl_Operators_Scon2(_name, _op) \
	YB_Impl_Operators_Scon_or_ncon(_name, _op)
#define YB_Impl_Operators_Sncon(_name, _op) \
	YB_Impl_Operators_Scon_or_ncon_lr(_name, _op) \
	YB_Impl_Operators_Sncon2(_name, _op) \
	YB_Impl_Operators_Scon_or_ncon(_name, _op)

YB_Impl_Operators_Scon(con_mul, *)
YB_Impl_Operators_Scon(con_add, +)
YB_Impl_Operators_Sncon(ncon_sub, -)
YB_Impl_Operators_Sncon(ncon_div, /)
YB_Impl_Operators_Sncon(ncon_mod, %)
YB_Impl_Operators_Scon(con_xor, ^)
YB_Impl_Operators_Scon(con_and, &)
YB_Impl_Operators_Scon(con_or, |)

#undef YB_Impl_Operators_Sncon
#undef YB_Impl_Operators_Scon
#undef YB_Impl_Operators_Sncon2
#undef YB_Impl_Operators_Scon2
#undef YB_Impl_Operators_Scon_or_ncon_lr
#undef YB_Impl_Operators_Scon_or_ncon

// TODO: Replace 'yconstfn' to deduced '_tOpt' result.
#define YB_Impl_Operators_Sbin(_name, _op) \
	YB_Impl_Operators_SHead2(_name##_l) \
	{ \
		YB_Impl_Operators_friend_l(yconstfn, _op, _type, _type2) \
	}; \
	\
	YB_Impl_Operators_SHead2(_name##_r) \
	{ \
		YB_Impl_Operators_friend_r(yconstfn, _op, _type, _type2) \
	};

YB_Impl_Operators_Sbin(con_lshl, <<)
YB_Impl_Operators_Sbin(con_rshl, >>)

#undef YB_Impl_Operators_Sbin

#undef YB_Impl_Operators_SHead1
#undef YB_Impl_Operators_SHead2
//@}

} // namespace details;


namespace dependent_operators
{

//! \since build 681
//@{
YB_Impl_Operators_Head2_de(less_than_comparable)
	: details::lt_1<_type, _type2, _tOpt>,
	details::lt_2<_type, _type2, _tOpt>,
	details::lt_3<_type, _type2, _tOpt>, details::lt_4<_type, _type2, _tOpt>,
	details::lt_5<_type, _type2, _tOpt>, details::lt_6<_type, _type2, _tOpt>
{};

YB_Impl_Operators_Head2_spec(less_than_comparable)
	: details::lts_1<_type, _tOpt>, details::lts_2<_type, _tOpt>,
	details::lts_3<_type, _tOpt>
{};


YB_Impl_Operators_Head2_de(equality_comparable)
	: details::ec_1<_type, _type2, _tOpt>, details::ec_2<_type, _type2, _tOpt>,
	details::ec_3<_type, _type2, _tOpt>
{};

YB_Impl_Operators_Head2_spec(equality_comparable)
	: details::ecs_1<_type, _tOpt>
{};


YB_Impl_Operators_Head2_de(equivalent)
	: details::eq_1<_type, _type2, _tOpt>
{};

YB_Impl_Operators_Head2_spec(equivalent)
	: details::eqs_1<_type, _tOpt>
{};


YB_Impl_Operators_Head2_de(partially_ordered)
	: details::po_1<_type, _type2, _tOpt>, details::po_2<_type, _type2, _tOpt>,
	details::lt_3<_type, _type2, _tOpt>, details::lt_4<_type, _type2, _tOpt>,
	details::po_5<_type, _type2, _tOpt>, details::po_6<_type, _type2, _tOpt>
{};

YB_Impl_Operators_Head2_spec(partially_ordered)
	: details::pos_1<_type, _tOpt>, details::pos_2<_type, _tOpt>,
	details::pos_3<_type, _tOpt>
{};


#define YB_Impl_Operators_Commutative(_name, _op, _dname) \
	YB_Impl_Operators_Head2_de(_name) \
		: _dname##_lr_l<_type, _type2, _tOpt>, \
		_dname##_lr_r<_type, _type2, _tOpt>, \
		_dname##_s1<_type, _type2, _tOpt>, _dname##_s2<_type, _type2, _tOpt> \
	{}; \
	\
	YB_Impl_Operators_Head2_spec(_name) \
		: _dname##_l<_type, _tOpt>, _dname##_r<_type, _tOpt> \
	{};


#define YB_Impl_Operators_NonCommutative(_name, _op, _dname) \
	YB_Impl_Operators_Head2_de(_name) \
		: _dname##_lr_l<_type, _type2, _tOpt>, _dname##_lr_r<_type, _type2, _tOpt> \
	{}; \
	\
	YB_Impl_Operators_Head2(_name##_left) \
		: _dname##_left_s1<_type, _type2, _tOpt>, \
		_dname##_left_s2<_type, _type2, _tOpt> \
	{}; \
	\
	YB_Impl_Operators_Head2_spec(_name) \
		: _dname##_l<_type, _tOpt>, _dname##_r<_type, _tOpt> \
	{};

YB_Impl_Operators_Commutative(multipliable, *, details::con_mul)
YB_Impl_Operators_Commutative(addable, +, details::con_add)
YB_Impl_Operators_NonCommutative(subtractable, -, details::ncon_sub)
YB_Impl_Operators_NonCommutative(dividable, /, details::ncon_div)
YB_Impl_Operators_NonCommutative(modable, %, details::ncon_mod)
YB_Impl_Operators_Commutative(xorable, ^, details::con_xor)
YB_Impl_Operators_Commutative(andable, &, details::con_and)
YB_Impl_Operators_Commutative(orable, |, details::con_or)

#undef YB_Impl_Operators_NonCommutative
#undef YB_Impl_Operators_Commutative


#define YB_Impl_Operators_Binary(_name, _op, _dname) \
	YB_Impl_Operators_Head2_de(_name) \
		: _dname##_l<_type, _type2, _tOpt>, _dname##_r<_type, _type2, _tOpt> \
	{};

YB_Impl_Operators_Binary(left_shiftable, <<, details::con_lshl)
YB_Impl_Operators_Binary(right_shiftable, >>, details::con_rshl)

#undef YB_Impl_Operators_Binary
//@}

YB_Impl_Operators_Head1(incrementable)
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

YB_Impl_Operators_Head1(decrementable)
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

//! \since build 681
//@{
YB_Impl_Operators_Head2(dereferenceable)
{
	// TODO: Add non-const overloaded version? SFINAE?
	// TODO: Use '_tOpt'.
	yconstfn decltype(ystdex::constfn_addressof(std::declval<const _type2&>()))
	operator->() const ynoexcept_spec(*std::declval<const _type&>())
	{
		return ystdex::constfn_addressof(*static_cast<const _type&>(*this));
	}
};

YB_Impl_Operators_Head3(indexable)
{
	// TODO: Add non-const overloaded version? SFINAE?
	// TODO: Use '_tOpt'.
	yconstfn _type3
	operator[](_type2 n) const
		ynoexcept_spec(*(std::declval<const _type&>() + n))
	{
		return *(static_cast<const _type&>(*this) + n);
	}
};


YB_Impl_Operators_Head2_de(totally_ordered)
	: less_than_comparable<_type, _type2, _tOpt>,
	equality_comparable<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2_de(additive)
	: addable<_type, _type2, _tOpt>, subtractable<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2_de(multiplicative)
	: multipliable<_type, _type2, _tOpt>, dividable<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2_de(integer_multiplicative)
	: multiplicative<_type, _type2, _tOpt>, modable<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2_de(arithmetic)
	: additive<_type, _type2, _tOpt>, multiplicative<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2_de(integer_arithmetic)
	: additive<_type, _type2, _tOpt>,
	integer_multiplicative<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2_de(bitwise)
	: xorable<_type, _type2, _tOpt>, andable<_type, _type2, _tOpt>,
	orable<_type, _type2, _tOpt>
{};
//@}


YB_Impl_Operators_Head1(unit_steppable)
	: incrementable<_type>, decrementable<_type>
{};


//! \since build 681
//@{
YB_Impl_Operators_Head2_de(shiftable) : left_shiftable<_type, _type2, _tOpt>,
	right_shiftable<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2_de(ring_operators) : additive<_type, _type2, _tOpt>,
	subtractable_left<_type, _type2, _tOpt>, multipliable<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2_de(ordered_ring_operators) : ring_operators<_type,
	_type2, _tOpt>, totally_ordered<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2_de(field_operators)
	: ring_operators<_type, _type2, _tOpt>, dividable<_type, _type2, _tOpt>,
	dividable_left<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2_de(ordered_field_operators) : field_operators<_type,
	_type2, _tOpt>, totally_ordered<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2_de(euclidean_ring_operators)
	: ring_operators<_type, _type2, _tOpt>, dividable<_type, _type2, _tOpt>,
	dividable_left<_type, _type2, _tOpt>, modable<_type, _type2, _tOpt>,
	modable_left<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2_de(ordered_euclidean_ring_operators)
	: totally_ordered<_type, _type2, _tOpt>,
	euclidean_ring_operators<_type, _type2, _tOpt>
{};
//@}


//! \since build 576
YB_Impl_Operators_Head2(input_iteratable) : equality_comparable<_type, _type,
	_tOpt>, incrementable<_type>, dereferenceable<_type, _type2, _tOpt>
{};


//! \since build 681
YB_Impl_Operators_Head2(output_iteratable) : incrementable<_type>
{};


//! \since build 576
//@{
YB_Impl_Operators_Head2(forward_iteratable)
	: input_iteratable<_type, _type2, _tOpt>
{};


YB_Impl_Operators_Head2(bidirectional_iteratable)
	: forward_iteratable<_type, _type2, _tOpt>, decrementable<_type>
{};


YB_Impl_Operators_Head3(random_access_iteratable) : bidirectional_iteratable<
	_type, _type3, _tOpt>, less_than_comparable<_type, _type, _tOpt>,
	additive<_type, _type2, _tOpt>, indexable<_type, _type2, _type, _tOpt>
{};
//@}

/*!
\since build 682
\note 第三模板参数调整生成的操作符的声明。
\note 当前二元和三元操作符都使用 yconstfn 。
\todo 实现调整选项。
*/
template<class _type, typename _type2 = _type, class _tOpt = use_constfn>
struct operators : totally_ordered<_type, _type2, _tOpt>,
	integer_arithmetic<_type, _type2, _tOpt>, bitwise<_type, _type2, _tOpt>
{};

} // namespace dependent_operators;

//! \since build 586
using namespace dependent_operators;

#undef YB_Impl_Operators_Head1
#undef YB_Impl_Operators_Head2
#undef YB_Impl_Operators_Head2_de
#undef YB_Impl_Operators_Head2_spec
#undef YB_Impl_Operators_Head3
#undef YB_Impl_Operators_friend_l
#undef YB_Impl_Operators_friend_r
#undef YB_Impl_Operators_friend_s
#undef YB_Impl_Operators_friend

} // namespace ystdex;

#endif

