﻿/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file enum.hpp
\ingroup YStandardEx
\brief 枚举相关操作。
\version r124
\author FrankHB <frankhb1989@gmail.com>
\since build 629
\par 创建时间:
	2015-09-02 10:28:23 +0800
\par 修改时间:
	2015-11-05 01:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Enum
*/


#ifndef YB_INC_ystdex_enum_hpp_
#define YB_INC_ystdex_enum_hpp_ 1

#include "tuple.hpp" // for false_type, underlying_type_t, _t, and_, or_,
//	is_enum, common_type_t, vseq::find, std::tuple;

namespace ystdex
{

//! \since build 629
//@{
//! \ingroup unary_type_traits
template<typename>
struct is_enum_union : false_type
{};


//! \ingroup trasformation_traits
//@{
template<typename _type>
struct wrapped_enum_traits
{
	using type = underlying_type_t<_type>;
};

template<typename _type>
using wrapped_enum_traits_t = _t<wrapped_enum_traits<_type>>;
//@}


template<typename... _types>
class enum_union
{
	static_assert(and_<or_<is_enum<_types>, is_enum_union<_types>>...>(),
		"Invalid types found.");

public:
	using underlying_type = common_type_t<wrapped_enum_traits_t<_types>...>;

private:
	underlying_type value;

public:
	yconstfn
	enum_union() = default;
	explicit yconstfn
	enum_union(underlying_type i) ynothrow
		: value(i)
	{}
	template<typename _type,
		yimpl(typename = vseq::find<std::tuple<_types...>, _type>())>
	yconstfn
	enum_union(_type e) ynothrow
		: value(static_cast<underlying_type>(e))
	{}
	yconstfn
	enum_union(const enum_union&) = default;

	enum_union&
	operator=(const enum_union&) = default;

	explicit yconstfn
	operator underlying_type() const ynothrow
	{
		return value;
	}
};


//! \relates enum_union
//@{
template<typename... _types>
struct is_enum_union<enum_union<_types...>> : true_type
{};

template<typename... _types>
struct wrapped_enum_traits<enum_union<_types...>>
{
	using type = typename enum_union<_types...>::underlying_type;
};

template<typename _type>
yconstfn yimpl(enable_if_t)<is_enum_union<_type>::value,
	wrapped_enum_traits_t<_type>>
underlying(_type val) ynothrow
{
	return wrapped_enum_traits_t<_type>(val);
}
//@}
//@}

} // namespace ystdex;

#endif

