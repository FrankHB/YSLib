/*
	© 2009-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup YBase YBaseLib
\brief YFramework 基础库。
*/
/*!	\defgroup YStandardEx YStandardExtend
\ingroup YBase
\brief YFramework 标准扩展库。
*/

/*!	\file ydef.h
\ingroup YBase
\brief 系统环境和公用类型和宏的基础定义。
\version r3504
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-02 21:42:44 +0800
\par 修改时间:
	2018-12-02 16:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YDefinition
*/


#ifndef YB_INC_ydef_h_
#define YB_INC_ydef_h_ 1

/*!	\defgroup lang_impl_versions Language Implementation Versions
\brief 语言实现的版本。
\since build 373
*/
//@{

/*!
\def YB_IMPL_GNUC
\brief GNU C 实现支持版本。
\since build 595

定义为 100 进位制的三重版本编号和。
*/

/*!
\def YB_IMPL_MSCPP
\brief Microsoft C++ 实现支持版本。
\since build 313
\see https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros 。
\see https://blogs.msdn.microsoft.com/vcblog/2016/10/05/visual-c-compiler-version/ 。

定义为 _MSC_VER 描述的版本号。
*/

/*!
\def YB_IMPL_GNUCPP
\brief GNU C++ 实现支持版本。
\since build 313

定义为 100 进位制的三重版本编号和。
*/

/*!
\def YB_IMPL_CLANGCPP
\brief LLVM/Clang++ 实现支持版本。
\since build 458

定义为 100 进位制的三重版本编号和。
*/

#if defined(__GNUC__)
#	undef YB_IMPL_GNUC
#	define YB_IMPL_GNUC \
	(__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif
#ifdef __cplusplus
#	if defined(_MSC_VER) && !defined(__clang__)
#		undef YB_IMPL_MSCPP
#		define YB_IMPL_MSCPP _MSC_VER
#	elif defined(__clang__)
#		undef YB_IMPL_CLANGPP
#		if defined(__apple_build_version__)
#		define YB_IMPL_CLANGPP (40300 + __clang_patchlevel__)
#		else
#			define YB_IMPL_CLANGPP (__clang__ * 10000 + __clang_minor__ * 100 \
	+ __clang_patchlevel__)
#		endif
#	elif defined(__GNUG__)
#		undef YB_IMPL_GNUCPP
#		define YB_IMPL_GNUCPP \
	(__GNUG__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#	else
// TODO: Deferred. Complete version checking for compiler and library
//	implementations, e.g. EDG frontends.
//#ifdef __GNUC__
//#	include <tr1/type_traits>
#		error "This language implementation is not supported."
#	endif
#else
#	error "This header is only for C++."
#endif

#if YB_IMPL_MSCPP >= 1400
//! \since build 454
#	ifndef _CRT_SECURE_NO_WARNINGS
#		define _CRT_SECURE_NO_WARNINGS
#	endif
//! \since build 835
#	ifndef _SCL_SECURE_NO_WARNINGS
#		define _SCL_SECURE_NO_WARNINGS
#	endif
#endif
//@}

/*!
\brief 特性检测宏补充定义：若不可用则替换为预处理记号 0 。
\since build 484
*/
//@{
//! \since build 628
#ifndef __has_attribute
#	define __has_attribute(...) 0
#endif

//! \since build 535
#ifndef __has_builtin
#	define __has_builtin(...) 0
#endif

//! \since build 591
#ifndef __has_cpp_attribute
#	define __has_cpp_attribute(...) 0
#endif

#ifndef __has_extension
#	define __has_extension(...) 0
#endif

#ifndef __has_feature
#	define __has_feature(...) 0
#endif

//! \since build 831
#ifndef __has_include
#	define __has_include(...) 0
#endif
//@}

//! \see https://docs.microsoft.com/en-us/cpp/visual-cpp-language-conformance 。
//@{
/*!
\brief \c constexpr 特性测试宏。
\see WG21 P0941R2 2.2 。
\since build 628
*/
//! \since build 628
//@{
#ifndef __cpp_constexpr
#	if __has_feature(cxx_relaxed_constexpr) || __cplusplus >= 201304L
#		define __cpp_constexpr 201304L
#	elif __has_feature(cxx_constexpr) || __cplusplus >= 200704L \
	|| YB_IMPL_GNUCPP >= 40600 || YB_IMPL_MSCPP >= 1900
#		define __cpp_constexpr 200704L
#	endif
#endif
//@}
/*!
\since build 833
\see P0136R1 。
\see https://clang.llvm.org/docs/LanguageExtensions.html 。
\see https://gcc.gnu.org/projects/cxx-status.html 。
\see https://msdn.microsoft.com/en-us/library/hh409293.aspx 。
*/
//@{
#ifndef __cpp_inheriting_constructors
#	if (YB_IMPL_MSCPP >= 1914 && _MSVC_LANG >= 201511) || __cplusplus >= 201511L
#		define __cpp_inheriting_constructors 201511L
#	elif __has_feature(cxx_inheriting_constructors) || YB_IMPL_MSCPP > 1900 \
	|| __cplusplus >= 200802L
#		define __cpp_inheriting_constructors 200802L
#	endif
#endif
//@}
/*!
\see https://reviews.llvm.org/D32950 。
\see https://blogs.msdn.microsoft.com/vcblog/2018/04/09/msvc-now-correctly-reports-__cplusplus/ 。
\since build 831
*/
//@{
// NOTE: Microsoft VC++ compiler has support the feature since version 15.5.
//	However,  the %__cplusplus can only work after 15.7 Preview 3 with
//	additional option It can be more precise when %_MSC_FULL_VER is
//	incorperated, but there is lack of documentation so additional test work is
//	needed to make the improvement.
#ifndef __cpp_inline_variables
#	if __has_feature(cxx_inline_variables) \
	|| __has_extension(cxx_inline_variables) || YB_IMPL_MSCPP > 1912 \
	|| __cplusplus >= 201606L
#		define __cpp_inline_variables 201606L
#	endif
#endif
//@}
/*!
\see WG21 P0012R1 。
\see https://blogs.msdn.microsoft.com/vcblog/2018/04/09/msvc-now-correctly-reports-__cplusplus/ 。
\since build 845
*/
//@{
// NOTE: Ditto about Microsoft VC++ support as %__cpp_inline_variables.
#ifndef __cpp_noexcept_function_type
#	if __has_feature(cxx_noexcept_function_type) \
	|| __has_extension(cxx_noexcept_function_type) || YB_IMPL_MSCPP > 1912 \
	|| __cplusplus >= 201510L
#		define __cpp_noexcept_function_type 201510L
#	endif
#endif
//@}
//@}

#include <cstddef> // for __cpp_lib_byte, std::byte, std::nullptr_t,
//	std::size_t, std::ptrdiff_t, offsetof;
#include <cstdlib> // for std::abort;
#include <climits> // for CHAR_BIT;
#include <cstdint> // for std::uint8_t;
#include <cassert> // for assert;
#include <cwchar> // for std::wint_t;
#include <utility> // for std::forward;
#include <type_traits> // for std::is_class, std::is_standard_layout;

/*!
\see WG21 P0941R2 2.2 。
\see https://docs.microsoft.com/en-us/cpp/visual-cpp-language-conformance 。
*/
//@{
/*!
\brief \<cstddef\> 特性测试宏。
\since build 832
\see https://blogs.msdn.microsoft.com/vcblog/2017/05/10/c17-features-in-vs-2017-3/ 。
\see https://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html#status.iso.2017 。
*/
//@{
#ifndef __cpp_lib_byte
#	if ((YB_IMPL_MSCPP >= 1911 && _MSVC_LANG >= 201603) \
	|| YB_IMPL_GCC >= 70100 || __cplusplus >= 201603L) \
	&& !(YB_IMPL_MSCPP >= 1911 && _MSVC_LANG >= 201603 && _HAS_STD_BYTE != 0)
#		define __cpp_lib_byte 201603L
#	endif
#endif
//@}
/*!
\brief \<type_traits\> 特性测试宏。
\since build 679
\see https://blogs.msdn.microsoft.com/vcblog/2016/10/11/c1417-features-and-stl-fixes-in-vs-15-preview-5/ 。
*/
//@{
#ifndef __cpp_lib_bool_constant
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201505L
#		define __cpp_lib_bool_constant 201505L
#	endif
#endif
//! \since build 832
//@{
#ifndef __cpp_lib_is_invocable
#	if (YB_IMPL_MSCPP >= 1911 && _MSVC_LANG >= 201703) || __cplusplus >= 201703L
#		define __cpp_lib_is_invocable 201703L
#	endif
#endif
#ifndef __cpp_lib_is_null_pointer
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201309L
#		define __cpp_lib_is_null_pointer 201309L
#	endif
#endif
//! \since build 834
//@{
#ifndef __cpp_lib_is_swappable
#	if YB_IMPL_MSCPP > 1900 || __cplusplus >= 201603L
#		define __cpp_lib_is_swappable 201603L
#	endif
#endif
//@}
#ifndef __cpp_lib_transformation_trait_aliases
#	if YB_IMPL_MSCPP >= 1800 || __cplusplus >= 201304L
#		define __cpp_lib_transformation_trait_aliases 201304L
#	endif
#endif
//@}
#ifndef __cpp_lib_void_t
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201411L
#		define __cpp_lib_void_t 201411L
#	endif
#endif
//@}
/*!
\brief \<utility\> 特性测试宏。
\see https://docs.microsoft.com/en-us/cpp/visual-cpp-language-conformance 。
\since build 628
*/
//@{
//! \since build 833
//@{
#ifndef __cpp_lib_as_const
#	if YB_IMPL_MSCPP >= 1911 || __cplusplus >= 201510L
#		define __cpp_lib_as_const 201510L
#	endif
#endif
//@}
#ifndef __cpp_lib_exchange_function
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201304L
#		define __cpp_lib_exchange_function 201304L
#	endif
#endif
#ifndef __cpp_lib_integer_sequence
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201304L
#		define __cpp_lib_integer_sequence 201304L
#	endif
#endif
#ifndef __cpp_lib_tuple_element_t
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201402L
#		define __cpp_lib_tuple_element_t 201402L
#	endif
#endif
//@}
//@}


/*!	\defgroup preprocessor_helpers Perprocessor Helpers
\brief 预处理器通用助手宏。
\since build 454
*/
//@{
/*!
\brief 替换列表。
\note 通过括号保护，可用于传递带逗号的参数。
\since build 689
*/
#define YPP_Args(...) __VA_ARGS__

//! \brief 替换为空的预处理记号。
#define YPP_Empty

/*!
\brief 替换为逗号的预处理记号。
\note 可用于代替宏的实际参数中出现的逗号。
*/
#define YPP_Comma ,

/*!
\brief 记号连接。
\sa YPP_Join
*/
#define YPP_Concat(_x, _y) _x ## _y

/*!
\brief 带宏替换的记号连接。
\see WG21 N4140 16.3.3[cpp.concat]/3 。
\see http://gcc.gnu.org/onlinedocs/cpp/Concatenation.html 。
\see https://www.securecoding.cert.org/confluence/display/cplusplus/PRE05-CPP.+Understand+macro+replacement+when+concatenating+tokens+or+performing+stringification 。

注意 ISO C++ 未确定宏定义内 # 和 ## 操作符求值顺序。
注意宏定义内 ## 操作符修饰的形式参数为宏时，此宏不会被展开。
*/
#define YPP_Join(_x, _y) YPP_Concat(_x, _y)

/*!
\brief 替换记号为字符串。
\since build 715
*/
#define YPP_Stringify(_x) #_x
//@}


/*!
\brief 实现标签。
\since build 474
\todo 检查语言实现的必要支持：可变参数宏。

接口代码中标注不作为公开接口实现而不应被用户依赖具体内容的部分的宏。
类似于 ISO C++ 中的 \c unspecified 或标识为 <tt>exposition only</tt> 部分的代码。
保证记号被原样替换。
以 \c yimpl() 的形式使用时可用于表示实现可选扩展的接口，用于序列末尾等。
*/
#define yimpl(...) __VA_ARGS__


/*!	\defgroup YBase_replacement_features YBase Replacement features
\brief YBase 替代特性。
\since build 837

YBase 提供的替代 ISO C++ 特性的接口。
若接口是类或类模板，可能具有扩展特性的成员以及与使用相关类型作为参数的函数或函数模板。
*/

/*!	\defgroup YBase_replacement_extensions YBase Replacement extensions
\brief YBase 替代扩展。
\since build 837

YBase 提供的替代 ISO C++ 扩展特性的接口。
不是原始的被替代的接口但在包含其规格（如标准库头）内的接口。
*/

/*!	\defgroup lang_impl_features Language Implementation Features
\brief 语言实现的特性。
\since build 294
*/
//@{
/*!
\def YB_HAS_ALIGNAS
\brief 内建 alignas 支持。
\since build 389
*/
#undef YB_HAS_ALIGNAS
#define YB_HAS_ALIGNAS \
	(__has_feature(cxx_alignas) || __has_extension(cxx_alignas) \
		|| YB_IMPL_GNUCPP >= 40800)

/*!
\def YB_HAS_ALIGNOF
\brief 内建 alignof 支持。
\since build 315
*/
#undef YB_HAS_ALIGNOF
#define YB_HAS_ALIGNOF (__cplusplus >= 201103L || YB_IMPL_GNUCPP >= 40500)

/*!
\def YB_HAS_BUILTIN_NULLPTR
\brief 内建 nullptr 支持。
\since build 313
*/
#undef YB_HAS_BUILTIN_NULLPTR
#define YB_HAS_BUILTIN_NULLPTR \
	(__has_feature(cxx_nullptr) || __has_extension(cxx_nullptr) \
		|| __cplusplus >= 201103L || YB_IMPL_GNUCPP >= 40600 || \
		YB_IMPL_MSCPP >= 1600)

/*!
\def YB_HAS_NOEXCPT
\brief noexcept 支持。
\since build 319
*/
#undef YB_HAS_NOEXCEPT
#define YB_HAS_NOEXCEPT \
	(__has_feature(cxx_noexcept) || __has_extension(cxx_noexcept) || \
		__cplusplus >= 201103L || YB_IMPL_GNUCPP >= 40600 \
		|| YB_IMPL_MSCPP >= 1900)

/*!
\def YB_HAS_THREAD_LOCAL
\brief thread_local 支持。
\see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=1773 。
\since build 425
*/
#undef YB_HAS_THREAD_LOCAL
#define YB_HAS_THREAD_LOCAL \
	(__has_feature(cxx_thread_local) || (__cplusplus >= 201103L \
		&& !YB_IMPL_GNUCPP) || (YB_IMPL_GNUCPP >= 40800 && _GLIBCXX_HAVE_TLS))
//@}


/*!
\def YB_ABORT
\brief 非正常终止程序。
\note 可能使用体系结构相关实现或标准库 std::abort 函数等。
\since build 535
*/
#if __has_builtin(__builtin_trap) || YB_IMPL_GNUCPP >= 40203
#	define YB_ABORT __builtin_trap()
#else
#	define YB_ABORT std::abort()
#endif


/*!	\defgroup language_compatibility_features Language Compatibility Features
\ingroup YBase_replacement_features
\ingroup YBase_replacement_extensions
\brief 语言兼容特性。
\since build 835
*/

/*!	\defgroup language_implementation_hints Language Implementation Hints
\brief 语言实现的提供的附加提示。
\since build 837

保证忽略时不导致运行时语义差异的提示，主要用于便于实现可能的优化。
*/
//@{
/*!
\def YB_ATTR
\brief GNU 风格属性。
\warning 不检查指令。用户应验证可能使用的指令中的标识符在宏替换后能保持正确。
\since build 373
*/
#if YB_IMPL_GNUC >= 20500
#	define YB_ATTR(...) __attribute__((__VA_ARGS__))
#else
#	define YB_ATTR(...)
#endif

/*!
\def YB_ATTR_STD
\brief C++ 标准属性。
\note 注意和 GNU 风格不同，在使用时受限，如不能修饰 lambda 表达式非类型的声明。
\warning 不检查指令。用户应验证可能使用的指令中的标识符在宏替换后能保持正确。
\since build 605
*/
#if __cpp_attributes >= 200809L || __has_feature(cxx_attributes)
#	define YB_ATTR_STD(...) [[__VA_ARGS__]]
#else
#	define YB_ATTR_STD(...)
#endif

/*!
\def YB_ATTR_fallthrough
\brief 用于 switch 语句的 fallthrough 属性。
\since build 793
\see WG21 P0188R1 。
*/
#if __has_cpp_attribute(fallthrough) \
	&& (!YB_IMPL_CLANGPP || __cplusplus >= 201703L)
// XXX: This is ruled out for [-Wc++17-extensions] in Clang++.
#	define YB_ATTR_fallthrough YB_ATTR_STD(fallthrough)
#elif __has_cpp_attribute(clang::fallthrough)
#	define YB_ATTR_fallthrough YB_ATTR_STD(clang::fallthrough)
#elif __has_cpp_attribute(gnu::fallthrough)
#	define YB_ATTR_fallthrough YB_ATTR_STD(gnu::fallthrough)
#elif __has_attribute(fallthrough)
#	define YB_ATTR_fallthrough YB_ATTR(fallthrough)
#else
#	define YB_ATTR_fallthrough
#endif

/*!
\def YB_ATTR_gnu_printf
\brief GNU 风格 printf 属性。
\note Clang++ 未支持此属性，警告 [-Wignored-attributes] 。
\since build 615
*/
#if YB_IMPL_GNUC >= 40400 && !YB_IMPL_CLANGPP
#	define YB_ATTR_gnu_printf(...) \
	YB_ATTR(__format__ (__gnu_printf__, __VA_ARGS__))
#elif YB_IMPL_GNUC >= 20604 && !YB_IMPL_CLANGPP
#	define YB_ATTR_gnu_printf(...) YB_ATTR(__format__ (__printf__, __VA_ARGS__))
#else
#	define YB_ATTR_gnu_printf(...)
#endif

/*!
\def YB_ATTR_maybe_unused
\brief 标记可被忽略的属性。
\since build 833
\see WG21 P0212R1 。
\see https://clang.llvm.org/docs/AttributeReference.html#maybe-unused-unused-gnu-unused 。
\see https://gcc.gnu.org/projects/cxx-status.html 。
*/
#if __has_cpp_attribute(maybe_unused)
#	define YB_ATTR_maybe_unused YB_ATTR_STD(maybe_unused)
#elif __has_cpp_attribute(gnu::unused)
#	define YB_ATTR_maybe_unused YB_ATTR_STD(gnu::unused)
#elif __has_attribute(unused)
#	define YB_ATTR_maybe_unused YB_ATTR(unused)
#else
#	define YB_ATTR_maybe_unused
#endif

/*!
\def YB_ATTR_nodiscard
\brief 标记返回值不被忽略的属性。
\since build 833
\see WG21 P0189R1 。
\see https://clang.llvm.org/docs/AttributeReference.html#nodiscard-warn-unused-result-clang-warn-unused-result-gnu-warn-unused-result 。
\see https://gcc.gnu.org/projects/cxx-status.html 。
*/
#if __has_cpp_attribute(nodiscard) \
	&& (!YB_IMPL_CLANGPP || __cplusplus >= 201703L)
// XXX: This is ruled out for [-Wc++17-extensions] in Clang++.
#	define YB_ATTR_nodiscard YB_ATTR_STD(nodiscard)
#elif __has_cpp_attribute(clang::warn_unused_result)
#	define YB_ATTR_nodiscard YB_ATTR_STD(clang::warn_unused_result)
#elif __has_cpp_attribute(gnu::warn_unused_result)
#	define YB_ATTR_nodiscard YB_ATTR_STD(gnu::warn_unused_result)
#elif __has_attribute(warn_unused_result)
#	define YB_ATTR_nodiscard YB_ATTR(warn_unused_result)
#else
#	define YB_ATTR_nodiscard
#endif

/*!
\brief 修饰类定义要求实现不生成初始化动态类型信息。
\note 只适用于定义不在初始化派生类对象时的动态类型的类，否则行为未定义。
\see https://docs.microsoft.com/en-us/cpp/cpp/novtable 。
\see http://releases.llvm.org/3.7.0/tools/clang/docs/AttributeReference.html 。
\see https://clang.llvm.org/docs/AttributeReference.html#novtable 。
\see http://lists.llvm.org/pipermail/cfe-commits/Week-of-Mon-20150720/133747.html 。
\since build 842
\todo 确认可忽略不支持情形的 Clang++ 最近可用的版本。

修饰类定义，允许实现省略若干在初始化类的对象时的隐含初始化。
这些初始化通常用于保证派生类的动态类型正确实现 ISO C++ 调用基类虚函数的语义。
只适用于定义不在初始化对象时调用基类虚函数和使用 RTTI
	（作为操作数使用 dynamic_cast 或 typeid ）的类，否则行为未定义。
一般用于只有纯虚函数（除析构函数是非纯的虚函数）的基类。
派生类的构造函数使用这些依赖其动态类型的操作已引起未定义行为，
	参见 ISO C++ [abstract.class]/6 ；
	因此当基类只有纯虚函数时，不改变程序的预期语义，而仅用于优化。
其它情形派生类需要排除更多的限制。
当前 Microsoft VC++ 或 Clang++ （使用 Microsoft ABI ）支持这项特性。
对不支持的情形，近期版本 Clang++ 可直接忽略。
*/
#if __has_cpp_attribute(novtable) || YB_IMPL_MSVC >= 1100 \
	|| YB_IMPL_CLANGPP >= 30700
#	define YB_ATTR_novtable YB_ATTR(novtable)
#else
#	define YB_ATTR_novtable
#endif

/*!
\def YB_ATTR_returns_nonnull
\brief 指示返回非空属性。
\since build 676
\see http://reviews.llvm.org/rL199626 。
\see http://reviews.llvm.org/rL199790 。
\todo 确认 Clang++ 最低可用的版本。
*/
#if __has_attribute(returns_nonnull) || YB_IMPL_GNUC >= 40900 \
	|| YB_IMPL_CLANGPP >= 30500
#	define YB_ATTR_returns_nonnull YB_ATTR(returns_nonnull)
#else
#	define YB_ATTR_returns_nonnull
#endif

/*!
\def YB_ALLOCATOR
\brief 指示返回指针的函数或函数模板若为非空值则对应的存储不和其它对象指针共享别名。
\note 指示行为类似 std::malloc 或 std::calloc 等的函数。
\warning 要求满足指示的假定，否则行为未定义。
\see https://docs.microsoft.com/en-us/cpp/cpp/restrict?view=vs-2017 。
\see https://blogs.msdn.microsoft.com/vcblog/2015/10/21/memory-profiling-in-visual-c-2015/ 。
\see https://gitlab.gnome.org/GNOME/glib/issues/1465 。
\since build 373

指示函数若返回非空指针，返回的指针指向的存储中的指针不是其它任何有效对象指针的别名，
且指针指向的存储内容不由其它存储决定。
*/
#if YB_IMPL_MSCPP >= 1900 && !defined(__EDG__) && !defined _CORECRT_BUILD
#	define YB_ALLOCATOR YB_ATTR(allocator) YB_ATTR(restrict)
#elif YB_IMPL_MSCPP >= 1400
#	define YB_ALLOCATOR YB_ATTR_nodiscard YB_ATTR(restrict)
#elif __has_attribute(__malloc__) || YB_IMPL_GNUCPP >= 20296
#	define YB_ALLOCATOR YB_ATTR_nodiscard YB_ATTR(__malloc__)
#else
#	define YB_ALLOCATOR YB_ATTR_nodiscard
#endif

/*!
\def YB_ASSUME(expr)
\brief 假定表达式总是成立。
\note 若假定成立有利于优化。
\warning 若假定不成立则行为未定义。
\since build 535
*/
#if YB_IMPL_MSCPP >= 1200
#	define YB_ASSUME(_expr) __assume(_expr)
#elif __has_builtin(__builtin_unreachable) || YB_IMPL_GNUCPP >= 40500
#	define YB_ASSUME(_expr) ((_expr) ? void(0) : __builtin_unreachable())
#else
#	define YB_ASSUME(_expr) ((_expr) ? void(0) : YB_ABORT)
#endif

/*!
\def YB_EXPECT(expr, constant)
\def YB_LIKELY(expr)
\def YB_UNLIKELY(expr)
\brief 分支预测提示。
\since build 313
*/
#if __has_builtin(__builtin_expect) || YB_IMPL_GNUCPP >= 29600
#	define YB_EXPECT(_expr, _constant) (__builtin_expect(_expr, _constant))
#	define YB_LIKELY(_expr) (__builtin_expect(bool(_expr), 1))
#	define YB_UNLIKELY(_expr) (__builtin_expect(bool(_expr), 0))
#else
#	define YB_EXPECT(_expr, _constant) (_expr)
#	define YB_LIKELY(_expr) (_expr)
#	define YB_UNLIKELY(_expr) (_expr)
#endif

/*!
\def YB_FLATTEN
\brief 标记函数尽可能内联定义内的调用。
\see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=48731 。
\since build 646
*/
#if (__has_attribute(__flatten__) || YB_IMPL_GNUCPP >= 40102) \
	&& YB_IMPL_GNUCPP != 40600
#	define YB_FLATTEN YB_ATTR(__flatten__)
#else
#	define YB_FLATTEN
#endif

/*!
\def YB_NONNULL
\brief 指定假定保证不为空指针的函数参数。
\warning 当指定的函数实际为空时行为未定义。
\since build 524
*/
#if YB_IMPL_GNUCPP >= 30300
#	define YB_NONNULL(...) YB_ATTR(__nonnull__(__VA_ARGS__))
#else
#	define YB_NONNULL(...)
#endif

/*!
\def YB_NORETURN
\brief 指定无返回值函数。
\note 不保证适用修饰 lambda 。这种情况可使用 YB_ATTR(noreturn) 代替。
\warning 当指定的函数调用实际返回时行为未定义。
\see https://msdn.microsoft.com/en-us/library/hh567368.aspx 。
\since build 396
*/
#if __has_cpp_attribute(noreturn) >= 200809 || YB_IMPL_MSCPP >= 1900 \
	|| YB_IMPL_GNUCPP >= 40800 || __cplusplus >= 201103L
#	define YB_NORETURN [[noreturn]]
#elif __has_attribute(__noreturn__) || YB_IMPL_GNUCPP >= 20296
#	define YB_NORETURN YB_ATTR(__noreturn__)
#elif YB_IMPL_MSCPP >= 1200
#	define YB_NORETURN __declspec(noreturn)
#else
#	define YB_NORETURN
#endif

#if YB_IMPL_MSCPP >= 1200
//! \since build 454
#	pragma warning(disable: 4646)
#endif

/*!
\post 函数外可访问的存储保持不变。
\warning 用户应保证省略假定的等价副作用不影响程序的可观察行为，否则行为未定义。
\warning 不同翻译单元内的函数应具有相同的指示，否则行为未定义。
\note 假定函数保证至少有一条路径可返回；返回类型 void 时无意义。
\note 假定函数无外部可见的副作用：局部记忆化合并重复调用后不改变可观察行为。
\note 假定以外的等价的副作用可能按 as-if 规则被省略；指示可影响未指定的语义等价性。
\note 被忽略的副作用可能是控制副作用，如依赖 C++ 语义下外部不可见状态的异常抛出。
\note 无视 virtual 关键字，但修饰基类函数时也表示对覆盖版本的要求。
\note 当前对 virtual 关键字的行为不是实现公开的接口，由分析具体实现保证。
\see WG21 P0078R0 。
*/
//@{
/*!
\def YB_PURE
\brief 指示函数或函数模板实例为纯函数。
\since build 373

指示函数或函数模板的求值仅用于计算返回值，无影响其它顶层块作用域外存储的副作用，
且返回值只依赖参数和/或编译时确定内存位置（如静态存储的对象的）存储的值。
假定条件包括：
不修改函数外部的存储；
不访问函数外部 volatile 存储；
不调用不可被 YB_PURE 安全指定的函数。
*/
#if __has_attribute(__pure__) || YB_IMPL_GNUCPP >= 20296
#	define YB_PURE YB_ATTR(__pure__)
#else
#	define YB_PURE
#endif

/*!
\def YB_STATELESS
\brief 指示函数或函数模板实例为无状态函数。
\pre 若参数是对象指针或引用类型，应保证指向或引用的对象是其它参数，或者不被使用。
\since build 373

指示函数或函数模板的求值仅用于计算返回值，无影响其它顶层块作用域外存储的副作用，
假定函数调用的结果总是相同：返回值总是不可分辨的右值或指示同一个内存位置的左值。
	任意以一次调用结果替代调用或合并重复调用时不改变可观察行为。
且返回值只依赖参数的值，和其它存储无关。
假定条件包括：
不访问函数外部的存储；
不调用不可被 YB_STATELESS 安全指定的函数。
可被安全指定的函数或函数模板是 YB_PURE 限定的函数或函数模板的真子集。
*/
#if __has_attribute(__const__) || YB_IMPL_GNUCPP >= 20500
#	define YB_STATELESS YB_ATTR(__const__)
#else
#	define YB_STATELESS
#endif
//@}
//@}


/*!	\defgroup lib_options Library Options
\brief 库选项。
\since build 373
*/
//@{
/*!
\def YB_DLL
\brief 使用 YBase 动态链接库。
\since build 362
*/
/*!
\def YB_BUILD_DLL
\brief 构建 YBase 动态链接库。
\since build 362
*/
/*!
\def YB_API
\brief YBase 应用程序编程接口：用于向库文件约定链接。
\since build 362
\todo 判断语言特性支持。
*/
#if defined(YB_DLL) && defined(YB_BUILD_DLL)
#	error "DLL could not be built and used at the same time."
#endif

#if YB_IMPL_MSCPP \
	|| (YB_IMPL_GNUCPP && (defined(__MINGW32__) || defined(__CYGWIN__)))
#	ifdef YB_DLL
#		define YB_API __declspec(dllimport)
#	elif defined(YB_BUILD_DLL)
#		define YB_API __declspec(dllexport)
#	else
#		define YB_API
#	endif
#elif defined(YB_BUILD_DLL) && (__has_attribute(__visibility__) \
	|| YB_IMPL_GNUCPP >= 40000 || YB_IMPL_CLANGPP)
#	define YB_API YB_ATTR(__visibility__("default"))
#else
#	define YB_API
#endif


/*!
\def YB_Use_DynamicExceptionSpecification
\brief 使用 YBase 动态异常规范。
\since build 595
*/
#if false && !defined(NDEBUG)
#	define YB_Use_DynamicExceptionSpecification 1
#endif

/*!
\def YB_Use_YAssert
\brief 使用断言。
\since build 313
*/
/*!
\def YB_Use_YTrace
\brief 使用调试跟踪记录。
\since build 313
*/
/*!
\def YB_Use_StrictNoThrow
\brief 使用严格无异常抛出规范。
\since build 595
*/
#ifndef NDEBUG
#	ifndef YB_Use_YAssert
#		define YB_Use_YAssert 1
#	endif
#elif !defined(YB_Use_StrictNoThrow)
#	define YB_Use_StrictNoThrow 1
#endif
#define YB_Use_YTrace 1
//@}


/*!	\defgroup YBase_pseudo_keyword YBase Specified Pseudo-Keywords
\brief YBase 指定的替代关键字。
\since build 362
*/
//@{
/*!
\def yalignas
\brief 指定特定类型的对齐。
\note 同 C++11 alignas 作用于类型时的语义。
\since build 591
\todo 判断没有内建关键字支持时属性存在及没有属性支持时的其它情况。
*/
#if YB_HAS_ALIGNAS
#	define yalignas alignas
#	define yalignas_type alignas
#else
#	define yalignas(_expr) YB_ATTR(__aligned__(_expr))
#	define yalignas_type(_type) YB_ATTR(__aligned__(__alignof(_type)))
#endif

/*!
\def yalignof
\brief 指定特定类型的对齐。
\note 同 C++11 alignof 作用于类型时的语义。
\since build 315
*/
#if YB_HAS_ALIGNOF
#	define yalignof alignof
#else
#	define yalignof(_type) std::alignment_of<_type>::value
#endif

/*!
\def yconstexpr
\brief 指定编译时常量表达式。
\note 同 C++11 constepxr 作用于编译时常量的语义。
\since build 246
*/
/*!
\def yconstfn
\brief 指定编译时常量函数。
\note 同 C++11 constepxr 作用于编译时常量函数的语义。
\since build 266
*/
#if __cpp_constexpr >= 200704L
#	define yconstexpr constexpr
#	define yconstfn constexpr
#else
#	define yconstexpr
#	define yconstfn inline
#endif

/*!
\def yconstexpr_inline
\brief 可选的内联 constexpr 变量。
\warning 不应依赖变量的链接以避免可能造成违反 ODR 。
\since build 831
*/
#if __cpp_inline_variables >= 201606L
#	define yconstexpr_inline inline yconstexpr
#else
#	define yconstexpr_inline yconstexpr
#endif

/*!
\def yconstfn_relaxed
\brief 指定编译时没有 C++11 限制和隐式成员 const 的常量函数。
\note 同 C++14 constepxr 作用于编译时常量函数的语义。
\since build 591
*/
#if __cpp_constexpr >= 201304L
#	define yconstfn_relaxed constexpr
#else
#	define yconstfn_relaxed inline
#endif

/*!
\def yfname
\brief 展开为表示函数名的预定义变量的宏。
\since build 628
\todo 判断语言实现版本。
*/
#if YB_IMPL_MSCPP || __INTEL_COMPILER >= 600 || __IBMCPP__ >= 500
#	define yfname __FUNCTION__
#elif __BORLANDC__ >= 0x550
#	define yfname __FUNC__
#elif __cplusplus >= 201103 || __STDC_VERSION__ >= 199901
#	define yfname __func__
#else
#	define yfname "<unknown-fn>"
#endif

/*!
\def yfsig
\brief 展开为表示函数签名的预定义变量的宏。
\since build 628
\todo 判断语言实现版本。
*/
#if YB_IMPL_GNUCPP || YB_IMPL_CLANGPP
#	define yfsig __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#	define yfsig __FUNCSIG__
#elif __INTEL_COMPILER >= 600 || __IBMCPP__ >= 500
#	define yfsig __FUNCTION__
#elif __BORLANDC__ >= 0x550
#	define yfsig __FUNC__
#elif __cplusplus >= 201103 || __STDC_VERSION__ >= 199901
#	define yfsig __func__
#else
#	define yfsig "<unknown-fsig>"
#endif

/*!
\def ynoexcept
\brief YSLib 无异常抛出保证：指定特定的异常规范。
\since build 319
*/
/*!
\def ynoexcept_assert
\brief 表达式 \c noexcept 静态断言。
\since build 586
*/
#if YB_HAS_NOEXCEPT
#	define ynoexcept noexcept
#	define ynoexcept_assert(_msg, ...) \
	static_assert(noexcept(__VA_ARGS__), _msg)
#else
#	define ynoexcept(...)
#	define ynoexcept_assert(_msg, ...)
#endif

/*!
\def ynoexcept_param
\brief 影响函数类型的 noexcept 限定符参数。
\since build 845
*/
/*!
\def ynoexcept_qual
\brief 影响函数类型的 noexcept 限定符表达式。
\since build 845
*/
#if __cpp_noexcept_function_type >= 201510L
#	if !YB_HAS_NOEXCEPT
#		error "Invalid language implementation found."
#	endif
#	define ynoexcept_param(_n) , bool _n
#	define ynoexcept_qual(...) noexcept(__VA_ARGS__)
#else
#	define ynoexcept_param(_n)
#	define ynoexcept_qual(...)
#endif

/*!
\def ynoexcept_spec
\brief 表达式 \c noexcept 异常规范。
\since build 586
*/
#define ynoexcept_spec(...) ynoexcept(noexcept(__VA_ARGS__))

/*!
\def ynothrow
\brief YSLib 无异常抛出保证：若支持 noexcept 关键字，
	指定特定的 noexcept 异常规范。
\note YB_IMPL_MSCPP >= 1200 时支持 __declspec(nothrow) 行为和 throw() 基本一致，
	但语法（顺序）不同。
\note 指定 ynothrow 的函数具有 wide constraint ，
	即保证违反前置条件不引起未定义行为。
\sa ynothrowv
\sa ynoexcept
*/
#if YB_HAS_NOEXCEPT
#	define ynothrow ynoexcept
#elif YB_IMPL_GNUCPP >= 30300
#	define ynothrow __attribute__((nothrow))
#else
#	define ynothrow ythrow()
#endif

/*!
\def ynothrowv
\brief YSLib 无异常抛出保证验证：有条件地使用无异常抛出规范。
\note 指定 ynothrowv 的函数具有 narrow contract 形式的约束条件，
	即不保证违反前置条件时不引起未定义行为。
\sa ynothrow
\since build 461

按 WG21 N3248 要求，表示 narrow contract 形式的约束条件的无异常抛出接口。
对应接口违反约束可引起未定义行为。
因为可能显著改变程序的可观察行为，需要允许抛出异常进行验证时不适用。
除非能静态验证不抛出异常，一般只应直接或间接调用 ynothrow 安全修饰的函数。
验证结束后，确保不存在未定义行为时可以启用以提升性能。
*/
#if YB_Use_StrictNoThrow
#	define ynothrowv ynothrow
#else
#	define ynothrowv
#endif

/*!
\def ythread
\brief 线程局部存储：若实现支持，指定为 \c thread_local 。
\warning MinGW GCC 使用的 emutls 实现缺陷导致静态初始化可能失败。
\since build 425
\todo 加入 \c __thread 和 \c __declspec(thread) 。
*/
#if YB_HAS_THREAD_LOCAL && defined(_MT)
#	define ythread thread_local
#else
#	define ythread static
#endif

/*!
\def ythrow
\brief YSLib 动态异常规范：根据是否使用异常规范宏指定或忽略动态异常规范。
\note 动态异常规范已从 ISO C++17 移除，用户代码不应假定支持。
\note 主要为可读性保留， ythrow = "yielded throwing" 。
\see WG21 P0003R5 。
*/
#if YB_Use_DynamicExceptionSpecification
#	define ythrow throw
#else
#	define ythrow(...)
#endif
//@}


namespace ystdex
{

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
using octet = std::uint8_t;
#	else
using octet = void;
#endif

using std::ptrdiff_t;
using std::size_t;
using std::wint_t;

#if YB_HAS_BUILTIN_NULLPTR
using std::nullptr_t;
#else
/*!
\brief 空指针类。
\see https://en.wikibooks.org/wiki/More_C++_Idioms/nullptr 。
*/
const class nullptr_t
{
public:
	//! \brief 转换任意类型至空非成员或静态成员指针。
	template<typename _type>
	yconstfn
	operator _type*() const
	{
		return 0;
	}

	//! \brief 转换任意类型至空非静态成员指针。
	template<class _tClass, typename _type>
	yconstfn
	operator _type _tClass::*() const
	{
		return 0;
	}
	//! \brief 支持关系运算符重载。
	template<typename _type>
	yconstfn bool
	equals(const _type& rhs) const
	{
		return rhs == 0;
	}

	//! \brief 禁止取 nullptr 的指针。
	void operator&() const = delete;
} nullptr = {};

//! \since build 316
//@{
template<typename _type>
yconstfn bool
operator==(nullptr_t lhs, const _type& rhs)
{
	return lhs.equals(rhs);
}
template<typename _type>
yconstfn bool
operator==(const _type& lhs, nullptr_t rhs)
{
	return rhs.equals(lhs);
}

template<typename _type>
yconstfn bool
operator!=(nullptr_t lhs, const _type& rhs)
{
	return !lhs.equals(rhs);
}
template<typename _type>
yconstfn bool
operator!=(const _type& lhs, nullptr_t rhs)
{
	return !rhs.equals(lhs);
}
//@}
#endif


/*!	\defgroup customization Customization
\brief 定制化接口。

用于用户代码定制的接口。
*/

/*!	\defgroup customization_points Customization Points
\ingroup customization
\brief 定制点。
\see WG21 N4381 。
\see WG21 N4778 [namespace.std] 。
\since build 845

用于用户代码定制的调用接口。
和标准库的概念类似，但适用于 ystdex 和相关的命名空间；
	且外延除函数模板外，还包含类模板。
*/

/*!	\defgroup helper_functions Helper Functions
\brief 助手功能。
\since build 243

仅帮助简化编码形式或确定接口，并不包含编译期之后逻辑功能实现的代码设施。
*/

/*!	\defgroup tags Tags
\brief 标签。
\note 可能是类型或元类型。
\since build 447

用于标记重载或其它类型接口的类型或模板。
*/

/*!	\defgroup traits Traits
\brief 特征。
\since build 845

具有特定成员，提供翻译时确定的信息的类型或木板。
*/

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
\brief 成员偏移计算静态类型检查。
\see ISO C++ 18.2/4 。
\since build 325
*/
template<bool _bMemObjPtr, bool _bNoExcept, class _type>
class offsetof_check
{
	static_assert(std::is_class<_type>::value, "Non class type found.");
	static_assert(std::is_standard_layout<_type>::value,
		"Non standard layout type found.");
	static_assert(_bMemObjPtr, "Non-static member object violation found.");
	static_assert(_bNoExcept, "Exception guarantee violation found.");
};


/*!
\ingroup YBase_pseduo_keyword
\brief 标记未使用的表达式。
\note 显式转换为 void 类型以标记表达式未被作为子表达式使用，可避免某些实现的警告。
\warning 避免在表达式中误用逗号。
\since build 435
*/
#define yunused(...) static_cast<void>(__VA_ARGS__)

/*!
\ingroup YBase_pseudo_keyword
\brief 带有静态类型检查的成员偏移计算。
\see ISO C++ 18.2/4 。
\note 某些 G++ 和 Clang++ 版本可使用 __builtin_offsetof 及 -Winvalid-offsetof ，
	但可移植性较差。
\since build 325
*/
#define yoffsetof(_type, _member) \
	(decltype(sizeof(ystdex::offsetof_check<std::is_member_object_pointer< \
	decltype(&_type::_member)>::value, ynoexcept(offsetof(_type, _member)), \
	_type>))(offsetof(_type, _member)))

/*!
\ingroup YBase_pseudo_keyword
\brief 根据参数类型使用 std::forward 传递对应参数。
\since build 245

传递参数：按类型保持值类别(value catory) 和 cv-qualifier 。
当表达式类型为函数或函数引用类型时，结果为左值(lvalue) ，否则：
当且仅当左值引用类型时结果为左值（此时类型不变）；
否则结果为对应的右值引用类型的消亡值(xvalue) 。
按类型传递不能直接通过函数模板匹配不同的引用传递至 std::forward 实现。
因为函数模板的传递形式参数无法完全保留函数的实际参数的类型，
	不根据实际参数的类型而是根据实际参数的值类别决定形式参数是左值或右值引用类型，
	导致 std::forward 的实际参数类型不保证和作为函数实际参数的表达式类型相同，
	而使传递的值类别和表达式可能具有的引用类型不一致：
使用包含以 && 为类型声明符的模板参数的传递参数时，由于 std::forward 传递的是
	函数模板实例中和函数参数一致（可能是左值引用或右值引用的类型）的形式参数，
	使用宏或函数模板都能传递对象左值引用表达式为左值，对象右值引用表达式为右值；
而使传递对象右值引用类型的左值表达式，如对象右值引用声明的变量的 id-expression 时，
	结果总被传递为和表达式类型无关的左值。
*/
#define yforward(_expr) std::forward<decltype(_expr)>(_expr)

/*!
\brief 无序列依赖表达式组求值实现。
\return 第一参数的引用。
\since build 594
*/
template<typename _type, typename... _tParams>
yconstfn auto
unsequenced(_type&& arg, _tParams&&...) ynothrow -> decltype(yforward(arg))
{
	return yforward(arg);
}

/*!
\ingroup YBase_pseudo_keyword
\brief 无序列依赖表达式组求值。
\note 由于实现限制，无法用于 void 类型表达式组。
\note 使用一元形式 <tt>yunsequenced((_expr))</tt> 的形式标记表达式组
	但不取消序列关系。
\note 支持嵌套使用。
\warning 非一元形式禁止用于产生对同一对象的未序列化的(unsequenced) 副作用
	的表达式，否则存在未定义行为。
\warning 非一元形式不适用于对顺序有依赖的表达式，包括所有可能抛出异常且对抛出
	顺序敏感（例如 std::bad_cast 处理顺序不同可能造成内存泄露）的表达式。
\since build 266
*/
#define yunseq ystdex::unsequenced

} // namespace ystdex;

#endif

