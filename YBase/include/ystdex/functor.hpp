/*
	© 2010-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file functor.hpp
\ingroup YStandardEx
\brief 通用仿函数。
\version r1050
\author FrankHB <frankhb1989@gmail.com>
\since build 588
\par 创建时间:
	2015-03-29 00:35:44 +0800
\par 修改时间:
	2022-02-20 17:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Functor

扩展标准库头 \c \<functional> ，提供若干函数对象。
*/


#ifndef YB_INC_ystdex_functor_hpp_
#define YB_INC_ystdex_functor_hpp_ 1

#include "ref.hpp" // for <functional>, __cpp_lib_transparent_operators,
//	is_detected, enable_if_t, ystdex::addressof, nor_, not_,
//	is_reference_wrapper, and_;
#include "type_traits.hpp" // for ystdex::addrof_t, ystdex::indirect_t,
//	ystdex::first_t, ystdex::second_t;
#include <numeric> // for std::accumulate;

/*!
\brief \c \<functional> 特性测试宏。
\see ISO C++20 [version.syn] 。
\see WG21 P0941R2 2.2 。
\see https://blogs.msdn.microsoft.com/vcblog/2015/06/19/c111417-features-in-vs-2015-rtm/ 。
\since build 679
*/
//@{
#ifndef __cpp_lib_transparent_operators
#	if YB_IMPL_MSCPP >= 1800 || __cplusplus >= 201210L
#		define __cpp_lib_transparent_operators 201210L
#	endif
#endif
//@}

namespace ystdex
{

//! \since build 679
//@{
namespace details
{

//! \since build 680
template<class _type, typename = void>
using mem_is_transparent_t = typename _type::is_transparent;

} // namespace details;

//! \note 第二参数用于传递 SFINAE 模板参数，可为键类型。
//@{
/*!
\ingroup unary_type_traits
\brief 判断 _type 是否包含 is_transparent 成员类型。
\since build 683
*/
template<typename _type, typename _tDummy = void>
struct has_mem_is_transparent
	: is_detected<details::mem_is_transparent_t, _type, _tDummy>
{};

/*!
\ingroup metafunctions
\brief 移除不满足包含 is_transparent 成员类型比较器的重载。
\sa enable_if_t
\sa has_mem_is_transparent
\see WG21 N3657 。
\since build 679
*/
template<typename _fComp, typename _tDummy, typename _type = void>
using enable_if_transparent_t
	= enable_if_t<has_mem_is_transparent<_fComp, _tDummy>::value, _type>;
//@}
//@}

/*!	\defgroup functors General Functors
\brief 仿函数。
\note 函数对象包含函数指针和仿函数。
\warning 其中的类类型一般可被继承但非虚析构。
\since build 243
*/
//@{
/*!
\brief 恒等仿函数。
\since build 689

类似 ISO C++20 的 std::identity ，但支持模板参数。
这个仿函数的命名被保持和范畴论的恒等函子(identity functor) 的一般表达一致。
*/
//@{
template<typename _type = void>
struct id
{
	YB_PURE yconstfn _type
	operator()(const _type& x) const ynothrow
	{
		return x;
	}

	//! \since build 756
	YB_STATELESS yconstfn
	operator add_ptr_t<_type(_type)>() const ynothrow
	{
		return [](_type x) ynothrow -> _type{
			return x;
		};
	}
};

template<>
struct id<void>
{
	//! \since build 939
	template<typename _type>
	YB_PURE yconstfn _type&&
	operator()(_type&& x) const ynothrow
	{
		return yforward(x);
	}

	//! \since build 756
	template<typename _type>
	YB_STATELESS yconstfn
	operator add_ptr_t<_type(_type)>() const ynothrow
	{
		return [](_type x) ynothrow -> _type{
			return x;
		};
	}
};
//@}


/*!
\brief ystdex::addressof 仿函数。
\since build 660
*/
//@{
template<typename _type = void>
struct addressof_op
{
	yconstfn _type*
	operator()(_type& x) const ynothrow
	{
		return ystdex::addressof(x);
	}
};

template<>
struct addressof_op<void>
{
	template<typename _type>
	yconstfn _type*
	operator()(_type& x) const ynothrow
	{
		return ystdex::addressof(x);
	}
};
//@}


/*!
\brief 成员 get 操作。
\since build 537
*/
//@{
template<typename _type = void>
struct mem_get
{
	auto
	operator()(const _type& x) const -> decltype(x.get())
	{
		return yforward(x.get());
	}
};

template<>
struct mem_get<void>
{
	template<typename _type>
	auto
	operator()(_type&& x) const ynoexcept_spec(std::declval<_type>().get())
		-> decltype(x.get())
	{
		return yforward(x.get());
	}
};
//@}

/*!
\brief 相等关系仿函数。
\note 除 reference_wrapper 相关的重载外同 boost::is_equal 。
\since build 450
*/
struct is_equal
{
	//! \since build 594
	template<typename _type1, typename _type2>
	yconstfn yimpl(enable_if_t)<nor_<is_reference_wrapper<_type1>,
		is_reference_wrapper<_type2>>::value, bool>
	operator()(const _type1& x, const _type2& y) const ynoexcept_spec(x == y)
	{
		return x == y;
	}
	//! \since build 554
	//@{
	template<typename _type1, typename _type2>
	yconstfn yimpl(enable_if_t)<and_<is_reference_wrapper<_type1>,
		not_<is_reference_wrapper<_type2>>>::value, bool>
	operator()(const _type1& x, const _type2& y) const ynothrow
	{
		return ystdex::addressof(x.get()) == ystdex::addressof(y);
	}
	template<typename _type1, typename _type2>
	yconstfn yimpl(enable_if_t)<and_<not_<is_reference_wrapper<_type1>>,
		is_reference_wrapper<_type2>>::value, bool>
	operator()(const _type1& x, const _type2& y) const ynothrow
	{
		return ystdex::addressof(x) == ystdex::addressof(y.get());
	}
	template<typename _type1, typename _type2>
	yconstfn yimpl(enable_if_t)<and_<is_reference_wrapper<_type1>,
		is_reference_wrapper<_type2>>::value, bool>
	operator()(const _type1& x, const _type2& y) const ynothrow
	{
		return ystdex::addressof(x.get()) == ystdex::addressof(y.get());
	}
	//@}
};

//! \since build 880
//@{
// XXX: See $2019-01 @ %Documentation::Workflow.
#if YB_IMPL_GNUCPP >= 60000
#	define YB_Impl_Functor_pushf YB_Diag_Push \
	YB_Diag_Ignore(suggest-attribute=const) \
	YB_Diag_Ignore(suggest-attribute=pure)
#	define YB_Impl_Functor_popf YB_Diag_Pop
#else
#	define YB_Impl_Functor_pushf
#	define YB_Impl_Functor_popf
#endif
//@}

// NOTE: There is no 'constexpr' in WG21 N3936; but it is in ISO/IEC 14882:2014
//	and drafts later.
#define YB_Impl_Functor_Ops_Primary(_q, _n, _tRet, _using_stmt, _expr, ...) \
	template<typename _type = void> \
	struct _n \
	{ \
		using first_argument_type = _type; \
		_using_stmt \
		using result_type = _tRet; \
		\
		YB_Impl_Functor_pushf \
		yconstfn _tRet \
		operator()(__VA_ARGS__) const _q \
		{ \
			return _expr; \
		} \
		YB_Impl_Functor_popf \
	};

#define YB_Impl_Functor_Ops_Spec(_q, _n, _tparams, _params, _expr) \
	template<> \
	struct _n<void> \
	{ \
		using is_transparent = yimpl(void); \
		\
		YB_Impl_Functor_pushf \
		template<_tparams> \
		yconstfn auto \
		operator()(_params) const _q -> decltype(_expr) \
		{ \
			return _expr; \
		} \
		YB_Impl_Functor_popf \
	};

// NOTE: Since ISO C++11, the standard does allow change to a stricter
//	noexcept-specification, see [res.on.exception.handling]. Also note WG21
//	P0509R1 accepted by WG21 N4664 to resolve GB 41 changes the order of
//	paragraphs, but not the relevant rule.
#define YB_Impl_Functor_Ops_Spec_Ptr(_n) \
	template<typename _type> \
	struct _n<_type*> \
	{ \
		using first_argument_type = _type*; \
		using second_argument_type = _type*; \
		using result_type = bool; \
		\
		YB_Impl_Functor_pushf \
		yconstfn bool \
		operator()(_type* x, _type* y) const yimpl(ynothrow) \
		{ \
			return std::_n<_type*>()(x, y); \
		} \
		YB_Impl_Functor_popf \
	};

#define YB_Impl_Functor_Ops_using(_type2) using second_argument_type = _type2;

#define YB_Impl_Functor_Ops1(_n, _op, _tRet) \
	YB_Impl_Functor_Ops_Primary(, _n, _tRet, , _op x, const _type& x) \
	\
	YB_Impl_Functor_Ops_Spec(, _n, typename _type, _type&& x, _op yforward(x))

#define YB_Impl_Functor_Ops2(_n, _op, _tRet) \
	YB_Impl_Functor_Ops_Primary(, _n, _tRet, YB_Impl_Functor_Ops_using(_type), \
		x _op y, const _type& x, const _type& y) \
	\
	YB_Impl_Functor_Ops_Spec(, _n, typename _type1 YPP_Comma typename \
		_type2, _type1&& x YPP_Comma _type2&& y, yforward(x) _op yforward(y))

#define YB_Impl_Functor_Binary(_n, _op) \
	YB_Impl_Functor_Ops2(_n, _op, _type)

#define YB_Impl_Functor_Unary(_n, _op) \
	YB_Impl_Functor_Ops1(_n, _op, _type)

//! \since build 679
inline namespace cpp2014
{

#if __cpp_lib_transparent_operators >= 201210L
using std::plus;
using std::minus;
using std::multiplies;
using std::divides;
using std::modulus;
using std::negate;

using std::equal_to;
using std::not_equal_to;
using std::greater;
//! \since build 680
using std::less;
using std::greater_equal;
using std::less_equal;

using std::logical_and;
using std::logical_or;
using std::logical_not;

using std::bit_and;
//! \since build 750
using std::bit_or;
using std::bit_xor;
using std::bit_not;
#else
/*!
\note 同 ISO WG21 N4296 对应标准库仿函数。
\since build 578
*/
//@{
//! \brief 加法仿函数。
YB_Impl_Functor_Binary(plus, +)

//! \brief 减法仿函数。
YB_Impl_Functor_Binary(minus, -)

//! \brief 乘法仿函数。
YB_Impl_Functor_Binary(multiplies, *)

/*!
\brief 除法仿函数。
\since build 679
*/
YB_Impl_Functor_Binary(divides, /)

//! \brief 模运算仿函数。
YB_Impl_Functor_Binary(modulus, %)

/*!
\brief 取反仿函数。
\since build 656
*/
YB_Impl_Functor_Unary(negate, -)

#define YB_Impl_Functor_bool(_n, _op) \
	YB_Impl_Functor_Ops2(_n, _op, bool)

#define YB_Impl_Functor_bool_Ordered(_n, _op) \
	YB_Impl_Functor_bool(_n, _op) \
	\
	YB_Impl_Functor_Ops_Spec_Ptr(_n)

#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Push
	YB_Diag_Ignore(float-equal)
#endif
//! \brief 等于关系仿函数。
YB_Impl_Functor_bool(equal_to, ==)

//! \brief 不等于关系仿函数。
YB_Impl_Functor_bool(not_equal_to, !=)
#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
	YB_Diag_Pop
#endif

//! \brief 大于关系仿函数。
YB_Impl_Functor_bool_Ordered(greater, >)

//! \brief 小于关系仿函数。
YB_Impl_Functor_bool_Ordered(less, <)

//! \brief 大于等于关系仿函数。
YB_Impl_Functor_bool_Ordered(greater_equal, >=)

//! \brief 小于等于关系仿函数。
YB_Impl_Functor_bool_Ordered(less_equal, <=)

//! \since build 656
//@{
//! \brief 逻辑与仿函数。
YB_Impl_Functor_bool(logical_and, &&)

//! \brief 逻辑或仿函数。
YB_Impl_Functor_bool(logical_or, ||)

//! \brief 逻辑非仿函数。
YB_Impl_Functor_Ops1(logical_not, !, bool)

//! \brief 位与仿函数。
YB_Impl_Functor_Binary(bit_and, &)

//! \brief 位或仿函数。
YB_Impl_Functor_Binary(bit_or, |)

//! \brief 位异或仿函数。
YB_Impl_Functor_Binary(bit_xor, ^)

//! \brief 位取反仿函数。
// NOTE: Available in %std since ISO C++14.
YB_Impl_Functor_Unary(bit_not, ~)
//@}
//@}
#endif

} // inline namespace cpp2014;

/*!
\ingroup YBase_replacement_extensions
\since build 668
*/
//@{
//! \brief 一元 & 操作。
YB_Impl_Functor_Ops1(addrof, &, addrof_t<const _type&>)

//! \brief 一元 * 操作。
YB_Impl_Functor_Ops1(indirect, *, indirect_t<const _type&>)

//! \brief 引用等价关系仿函数。
//@{
YB_Impl_Functor_Ops_Primary(ynothrow, ref_eq, bool,
	YB_Impl_Functor_Ops_using(_type), ystdex::addressof(x)
	== ystdex::addressof(y), const _type& x, const _type& y)

///! \since build 824
YB_Impl_Functor_Ops_Spec(ynothrow, ref_eq,
	typename _type1 YPP_Comma typename _type2, _type1&& x YPP_Comma _type2&& y,
	ystdex::addressof(yforward(x)) == ystdex::addressof(yforward(y)))
//@}

//! \since build 830
//@{
YB_Impl_Functor_Ops_Primary(ynothrow, first_of, first_t<_type&>, , x.first,
	_type& x)

// NOTE: More parentheses are needed to keep the correct value category.
YB_Impl_Functor_Ops_Spec(ynothrow, first_of, typename _type, _type&& x,
	yforward((x.first)))

YB_Impl_Functor_Ops_Primary(ynothrow, second_of, second_t<_type&>, , x.second,
	_type& x)

// NOTE: More parentheses are needed to keep the correct value category.
YB_Impl_Functor_Ops_Spec(ynothrow, second_of, typename _type, _type&& x,
	yforward((x.second)))
//@}
//@}

#undef YB_Impl_Functor_bool_Ordered
#undef YB_Impl_Functor_bool
#undef YB_Impl_Functor_Unary
#undef YB_Impl_Functor_Binary
#undef YB_Impl_Functor_Ops2
#undef YB_Impl_Functor_Ops1
#undef YB_Impl_Functor_Ops_using
#undef YB_Impl_Functor_Ops_Spec
#undef YB_Impl_Functor_Ops_Primary
#undef YB_Impl_Functor_popf
#undef YB_Impl_Functor_pushf


/*!
\brief 合并值序列。
\note 语义同 Boost.Signal2 的 \c boost::last_value 但对非 \c void 类型使用默认值。
\since build 675
*/
//@{
template<typename _type>
struct default_last_value
{
	template<typename _tIn>
	_type
	operator()(_tIn first, _tIn last, const _type& val = {}) const
	{
		return std::accumulate(first, last, val,
			[](const _type&, decltype(*first) v){
			return yforward(v);
		});
	}
};

template<>
struct default_last_value<void>
{
	template<typename _tIn>
	void
	operator()(_tIn first, _tIn last) const
	{
		for(; first != last; ++first)
			*first;
	}
};
//@}


//! \brief 选择自增/自减运算仿函数。
//@{
template<bool, typename _tScalar>
struct xcrease_t
{
	//! \since build 537
	inline _tScalar&
	operator()(_tScalar& _x) const
	{
		return ++_x;
	}
};
template<typename _tScalar>
struct xcrease_t<false, _tScalar>
{
	//! \since build 537
	inline _tScalar&
	operator()(_tScalar& _x) const
	{
		return --_x;
	}
};
//@}


/*!
\brief 选择加法/减法复合赋值运算仿函数。
\since build 284
*/
//@{
template<bool, typename _tScalar1, typename _tScalar2>
struct delta_assignment
{
	yconstfn _tScalar1&
	operator()(_tScalar1& x, _tScalar2 y) const
	{
		return x += y;
	}
};
template<typename _tScalar1, typename _tScalar2>
struct delta_assignment<false, _tScalar1, _tScalar2>
{
	yconstfn _tScalar1&
	operator()(_tScalar1& x, _tScalar2 y) const
	{
		return x -= y;
	}
};
//@}


//! \brief 选择自增/自减运算。
template<bool _bIsPositive, typename _tScalar>
yconstfn _tScalar&
xcrease(_tScalar& _x)
{
	return xcrease_t<_bIsPositive, _tScalar>()(_x);
}

/*!
\brief 选择加法/减法复合赋值运算。
\since build 284
*/
template<bool _bIsPositive, typename _tScalar1, typename _tScalar2>
yconstfn _tScalar1&
delta_assign(_tScalar1& _x, _tScalar2& _y)
{
	return delta_assignment<_bIsPositive, _tScalar1, _tScalar2>()(_x, _y);
}
//@}

} // namespace ystdex;

#endif

