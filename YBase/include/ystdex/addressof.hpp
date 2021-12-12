/*
	© 2015-2016, 2018-2019, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file addressof.hpp
\ingroup YStandardEx
\brief 一元操作符 & 和取指针的相关接口。
\version r232
\author FrankHB <frankhb1989@gmail.com>
\since build 660
\par 创建时间:
	2015-12-17 10:07:56 +0800
\par 修改时间:
	2021-11-20 23:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::AddressOf

提供和一元操作符 & 相关的接口：
检查是否重载 operator& 的特征；
非重载时能有 constexpr 替代 ISO C++17 前 std::addressof 的 ystdex::addressof 。
*/


#ifndef YB_INC_ystdex_addressof_hpp_
#define YB_INC_ystdex_addressof_hpp_ 1

#include "meta.hpp" // for internal "meta.hpp", std::declval, is_detected, or_;
#include <memory> // for std::addressof;

/*!
\brief \c \<memory> 特性测试宏。
\see https://isocpp.org/std/standing-documents/sd-6-sg10-feature-test-recommendations 。
\see https://blogs.msdn.microsoft.com/vcblog/2015/06/19/c111417-features-in-vs-2015-rtm/ 。
\since build 832
*/
//@{
// XXX: There was once no feature test macro provided. See http://www.open-std.org/pipermail/features/2016-March/000399.html.
// XXX: It is known 'std::addressof' has 'constexpr' in Microsoft VC++ 15.6.4.
//	Not sure which is the initial version as it seems undocumented.
// TODO: Get more accurate version.
#ifndef __cpp_lib_addressof_constexpr
#	if (YB_IMPL_MSCPP >= 1913 && _MSVC_LANG >= 201606) || __cplusplus >= 201606L
#		define __cpp_lib_addressof_constexpr 201606L
#	endif
#endif
//@}

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

//! \ingroup unary_type_traits
//@{
/*!
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
\brief 判断是否无法重载 & 操作符接受指定类型的表达式。
\note 支持参数为不完整类型的情形。不排除任意可能重载 & 的类型。
\since build 864
*/
template<typename _type>
struct has_no_overloaded_addressof
	: or_<is_arithmetic<remove_cvref_t<_type>>, is_array<remove_cvref_t<_type>>,
	is_function<remove_reference_t<_type>>, is_null_pointer<_type>>
{};
//@}


/*!
\brief 尝试对非重载 operator& 提供 constexpr 的 std::addressof 替代。
\since build 831
\see LWG 2296 。
\see https://reviews.llvm.org/rL186053 。

提供和 ISO C++17 的 std::addressof 尽可能相同的接口。
在 ISO C++17 前， constexpr 需要扩展支持。
返回值的 nodiscard 标记是类似 P0600R1 理由的兼容扩展。
Microsoft VC++ 15.7.5 也使用 [[nodiscard]] 。
*/
//@{
#if __cpp_lib_addressof_constexpr >= 201606L
using std::addressof;
#else
// NOTE: This is implemented since GCC r240873.
#	if __has_builtin(__builtin_addressof) || YB_IMPL_GNUCPP >= 70100 \
	|| YB_IMPL_CLANGPP >= 30300
//! \since build 833
template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_PURE yconstfn _type*
addressof(_type& r) ynothrow
{
	return __builtin_addressof(r);
}
#	else
//! \since build 833
//@{
template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_PURE yconstfn
	yimpl(enable_if_t)<has_no_overloaded_addressof<_type>::value, _type*>
addressof(_type& r) ynothrow
{
	return &r;
}
/*!
\note 因为可移植性需要，不能直接提供 constexpr 。
\warning ISO C++17 前无 constexpr 支持。
*/
template<typename _type,
	yimpl(typename = enable_if_t<!has_no_overloaded_addressof<_type>::value>)>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_PURE inline _type*
addressof(_type& r) ynothrow
{
	return std::addressof(r);
}
//@}
#	endif
//! \see LWG 2598 。
template<class _type>
const _type*
addressof(const _type&&) = delete;
#endif
//@}


/*!
\brief 取 \c void* 类型的指针。
\note 适合 std::fprintf 等的 \c %p 转换规格。
\warning 若参数指向 const 对象，使用返回值修改指向的左值，或在占据的存储上
	重新分配对象，会引起未定义行为。
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

