/*
	Copyright by FrankHB 2009 - 2013.

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
\version r2293
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-02 21:42:44 +0800
\par 修改时间:
	2013-06-24 21:28 +0800
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
\def YB_IMPL_CPP
\brief C++ 实现支持版本。
\since build 313

定义为 __cplusplus 。
*/

/*!
\def YB_IMPL_MSCPP
\brief Microsoft C++ 实现支持版本。
\since build 313

定义为 _MSC_VER 描述的版本号。
*/

/*!
\def YB_IMPL_GNUCPP
\brief GNU C++ 实现支持版本。
\since build 313

定义为 100 进位制的三重版本编号和。
*/

#ifdef __cplusplus
#	define YB_IMPL_CPP __cplusplus
#	ifdef _MSC_VER
#		undef YB_IMPL_MSCPP
#		define YB_IMPL_MSCPP _MSC_VER
#	elif defined(__GNUC__)
#		undef YB_IMPL_GNUCPP
#		define YB_IMPL_GNUCPP (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 \
			+ __GNUC_PATCHLEVEL__)
#	else
// TODO: Complete version checking for compiler and library implementations.
//#ifdef __GNUC__
//#	include <tr1/type_traits>
#		error This language implementation is not supported!
#	endif
#else
#	error This header is only for C++!
#endif

//@}

#include <cstddef> // for std::nullptr_t, std::size_t, std::ptrdiff_t, offsetof;
#include <climits> // for CHAR_BIT;
#include <cassert> // for assert;
#include <cstdint>
#include <cwchar> // for std::wint_t;
#include <utility> // for std::forward;
#include <type_traits> // for std::is_class, std::is_standard_layout;


/*
\def yjoin
\brief 带宏替换的记号连接。
\see http://gcc.gnu.org/onlinedocs/cpp/Concatenation.html 。
\see https://www.securecoding.cert.org/confluence/display/cplusplus/PRE05-CPP.+Understand+macro+replacement+when+concatenating+tokens+or+performing+stringification 。
\since build 409

 ISO/IEC C++ 未确定宏定义内 # 和 ## 操作符求值顺序。
注意 GCC 中，宏定义内 ## 操作符修饰的形式参数为宏时，此宏不会被展开。
*/
#define yjoin(x, y) yJOIN(x, y)

/*
\def yJOIN
\brief 记号连接。
\sa yjoin
\since build 304
*/
#define yJOIN(x, y) x ## y


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
#define YB_HAS_ALIGNAS (YB_IMPL_GNUCPP >= 40800)

/*!
\def YB_HAS_ALIGNOF
\brief 内建 alignof 支持。
\since build 315
*/
#undef YB_HAS_ALIGNOF
#define YB_HAS_ALIGNOF (YB_IMPL_CPP >= 201103L || YB_IMPL_GNUCPP >= 40500)

/*!
\def YB_HAS_BUILTIN_NULLPTR
\brief 内建 nullptr 支持。
\since build 313
*/
#undef YB_HAS_BUILTIN_NULLPTR
#define YB_HAS_BUILTIN_NULLPTR (YB_IMPL_CPP >= 201103L \
	|| YB_IMPL_GNUCPP >= 40600 || YB_IMPL_MSCPP >= 1600)

/*!
\def YB_HAS_CONSTEXPR
\brief constexpr 支持。
\since build 313
*/
#undef YB_HAS_CONSTEXPR
#define YB_HAS_CONSTEXPR (YB_IMPL_CPP >= 201103L || YB_IMPL_GNUCPP >= 40600)

/*!
\def YB_HAS_NOEXCPT
\brief noexcept 支持。
\since build 319
*/
#undef YB_HAS_NOEXCEPT
#define YB_HAS_NOEXCEPT (YB_IMPL_CPP >= 201103L || YB_IMPL_GNUCPP >= 40600)

//@}


/*!	\defgroup lang_impl_hints Language Implementation Hints
\brief 语言实现的提供的附加提示。
\since build 294

保证忽略时不导致运行时语义差异的提示，主要用于便于实现可能的优化。
*/
//@{

/*!
\def YB_ATTR
\brief 属性。
\warning 不对指令进行检查。用户应验证可能使用的指令中的标识符在宏替换后能保持正确。
\since build 373
*/
#if YB_IMPL_GNUCPP >= 20500
#	define YB_ATTR(...) __attribute__((__VA_ARGS__))
#else
#	define YB_ATTR(...)
#endif

/*!
\def YB_ALLOCATOR
\brief 指示修饰的是分配器，或返回分配器调用的函数或函数模板。
\note 指示行为类似 std::malloc 或 std::calloc 等的函数。
\warning 要求满足指示的假定，否则行为未定义。
\since build 373

指示函数若返回非空指针，返回的指针不是其它任何有效指针的别名，
且指针指向的存储内容不由其它存储决定。
*/
#if YB_IMPL_GNUCPP >= 20296
#	define YB_ALLOCATOR YB_ATTR(__malloc__)
#else
#	define YB_ALLOCATOR
#endif

/*!
\def YB_EXPECT(expr, constant)
\def YB_LIKELY(expr)
\def YB_UNLIKELY(expr)
\brief 分支预测提示。
\since build 313
*/
#if YB_IMPL_GNUCPP >= 29600
#	define YB_EXPECT(expr, constant) (__builtin_expect(expr, constant))
#	define YB_LIKELY(expr) (__builtin_expect(bool(expr), 1))
#	define YB_UNLIKELY(expr) (__builtin_expect(bool(expr), 0))
#else
#	define YB_EXPECT(expr, constant) (expr)
#	define YB_LIKELY (expr) (expr)
#	define YB_UNLIKELY (expr) (expr)
#endif

/*!
\def YB_NORETURN
\brief 指定无返回值函数。
\warning 当指定的函数调用实际返回时行为未定义。
\since build 396
\todo 使用 ISO C++11 noreturn 属性。
*/
#if YB_IMPL_GNUCPP >= 20296
#	define YB_NORETURN YB_ATTR(__noreturn__)
#else
#	define YB_NORETURN
#endif

/*!
\def YB_PURE
\brief 指示函数或函数模板实例为纯函数。
\post 函数外可访问的存储保持不变。
\note 假定函数保证可返回；返回类型 void 时无意义。
\note 假定函数无外部可见的副作用：局部记忆化合并重复调用后不改变可观察行为。
\note 不修改函数外部的存储；不访问函数外部 volatile 存储；
	通常不调用不可被 YB_PURE 安全指定的函数。
\warning 要求满足指示的假定，否则行为未定义。
\since build 373

指示函数或函数模板的求值是返回值的计算，无影响其它的存储的副作用，
且返回值只依赖于参数和/或编译时确定内存位置（如具有静态存储期的对象的）存储的值。
*/
#if YB_IMPL_GNUCPP >= 20296
#	define YB_PURE YB_ATTR(__pure__)
#else
#	define YB_PURE
#endif

/*!
\def YB_STATELESS
\brief 指示函数或函数模板实例为无状态函数。
\post 函数外可访问的存储保持不变。
\note 假定函数保证可返回；返回类型 void 时无意义。
\note 假定函数无外部可见的副作用：局部记忆化合并重复调用后不改变可观察行为。
\note 假定函数调用的结果总是相同：返回值总是不可分辨的右值或指示同一个内存位置的左值。
	任意以一次调用结果替代调用或合并重复调用时不改变可观察行为。
\note 不访问函数外部的存储；通常不调用不可被 YB_STATELESS 安全指定的函数。
\note 可被安全指定的函数或函数模板是 YB_PURE 限定的函数或函数模板的真子集。
\warning 要求满足指示的假定，否则行为未定义。
\since build 373

指示函数或函数模板的求值是返回值的计算，且返回值只依赖于参数的值，和其它存储无关。
若参数是对象指针或引用类型，还必须保证指向或引用的对象是其它参数，或者不被使用。
函数实现不能调用其它不能以 YB_STATELESS 限定的函数。
*/
#if YB_IMPL_GNUCPP >= 20500
#	define YB_STATELESS YB_ATTR(__const__)
#else
#	define YB_STATELESS
#endif

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
\todo 判断语言实现。
*/
#if defined(YB_DLL) && defined(YB_BUILD_DLL)
#	error DLL could not be built and used at the same time!
#endif

#ifdef YB_DLL
#	define YB_API __declspec(dllimport)
#elif defined(YB_BUILD_DLL)
#	define YB_API __declspec(dllexport)
#else
#	define YB_API
#endif


/*!
\def YB_USE_EXCEPTION_SPECIFICATION
\brief 使用 YBase 断言。
\since build 313
*/
#ifndef NDEBUG
#	define YB_USE_YASSERT
#endif


/*!
\def YB_USE_EXCEPTION_SPECIFICATION
\brief 使用 YBase 动态异常规范。
\since build 362
*/
#if 0 && !defined(NDEBUG)
#	define YB_USE_EXCEPTION_SPECIFICATION 1
#endif

//@}



/*!	\defgroup YBase_pseudo_keyword YBase Specified Pseudo-Keywords
\brief YBase 指定的替代关键字。
\since build 362
*/


/*!
\ingroup YBase_pseudo_keyword
\def yalignof
\brief 指定特定类型的对齐。
\note 同 C++11 alignof 作用于类型时的语义。
\since build 315
\todo 判断是否可使用 TR1 的情形。
*/
#if YB_HAS_ALIGNOF
#	define yalignof alignof
#else
#	define yalignof(_type) std::alignment_of<_type>::value
#endif


/*!
\ingroup YBase_pseudo_keyword
\def yconstexpr
\brief 指定编译时常量表达式。
\note 同 C++11 constepxr 作用于编译时常量的语义。
*/
/*!
\ingroup YBase_pseudo_keyword
\def yconstfn
\brief 指定编译时常量函数。
\note 同 C++11 constepxr 作用于编译时常量函数的语义。
*/
#if YB_HAS_CONSTEXPR
#	define yconstexpr constexpr
#	define yconstfn constexpr
#else
#	define yconstexpr const
#	define yconstfn inline
#endif


/*!
\ingroup YBase_pseudo_keyword
\def ythrow
\brief YSLib 动态异常规范：根据是否使用异常规范宏指定或忽略动态异常规范。
\note ythrow = "yielded throwing" 。
*/
#if YB_USE_EXCEPTION_SPECIFICATION
#	define ythrow throw
#else
#	define ythrow(...)
#endif

/*!
\ingroup YBase_pseudo_keyword
\def ynothrow
\brief YSLib 无异常抛出保证：若支持 noexcept 关键字，指定特定的 noexcept 异常规范。
*/
#if YB_HAS_NOEXCEPT
#	define ynothrow ynoexcept
#else
#	define ynothrow ythrow()
#endif

/*!
\ingroup YBase_pseudo_keyword
\def ynoexcept
\brief YSLib 无异常抛出保证：指定特定的异常规范。
\since build 319
*/
#if YB_HAS_NOEXCEPT
#	define ynoexcept noexcept
#else
#	define ynoexcept(...)
#endif


/*!
\ingroup YBase_pseudo_keyword
\def yconstraint
\brief 约束：接口语义。
\note 和普通断言相比强调接口契约。对于移植特定的平台实现时应予以特别注意。
\since build 298

运行时检查的接口语义约束断言。不满足此断言的行为是接口明确地未定义的，行为不可预测。
*/
#define yconstraint assert

/*!
\ingroup YBase_pseudo_keyword
\def yassume
\brief 假定：环境语义。
\note 和普通断言相比强调非接口契约。对于移植特定的平台实现时应予以特别注意。
\since build 298

运行时检查的环境条件约束断言。用于明确地非 yconstraint 适用的情形。
*/
#define yassume assert


namespace ystdex
{

/*!
\brief 字节类型。
\note ISO C++ 对访问存储的 glvalue 的类型有严格限制，当没有对象生存期保证时，
	仅允许（可能 cv 修饰的） char 和 unsigned char 及其指针/引用或 void* ，
	而不引起未定义行为(undefined behavior) 。
\since build 209
*/
typedef unsigned char byte;

#if CHAR_BIT == 8
/*!
\brief 八位组类型。
\note 一字节不保证等于 8 位，但一个八位组保证等于 8 位。
\since build 417
*/
typedef byte octet;
#	else
typedef void octet;
#endif

typedef int errno_t;
using std::ptrdiff_t;
using std::size_t;
using std::wint_t;

#if YB_HAS_BUILTIN_NULLPTR

using std::nullptr_t;

#else

/*!
\brief 空指针类。
\see 代码参考：http://topic.csdn.net/u/20100924/17/BE0C26F8-5127-46CD-9136-C9A96AAFDA76.html 。
*/
const class nullptr_t
{
public:
	/*
	\brief 转换任意类型至空非成员或静态成员指针。
	*/
	template<typename _type>
	inline
	operator _type*() const
	{
		return 0;
	}

	/*
	\brief 转换任意类型至空非静态成员指针。
	*/
	template<class _tClass, typename _type>
	inline
	operator _type _tClass::*() const
	{
		return 0;
	}
	/*
	\brief 支持关系运算符重载。
	*/
	template<typename _type>
	bool
	equals(const _type& rhs) const
	{
		return rhs == 0;
	}

	/*
	\brief 禁止取 nullptr 的指针。
	*/
	void operator&() const = delete;
} nullptr = {};

//! \since build 316
//@{
template<typename _type>
inline bool
operator==(nullptr_t lhs, const _type& rhs)
{
	return lhs.equals(rhs);
}
template<typename _type>
inline bool
operator==(const _type& lhs, nullptr_t rhs)
{
	return rhs.equals(lhs);
}

template<typename _type>
inline bool
operator!=(nullptr_t lhs, const _type& rhs)
{
	return !lhs.equals(rhs);
}
template<typename _type>
inline bool
operator!=(const _type& lhs, nullptr_t rhs)
{
	return !rhs.equals(lhs);
}
//@}

#endif


/*!
\brief 空基类模板。
\since build 260
*/
template<typename...>
struct empty_base
{};


/*!	\defgroup helper_functions Helper Functions
\brief 助手功能。
\since build 243

仅帮助简化编码形式或确定接口，并不包含编译期之后逻辑功能实现的代码设施。
*/

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
\ingroup YBase_pseudo_keyword
\def yoffsetof
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

传递参数：按类型保持值类别(value catory) 和 const 修饰符。
当表达式类型为函数或函数引用类型时，结果为左值(lvalue) ，否则：
当且仅当左值引用类型时结果为左值（此时类型不变）；
否则结果为对应的右值引用类型的消亡值(xvalue) 。
*/
#define yforward(_expr) std::forward<decltype(_expr)>(_expr)

/*!
\brief 无序列依赖表达式组求值实现。
\return 第一个参数的引用。
\since build 296
*/
template<typename _type, typename... _tParams>
yconstfn auto
unsequenced(_type&& arg, _tParams&&...) -> decltype(yforward(arg))
{
	return yforward(arg);
}

/*!
\ingroup YBase_pseudo_keyword
\brief 无序列依赖表达式组求值。
\note 由于实现限制，无法用于 void 类型表达式组。
\note 使用一元形式 %yunsequenced((_expr)) 的形式标记表达式组但不取消序列关系。
\note 支持嵌套使用。
\warning 非一元形式禁止用于产生对于同一对象的未序列化的(unsequenced) 副作用
	的表达式，否则存在未定义行为。
\warning 非一元形式不适用于对顺序有依赖的表达式，包括所有可能抛出异常且对抛出顺序
	敏感（例如 std::bad_cast 处理顺序不同可能造成内存泄露）的表达式。
\since build 266
*/
#define yunseq ystdex::unsequenced

} // namespace ystdex;

#endif

