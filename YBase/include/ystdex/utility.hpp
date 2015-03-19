/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file utility.hpp
\ingroup YStandardEx
\brief 实用设施。
\version r2413
\author FrankHB <frankhb1989@gmail.com>
\since build 189
\par 创建时间:
	2010-05-23 06:10:59 +0800
\par 修改时间:
	2015-03-19 10:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Utilities
*/


#ifndef YB_INC_ystdex_utility_hpp_
#define YB_INC_ystdex_utility_hpp_ 1

#include "type_op.hpp" // for ../ydef.h, ystdex::qualified_decay;
#include "cassert.h"
#include <stdexcept> // for std::logic_error;
#include <memory> // for std::addressof;

namespace ystdex
{

/*
\ingroup helper_functions
\brief 交换值并返回旧值。
\return 被替换的原值。
\see ISO WG21/N3797 20.2.3[utility.exchange] 。
\see http://www.open-std.org/JTC1/sc22/WG21/docs/papers/2013/n3668.html 。
*/
template<typename _type, typename _type2 = _type>
_type
exchange(_type& obj, _type2&& new_val)
{
	_type old_val = std::move(obj);

	obj = std::forward<_type2>(new_val);
	return old_val;
}


/*!
\ingroup helper_functions
\brief 退化复制。
\see ISO C++11 30.2.6[thread.decaycopy] 。
\see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2011/n3255.html 。
\since build 439
*/
template<typename _type>
yconstfn decay_t<_type>
decay_copy(_type&& arg)
{
	return std::forward<_type>(arg);
}

/*!
\ingroup helper_functions
\brief 退化传递。
\note 类似 decay_copy ，但仅对函数或数组及其引用类型退化，不复制其它值。
\since build 383
*/
template<typename _type>
yconstfn typename qualified_decay<_type>::type
decay_forward(_type&& arg)
{
	return std::forward<_type>(arg);
}


/*!
\ingroup helper_functions
\brief 计算指定数组类型对象的长度。
\since build 291
*/
//@{
template<typename _type, size_t _vN>
yconstfn size_t
arrlen(_type(&)[_vN])
{
	return _vN;
}
template<typename _type, size_t _vN>
yconstfn size_t
arrlen(_type(&&)[_vN])
{
	return _vN;
}
//@}

/*!
\brief 取枚举值的底层整数。
\since build 585
*/
template<typename _type>
yconstfn underlying_type_t<_type>
underlying(_type val) ynothrow
{
	return underlying_type_t<_type>(val);
}


//! \since build 560
//@{
/*!
\brief 被包装的指针，满足 \c NullPointer 要求同时满足转移后为空。
\tparam _type 被包装的指针。
\pre _type 满足 \c NullPointer 要求且值转换为 \c bool 类型时不抛出异常。
\todo 检查值初始化和 \c nullptr 比较相等。
*/
template<typename _type>
class nptr
{
public:
	using pointer = _type;

private:
	pointer ptr{};

public:
	nptr() = default;
	yconstfn
	nptr(std::nullptr_t)
		: pointer()
	{}
	nptr(pointer p)
		: ptr(p)
	{}
	nptr(const nptr&) = default;
	nptr(nptr&& np) ynothrow
	{
		np.swap(*this);
	}

	nptr&
	operator=(const nptr&) = default;
	nptr&
	operator=(nptr&& np) ynothrow
	{
		np.swap(*this);
		return *this;
	}

	yconstfn bool
	operator!() const ynothrow
	{
		return bool(*this);
	}

	yconstfn explicit
	operator bool() const ynothrow
	{
#if YB_HAS_NOEXCEPT
		static_assert(noexcept(bool(ptr)), "Invalid type found.");
#endif
		return bool(ptr);
	}

	//! \since build 566
	yconstfn const pointer&
	get() const ynothrow
	{
		return ptr;
	}

	pointer&
	get_ref() ynothrow
	{
		return ptr;
	}

	void
	swap(nptr& np) ynothrow
	{
		using std::swap;
#if YB_HAS_NOEXCEPT
		static_assert(noexcept(swap(ptr, np.ptr)), "Invalid type found.");
#endif

		swap(ptr, np.ptr);
	}
};

//! \relates nptr
//@{
template<typename _type>
yconstfn bool
operator==(const nptr<_type>& x, const nptr<_type>& y)
{
	return x.get() == y.get();
}

template<typename _type>
yconstfn bool
operator!=(const nptr<_type>& x, const nptr<_type>& y)
{
	return !(x == y);
}

//! \since build 562
//@{
template<typename _type>
yconstfn bool
operator<(const nptr<_type>& x, const nptr<_type>& y)
{
	return x.get() < y.get();
}

template<typename _type>
yconstfn bool
operator<=(const nptr<_type>& x, const nptr<_type>& y)
{
	return !(y < x);
}

template<typename _type>
yconstfn bool
operator>(const nptr<_type>& x, const nptr<_type>& y)
{
	return y < x;
}

template<typename _type>
yconstfn bool
operator>=(const nptr<_type>& x, const nptr<_type>& y)
{
	return !(x < y);
}
//@}

//! \since build 563
template<typename _type>
inline void
swap(nptr<_type>& x, nptr<_type>& y) ynothrow
{
	x.swap(y);
}
//@}
//@}


/*!
\brief 包装类类型的值的对象。
\warning 非虚析构。
\since build 477
*/
template<typename _type>
struct boxed_value
{
	_type value;

	//! \since build 539
	//@{
	yconstfn
	boxed_value() = default;
	template<typename _tParam,
		yimpl(typename = exclude_self_ctor_t<boxed_value, _tParam>)>
	yconstfn
	boxed_value(_tParam&& arg)
		: value(yforward(arg))
	{}
	template<typename _tParam1, typename _tParam2, typename... _tParams>
	yconstfn
	boxed_value(_tParam1&& arg1, _tParam2&& arg2, _tParams&&... args)
		: value(yforward(arg1), yforward(arg2), yforward(args)...)
	{}
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
using classify_value_t = conditional_t<std::is_class<_type>::value, _type,
	boxed_value<_type>>;


/*!
\brief 按标识调用函数，保证调用一次。
\note 类似 std::call_once ，但不保证线程安全性。
\note ISO C++11（至 N3691 ） 30.4 synopsis 处的声明存在错误。
\bug 未实现支持成员指针。
\see https://github.com/cplusplus/draft/issues/151 。
\since build 327

当标识为 true 时候无作用，否则调用函数。
*/
template<typename _fCallable, typename... _tParams>
inline void
call_once(bool& b, _fCallable&& f, _tParams&&... args)
{
	if(!b)
	{
		f(yforward(args)...);
		b = true;
	}
}


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
\note 线程安全取决于 call_once 对 _tOnceFlag 的支持。
	若对支持 \c \<mutex\> 的实现使用 std::once_flag ，
	对应 std::call_once ，则是线程安全的；
	若使用 bool ，对应 ystdex::call_once ，不保证线程安全。
	其它类型可使用用户自行定义 call_once 。
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
class call_once_init
{
public:
	using object_type = _type;
	using flag_type = _tOnceFlag;

	template<typename... _tParams>
	call_once_init(_tParams&&... args)
	{
		call_once(get_init_flag(), init<_tParams...>, yforward(args)...);
	}
	~call_once_init()
	{
		call_once(get_uninit_flag(), uninit);
	}

	static object_type&
	get()
	{
		yassume(get_object_ptr());
		return *get_object_ptr();
	}

private:
	static flag_type&
	get_init_flag()
	{
		static flag_type flag;

		return flag;
	}

	static object_type*&
	get_object_ptr()
	{
		static object_type* ptr;

		return ptr;
	}

	static flag_type&
	get_uninit_flag()
	{
		static flag_type flag;

		return flag;
	}

	template<typename... _tParams>
	static void
	init(_tParams&&... args)
	{
		get_object_ptr() = new object_type(yforward(args)...);
	}

	static void
	uninit()
	{
		delete get_object_ptr();
	}
};


namespace details
{

//! \since build 569
//@{
template<typename _type, typename _tToken,
	bool _bRef = is_reference<_tToken>::value>
struct state_guard_traits
{
	static void
	save(_tToken t, _type& val) ynoexcept(
		noexcept(std::declval<_tToken&>()(true, std::declval<_type&>())))
	{
		t(true, val);
	}

	static void
	restore(_tToken t, _type& val) ynoexcept(
		noexcept(std::declval<_tToken&>()(false, std::declval<_type&>())))
	{
		t(false, val);
	}
};

template<typename _type, typename _tToken>
struct state_guard_traits<_type, _tToken, true>
{
	//! \todo 按 ISO C++ [utility.swap] 要求确定异常规范。
	static void
	save(_tToken t, _type& val)
	{
		using std::swap;

		swap(val, static_cast<_type&>(t));
	}

	//! \todo 按 ISO C++ [utility.swap] 要求确定异常规范。
	static void
	restore(_tToken t, _type& val)
	{
		using std::swap;

		swap(val, static_cast<_type&>(t));
	}
};


template<typename _type, typename _tToken>
struct state_guard_impl : private state_guard_traits<_type, _tToken>
{
	using value_type = _type;
	using token_type = _tToken;

	token_type token;
	union
	{
		value_type value;
		byte data[sizeof(value_type)];
	};

	state_guard_impl(token_type t)
		: token(t)
	{}
	state_guard_impl(const state_guard_impl&) = delete;
	~state_guard_impl()
	{}

	template<typename... _tParams>
	void
	construct_and_save(_tParams&&... args)
	{
		new(std::addressof(value)) value_type(yforward(args)...);
		save();
	}

	void
	destroy()
	{
		value.~value_type();
	}

	void
	save() ynoexcept(noexcept(state_guard_traits<value_type, token_type>
		::save(std::declval<token_type&>(), std::declval<value_type&>())))
	{
		state_guard_traits<value_type, token_type>::save(token, value);
	}

	void
	restore() ynoexcept(noexcept(state_guard_traits<value_type, token_type>
		::restore(std::declval<token_type&>(),
		std::declval<value_type&>())))
	{
		state_guard_traits<value_type, token_type>::restore(token, value);
	}

	void
	restore_and_destroy() ynoexcept(
		noexcept(state_guard_traits<value_type, token_type>::restore(
		std::declval<token_type&>(), std::declval<value_type&>())))
	{
		restore();
		destroy();
	}
};
//@}

} // namespace details;

/*!
\brief 使用临时状态暂存对象的 scope guard 。
\since build 569
\todo 支持分配器。
\todo 支持有限的复制和转移。
*/
//@{
template<typename _type, typename _tCond = bool,
	typename _tToken = std::function<void(bool, _type&)>>
class state_guard : private details::state_guard_impl<_type, _tToken>
{
private:
	using base = details::state_guard_impl<_type, _tToken>;

public:
	using typename base::value_type;
	using typename base::token_type;
	using condition_type = _tCond;

	using base::token;
	using base::value;
	using base::data;
	mutable condition_type enabled{};

	template<typename... _tParams>
	state_guard(condition_type cond, token_type t, _tParams&&... args)
		: base(t),
		enabled(cond)
	{
		if(enabled)
			base::construct_and_save(yforward(args)...);
	}
	~state_guard() ynoexcept(
		noexcept(std::declval<state_guard&>().base::restore_and_destroy()))
	{
		if(enabled)
			base::restore_and_destroy();
	}

	void
	dismiss()
	{
		if(enabled)
			base::destroy();
		enabled = condition_type();
	}
};

template<typename _type, typename _tToken>
class state_guard<_type, void, _tToken>
	: private details::state_guard_impl<_type, _tToken>
{
private:
	using base = details::state_guard_impl<_type, _tToken>;

public:
	using typename base::value_type;
	using typename base::token_type;
	using condition_type = void;

	using base::token;
	using base::value;
	using base::data;

	template<typename... _tParams>
	state_guard(token_type t, _tParams&&... args)
		: base(t)
	{
		base::construct_and_save(yforward(args)...);
	}
	~state_guard()
	{
		base::restore_and_destroy();
	}
};
//@}


/*!
\brief 使用 ADL swap 调用暂存对象的 scope guard 。
\since build 569
\todo 支持分配器。
\todo 支持有限的复制和转移。
*/
template<typename _type, typename _tCond = bool, typename _tRef = _type&>
using swap_guard = state_guard<_type, _tCond, _tRef>;

} // namespace ystdex;

#if !YB_HAS_BUILTIN_NULLPTR
using ystdex::nullptr;
#endif

#endif

