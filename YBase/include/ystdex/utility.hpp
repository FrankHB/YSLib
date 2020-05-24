/*
	© 2010-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file utility.hpp
\ingroup YStandardEx
\brief 实用设施。
\version r3570
\author FrankHB <frankhb1989@gmail.com>
\since build 189
\par 创建时间:
	2010-05-23 06:10:59 +0800
\par 修改时间:
	2020-05-24 11:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Utilities
*/


#ifndef YB_INC_ystdex_utility_hpp_
#define YB_INC_ystdex_utility_hpp_ 1

#include "swap.hpp" // for "swap.hpp", is_lvalue_reference, conditional_t,
//	remove_reference_t, std::forward, add_const_t, std::move,
//	is_nothrow_constructible, enable_if_convertible_t, exclude_self_t;
#include "type_op.hpp" // for exclude_self_params_t;
#include "placement.hpp" // for yassume, ystdex::construct_in,
//	ystdex::destruct_in;
#include "base.h" // for noncopyable, nonmovable;
#include <functional> // for std::bind, std::ref
#include <tuple> // for tuple workaround;

namespace ystdex
{

/*!
\see https://stackoverflow.com/questions/46171843/moving-in-range-based-loop-in-generic-c-code 。
\since build 845
*/
//@{
//! \brief 按指定类型蕴含的值类别传递参数的结果类型。
template<typename _type, typename _type2>
using forward_like_t = conditional_t<is_lvalue_reference<_type>::value,
    remove_reference_t<_type2>&, remove_reference_t<_type2>&&>;

//! \brief 按第一模板参数指定类型蕴含的值类别传递参数。
template<typename _type, typename _type2>
YB_ATTR_nodiscard YB_STATELESS forward_like_t<_type, _type2>
forward_like(_type2&& x)
{
	return std::forward<forward_like_t<_type, _type2>>(yforward(x));
}
//@}

inline namespace cpp2017
{

/*!
\brief 转换 const 引用。
\see WG21 P0007R1 。
*/
//@{
#if __cpp_lib_as_const >= 201510L
using std::as_const;
#else
//! \since build 593
template<typename _type>
YB_ATTR_nodiscard YB_STATELESS inline add_const_t<_type>&
as_const(_type& t)
{
	return t;
}
//! \since build 833
template<class _type>
void
as_const(const _type&&) = delete;
#endif
//@}

} // inline namespace cpp2017;

/*!
\brief 按参数复制或转移。
\since build 787
*/
template<typename _type, typename _type2 = _type>
YB_ATTR_nodiscard inline _type
copy_or_move(bool copy, _type& obj)
{
	if(copy)
		return ystdex::as_const(obj);
	return std::move(obj);
}


/*!
\ingroup helper_functions
\brief 退化复制。
\see ISO C++11 30.2.6 [thread.decaycopy] 。
\see WG21 N3255 。
\since build 650
*/
template<typename _type>
YB_ATTR_nodiscard yconstfn
	yimpl(enable_if_convertible_t)<_type, decay_t<_type>, decay_t<_type>>
decay_copy(_type&& arg)
{
	return yforward(arg);
}


/*!
\brief 取枚举值的底层整数。
\since build 629
*/
template<typename _type, yimpl(typename = enable_if_t<is_enum<_type>::value>)>
YB_ATTR_nodiscard YB_STATELESS yconstfn underlying_type_t<_type>
underlying(_type val) ynothrow
{
	return underlying_type_t<_type>(val);
}

namespace details
{

template<typename _type>
struct is_nt_dc : is_nothrow_constructible<_type>
{};

#if defined(__GLIBCXX__) \
	&& (YB_IMPL_GNUCPP && YB_IMPL_GNUCPP < 80200 && YB_IMPL_GNUCPP > 70100)
// XXX: Workaround of libstdc++ non-SFINAE friendly std::tuple implementation.
//	It is unknown which versions are exactly affected. See $2019-01
//	@ %Documentation::Workflow.
template<size_t _vIdx, typename _type, typename... _types>
struct is_nt_dc<std::_Tuple_impl<_vIdx, _type, _types...>>
	: and_<is_nothrow_constructible<_type>,
	is_nt_dc<std::_Tuple_impl<_vIdx + 1, _types...>>>
{};
#endif

} // namespace details;

/*!
\brief 包装类类型的值的对象。
\warning 非虚析构。
\since build 477
*/
template<typename _type>
struct boxed_value
{
	//! \since build 851
	using nothrow_default_constructible = details::is_nt_dc<_type>;

	//! \since build 677
	mutable _type value;

	//! \since build 851
	//@{
	template<yimpl(typename _tParam = _type,
		typename = enable_if_t<is_constructible<_tParam>::value>)>
	yconstfn
	boxed_value() ynoexcept(nothrow_default_constructible())
		: value()
	{}
	template<yimpl(typename _tParam = _type,
		typename = enable_if_t<is_constructible<_tParam>::value>)>
	explicit
	boxed_value(default_init_t) ynoexcept(nothrow_default_constructible())
	{}
	//@}
	//! \since build 848
	template<typename... _tParams,
		yimpl(typename = exclude_self_params_t<boxed_value, _tParams...>,
		typename = enable_if_t<is_constructible<_type, _tParams...>::value>)>
	yconstfn
	boxed_value(_tParams&&... args)
		ynoexcept(is_nothrow_constructible<_type, _tParams...>())
		: value(yforward(args)...)
	{}
	//! \since build 539
	//@{
	boxed_value(const boxed_value&) = default;
	boxed_value(boxed_value&&) = default;

	boxed_value&
	operator=(const boxed_value&) = default;
	boxed_value&
	operator=(boxed_value&&) = default;
	//@}

	YB_ATTR_nodiscard
	operator _type&() ynothrow
	{
		return value;
	}

	YB_ATTR_nodiscard
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
YB_ATTR_nodiscard inline _type&
parameterize_static_object()
{
	static _type obj;

	return obj;
}
/*!
\brief 非类型参数化静态对象。
\note 适合代替参数重复出现或需要作为公开接口的块作用域 static 声明。
\warning 不可重入。
\warning 非线程安全。
\since build 301
*/
template<typename _type, size_t...>
YB_ATTR_nodiscard _type&
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

	YB_ATTR_nodiscard static object_type&
	get() ynothrow
	{
		yassume(get_object_ptr());
		return *get_object_ptr();
	}

private:
	YB_ATTR_nodiscard static size_t&
	get_count() ynothrow
	{
		ythread size_t count;

		return count;
	}
	YB_ATTR_nodiscard static object_type*&
	get_object_ptr() ynothrow
	{
		ythread object_type* ptr;

		return ptr;
	}

public:
	YB_ATTR_nodiscard static size_t
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
	若对支持 \c \<mutex> 的实现使用 std::once_flag ，
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
	YB_ATTR_nodiscard object_type&
	get() ynothrow
	{
		return storage.template access<object_type>();
	}
	//! \since build 693
	YB_ATTR_nodiscard const object_type&
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

