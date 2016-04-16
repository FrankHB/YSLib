/*
	© 2015-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file addressof.hpp
\ingroup YStandardEx
\brief 一元操作符 & 和取指针的相关接口。
\version r126
\author FrankHB <frankhb1989@gmail.com>
\since build 660
\par 创建时间:
	2015-12-17 10:07:56 +0800
\par 修改时间:
	2016-04-16 11:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::AddressOf

提供和一元操作符 & 相关的接口：
检查是否重载 operator& 的特征；
以及非重载时能有 constexpr 的 std::addressof 替代 ystdex::constfn_addressof 。
*/


#ifndef YB_INC_ystdex_addressof_hpp_
#define YB_INC_ystdex_addressof_hpp_ 1

#include "type_traits.hpp" // for "type_traits.hpp", std::declval, or_,
//	is_detected;
#include <memory> // for std::addressof;

namespace ystdex
{

namespace details
{

//! \since build 649
//@{
template<typename _type>
using addressof_mem_t = decltype(std::declval<const _type&>().operator&());

template<typename _type>
using addressof_free_t = decltype(operator&(std::declval<const _type&>()));
//@}

} // namespace details;

/*!
\ingroup unary_type_traits
\brief 判断是否存在合式重载 & 操作符接受指定类型的表达式。
\pre 参数不为不完整类型。
\since build 649
*/
template<typename _type>
struct has_overloaded_addressof
	: or_<is_detected<details::addressof_mem_t, _type>,
	is_detected<details::addressof_free_t, _type>>
{};


/*!
\brief 尝试对非重载 operator& 提供 constexpr 的 std::addressof 替代。
\note 当前 ISO C++ 的 std::addressof 无 constexpr 。
\since build 591
\see http://wg21.cmeerw.net/lwg/issue2296 。
*/
//@{
template<typename _type>
yconstfn yimpl(enable_if_t)<!has_overloaded_addressof<_type>::value, _type*>
constfn_addressof(_type& r)
{
	return &r;
}
//! \note 因为可移植性需要，不能直接提供 constexpr 。
template<typename _type,
	yimpl(typename = enable_if_t<has_overloaded_addressof<_type>::value>)>
inline _type*
constfn_addressof(_type& r)
{
	return std::addressof(r);
}
//@}


/*!
\brief 取 \c void* 类型的指针。
\note 适合 std::fprintf 等的 \c %p 转换规格。
\since build 563
*/
//@{
template<typename _type>
yconstfn yimpl(enable_if_t)<or_<is_object<_type>, is_void<_type>>::value, void*>
pvoid(_type* p) ynothrow
{
	return const_cast<void*>(static_cast<const volatile void*>(p));
}
//! \note ISO C++11 指定函数指针 \c reinterpret_cast 为对象指针有条件支持。
template<typename _type>
inline yimpl(enable_if_t)<is_function<_type>::value, void*>
pvoid(_type* p) ynothrow
{
	return const_cast<void*>(reinterpret_cast<const volatile void*>(p));
}

/*!
\return 非空指针。
\since build 552
*/
template<typename _type>
yconstfn void*
pvoid_ref(_type&& ref)
{
	return ystdex::pvoid(std::addressof(ref));
}
//@}

} // namespace ystdex;

#endif

