/*
	© 2012-2013, 2015-2016, 2018-2019, 2021-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file integer_sequence.hpp
\ingroup YStandardEx
\brief 整数序列元编程接口。
\version r634
\author FrankHB <frankhb1989@gmail.com>
\since build 589
\par 创建时间:
	2013-03-30 00:55:06 +0800
\par 修改时间:
	2022-02-05 19:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::IntegerSequence
*/


#ifndef YB_INC_ystdex_sequence_hpp_
#define YB_INC_ystdex_sequence_hpp_ 1

#include "variadic.hpp" // for "variadic.hpp", __cpp_lib_integer_sequence,
//	std::integer_sequence, std::index_sequence, is_integral, size_t, empty_base,
//	vseq::defer_i, _t, common_type_t;

/*!
\brief \c \<utility> 特性测试宏。
\see ISO C++20 [version.syn] 。
\see WG21 P0941R2 2.2 。
\see https://docs.microsoft.com/cpp/visual-cpp-language-conformance 。
\since build 628
*/
#ifndef __cpp_lib_integer_sequence
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201304L
#		define __cpp_lib_integer_sequence 201304L
#	endif
#endif

namespace ystdex
{

inline namespace cpp2014
{

/*!
\ingroup meta_types
\see ISO C++14 20.5 [intseq] 。
\since build 589
*/
//@{
#if __cpp_lib_integer_sequence >= 201304L
using std::integer_sequence;
using std::index_sequence;
#else
/*!
\ingroup YBase_replacement_features
\tparam _tInt 序列元素类型。
\pre 静态断言：_tInt 是整数类型。
*/
template<typename _tInt, _tInt... _vSeq>
struct integer_sequence
{
	//! \since build 938
	yimpl(static_assert)(is_integral<_tInt>::value,
		"Invalid 1st template parameter type found.");
	using value_type = _tInt;

	static yconstfn size_t
	size() ynoexcept
	{
		return sizeof...(_vSeq);
	}
};

//! \ingroup YBase_replacement_features
template<size_t... _vSeq>
using index_sequence = integer_sequence<size_t, _vSeq...>;
#endif
//@}

} // inline namespace cpp2014;


//! \since build 728
//@{
namespace fseq
{

/*!
\ingroup fseq_operations
\since build 728
*/
//@{
template<typename _tInt, _tInt _vInt, _tInt... _vSeq>
yconstfn std::integral_constant<_tInt, _vInt>
front(integer_sequence<_tInt, _vInt, _vSeq...>) ynothrow
{
	return {};
}

template<typename _tInt, _tInt _vInt, _tInt... _vSeq>
yconstfn integer_sequence<_tInt, _vSeq...>
pop_front(integer_sequence<_tInt, _vInt, _vSeq...>) ynothrow
{
	return {};
}

template<typename _func, typename _tState, typename _tInt>
yconstfn _tState
fold(_func, _tState s, integer_sequence<_tInt>) ynothrow
{
	return s;
}
template<typename _func, typename _tState, typename _tInt, _tInt _vInt,
	_tInt... _vSeq>
yconstfn auto
fold(_func f, _tState s, integer_sequence<_tInt, _vInt, _vSeq...> xs) ynothrow
	-> decltype(fold(f, f(s, front(xs)), pop_front(xs)))
{
	return fold(f, f(s, front(xs)), pop_front(xs));
}


struct plus
{
	//! \since build 730
	template<typename _tInt, _tInt _v1, _tInt _v2>
	yconstfn auto
	operator()(std::integral_constant<_tInt, _v1>, std::integral_constant<_tInt,
		_v2>) const ynothrow -> std::integral_constant<_tInt, _v1 + _v2>
	{
		return {};
	}
};


struct minus
{
	//! \since build 730
	template<typename _tInt, _tInt _v1, _tInt _v2>
	yconstfn auto
	operator()(std::integral_constant<_tInt, _v1>, std::integral_constant<_tInt,
		_v2>) const ynothrow -> std::integral_constant<_tInt, _v1 - _v2>
	{
		return {};
	}
};
//@}

} // namespace fseq;
//@}

//! \since build 589
//@{
namespace vseq
{

//! \since build 684
//@{
template<class _tSeq, size_t... _vIdxSeq>
struct project<_tSeq, index_sequence<_vIdxSeq...>, enable_for_instances<_tSeq>>
{
	using type = instance_apply_t<_tSeq,
		empty_base<at_t<params_of_t<_tSeq>, _vIdxSeq>...>>;
};


//! \since build 688
template<typename _tInt>
struct bound_integer_sequence
{
	template<typename... _types>
	using apply = identity<integer_sequence<_tInt, _types::value...>>;
};


template<typename _tInt, _tInt... _vSeq>
struct ctor_of<integer_sequence<_tInt, _vSeq...>>
{
	using type = bound_integer_sequence<_tInt>;
};


template<typename _tInt, _tInt... _vSeq>
struct params_of<integer_sequence<_tInt, _vSeq...>>
{
	using type = empty_base<integral_constant<_tInt, _vSeq>...>;
};
//@}


template<typename _tInt, _tInt... _vSeq1, _tInt... _vSeq2>
struct vec_add<integer_sequence<_tInt, _vSeq1...>,
	integer_sequence<_tInt, _vSeq2...>>
{
	using type = integer_sequence<_tInt, (_vSeq1 + _vSeq2)...>;
};


template<typename _tInt, _tInt... _vSeq1, _tInt... _vSeq2>
struct vec_subtract<integer_sequence<_tInt, _vSeq1...>,
	integer_sequence<_tInt, _vSeq2...>>
{
	using type = integer_sequence<_tInt, (_vSeq1 - _vSeq2)...>;
};


/*!
\brief 可变参数整数列表延迟求值。
\sa defer_i
\since build 650
*/
template<typename _tInt, template<_tInt...> class _gOp, _tInt... _vSeq>
struct vdefer_i
	: vseq::defer_i<_tInt, _gOp, integer_sequence<_tInt, _vSeq...>>
{};

} // namespace vseq;
//@}

//! \since build 684
using vseq::vdefer_i;

/*!
\ingroup metafunctions
\since build 590
*/
//@{
/*!
\brief 取整数序列的自然数后继。
\tparam _tInt 序列元素类型。
\pre 静态断言：_tInt 是整数类型。
*/
//@{
template<typename _tInt, _tInt, class>
struct make_successor;

template<typename _tInt, _tInt _vBase, class _tSeq>
using make_successor_t = _t<make_successor<_tInt, _vBase, _tSeq>>;

template<typename _tInt, _tInt _vBase, _tInt... _vSeq>
struct make_successor<_tInt, _vBase, integer_sequence<_tInt, _vSeq...>>
{
private:
	//! \since build 938
	static_assert(is_integral<_tInt>::value,
		"Invalid 1st template parameter type found.");
	using common_t = common_type_t<size_t, _tInt>;

public:
	// XXX: Conversion to '_tInt' might be implementation-defined.
	using type
		= integer_sequence<_tInt, _vSeq..., static_cast<_tInt>(static_cast<
		common_t>(_vBase) + static_cast<common_t>(sizeof...(_vSeq)))>;
};
//@}


//! \brief 取皮亚诺公理决定的整数序列。
//@{
template<typename _tInt, _tInt, size_t>
struct make_peano_sequence;

//! \since build 590
template<typename _tInt, _tInt _vBase, size_t _vN>
using make_peano_sequence_t
	= _t<make_peano_sequence<_tInt, _vBase, _vN>>;

template<typename _tInt, _tInt _vBase, size_t _vN>
struct make_peano_sequence
{
	using type = make_successor_t<_tInt, _vBase,
		make_peano_sequence_t<_tInt, _vBase, _vN - 1>>;
};

template<typename _tInt, _tInt _vBase>
struct make_peano_sequence<_tInt, _vBase, 0>
{
	using type = integer_sequence<_tInt>;
};
//@}


//! \brief 取自然数序列。
//@{
template<typename _tInt, size_t _vN>
using make_natural_sequence = make_peano_sequence<_tInt, 0, _vN>;

template<typename _tInt, size_t _vN>
using make_natural_sequence_t
	= _t<make_natural_sequence<_tInt, _vN>>;
//@}

/*!
\brief 判断是否具有相同的指定值。
\since build 651
*/
template<typename _tInt, _tInt _vDefault, _tInt... _values>
using same_value_as = is_same<integer_sequence<_tInt, _vDefault,
	_values...>, integer_sequence<_tInt, _values..., _vDefault>>;
//@}


inline namespace cpp2014
{

/*!
\ingroup metafunctions
\see ISO C++14 20.5 [intseq] 。
\since build 589
*/
//@{
#if __cpp_lib_integer_sequence >= 201304L
using std::make_integer_sequence;
using std::make_index_sequence;

using std::index_sequence_for;
#else
template<typename _tInt, size_t _vN>
using make_integer_sequence = make_natural_sequence_t<_tInt, _vN>;

template<size_t _vN>
using make_index_sequence = make_integer_sequence<size_t, _vN>;


template<typename... _types>
using index_sequence_for = make_index_sequence<sizeof...(_types)>;
#endif
//@}

} // inline namespace cpp2014;

} // namespace ystdex;

#endif

