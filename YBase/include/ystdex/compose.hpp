/*
	© 2012, 2014-2016, 2018, 2021-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file compose.hpp
\ingroup YStandardEx
\brief 函数复合。
\version r5167
\author FrankHB <frankhb1989@gmail.com>
\since build 939
\par 创建时间:
	2022-02-13 09:10:55 +0800
\par 修改时间:
	2022-02-14 22:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Compose
*/


#ifndef YB_INC_ystdex_compose_hpp_
#define YB_INC_ystdex_compose_hpp_ 1

#include "functor.hpp" // for internal "functor.hpp", and_, addressof_op, less,
//	mem_get;
#include "placement.hpp" // for is_bitwise_swappable;
#include <tuple> // for std::tuple;
#include "integer_sequence.hpp" // for index_sequence, index_sequence_for;

namespace ystdex
{

/*!
\ingroup functors
\brief 复合函数。
\since build 537
*/
template<typename _func, typename _func2>
struct composed
{
	_func f;
	_func2 g;

	/*!
	\note 每个函数只在函数调用表达式中出现一次。
	\since build 735
	*/
	template<typename... _tParams>
	yconstfn auto
	operator()(_tParams&&... args) const ynoexcept_spec(f(g(yforward(args)...)))
		-> decltype(f(g(yforward(args)...)))
	{
		return f(g(yforward(args)...));
	}
};

//! \relates composed
//@{
/*!
\brief 函数复合。
\note 最后一个参数最先被调用，可以为多元函数；其它被复合的函数需要保证有一个参数。
\return 复合的可调用对象。
\since build 537
*/
//@{
template<typename _func, typename _func2>
YB_ATTR_nodiscard YB_PURE yconstfn composed<_func, _func2>
compose(_func f, _func2 g)
{
	return composed<_func, _func2>{f, g};
}
//! \since build 731
template<typename _func, typename _func2, typename _func3, typename... _funcs>
YB_ATTR_nodiscard YB_PURE yconstfn auto
compose(_func f, _func2 g, _func3 h, _funcs... args)
	-> decltype(ystdex::compose(ystdex::compose(f, g), h, args...))
{
	return ystdex::compose(ystdex::compose(f, g), h, args...);
}
//@}

//! \since build 927
template<typename _func, typename _func2>
struct is_bitwise_swappable<composed<_func, _func2>>
	: and_<is_bitwise_swappable<_func>, is_bitwise_swappable<_func2>>
{};
//@}


/*!
\ingroup functors
\brief 多元分发的复合函数。
\since build 735
*/
template<typename _func, typename _func2>
struct composed_n
{
	_func f;
	_func2 g;

	//! \note 第二函数会被分发：多次出现在函数调用表达式中。
	template<typename... _tParams>
	yconstfn auto
	operator()(_tParams&&... args) const ynoexcept_spec(f(g(yforward(args))...))
		-> decltype(f(g(yforward(args))...))
	{
		return f(g(yforward(args))...);
	}
};

//! \relates composed_n
//@{
/*!
\brief 单一分派的多元函数复合。
\note 第一参数最后被调用，可以为多元函数；其它被复合的函数需要保证有一个参数。
\return 单一分派的多元复合的可调用对象。
\since build 735
*/
//@{
//! \since build 740
template<typename _func, typename _func2>
yconstfn composed_n<_func, _func2>
compose_n(_func f, _func2 g)
{
	return composed_n<_func, _func2>{f, g};
}
template<typename _func, typename _func2, typename _func3, typename... _funcs>
yconstfn auto
compose_n(_func f, _func2 g, _func3 h, _funcs... args)
	-> decltype(ystdex::compose_n(ystdex::compose_n(f, g), h, args...))
{
	return ystdex::compose_n(ystdex::compose_n(f, g), h, args...);
}
//@}

//! \since build 927
template<typename _func, typename _func2>
struct is_bitwise_swappable<composed_n<_func, _func2>>
	: and_<is_bitwise_swappable<_func>, is_bitwise_swappable<_func2>>
{};
//@}


/*!
\ingroup functors
\brief 多元复合函数。
\since build 735
*/
template<typename _func, typename... _funcs>
struct generalized_composed
{
	_func f;
	std::tuple<_funcs...> g;

	template<typename... _tParams>
	yconstfn auto
	operator()(_tParams&&... args) const ynoexcept_spec(yimpl(call(
		index_sequence_for<_tParams...>(), yforward(args)...))) -> decltype(
		yimpl(call(index_sequence_for<_tParams...>(), yforward(args)...)))
	{
		return call(index_sequence_for<_tParams...>(), yforward(args)...);
	}

private:
	template<size_t... _vSeq, typename... _tParams>
	yconstfn auto
	call(index_sequence<_vSeq...>, _tParams&&... args) const
		ynoexcept_spec(f(std::get<_vSeq>(g)(yforward(args))...))
		-> decltype(f(std::get<_vSeq>(g)(yforward(args))...))
	{
		return f(std::get<_vSeq>(g)(yforward(args))...);
	}
};

//! \relates generalized_composed
//@{
/*!
\brief 多元函数复合。
\return 以多元函数复合的可调用对象。
\note 使用 ADL make_tuple 。
\since build 735
*/
template<typename _func, typename... _funcs>
yconstfn generalized_composed<_func, std::tuple<_funcs...>>
generalized_compose(_func f, _funcs... args)
{
	return generalized_composed<_func,
		std::tuple<_funcs...>>{f, make_tuple(args...)};
}

//! \since build 927
template<typename _func, typename... _funcs>
struct is_bitwise_swappable<generalized_composed<_func, _funcs...>>
	: and_<is_bitwise_swappable<_func>, is_bitwise_swappable<_funcs>...>
{};
//@}


/*!
\ingroup functors
\since build 824
*/
//@{
//! \brief get 成员等于仿函数。
//@{
template<typename _type = void>
struct get_equal_to
	: composed_n<equal_to<_type*>, composed<addressof_op<_type>, mem_get<>>>
{};

template<>
struct get_equal_to<void>
	: composed_n<equal_to<>, composed<addressof_op<>, mem_get<>>>
{};
//@}

//! \brief get 成员小于仿函数。
//@{
template<typename _type = void>
struct get_less
	: composed_n<less<_type*>, composed<addressof_op<_type>, mem_get<>>>
{};

template<>
struct get_less<void>
	: composed_n<less<>, composed<addressof_op<>, mem_get<>>>
{};
//@}
//@}

} // namespace ystdex;

#endif

