/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file exception.h
\ingroup LibDefect
\brief 标准库实现 \c \<exception\> 修正。
\version r369
\author FrankHB <frankhb1989@gmail.com>
\since build 550
\par 创建时间:
	2014-11-01 00:13:53 +0800
\par 修改时间:
	2015-12-10 11:51 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	LibDefect::Exception
*/


#ifndef YB_INC_libdefect_exception_h_
#define YB_INC_libdefect_exception_h_ 1

#include <exception>

// NOTE: It is decided to only support G++ 4.6+. Thus the 'noexcept' and
//	'nullptr' can be used in C++11 mode then.
// NOTE: It seems there are no these 2 C++11 'get_*' routines before libstdc++
//	4.9. So here just adds them.
// NOTE: No exception propagation and nested exception supported by libstdc++
//	when 'ATOMIC_INT_LOCK_FREE < 2'. This module is mainly for workaround it.
//	See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=58938.
// NOTE: It is initially intended to be binary-compatible with libstdc++ 4.8 +
//	with '-std=c++11' enabled, except for deprecated std::copy_exception. (See
//	LWG1170, also notes below for std::make_exception_ptr.)
//	However, the implementation of nested exception in libstdc++ previous than
//	5.0 is not conforming to the formal standard, so 5.0 trunk code is adapted.
//	See http://stackoverflow.com/questions/25324262/stdthrow-with-nested-expects-polymorphic-type-in-c11.
//	See also https://gcc.gnu.org/bugzilla/show_bug.cgi?id=62154.
// NOTE: To avoid 'error: attributes are not allowed on a function-definition',
//	'[[noreturn]]' or '__attribute__((__noreturn__))' can be used at front of,
//	the function definition instead of '__attribute__((__noreturn__))' after a
//	function declarator for. [[noreturn]] does not work on G++ earlier than 4.8.
//	For compatibility reason this is not used.

#if defined(__GLIBCXX__) \
	&& (defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L) \
	&& __GNUC__ * 100 + __GNUC_MINOR__ < 409

#	pragma GCC visibility push(default)

namespace std
{

terminate_handler
get_terminate() noexcept;

unexpected_handler
get_unexpected() noexcept;

} // namespacee std;

#	pragma GCC visibility pop

#endif

#if defined(__GLIBCXX__) \
	&& (defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L) \
	&& ATOMIC_INT_LOCK_FREE < 2

#	pragma GCC visibility push(default)

#	include <bits/c++config.h>
#	include <bits/exception_defines.h>

extern "C++"
{

namespace std
{

// NOTE: Following code is compatible with <bits/exception_ptr.h>.

class type_info;

namespace __exception_ptr
{

class exception_ptr;

} // namespace __exception_ptr;

using __exception_ptr::exception_ptr;

exception_ptr
current_exception() noexcept;

__attribute__((__noreturn__)) void
rethrow_exception(exception_ptr);

namespace __exception_ptr
{

class exception_ptr
{
	void* _M_exception_object = {};

	explicit
	exception_ptr(void* __e) noexcept;

	void
	_M_addref() noexcept;

	void
	_M_release() noexcept;

	void*
	_M_get() const noexcept __attribute__((__pure__));

	friend exception_ptr
	std::current_exception() noexcept;

	friend void
	std::rethrow_exception(exception_ptr);

public:
	exception_ptr() noexcept;
	exception_ptr(const exception_ptr&) noexcept;
	exception_ptr(nullptr_t) noexcept
		: _M_exception_object()
	{}
	exception_ptr(exception_ptr&& __o) noexcept
		: _M_exception_object(__o._M_exception_object)
	{
		__o._M_exception_object = {};
	}
	~exception_ptr() noexcept;

#	ifdef _GLIBCXX_EH_PTR_COMPAT
	typedef void(exception_ptr::*__safe_bool)();

	exception_ptr(__safe_bool) noexcept;
#	endif

	exception_ptr&
	operator=(const exception_ptr&) noexcept;
	exception_ptr&
	operator=(exception_ptr&& __o) noexcept
	{
		exception_ptr(static_cast<exception_ptr&&>(__o)).swap(*this);
		return *this;
	}

	void
	swap(exception_ptr&) noexcept;

#	ifdef _GLIBCXX_EH_PTR_COMPAT
	// Retained for compatibility with CXXABI_1.3.
	void
	_M_safe_bool_dummy() noexcept __attribute__((__const__));

	bool
	operator!() const noexcept __attribute__((__pure__));

	operator __safe_bool() const noexcept;
#	endif

	explicit
	operator bool() const
	{
		return _M_exception_object;
	}

	friend bool
	operator==(const exception_ptr&, const exception_ptr&)
		noexcept __attribute__((__pure__));

	const class std::type_info*
	__cxa_exception_type() const noexcept
		__attribute__((__pure__));
};

bool
operator==(const exception_ptr&, const exception_ptr&) noexcept
	__attribute__((__pure__));

bool
operator!=(const exception_ptr&, const exception_ptr&) noexcept
	__attribute__((__pure__));

inline void
swap(exception_ptr& __lhs, exception_ptr& __rhs)
{
	__lhs.swap(__rhs);
}

} // namespace __exception_ptr;

// NOTE: See also https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56905. Symbol for
//	compatibility with experimental C++0x implementation is not supported.
template<typename _Ex>
exception_ptr
make_exception_ptr(_Ex __ex) noexcept
{
	__try
	{
#	ifdef __EXCEPTIONS
		throw __ex;
#	endif
	}
	__catch(...)
	{
		return current_exception();
	}
}

// NOTE: Following code is compatible with <bits/nested_exception.h>.

class nested_exception
{
	exception_ptr _M_ptr;

public:
	nested_exception() noexcept
		: _M_ptr(current_exception())
	{}
	nested_exception(const nested_exception&) noexcept = default;
	virtual
	~nested_exception() noexcept = default;

	nested_exception&
	operator=(const nested_exception&) noexcept = default;

	__attribute__((__noreturn__)) void
	rethrow_nested() const
	{
		if(_M_ptr)
			rethrow_exception(_M_ptr);
		std::terminate();
	}

	exception_ptr
	nested_ptr() const noexcept
	{
		return _M_ptr;
	}
};

template<typename _Except>
struct _Nested_exception : public _Except, public nested_exception
{
	explicit
	_Nested_exception(const _Except& __ex)
		: _Except(__ex)
	{}
	explicit
	_Nested_exception(_Except&& __ex)
		: _Except(static_cast<_Except&&>(__ex))
	{}
};

template<typename _Tp,
	bool __with_nested = !__is_base_of(nested_exception, _Tp)>
struct _Throw_with_nested_impl
{
	template<typename _Up>
	__attribute__((__noreturn__)) static inline void
	_S_throw(_Up&& __t)
	{
		throw _Nested_exception<_Tp>{static_cast<_Up&&>(__t)};
	}
};

template<typename _Tp>
struct _Throw_with_nested_impl<_Tp, false>
{
	template<typename _Up>
	__attribute__((__noreturn__)) static inline void
	_S_throw(_Up&& __t)
	{
		throw static_cast<_Up&&>(__t);
	}
};


template<typename _Tp, bool = __is_class(_Tp)>
struct _Throw_with_nested_helper
	: _Throw_with_nested_impl<_Tp>
{};

template<typename _Tp>
struct _Throw_with_nested_helper<_Tp, false>
	: _Throw_with_nested_impl<_Tp, false>
{};

template<typename _Tp>
struct _Throw_with_nested_helper<_Tp&, false>
	: _Throw_with_nested_helper<_Tp>
{};

template<typename _Tp>
struct _Throw_with_nested_helper<_Tp&&, false>
	: _Throw_with_nested_helper<_Tp>
{};

template<typename _Tp>
__attribute__((__noreturn__)) inline void
throw_with_nested(_Tp&& __t)
{
	_Throw_with_nested_helper<_Tp>::_S_throw(static_cast<_Tp&&>(__t));
}

template<typename _Tp, bool = __is_polymorphic(_Tp)>
struct _Rethrow_if_nested_impl
{
	static void
	_S_rethrow(const _Tp& __t)
	{
		if(const auto __tp = dynamic_cast<const nested_exception*>(&__t))
			__tp->rethrow_nested();
	}
};

template<typename _Tp>
struct _Rethrow_if_nested_impl<_Tp, false>
{
	static void
	_S_rethrow(const _Tp&)
	{}
};

template<typename _Ex>
inline void
rethrow_if_nested(const _Ex& __ex)
{
	_Rethrow_if_nested_impl<_Ex>::_S_rethrow(__ex);
}

} // namespace std;

} // extern "C++";

#	pragma GCC visibility pop

#endif

#endif

