/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file variadic.hpp
\ingroup YStandardEx
\brief C++ 变长参数相关操作。
\version r85
\author FrankHB <frankhb1989@gmail.com>
\since build 412
\par 创建时间:
	2013-06-06 11:38:15 +0800
\par 修改时间:
	2013-06-06 12:10 +0800
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
\brief 变量参数标记。
\since build 303
*/
template<size_t... _vSeq>
struct variadic_sequence
{};


/*!
\ingroup meta_operations
\brief 取自然数变量标记后继。
\since build 303
*/
//@{
template<class>
struct make_successor;

template<size_t... _vSeq>
struct make_successor<variadic_sequence<_vSeq...>>
{
	typedef variadic_sequence<_vSeq..., sizeof...(_vSeq)> type;
};
//@}


/*!
\ingroup meta_operations
\brief 取自然数变量标记序列。
\since build 303
*/
//@{
template<size_t _vN>
struct make_natural_sequence
{
	typedef typename make_successor<typename
		make_natural_sequence<_vN - 1>::type>::type type;
};

template<>
struct make_natural_sequence<0>
{
	typedef variadic_sequence<> type;
};
//@}

} // namespace ystdex;

#endif

