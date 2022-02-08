﻿/*
	© 2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file compressed_pair.hpp
\ingroup YStandardEx
\brief 压缩存储对类型。
\version r242
\author FrankHB <frankhb1989@gmail.com>
\since build 189
\par 创建时间:
	2022-02-08 23:56:14 +0800
\par 修改时间:
	2022-02-09 03:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CompressedPair

\see https://www.boost.org/doc/libs/1_78_0/libs/utility/doc/html/utility/utilities/compressed_pair.html 。
\see libc++ <__memory/compressed_pair> 。
*/


#ifndef YB_INC_ystdex_compressed_pair_hpp_
#define YB_INC_ystdex_compressed_pair_hpp_ 1

#include "placement.hpp" // for internal "placement.hpp", _tIdx, default_init_t,
//	value_init_t, std::piecewise_construct_t, value_init, is_bitwise_swappable;
#include "integer_sequence.hpp" // for and_, is_empty, not_, is_final,
//	index_sequence, is_default_constructible, index_sequence_for,
//	vseq::seq_size, remove_cvref_t;
#include "swap.hpp" // for is_nothrow_swappable;

namespace ystdex
{

/*!
\warning 非虚析构。
\since build 938
*/
//@{
/*!
\brief 可压缩空基类的元素。
\note 模板参数分别表示元素值的类型、支持在同一个类继承的冗余索引和指定压缩。
*/
//@{
template<typename _type, size_t,
	bool = and_<is_empty<_type>, not_<is_final<_type>>>::value>
class compressed_pair_element
{
public:
	using value_type = _type;
	using reference = _type&;
	using const_reference = const _type&;

private:
	value_type value;

public:
	yconstfn
	compressed_pair_element(default_init_t)
	{}
	yconstfn
	compressed_pair_element(value_init_t)
		: value()
	{}
	template<typename _tParam,
		yimpl(typename = exclude_self_t<compressed_pair_element, _tParam>)>
	yconstfn
	compressed_pair_element(_tParam&& val)
		: value(yforward(val))
	{}
	template<typename... _tParams, typename _tTuple, size_t... _vSeq>
	yconstfn
	compressed_pair_element(std::piecewise_construct_t, _tTuple&& t,
		index_sequence<_vSeq...>)
		: value(yforward(std::get<_vSeq...>(yforward(t))))
	{}

	YB_ATTR_nodiscard yconstfn_relaxed YB_PURE _type&
	get() ynothrow
	{
		return value;
	}
	YB_ATTR_nodiscard yconstfn YB_PURE const _type&
	get() const ynothrow
	{
		return value;
	}
};

template<typename _type, size_t _tIdx>
class compressed_pair_element<_type, _tIdx, true> : private _type
{
public:
	using value_type = _type;
	using reference = _type&;
	using const_reference = const _type&;

private:
	value_type value;

public:
	yconstfn
	compressed_pair_element() = default;
	yconstfn
	compressed_pair_element(default_init_t)
	{}
	yconstfn
	compressed_pair_element(value_init_t)
		: value_type()
	{}
	template<typename _tParam,
		yimpl(typename = exclude_self_t<compressed_pair_element, _tParam>)>
	yconstfn
	compressed_pair_element(_tParam&& val)
		: value_type(yforward(val))
	{}
	template<typename... _tParams, typename _tTuple, size_t... _vSeq>
	yconstfn
	compressed_pair_element(std::piecewise_construct_t, _tTuple&& t,
		index_sequence<_vSeq...>)
		: value_type(yforward(std::get<_vSeq...>(yforward(t))))
	{}

	YB_ATTR_nodiscard yconstfn_relaxed YB_PURE _type&
	get() ynothrow
	{
		return *this;
	}
	YB_ATTR_nodiscard yconstfn YB_PURE const _type&
	get() const ynothrow
	{
		return *this;
	}
};
//@}


template<typename _type1, typename _type2>
class compressed_pair : private compressed_pair_element<_type1, 0>,
	private compressed_pair_element<_type2, 1>
{
	static_assert(!is_same<_type1, _type2>(),
		"Duplicate template parameters are currently not supported.");

public:
	using base1 = compressed_pair_element<_type1, 0>;
	using base2 = compressed_pair_element<_type2, 1>;

	template<yimpl(typename _tParam1 = _type1, typename _tParam2 = _type2,
		typename = enable_if_t<is_default_constructible<_tParam1>::value
		&& is_default_constructible<_tParam2>::value>)>
	yconstfn
	compressed_pair()
		: base1(value_init), base2(value_init)
	{}
	template<typename _tParam1, typename _tParam2>
	yconstfn
	compressed_pair(_tParam1&& arg1, _tParam2&& arg2)
		: base1(yforward(arg1)), base2(yforward(arg2))
	{}
	template<typename _tTuple1, typename _tTuple2>
	yconstfn
	compressed_pair(std::piecewise_construct_t tag, _tTuple1&& t1,
		_tTuple2&& t2)
		: base1(tag, yforward(t1), index_sequence_for<vseq::seq_size<
		remove_cvref_t<_tTuple1>>>()), base2(tag, yforward(t2),
		index_sequence_for<vseq::seq_size<remove_cvref_t<_tTuple2>>>())
	{}

	YB_ATTR_nodiscard YB_PURE yconstfn_relaxed typename base1::reference
	first() ynothrow
	{
		return first_base().get();
	}
	YB_ATTR_nodiscard YB_PURE yconstfn typename base1::const_reference
	first() const ynothrow
	{
		return first_base().get();
	}

	YB_ATTR_nodiscard YB_PURE yconstfn_relaxed base1&
	first_base() ynothrow
	{
		return static_cast<base1&>(*this);
	}
	YB_ATTR_nodiscard YB_PURE yconstfn const base1&
	first_base() const ynothrow
	{
		return static_cast<const base1&>(*this);
	}

	YB_ATTR_nodiscard YB_PURE yconstfn_relaxed typename base2::reference
	second() ynothrow
	{
		return second_base().get();
	}
	YB_ATTR_nodiscard YB_PURE yconstfn typename base2::const_reference
	second() const ynothrow
	{
		return second_base().get();
	}

	YB_ATTR_nodiscard YB_PURE yconstfn_relaxed base2&
	second_base() ynothrow
	{
		return static_cast<base2&>(*this);
	}
	YB_ATTR_nodiscard YB_PURE yconstfn const base2&
	second_base() const ynothrow
	{
		return static_cast<const base2&>(*this);
	}

	friend void
	swap(compressed_pair& x, compressed_pair& y)
		ynoexcept(and_<is_nothrow_swappable<_type1>,
		is_nothrow_swappable<_type2>>())
	{
		ystdex::swap_dependent(x.first(), y.first()),
		ystdex::swap_dependent(x.second(), y.second());
	}
};

//! \relates comporessed_pair
template<typename _type1, typename _type2>
struct is_bitwise_swappable<compressed_pair<_type1, _type2>>
	: and_<is_bitwise_swappable<_type1>, is_bitwise_swappable<_type2>>
{};
//@}

} // namespace ystdex;

#endif

