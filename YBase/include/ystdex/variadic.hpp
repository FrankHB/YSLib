/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file variadic.hpp
\ingroup YStandardEx
\brief C++ 变长参数相关操作。
\version r346
\author FrankHB <frankhb1989@gmail.com>
\since build 412
\par 创建时间:
	2013-06-06 11:38:15 +0800
\par 修改时间:
	2015-01-31 02:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Variadic
*/


#ifndef YB_INC_ystdex_variadic_hpp_
#define YB_INC_ystdex_variadic_hpp_ 1

#include "../ydef.h" // for ystdex::size_t;

namespace ystdex
{

/*!
\ingroup meta_types
\brief 变长参数标记的整数序列。
\since build 303
*/
template<size_t... _vSeq>
struct variadic_sequence
{
	/*!
	\brief 取序列长度。
	\note 同 C++1y std::integer_sequence::size 。
	\since build 447
	*/
	static yconstfn size_t
	size()
	{
		return sizeof...(_vSeq);
	}
};


/*!	\defgroup vseq_operations Variadic Sequence Operations
\ingroup meta_operations
\brief 变长参数标记的序列相关的元操作。
\since build 447

形式为模板类名声明和特化的相关操作，被操作的序列是类类型。
除此处对 ystdex::variadic_sequence 外的特化外，可有其它类类型的特化。
特化至少需保证具有表示和此处特化意义相同的 type 类型成员。
对非类型元素，成员 value 表示结果，成员 type 表示对应的序列类型。
*/


/*!
\ingroup vseq_operations
\brief 拆分序列。
\since build 447

拆分序列为首元素和其余部分。
*/
//@{
template<class>
struct sequence_split;

template<class _tSeq>
using sequence_split_t = typename sequence_split<_tSeq>::type;

template<size_t _vHead, size_t... _vTail>
struct sequence_split<variadic_sequence<_vHead, _vTail...>>
{
	static yconstexpr size_t value = _vHead;

	using type = variadic_sequence<value>;
	using tail = variadic_sequence<_vTail...>;
};
//@}


/*!
\ingroup vseq_operations
\brief 合并序列。
\since build 447
*/
//@{
template<class, class>
struct sequence_cat;

template<class _tSeq1, class _tSeq2>
using sequence_cat_t = typename sequence_cat<_tSeq1, _tSeq2>::type;

template<size_t... _vSeq1, size_t... _vSeq2>
struct sequence_cat<variadic_sequence<_vSeq1...>, variadic_sequence<_vSeq2...>>
{
	using type = variadic_sequence<_vSeq1..., _vSeq2...>;
};
//@}


/*!
\ingroup vseq_operations
\brief 取序列元素。
\since build 447
*/
//@{
template<size_t, class>
struct sequence_element;

template<size_t _vIdx, class _tSeq>
using sequence_element_t = typename sequence_element<_vIdx, _tSeq>::type;

template<size_t _vIdx>
struct sequence_element<_vIdx, variadic_sequence<>>;

template<size_t... _vSeq>
struct sequence_element<0, variadic_sequence<_vSeq...>>
{
private:
	using vseq = variadic_sequence<_vSeq...>;

public:
	static yconstexpr auto value = sequence_split<vseq>::value;

	using type = sequence_split_t<vseq>;
};

template<size_t _vIdx, size_t... _vSeq>
struct sequence_element<_vIdx, variadic_sequence<_vSeq...>>
{
private:
	using sub = sequence_element<_vIdx - 1,
		typename sequence_split<variadic_sequence<_vSeq...>>::tail>;

public:
	static yconstexpr auto value = sub::value;
	using type = typename sub::type;
};
//@}


/*!
\ingroup vseq_operations
\brief 投影操作。
\since build 447
*/
//@{
template<class, class>
struct sequence_project;

template<class _tSeq, class _tIdxSeq>
using sequence_project_t = typename sequence_project<_tSeq, _tIdxSeq>::type;

template<size_t... _vSeq, size_t... _vIdxSeq>
struct sequence_project<variadic_sequence<_vSeq...>,
	variadic_sequence<_vIdxSeq...>>
{
	using type = variadic_sequence<
		sequence_element<_vIdxSeq, variadic_sequence<_vSeq...>>::value...>;
};
//@}


/*!
\ingroup vseq_operations
\brief 取逆序列。
\since build 447
*/
//@{
template<class>
struct sequence_reverse;

template<class _tSeq>
using sequence_reverse_t = typename sequence_reverse<_tSeq>::type;

template<>
struct sequence_reverse<variadic_sequence<>>
{
	using type = variadic_sequence<>;
};

template<size_t... _vSeq>
struct sequence_reverse<variadic_sequence<_vSeq...>>
{
private:
	using vseq = variadic_sequence<_vSeq...>;

public:
	using type = sequence_cat_t<sequence_reverse_t<typename
		sequence_split<vseq>::tail>, sequence_split_t<vseq>>;
};
//@}


/*!
\ingroup vseq_operations
\brief 重复连接序列元素。
\since build 572
*/
//@{
template<size_t, class>
struct sequence_join_n;

template<size_t _vN, class _tSeq>
using sequence_join_n_t = typename sequence_join_n<_vN, _tSeq>::type;

template<size_t... _vSeq>
struct sequence_join_n<0, variadic_sequence<_vSeq...>>
{
	using type = variadic_sequence<>;
};

template<size_t... _vSeq>
struct sequence_join_n<1, variadic_sequence<_vSeq...>>
{
	using type = variadic_sequence<_vSeq...>;
};

template<size_t _vN, size_t... _vSeq>
struct sequence_join_n<_vN, variadic_sequence<_vSeq...>>
{
private:
	using unit = variadic_sequence<_vSeq...>;
	using half = sequence_join_n_t<_vN / 2, unit>;

public:
	using type = sequence_cat_t<sequence_cat_t<half, half>,
		sequence_join_n_t<_vN % 2, unit>>;
};
//@}


/*!
\ingroup vseq_operations
\brief 拆分序列前若干元素。
\note 使用二分实现减少递归实例化深度。
\since build 507
*/
//@{
template<size_t, class>
struct sequence_split_n;

template<size_t _vIdx, class _tSeq>
using sequence_split_n_t = typename sequence_split_n<_vIdx, _tSeq>::type;

template<size_t... _vSeq>
struct sequence_split_n<0, variadic_sequence<_vSeq...>>
{
	using type = variadic_sequence<>;
	using tail = variadic_sequence<_vSeq...>;
};

template<size_t _vHead, size_t... _vSeq>
struct sequence_split_n<1, variadic_sequence<_vHead, _vSeq...>>
{
	using type = variadic_sequence<_vHead>;
	using tail = variadic_sequence<_vSeq...>;
};

template<size_t _vIdx, size_t... _vSeq>
struct sequence_split_n<_vIdx, variadic_sequence<_vSeq...>>
{
private:
	using half = sequence_split_n<_vIdx / 2, variadic_sequence<_vSeq...>>;
	using last = sequence_split_n<_vIdx - _vIdx / 2, typename half::tail>;

public:
	using type = sequence_cat_t<typename half::type, typename last::type>;
	using tail = typename last::tail;
};
//@}


/*!
\ingroup vseq_operations
\brief 二元操作合并应用。
\pre 二元操作符合交换律和结合律。
\since build 507
*/
//@{
template<class, typename, class>
struct sequence_fold;

template<class _fBinary, typename _tState, class _type>
using sequence_fold_t = typename sequence_fold<_fBinary, _tState, _type>::type;

template<class _fBinary, class _tState>
struct sequence_fold<_fBinary, _tState, variadic_sequence<>>
{
	using type = _tState;

	static yconstexpr auto value = _tState::value;
};

template<class _fBinary, class _tState, size_t _vHead>
struct sequence_fold<_fBinary, _tState, variadic_sequence<_vHead>>
{
	static yconstexpr auto value = _fBinary()(_tState::value, _vHead);

	using type = variadic_sequence<value>;
};

template<class _fBinary, class _tState, size_t... _vSeq>
struct sequence_fold<_fBinary, _tState, variadic_sequence<_vSeq...>>
{
private:
	using parts
		= sequence_split_n<sizeof...(_vSeq) / 2, variadic_sequence<_vSeq...>>;
	using head = typename parts::type;
	using tail = typename parts::tail;

public:
	static yconstexpr auto value = sequence_fold<_fBinary,
		std::integral_constant<size_t,
		sequence_fold<_fBinary, _tState, head>::value>, tail>::value;

	using type = variadic_sequence<value>;
};
//@}


/*!
\brief 直接接受 size_t 类型值二元操作合并应用。
\sa sequence_fold
\since build 507
*/
template<class _fBinary, size_t _vState, size_t... _vSeq>
using vseq_fold = sequence_fold<_fBinary,
	std::integral_constant<size_t, _vState>, variadic_sequence<_vSeq...>>;


/*!
\ingroup meta_operations
\brief 取整数序列的自然数后继。
\since build 303
*/
//@{
template<class>
struct make_successor;

//! \since build 447
template<class _tSeq>
using make_successor_t = typename make_successor<_tSeq>::type;

template<size_t... _vSeq>
struct make_successor<variadic_sequence<_vSeq...>>
{
	using type = variadic_sequence<_vSeq..., sizeof...(_vSeq)>;
};
//@}


/*!
\ingroup meta_operations
\brief 取自然数变量标记序列。
\since build 303
*/
//@{
template<size_t>
struct make_natural_sequence;

//! \since build 447
template<size_t _vN>
using make_natural_sequence_t = typename make_natural_sequence<_vN>::type;

template<size_t _vN>
struct make_natural_sequence
{
	using type = make_successor_t<make_natural_sequence_t<_vN - 1>>;
};

template<>
struct make_natural_sequence<0>
{
	using type = variadic_sequence<>;
};
//@}

} // namespace ystdex;

#endif

