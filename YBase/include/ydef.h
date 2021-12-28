/*
	© 2009-2021 FrankHB.

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
\brief 语言实现和系统环境相关特性及公用类型和宏的基础定义。
\version r4408
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-02 21:42:44 +0800
\par 修改时间:
	2021-12-26 20:03 +0800
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
这个宏用于检查包括 GCC 和 Clang 等 GNU C 兼容实现的版本。
因为 YBase 只支持 C++（而不包括 C ），不提供独立的 GCC 的 C 实现版本的检查。
*/

/*!
\def YB_IMPL_MSCPP
\brief Microsoft C++ 实现支持版本。
\since build 313
\see https://docs.microsoft.com/cpp/preprocessor/predefined-macros 。
\see https://blogs.msdn.microsoft.com/vcblog/2016/10/05/visual-c-compiler-version/ 。

定义为 _MSC_VER 描述的版本号。
包括 Microsoft VC++ 的编译器，不支持更早的 MSC 编译器和 Clang++ 前端实现。
因为文档中最早支持的 _MSC_VER 的值是 1200（对应 Microsoft VC++ 6.0 ），
没有其它确切版本的特性但在 Microsoft VC++ 6.0 以来可用特性的最小版本可使用这个值。
*/

/*!
\def YB_IMPL_GNUCPP
\brief GNU C++ 实现支持版本。
\since build 313

定义为 100 进位制的三重版本编号和。
包括 G++ ，不包括 Clang++ 实现。
*/

/*!
\def YB_IMPL_CLANGPP
\brief LLVM/Clang++ 实现支持版本。
\see https://releases.llvm.org/2.1/docs/ReleaseNotes.html 。
\see https://releases.llvm.org/2.6/docs/ReleaseNotes.html 。
\see https://releases.llvm.org/2.7/docs/ReleaseNotes.html 。
\see https://releases.llvm.org/2.8/docs/ReleaseNotes.html 。
\since build 458

定义为 100 进位制的三重版本编号和。
因为 Clang 最早发布于 LLVM 2.1 ，所以不需要区分 20100 以下的版本号。
最早的官方发布的 Clang 版本是 LLVM 2.6。
Clang 在 LLVM 2.7 默认启用 C++ ，在 LLVM 2.8 完全支持 ISO C++03（除 export 外）。
专用于 C++ 的特性可以使用上述版本号作为替代。
*/

#if defined(__GNUC__)
#	undef YB_IMPL_GNUC
#	define YB_IMPL_GNUC \
	(__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif
// XXX: Some implementations may not properly support the macros. See
//	https://docs.microsoft.com/cpp/build/reference/zc-cplusplus and
//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=1773 for instance.
#ifdef __cplusplus
#	if defined(_MSC_VER) && !defined(__clang__)
#		undef YB_IMPL_MSCPP
#		define YB_IMPL_MSCPP _MSC_VER
#	elif defined(__clang__)
#		undef YB_IMPL_CLANGPP
#		if defined(__apple_build_version__)
#		define YB_IMPL_CLANGPP (40300 + __clang_patchlevel__)
#		else
#			define YB_IMPL_CLANGPP \
	(__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#		endif
#	elif defined(__GNUG__)
#		undef YB_IMPL_GNUCPP
// XXX: This should be available since GCC 3.0.
#		if defined(__GNUC_PATCHLEVEL__)
#			define YB_IMPL_GNUCPP \
	(__GNUG__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#		else
#			define YB_IMPL_GNUCPP (__GNUG__ * 10000 + __GNUC_MINOR__ * 100)
#		endif
#	else
// TODO: Complete version checking for more compiler and library standard
//	implementations, e.g. EDG frontends including ICPC, NVCC.
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
/*!
\see http://clang.llvm.org/docs/LanguageExtensions.html#has-attribute 。
\since build 628
*/
#ifndef __has_attribute
#	define __has_attribute(...) 0
#endif

/*!
\see http://clang.llvm.org/docs/LanguageExtensions.html#has-builtin 。
\see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66970 。
\since build 535
*/
#ifndef __has_builtin
#	define __has_builtin(...) 0
#endif

/*!
\see http://clang.llvm.org/docs/LanguageExtensions.html#has-cpp-attribute 。
\since build 591
*/
#ifndef __has_cpp_attribute
#	define __has_cpp_attribute(...) 0
#endif

//! \see http://clang.llvm.org/docs/LanguageExtensions.html#has-feature-and-has-extension 。
//@{
#ifndef __has_extension
#	define __has_extension(...) __has_feature(__VA_ARGS__)
#endif

#ifndef __has_feature
#	define __has_feature(...) 0
#endif
//@}

/*!
\see http://clang.llvm.org/docs/LanguageExtensions.html#has-include 。
\since build 831
*/
#ifndef __has_include
#	define __has_include(...) 0
#endif
//@}

/*!
\see ISO C++20 [version.syn] 。
\see WG21 P0941R2 2.2 。
\see https://docs.microsoft.com/cpp/visual-cpp-language-conformance 。
*/
//@{
/*!
\see https://docs.microsoft.com/cpp/preprocessor/predefined-macros 。
\since build 935
*/
//@{
#ifndef __cpp_aligned_new
#	if (YB_IMPL_MSCPP >= 1912 && _MSVC_LANG >= 201606L) \
	|| __cplusplus >= 201606L
#		define __cpp_aligned_new 201606L
#	endif
#endif
//@}
/*!
\brief \c constexpr 特性测试宏。
\since build 628
*/
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
//! \see https://docs.microsoft.com/cpp/preprocessor/predefined-macros 。
//@{
//! \since build 930
//@{
#ifndef __cpp_if_constexpr
// NOTE: Microsoft Visual C++ has an extension in '/std:c++14' with warning
//	C4984.
#	if __cplusplus >= 201703L || \
	(YB_IMPL_MSCPP >= 1911 && _MSVC_LANG >= 201402L)
#		define __cpp_constexpr 201606L
#	endif
#endif
//@}
/*!
\see WG21 P0136R1 。
\see https://clang.llvm.org/docs/LanguageExtensions.html 。
\see https://gcc.gnu.org/projects/cxx-status.html 。
\see https://msdn.microsoft.com/library/hh409293.aspx 。
\since build 833
*/
//@{
#ifndef __cpp_inheriting_constructors
#	if (YB_IMPL_MSCPP >= 1914 && _MSVC_LANG >= 201511L) \
	|| __cplusplus >= 201511L
#		define __cpp_inheriting_constructors 201511L
#	elif __has_feature(cxx_inheriting_constructors) || YB_IMPL_MSCPP > 1900 \
	|| __cplusplus >= 200802L
#		define __cpp_inheriting_constructors 200802L
#	endif
#endif
//@}
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

#include <cstdlib> // for std::abort;
#include <utility> // for std::forward;

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
\brief 替换预处理记号为字符串。
\since build 715
*/
#define YPP_Stringify(_x) #_x

/*!
\brief 替换被宏替换的预处理记号为字符串。
\since build 880
*/
#define YPP_Stringize(_x) YPP_Stringify(_x)
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


/*!
\see https://gcc.gnu.org/legacy-ml/gcc-help/2015-07/msg00064.html 。
\see https://reviews.llvm.org/rL69560 。
\since build 880
*/
//@{
/*!
\def YPP_Diag_Push
\brief 保存现有的诊断状态。
*/
/*!
\def YPP_Diag_Pop
\brief 恢复保存的诊断状态。
*/
/*!
\def YPP_Diag_Ignore
\brief 忽略诊断选项。
*/
#if YB_IMPL_MSCPP
#	define YB_Diag_Push _Pragma("warning(push)")
#	define YB_Diag_Pop _Pragma("warning(pop)")
#	define YB_Diag_Ignore(_opt) _Pragma(YPP_Stringize(warning(disable: _opt)))
#elif YB_IMPL_CLANGPP >= 20600
// NOTE: See https://reviews.llvm.org/rL75431.
// XXX: The 'clang' pragma is preferred to 'GCC' since it can be Clang-specific.
//	See https://releases.llvm.org/3.1/tools/clang/docs/UsersManual.html#diagnostics_pragmas
//	and https://stackoverflow.com/a/11853015.
#	define YB_Diag_Push _Pragma("clang diagnostic push")
#	define YB_Diag_Pop _Pragma("clang diagnostic pop")
#	define YB_Diag_Ignore(_opt) _Pragma( \
	YPP_Stringize(clang diagnostic ignored YPP_Stringize(YPP_Concat(-W, _opt))))
// NOTE: GCC supported it since 4.2.0 (see also
//	https://gcc.gnu.org/legacy-ml/gcc-help/2015-07/msg00063.html), but it seemed
//	buggy until 4.6.0 on templates.
#elif YB_IMPL_GNUCPP >= 40200
#	define YB_Diag_Push _Pragma("GCC diagnostic push")
#	define YB_Diag_Pop _Pragma("GCC diagnostic pop")
#	define YB_Diag_Ignore(_opt) _Pragma( \
	YPP_Stringize(GCC diagnostic ignored YPP_Stringize(YPP_Concat(-W, _opt))))
#else
#	define YB_Diag_Push
#	define YB_Diag_Pop
#	define YB_Diag_Ignore
#endif
//@}


/*!	\defgroup YBase_replacement_features YBase Replacement Features
\brief YBase 替代特性。
\since build 837

YBase 提供的替代 ISO C++ 特性的接口。
若接口是类或类模板，可能具有扩展特性的成员以及与使用相关类型作为参数的函数或函数模板。
*/

/*!	\defgroup YBase_replacement_extensions YBase Replacement Extensions
\brief YBase 替代扩展。
\since build 837

YBase 提供的替代 ISO C++ 扩展特性的接口。
不是原始的被替代的接口但在包含其规格（如标准库头）内的接口。
*/

/*!	\defgroup lang_impl_features Language Implementation Features
\brief 语言实现的特性。
\see https://blogs.msdn.microsoft.com/vcblog/2015/06/19/c111417-features-in-vs-2015-rtm/ 。
\see http://clang.llvm.org/docs/LanguageExtensions.html 。
\since build 294
*/
//@{
/*!
\def YB_HAS_ALIGNAS
\brief 内建 alignas 支持。
\since build 389
*/
#undef YB_HAS_ALIGNAS
#define YB_HAS_ALIGNAS (__has_feature(cxx_alignas) \
	|| __has_extension(cxx_alignas) || __cplusplus >= 201103L \
	|| YB_IMPL_GNUCPP >= 40800 || YB_IMPL_MSCPP >= 1800)

/*!
\def YB_HAS_ALIGNOF
\brief 内建 alignof 支持。
\see https://blogs.msdn.microsoft.com/vcblog/2015/06/19/c111417-features-in-vs-2015-rtm/ 。
\since build 315
*/
#undef YB_HAS_ALIGNOF
#define YB_HAS_ALIGNOF (__has_feature(cxx_alignof) \
	|| __has_extension(cxx_alignof) || __cplusplus >= 201103L \
	|| YB_IMPL_GNUCPP >= 40500 || YB_IMPL_MSCPP >= 1800)

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
\see https://docs.microsoft.com/cpp/visual-cpp-language-conformance 。
\see https://gcc.gnu.org/projects/cxx-status.html 。
\since build 425
*/
#undef YB_HAS_THREAD_LOCAL
// XXX: The macro %_GLIBCXX_HAVE_TLS is independ on the mode used. For all
//	versions of G++ having unreliable %__cplusplus, 'thread_local' is assumed
//	not supported.
#define YB_HAS_THREAD_LOCAL \
	(__has_feature(cxx_thread_local) || (__cplusplus >= 201103L \
		&& (!YB_IMPL_GNUCPP || _GLIBCXX_HAVE_TLS)) || YB_IMPL_MSCPP >= 1900)
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
\warning 不附加检查实现。用户应验证可能使用的指令中的标识符在宏替换后能保持正确。
\since build 837

保证忽略时不导致运行时语义差异的提示，主要用于便于实现可能的优化。
*/
//@{
/*!
\def YB_ATTR
\brief GNU 风格属性。
\since build 373
*/
#if YB_IMPL_GNUC >= 20500
#	define YB_ATTR(...) __attribute__((__VA_ARGS__))
#else
#	define YB_ATTR(...)
#endif

/*!
\see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60503 。
\see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=89640 。
\see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90333 。
*/
//@{
/*!
\def YB_ATTR_LAMBDA
\brief 允许在 lambda 表达式的参数列表后使用的属性。
\sa YB_ATTR_LAMBDA_PFX
\sa YB_ATTR_LAMBDA_QUAL
\see https://fedoraproject.org/wiki/Releases/30/ChangeSet 。
\since build 860

选择性启用 lambda 表达式的扩展属性。
属性预期应用在 lambda 表达式的参数列表之后。参见以下 YB_ATTR_LAMBDA_QUAL 的使用。
一般地，从属 lambda 表达式的扩展属性，优先使用 YB_ATTR_LAMBDA_PFX ；
为兼容性，同时可用 YB_ATTR_LAMBDA 最大化支持 YB_ATTR_LAMBDA_PFX 没有生效的情形。
标准属性在此位置从属返回类型而不是 lambda 表达式；
	若需标准属性从属 lambda 表达式，应使用 YB_ATTR_LAMBDA_PFX 。
G++ 9.0 第一次修复 GCC PR 60503 ，但因为其错误的实现，不支持扩展属性。
虽然 GCC 9.0 不是正式发布版本，但已有实际下游使用（如 Fedora 30 使用 GCC 9.0.1 ），
	在此一并变通。
GCC 9.0 之后修复了 PR 60503 ，当存在 trailing-return-type 时，
	这里的属性仍视为从属返回类型，无法支持从属 lambda 表达式的扩展属性。
上述问题在 GCC PR 90333 中修复。
这里不区分是否存在 trailing-return-type ，在适用 PR 90333 的修复前，
	总是忽略所有属性。
*/
#if !((YB_IMPL_GNUCPP >= 90000 && YB_IMPL_GNUCPP < 90400) \
	|| (YB_IMPL_GNUCPP >= 100000 && YB_IMPL_GNUCPP < 100300))
#	define YB_ATTR_LAMBDA(...) YB_ATTR(__VA_ARGS__)
#else
#	define YB_ATTR_LAMBDA(...)
#endif

/*!
\def YB_ATTR_LAMBDA_PFX
\brief 允许在 lambda 表达式的参数列表前使用的属性。
\sa YB_ATTR_LAMBDA_QUAL
\see WG21 P02173R0 。
\see https://reviews.llvm.org/D95691 。
\since build 918

选择性启用 lambda 表达式在 lambda-introducer 后的扩展属性。
此语法可以支持：
<tt>[] [[]] () noexcept -> void{}</tt> 。
<tt>[] __attribute__(()) () noexcept -> void{}</tt> 。
配合其它宏的语法示例：
<tt>[] YB_ATTR_STD() () ynothrow -> void{}</tt> 。
<tt>[] YB_ATTR() () ynothrow -> void{}</tt> 。
在第一次修复 GCC PR 60503 后，G++ 9.0 已支持在此位置的标准属性，但不支持扩展属性。
在此要求支持扩展属性，因此要求 GCC 9.3 或 GCC 10 的发布版本支持。
这也是唯一在 G++ 9.1 之后中插入从属 lambda 表达式（而非类型）的属性的方式。
对 Clang++ ，仅在 C++2b 模式支持，且仅支持标准属性。暂不在 Clang++ 启用此语法。
另见以下 YB_ATTR_LAMBDA_QUAL 的使用。
*/
#if (YB_IMPL_GNUCPP >= 90300 && YB_IMPL_GNUCPP < 100000) \
	|| YB_IMPL_GNUCPP >= 100100
#	define YB_ATTR_LAMBDA_PFX(...) YB_ATTR(__VA_ARGS__)
#else
#	define YB_ATTR_LAMBDA_PFX(...)
#endif

/*!
\def YB_ATTR_LAMBDA_QUAL
\brief 允许在 lambda 表达式的参数列表后和限定符共用的属性。
\since build 864

在 lambda 表达式中可能同时支持标准属性、限定符和（不修饰返回类型的）扩展属性。
语法示例：
<tt>[]() __attribute__(()) noexcept [[]] -> void{}</tt> 。
配合其它宏的语法示例：
<tt>[]() YB_ATTR() ynothrow YB_ATTR_STD() -> void{}</tt> 。
Clang++ 9 正确支持这些属性。
G++ 9 之前错误地实现了标准属性的顺序。GCC PR 60503 修复后仍存在其它问题。
为兼容不同实现版本一致地支持标准属性，可能需要调换标准属性和限定符的顺序。
G++ 9 之前可支持：
<tt>[]() YB_ATTR() ynothrow -> void{}</tt> ；
<tt>[]() YB_ATTR_STD() ynothrow -> void{}</tt> 。
G++ 9.1 起可支持：
<tt>[]() ynothrow YB_ATTR_STD() YB_ATTR(){}</tt> ；
<tt>[]() ynothrow YB_ATTR_STD() -> void{}</tt> 。
使用 G++ 9.1 后的一些版本时，显式返回类型在此可能和 YB_ATTR() 冲突而不能同时使用。
作为变通，正确的 YB_ATTR 使用 YB_ATTR_LAMBDA 代替，此时，语法如以下之一：
<tt>[]() YB_ATTR_LAMBDA_QUAL(ynothrow, YB_ATTR_LAMBDA()) -> void{}</tt> ；
<tt>[]() YB_ATTR_LAMBDA_QUAL(ynothrow, YB_ATTR_STD()) -> void{}</tt> ；
<tt>[]() YB_ATTR_LAMBDA_QUAL(ynothrow, YB_ATTR()){}</tt> ；
<tt>[]() YB_ATTR_LAMBDA_QUAL(ynothrow, YB_ATTR_STD()){}</tt> ；
<tt>[]() YB_ATTR_LAMBDA() -> void{}</tt> ；
<tt>[]() YB_ATTR_STD() -> void{}</tt> ；
<tt>[]() YB_ATTR(){}</tt> ；
<tt>[]() YB_ATTR_STD(){}</tt> 。
使用 YB_ATTR_LAMBDA 同时也可对 G++ 9.0 不支持扩展属性进行变通。
G++ 9.0 起的一些版本中，YB_ATTR_LAMBDA 中的属性被忽略；
考虑添加 YB_ATTR_LAMBDA_PFX 属性替代，以使这些属性被最大化地支持。
特别地，不论限定符，trailing-return-type 会使从属 lambda 的表达式的属性受到限制：
	此处标准属性被视为从属返回类型；
	此处扩展属性是语法错误。
关于 G++ 支持扩展属性允许出现在声明中的位置的决策，参见 GCC PR 89640 中的评论；
	在 GCC PR 90333 完全解决后，扩展属性上的这一限制被重新撤销，
	但因可用 YB_ATTR_LAMBDA_PFX ，在此不再使用先前版本的顺序。
注意，Clang++ 不支持从属 lambda 表达式的标准属性（如 [[noreturn]] ），
	这类属性不论是否使用同 YB_ATTR_LAMBDA_PFX 的方式，不能选择 YB_ATTR_STD 。
关于从属 lambda 表达式的标准属性，参见 YB_ATTR_LAMBDA_PFX 。
*/
#if !(YB_IMPL_GNUCPP && YB_IMPL_GNUCPP < 90000) && !YB_IMPL_CLANGPP
#	define YB_ATTR_LAMBDA_QUAL(_q, ...) _q __VA_ARGS__
#else
#	define YB_ATTR_LAMBDA_QUAL(_q, ...) __VA_ARGS__ _q
#endif

/*!
\def YB_ANNOTATE_LAMBDA
\brief 标注混合属性的 lambda 声明符。
\sa YB_ATTR_LAMBDA
\sa YB_ATTR_LAMBDA_PFX
\sa YB_ATTR_LAMBDA_QUAL
\since build 918
*/
#define YB_LAMBDA_ANNOTATE(_arglist, _q, ...) \
	YB_ATTR_LAMBDA_PFX(__VA_ARGS__) _arglist \
		YB_ATTR_LAMBDA_QUAL(_q, YB_ATTR_LAMBDA(__VA_ARGS__))
//@}


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
\def YB_ATTR_always_inline
\brief 指定函数被调用时的内联的属性。
\note 不能内联的情形可能引起诊断。
\see https://clang.llvm.org/docs/AttributeReference.html#always-inline-force-inline 。
\see https://gcc.gnu.org/git/?p=gcc.git;a=commit;h=6aa77e6c394b8d61c25d146e7d106f4ddf17ca54 。
\see https://reviews.llvm.org/rL58304 。
\since build 931

在函数或函数模板上指定要求函数或函数模板的实例的调用被内联。
不保证隐含 inline 。需影响 ODR 时，仍需指定显式的 inline 或 constexpr 等。
一些情形可能不能内联，如递归调用和调试模式的函数调用。详见具体实现的定义。
应注意直接使用本属性不保证提升生成的代码质量。
一般应仅在要求不生成定义或可确定内部可内联时使用，以避免：
无法内联时引起诊断（可能是错误）；
阻止在内部调用的函数无法在此内联展开后停止内联而引起代码膨胀，引起不必要的调用开销；
其它非预期的方式影响实现自动的内联的判断，而引起代码缺陷。
对和 GCC 兼容的实现，可配合 YB_ATTR(noinline) 或 YB_FLATTEN 控制内联展开的位置。
*/
// XXX: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=14950.
#if __has_attribute(__always_inline__) || YB_IMPL_CLANGPP >= 20500 \
	|| YB_IMPL_GNUCPP >= 40000 \
	|| (YB_IMPL_GNUCPP >= 30100 && YB_IMPL_GNUCPP < 30400)
#	define YB_ATTR_always_inline __attribute__((__always_inline__))
#elif YB_IMPL_MSCPP >= 1200
// NOTE: The Microsoft-specific keyword '__forceinline' may implies inline,
//	but this should not be relied on. See also
//	https://docs.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4141.
#	define YB_ATTR_always_inline \
	YB_Diag_Push YB_Diag_Ignore(4141) __forceinline YB_Diag_Pop
#else
#	define YB_ATTR_always_inline
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
#elif __has_attribute(__fallthrough__)
#	define YB_ATTR_fallthrough YB_ATTR(__fallthrough__)
#else
#	define YB_ATTR_fallthrough
#endif

/*!
\def YB_ATTR_gnu_printf
\brief GNU 风格 printf 属性。
\note 早期版本 Clang++ 未支持此属性，警告 [-Wignored-attributes] 。
\since build 615
\see https://clang.llvm.org/docs/AttributeReference.html#format 。
*/
#if YB_IMPL_GNUCPP >= 40400
#	define YB_ATTR_gnu_printf(...) \
	YB_ATTR(__format__(__gnu_printf__, __VA_ARGS__))
#elif __has_cpp_attribute(format) || YB_IMPL_GNUCPP >= 20604
#	define YB_ATTR_gnu_printf(...) YB_ATTR(__format__(__printf__, __VA_ARGS__))
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
#elif __has_attribute(__unused__)
#	define YB_ATTR_maybe_unused YB_ATTR(__unused__)
#else
#	define YB_ATTR_maybe_unused
#endif

/*!
\def YB_ATTR_nodiscard
\brief 标记返回值不被忽略的属性。
\since build 833
\see WG21 P0189R1 。
\see https://clang.llvm.org/docs/AttributeReference.html#nodiscard-warn-unused-result 。
\see https://gcc.gnu.org/projects/cxx-status.html 。
\see https://docs.microsoft.com/cpp/code-quality/annotating-function-behavior 。
*/
#if __has_cpp_attribute(nodiscard) \
	&& (!YB_IMPL_CLANGPP || __cplusplus >= 201703L)
// XXX: This is ruled out for [-Wc++17-extensions] in Clang++.
#	define YB_ATTR_nodiscard YB_ATTR_STD(nodiscard)
#elif __has_cpp_attribute(clang::warn_unused_result)
#	define YB_ATTR_nodiscard YB_ATTR_STD(clang::warn_unused_result)
#elif __has_cpp_attribute(gnu::warn_unused_result)
#	define YB_ATTR_nodiscard YB_ATTR_STD(gnu::warn_unused_result)
#elif __has_attribute(__warn_unused_result__)
#	define YB_ATTR_nodiscard YB_ATTR(__warn_unused_result__)
#elif YB_IMPL_MSCPP >= 1700
// XXX: Assume SAL is available.
#	define YB_ATTR_nodiscard _Check_return_
#else
#	define YB_ATTR_nodiscard
#endif

/*!
\brief 修饰类定义要求实现不生成初始化动态类型信息。
\note 只适用于定义不在初始化派生类对象时的动态类型的类，否则行为未定义。
\see https://docs.microsoft.com/cpp/cpp/novtable 。
\see http://releases.llvm.org/3.7.0/tools/clang/docs/AttributeReference.html 。
\see https://clang.llvm.org/docs/AttributeReference.html#novtable 。
\see http://lists.llvm.org/pipermail/cfe-commits/Week-of-Mon-20150720/133747.html 。
\since build 842
\todo 确认可忽略不支持情形的 Clang++ 最近可用的版本。

修饰类定义，允许实现省略若干在初始化类的对象时的隐含初始化。
这些初始化通常用于保证派生类的动态类型正确实现 ISO C++ 调用基类虚函数的语义。
只适用于定义不在初始化对象时调用基类虚函数和使用 RTTI
	（作为操作数使用 dynamic_cast 或 typeid ）的类，否则行为未定义。
一般用于特殊成员函数外的成员函数只有纯虚函数（析构函数是非纯的虚函数）的基类。
派生类的构造函数使用这些依赖其动态类型的操作已引起未定义行为，
	参见 ISO C++ [abstract.class]/6 ；
	因此当基类只有纯虚函数时，不改变程序的预期语义，而仅用于优化。
其它情形派生类需要排除更多的限制。
当前 Microsoft VC++ 或 Clang++（使用 Microsoft ABI ）支持这项特性。
对不支持的情形，近期版本 Clang++ 可直接忽略。
*/
#if __has_cpp_attribute(novtable)
#	define YB_ATTR_novtable YB_ATTR(novtable)
#elif YB_IMPL_MSCPP >= 1100 || (YB_IMPL_CLANGPP >= 30700 && YB_IMPL_MSCPP)
#	define YB_ATTR_novtable __declspec(novtable)
#else
#	define YB_ATTR_novtable
#endif

/*!
\def YB_ATTR_returns_nonnull
\brief 指示返回非空属性。
\since build 676
\see https://clang.llvm.org/docs/AttributeReference.html#returns-nonnull 。
\see http://reviews.llvm.org/rL199626 。
\see http://reviews.llvm.org/rL199790 。
\todo 确认 Clang++ 最低可用的版本。
*/
#if __has_attribute(__returns_nonnull__) || YB_IMPL_GNUCPP >= 40900 \
	|| YB_IMPL_CLANGPP >= 30500
#	define YB_ATTR_returns_nonnull YB_ATTR(__returns_nonnull__)
#else
#	define YB_ATTR_returns_nonnull
#endif

/*!
\def YB_ALLOCATOR
\brief 指示返回指针的函数或函数模板若为非空值则对应的存储不和其它对象指针共享别名。
\note 指示行为类似 std::malloc 或 std::calloc 等的函数。
\warning 要求满足指示的假定，否则行为未定义。
\see https://docs.microsoft.com/cpp/cpp/restrict?view=vs-2017 。
\see https://blogs.msdn.microsoft.com/vcblog/2015/10/21/memory-profiling-in-visual-c-2015/ 。
\see https://blogs.msdn.microsoft.com/vcblog/2016/05/25/tracking-custom-memory-allocations-with-visual-studio-15-preview-2/ 。
\see https://gitlab.gnome.org/GNOME/glib/issues/1465 。
\since build 373

指示函数若返回非空指针，返回的指针指向的存储中的指针不是其它任何有效对象指针的别名，
且指针指向的存储内容不由其它存储决定。
*/
#if YB_IMPL_MSCPP >= 1900 && !defined(__EDG__) && !defined _CORECRT_BUILD
#	define YB_ALLOCATOR __declspec(allocator) __declspec(restrict)
#elif YB_IMPL_MSCPP >= 1400
#	define YB_ALLOCATOR YB_ATTR_nodiscard __declspec(restrict)
#elif __has_attribute(__malloc__) || YB_IMPL_GNUCPP >= 20296
#	define YB_ALLOCATOR YB_ATTR_nodiscard YB_ATTR(__malloc__)
#else
#	define YB_ALLOCATOR YB_ATTR_nodiscard
#endif

/*!
\def YB_ASSUME(_expr)
\brief 假定表达式的求值总是为真。
\note 未指定表达式是否被求值。
\note 可作为优化提示。可能影响控制流预测，一般接近对应条件被求值的位置局部使用。
\note 一般应避免表达式求值的副作用。预期的求值性质和被 YB_PURE 的函数调用相同。
\warning 若假定不成立则行为未定义。
\see https://clang.llvm.org/docs/LanguageExtensions.html#builtin-assume 。
\see https://reviews.llvm.org/rL217349 。
\see https://bugs.llvm.org/show_bug.cgi?id=32424 。
\since build 535
*/
#if YB_IMPL_MSCPP >= 1200
#	define YB_ASSUME(_expr) __assume(_expr)
#elif YB_IMPL_CLANGPP >= 30501
// NOTE: The warning [-Wassume] is largely useless with misleading diagnostics.
// NOTE: The warning [-Wpointer-ignore] is available since Clang 3.4.1. Ignoring
//	the warning makes it usable in cases like variable in %_expr having nonnull
//	attributes.
#	define YB_ASSUME(_expr) \
	YB_Diag_Push YB_Diag_Ignore(pointer-bool-conversion) \
		YB_Diag_Ignore(assume) __builtin_assume(_expr) YB_Diag_Pop
#elif __has_builtin(__builtin_assume)
#	define YB_ASSUME(_expr) __builtin_assume(_expr)
#elif __has_builtin(__builtin_unreachable) || YB_IMPL_GNUCPP >= 40500
// NOTE: It is ideal to get away the default warning [-Wnonnull-compare]
//	enabled by '-Wall'. (See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=17308
//	and https://gcc.gnu.org/bugzilla/show_bug.cgi?id=69850.) However, the pragma
//	in the following does not work, as
//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60875.
#		if false
#			define YB_ASSUME(_expr) \
	((YB_Diag_Push YB_Diag_Ignore(nonnull-compare) _expr \
		YB_Diag_Pop) ? void() : __builtin_unreachable())
#		endif
// NOTE: To work the problem avoie around, the warning is disabled globally.
#		if YB_IMPL_GNUCPP >= 60000
#			pragma GCC diagnostic ignored "-Wnonnull-compare"
#		endif
#		define YB_ASSUME(_expr) ((_expr) ? void() : __builtin_unreachable())
#else
#	define YB_ASSUME(_expr) ((_expr) ? void() : YB_ABORT)
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
\see https://bugs.llvm.org/show_bug.cgi?id=7559 。
\see https://reviews.llvm.org/rL209217 。
\since build 646
*/
#if ((__has_attribute(__flatten__) || (YB_IMPL_GNUCPP >= 40102 \
	&& YB_IMPL_GNUCPP != 40600) || YB_IMPL_CLANGPP >= 30402)) \
	&& !__OPTIMIZE_SIZE__
// XXX: This seems not optimal for G++ with '-Os', althouth in theory they
//	could be better applied together. For Clang++ which implementing it by
//	recursively 'always_inline', it could be worse.
#	define YB_FLATTEN YB_ATTR(__flatten__)
#else
#	define YB_FLATTEN
#endif

/*!
\def YB_NONNULL
\brief 指定假定保证不为空指针的函数参数。
\warning 当指定的函数实际为空时行为未定义。
\see https://clang.llvm.org/docs/AttributeReference.html#id15 。
\since build 524
*/
#if __has_attribute(__nonnull__) || YB_IMPL_GNUCPP >= 30300
#	define YB_NONNULL(...) YB_ATTR(__nonnull__(__VA_ARGS__))
#else
#	define YB_NONNULL(...)
#endif

/*!
\def YB_NORETURN
\brief 指定无返回值函数。
\note 不保证适用修饰 lambda 。这种情况可使用 YB_ATTR_LAMBDA(noreturn) 代替。
\warning 当指定的函数调用实际返回时行为未定义。
\sa YB_ATTR
\sa YB_ATTR_LAMBDA
\see https://msdn.microsoft.com/library/hh567368.aspx 。
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
	YB_Diag_Ignore(4646)
	// NOTE: See https://docs.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-3-c4646.
	// NOTE: The warning "function declared with __declspec(noreturn) has
	//	non-void return type" is unwanted since the type of function may be
	//	significant even it does not return.
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
\see https://reviews.llvm.org/rL230763 。
\see https://bugs.llvm.org/show_bug.cgi?id=43275 。

指示函数或函数模板的求值仅用于计算返回值，无影响其它顶层块作用域外存储的副作用，
且返回值只依赖参数和/或编译时确定内存位置（如静态存储的对象的）存储的值。
假定条件包括：
不修改函数外部的存储；
不访问函数外部 volatile 存储；
不调用具有不可忽略副作用而不可被 YB_PURE 安全指定的函数。
*/
// XXX: The conditions latter implies '_LIBUNWIND_ARM_EHABI' in libunwind and
//	'_LIBCXXABI_ARM_EHABI' in libc++.
#if (__has_attribute(__pure__) || YB_IMPL_GNUCPP >= 20296) \
	&& !(YB_IMPL_CLANGPP && defined(__arm__) \
	&& !defined(__USING_SJLJ_EXCEPTIONS__) && !defined(__ARM_DWARF_EH__))
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
不调用具有不可忽略副作用而不可被 YB_STATELESS 安全指定的函数。
可被安全指定的函数或函数模板是 YB_PURE 限定的函数或函数模板的真子集；
不抛出异常。
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
	|| (YB_IMPL_GNUC && (defined(__MINGW32__) || defined(__CYGWIN__)))
#	ifdef YB_DLL
#		define YB_API __declspec(dllimport)
#	elif defined(YB_BUILD_DLL)
#		define YB_API __declspec(dllexport)
#	else
#		define YB_API
#	endif
// NOTE: See https://gcc.gnu.org/gcc-4.0/changes.html. The attribute on the
//	class types are required. If it is detected by %__has_attribute, this
//	capability is assumed since GCC < 4 has no built-in %__has_attribute
//	support.
// NOTE: See https://reviews.llvm.org/rL110315.
#elif defined(YB_BUILD_DLL) && (__has_attribute(__visibility__) \
	|| YB_IMPL_GNUCPP >= 40000 || YB_IMPL_CLANGPP >= 20800)
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
#		define YB_Use_YAssert true
#	endif
#elif !defined(YB_Use_StrictNoThrow)
#	define YB_Use_StrictNoThrow true
#endif
#ifndef YB_Use_YTrace
#	define YB_Use_YTrace true
#endif
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
\def yconstexpr_if
\brief 可选的 if constexpr 条件判断。
\since build 930
*/
#if __cpp_if_constexpr >= 201606L
#	define yconstexpr_if if constexpr
#else
#	define yconstexpr_if if
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
#elif __cplusplus >= 201103L || __STDC_VERSION__ >= 199901L
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
#elif __cplusplus >= 201103L || __STDC_VERSION__ >= 199901L
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
\see CWG 2355 。
\see https://developercommunity.visualstudio.com/t/vc-2017-rejects-dependent-noexcept-specifier-in-te/441268 。
*/
//@{
// XXX: See archived YSLib issues 39.
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
//@}
#if __cpp_noexcept_function_type >= 201510L && !YB_IMPL_MSCPP
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
\todo 判断 \c __thread 和 \c __declspec(thread) 的语言实现版本。
*/
// XXX: %_MT is specific to Win32 and it is not considered a prerequisition of
//	TLS now.
#if YB_HAS_THREAD_LOCAL
#	define ythread thread_local
#elif defined(_WIN32) && (YB_IMPL_MSCPP || defined(__ICL) || defined(__DMC__) \
	|| defined(__BORLANDC__))
#	define ythread __declspec(thread)
// NOTE: Old versions of Mac OS do not support '__thread'. Other platforms
//	compatible to GNU C should support it.
#elif (defined(__APPLE__) && YB_IMPL_CLANGPP && defined(MAC_OS_X_VERSION_10_7) \
	&& (defined(__x86_64__) || defined(__i386__))) || YB_IMPL_GUNC
#	define ythread __thread
#else
#	define ythread
#endif

/*!
\def ythrow
\brief YSLib 动态异常规范：根据是否使用异常规范宏指定或忽略动态异常规范。
\note 动态异常规范已从 ISO C++17 移除，用户代码不应假定支持。
\note 主要为可读性保留，ythrow = "yielded throwing" 。
\see WG21 P0003R5 。
*/
#if YB_Use_DynamicExceptionSpecification
#	define ythrow throw
#else
#	define ythrow(...)
#endif

/*!
\brief 标记未使用的表达式。
\note 显式转换为 void 类型以标记表达式未被作为子表达式使用，可避免某些实现的警告。
\warning 避免在表达式中误用逗号。
\since build 435
*/
#define yunused(...) static_cast<void>(__VA_ARGS__)
//@}


namespace ystdex
{

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

//! \warning 其中的类类型一般可被继承但非虚析构。
//@{
/*!	\defgroup tags Tags
\brief 标签。
\note 可能是类型或元类型。
\since build 447

用于标记重载或其它类型接口的类型或模板。
*/

/*!	\defgroup traits Traits
\brief 特征。
\since build 845

具有特定成员，提供翻译时确定的信息的类型或模板。
*/
//@}


/*!
\ingroup YBase_pseudo_keyword
\brief 根据参数类型使用 std::forward 转发对应参数。
\since build 245

转发参数：按类型传递参数，保持值类别(value catrgory) 和 cv-qualifier 。
当作为宏参数的被转发表达式的类型为函数或函数引用类型时，结果为左值(lvalue) ，否则：
当且仅当被转发表达式的类型为左值引用类型时，结果为左值（此时类型不变）；
否则，结果为对应的右值引用类型的消亡值(xvalue) 。
按类型传递不能直接实现为一个具有转发引用(forwarding reference) 类型的模板参数的
	函数模板匹配不同的引用然后以其实例的函数形式参数作为实际参数调用 std::forward ，
	而需要使用宏实现，以确保在不同被转发表达式类型上的一致性：
转发引用类型的函数形式参数无法完全保留作为其对应函数实际参数的被转发表达式的类型
	（即不按被转发表达式的类型而是其值类别推导转发引用中的模板参数，
	再确定作为函数的形式参数的转发引用被实例化为左值或右值引用类型），
	于是，函数模板实例中的形式参数类型不保证和被转发表达式类型相同；
而 std::forward 转发结果类型的模板参数只能从函数模板实例的形式参数类型中取得；
因此 std::forward 使用的模板参数不能总是保证结果的值类别和被转发表达式中的类型对应，
	确保左值引用类型总是转发为左值且右值引用类型总是转发为右值；
若转发对象右值引用类型的左值表达式，如按对象右值引用类型声明的变量的
	id-expression 时，函数模板转发的结果总被传递为左值，
	即便被转发表达式可能只能安全地绑定到右值。
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

