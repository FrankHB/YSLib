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
\version r2827
\author FrankHB <frankhb1989@gmail.com>
\since build 260
\par 创建时间:
	2011-11-13 14:58:05 +0800
\par 修改时间:
	2016-09-21 15:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Operators
\see http://www.boost.org/doc/libs/1_60_0/boost/operators.hpp 。
\see https://github.com/taocpp/operators 。

用法同 Boost.Operators ，但不公开带数字后缀的接口。
*/


#ifndef YB_INC_ystdex_operators_hpp_
#define YB_INC_ystdex_operators_hpp_ 1

#include "addressof.hpp" // for "type_traits.hpp", false_, true_, _t,
//	empty_base, ystdex::constfn_addressof;
#include "integer_sequence.hpp" // for index_sequence, vseq::defer_apply_t,
//	vseq::_a, vseq::fold_t;

namespace ystdex
{

/*!
\brief 依赖名称查找操作：作为 ADL 的边界。
\note 操作模板不指定为类模板或别名模板。
\since build 682
*/
namespace dep_ops
{

//! \since build 684
using no_constfn = yimpl(false_);
//! \since build 682
using use_constfn = yimpl(true_);

} // namespace dep_ops;

#define YB_Impl_Operators_DeOpt class _tOpt = yimpl(use_constfn)
#define YB_Impl_Operators_H_n(_args) template<class _type, _args>
#define YB_Impl_Operators_H1 \
	YB_Impl_Operators_H_n(YB_Impl_Operators_DeOpt)
#define YB_Impl_Operators_H2 \
	YB_Impl_Operators_H_n(typename _type2 YPP_Comma YB_Impl_Operators_DeOpt)
#define YB_Impl_Operators_H2_de \
	YB_Impl_Operators_H_n(typename _type2 = _type YPP_Comma \
		YB_Impl_Operators_DeOpt)
#define YB_Impl_Operators_H2_Alias(_name, ...) \
	YB_Impl_Operators_H2 \
	yimpl(using) _name = __VA_ARGS__;
#define YB_Impl_Operators_H2_Alias_de(_name, ...) \
	YB_Impl_Operators_H2_de \
	yimpl(using) _name = __VA_ARGS__;
#define YB_Impl_Operators_H3 \
	YB_Impl_Operators_H_n(typename _type2 YPP_Comma typename _type3 \
		YPP_Comma YB_Impl_Operators_DeOpt)

namespace details
{

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


//! \since build 682
//@{
template<class... _types>
struct ebases : _types...
{};

template<size_t _vN, class _type, typename _type2, class _tOpt> \
struct bin_ops;

template<class, typename, class, class>
struct ops_seq;

template<class _type, typename _type2, class _tOpt, size_t... _vSeq>
struct ops_seq<_type, _type2, _tOpt, index_sequence<_vSeq...>>
{
	using type = ebases<bin_ops<_vSeq, _type, _type2, _tOpt>...>;
};
//@}


#define YB_Impl_Operators_f(_c, _op, _tRet, _expr, ...) \
	friend _c YB_ATTR(always_inline) _tRet \
	operator _op(__VA_ARGS__) ynoexcept(noexcept(_expr)) \
	{ \
		return (_expr); \
	}
#if YB_IMPL_GNUCPP && YB_IMPL_GNUCPP < 50000
#	define YB_Impl_Operators_bin_ts(_n, _f, _c, _opt, ...) \
	YB_Impl_Operators_H_n(typename _type2) \
	struct bin_ops<_n, _type, _type2, dep_ops::_opt> \
	{ \
		_f(_c, __VA_ARGS__) \
	};
#else
#	define YB_Impl_Operators_bin_ts(_n, _f, _c, _opt, ...) \
	YB_Impl_Operators_H_n(typename _type2) \
	struct bin_ops<_n, _type, _type2, dep_ops::_opt> \
	{ \
		template<yimpl(typename = void)> \
		_f(_c, __VA_ARGS__) \
	};
#endif
#define YB_Impl_Operators_bin_spec(_n, _f, ...) \
	YB_Impl_Operators_bin_ts(_n, _f, inline, no_constfn, __VA_ARGS__) \
	YB_Impl_Operators_bin_ts(_n, _f, yconstfn, use_constfn, __VA_ARGS__)
// NOTE: The trunk libstdc++ std::experimental::string_view comparison should
//	depend on the same technique.
// TODO: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52072. It is strange
//	to still have this bug. Not fully tested for G++ 5. See also https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67426.
#if true // YB_IMPL_GNUCPP
#	define YB_Impl_Operators_cmpf(_c, _op, _expr, _ptp, _ptp2) \
	YB_Impl_Operators_f(_c, _op, bool, _expr, const _ptp& x, const _ptp2& y)
#else
#	define YB_Impl_Operators_cmpf(_c, _op, _expr, _ptp, _ptp2) \
	YB_Impl_Operators_f(_c, _op, bool, _expr, const _ptp& x, const _ptp2& y) \
	YB_Impl_Operators_f(_c, _op, bool, _expr, const op_idt_t<_ptp>& x, \
		const _ptp2& y) \
	YB_Impl_Operators_f(_c, _op, bool, _expr, const _ptp& x, \
		const op_idt_t<_ptp2>& y)
#endif

//! \since build 682
//@{
#define YB_Impl_Operators_cmp(_n, _op, _expr, _ptp, _ptp2) \
	YB_Impl_Operators_bin_spec(_n, YB_Impl_Operators_cmpf, _op, _expr, \
		_ptp, _ptp2)

// TODO: Add strictly partial order comparison support to reduce duplicate code
//	between 'less_than_comparable' and 'partially_ordered'.

// NOTE: Range 0-5 is reserved for %less_than_comparable. Range 2-3 is also
//	used by %partially_ordered.
YB_Impl_Operators_cmp(0, <=, !bool(x > y), _type, _type2)
YB_Impl_Operators_cmp(1, >=, !bool(x < y), _type, _type2)
YB_Impl_Operators_cmp(2, >, y < x, _type2, _type)
YB_Impl_Operators_cmp(3, <, y > x, _type2, _type)
YB_Impl_Operators_cmp(4, <=, !bool(y < x), _type2, _type)
YB_Impl_Operators_cmp(5, >=, !bool(y > x), _type2, _type)

// NOTE: Range 6-8 is reserved for %equality_comparable.
YB_Impl_Operators_cmp(6, ==, y == x, _type2, _type)
YB_Impl_Operators_cmp(7, !=, !bool(x == y), _type2, _type)
YB_Impl_Operators_cmp(8, !=, !bool(x == y), _type, _type2)

// NOTE: Range 9-10 is reserved for %equivalent.
YB_Impl_Operators_cmp(9, !=, !bool(x < y) && !bool(x > y), _type, _type2)
YB_Impl_Operators_cmp(10, !=, !bool(x < y) && !bool(y < x), _type, _type2)

// NOTE: Range 11-14 is reserved for %partially_ordered.
YB_Impl_Operators_cmp(11, <=, bool(x < y) || bool(x == y), _type, _type2)
YB_Impl_Operators_cmp(12, >=, bool(x > y) || bool(x == y), _type, _type2)
YB_Impl_Operators_cmp(13, <=, bool(y > x) || bool(y == x), _type2, _type)
YB_Impl_Operators_cmp(14, >=, bool(y < x) || bool(y == x), _type2, _type)

#undef YB_Impl_Operators_cmp
#undef YB_Impl_Operators_cmpf


// NOTE: Offset range 0-1 is reserved for general binary operations. Range 2-3
//	is for commutative operations. Range 4-5 is for non commutative "left"
//	operations.
yconstfn size_t
ops_bin_id(size_t id, size_t off) ynothrow
{
	return (id + 1) * 16 + off;
}

template<size_t _vID, size_t... _vSeq>
using ops_bin_id_seq = index_sequence<ops_bin_id(_vID, _vSeq)...>;

#define YB_Impl_Operators_bin_tmpl(_id, _off, _op, ...) \
	YB_Impl_Operators_bin_spec(ops_bin_id(_id YPP_Comma _off), \
		YB_Impl_Operators_f, _op, _type, __VA_ARGS__)
#define YB_Impl_Operators_bin(_id, _op) \
	YB_Impl_Operators_bin_tmpl(_id, 0, _op, x _op##= y, _type x, \
		const _type2& y) \
	YB_Impl_Operators_bin_tmpl(_id, 1, _op, x _op##= std::move(y), _type x, \
		_type2&& y)
#define YB_Impl_Operators_con(_id, _op) \
	YB_Impl_Operators_bin(_id, _op) \
	YB_Impl_Operators_bin_tmpl(_id, 2, _op, y _op##= x, const _type2& x, \
		_type y) \
	YB_Impl_Operators_bin_tmpl(_id, 3, _op, y _op##= std::move(x), _type2&& x, \
		_type y)
#define YB_Impl_Operators_left(_id, _op) \
	YB_Impl_Operators_bin(_id, _op) \
	YB_Impl_Operators_bin_tmpl(_id, 4, _op, _type(x) _op##= y, \
		const _type2& x, const _type& y) \
	YB_Impl_Operators_bin_tmpl(_id, 5, _op, \
		_type(x) _op##= std::move(y), const _type2& x, _type&& y)

YB_Impl_Operators_con(0, +)
YB_Impl_Operators_left(1, -)
YB_Impl_Operators_con(2, *)
YB_Impl_Operators_left(3, /)
YB_Impl_Operators_left(4, %)
YB_Impl_Operators_con(5, ^)
YB_Impl_Operators_con(6, &)
YB_Impl_Operators_con(7, |)
YB_Impl_Operators_bin(8, <<)
YB_Impl_Operators_bin(9, >>)

#undef YB_Impl_Operators_left
#undef YB_Impl_Operators_con
#undef YB_Impl_Operators_bin
#undef YB_Impl_Operators_bin_tmpl

#undef YB_Impl_Operators_bin_spec
#undef YB_Impl_Operators_bin_ts
#undef YB_Impl_Operators_f

template<class _type, typename _type2, class _tOpt,
	template<typename...> class... _gOps>
using flat_ops = vseq::defer_t<ebases, vseq::fold_t<vseq::_a<vseq::concat_t>,
	empty_base<>,
	empty_base<vseq::defer_t<empty_base, _gOps<_type, _type2, _tOpt>>...>>>;
//@}

} // namespace details;


namespace dep_ops
{

//! \since build 682
//@{
#define YB_Impl_Operators_Compare(_name, _bseq, _bseq_s) \
	YB_Impl_Operators_H2_Alias_de(_name, _t<details::ops_seq<_type, _type2, \
		_tOpt, cond_t<is_same<_type, _type2>, index_sequence<_bseq_s>, \
		index_sequence<_bseq>>>>)

YB_Impl_Operators_Compare(less_than_comparable, 0 YPP_Comma 1 YPP_Comma 2
	YPP_Comma 3 YPP_Comma 4 YPP_Comma 5, 2 YPP_Comma 4 YPP_Comma 1)


YB_Impl_Operators_Compare(equality_comparable, 6 YPP_Comma 7 YPP_Comma 8,
	7)


YB_Impl_Operators_Compare(equivalent, 9, 10)


YB_Impl_Operators_Compare(partially_ordered, 11 YPP_Comma 12 YPP_Comma 2
	YPP_Comma 3 YPP_Comma 13 YPP_Comma 14, 2 YPP_Comma 11 YPP_Comma 14)

#undef YB_Impl_Operators_Compare


#define YB_Impl_Operators_Binary_Tmpl(_id, _name, _bseq) \
	YB_Impl_Operators_H2_Alias_de(_name, _t<details::ops_seq<_type, _type2, \
		_tOpt, details::ops_bin_id_seq<_id, _bseq>>>)
#define YB_Impl_Operators_Binary(_id, _name) \
	YB_Impl_Operators_Binary_Tmpl(_id, _name, 0 YPP_Comma 1)
#define YB_Impl_Operators_Commutative(_id, _name) \
	YB_Impl_Operators_H2_Alias_de(_name, _t< \
		details::ops_seq<_type, _type2, _tOpt, cond_t<is_same<_type, _type2>, \
		details::ops_bin_id_seq<_id, 0 YPP_Comma 1>, \
		details::ops_bin_id_seq<_id, 0 YPP_Comma 1 YPP_Comma 2 YPP_Comma 3>>>>)
#define YB_Impl_Operators_Left(_id, _name) \
	YB_Impl_Operators_H2_Alias_de(_name, _t<details::ops_seq<_type, _type2, \
		_tOpt, details::ops_bin_id_seq<_id, 0 YPP_Comma 1>>>) \
	YB_Impl_Operators_H2_Alias_de(_name##_left, _t<details::ops_seq<_type, \
		_type2, _tOpt, details::ops_bin_id_seq<_id, 4 YPP_Comma 5>>>)

YB_Impl_Operators_Commutative(0, addable)


YB_Impl_Operators_Left(1, subtractable)


YB_Impl_Operators_Commutative(2, multipliable)


YB_Impl_Operators_Left(3, dividable)


YB_Impl_Operators_Left(4, modable)


YB_Impl_Operators_Binary(5, left_shiftable)


YB_Impl_Operators_Binary(6, right_shiftable)


YB_Impl_Operators_Commutative(7, xorable)


YB_Impl_Operators_Commutative(8, andable)


YB_Impl_Operators_Commutative(9, orable)

#undef YB_Impl_Operators_Binary_Tmpl
#undef YB_Impl_Operators_Left
#undef YB_Impl_Operators_Commutative
#undef YB_Impl_Operators_Binary


YB_Impl_Operators_H1
yimpl(struct) incrementable
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

YB_Impl_Operators_H1
yimpl(struct) decrementable
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

YB_Impl_Operators_H2
yimpl(struct) dereferenceable
{
	// TODO: Add non-const overloaded version? SFINAE?
	// TODO: Use '_tOpt'.
	yconstfn decltype(ystdex::constfn_addressof(std::declval<const _type2&>()))
	operator->() const ynoexcept_spec(*std::declval<const _type&>())
	{
		return ystdex::constfn_addressof(*static_cast<const _type&>(*this));
	}
};

YB_Impl_Operators_H3
yimpl(struct) indexable
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


#define YB_Impl_Operators_FlatAlias2_de(_name, ...) \
	YB_Impl_Operators_H2_Alias_de(_name, details::flat_ops<_type, _type2, \
		_tOpt, __VA_ARGS__>)
	
YB_Impl_Operators_FlatAlias2_de(totally_ordered, less_than_comparable,
	equality_comparable)


YB_Impl_Operators_FlatAlias2_de(additive, addable, subtractable)


YB_Impl_Operators_FlatAlias2_de(multiplicative, multipliable, dividable)


YB_Impl_Operators_FlatAlias2_de(integer_multiplicative, multiplicative, modable)


YB_Impl_Operators_FlatAlias2_de(arithmetic, additive, multiplicative)


YB_Impl_Operators_FlatAlias2_de(integer_arithmetic, additive,
	integer_multiplicative)


YB_Impl_Operators_FlatAlias2_de(bitwise, xorable, andable, orable)


YB_Impl_Operators_H1
yimpl(using) unit_steppable
	= details::ebases<incrementable<_type>, decrementable<_type>>;


YB_Impl_Operators_FlatAlias2_de(shiftable, left_shiftable, right_shiftable)


YB_Impl_Operators_FlatAlias2_de(ring_operators, additive, subtractable_left,
	multipliable)


YB_Impl_Operators_FlatAlias2_de(ordered_ring_operators, ring_operators,
	totally_ordered)


YB_Impl_Operators_FlatAlias2_de(field_operators, ring_operators, dividable,
	dividable_left)


YB_Impl_Operators_FlatAlias2_de(ordered_field_operators, field_operators,
	totally_ordered)


YB_Impl_Operators_FlatAlias2_de(euclidean_ring_operators, ring_operators,
	dividable, dividable_left, modable, modable_left)


YB_Impl_Operators_FlatAlias2_de(ordered_euclidean_ring_operators,
	totally_ordered, euclidean_ring_operators)


YB_Impl_Operators_H2_Alias(input_iteratable, details::ebases<
	equality_comparable<_type, _type, _tOpt>, incrementable<_type>,
	dereferenceable<_type, _type2, _tOpt>>)


YB_Impl_Operators_H2_Alias(output_iteratable,
	details::ebases<incrementable<_type>>)


YB_Impl_Operators_H2_Alias(forward_iteratable,
	details::ebases<input_iteratable<_type, _type2, _tOpt>>)


YB_Impl_Operators_H2_Alias(bidirectional_iteratable, details::ebases<
	forward_iteratable<_type, _type2, _tOpt>, decrementable<_type>>)


YB_Impl_Operators_H3
yimpl(using) random_access_iteratable
	= details::ebases<bidirectional_iteratable<_type, _type3, _tOpt>,
	less_than_comparable<_type, _type, _tOpt>,
	additive<_type, _type2, _tOpt>, indexable<_type, _type2, _type, _tOpt>>;

/*!
\note 第三模板参数调整生成的操作符的声明。
\note 当前二元和三元操作符都使用 yconstfn 。
\todo 实现调整选项。
*/
YB_Impl_Operators_FlatAlias2_de(operators, totally_ordered, integer_arithmetic,
	bitwise)

#undef YB_Impl_Operators_FlatAlias2_de
//@}

} // namespace dep_ops;

//! \since build 586
using namespace dep_ops;

#undef YB_Impl_Operators_H3
#undef YB_Impl_Operators_H2_Alias_de
#undef YB_Impl_Operators_H2_Alias
#undef YB_Impl_Operators_H2
#undef YB_Impl_Operators_H1
#undef YB_Impl_Operators_H_n
#undef YB_Impl_Operators_DeOpt

} // namespace ystdex;

#endif

