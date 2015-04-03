/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file mixin.hpp
\ingroup YStandardEx
\brief 基于类继承的混入接口。
\version r163
\author FrankHB <frankhb1989@gmail.com>
\since build 477
\par 创建时间:
	2014-02-17 00:07:20 +0800
\par 修改时间:
	2015-03-31 11:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Mixin
*/


#ifndef YB_INC_ystdex_mixin_hpp_
#define YB_INC_ystdex_mixin_hpp_ 1

#include "tuple.hpp" // for std::tuple, ystdex::index_sequence_for,
//	ystdex::tuple_element_t
#include "utility.hpp" // for ../ydef.h, ystdex::classify_value_t;

namespace ystdex
{

//! \since build 477
//@{
/*
\brief 混入对象。
\warning 非显式虚析构；是否为多态类取决于参数。
*/
template<class... _tBases>
class mixin : public _tBases...
{
public:
	using tuple_type = std::tuple<_tBases...>;

	yconstfn
	mixin() = default;
	template<typename _tParam,
		yimpl(typename = exclude_self_ctor_t<mixin, _tParam>)>
	yconstfn
	mixin(_tParam&& arg)
		: _tBases(yforward(arg))...
	{}
	template<typename _tParam1, typename _tParam2, typename... _tParams>
	yconstfn
	mixin(_tParam1&& arg1, _tParam2&& arg2, _tParams&&... args)
		: mixin(std::forward_as_tuple(yforward(arg1), yforward(arg2),
		yforward(args)...))
	{}
	template<typename... _tParams>
	yconstfn
	mixin(const std::tuple<_tParams...>& tp,
		yimpl(enable_if_t<(sizeof(tp), sizeof...(_tBases) == 1)>* = {}))
		: mixin(std::get<0>(tp))
	{}
	template<typename... _tParams>
	yconstfn
	mixin(std::tuple<_tParams...>&& tp,
		yimpl(enable_if_t<(sizeof(tp), sizeof...(_tBases) == 1)>* = {}))
		: mixin(std::get<0>(std::move(tp)))
	{}
	template<typename... _tParams>
	yconstfn
	mixin(const std::tuple<_tParams...>& tp,
		yimpl(enable_if_t<(sizeof(tp), sizeof...(_tBases) > 1)>* = {}))
		: mixin(index_sequence_for<_tParams...>(), tp)
	{}
	template<typename... _tParams>
	yconstfn
	mixin(std::tuple<_tParams...>&& tp,
		yimpl(enable_if_t<(sizeof(tp), sizeof...(_tBases) > 1)>* = {}))
		: mixin(index_sequence_for<_tParams...>(), std::move(tp))
	{}
	template<size_t... _vSeq, typename... _tParams>
	yconstfn
	mixin(index_sequence<_vSeq...>, const std::tuple<_tParams...>& tp)
		: _tBases(yforward(std::get<_vSeq>(tp)))...
	{}
	template<size_t... _vSeq, typename... _tParams>
	yconstfn
	mixin(index_sequence<_vSeq...>, std::tuple<_tParams...>&& tp)
		: _tBases(yforward(std::get<_vSeq>(tp)))...
	{}
	yconstfn
	mixin(const mixin&) = default;
	yconstfn
	mixin(mixin&&) = default;

	tuple_type
	to_tuple() const
	{
		return this->template to_tuple(index_sequence_for<_tBases...>());
	}
	template<size_t... _vSeq>
	tuple_type
	to_tuple(index_sequence<_vSeq...>) const
	{
		return tuple_type(
			static_cast<const tuple_element_t<_vSeq, tuple_type>&>(*this)...);
	}
};


namespace details
{

template<class, class>
struct wrap_mixin_helper;

template<size_t... _vSeq, typename... _types>
struct wrap_mixin_helper<index_sequence<_vSeq...>, std::tuple<_types...>>
{
	using type = mixin<
		classify_value_t<tuple_element_t<_vSeq, std::tuple<_types...>>>...>;
};

} // namespace details;


/*!
\ingroup metafunctions
\brief 包装为混入类。
\note 对非类类型首先使用 classify_value_t 包装。
*/
template<typename... _types>
using wrap_mixin_t = typename details::wrap_mixin_helper<
	index_sequence_for<_types...>, std::tuple<_types...>>::type;
//@}

} // namespace ystdex;

#endif

