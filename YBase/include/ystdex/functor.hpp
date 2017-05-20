/*
	© 2010-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file functor.hpp
\ingroup YStandardEx
\brief 通用仿函数。
\version r851
\author FrankHB <frankhb1989@gmail.com>
\since build 588
\par 创建时间:
	2015-03-29 00:35:44 +0800
\par 修改时间:
	2017-05-18 09:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Functor

扩展标准库头 <functional> ，提供若干函数对象。
*/


#ifndef YB_INC_ystdex_functor_hpp_
#define YB_INC_ystdex_functor_hpp_ 1

#include "ref.hpp" // for enable_if_t, is_detected, ystdex::constfn_addressof,
//	not_, or_, is_reference_wrapper, and_, std::greater, std::less,
//	std::greater_equal, std::less_equal, addrof_t, indirect_t;
#include <string> // for std::char_traits;
#include <algorithm> // for std::lexicographical_compare;

#if YB_IMPL_MSCPP >= 1900
/*!
\brief \<algorithm\> 特性测试宏。
\see WG21 P0096R1 3.5 。
\since build 628
*/
#	ifndef __cpp_lib_robust_nonmodifying_seq_ops
#		define __cpp_lib_robust_nonmodifying_seq_ops 201304
#	endif
#endif

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
\since build 243
*/
//@{
/*!
\brief 恒等仿函数。
\since build 689
*/
//@{
template<typename _type = void>
struct id
{
	yconstfn _type
	operator()(const _type& x) const ynothrow
	{
		return x;
	}

	//! \since build 756
	yconstfn
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
	template<typename _type>
	yconstfn _type
	operator()(_type&& x) const ynothrow
	{
		return yforward(x);
	}

	//! \since build 756
	template<typename _type>
	yconstfn
	operator add_ptr_t<_type(_type)>() const ynothrow
	{
		return [](_type x) ynothrow -> _type{
			return x;
		};
	}
};
//@}


/*!
\brief std::addressof 仿函数。
\since build 660
*/
//@{
template<typename _type = void>
struct addressof_op
{
	yconstfn _type*
	operator()(_type& x) const ynothrow
	{
		return ystdex::constfn_addressof(x);
	}
};

template<>
struct addressof_op<void>
{
	template<typename _type>
	yconstfn _type*
	operator()(_type& x) const ynothrow
	{
		return ystdex::constfn_addressof(x);
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
	operator()(_type&& x) const ynoexcept_spec(std::declval<_type&&>().get())
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
	yconstfn yimpl(enable_if_t)<not_<or_<is_reference_wrapper<_type1>,
		is_reference_wrapper<_type2>>>::value, bool>
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
		return std::addressof(x.get()) == std::addressof(y);
	}
	template<typename _type1, typename _type2>
	yconstfn yimpl(enable_if_t)<and_<not_<is_reference_wrapper<_type1>>,
		is_reference_wrapper<_type2>>::value, bool>
	operator()(const _type1& x, const _type2& y) const ynothrow
	{
		return std::addressof(x) == std::addressof(y.get());
	}
	template<typename _type1, typename _type2>
	yconstfn yimpl(enable_if_t)<and_<is_reference_wrapper<_type1>,
		is_reference_wrapper<_type2>>::value, bool>
	operator()(const _type1& x, const _type2& y) const ynothrow
	{
		return std::addressof(x.get()) == std::addressof(y.get());
	}
	//@}
};

//! \brief 引用相等关系仿函数。
template<typename _type>
struct ref_eq
{
	yconstfn bool
	operator()(const _type& _x, const _type& _y) const
	{
		return &_x == &_y;
	}
};

// NOTE: There is 'constexpr' in WG21 N3936; but it is in ISO/IEC 14882:2014.
#define YB_Impl_Functor_Ops_Primary(_n, _tRet, _using_stmt, _expr, ...) \
	template<typename _type = void> \
	struct _n \
	{ \
		using first_argument_type = _type; \
		_using_stmt \
		using result_type = _tRet; \
		\
		yconstfn _tRet \
		operator()(__VA_ARGS__) const \
		{ \
			return _expr; \
		} \
	};

#define YB_Impl_Functor_Ops_Spec(_n, _tparams, _params, _expr) \
	template<> \
	struct _n<void> \
	{ \
		using is_transparent = yimpl(void); \
		\
		template<_tparams> \
		yconstfn auto \
		operator()(_params) const -> decltype(_expr) \
		{ \
			return _expr; \
		} \
	};

#define YB_Impl_Functor_Ops_Spec_Ptr(_n) \
	template<typename _type> \
	struct _n<_type*> \
	{ \
		using first_argument_type = _type*; \
		using second_argument_type = _type*; \
		using result_type = bool; \
		\
		yconstfn bool \
		operator()(_type* x, _type* y) const \
		{ \
			return std::_n<_type*>()(x, y); \
		} \
	};

#define YB_Impl_Functor_Ops_using(_tRet) using second_argument_type = _tRet;

#define YB_Impl_Functor_Ops1(_n, _op, _tRet) \
	YB_Impl_Functor_Ops_Primary(_n, _tRet, , _op x, const _type& x) \
	\
	YB_Impl_Functor_Ops_Spec(_n, typename _type, _type&& x, _op yforward(x))

#define YB_Impl_Functor_Ops2(_n, _op, _tRet) \
	YB_Impl_Functor_Ops_Primary(_n, _tRet, \
		YB_Impl_Functor_Ops_using(_tRet), x _op y, const _type& x, \
		const _type& y) \
	\
	YB_Impl_Functor_Ops_Spec(_n, typename _type1 YPP_Comma typename \
		_type2, _type1&& x YPP_Comma _type2&& y, yforward(x) _op yforward(y))

#define YB_Impl_Functor_Binary(_n, _op) \
	YB_Impl_Functor_Ops2(_n, _op, _type)

#define YB_Impl_Functor_Unary(_n, _op) \
	YB_Impl_Functor_Ops1(_n, _op, _type)

//! \since build 679
inline namespace cpp2014
{

#if __cpp_lib_transparent_operators >= 201210 || __cplusplus >= 201402L
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

//! \brief 等于关系仿函数。
YB_Impl_Functor_bool(equal_to, ==)

//! \brief 不等于关系仿函数。
YB_Impl_Functor_bool(not_equal_to, !=)

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
\note YStandardEx 扩展。
\since build 668
*/
//@{
//! \brief 一元 & 操作。
YB_Impl_Functor_Ops1(addrof, &, addrof_t<const _type&>)

//! \brief 一元 * 操作。
YB_Impl_Functor_Ops1(indirect, *, indirect_t<const _type&>)
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

