﻿/*
	© 2011-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file memory.hpp
\ingroup YStandardEx
\brief 存储和智能指针特性。
\version r4024
\author FrankHB <frankhb1989@gmail.com>
\since build 209
\par 创建时间:
	2011-05-14 12:25:13 +0800
\par 修改时间:
	2020-01-12 18:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Memory

扩展标准库头 \c \<memory> ，提供对智能指针类型的操作及判断迭代器不可解引用的模板。
*/


#ifndef YB_INC_ystdex_memory_hpp_
#define YB_INC_ystdex_memory_hpp_ 1

#include "placement.hpp" // for internal "placement.hpp", <memory>,
//	detected_or_t, is_unqualified_object, is_copy_constructible, is_class_type,
//	and_, is_same, std::pointer_traits, std::declval, is_detected, true_,
//	cond_or_t, detected_t, is_detected_exact, vdefer, false_, cond,
//	is_lvalue_reference, exclude_self_t, enable_if_convertible_t, bool_,
//	remove_reference_t, indirect_element_t, not_, is_void, remove_pointer_t,
//	yconstraint, is_pointer, enable_if_t, is_array, extent, remove_extent_t,
//	is_polymorphic, std::allocator_arg, is_explicitly_constructible,
//	is_implicitly_constructible;
#include "type_op.hpp" // for has_mem_value_type, cond_or;
#include "pointer.hpp" // for internal "pointer.hpp", "iterator_op.hpp",
//	ystdex::swap_dependent, ystdex::to_address;
#include <limits> // for std::numeric_limits;
#include "exception.h" // for throw_invalid_construction;
#include "ref.hpp" // for is_reference_wrapper;
#include "tuple.hpp" // for std::forward_as_tuple, std::make_tuple,
//	std::piecewise_construct_t, std::piecewise_construct, ystdex::apply;

/*!
\brief \c \<memory> 特性测试宏。
\see WG21 P0941R2 2.2 。
\since build 628
*/
//@{
/*!
\see LWG 2108 。
\see LWG 2467 。
\see https://blogs.msdn.microsoft.com/vcblog/2016/01/22/vs-2015-update-2s-stl-is-c17-so-far-feature-complete/ 。
\since build 834
*/
//@{
#ifndef __cpp_lib_allocator_traits_is_always_equal
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201411L
#		define __cpp_lib_allocator_traits_is_always_equal 201411L
#	endif
#endif
//@}
#ifndef __cpp_lib_make_unique
#	if YB_IMPL_MSCPP >= 1800 || __cplusplus >= 201304L
#		define __cpp_lib_make_unique 201304L
#	endif
#endif
//@}

/*!	\defgroup allocators Allcators
\brief 分配器。
\see WG21 N4606 17.6.3.5 [allocator.requirements] 。
\since build 746
*/

namespace ystdex
{

//! \since build 830
#if __cpp_lib_allocator_traits_is_always_equal >= 201411L \
	|| __cplusplus >= 201411L
#	define YB_Impl_Has_allocator_traits_is_always_equal true
#else
#	define YB_Impl_Has_allocator_traits_is_always_equal false
#endif


//! \since build 595
namespace details
{

//! \since build 830
//@{
#if !YB_Impl_Has_allocator_traits_is_always_equal
template<typename _type, typename... _tParams>
using mem_is_always_equal_t = typename _type::is_always_equal;
#endif
//@}

} // namespace details;

//! \since build 595
//@{
inline namespace cpp2017
{

#if YB_Impl_Has_allocator_traits_is_always_equal
using std::allocator_traits;
#else
template<class _tAlloc>
struct allocator_traits : std::allocator_traits<_tAlloc>
{
	using is_always_equal = detected_or_t<is_empty<_tAlloc>,
		details::mem_is_always_equal_t, _tAlloc>;
};
#endif

} // inline namespace cpp2017;


//! \since build 848
//@{
//! \ingroup unary_type_traits
template<class _tAlloc>
using alloc_value_t = typename allocator_traits<_tAlloc>::value_type;

//! \since build 867
//@{
template<class _tAlloc>
using alloc_pointer_t = typename allocator_traits<_tAlloc>::pointer;

template<class _tAlloc>
using alloc_const_pointer_t = typename allocator_traits<_tAlloc>::const_pointer;

template<class _tAlloc>
using alloc_size_type_t = typename allocator_traits<_tAlloc>::size_type;

template<class _tAlloc>
using alloc_difference_type_t
	= typename allocator_traits<_tAlloc>::difference_type;
//@}

//! \ingroup binary_type_traits
//@{
template<class _tAlloc, typename _type>
using rebind_alloc_t
	= typename allocator_traits<_tAlloc>::template rebind_alloc<_type>;

template<class _tAlloc, typename _type>
using rebind_traits_t
	= typename allocator_traits<_tAlloc>::template rebind_traits<_type>;
//@}
//@}


/*!
\ingroup unary_type_traits
\brief 判断类型是否符合分配器要求的目标类型。
\note _type 可以是不完整类型。
\see LWG 2447 。
\since build 650
*/
template<typename _type>
struct is_allocatable : is_unqualified_object<_type>
{};


//! \since build 595
namespace details
{

template<typename _type>
using is_copy_constructible_class
	= and_<is_copy_constructible<_type>, is_class_type<_type>>;

//! \since build 867
//@{
template<class _tAlloc>
using mem_pointer_t = typename _tAlloc::pointer;

template<class _tAlloc>
using mem_const_pointer_t = typename _tAlloc::const_pointer;

template<class _tAlloc>
using mem_const_pointer_conv_t = is_convertible<mem_pointer_t<_tAlloc>,
	mem_const_pointer_t<_tAlloc>>;

template<class _tAlloc>
using mem_void_pointer_t = typename _tAlloc::void_pointer;

template<class _tAlloc>
using mem_void_pointer_conv_t = is_convertible<mem_pointer_t<_tAlloc>,
	mem_void_pointer_t<_tAlloc>>;

template<class _tAlloc>
using mem_const_void_pointer_t = typename _tAlloc::const_void_pointer;

template<class _tAlloc>
using mem_const_void_pointer_conv_t = and_<is_convertible<mem_pointer_t<
	_tAlloc>, mem_const_void_pointer_t<_tAlloc>>,
	is_convertible<mem_const_pointer_t<_tAlloc>,
	mem_const_void_pointer_t<_tAlloc>>, is_convertible<
	mem_void_pointer_t<_tAlloc>, mem_const_void_pointer_t<_tAlloc>>>;

template<class _tAlloc>
using same_pointer_to_test_t
	= is_same<decltype(std::pointer_traits<mem_pointer_t<_tAlloc>>::pointer_to(
	*std::declval<mem_pointer_t<_tAlloc>>())), mem_pointer_t<_tAlloc>>;

template<class _tAlloc>
using same_pointer_to_t = cond_or_t<is_detected<mem_pointer_t, _tAlloc>, true_,
	same_pointer_to_test_t, _tAlloc>;

template<class _tAlloc>
using mem_allocate_t = decltype(std::declval<_tAlloc&>().allocate(size_t(0)));

template<class _tAlloc, typename _tPointer = decltype(
	std::declval<typename allocator_traits<_tAlloc>::const_void_pointer>())>
using ator_allocate_hint_t = decltype(allocator_traits<_tAlloc>::allocate(
	std::declval<_tAlloc&>(), size_t(0), std::declval<_tPointer>()));

template<class _tAlloc>
using mem_deallocate_t = decltype(std::declval<_tAlloc&>().deallocate(
	std::declval<mem_allocate_t<_tAlloc>>(), size_t(0)));

template<class _tAlloc>
using ator_max_size_t
	= decltype(allocator_traits<_tAlloc>::max_size(std::declval<_tAlloc&>()));

// XXX: Details member %mem_value_type_t across headers.
template<class _type, typename _tPointer = detected_t<alloc_pointer_t,
	_type>, typename _tSize = detected_t<alloc_size_type_t, _type>,
	typename _tDifference = detected_t<alloc_difference_type_t, _type>>
struct check_allocator_members : and_<cond_or_t<is_detected<
	mem_const_pointer_t, _type>, true_, mem_const_pointer_conv_t, _type>,
	cond_or_t<is_detected<mem_void_pointer_t, _type>, true_,
	mem_void_pointer_conv_t, _type>, cond_or_t<is_detected<
	mem_const_void_pointer_t, _type>, true_, mem_const_void_pointer_conv_t,
	_type>, is_unsigned<_tSize>, is_signed<_tDifference>,
	same_pointer_to_t<_type>,
	is_detected_exact<_tPointer, mem_allocate_t, _type>,
	is_detected_exact<_tPointer, ator_allocate_hint_t, _type>,
	is_detected<mem_deallocate_t, _type>,
	is_detected_exact<_tSize, ator_max_size_t, _type>>
{};

template<typename _type,
	typename _tValue = detected_or_t<void, mem_value_type_t, _type>>
struct is_allocator
	: cond_or_t<is_allocatable<_tValue>, false_, check_allocator_members, _type>
{};

template<typename _type, typename _tValue>
using same_alloc_value_t = is_same<alloc_value_t<_type>, _tValue>;
//@}

//! \since build 650
template<typename _type>
using nested_allocator_t = typename _type::allocator_type;


//! \since build 746
//@{
template<typename _type, typename... _tParams>
using mem_new_t
	= decltype(_type::operator new(std::declval<_tParams>()...));

template<typename _type, typename... _tParams>
using mem_delete_t
	= decltype(_type::operator delete(std::declval<_tParams>()...));
//@}

} // namespace details;

/*!
\ingroup type_traits_operations
\since build 746
*/
//@{
template<typename _type, typename... _tParams>
struct has_mem_new : is_detected<details::mem_new_t, _type, _tParams...>
{};

template<typename _type, typename... _tParams>
struct has_mem_delete : is_detected<details::mem_delete_t, _type, _tParams...>
{};
//@}


/*!
\pre _type 是非类类型或完整的类类型。
\note 可能有假阳性结果。
\see ISO C++17 [allocator.requirements] 。
\since build 867
*/
//@{
/*!
\ingroup unary_type_traits
\brief 判断类型是分配器。

判断分配器，检查符合以下要求：
成员类型 value_type 是没有被 cv 修饰的（可能不完整的）对象类型；
不涉及其它分配器、 操作符、容器传播或 is_always_equal 的其它表达式符合类型要求；
可以符合分配器要求的值调用 allocate 、deallocator 和 max_size 。
因为 ADL ，带有非限定形式操作符不能被可靠地检查而支持不完整的 value_type 。
*/
template<typename _type>
struct is_allocator : details::is_allocator<_type>
{};


/*!
\ingroup binary_type_traits
\pre _tValue 是（可能不完整的）对象类型。
\brief 判断类型是指定值类型的分配器。
*/
template<typename _type, typename _tValue>
struct is_allocator_for : cond_or_t<is_allocator<_type>, false_,
	details::same_alloc_value_t, _type, _tValue>
{};


/*!
\ingroup unary_type_traits
\brief 判断类型是 byte 分配器。
*/
template<typename _type>
struct is_byte_allocator : is_allocator_for<_type, byte>
{};
//@}


/*!
\ingroup unary_type_traits
\brief 判断类型具有嵌套的成员 allocator_type 指称一个构造器类型。
\note 若嵌套的类型是满足具有 value_type 的可复制构造类类型，则视为构造器类型。
*/
template<typename _type>
struct has_nested_allocator
	: is_allocator<detected_t<details::nested_allocator_t, _type>>
{};


/*!
\ingroup metafunctions
\brief 取嵌套成员分配器类型，若不存在则使用第二模板参数指定的默认类型。
*/
template<typename _type, class _tDefault = std::allocator<_type>>
struct nested_allocator : cond<has_nested_allocator<_type>,
	detected_t<details::nested_allocator_t, _type>, _tDefault>
{};
//@}


/*!
\brief 启用 allocator_type 的基类。
\warning 非虚析构。
\since build 844
*/
//@{
template<class _tCon, bool = has_nested_allocator<_tCon>::value>
struct nested_allocator_base
{
	using allocator_type = typename _tCon::allocator_type;
};

template<class _tCon>
struct nested_allocator_base<_tCon, false>
{};
//@}


namespace details
{

//! \since build 846
template<class _tAlloc>
class allocator_delete_base
{
protected:
	using ator_traits = allocator_traits<remove_cvref_t<_tAlloc>>;
	using stored_ator_t
		= cond_t<is_lvalue_reference<_tAlloc>, lref<_tAlloc>, _tAlloc>;

	//! \since build 843
	mutable stored_ator_t alloc;

	template<class _tParam,
		yimpl(typename = exclude_self_t<allocator_delete_base, _tParam>)>
	allocator_delete_base(_tParam&& a) ynothrow
		: alloc(yforward(a))
	{}
	allocator_delete_base(const allocator_delete_base&) = default;
	allocator_delete_base(allocator_delete_base&&) = default;

	allocator_delete_base&
	operator=(const allocator_delete_base&) = default;
	allocator_delete_base&
	operator=(allocator_delete_base&&) = default;

	YB_ATTR_nodiscard YB_PURE _tAlloc&
	get_allocator() const
	{
		return alloc;
	}
};


//! \since build 843
//@{
template<typename _tAlloc>
yconstfn_relaxed void
do_alloc_on_copy(_tAlloc&, const _tAlloc&, false_) ynothrow
{}
template<typename _tAlloc>
yconstfn_relaxed void
do_alloc_on_copy(_tAlloc& x, const _tAlloc& y, true_) ynoexcept_spec(x = y)
{
	x = y;
}

template<typename _tAlloc>
yconstfn_relaxed void
do_alloc_on_move(_tAlloc&, _tAlloc&, false_) ynothrow
{}
/*!
\pre 参数指定的类型不在转移时抛出异常而退出。
\since build 865
*/
template<typename _tAlloc>
yconstfn_relaxed void
do_alloc_on_move(_tAlloc& x, _tAlloc& y, true_) ynothrow
{
	// NOTE: As per ISO C++17 [allocator.requirements], this shall exit via
	//	an exception.
	x = std::move(y);
}

template<typename _tAlloc>
yconstfn_relaxed void
do_alloc_on_swap(_tAlloc&, _tAlloc&, false_) ynothrow
{}
template<typename _tAlloc>
yconstfn_relaxed void
do_alloc_on_swap(_tAlloc& x, _tAlloc& y, true_)
	ynoexcept_spec(ystdex::swap_dependent(x, y))
{
	ystdex::swap_dependent(x, y);
}
//@}

} // namespace details;


/*!
\note 模板参数可能是引用。
\see $2018-12 @ %Documentation::Workflow.
*/
//@{
/*!
\brief 释放分配器分配的存储资源的删除器。
\since build 846
*/
template<class _tAlloc>
class allocator_guard_delete : private details::allocator_delete_base<_tAlloc>
{
private:
	//! \since build 846
	using base = details::allocator_delete_base<_tAlloc>;
	//! \since build 846
	using typename base::ator_traits;

public:
	//! \since build 595
	using pointer = typename ator_traits::pointer;
	//! \since build 595
	using size_type = typename ator_traits::size_type;

private:
	//! \since build 843
	size_type count;

public:
	//! \since build 843
	allocator_guard_delete(_tAlloc a, size_type n = 1) ynothrow
		: base(yforward(a)), count(n)
	{}
	allocator_guard_delete(const allocator_guard_delete&) = default;
	allocator_guard_delete(allocator_guard_delete&&) = default;

	allocator_guard_delete&
	operator=(const allocator_guard_delete&) = default;
	allocator_guard_delete&
	operator=(allocator_guard_delete&&) = default;

	//! \since build 595
	void
	operator()(pointer p) const ynothrowv
	{
		ator_traits::deallocate(base::alloc, p, count);
	}

	//! \since build 846
	yimpl(using) base::get_allocator;

	YB_ATTR_nodiscard YB_PURE size_type
	get_count() const
	{
		return count;
	}
};


/*!
\brief 释放分配器分配的对象的删除器。
\since build 595
*/
template<class _tAlloc>
class allocator_delete : private details::allocator_delete_base<_tAlloc>
{
private:
	//! \since build 846
	using base = details::allocator_delete_base<_tAlloc>;
	//! \since build 846
	using typename base::ator_traits;

public:
	//! \since build 595
	using pointer = typename ator_traits::pointer;

	//! \since build 847
	template<class _tParam,
		yimpl(typename = exclude_self_t<allocator_delete, _tParam>)>
	allocator_delete(_tParam&& a) ynothrow
		: base(yforward(a))
	{}
	//! \since build 843
	//@{
	allocator_delete(const allocator_delete&) = default;
	allocator_delete(allocator_delete&&) = default;

	allocator_delete&
	operator=(const allocator_delete&) = default;
	allocator_delete&
	operator=(allocator_delete&&) = default;
	//@}

	void
	operator()(pointer p) const ynothrowv
	{
		ator_traits::destroy(base::alloc, ystdex::to_address(p));
		ator_traits::deallocate(base::alloc, p, 1);
	}

	//! \since build 846
	yimpl(using) base::get_allocator;
};
//@}


/*!
\brief 分配器守卫。
\since build 843
*/
template<class _tAlloc>
using allocator_guard
	= std::unique_ptr<alloc_value_t<_tAlloc>, allocator_guard_delete<_tAlloc>>;


/*!
\brief 构造分配器守卫。
\since build 843
*/
template<class _tAlloc>
YB_ATTR_nodiscard inline allocator_guard<_tAlloc>
make_allocator_guard(_tAlloc& a,
	typename allocator_traits<_tAlloc>::size_type n = 1)
{
	return allocator_guard<_tAlloc>(allocator_traits<_tAlloc>::allocate(a, n),
		allocator_guard_delete<_tAlloc>(a, n));
}


/*!
\brief 使用分配器创建对象。
\return 分配的对象指针。
\since build 849
*/
template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard auto
create_with_allocator(_tAlloc&& a, _tParams&&... args)
	-> std::unique_ptr<_type, allocator_delete<remove_cvref_t<_tAlloc>>>
{
	using ator_t = remove_cvref_t<_tAlloc>;
	using ator_del_t = allocator_delete<ator_t>;
	auto gd(ystdex::make_allocator_guard(a));

	allocator_traits<ator_t>::construct(a, gd.get(), yforward(args)...);
	return std::unique_ptr<_type, ator_del_t>(gd.release(), ator_del_t(a));
}


/*!
\brief 使用分配器创建 std::unique_ptr 。
\see WG21 P0316R0 。
\since build 847
*/
template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard auto
allocate_unique(const _tAlloc& alloc, _tParams&&... args)
	-> std::unique_ptr<_type, allocator_delete<rebind_alloc_t<_tAlloc, _type>>>
{
	rebind_alloc_t<_tAlloc, _type> a(alloc);

	return ystdex::create_with_allocator<_type>(a, yforward(args)...);
}
/*!
\brief 使用分配器和初始化列表参数创建 std::unique_ptr 。
\since build 851
*/
template<typename _type, class _tAlloc, typename _tValue>
YB_ATTR_nodiscard auto
allocate_unique(const _tAlloc& alloc, std::initializer_list<_tValue> il)
	-> std::unique_ptr<_type, allocator_delete<rebind_alloc_t<_tAlloc, _type>>>
{
	rebind_alloc_t<_tAlloc, _type> a(alloc);

	return ystdex::create_with_allocator<_type>(a, il);
}

/*!
\brief 使用分配器创建 std::shared_ptr 。
\note 仅实现 WG21 P0674R1 和 ISO C++17 的交集，类似 libstdc++ 。
\see LWG 2070 。
\see https://developercommunity.visualstudio.com/content/problem/417142/lwg-2070p0674r1-stdallocate-shared-is-not-conformi.html
\since build 849
\todo 实现 WG21 P0674R1 。
\todo 在 ISO C++20 出版后更新 __cplusplus 的值。
*/
#if __cplusplus > 201703L || (__GLIBCXX__ && !(__GLIBCXX__ <= 20111108))
// NOTE: See https://gcc.gnu.org/viewcvs/gcc/trunk/libstdc++-v3/include/bits/shared_ptr_base.h?r1=181171&r2=181170&pathrev=181171.
// NOTE: LWG 2070 is not implemented by libc++ yet.
using std::allocate_shared;
#else
template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard std::shared_ptr<_type>
allocate_shared(const _tAlloc& alloc, _tParams&&... args)
{
	return std::shared_ptr<_type>(
		ystdex::allocate_unique<_type>(alloc, yforward(args)...));
}
#endif
/*!
\brief 使用分配器和初始化列表参数创建 std::shared_ptr 。
\since build 851
*/
template<typename _type, class _tAlloc, typename _tValue>
YB_ATTR_nodiscard std::shared_ptr<_type>
allocate_shared(const _tAlloc& alloc, std::initializer_list<_tValue> il)
{
	return std::shared_ptr<_type>(ystdex::allocate_unique<_type>(alloc, il));
}


/*!
\ingroup allocators
\since build 746
*/
//@{
/*!
\brief 类分配器。
\warning 非虚析构。

和 std::allocator 类似但自动检查类的成员代替 ::operator new/::operator delete 。
*/
template<class _type>
struct class_allocator : std::allocator<_type>
{
	class_allocator() = default;
	using std::allocator<_type>::allocator;
	class_allocator(const class_allocator&) = default;

	YB_ALLOCATOR _type*
	allocate(size_t n)
	{
		return _type::operator new(n * sizeof(_type));
	}

	//! \since build 778
	void
	deallocate(_type* p, size_t) ynothrow
	{
		// TODO: What if the size hint is supported by %_type?
		return _type::operator delete(p);
	}
};


/*!
\brief 局部分配器。

对支持类作用域分配的类类型为 class_allocator ，否则为 std::allocator 。
*/
template<typename _type>
using local_allocator = cond_t<and_<has_mem_new<_type, size_t>,
	has_mem_delete<_type*>>, class_allocator<_type>, std::allocator<_type>>;
//@}


//! \since build 830
//@{
//! \brief 按分配器特征在传播容器时复制赋值分配器。
template<typename _tAlloc>
yconstfn_relaxed void
alloc_on_copy(_tAlloc& x, const _tAlloc& y) ynothrow
{
	details::do_alloc_on_copy(x, y, typename allocator_traits<
		_tAlloc>::propagate_on_container_copy_assignment());
}
/*!
\brief 按分配器特征在传播容器时复制分配器。
\since build 830
*/
template<typename _tAlloc>
YB_ATTR_nodiscard yconstfn _tAlloc
alloc_on_copy(const _tAlloc& a)
{
	// NOTE: As per ISO C++17 [allocator.requirements]/4, this shall not throw
	// exceptions.
	return allocator_traits<_tAlloc>::select_on_container_copy_construction(a);
}

//! \brief 按分配器特征在传播容器时转移分配器。
template<typename _tAlloc>
yconstfn_relaxed void
alloc_on_move(_tAlloc& x, _tAlloc& y) ynothrow
{
	// NOTE: As per ISO C++17 [allocator.requirements]/4, this shall not throw
	// exceptions. Even without this requirement, it still shall not exit via an
	//	exception. See %details::do_alloc_on_move for the rationale.
	details::do_alloc_on_move(x, y, typename
		allocator_traits<_tAlloc>::propagate_on_container_move_assignment());
}

//! \brief 按分配器特征在传播容器时交换分配器。
template<typename _tAlloc>
yconstfn_relaxed void
alloc_on_swap(_tAlloc& x, _tAlloc& y) ynothrow
{
	// NOTE: As per ISO C++17 [allocator.requirements]/4, this shall not throw
	// exceptions.
	details::do_alloc_on_swap(x, y,
		typename allocator_traits<_tAlloc>::propagate_on_container_swap());
}
//@}


namespace details
{

//! \since build 847
//@{
template<typename _type, typename... _tParams>
YB_NORETURN inline _type
create_or_throw_impl(void*, _tParams&&...)
{
	throw_invalid_construction();
}
//! \throw ystdex::invalid_construction 参数类型无法用于初始化持有者。
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard inline auto
create_or_throw_impl(nullptr_t, _tParams&&... args)
	-> decltype(_type(yforward(args)...))
{
	return _type(yforward(args)...);
}

template<typename _type, class _tAlloc, typename... _tParams>
YB_NORETURN _type*
create_or_throw_with_allocator_impl(void*, _tAlloc&, _tParams&&...)
{
	throw_invalid_construction();
}
template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard inline auto
create_or_throw_with_allocator_impl(nullptr_t, _tAlloc& a, _tParams&&... args)
	-> decltype(ystdex::create_with_allocator<_type>(a, yforward(args)...))
{
	return ystdex::create_with_allocator<_type>(a, yforward(args)...);
}

template<typename _type, typename... _tParams>
YB_NORETURN inline _type*
new_or_throw_impl(void*, _tParams&&...)
{
	throw_invalid_construction();
}
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard inline auto
new_or_throw_impl(nullptr_t,  _tParams&&... args)
	-> decltype(new _type(yforward(args)...))
{
	return new _type(yforward(args)...);
}
//@}

//! \since build 617
//@{
template<typename _type>
struct pack_obj_impl
{
	template<typename... _tParams>
	YB_ATTR_nodiscard static _type
	pack(_tParams&&... args)
	{
		return _type(yforward(args)...);
	}
};

template<typename _type, class _tDeleter>
struct pack_obj_impl<std::unique_ptr<_type, _tDeleter>>
{
	template<typename... _tParams>
	YB_ATTR_nodiscard static std::unique_ptr<_type>
	pack(_tParams&&... args)
	{
		return std::unique_ptr<_type>(yforward(args)...);
	}
};

template<typename _type>
struct pack_obj_impl<std::shared_ptr<_type>>
{
	template<typename... _tParams>
	YB_ATTR_nodiscard static std::shared_ptr<_type>
	pack(_tParams&&... args)
	{
		return std::shared_ptr<_type>(yforward(args)...);
	}
};
//@}

} // namespace details;


/*!
\throw invalid_construction 初始化非合式。
\note 第一模板参数决定创建对象的类型。
\since build 847
*/
//@{
/*!
\brief 调用构造函数创建对象或抛出异常。
\return 创建的对象。
\throw ystdex::invalid_construction 参数类型无法用于初始化持有者。
*/
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard inline _type
create_or_throw(_tParams&&... args)
{
	return details::create_or_throw_impl<_type>(nullptr, yforward(args)...);
}

//! \brief 调用 ystdex::create_with_allocator 表达式创建对象或抛出异常。
template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard _type*
create_or_throw_with_allocator(_tAlloc&& a, _tParams&&... args)
{
	return details::create_or_throw_with_allocator_impl<_type>(nullptr, a,
		yforward(args)...);
}

//! \brief 调用 new 表达式创建对象或抛出异常。
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard _type*
new_or_throw(_tParams&&... args)
{
	return details::new_or_throw_impl<_type>(nullptr, yforward(args)...);
}
//@}


/*!
\ingroup unary_type_traits
\brief 判断指定类型是否为已知的持有共享所有权的对象类型。
\tparam _type 需要判断特征的类型参数。
\note 用户可继承此特征再特化派生的特征扩展新的类型。
\since build 784
*/
//@{
template<typename _type>
struct is_sharing : false_
{};

template<typename _type>
struct is_sharing<std::shared_ptr<_type>> : true_
{};

template<typename _type>
struct is_sharing<std::weak_ptr<_type>> : true_
{};
//@}


/*!
\brief 使用显式析构函数调用和 std::free 的删除器。
\note 数组类型的特化无定义。
\warning 非虚析构。
\since build 561

除使用 std::free 代替 \c ::operator delete，和 std::default_deleter
的非数组类型相同。注意和直接使用 std::free 不同，会调用析构函数且不适用于数组。
*/
//@{
template<typename _type>
struct free_delete
{
	yconstfn free_delete() ynothrow = default;
	template<typename _type2,
		yimpl(typename = enable_if_convertible_t<_type2*, _type*>)>
	free_delete(const free_delete<_type2>&) ynothrow
	{}

	//! \since build 595
	void
	operator()(_type* p) const ynothrowv
	{
		p->~_type();
		std::free(p);
	}
};

template<typename _type>
struct free_delete<_type[]>
{
	//! \since build 634
	//@{
	static_assert(is_trivially_destructible<_type>(), "Invalid type found");

	yconstfn free_delete() ynothrow = default;
	template<typename _type2,
		yimpl(typename = enable_if_convertible_t<_type2(*)[], _type(*)[]>)>
	free_delete(const free_delete<_type2[]>&) ynothrow
	{}

	template<typename _type2,
		yimpl(typename = enable_if_convertible_t<_type2(*)[], _type(*)[]>)>
	void
	operator()(_type2* p) const ynothrowv
	{
		std::free(p);
	}
	//@}
};
//@}


/*!
\since build 834
\see ISO C++17 [depr.temporary.buffer] 。
\see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0174r2.html#3.2 。
*/
//@{
namespace details
{

#if __cpp_aligned_new >= 201606L
#	define YB_Impl_aligned_new true
#endif
template<typename _type>
using over_aligned_t
#if YB_Impl_aligned_new
	= bool_<(yalignof(_type) > __STDCPP_DEFAULT_NEW_ALIGNMENT__)>;
#else
	= false_;
#endif

YB_ALLOCATOR inline void*
new_aligned(size_t size, size_t, false_) ynothrow
{
	return ::operator new(size, std::nothrow);
}
#if YB_Impl_aligned_new
YB_ALLOCATOR inline void*
new_aligned(size_t size, size_t alignment, true_) ynothrow
{
	return ::operator new(size, std::align_val_t(alignment), std::nothrow);
}
#endif

inline void
delete_aligned(void* ptr, size_t, false_) ynothrow
{
	// NOTE: No %std::nothrow is needed here. See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=86954.
	::operator delete(ptr);
}
#if YB_Impl_aligned_new
inline void
delete_aligned(void* ptr, size_t alignment, true_) ynothrow
{
	::operator delete(ptr, std::align_val_t(alignment));
}
#endif
#undef YB_Impl_aligned_new

} // namespace details;


#if (YB_IMPL_MSCPP >= 1912 && _MSVC_LANG >= 201606) || (__GLIBCXX__ \
	&& (__GLIBCXX__ <= 20150815 || YB_IMPL_GNUCPP < 90000)) \
	|| __cplusplus >= 201611L
template<typename _type>
YB_ATTR_nodiscard inline std::pair<_type*, ptrdiff_t>
get_temporary_buffer(ptrdiff_t n) ynothrow
{
	const ptrdiff_t m(std::numeric_limits<ptrdiff_t>::max() / sizeof(_type));

	// NOTE: Like libstdc++ and libc++ but not Microsoft VC++ 2017, this does
	//	not fail fast.
	if(n > m)
		n = m;
	while(n > 0)
	{
		if(const auto p = details::new_aligned(static_cast<size_t>(n)
			* sizeof(_type), yalignof(_type), details::over_aligned_t<_type>()))
			return {static_cast<_type*>(p), n};
		n /= 2;
	}
	return {nullptr, 0};
}

template<typename _type>
inline void
return_temporary_buffer(_type* p) ynothrow
{
	details::delete_aligned(p, yalignof(_type),
		details::over_aligned_t<_type>());
}
#else
using std::get_temporary_buffer;
using std::return_temporary_buffer;
#endif
//@}


/*!
\brief 使用显式 ystdex::return_temporary_buffer 调用释放存储的删除器。
\note 非数组类型的特化无定义。
\warning 非虚析构。
\since build 627
*/
//@{
template<typename>
struct temporary_buffer_delete;

template<typename _type>
struct temporary_buffer_delete<_type[]>
{
	yconstfn temporary_buffer_delete() ynothrow = default;

	void
	operator()(_type* p) const ynothrowv
	{
		ystdex::return_temporary_buffer(p);
	}
};
//@}


/*!
\brief 临时缓冲区。
\warning 非虚析构。
\since build 627
*/
//@{
template<typename _type>
class temporary_buffer
{
public:
	using array_type = _type[];
	using element_type = _type;
	using deleter = temporary_buffer_delete<array_type>;
	using pointer = std::unique_ptr<_type, deleter>;

private:
	std::pair<pointer, size_t> buf;

public:
	//! \throw std::bad_cast 取临时存储失败。
	temporary_buffer(size_t n)
		: buf([n]{
			// NOTE: See LWG 2072.
			const auto pr(ystdex::get_temporary_buffer<_type>(ptrdiff_t(n)));

			if(pr.first)
				return std::pair<pointer, size_t>(pointer(pr.first),
					size_t(pr.second));
			throw std::bad_cast();
		}())
	{}
	temporary_buffer(temporary_buffer&&) = default;
	temporary_buffer&
	operator=(temporary_buffer&&) = default;

	//! \since build 634
	YB_ATTR_nodiscard yconstfn const pointer&
	get() const ynothrow
	{
		return buf.first;
	}

	YB_ATTR_nodiscard YB_PURE pointer&
	get_pointer_ref() ynothrow
	{
		return buf.first;
	}

	YB_ATTR_nodiscard YB_PURE size_t
	size() const ynothrow
	{
		return buf.second;
	}
};
//@}


/*!
\ingroup metafunctions
\since build 671
*/
//@{
//! \brief 取删除器的 \c pointer 成员类型。
template<class _tDeleter>
using deleter_member_pointer_t
	= typename remove_reference_t<_tDeleter>::pointer;

//! \brief 取 unique_ptr 包装的指针类型。
template<typename _type, class _tDeleter>
using unique_ptr_pointer
	= detected_or_t<_type*, deleter_member_pointer_t, _tDeleter>;

/*!
\brief 取指针对应的元素类型。

当指针为空指针时为 \c void ，否则间接操作的元素类型。
*/
template<typename _tPointer, typename _tDefault = void>
using defer_element = cond_or<not_<is_void<remove_pointer_t<_tPointer>>>,
	_tDefault, indirect_element_t, _tPointer>;
//@}


/*!
\ingroup is_undereferenceable
\brief 判断智能指针实例是否不可解引用。
\since build 857
*/
//@{
template<typename _type, typename _fDeleter>
inline bool
is_undereferenceable(const std::unique_ptr<_type, _fDeleter>& p) ynothrow
{
	return !p;
}
template<typename _type>
inline bool
is_undereferenceable(const std::shared_ptr<_type>& p) ynothrow
{
	return !p;
}
template<typename _type>
inline bool
is_undereferenceable(const std::weak_ptr<_type>& p) ynothrow
{
	// XXX: Although conforming, this is actually not sufficient. There can be
	//	some false negative results when the pointer is null.
	return !p.expired();
}
//@}

/*!	\defgroup get_raw Get Raw Pointers
\brief 取内建指针。
\since build 422
*/
//@{
template<typename _type>
YB_ATTR_nodiscard YB_STATELESS yconstfn _type*
get_raw(_type* const& p) ynothrow
{
	return p;
}
//! \since build 550
template<typename _type, typename _fDeleter>
YB_ATTR_nodiscard YB_PURE yconstfn auto
get_raw(const std::unique_ptr<_type, _fDeleter>& p) ynothrow
	-> decltype(p.get())
{
	return p.get();
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE yconstfn _type*
get_raw(const std::shared_ptr<_type>& p) ynothrow
{
	return p.get();
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE yconstfn _type*
get_raw(const std::weak_ptr<_type>& p) ynothrow
{
	return p.lock().get();
}
//@}

/*!	\defgroup owns_any Owns Any Check
\brief 检查是否是所有者。
\since build 784
*/
//@{
template<typename _type>
YB_ATTR_nodiscard YB_STATELESS yconstfn bool
owns_any(_type* const&) ynothrow
{
	return {};
}
//! \since build 550
template<typename _type, typename _fDeleter>
YB_ATTR_nodiscard YB_PURE yconstfn bool
owns_any(const std::unique_ptr<_type, _fDeleter>& p) ynothrow
{
	return bool(p);
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE yconstfn bool
owns_any(const std::shared_ptr<_type>& p) ynothrow
{
	return p.use_count() > 0;
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE yconstfn bool
owns_any(const std::weak_ptr<_type>& p) ynothrow
{
	return !p.expired();
}
//@}

/*!	\defgroup owns_nonnull Owns Nonnull Check
\brief 检查是否是非空对象的所有者。
\since build 784
*/
//@{
template<typename _type>
YB_ATTR_nodiscard YB_STATELESS yconstfn bool
owns_nonnull(_type* const&) ynothrow
{
	return {};
}
//! \since build 550
template<typename _type, typename _fDeleter>
YB_ATTR_nodiscard YB_PURE yconstfn bool
owns_nonnull(const std::unique_ptr<_type, _fDeleter>& p) ynothrow
{
	return bool(p);
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE yconstfn bool
owns_nonnull(const std::shared_ptr<_type>& p) ynothrow
{
	return bool(p);
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE yconstfn bool
owns_nonnull(const std::weak_ptr<_type>& p) ynothrow
{
	return bool(p.lock());
}
//@}

/*!	\defgroup owns_unique Owns Uniquely Check
\brief 检查是否是唯一所有者。
\since build 759
*/
//@{
template<typename _type>
YB_ATTR_nodiscard YB_STATELESS yconstfn bool
owns_unique(const _type&) ynothrow
{
	return !is_reference_wrapper<_type>();
}
template<typename _type, class _tDeleter>
YB_ATTR_nodiscard inline bool
owns_unique(const std::unique_ptr<_type, _tDeleter>& p) ynothrow
{
	return bool(p);
}
//! \note 返回 true 不一定表示指针非空。
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline bool
owns_unique(const std::shared_ptr<_type>& p) ynothrow
{
	return p.unique();
}
//! \since build 784
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline bool
owns_unique(const std::weak_ptr<_type>& p) ynothrow
{
	return p.use_count() == 1;
}

template<typename _type>
YB_ATTR_nodiscard YB_STATELESS yconstfn bool
owns_unique_nonnull(const _type&) ynothrow
{
	return !is_reference_wrapper<_type>();
}
//! \pre 参数非空。
//@{
template<typename _type, class _tDeleter>
YB_ATTR_nodiscard YB_PURE inline bool
owns_unique_nonnull(const std::unique_ptr<_type, _tDeleter>& p) ynothrow
{
	yconstraint(p);
	return true;
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline bool
owns_unique_nonnull(const std::shared_ptr<_type>& p) ynothrow
{
	yconstraint(p);
	return p.unique();
}
//! \since build 784
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline bool
owns_unique_nonnull(const std::weak_ptr<_type>& p) ynothrow
{
	yconstraint(!p.expired());
	return p.use_count() == 1;
}
//@}
//@}

/*!	\defgroup reset Reset Pointers
\brief 安全删除指定引用的指针指向的对象。
\post 指定引用的指针为空。
\since build 209
*/
//@{
//! \since build 759
template<typename _type, class _tDeleter>
inline bool
reset(std::unique_ptr<_type, _tDeleter>& p) ynothrow
{
	if(p.get())
	{
		p.reset();
		return true;
	}
	return {};
}
template<typename _type>
inline bool
reset(std::shared_ptr<_type>& p) ynothrow
{
	if(p.get())
	{
		p.reset();
		return true;
	}
	return {};
}
//@}

/*!	\defgroup unique_raw Get Unique Pointer
\ingroup helper_functions
\brief 使用指定类型指针构造 std::unique_ptr 实例。
\tparam _type 被指向类型。
\note 不检查指针是否有效。
\since build 578
*/
//@{
template<typename _type, typename _pSrc>
YB_ATTR_nodiscard yconstfn std::unique_ptr<_type>
unique_raw(_pSrc* p) ynothrow
{
	return std::unique_ptr<_type>(p);
}
template<typename _type>
YB_ATTR_nodiscard yconstfn std::unique_ptr<_type>
unique_raw(_type* p) ynothrow
{
	return std::unique_ptr<_type>(p);
}
template<typename _type, typename _tDeleter, typename _pSrc>
YB_ATTR_nodiscard yconstfn std::unique_ptr<_type, _tDeleter>
unique_raw(_pSrc* p, _tDeleter&& d) ynothrow
{
	return std::unique_ptr<_type, _tDeleter>(p, yforward(d));
}
template<typename _type, typename _tDeleter>
YB_ATTR_nodiscard yconstfn std::unique_ptr<_type, _tDeleter>
unique_raw(_type* p, _tDeleter&& d) ynothrow
{
	return std::unique_ptr<_type, _tDeleter>(p, yforward(d));
}
/*!
\note 使用空指针构造空实例。
\since build 319
*/
template<typename _type>
YB_ATTR_nodiscard yconstfn std::unique_ptr<_type>
unique_raw(nullptr_t) ynothrow
{
	return std::unique_ptr<_type>();
}
//@}


/*!	\defgroup share_raw Get Shared Pointer
\ingroup helper_functions
\brief 使用指定类型指针构造 std::shared_ptr 实例。
\tparam _type 被指向类型。
\note 不检查指针是否有效。
\since build 204
*/
//@{
//! \since build 627
//@{
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard yconstfn std::shared_ptr<_type>
share_raw(_type* p, _tParams&&... args)
{
	return std::shared_ptr<_type>(p, yforward(args)...);
}
/*!
\tparam _pSrc 指定指针类型。
\pre 静态断言： remove_reference_t<_pSrc> 是内建指针。
*/
template<typename _type, typename _pSrc, typename... _tParams>
YB_ATTR_nodiscard yconstfn std::shared_ptr<_type>
share_raw(_pSrc&& p, _tParams&&... args)
{
	static_assert(is_pointer<remove_reference_t<_pSrc>>(),
		"Invalid type found.");

	return std::shared_ptr<_type>(p, yforward(args)...);
}
//@}
/*!
\note 使用空指针构造空对象。
\since build 319
*/
template<typename _type>
YB_ATTR_nodiscard yconstfn std::shared_ptr<_type>
share_raw(nullptr_t) ynothrow
{
	return std::shared_ptr<_type>();
}
/*!
\note 使用空指针和其它参数构造空对象。
\pre 参数复制构造不抛出异常。
\since build 627
*/
template<typename _type, class... _tParams>
YB_ATTR_nodiscard yconstfn
	yimpl(enable_if_t)<sizeof...(_tParams) != 0, std::shared_ptr<_type>>
share_raw(nullptr_t, _tParams&&... args) ynothrow
{
	return std::shared_ptr<_type>(nullptr, yforward(args)...);
}
//@}


inline namespace cpp2014
{

#if __cpp_lib_make_unique >= 201304L
//! \since build 617
using std::make_unique;
#else
/*!
\ingroup helper_functions
\brief 使用 new 和指定参数构造指定类型的 std::unique_ptr 对象。
\tparam _type 被指向类型。
*/
//@{
/*!
\note 使用值初始化。
\see http://herbsutter.com/gotw/_102/ 。
\see WG21 N3656 。
\see WG21 N3797 20.7.2[memory.syn] 。
\since build 476
*/
//@{
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard yconstfn
	yimpl(enable_if_t<!is_array<_type>::value, std::unique_ptr<_type>>)
make_unique(_tParams&&... args)
{
	return std::unique_ptr<_type>(new _type(yforward(args)...));
}
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard yconstfn yimpl(enable_if_t<is_array<_type>::value
	&& extent<_type>::value == 0, std::unique_ptr<_type>>)
make_unique(size_t size)
{
	return std::unique_ptr<_type>(new remove_extent_t<_type>[size]());
}
template<typename _type,  typename... _tParams>
yimpl(enable_if_t<extent<_type>::value != 0>)
make_unique(_tParams&&...) = delete;
//@}
#endif

} // inline namespace cpp2014;

/*!
\note 使用默认初始化。
\see WG21 N3588 A4 。
\since build 526
*/
//@{
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard yconstfn
	yimpl(enable_if_t<!is_array<_type>::value, std::unique_ptr<_type>>)
make_unique_default_init()
{
	return std::unique_ptr<_type>(new _type);
}
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard yconstfn yimpl(enable_if_t<is_array<_type>::value
	&& extent<_type>::value == 0, std::unique_ptr<_type>>)
make_unique_default_init(size_t size)
{
	return std::unique_ptr<_type>(new remove_extent_t<_type>[size]);
}
template<typename _type,  typename... _tParams>
yimpl(enable_if_t<extent<_type>::value != 0>)
make_unique_default_init(_tParams&&...) = delete;
//@}
/*!
\ingroup helper_functions
\brief 使用指定类型的初始化列表构造指定类型的 std::unique_ptr 对象。
\tparam _type 被指向类型。
\tparam _tElem 初始化列表的元素类型。
\since build 574
*/
template<typename _type, typename _tElem>
YB_ATTR_nodiscard yconstfn std::unique_ptr<_type>
make_unique(std::initializer_list<_tElem> il)
{
	return ystdex::cpp2014::make_unique<_type>(il);
}

/*!
\note 使用删除器。
\since build 562
*/
//@{
//! \since build 843
template<typename _type, typename _tDeleter, typename... _tParams>
YB_ATTR_nodiscard yconstfn yimpl(enable_if_t<!is_array<_type>::value,
	std::unique_ptr<_type, _tDeleter>>)
make_unique_with(_tDeleter&& d, _tParams&&... args)
{
	return std::unique_ptr<_type, _tDeleter>(new _type(yforward(args)...),
		yforward(d));
}
template<typename _type, typename _tDeleter, typename... _tParams>
YB_ATTR_nodiscard yconstfn yimpl(enable_if_t<is_array<_type>::value
	&& extent<_type>::value == 0, std::unique_ptr<_type>>)
make_unique_with(_tDeleter&& d, size_t size)
{
	return std::unique_ptr<_type, _tDeleter>(new remove_extent_t<_type>[size],
		yforward(d));
}
template<typename _type, typename _tDeleter, typename... _tParams>
yimpl(enable_if_t<is_array<_type>::value && extent<_type>::value != 0>)
make_unique_with(_tDeleter&&, _tParams&&...) = delete;
//@}
//@}

//! \ingroup helper_functions
//@{
//! \tparam _type 被指向类型。
//@{
//! \since build 851
using std::make_shared;
/*!
\ingroup YBase_replacement_extensions
\brief 使用指定类型的初始化列表构造指定类型的 std::shared_ptr 对象。
\tparam _tValue 初始化列表的元素类型。
\since build 529
*/
template<typename _type, typename _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<_type>
make_shared(std::initializer_list<_tValue> il)
{
	return std::make_shared<_type>(il);
}

/*!
\brief 使用指定的 std::shared_ptr 实例的对象创建对应的 std::weak_ptr 实例的对象。
\since build 779
*/
template<typename _type>
YB_ATTR_nodiscard inline std::weak_ptr<_type>
make_weak(const std::shared_ptr<_type>& p) ynothrow
{
	return p;
}
//@}


//! \since build 783
//@{
//! \brief 复制值创建对应的 std::shared_ptr 实例的对象。
template<typename _tValue, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<decay_t<_type>>
share_copy(const _tValue& v)
{
	return ystdex::make_shared<decay_t<_type>>(v);
}
//! \since build 851
//@{
//! \brief 复制初始化列表创建对应的 std::shared_ptr 实例的对象。
template<typename _tValue, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<decay_t<_type>>
share_copy(std::initializer_list<_tValue> il)
{
	return ystdex::make_shared<decay_t<_type>>(il);
}
//! \brief 复制初始化值以分配器创建对应的 std::shared_ptr 实例的对象。
template<typename _tValue, class _tAlloc, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<decay_t<_type>>
share_copy(const _tAlloc& a, const _tValue& v)
{
	return ystdex::allocate_shared<decay_t<_type>>(a, v);
}
//! \brief 复制初始化列表以分配器创建对应的 std::shared_ptr 实例的对象。
template<typename _tValue, class _tAlloc, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<decay_t<_type>>
share_copy(const _tAlloc& a, std::initializer_list<_tValue> il)
{
	return ystdex::allocate_shared<decay_t<_type>>(a, il);
}
//@}

//! \brief 传递值创建对应的 std::shared_ptr 实例的对象。
template<typename _tValue, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<decay_t<_type>>
share_forward(_tValue&& v)
{
	return ystdex::make_shared<decay_t<_type>>(yforward(v));
}
//! \since build 851
//@{
//! \brief 传递初始化列表创建对应的 std::shared_ptr 实例的对象。
template<typename _tValue, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<decay_t<_type>>
share_forward(std::initializer_list<_tValue> il)
{
	return ystdex::make_shared<decay_t<_type>>(il);
}
//! \brief 传递值以分配器创建对应的 std::shared_ptr 实例的对象。
template<typename _tValue, class _tAlloc, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<decay_t<_type>>
share_forward(const _tAlloc& a, _tValue&& v)
{
	return ystdex::allocate_shared<decay_t<_type>>(a, yforward(v));
}
//! \brief 传递初始化列表以分配器创建对应的 std::shared_ptr 实例的对象。
template<typename _tValue, class _tAlloc, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<decay_t<_type>>
share_forward(const _tAlloc& a, std::initializer_list<_tValue> il)
{
	return ystdex::allocate_shared<decay_t<_type>>(a, il);
}
//@}

//! \brief 转移值创建对应的 std::shared_ptr 实例的对象。
template<typename _tValue, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<decay_t<_type>>
share_move(_tValue&& v)
{
	return ystdex::make_shared<decay_t<_type>>(std::move(v));
}
//! \since build 851
//@{
//! \brief 转移初始化列表创建对应的 std::shared_ptr 实例的对象。
template<typename _tValue, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<decay_t<_type>>
share_move(std::initializer_list<_tValue> il)
{
	return ystdex::make_shared<decay_t<_type>>(il);
}
//! \brief 转移值以分配器创建对应的 std::shared_ptr 实例的对象。
template<typename _tValue, class _tAlloc, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<decay_t<_type>>
share_move(const _tAlloc& a, _tValue&& v)
{
	return ystdex::allocate_shared<decay_t<_type>>(a, std::move(v));
}
//! \brief 转移初始化列表以分配器创建对应的 std::shared_ptr 实例的对象。
template<typename _tValue, class _tAlloc, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::shared_ptr<decay_t<_type>>
share_move(const _tAlloc& a, std::initializer_list<_tValue> il)
{
	return ystdex::allocate_shared<decay_t<_type>>(a, il);
}
//@}
//@}

//! \since build 841
//@{
//! \brief 复制值创建对应的 std::unique_ptr 实例的对象。
template<typename _tValue, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::unique_ptr<decay_t<_type>>
unique_copy(const _tValue& v)
{
	return ystdex::make_unique<decay_t<_type>>(v);
}
//! \since build 851
//@{
template<typename _tValue, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::unique_ptr<decay_t<_type>>
unique_copy(std::initializer_list<_tValue> il)
{
	return ystdex::make_unique<decay_t<_type>>(il);
}
template<typename _tValue, class _tAlloc, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::unique_ptr<decay_t<_type>>
unique_copy(const _tAlloc& a, const _tValue& v)
{
	return ystdex::allocate_unique<decay_t<_type>>(a, v);
}
template<typename _tValue, class _tAlloc, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::unique_ptr<decay_t<_type>>
unique_copy(const _tAlloc& a, std::initializer_list<_tValue> il)
{
	return ystdex::allocate_unique<decay_t<_type>>(a, il);
}
//@}

//! \brief 传递值创建对应的 std::unique_ptr 实例的对象。
template<typename _tValue, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::unique_ptr<decay_t<_type>>
unique_forward(_tValue&& v)
{
	return ystdex::make_unique<decay_t<_type>>(yforward(v));
}
//! \since build 851
//@{
template<typename _tValue, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::unique_ptr<decay_t<_type>>
unique_forward(std::initializer_list<_tValue> il)
{
	return ystdex::make_unique<decay_t<_type>>(il);
}
template<typename _tValue, class _tAlloc, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::unique_ptr<decay_t<_type>>
unique_forward(const _tAlloc& a, _tValue&& v)
{
	return ystdex::allocate_unique<decay_t<_type>>(a, yforward(v));
}
template<typename _tValue, class _tAlloc, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::unique_ptr<decay_t<_type>>
unique_forward(const _tAlloc& a, std::initializer_list<_tValue> il)
{
	return ystdex::allocate_unique<decay_t<_type>>(a, il);
}
//@}

//! \brief 转移值创建对应的 std::unique_ptr 实例的对象。
template<typename _tValue, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::unique_ptr<decay_t<_type>>
unique_move(_tValue&& v)
{
	return ystdex::make_unique<decay_t<_type>>(std::move(v));
}
//! \since build 851
//@{
template<typename _tValue, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::unique_ptr<decay_t<_type>>
unique_move(std::initializer_list<_tValue> il)
{
	return ystdex::make_unique<decay_t<_type>>(il);
}
template<typename _tValue, class _tAlloc, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::unique_ptr<decay_t<_type>>
unique_move(const _tAlloc& a, _tValue&& v)
{
	return ystdex::allocate_unique<decay_t<_type>>(a, std::move(v));
}
template<typename _tValue, class _tAlloc, typename _type = _tValue>
YB_ATTR_nodiscard yconstfn std::unique_ptr<decay_t<_type>>
unique_move(const _tAlloc& a, std::initializer_list<_tValue> il)
{
	return ystdex::allocate_unique<decay_t<_type>>(a, il);
}
//@}
//@}
//@}


/*!
\brief 智能指针转换。
\since build 595
*/
//@{
template<typename _tDst, typename _type>
YB_ATTR_nodiscard YB_PURE inline std::unique_ptr<_tDst>
static_pointer_cast(std::unique_ptr<_type> p) ynothrow
{
	return std::unique_ptr<_tDst>(static_cast<_tDst*>(p.release()));
}
template<typename _tDst, typename _type, typename _tDeleter>
YB_ATTR_nodiscard YB_PURE inline std::unique_ptr<_tDst, _tDeleter>
static_pointer_cast(std::unique_ptr<_type, _tDeleter> p) ynothrow
{
	return std::unique_ptr<_tDst, _tDeleter>(static_cast<_tDst*>(p.release()),
		std::move(p.get_deleter()));
}

template<typename _tDst, typename _type>
YB_ATTR_nodiscard YB_PURE std::unique_ptr<_tDst>
dynamic_pointer_cast(std::unique_ptr<_type>& p) ynothrow
{
	if(auto p_res = dynamic_cast<_tDst*>(p.get()))
	{
		p.release();
		return std::unique_ptr<_tDst>(p_res);
	}
	return std::unique_ptr<_tDst>();
}
template<typename _tDst, typename _type>
YB_ATTR_nodiscard YB_PURE inline std::unique_ptr<_tDst>
dynamic_pointer_cast(std::unique_ptr<_type>&& p) ynothrow
{
	return ystdex::dynamic_pointer_cast<_tDst, _type>(p);
}
template<typename _tDst, typename _type, typename _tDeleter>
YB_ATTR_nodiscard YB_PURE std::unique_ptr<_tDst, _tDeleter>
dynamic_pointer_cast(std::unique_ptr<_type, _tDeleter>& p) ynothrow
{
	if(auto p_res = dynamic_cast<_tDst*>(p.get()))
	{
		p.release();
		return std::unique_ptr<_tDst, _tDeleter>(p_res);
	}
	return std::unique_ptr<_tDst, _tDeleter>(nullptr);
}
template<typename _tDst, typename _type, typename _tDeleter>
YB_ATTR_nodiscard YB_PURE inline std::unique_ptr<_tDst, _tDeleter>
dynamic_pointer_cast(std::unique_ptr<_type, _tDeleter>&& p) ynothrow
{
	return ystdex::dynamic_pointer_cast<_tDst, _type, _tDeleter>(p);
}

template<typename _tDst, typename _type>
YB_ATTR_nodiscard std::unique_ptr<_tDst>
const_pointer_cast(std::unique_ptr<_type> p) ynothrow
{
	return std::unique_ptr<_tDst>(const_cast<_tDst*>(p.release()));
}
template<typename _tDst, typename _type, typename _tDeleter>
YB_ATTR_nodiscard std::unique_ptr<_tDst, _tDeleter>
const_pointer_cast(std::unique_ptr<_type, _tDeleter> p) ynothrow
{
	return std::unique_ptr<_tDst, _tDeleter>(const_cast<_tDst*>(p.release()),
		std::move(p.get_deleter()));
}
//@}


//! \since build 746
//@{
//! \brief 使用 new 复制对象。
template<typename _type>
YB_ATTR_nodiscard inline _type*
clone_monomorphic(const _type& v)
{
	return new _type(v);
}
//! \brief 使用分配器复制对象。
template<typename _type, class _tAlloc>
YB_ATTR_nodiscard inline auto
clone_monomorphic(const _type& v, _tAlloc&& a)
	-> decltype(ystdex::create_with_allocator<_type>(yforward(a), v))
{
	return ystdex::create_with_allocator<_type>(yforward(a), v);
}

//! \brief 若指针非空，使用 new 复制指针指向的对象。
template<typename _tPointer>
YB_ATTR_nodiscard inline auto
clone_monomorphic_ptr(const _tPointer& p)
	-> decltype(ystdex::clone_monomorphic(*p))
{
	using ptr_t = decltype(ystdex::clone_monomorphic(*p));

	return p ? ystdex::clone_monomorphic(*p) : ptr_t();
}
//! \brief 若指针非空，使用分配器复制指针指向的对象。
template<typename _tPointer, class _tAlloc>
YB_ATTR_nodiscard inline auto
clone_monomorphic_ptr(const _tPointer& p, _tAlloc&& a)
	-> decltype(ystdex::clone_monomorphic(*p, yforward(a)))
{
	using ptr_t = decltype(ystdex::clone_monomorphic(*p, yforward(a)));

	return p ? ystdex::clone_monomorphic(*p, yforward(a)) : ptr_t();
}

/*!
\brief 使用 \c clone 成员函数复制多态类类型对象。
\pre 静态断言： <tt>is_polymorphic<decltype(v)>()</tt> 。
*/
template<class _type>
YB_ATTR_nodiscard inline auto
clone_polymorphic(const _type& v) -> decltype(v.clone())
{
	static_assert(is_polymorphic<_type>(), "Non-polymorphic class type found.");

	return v.clone();
}

//! \brief 若指针非空，使用 \c clone 成员函数复制指针指向的多态类类型对象。
template<class _tPointer>
YB_ATTR_nodiscard inline auto
clone_polymorphic_ptr(const _tPointer& p) -> decltype(clone_polymorphic(*p))
{
	return
		p ? ystdex::clone_polymorphic(*p) : decltype(clone_polymorphic(*p))();
}
//@}


#if __cplusplus > 201703L && (!defined(__GLIBCXX__) || __GLIBCXX__ > 20190731)
// XXX: See https://github.com/cplusplus/draft/issues/3111.
// XXX: See https://gcc.gnu.org/viewcvs/gcc/trunk/libstdc++-v3/include/std/memory?r1=273515&r2=273945.
//! \since build 863
//@{
using std::uses_allocator_construction_args;
using std::make_obj_using_allocator;
using std::uninitialized_construct_using_allocator;
//@}
#else
/*!
\see WG21 P0591R4 。
\since build 850
*/
//@{
namespace details
{

template<typename _type>
struct is_pair : false_
{};

template<typename _type1, typename _type2>
struct is_pair<std::pair<_type1, _type2>> : true_
{};


template<typename _type, class _tAlloc>
struct pair_has_allocator : std::uses_allocator<_type, _tAlloc>
{};

template<typename _type1, typename _type2, class _tAlloc>
struct pair_has_allocator<std::pair<_type1, _type2>, _tAlloc>
	: or_<pair_has_allocator<_type1, _tAlloc>,
	pair_has_allocator<_type2, _tAlloc>>
{};

template<typename, class, typename...>
struct uaca_res;

template<typename _type, class _tAlloc, typename... _tParams>
using uaca_res_t = _t<uaca_res<_type, _tAlloc, _tParams...>>;

template<typename _type, class _tAlloc>
struct uaa_func
{
	const _tAlloc& a;

	template<typename... _tParams>
	YB_ATTR_nodiscard auto
	operator()(_tParams&&...) const
		-> uaca_res_t<_type, _tAlloc, _tParams...>;
};


template<typename, class _bPair, class _bPfx, class _tAlloc,
	typename... _tParams>
std::tuple<_tParams&&...>
uses_allocator_args(_bPair, false_, _bPfx, const _tAlloc&, _tParams&&... args)
{
	return std::forward_as_tuple(yforward(args)...);
}
template<typename, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard
	std::tuple<const std::allocator_arg_t&, const _tAlloc&, _tParams&&...>
uses_allocator_args(false_, true_, true_, const _tAlloc& a, _tParams&&... args)
{
	return std::forward_as_tuple(std::allocator_arg, a, yforward(args)...);
}
template<typename, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard std::tuple<_tParams&&..., const _tAlloc&>
uses_allocator_args(false_, true_, false_, const _tAlloc& a, _tParams&&... args)
{
	return std::forward_as_tuple(yforward(args)..., a);
}
template<class _type, class _tAlloc>
YB_ATTR_nodiscard auto
uses_allocator_args(true_, true_, false_, const _tAlloc&)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<>, std::tuple<>>;
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard auto
uses_allocator_args(true_, true_, false_, const _tAlloc&,
	const std::pair<_type1, _type2>&)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<const _type1&>, std::tuple<const _type2&>>;
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a,
	std::pair<_type1, _type2>&&)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<_type1&&>, std::tuple<_type2&&>>;
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a, _type1&&, _type2&&)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<_type1&&>, std::tuple<_type2&&>>;
//! \since build 861
template<typename _type, class _tAlloc, class _tTuple1, class _tTuple2>
YB_ATTR_nodiscard auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a,
	std::piecewise_construct_t, _tTuple1&& x, _tTuple2&& y) -> decltype(
	std::make_tuple(std::piecewise_construct, ystdex::apply(
	uaa_func<remove_cv_t<typename _type::first_type>, _tAlloc>{a}, yforward(x)),
	ystdex::apply(uaa_func<remove_cv_t<typename _type::second_type>,
	_tAlloc>{a}, yforward(y))))
{
	return std::make_tuple(std::piecewise_construct, ystdex::apply(uaa_func<
		remove_cv_t<typename _type::first_type>, _tAlloc>{a}, yforward(x)),
		ystdex::apply(uaa_func<remove_cv_t<typename _type::second_type>,
		_tAlloc>{a}, yforward(y)));
}

} // unnamed namespace details;

// XXX: Blocked. 'yforward' in the trailing-return-type cause G++ 5.3 20151204
//	crash: internal compiler error: Segmentation fault.
template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard auto
uses_allocator_construction_args(const _tAlloc& a, _tParams&&... args) -> yimpl(
	decltype(details::uses_allocator_args<_type>(details::is_pair<_type>(),
	details::pair_has_allocator<_type, _tAlloc>(), is_constructible<_type,
	std::allocator_arg_t, _tAlloc, _tParams...>(), a,
	std::forward<_tParams&&>(args)...)))
{
	return details::uses_allocator_args<_type>(details::is_pair<_type>(),
		details::pair_has_allocator<_type, _tAlloc>(), is_constructible<_type,
		std::allocator_arg_t, _tAlloc, _tParams...>(), a, yforward(args)...);
}

namespace details
{

template<typename _type, class _tAlloc, typename... _tParams>
struct uaca_res
{
	using type = decltype(ystdex::uses_allocator_construction_args<_type>(
		std::declval<const _tAlloc&>(), std::declval<_tParams>()...));
};

template<typename _type, class _tAlloc>
	template<typename... _tParams>
auto
uaa_func<_type, _tAlloc>::operator()(_tParams&&... args) const
	-> uaca_res_t<_type, _tAlloc, _tParams...>
{
	return
		ystdex::uses_allocator_construction_args<_type>(a, yforward(args)...);
}

template<class _type, class _tAlloc>
auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a) -> uaca_res_t<_type,
	_tAlloc, const std::piecewise_construct_t&, std::tuple<>, std::tuple<>>
{
	return ystdex::uses_allocator_construction_args<_type>(a,
		std::piecewise_construct, std::tuple<>(), std::tuple<>());
}
template<typename _type, class _tAlloc, typename _type1, typename _type2>
auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a,
	const std::pair<_type1, _type2>& arg)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<const _type1&>, std::tuple<const _type2&>>
{
	return ystdex::uses_allocator_construction_args<_type>(a,
		std::piecewise_construct, std::forward_as_tuple(arg.first),
		std::forward_as_tuple(arg.second));
}
template<typename _type, class _tAlloc, typename _type1, typename _type2>
auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a,
	std::pair<_type1, _type2>&& arg)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<_type1&&>, std::tuple<_type2&&>>
{
	return ystdex::uses_allocator_construction_args<_type>(a,
		std::piecewise_construct, std::forward_as_tuple(yforward(arg.first)),
		std::forward_as_tuple(yforward(arg.second)));
}
template<typename _type, class _tAlloc, typename _type1, typename _type2>
auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a, _type1&& arg1,
	_type2&& arg2)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<_type1&&>, std::tuple<_type2&&>>
{
	return ystdex::uses_allocator_construction_args<_type>(a,
		std::piecewise_construct, std::forward_as_tuple(yforward(arg1)),
		std::forward_as_tuple(yforward(arg2)));
}

template<typename _type, class _tAlloc>
struct ucua_func
{
	void* pv;

	template<typename... _tParams>
	_type*
	operator()(_tParams&&... args) const
		ynoexcept(is_nothrow_constructible<_type, _tParams...>())
	{
		return ::new(pv) _type(yforward(args)...);
	}
};

// TODO: Detect more implemenations?
// XXX: This is a workaround to implementation without support of WG21 N4387.
#	if (defined(__GLIBCXX__) && (__GLIBCXX__ <= 20150630 \
	|| YB_IMPL_GNUCPP < 60000)) && !(_LIBCPP_VERSION > 4000 \
	|| __cplusplus >= 201703L)
// NOTE: See See https://gcc.gnu.org/viewcvs/gcc/trunk/libstdc++-v3/include/bits/stl_pair.h?r1=225189&r2=225188&pathrev=225189,
//	and https://reviews.llvm.org/rL276605.
template<typename _type1, typename _type2, class _tAlloc>
struct ucua_func<std::pair<_type1, _type2>, _tAlloc>
{
	void* pv;

	template<typename... _tParams>
	auto
	operator()(_tParams&&... args) const
		-> decltype(::new(pv) std::pair<_type1, _type2>(yforward(args)...))
	{
		return ::new(pv) std::pair<_type1, _type2>(yforward(args)...);
	}
	template<typename _tParam1, typename _tParam2>
	enable_if_t<and_<not_<is_constructible<_tParam1, _tParam2>>,
		is_explicitly_constructible<_type1, _tParam1>,
		is_implicitly_constructible<_type2, _tParam2>>::value,
		std::pair<_type1, _type2>*>
	operator()(_tParam1&& arg1, _tParam2&& arg2) const
	{
		return ::new(pv)
			std::pair<_type1, _type2>(_type1(arg1), yforward(arg2));
	}
	template<typename _tParam1, typename _tParam2>
	enable_if_t<and_<not_<is_constructible<_tParam1, _tParam2>>,
		is_implicitly_constructible<_type1, _tParam1>,
		is_explicitly_constructible<_type2, _tParam2>>::value,
		std::pair<_type1, _type2>*>
	operator()(_tParam1&& arg1, _tParam2&& arg2) const
	{
		return ::new(pv)
			std::pair<_type1, _type2>(yforward(arg1), _type2(arg2));
	}
	template<typename _tParam1, typename _tParam2>
	enable_if_t<and_<not_<is_constructible<_tParam1, _tParam2>>,
		is_explicitly_constructible<_type1, _tParam1>,
		is_explicitly_constructible<_type2, _tParam2>>::value,
		std::pair<_type1, _type2>*>
	operator()(_tParam1&& arg1, _tParam2&& arg2) const
	{
		return ::new(pv)
			std::pair<_type1, _type2>(_type1(arg1), _type2(arg2));
	}
};
#	endif

} // namespace details;

template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard _type
make_obj_using_allocator(const _tAlloc& a, _tParams&&... args)
{
	return ystdex::make_from_tuple<_type>(
		ystdex::uses_allocator_construction_args<_type>(a, yforward(args)...));
}

template<typename _type, class _tAlloc, typename... _tParams>
_type*
uninitialized_construct_using_allocator(_type* p, const _tAlloc& a,
	_tParams&&... args)
{
	return ystdex::apply(details::ucua_func<_type, _tAlloc>{p},
		ystdex::uses_allocator_construction_args<_type>(a, yforward(args)...));
}
//@}
#endif


//! \since build 588
//@{
/*!
\brief 打包对象：通过指定参数构造对象。
\since build 617

对 std::unique_ptr 和 std::shared_ptr 的实例，使用 make_unique
和 std::make_shared 构造，否则直接使用参数构造。
*/
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard auto
pack_object(_tParams&&... args)
	-> decltype(yimpl(details::pack_obj_impl<_type>::pack(yforward(args)...)))
{
	return details::pack_obj_impl<_type>::pack(yforward(args)...);
}


//! \brief 打包的对象：使用 pack_object 得到的对象包装。
template<typename _type, typename _tPack = std::unique_ptr<_type>>
struct object_pack
{
public:
	using value_type = _type;
	using pack_type = _tPack;

private:
	pack_type pack;

public:
	//! \note 使用 ADL pack_object 。
	template<typename... _tParams>
	object_pack(_tParams&&... args)
		: pack(pack_object<_tPack>(yforward(args)...))
	{}

	YB_ATTR_nodiscard YB_PURE
	operator pack_type&() ynothrow
	{
		return pack;
	}
	YB_ATTR_nodiscard YB_PURE
	operator const pack_type&() const ynothrow
	{
		return pack;
	}
};
//@}

#undef YB_Impl_Has_allocator_traits_is_always_equal

} // namespace ystdex;

#endif

