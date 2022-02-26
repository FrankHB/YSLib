/*
	© 2012, 2014-2015, 2018-2019, 2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file bind.hpp
\ingroup YStandardEx
\brief 函数部分应用。
\version r5305
\author FrankHB <frankhb1989@gmail.com>
\since build 939
\par 创建时间:
	2022-02-13 09:10:49 +0800
\par 修改时间:
	2022-02-14 18:01 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Bind
*/


#ifndef YB_INC_ystdex_bind_hpp_
#define YB_INC_ystdex_bind_hpp_ 1

#include "../ydef.h"
#include <functional> // for <functional>, std::bind, std::placeholders;

namespace ystdex
{

/*!
\brief 复合调用 std::bind 和 std::placeholders::_1 。
\note ISO C++ 要求 std::placeholders::_1 被实现支持。
*/
//@{
//! \since build 628
template<typename _func, typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline auto
bind1(_func&& f, _tParams&&... args) -> decltype(
	std::bind(yforward(f), std::placeholders::_1, yforward(args)...))
{
	return std::bind(yforward(f), std::placeholders::_1, yforward(args)...);
}
//! \since build 653
template<typename _tRes, typename _func, typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline auto
bind1(_func&& f, _tParams&&... args) -> decltype(
	std::bind<_tRes>(yforward(f), std::placeholders::_1, yforward(args)...))
{
	return
		std::bind<_tRes>(yforward(f), std::placeholders::_1, yforward(args)...);
}
//@}

/*!
\brief 复合调用 ystdex::bind1 和 std::placeholders::_2 以实现值的设置。
\note 从右到左逐个应用参数。
\note ISO C++ 要求 std::placeholders::_2 被实现支持。
\since build 651
*/
template<typename _func, typename _func2, typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline auto
bind_forward(_func&& f, _func2&& f2, _tParams&&... args)
	-> decltype(ystdex::bind1(yforward(f), std::bind(yforward(f2),
	std::placeholders::_2, yforward(args)...)))
{
	return ystdex::bind1(yforward(f), std::bind(yforward(f2),
		std::placeholders::_2, yforward(args)...));
}

} // namespace ystdex;

#endif

