/*
	© 2010-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file utility.hpp
\ingroup YStandardEx
\brief 实用设施。
\version r3265
\author FrankHB <frankhb1989@gmail.com>
\since build 189
\par 创建时间:
	2010-05-23 06:10:59 +0800
\par 修改时间:
	2017-05-10 23:10 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Utilities
*/


#ifndef YB_INC_ystdex_utility_hpp_
#define YB_INC_ystdex_utility_hpp_ 1

#include "type_pun.hpp" // for "type_pun.hpp", add_const_t,
//	is_nothrow_constructible, is_nothrow_assignable, std::move, add_volatile_t,
//	bool_, is_standard_layout, pun_storage_t, is_nothrow_swappable_with,
//	std::swap, aligned_replace_cast, exclude_self_t, default_init,
//	replace_storage_t;
#include <functional> // for std::bind, std::ref
#include "memory.hpp" // for std::addressof, yassume, ystdex::construct_in,
//	ystdex::destruct_in;
#include "base.h" // for noncopyable, nonmovable;

namespace ystdex
{

/*!
\brief 转换 const 引用。
\see WG21 N4380 。
\since build 593
*/
template<typename _type>
inline add_const_t<_type>&
as_const(_type& t)
{
	return t;
}


/*!
\brief 交换非 volatile 左值和 volatile 左值。
\pre 可赋值。
\since build 704
*/
//@{
template<typename _type>
yimpl(enable_if_t)<is_nothrow_constructible<_type, volatile _type&&>()
	&& is_nothrow_assignable<_type&, volatile _type&&>()
	&& is_nothrow_assignable<volatile _type&, _type&&>()>
swap_volatile(_type& x, volatile _type& y)
	ynoexcept(is_nothrow_constructible<_type, volatile _type&>()
	&& noexcept(x = std::move(y)) && noexcept(y = std::move(x)))
{
	auto t(std::move(x));

	x = std::move(y);
	y = std::move(t);
}
template<typename _type>
inline auto
swap_volatile(volatile _type& x, _type& y)
	ynoexcept_spec(ystdex::swap_volatile(y, x))
	-> decltype(ystdex::swap_volatile(y, x))
{
	ystdex::swap_volatile(y, x);
}
template<typename _type, size_t _vN>
void
swap_volatile(_type(&x)[_vN], add_volatile_t<_type[_vN]>& y)
	ynoexcept(noexcept(ystdex::swap_volatile(x[0], y[0])))
{
	auto first(std::addressof(x[0]));
	auto first2(std::addressof(y[0]));

	for(const auto last(first + _vN); first != last; yunseq(++first, ++first2))
		ystdex::swap_volatile(*first, *first2);
}
//@}

//! \since build 704
namespace details
{

template<typename _type, typename _type2>
using swap_volatile_avail = bool_<is_volatile<remove_reference_t<_type>>::value
	!= is_volatile<remove_reference_t<_type2>>::value>;

#if YB_HAS_NOEXCEPT
template<typename _type, typename _type2>
using swap_volatile_noexcept = and_<swap_volatile_avail<_type, _type2>, bool_<
	noexcept(swap_volatile(std::declval<_type>(), std::declval<_type2>()))>>;
#else
template<typename _type, typename _type2>
using swap_volatile_noexcept = swap_volatile_avail<_type, _type2>;
#endif

} // namespace details;

/*!
\brief 交换相同标准布局类型可修改左值的存储。
\since build 620
*/
template<typename _type>
void
swap_underlying(_type& x, _type& y) ynothrow
{
	static_assert(is_standard_layout<_type>(),
		"Invalid underlying type found.");
	using utype = pun_storage_t<_type>;

	std::swap(ystdex::aligned_replace_cast<utype&>(x),
		ystdex::aligned_replace_cast<utype&>(y));
}

/*!
\brief 交换可能是 volatile 的左值或右值。
\pre 参数类型可交换，或具有一个 volatile 类型可用 swap_volatie 交换。
\since build 704
*/
//@{
//! \note 使用 ADL swap 或 std::swap 。
template<typename _type, typename _type2, yimpl(typename
	= enable_if_t<!details::swap_volatile_avail<_type, _type2>::value>)>
void
vswap(_type&& x, _type2&& y)
	ynoexcept(is_nothrow_swappable_with<_type&&, _type2&&>())
{
	using std::swap;

	swap(yforward(x), yforward(y));
}
//! \note 使用 ADL swap_volatile 或 ystdex::swap_volatile 。
//@{
template<typename _type, typename _type2>
inline auto	
vswap(_type&& x, _type2&& y) ynoexcept(detected_or_t<false_,
	details::swap_volatile_noexcept, _type, _type2>())
	-> yimpl(enable_if_t)<details::swap_volatile_avail<_type, _type2>::value>
{
	swap_volatile(yforward(x), yforward(y));
}
//@}
//@}


/*!
\brief 复制后交换。
\note 使用 ADL swap 或 std::swap 。
\since build 768
*/
template<typename _type, typename _type2 = _type>
inline _type&
copy_and_swap(_type& obj, const _type2& new_val)
{
	using std::swap;
	auto t(new_val);

	swap(t, obj);
	return obj;
}

/*!
\brief 按参数复制或转移。
\since build 787
*/
template<typename _type, typename _type2 = _type>
inline _type
copy_or_move(bool copy, _type& obj)
{
	if(copy)
		return ystdex::as_const(obj);
	return std::move(obj);
}

inline namespace cpp2014
{

#if __cpp_lib_exchange_function >= 201304 || __cplusplus > 201103L
using std::exchange;
#else
/*!
\brief 交换值并返回旧值。
\return 被替换的原值。
\see WG21 N3797 20.2.3[utility.exchange] 。
\see http://www.open-std.org/JTC1/sc22/WG21/docs/papers/2013/n3668.html 。
\since build 517
*/
template<typename _type, typename _type2 = _type>
_type
exchange(_type& obj, _type2&& new_val)
{
	_type old_val = std::move(obj);

	obj = std::forward<_type2>(new_val);
	return old_val;
}
#endif

} // inline namespace cpp2014;


/*!
\ingroup helper_functions
\brief 退化复制。
\see ISO C++11 30.2.6 [thread.decaycopy] 。
\see WG21 N3255 。
\since build 650
*/
template<typename _type>
yconstfn yimpl(enable_if_convertible_t)<_type, decay_t<_type>, decay_t<_type>>
decay_copy(_type&& arg)
{
	return std::forward<_type>(arg);
}


/*!
\brief 取枚举值的底层整数。
\since build 629
*/
template<typename _type, yimpl(typename = enable_if_t<is_enum<_type>::value>)>
yconstfn underlying_type_t<_type>
underlying(_type val) ynothrow
{
	return underlying_type_t<_type>(val);
}


/*!
\brief 包装类类型的值的对象。
\warning 非虚析构。
\since build 477
*/
template<typename _type>
struct boxed_value
{
	//! \since build 677
	mutable _type value;

	//! \since build 677
	//@{
	yconstfn
	boxed_value() ynoexcept(is_nothrow_constructible<_type>())
		: value()
	{}
	//! \since build 716
	yconstfn
	boxed_value(default_init_t) ynoexcept(is_nothrow_constructible<_type>())
	{}
	template<typename _tParam,
		yimpl(typename = exclude_self_t<boxed_value, _tParam>)>
	yconstfn
	boxed_value(_tParam&& arg)
		ynoexcept(is_nothrow_constructible<_type, _tParam&&>())
		: value(yforward(arg))
	{}
	template<typename _tParam1, typename _tParam2, typename... _tParams>
	yconstfn
	boxed_value(_tParam1&& arg1, _tParam2&& arg2, _tParams&&... args)
		ynoexcept(is_nothrow_constructible<_type, _tParam1&&, _tParam2&&,
		_tParams&&...>())
		: value(yforward(arg1), yforward(arg2), yforward(args)...)
	{}
	//@}
	//! \since build 539
	//@{
	boxed_value(const boxed_value&) = default;
	boxed_value(boxed_value&&) = default;

	boxed_value&
	operator=(const boxed_value&) = default;
	boxed_value&
	operator=(boxed_value&&) = default;
	//@}

	operator _type&() ynothrow
	{
		return value;
	}

	operator const _type&() const ynothrow
	{
		return value;
	}
};


/*!
\ingroup metafunctions
\brief 包装非类类型为类类型。
\since build 477
*/
template<typename _type>
using classify_value_t = cond_t<std::is_class<_type>, _type,
	boxed_value<_type>>;


/*!
\brief 类型参数化静态对象。
\warning 不可重入。
\warning 非线程安全。
\since build 303
*/
template<typename _type, typename, typename...>
inline _type&
parameterize_static_object()
{
	static _type obj;

	return obj;
}
/*!
\brief 非类型参数化静态对象。
\warning 不可重入。
\warning 非线程安全。
\since build 301
*/
template<typename _type, size_t...>
inline _type&
parameterize_static_object()
{
	static _type obj;

	return obj;
}


/*!
\brief 取类型标识和初始化调用指定的对象。
\tparam _tKey 起始类型参数化标识。
\tparam _tKeys 非起始类型参数化标识。
\tparam _fInit 初始化调用类型。
\tparam _tParams 初始化参数类型。
\return 初始化后的对象的左值引用。
\since build 327
*/
template<typename _tKey, typename... _tKeys, typename _fInit,
	typename... _tParams>
inline auto
get_init(_fInit&& f, _tParams&&... args) -> decltype(f(yforward(args)...))&
{
	using obj_type = decltype(f(yforward(args)...));

	auto& p(ystdex::parameterize_static_object<obj_type*, _tKey, _tKeys...>());

	if(!p)
		p = new obj_type(f(yforward(args)...));
	return *p;
}
/*!
\brief 取非类型标识和初始化调用指定的对象。
\tparam _vKeys 非类型参数化标识。
\tparam _fInit 初始化调用类型。
\tparam _tParams 初始化参数类型。
\return 初始化后的对象的左值引用。
\since build 327
*/
template<size_t... _vKeys, typename _fInit, typename... _tParams>
inline auto
get_init(_fInit&& f, _tParams&&... args) -> decltype(f(yforward(args)...))&
{
	using obj_type = decltype(f(yforward(args)...));

	auto& p(ystdex::parameterize_static_object<obj_type*, _vKeys...>());

	if(!p)
		p = new obj_type(f(yforward(args)...));
	return *p;
}


/*!	\defgroup init_mgr Initialization Managers
\brief 初始化管理器。
\since build 328

实现保存初始化和反初始化的状态的对象。不直接初始化对象，可以在头文件中直接定义。
保证初始化满足特定条件。
*/

/*!
\ingroup init_mgr
\brief 使用引用计数的静态初始化管理器。
\pre _type 满足 Destructible 。
\note 当实现支持静态 TLS 时为每线程单例，否则为全局静态单例。
\warning 对不支持 TLS 的实现非线程安全。
\sa ythread
\see http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Nifty_Counter 。
\since build 425

静态初始化，通过引用计数保证所有在定义本类型的对象后已有静态对象被初始化。
在所有翻译单元的本类型对象析构后自动反初始化。
*/
template<class _type>
class nifty_counter
{
public:
	using object_type = _type;

	template<typename... _tParams>
	nifty_counter(_tParams&&... args)
	{
		if(get_count()++ == 0)
			get_object_ptr() = new _type(yforward(args)...);
	}
	//! \since build 425
	//@{
	//! \since build 461
	~nifty_counter()
	{
		if(--get_count() == 0)
			delete get_object_ptr();
	}

	static object_type&
	get() ynothrow
	{
		yassume(get_object_ptr());
		return *get_object_ptr();
	}

private:
	static size_t&
	get_count() ynothrow
	{
		ythread size_t count;

		return count;
	}
	static object_type*&
	get_object_ptr() ynothrow
	{
		ythread object_type* ptr;

		return ptr;
	}

public:
	static size_t
	use_count() ynothrow
	{
		return get_count();
	}
	//@}
};


/*!
\ingroup init_mgr
\brief 使用 call_once 的静态初始化管理器。
\tparam _tOnceFlag 初始化调用标识。
\note 使用 ADL call_once 。
\note 线程安全取决于 call_once 对 _tOnceFlag 的支持。
	若对支持 \c \<mutex\> 的实现使用 std::once_flag ，
	对应 std::call_once ，则是线程安全的。
\since build 328
\todo 使用支持 lambda pack 展开的实现构造模板。
\todo 支持分配器。

静态初始化，使用 _tOnceFlag 类型的静态对象表示初始化和反初始化状态，
保证所有在定义本类型的对象后已有静态对象被初始化。
在所有翻译单元的本类型对象析构后自动反初始化。
初始化和反初始化调用没有限定符修饰的 call_once 初始化和反初始化。
用户可以自定义 _tOnceFlag 实际参数对应的 call_once ，但声明
	应与 std::call_once 和 ystdex::call_once 形式一致。
*/
template<typename _type, typename _tOnceFlag>
class call_once_init : private noncopyable, private nonmovable
{
public:
	using object_type = _type;
	using flag_type = _tOnceFlag;

private:
	//! \since build 693
	flag_type init_flag, uninit_flag;
	//! \since build 693
	replace_storage_t<_type> storage;

public:
	template<typename... _tParams>
	call_once_init(_tParams&&... args)
		: init_flag(), uninit_flag()
	{
		call_once(init_flag, std::bind(ystdex::construct_in<_type,
			_tParams&&...>, std::ref(get()), std::ref(args)...));
	}
	~call_once_init()
	{
		call_once(init_flag, std::bind(ystdex::destruct_in<_type>,
			std::ref(get())));
	}

	//! \since build 693
	object_type&
	get() ynothrow
	{
		return storage.template access<object_type>();
	}
	//! \since build 693
	const object_type&
	get() const ynothrow
	{
		return storage.template access<const object_type>();
	}
};

} // namespace ystdex;

#if !YB_HAS_BUILTIN_NULLPTR
using ystdex::nullptr;
#endif

#endif

