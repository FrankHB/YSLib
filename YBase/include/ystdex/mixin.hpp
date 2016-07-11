/*
	© 2014-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file mixin.hpp
\ingroup YStandardEx
\brief 基于类继承的混入接口。
\version r189
\author FrankHB <frankhb1989@gmail.com>
\since build 477
\par 创建时间:
	2014-02-17 00:07:20 +0800
\par 修改时间:
	2016-07-11 10:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Mixin
*/


#ifndef YB_INC_ystdex_mixin_hpp_
#define YB_INC_ystdex_mixin_hpp_ 1

#include "tuple.hpp" // for for "tuple.hpp", std::tuple, exclude_self_t,
//	std::get, index_sequence_for, tuple_element_t;
#include "utility.hpp" // for classify_value_t;

namespace ystdex
{

//! \since build 477
//@{
/*!
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
		yimpl(typename = exclude_self_t<mixin, _tParam>)>
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
		yimpl(enable_if_t<(sizeof...(_tBases) == 1)>* = {}))
		: mixin(std::get<0>(tp))
	{}
	template<typename... _tParams>
	yconstfn
	mixin(std::tuple<_tParams...>&& tp,
		yimpl(enable_if_t<(sizeof...(_tBases) == 1)>* = {}))
		: mixin(std::get<0>(std::move(tp)))
	{}
	template<typename... _tParams>
	yconstfn
	mixin(const std::tuple<_tParams...>& tp,
		yimpl(enable_if_t<(sizeof...(_tBases) > 1)>* = {}))
		: mixin(index_sequence_for<_tParams...>(), tp)
	{}
	template<typename... _tParams>
	yconstfn
	mixin(std::tuple<_tParams...>&& tp,
		yimpl(enable_if_t<(sizeof...(_tBases) > 1)>* = {}))
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


/*!
\ingroup metafunctions
\brief 包装为混入类。
\note 使用 classify_value_t 对非类类型包装为 boxed_value 实例。
*/
template<typename... _types>
using wrap_mixin_t = mixin<classify_value_t<_types>...>;
//@}

} // namespace ystdex;

#endif

