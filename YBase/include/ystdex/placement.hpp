/*
	© 2015-2016, 2018-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file placement.hpp
\ingroup YStandardEx
\brief 放置对象管理操作。
\version r1117
\author FrankHB <frankhb1989@gmail.com>
\since build 715
\par 创建时间:
	2016-08-03 18:56:31 +0800
\par 修改时间:
	2022-03-11 22:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Placement

提供放置对象分配和释放操作，包括 WG21 P0040R3 提议和 ISO C++17 兼容的标签接口。
*/


#ifndef YB_INC_ystdex_placement_hpp_
#define YB_INC_ystdex_placement_hpp_ 1

#include "addressof.hpp" // for "addressof.hpp", cond_t, is_void, _t, vdefer,
//	std::align, sizeof_t, size_t_, identity, empty_base, YB_ASSUME,
//	ystdex::addressof, is_lvalue_reference, std::pair, is_nothrow_destructible,
//	or_, is_trivially_copyable, is_empty, std::reference_wrapper,
//	std::unique_ptr, std::shared_ptr, std::weak_ptr, std::allocator,
//	std::allocator_traits, enable_if_convertible_t, and_;
#include "deref_op.hpp" // for yconstraint, YB_VerifyIterator;
#include "bit.hpp" // for is_power_of_2_positive;
#include "variadic.hpp" // for vseq, ctor_of, when, _a;
#include <iterator> // for std::iterator_traits;
#include <new> // for placement ::operator new from standard library;
// NOTE: The following code is necessary to check for header <optional> to
//	ensure it has %in_place_t consistently. Other implementations are in
//	"optional.h".
// NOTE: Check of %__cplusplus is needed because SD-6 issues are not resolved in
//	mainstream implementations yet. See https://groups.google.com/a/isocpp.org/forum/#!topic/std-discussion/1rO2FiqWgtI
//	and https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79433 for details. For
//	feature-test marcos, see https://isocpp.org/std/standing-documents/sd-6-sg10-feature-test-recommendations.
//	See also https://docs.microsoft.com/cpp/visual-cpp-language-conformance and
//	https://docs.microsoft.com/cpp/preprocessor/predefined-macros.
#if (YB_IMPL_MSCPP >= 1910 && _MSVC_LANG >= 201603L) \
	|| (__cplusplus >= 201603L && __has_include(<optional>))
#	include <optional>
// NOTE: See also WG21 P0941R0 with minor fixes of the specification about WG21
//	P0032R3. Since WG21 P0941R2, the macro value is combined as '201606L' with
//	WG21 P0504R0 support which is not used here, so the check remains the value
//	in WG21 P0941R1.
#	if (YB_IMPL_MSCPP >= 1910 && _MSVC_LANG >= 201603L) \
	|| __cpp_lib_optional >= 201603L
#		define YB_Has_optional 1
#	endif
// NOTE: As per the specification, single <optional> without may indicate WG21
//	N3672 <optional>, which is WG21 N3793 <experimental/optional> without some
//	minor bug fixes. This is not fully supported to suppress YStandardEx
//	replacement (see "optional.hpp").
#elif __cplusplus > 201402L && __has_include(<experimental/optional>)
#	include <experimental/optional>
#	if __cpp_lib_experimental_optional >= 201411L
#		define YB_Has_optional 2
#	endif
#endif

/*!
\brief \c \<optional> 特性测试宏。
\see ISO C++20 [version.syn] 。
\see WG21 P0941R2 2.2 。
\see https://docs.microsoft.com/cpp/visual-cpp-language-conformance 。
\see https://docs.microsoft.com/cpp/preprocessor/predefined-macros 。
\since build 842
*/
//@{
#ifndef __cpp_lib_optional
#	if (YB_IMPL_MSCPP >= 1910 && _MSVC_LANG >= 201606L) \
	|| __cplusplus >= 201606L
#		define __cpp_lib_optional 201606L
#	endif
#endif
//@}

/*!	\defgroup allocators Allcators
\brief 分配器。
\warning 一般非虚析构。
\see WG21 N4606 17.6.3.5 [allocator.requirements] 。
\since build 746
*/

namespace ystdex
{

/*!
\brief 返回参数指定的指针值对齐到指定的对齐值。
\pre 断言：指针非空。
\pre 断言：对齐值是 2 的整数次幂。
\pre 指针指向对象。
\since build 843
*/
template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_PURE inline bool
is_aligned_ptr(_type* p, size_t alignment
	= yalignof(cond_t<is_void<_type>, std::max_align_t, _type>)) ynothrow
{
	yconstraint(p);
	yconstraint(is_power_of_2_positive(alignment));

	// XXX: This can be bit and of 'reinterpret_cast<std::uintptr_t>(ptr)' and
	//	'alignment - 1'. However, for portability, %std::align is used instead,
	//	even inefficient in general.
	void* ptr(p);
	size_t space(_t<cond_t<is_void<_type>, identity<size_t_<1>>,
		vdefer<sizeof_t, _type>>>::value);

	return bool(std::align(alignment, space, ptr, space));
}


/*!
\ingroup YBase_pseudo_keyword
\def yaligned
\brief 假定指定指针值按指定对齐值对齐。
\sa yassume
\sa yverify
\see WG21 P0886R0 。
\see https://gcc.gnu.org/gcc-4.7/changes.html 。
\see https://clang.llvm.org/docs/LanguageExtensions.html#builtin-functions 。
\see https://bugs.llvm.org/show_bug.cgi?id=16294 。
\since build 864
*/
// XXX: Although GCC supports non-constant alignment in its test suite, Clang++
//	does not and it would complain with "error: argument to
//	'__builtin_assume_aligned' must be a constant integer". WG21 P0886R0 also
//	porposes the attribute having constant-expression (or a type-id).
#if YB_IMPL_GNUCPP >= 40700
#	define yaligned(_p, _align) \
	(yverify(ystdex::is_aligned_ptr(_p, _align)), \
		__builtin_assume_aligned(_p, _align))
#elif false && (__has_builtin(__builtin_assume_aligned) \
	|| YB_IMPL_CLANGPP >= 35100)
#	define yaligned(_p, _align) \
	(yverify(ystdex::is_aligned_ptr(_p, _align)), __builtin_constant_p(_align) \
		? __builtin_assume_aligned(_p, _align) : _p)
#else
#	define yaligned(_p, _align) \
		(yassume(ystdex::is_aligned_ptr(_p, _align)), _p)
#endif


//! \ingroup tags
//@{
/*!
\brief 默认初始化标记。
\since build 677
*/
yconstexpr_inline const struct default_init_t{} default_init{};
/*!
\brief 值初始化标记。
\since build 705
*/
yconstexpr_inline const struct value_init_t{} value_init{};
//@}


/*!
\brief 对非模板和不同模板参数相同的类型标签接口。
\since build 831

提供兼容原地放置操作标记类型的命名空间。
*/
namespace uniformed_tags
{

/*!
\ingroup YBase_replacement_features tags
\see WG21 P0032R3 。
\see WG21 N4606 20.2.7[utility.inplace] 。
\see https://stackoverflow.com/questions/40923097/stdin-place-t-and-friends-in-c17 。
\see https://github.com/cplusplus/draft/commit/d2d23690a253b91fb7ccb1631581bd9c8f2937d2 。
\since build 717
*/
//@{
//! \brief 原地标记类型。
struct in_place_tag
{
	in_place_tag() = delete;
};

// NOTE: See WG21 P0504R0.
//! \note 使用指针代替引用避免退化问题导致无法按值传递。
//@{
//! \brief 原地空标记类型。
using in_place_t = in_place_tag(*)(yimpl(empty_base<>));

//! \brief 原地类型标记模板。
template<typename _type>
using in_place_type_t = in_place_tag(*)(yimpl(empty_base<_type>));

//! \brief 原地索引标记模板。
template<size_t _vIdx>
using in_place_index_t = in_place_tag(*)(yimpl(size_t_<_vIdx>));
//@}

/*!
\ingroup helper_functions
\brief 原地标记函数。
\warning 调用引起未定义行为。
\note 注意 std::optional 不需要支持 std::in_place 作为值存储。
*/
yimpl(YB_NORETURN) inline in_place_tag
in_place(yimpl(empty_base<>))
{
	YB_ASSUME(false);
}
template<typename _type>
yimpl(YB_NORETURN) in_place_tag
in_place(yimpl(empty_base<_type>))
{
	YB_ASSUME(false);
}
template<size_t _vIdx>
yimpl(YB_NORETURN) in_place_tag
in_place(yimpl(size_t_<_vIdx>))
{
	YB_ASSUME(false);
}
//@}

} // namespace uniformed_tags;

/*!
\see ISO C++17 [utility.inplace] 。
\since build 831
*/
inline namespace cpp2017
{

#if YB_Has_optional == 1
using std::in_place_t;
using std::in_place;
using std::in_place_type_t;
using std::in_place_type;
using std::in_place_index_t;
using std::in_place_index;
#else
#	if YB_Has_optional == 2
using std::experimental::in_place_t;
using std::experimental::in_place;
#	else
using uniformed_tags::in_place_t;
// XXX: Using instead of %in_place_type and %in_place_index is not supported
//	here, even it is supported by %uniformed_tags::in_place;
using uniformed_tags::in_place;
#	endif

using uniformed_tags::in_place_type_t;
using uniformed_tags::in_place_index_t;

template<typename _type>
yimpl(YB_NORETURN) yimpl(uniformed_tags::in_place_tag)
in_place_type(yimpl(empty_base<_type>))
{
	YB_ASSUME(false);
}

template<size_t _vIdx>
yimpl(YB_NORETURN) yimpl(uniformed_tags::in_place_tag)
in_place_index(yimpl(size_t_<_vIdx>))
{
	YB_ASSUME(false);
}
#endif

} // inline namespace cpp2017;

//! \since build 851
//@{
//! \ingroup tags
template<typename _type>
struct in_place_index_ctor
{
	using type = in_place_index_t<_type::value>;
};


namespace vseq
{

//! \ingroup metafunctions
//@{
template<typename _type>
struct ctor_of<in_place_type_t<_type>, when<true>>
{
	using type = _a<in_place_type_t>;
};

template<size_t _vIdx>
struct ctor_of<in_place_index_t<_vIdx>, when<true>>
{
	using type = _a<in_place_index_ctor>;
};
//@}

} // namespace vseq;
//@}


/*!
\tparam _type 构造的对象类型。
\param obj 构造的存储对象。
\since build 716
*/
//@{
//! \brief 以默认初始化在对象中构造。
template<typename _type, typename _tObj>
YB_ATTR_nodiscard YB_ATTR_returns_nonnull yconstfn _type*
construct_default_within(_tObj& obj)
{
	return ::new(
		static_cast<void*>(static_cast<_tObj*>(ystdex::addressof(obj)))) _type;
}

/*!
\brief 以值初始化在对象中构造。
\tparam _tParams 用于构造对象的参数包类型。
\param args 用于构造对象的参数包。
*/
template<typename _type, typename _tObj, typename... _tParams>
// XXX: The code generated by x86_64-pc-linux G++ 11.1.0 at '-O3' (regardless of
//	any of the '-O' modes enforced here) is wrong. Similarly, i686-w64-mingw32
//	G++ at '-O3' 10.2.0 may cause crashes. Just disable the inlining for GCC.
#if (YB_IMPL_GNUCPP && !YB_IMPL_CLANGPP) && defined(NDEBUG) && __OPTIMIZE__
YB_ATTR_always_inline
#endif
YB_ATTR_nodiscard YB_ATTR_returns_nonnull yconstfn _type*
construct_within(_tObj& obj, _tParams&&... args)
{
	return ::new(static_cast<void*>(
		static_cast<_tObj*>(ystdex::addressof(obj)))) _type(yforward(args)...);
}
//@}

/*!
\brief 以默认初始化原地构造。
\tparam _tParams 用于构造对象的参数包类型。
\param args 用于构造对象的参数包。
\since build 716
*/
template<typename _type>
yconstfn_relaxed void
construct_default_in(_type& obj)
{
	yunused(ystdex::construct_default_within<_type>(obj));
}

/*!
\brief 以值初始化原地构造。
\tparam _tParams 用于构造对象的参数包类型。
\param args 用于构造对象的参数包。
\since build 692
*/
template<typename _type, typename... _tParams>
yconstfn_relaxed void
construct_in(_type& obj, _tParams&&... args)
{
	yunused(ystdex::construct_within<_type>(obj, yforward(args)...));
}

//! \since build 602
//@{
//! \tparam _tIter 迭代器类型。
//@{
/*!
\tparam _tParams 用于构造对象的参数包类型。
\param args 用于构造对象的参数包。
\pre 断言：指定范围末尾以外的迭代器满足 YB_VerifyIterator 。
*/
//@{
/*!
\brief 使用指定参数在迭代器指定的位置以指定参数初始化构造对象。
\param i 迭代器。
\note 显式转换为 void* 指针以实现标准库算法 uninitialized_* 实现类似的语义。
\see libstdc++ 5 和 Microsoft VC++ 2013 标准库在命名空间 std 内对指针类型的实现：
	_Construct 模板。
*/
template<typename _tIter, typename... _tParams>
void
construct(_tIter i, _tParams&&... args)
{
	using value_type = typename std::iterator_traits<_tIter>::value_type;

	YB_VerifyIterator(i);
	ystdex::construct_in<value_type>(*i, yforward(args)...);
}

/*!
\brief 使用指定参数在迭代器指定的位置以默认初始化构造对象。
\param i 迭代器。
\since build 716
*/
template<typename _tIter>
void
construct_default(_tIter i)
{
	using value_type = typename std::iterator_traits<_tIter>::value_type;

	YB_VerifyIterator(i);
	ystdex::construct_default_in<value_type>(*i);
}

/*!
\brief 使用指定的参数重复构造迭代器范围内的对象序列。
\note 参数被传递的次数和构造的对象数相同。
*/
template<typename _tIter, typename... _tParams>
void
construct_range(_tIter first, _tIter last, _tParams&&... args)
{
	for(; first != last; ++first)
		ystdex::construct(first, yforward(args)...);
}
//@}


//! \since build 835
inline namespace cpp2017
{

/*!
\brief 原地销毁。
\note 不保证支持带有执行策略的重载。
\see ISO C++17 [specialized.destroy] 。
\see WG21 P0040R3 。
\see https://docs.microsoft.com/cpp/visual-cpp-language-conformance 。
*/
//@{
/*!
\see libstdc++ 5 和 Microsoft VC++ 2013 标准库在命名空间 std 内对指针类型的实现：
	_Destroy 模板。
\see https://docs.microsoft.com/cpp/preprocessor/predefined-macros 。
*/
#if (YB_IMPL_MSCPP >= 1911 && _MSVC_LANG >= 201606L) \
	|| (((YB_IMPL_GNUCPP >= 80000 && __GLIBCXX__ > 20160914) \
	|| _LIBCPP_VERSION > 4000) && __cplusplus >= 201606L) \
	|| __cpp_lib_raw_memory_algorithms >= 201606L
//! \since build 835
//@{
using std::destroy_at;
using std::destroy;
using std::destroy_n;
//@}
#else
template<typename _type>
yconstfn_relaxed void
destroy_at(_type* location)
{
	yconstraint(location);
	location->~_type();
}

//! \see libstdc++ 5 标准库在命名空间 std 内对迭代器范围的实现： _Destroy 模板。
template<typename _tFwd>
yconstfn_relaxed void
destroy(_tFwd first, _tFwd last)
{
	for(; first != last; ++first)
		ystdex::destroy_at(ystdex::addressof(*first));
}

template<typename _tFwd, typename _tSize>
yconstfn_relaxed _tFwd
destroy_n(_tFwd first, _tSize n)
{
	// XXX: To reduce dependency on resolution of LWG 2598.
	static_assert(is_lvalue_reference<decltype(*first)>(),
		"Invalid iterator reference type found.");

	// NOTE: There can be improved diagnostics as https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80553,
	//	which is not required for conforming. Note WG21 P0411R0 is not adopted.
	// XXX: Excessive order refinment by ','?
	for(; n > 0; ++first, static_cast<void>(--n))
		ystdex::destroy_at(ystdex::addressof(*first));
	return first;
}
#endif
//@}
//@}

} // inline namespace cpp2017;


/*!
\brief 原地析构。
\tparam _type 用于析构对象的类型。
\param obj 析构的对象。
\since build 693
\sa destroy_at
*/
template<typename _type>
yconstfn_relaxed void
destruct_in(_type& obj)
{
	// XXX: See https://developercommunity.visualstudio.com/content/problem/431598/vc-fails-to-compile-the-noexcept-expression-inside.html.
#if YB_IMPL_MSCPP < 1921
	// XXX: This should allow private memberwise %operator delete.
	::delete static_cast<_type*>(nullptr);
#endif
	obj.~_type();
}

/*!
\brief 析构迭代器指向的对象。
\param i 迭代器。
\pre 断言：满足 YB_VerifyIterator 。
\sa destroy
*/
template<typename _tIter>
void
destruct(_tIter i)
{
	using value_type = typename std::iterator_traits<_tIter>::value_type;

	YB_VerifyIterator(i);
	ystdex::destruct_in<value_type>(*i);
}

/*!
\brief 析构d迭代器范围内的对象序列。
\note 保证顺序析构。
\sa destroy
*/
template<typename _tIter>
yconstfn_relaxed void
destruct_range(_tIter first, _tIter last)
{
	for(; first != last; ++first)
		ystdex::destruct(first);
}
//@}


//! \since build 835
inline namespace cpp2017
{

#define YB_Impl_UninitGuard_Begin \
	auto i = first; \
	\
	try \
	{

// NOTE: The order of destruction is unspecified.
#define YB_Impl_UninitGuard_End \
	} \
	catch(...) \
	{ \
		ystdex::destruct_range(first, i); \
		throw; \
	}

/*!
\note 不保证支持带有执行策略的重载。
\see WG21 P0040R3 。
\see https://docs.microsoft.com/cpp/visual-cpp-language-conformance 。
\see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=77619 。
\see http://lists.llvm.org/pipermail/cfe-commits/Week-of-Mon-20161010/173499.html 。
*/
//@{
#if (YB_IMPL_MSCPP >= 1911 && _MSVC_LANG >= 201606L) \
	|| (((YB_IMPL_GNUCPP >= 80000 && __GLIBCXX__ > 20160920) \
	|| _LIBCPP_VERSION > 4000) && __cplusplus >= 201606L) \
	|| __cpp_lib_raw_memory_algorithms >= 201606L
//! \since build 835
//@{
using std::uninitialized_default_construct;
using std::uninitialized_default_construct_n;
using std::uninitialized_value_construct;
using std::uninitialized_value_construct_n;
using std::uninitialized_move;
using std::uninitialized_move_n;
//@}
//! \since build 716
//@{
//! \brief 在范围内未初始化放置构造。
//@{
//! \see ISO C++17 [uninitialized.construct.default] 。
//@{
#else
template<typename _tFwd>
inline void
uninitialized_default_construct(_tFwd first, _tFwd last)
{
	YB_Impl_UninitGuard_Begin
		for(; first != last; ++first)
			ystdex::construct_default(first);
	YB_Impl_UninitGuard_End
}

template<typename _tFwd, typename _tSize>
_tFwd
uninitialized_default_construct_n(_tFwd first, _tSize n)
{
	YB_Impl_UninitGuard_Begin
		// XXX: Excessive order refinment by ','?
		for(; n > 0; ++first, static_cast<void>(--n))
			ystdex::construct_default(first);
	YB_Impl_UninitGuard_End
}
//@}

//! \see ISO C++17 [uninitialized.construct.value] 。
template<typename _tFwd>
inline void
uninitialized_value_construct(_tFwd first, _tFwd last)
{
	YB_Impl_UninitGuard_Begin
		ystdex::construct_range(first, last);
	YB_Impl_UninitGuard_End
}

template<typename _tFwd, typename _tSize>
_tFwd
uninitialized_value_construct_n(_tFwd first, _tSize n)
{
	YB_Impl_UninitGuard_Begin
		// XXX: Excessive order refinment by ','?
		for(; n > 0; ++first, static_cast<void>(--n))
			ystdex::construct(first);
	YB_Impl_UninitGuard_End
	return first;
}
//@}


/*!
\brief 转移初始化范围。
\see ISO C++17 [uninitialized.move] 。
\since build 716
*/
//@{
template<typename _tIn, class _tFwd>
_tFwd
uninitialized_move(_tIn first, _tIn last, _tFwd result)
{
	YB_Impl_UninitGuard_Begin
		for(; first != last; ++result, static_cast<void>(++first))
			ystdex::construct(result, std::move(*first));
	YB_Impl_UninitGuard_End
	return result;
}

template<typename _tIn, typename _tSize, class _tFwd>
std::pair<_tIn, _tFwd>
uninitialized_move_n(_tIn first, _tSize n, _tFwd result)
{
	YB_Impl_UninitGuard_Begin
		// XXX: Excessive order refinment by ','?
		for(; n > 0; ++result, ++first, static_cast<void>(--n))
			ystdex::construct(result, std::move(*first));
	YB_Impl_UninitGuard_End
	return {first, result};
}
//@}
#endif
//@}

} // inline namespace cpp2017;


/*!
\brief 在迭代器指定的未初始化的范围上构造对象。
\tparam _tFwd 输出范围前向迭代器类型。
\tparam _tParams 用于构造对象的参数包类型。
\param first 输出范围起始迭代器。
\param args 用于构造对象的参数包。
\note 参数被传递的次数和构造的对象数相同。
\note 接口不保证失败时的析构顺序。
*/
//@{
/*!
\param last 输出范围终止迭代器。
\note 和 std::unitialized_fill 类似，但允许指定多个初始化参数。
\see WG21 N4431 20.7.12.3[uninitialized.fill] 。
*/
template<typename _tFwd, typename... _tParams>
void
uninitialized_construct(_tFwd first, _tFwd last, _tParams&&... args)
{
	YB_Impl_UninitGuard_Begin
		for(; i != last; ++i)
			ystdex::construct(i, yforward(args)...);
	YB_Impl_UninitGuard_End
}

/*!
\tparam _tSize 范围大小类型。
\param n 范围大小。
\note 和 std::unitialized_fill_n 类似，但允许指定多个初始化参数。
\see WG21 N4431 20.7.12.4[uninitialized.fill.n] 。
*/
template<typename _tFwd, typename _tSize, typename... _tParams>
void
uninitialized_construct_n(_tFwd first, _tSize n, _tParams&&... args)
{
	YB_Impl_UninitGuard_Begin
		// NOTE: This form is by specification (WG21 N4431) of
		//	'std::unitialized_fill' literally.
		for(; n--; ++i)
			ystdex::construct(i, yforward(args)...);
	YB_Impl_UninitGuard_End
}
//@}
//@}

#undef YB_Impl_UninitGuard_End
#undef YB_Impl_UninitGuard_Begin


/*!
\brief 复制赋值操作。
\note 允许不可复制赋值或转移赋值的类型通过复制构造实现复制赋值。
\since build 941
*/
//@{
//! \note 直接使用赋值操作。
template<typename _type>
inline _type&
copy_assign(_type& x, const _type& y, true_) ynoexcept_spec(x = y)
{
	x = y;
	return x;
}
//! \note 使用复制构造和转移赋值操作。
template<typename _type>
inline _type&
copy_assign(_type& x, const _type& y, false_, true_) ynoexcept_spec(x = _type(y))
{
	x = _type(y);
	return x;
}
/*!
\note 使用析构和复制构造操作。
\note 非强异常安全：若复制构造对象抛出异常，被赋值的对象已被销毁。
*/
template<typename _type>
inline _type&
copy_assign(_type& x, const _type& y, false_, false_)
	ynoexcept_spec(ystdex::construct_in(x, y))
{
	static_assert(is_nothrow_destructible<_type>(), "Invalid type found.");

	ystdex::destruct_in(x);
	ystdex::construct_in(x, y);
	return x;
}
//! \warning 不完全强异常安全：对不可复制和转移赋值的类型。
//@{
//! \note 使用复制构造和转移赋值或析构和复制构造操作。
template<typename _type>
inline _type&
copy_assign(_type& x, const _type& y, false_) ynoexcept_spec(
	ystdex::copy_assign(x, y, false_(), is_move_assignable<_type>()))
{
	return ystdex::copy_assign(x, y, false_(), is_move_assignable<_type>());
}
//! \note 根据是否可直接复制赋值选择操作。
template<typename _type>
inline _type&
copy_assign(_type& x, const _type& y)
	ynoexcept_spec(ystdex::copy_assign(x, y, is_copy_assignable<_type>()))
{
	return ystdex::copy_assign(x, y, is_copy_assignable<_type>());
}
//@}
//@}


/*!
\ingroup customization_points
\ingroup unary_type_traits
\brief 判断类型是否可交换对象的表示。
\note 不排除假阴性结果。
\note 特定的程序提供的类型可特化本特征以满足语义要求或提供语义保证。
\see WG21 P1144R6 。
\since build 926
\todo 使用 is_trivially_swappable 或 is_trivially_relocatable 。

判断类型是否是交换时可使用交换对象的表示代替对象的交换操作。
交换对象的表示一般实现为三次 std::memcpy ，而不是 std::swap 主模板的三次赋值；
	这不要求对象可赋值。
由核心语言实现支持的可平凡交换 is_trivially_swappable
	可从类的成员和基类子对象的类型推断可交换性；
在没有直接的支持时，通过两种方式近似：
	若核心语言实现支持可平凡重定位特征 is_trivially_relocatable ，其直接蕴含
		is_trivially_swappable ，但除此之外还蕴含可平凡默认构造，这是不必要的；
	否则，可平凡复制特征 is_trivially_copyable 是另一种更保守的模拟近似。
模拟策略的有效性来自 is_trivially_copyable 保证能通过等效 std::memcpy
	的方式交换存储，但模拟是不充分的，因此不排除假阴性结果。
对类类型，没有虚函数及基类和子对象也可能满足交换时可重定位，
	因为缺少语义信息而不在默认策略中指定。
默认策略由主模板实现。替换默认策略时，具体类型和其它程序提供的类型可特化本特征。
特化不应提供假阳性结果，以避免提供错误的保证。
	例如，非 final 多态类除非排除作为多态基类使用，不应满足可平凡交换；
	类似的例子对平凡重定位的限制见 https://rawgit.com/Quuxplusone/draft/gh-pages/d1144-object-relocation.html#non-trivial-sample-polymorphic 。
当前实现不依赖基于核心语言的支持，在主模板中直接使用 is_trivially_copyable 模拟，
	并确保空类类型对象（即便不可复制）结果为真。
以下 is_trivially_copyable 提供的保证并不被本特征依赖：
对满足 is_trivially_copyable 的类型，保证连续存储。
因为允许使用不排除假阴性结果的模拟，本特征不使用 is_trivially_swappable 的名称，
	命名策略类似 https://gcc.gnu.org/bugzilla/show_bug.cgi?id=87106#c18 。
*/
//@{
template<typename _type>
struct is_bitwise_swappable
#if false
	: is_trivially_relocatable<_type>
#else
	: or_<is_trivially_copyable<_type>, is_empty<_type>>
#endif
{};

template<typename _type>
struct is_bitwise_swappable<_type&> : true_
{};

template<typename _type>
struct is_bitwise_swappable<_type&&> : true_
{};

template<typename _type>
struct is_bitwise_swappable<std::reference_wrapper<_type>> : true_
{};

template<typename _type, class _tDeleter>
struct is_bitwise_swappable<std::unique_ptr<_type, _tDeleter>>
	: and_<is_bitwise_swappable<_tDeleter>,
	is_bitwise_swappable<typename std::unique_ptr<_type, _tDeleter>::pointer>>
{};

template<typename _type>
struct is_bitwise_swappable<std::shared_ptr<_type>> : true_
{};

template<typename _type>
struct is_bitwise_swappable<std::weak_ptr<_type>> : true_
{};
//@}


//! \since build 716
//@{
/*!
\ingroup allocators
\pre 第二参数是可被继承可复制构造的分配器类型。
\brief 默认初始化构造分配器。
*/
template<typename _type, class _tAlloc = std::allocator<_type>>
class default_init_allocator : public _tAlloc
{
public:
	using allocator_type = _tAlloc;
	using traits_type = std::allocator_traits<allocator_type>;
	//! \ingroup functors
	template<typename _tOther>
	struct rebind
	{
		using other = default_init_allocator<_tOther,
			typename traits_type::template rebind_alloc<_tOther>>;
	};

	/*!
	\warning 语义视基类对象构造函数的形式可能不同。
	\sa __cpp_inheriting_constructors
	\see WG21 P0136R1 。
	*/
	using allocator_type::allocator_type;

	//! \since build 864
	template<typename _tOther>
	void
	construct(_tOther* p) ynoexcept(_tOther())
	{
		::new(static_cast<void*>(p)) _tOther;
	}
	template<typename _tOther, typename... _tParams>
	void
	construct(_type* p, _tParams&&... args)
	{
		traits_type::construct(static_cast<allocator_type&>(*this), p,
			yforward(args)...);
	}
};


/*!
\brief 放置存储的对象删除器：释放时调用伪析构函数。
\tparam _type 被删除的对象类型。
\pre _type 满足 Destructible 。
*/
template<typename _type, typename _tPointer = _type*>
struct placement_delete
{
	using pointer = _tPointer;

	yconstfn placement_delete() ynothrow = default;
	template<typename _type2,
		yimpl(typename = enable_if_convertible_t<_type2*, _type*>)>
	placement_delete(const placement_delete<_type2>&) ynothrow
	{}

	void
	operator()(pointer p) const ynothrowv
	{
		ystdex::destroy_at(p);
	}
};


//! \brief 独占放置存储的对象所有权的指针。
template<typename _type, typename _tPointer = _type*>
using placement_ptr
	= std::unique_ptr<_type, placement_delete<_type, _tPointer>>;
//@}


/*!
\brief 带记号标记的可选值。
\warning 非虚析构。
\since build 718
*/
template<typename _tToken, typename _type>
struct tagged_value
{
	using token_type = _tToken;
	using value_type = _type;

	token_type token;
	union
	{
		empty_base<> empty;
		mutable _type value;
	};

	//! \since build 864
	yconstfn
#if !YB_IMPL_GNUCPP || YB_IMPL_GNUCPP < 70000 || YB_IMPL_GNUCPP > 70300
	tagged_value() ynoexcept_spec(token_type())
	// XXX: Blocked. See $2019-08 @ %Documentation::Workflow.
#else
	tagged_value() ynoexcept(is_nothrow_default_constructible<token_type>())
#endif
		: token(), empty()
	{}
	explicit
	tagged_value(default_init_t)
	{}
	tagged_value(token_type t)
		: token(t), empty()
	{}
	template<typename... _tParams>
	explicit yconstfn
	tagged_value(token_type t, in_place_t, _tParams&&... args)
		: token(t), value(yforward(args)...)
	{}
	tagged_value(const tagged_value& v)
		: token(v.token)
	{}
	/*!
	\brief 析构：空实现。
	\note 不使用默认函数以避免派生此类的非平凡析构函数非合式而被删除。
	*/
	~tagged_value()
	{}

	template<typename... _tParams>
	yconstfn_relaxed void
	construct(_tParams&&... args)
	{
		ystdex::construct_in(value, yforward(args)...);
	}

	yconstfn_relaxed void
	destroy() ynoexcept(is_nothrow_destructible<value_type>())
	{
		ystdex::destruct_in(value);
	}

	yconstfn_relaxed void
	destroy_nothrow() ynothrow
	{
		ynoexcept_assert("Invalid type found.", value.~value_type());

		destroy();
	}
};

/*!
\relates tagged_value
\since build 926
*/
template<typename _tToken, typename _type>
struct is_bitwise_swappable<tagged_value<_tToken, _type>>
	: and_<is_bitwise_swappable<_tToken>, is_bitwise_swappable<_type>>
{};

} // namespace ystdex;

#endif

