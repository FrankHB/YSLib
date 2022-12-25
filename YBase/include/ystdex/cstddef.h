/*
	© 2011-2015, 2021-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstddef.h
\ingroup YStandardEx
\brief ISO C 标准库类型定义扩展。
\version r5538
\author FrankHB <frankhb1989@gmail.com>
\since build 933
\par 创建时间:
	2021-12-12 21:17:52 +0800
\par 修改时间:
	2022-11-28 19:53 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CStandardDefinition
*/


#ifndef YB_INC_ystdex_cstddef_h_
#define YB_INC_ystdex_cstddef_h_ 1

#include "../ydef.h"
#include <cstddef> // for __cpp_lib_byte, std::byte, std::nullptr_t,
//	std::size_t, std::ptrdiff_t, offsetof;
#include <climits> // for CHAR_BIT;
#include <type_traits> // for std::is_class, std::is_standard_layout,
//	std::is_member_object_pointer;

namespace ystdex
{

/*!
\brief \c \<cstddef> 特性测试宏。
\see ISO C++20 [version.syn] 。
\see WG21 P0941R2 2.2 。
\see https://docs.microsoft.com/cpp/visual-cpp-language-conformance 。
\see https://blogs.msdn.microsoft.com/vcblog/2017/05/10/c17-features-in-vs-2017-3/ 。
\see https://docs.microsoft.com/cpp/preprocessor/predefined-macros 。
\see https://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html#status.iso.2017 。
\since build 832
*/
//@{
#ifndef __cpp_lib_byte
#	if ((YB_IMPL_MSCPP >= 1911 && _MSVC_LANG >= 201603L) \
	|| YB_IMPL_GCC >= 70100 || __cplusplus >= 201603L) \
	&& !(YB_IMPL_MSCPP >= 1911 && _MSVC_LANG >= 201603L && _HAS_STD_BYTE != 0)
#		define __cpp_lib_byte 201603L
#	endif
#endif
//@}

/*!
\brief 字节类型。
\see WG21 P0298R2 。
\since build 209
\todo 检查 maybe_unused 属性，允许时使用和标准库定义一致的形式。

专用于表示字节的类型，不保证为整数类型或字符类型。
注意 ISO C++ 对访问存储的 glvalue 的类型有严格限制，当没有对象生存期保证时，
仅允许（可能 cv 修饰的） char 和 unsigned char 及其指针/引用或 void* ，
而不引起未定义行为(undefined behavior) 。
ISO C++17 核心语言特性对 std::byte 别名存储提供支持，其它情形只有 char 、
	signed char 和 unsigned char 满足可移植地支持别名。
由于 char 的算术操作行为是否按有符号处理未指定，使用 unsigned char
表示字节以便获得确定的行为，同时对字符处理（如 std::fgetc ）保持较好的兼容性。
使用 unsigned char 仍是实现细节。依赖此类型上不在 std::byte 的操作不被支持。
*/
#if __cpp_lib_byte >= 201603L
using std::byte;
#else
using byte = unsigned char;
#endif

#if CHAR_BIT == 8
/*!
\brief 八位组类型。
\note 一字节不保证等于 8 位，但一个八位组保证等于 8 位。
\since build 417
*/
using octet = unsigned char;
#	else
using octet = void;
#endif

//! \since build 209
using std::ptrdiff_t;
//! \since build 209
using std::size_t;

#if YB_IMPL_MSCPP && defined(__cplusplus_cli)
//! \see https://docs.microsoft.com/cpp/extensions/nullptr-cpp-component-extensions 。
using nullptr_t = decltype(__nullptr);
#elif YB_HAS_BUILTIN_NULLPTR
//! \note ystdex 内的 nullptr 使用一般应符合兼容 nullptr_t 的模拟实现。
using std::nullptr_t;
#else
/*!
\brief 空指针类。
\see https://en.wikibooks.org/wiki/More_C++_Idioms/nullptr 。

模拟 std::nullptr_t 。
和 std::nullptr_t 不同：
不保证是基本类型；
不保证大小和 void* 一致；
不支持标准库类型特征判断；
不支持使用整数 0 字面量（常量表达式）初始化（并可能因此存在更多的重载歧义）；
作为左值不支持 & 操作符。
*/
yconstexpr_inline const class nullptr_t
{
public:
	//! \brief 禁止取 nullptr 的指针。
	void
	operator&() const = delete;

	//! \since build 884
	//@{
	//! \brief 重载关系操作符。
	//@{
	template<typename _type>
	YB_ATTR_nodiscard YB_STATELESS friend yconstfn bool
	operator==(nullptr_t, const _type& y)
	{
		return y == 0;
	}
	template<typename _type>
	YB_ATTR_nodiscard YB_STATELESS friend yconstfn bool
	operator==(const _type& x, nullptr_t)
	{
		return x == 0;
	}

	template<typename _type>
	YB_ATTR_nodiscard YB_STATELESS friend yconstfn bool
	operator!=(nullptr_t, const _type& y)
	{
		return !(y == 0);
	}
	template<typename _type>
	YB_ATTR_nodiscard YB_STATELESS friend yconstfn bool
	operator!=(const _type& x, nullptr_t)
	{
		return !(x == 0);
	}
	//@}

	//! \brief 转换任意类型至空非成员或静态成员指针。
	template<typename _type>
	yconstfn
	operator _type*() const ynothrow
	{
		return 0;
	}

	//! \brief 转换任意类型至空非静态成员指针。
	template<class _tClass, typename _type>
	yconstfn
	operator _type _tClass::*() const ynothrow
	{
		return 0;
	}
	//@}
} nullptr = {};
#endif


/*!
\brief 空基类模板。
\since build 260
*/
template<typename...>
struct empty_base
{};

/*!
\ingroup tags
\brief 直接构造类型（直接构造重载用）。
\since build 520
*/
struct raw_tag
{};


/*!
\see ISO C++11 [support.types]/4 。
\since build 325
*/
//@{
/*!
\brief 成员偏移计算静态类型检查。
\warning 非虚析构。
*/
template<bool _bMemObjPtr, bool _bNoExcept, class _type>
struct offsetof_check
{
	static_assert(std::is_class<_type>::value, "Non class type found.");
	static_assert(std::is_standard_layout<_type>::value,
		"Non standard layout type found.");
	static_assert(_bMemObjPtr, "Non-static member object violation found.");
	static_assert(_bNoExcept, "Exception guarantee violation found.");
};


/*!
\ingroup YBase_pseudo_keyword
\brief 带有静态类型检查的成员偏移计算。
\note 某些实现可直接使用 __builtin_offsetof 及 -Winvalid-offsetof 。
\see https://gcc.gnu.org/onlinedocs/gcc-4.0.0/gcc/Offsetof.html 。
\see https://docs.microsoft.com/cpp/cpp-conformance-improvements-2017?view=vs-2017 。
\see https://reviews.llvm.org/rL46515 。
\see https://bugs.llvm.org/show_bug.cgi?id=31178 。
\see https://reviews.llvm.org/D66100 。
*/
#if __has_builtin(__builtin_offsetof) || YB_IMPL_GNUCPP >= 40000 \
	|| YB_IMPL_CLANGPP >= 20200
	// XXX: The %__has_builtin condition does not work for old versions of
	//	Clang due to LLVM PR31178. In this case, it is enabled by version
	//	detection.
#	define yoffsetof(_type, _member) \
	(decltype(sizeof(ystdex::offsetof_check< \
	std::is_member_object_pointer<decltype(&_type::_member)>::value, \
	ynoexcept(__builtin_offsetof(_type, _member)), _type>))( \
	__builtin_offsetof(_type, _member)))
#else
#	define yoffsetof(_type, _member) \
	(decltype(sizeof(ystdex::offsetof_check<std::is_member_object_pointer< \
	decltype(&_type::_member)>::value, ynoexcept(offsetof(_type, _member)), \
	_type>))(offsetof(_type, _member)))
#endif
//@}

} // namespace ystdex;

#endif

