/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file functor.hpp
\ingroup YStandardEx
\brief 通用仿函数。
\version r422
\author FrankHB <frankhb1989@gmail.com>
\since build 588
\par 创建时间:
	2015-03-29 00:35:44 +0800
\par 修改时间:
	2015-07-02 06:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Functor
*/


#ifndef YB_INC_ystdex_functor_hpp_
#define YB_INC_ystdex_functor_hpp_ 1

#include "ref.hpp" // for std::addressof, enable_if_t, wrapped_traits;
#include <string> // for std::char_traits;
#include <algorithm> // for std::lexicographical_compare;

namespace ystdex
{

/*!	\defgroup functors General Functors
\brief 仿函数。
\note 函数对象包含函数指针和仿函数。
\since build 243
*/
//@{
/*!
\brief std::addressof 仿函数。
\since build 537
*/
template<typename _type>
struct addressof_op
{
	_type*
	operator()(_type& x) const ynothrow
	{
		return std::addressof(x);
	}
};

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
	operator()(_type&& x) const ynoexcept(std::declval<_type&&>().get())
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
	//! \since build 554
	//@{
	//! \since build 594
	template<typename _type1, typename _type2>
	yconstfn yimpl(enable_if_t)<!wrapped_traits<_type1>::value
		&& !wrapped_traits<_type2>::value, bool>
	operator()(const _type1& x, const _type2& y) const ynoexcept_spec(x == y)
	{
		return x == y;
	}
	template<typename _type1, typename _type2>
	yconstfn yimpl(enable_if_t)<wrapped_traits<_type1>::value
		&& !wrapped_traits<_type2>::value, bool>
	operator()(const _type1& x, const _type2& y) const ynothrow
	{
		return std::addressof(x.get()) == std::addressof(y);
	}
	template<typename _type1, typename _type2>
	yconstfn yimpl(enable_if_t)<!wrapped_traits<_type1>::value
		&& wrapped_traits<_type2>::value, bool>
	operator()(const _type1& x, const _type2& y) const ynothrow
	{
		return std::addressof(x) == std::addressof(y.get());
	}
	template<typename _type1, typename _type2>
	yconstfn yimpl(enable_if_t)<wrapped_traits<_type1>::value
		&& wrapped_traits<_type2>::value, bool>
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

/*!
\note 同 ISO WG21/N4296 对应标准库仿函数。
\since build 578
*/
//@{
#define YB_Impl_Functional_Ops2(_n, _op, _tRet) \
	template<typename _type = void> \
	struct _n \
	{ \
		using first_argument_type = _type; \
		using second_argument_type = _type; \
		using result_type = _tRet; \
		\
		yconstfn bool \
		operator()(const _type& x, const _type& y) const \
		{ \
			return x _op y; \
		} \
	}; \
	\
	template<> \
	struct _n<void> \
	{ \
		using is_transparent = yimpl(void); \
		\
		template<typename _type1, typename _type2> \
		auto operator()(_type1&& x, _type2&& y) const \
			-> decltype(yforward(x) < yforward(y)) \
		{ \
			return yforward(x) < yforward(y); \
		} \
	};

#define YB_Impl_Functional_Arithmetic(_n, _op) \
	YB_Impl_Functional_Ops2(_n, _op, _type)

//! \brief 加法仿函数。
YB_Impl_Functional_Arithmetic(plus, +)

//! \brief 减法仿函数。
YB_Impl_Functional_Arithmetic(minus, -)

//! \brief 乘法仿函数。
YB_Impl_Functional_Arithmetic(multiplies, *)

//! \brief 除法仿函数。
YB_Impl_Functional_Arithmetic(devides, /)

//! \brief 模运算仿函数。
YB_Impl_Functional_Arithmetic(modulus, %)

#undef YB_Impl_Functional_Arithmetic

#define YB_Impl_Functional_Comparison(_n, _op) \
	YB_Impl_Functional_Ops2(_n, _op, bool)

//! \brief 等于关系仿函数。
YB_Impl_Functional_Comparison(equal_to, ==)

//! \brief 不等于关系仿函数。
YB_Impl_Functional_Comparison(not_equal_to, !=)

//! \brief 大于关系仿函数。
YB_Impl_Functional_Comparison(greater, >)

//! \brief 小于关系仿函数。
YB_Impl_Functional_Comparison(less, <)

//! \brief 大于等于关系仿函数。
YB_Impl_Functional_Comparison(greater_equal, >=)

//! \brief 小于等于关系仿函数。
YB_Impl_Functional_Comparison(less_equal, <=)

#undef YB_Impl_Functional_Comparison

#undef YB_Impl_Functional_Ops2
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


//! \brief 引用仿函数。
template<typename _type>
struct deref_op
{
	/*!
	\brief 对指定对象使用 operator& 并返回结果。
	*/
	yconstfn _type*
	operator()(_type& _x) const
	{
		return &_x;
	}
};


//! \brief const 引用仿函数。
template<typename _type>
struct const_deref_op
{
	/*!
	\brief 对指定 const 对象使用 operator& 并返回结果。
	*/
	inline const _type*
	operator()(const _type& _x) const
	{
		return &_x;
	}
};


/*!
\brief 间接访问比较仿函数。
\warning 非虚析构。
\since build 315
*/
template<typename _type, typename _tPointer = _type*,
	class _fCompare = std::less<_type>>
struct deref_comp
{
	/*
	\brief 比较指针指向的对象。
	\return 若参数有空指针则 false ，否则判断是否满足 _fCompare()(*_x, *_y) 。
	*/
	bool
	operator()(const _tPointer& _x, const _tPointer& _y) const
	{
		return bool(_x) && bool(_y) && _fCompare()(*_x, *_y);
	}
};


/*!
\brief 间接访问字符串比较仿函数。
\warning 非虚析构。
\since build 315
*/
template<typename _tChar, class _fCompare = std::less<_tChar>>
struct deref_str_comp
{
	/*!
	\brief 比较指定字符串首字符的指针。
	\return 若参数有空指针则 false ，否则判断指定字符串是否满足字典序严格偏序关系。
	*/
	bool
	operator()(const _tChar* x, const _tChar* y) const
	{
		using traits_type = std::char_traits<_tChar>;

		return x && y && std::lexicographical_compare(x, x + traits_type
			::length(x), y, y + traits_type::length(y), _fCompare());
	}
};
//@}

} // namespace ystdex;

#endif

