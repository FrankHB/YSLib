/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file utility.hpp
\ingroup YStandardEx
\brief 实用设施。
\version r1731
\author FrankHB <frankhb1989@gmail.com>
\since build 189
\par 创建时间:
	2010-05-23 06:10:59 +0800
\par 修改时间:
	2014-03-07 23:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Utilities
*/


#ifndef YB_INC_ystdex_utility_hpp_
#define YB_INC_ystdex_utility_hpp_ 1

#include "type_op.hpp" // for ../ydef.h, ystdex::qualified_decay;
#include "cassert.h"
#include <utility>
#include <stdexcept> // for std::logic_error;

namespace ystdex
{

//! \since build 475
//@{
//! \brief 异常：不支持的操作。
class YB_API unsupported : public std::logic_error
{
public:
	unsupported()
		: logic_error("Unsupported operation found.")
	{}
	template<typename _type>
	unsupported(_type&& arg)
		: logic_error(yforward(arg))
	{}
};


//! \brief 异常：未实现的操作。
class YB_API unimplemented : public unsupported
{
public:
	unimplemented()
		: unsupported("Unimplemented operation found.")
	{}
	template<typename _type>
	unimplemented(_type&& arg)
		: unsupported(yforward(arg))
	{}
};
//@}


/*!
\brief 不可复制对象：禁止派生类调用默认原型的复制构造函数和复制赋值操作符。
\warning 非虚析构。
\since build 373
*/
class noncopyable
{
protected:
	/*!
	\brief \c protected 构造：默认实现。
	\note 保护非多态类。
	*/
	yconstfn
	noncopyable() = default;
	/*!
	\brief \c protected 析构：默认实现。
	*/
	~noncopyable() = default;

public:
	/*!
	\brief 禁止复制构造。
	*/
	yconstfn
	noncopyable(const noncopyable&) = delete;

	/*!
	\brief 禁止赋值复制。
	*/
	noncopyable&
	operator=(const noncopyable&) = delete;
};


/*
\brief 不可转移对象：禁止继承此类的对象调用默认原型的转移构造函数和转移赋值操作符。
\warning 非虚析构。
\since build 373
*/
class nonmovable
{
protected:
	/*!
	\brief \c protected 构造：默认实现。
	\note 保护非多态类。
	*/
	yconstfn
	nonmovable() = default;
	/*!
	\brief \c protected 析构：默认实现。
	*/
	~nonmovable() = default;

public:
	/*!
	\brief 禁止复制构造。
	*/
	yconstfn
	nonmovable(const nonmovable&) = delete;

	/*!
	\brief 禁止赋值复制。
	*/
	nonmovable&
	operator=(const nonmovable&) = delete;
};


/*!
\brief 可动态复制的抽象基类。
\since build 475
*/
class YB_API cloneable
{
public:
#if YB_IMPL_GNUCPP < 40702
	//! \since build 475 as workaround for G++ 4.7.1
	//@{
	cloneable() = default;
	cloneable(const cloneable&) = default;
#if YB_IMPL_MSCPP
	//! \since build 483 as workaround for Visual C++ 2013
	cloneable(cloneable&&)
	{};
#else
	cloneable(cloneable&&) = default;
#endif
#else
	//@}
#endif
	virtual cloneable*
	clone() const = 0;

	virtual
	~cloneable()
	{}
};


/*!
\ingroup helper_functions
\brief 退化复制。
\see ISO C++11 30.2.6[thread.decaycopy] 。
\see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2011/n3255.html 。
\since build 439
*/
template<typename _type>
decay_t<_type>
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
typename qualified_decay<_type>::type
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
\brief 包装类类型的值的对象。
\warning 非虚析构。
\since build 477
*/
template<typename _type>
struct boxed_value
{
	_type value;

	template<typename... _tParams>
	yconstfn
	boxed_value(_tParams&&... args)
		: value(yforward(args)...)
	{}

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
\warning 对于不支持 TLS 的实现非线程安全。
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
	若对于支持 <tt><mutex></tt> 的实现，使用 std::once_flag ，
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

} // namespace ystdex;

#if !YB_HAS_BUILTIN_NULLPTR
using ystdex::nullptr;
#endif

#endif

