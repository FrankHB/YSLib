/*
	© 2014-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file exception.cpp
\ingroup LibDefect
\brief 标准库实现 \c \<exception\> 修正。
\version r632
\author FrankHB <frankhb1989@gmail.com>
\since build 550
\par 创建时间:
	2014-11-01 11:00:14 +0800
\par 修改时间:
	2018-10-19 03:34 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	LibDefect::Exception
*/


#include <cstddef> // for std::size_t and <bits/c++config.h> if using libstdc++.
#include <atomic> // for <bits/atomic_lockfree_defines.h> if using libstdc++.

// NOTE: See header <libdefect/exception.h>.
#if defined(__GLIBCXX__) && __GLIBCXX__ < 20170718 \
	&& (defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L) \
	&& (ATOMIC_INT_LOCK_FREE < 2 && __GNUC__ < 7)

#	include "libdefect/exception.h"
#	include <cxxabi.h> // for __cxxabiv1 and std::type_info;
#	include <bits/atomic_word.h> // for ::_Atomic_word;
#	include <unwind.h> // from libgcc, for __ARM_EABI_UNWINDER__, _Unwind_Ptr,
//	etc;
#	if defined(__CLANG_UNWIND_H) && !defined(__ARM_EABI_UNWINDER__) \
	&& defined(__arm__) && !defined(__USING_SJLJ_EXCEPTIONS__) \
	&& !defined(__ARM_DWARF_EH__)
// NOTE: See https://reviews.llvm.org/D15883.
#		define __ARM_EABI_UNWINDER__ 1
#	endif

#	pragma GCC visibility push(default)

namespace __cxxabiv1
{

struct __cxa_exception
{
	std::type_info* exceptionType;
	void(_GLIBCXX_CDTOR_CALLABI*exceptionDestructor)(void*);
	std::unexpected_handler unexpectedHandler;
	std::terminate_handler terminateHandler;
	__cxa_exception* nextException;
	int handlerCount;
#	ifdef __ARM_EABI_UNWINDER__
	__cxa_exception* nextPropagatingException;
	int propagationCount;
#	else
	int handlerSwitchValue;
	const unsigned char* actionRecord;
	const unsigned char* languageSpecificData;
	_Unwind_Ptr catchTemp;
	void* adjustedPtr;
#	endif
	_Unwind_Exception unwindHeader;
};

struct __cxa_refcounted_exception
{
	::_Atomic_word referenceCount;
	__cxa_exception exc;
};

struct __cxa_dependent_exception
{
	void* primaryException;
#	if __GNUC__ * 100 + __GNUC_MINOR__ >= 409
	void(_GLIBCXX_CDTOR_CALLABI*__padding)(void*);
#	endif
	std::unexpected_handler unexpectedHandler;
	std::terminate_handler terminateHandler;
	__cxa_exception* nextException;
	int handlerCount;
#	ifdef __ARM_EABI_UNWINDER__
	__cxa_exception* nextPropagatingException;
	int propagationCount;
#	else
	int handlerSwitchValue;
	const unsigned char* actionRecord;
	const unsigned char* languageSpecificData;
	_Unwind_Ptr catchTemp;
	void* adjustedPtr;
#	endif
	_Unwind_Exception unwindHeader;
};

struct __cxa_eh_globals
{
	__cxa_exception* caughtExceptions;
	unsigned int uncaughtExceptions;
#	ifdef __ARM_EABI_UNWINDER__
	__cxa_exception* propagatingExceptions;
#	endif
};

extern void
__terminate(std::terminate_handler) noexcept __attribute__((__noreturn__));


namespace
{

inline __cxa_exception*
__get_exception_header_from_obj(void* ptr)
{
	return reinterpret_cast<__cxa_exception*>(ptr) - 1;
}

inline __cxa_refcounted_exception*
__get_refcounted_exception_header_from_obj(void* ptr)
{
	return reinterpret_cast<__cxa_refcounted_exception*>(ptr) - 1;
}

//! \since build 715
inline __cxa_refcounted_exception*
__get_refcounted_exception_header_from_ue(_Unwind_Exception* exc)
{
	return reinterpret_cast<__cxa_refcounted_exception*>(exc + 1) - 1;
}

inline __cxa_dependent_exception*
__get_dependent_exception_from_ue(_Unwind_Exception* exc)
{
	return reinterpret_cast<__cxa_dependent_exception*>(exc + 1) - 1;
}

// NOTE: This should be safe when %_Unwind_Exception_Class is %::uint8_t.
#	if defined(__ARM_EABI_UNWINDER__) && !defined(__CLANG_UNWIND_H)
inline bool
__is_gxx_exception_class(_Unwind_Exception_Class c)
{
	// TODO: Deferred. Take advantage of the fact that 'c' will always be
	//	word-aligned.
	return c[0] == 'G' && c[1] == 'N' && c[2] == 'U' && c[3] == 'C'
		&& c[4] == 'C' && c[5] == '+' && c[6] == '+'
		&& (c[7] == '\0' || c[7] == '\x01');
}

inline bool
__is_dependent_exception(_Unwind_Exception_Class c)
{
	return c[7] == '\x01';
}

//! \since build 715
static inline void
__GXX_INIT_PRIMARY_EXCEPTION_CLASS(_Unwind_Exception_Class c)
{
	c[0] = 'G';
	c[1] = 'N';
	c[2] = 'U';
	c[3] = 'C';
	c[4] = 'C';
	c[5] = '+';
	c[6] = '+';
	c[7] = '\0';
}

inline void
__GXX_INIT_DEPENDENT_EXCEPTION_CLASS(_Unwind_Exception_Class c)
{
	c[0] = 'G';
	c[1] = 'N';
	c[2] = 'U';
	c[3] = 'C';
	c[4] = 'C';
	c[5] = '+';
	c[6] = '+';
	c[7] = '\x01';
}
#	else
const _Unwind_Exception_Class __gxx_primary_exception_class
	= (((((((_Unwind_Exception_Class('G')
	<< 8 | _Unwind_Exception_Class('N'))
	<< 8 | _Unwind_Exception_Class('U'))
	<< 8 | _Unwind_Exception_Class('C'))
	<< 8 | _Unwind_Exception_Class('C'))
	<< 8 | _Unwind_Exception_Class('+'))
	<< 8 | _Unwind_Exception_Class('+'))
	<< 8 | _Unwind_Exception_Class('\0'));

const _Unwind_Exception_Class __gxx_dependent_exception_class
	= (((((((_Unwind_Exception_Class('G')
	<< 8 | _Unwind_Exception_Class('N'))
	<< 8 | _Unwind_Exception_Class('U'))
	<< 8 | _Unwind_Exception_Class('C'))
	<< 8 | _Unwind_Exception_Class('C'))
	<< 8 | _Unwind_Exception_Class('+'))
	<< 8 | _Unwind_Exception_Class('+'))
	<< 8 | _Unwind_Exception_Class('\x01'));

inline bool
__is_gxx_exception_class(_Unwind_Exception_Class c)
{
	return c == __gxx_primary_exception_class
		|| c == __gxx_dependent_exception_class;
}

inline bool
__is_dependent_exception(_Unwind_Exception_Class c)
{
	return c & 1;
}

//! \since build 715
#		define __GXX_INIT_PRIMARY_EXCEPTION_CLASS(c) \
	c = __gxx_primary_exception_class
#		define __GXX_INIT_DEPENDENT_EXCEPTION_CLASS(c) \
	c = __gxx_dependent_exception_class
#	endif

inline void*
__get_object_from_ue(_Unwind_Exception* eo) noexcept
{
	return __is_dependent_exception(eo->exception_class) ?
		__get_dependent_exception_from_ue(eo)->primaryException : eo + 1;
}

inline void*
__get_object_from_ambiguous_exception(__cxa_exception* p_or_d) noexcept
{
	return __get_object_from_ue(&p_or_d->unwindHeader);
}

template<typename Ex>
constexpr
std::size_t unwindhdr()
{
	return offsetof(Ex, unwindHeader);
}

#	if __GNUC__ * 100 + __GNUC_MINOR__ >= 409
template<typename Ex>
constexpr
std::size_t termHandler()
{
	return unwindhdr<Ex>() - offsetof(Ex, terminateHandler);
}

static_assert(termHandler<__cxa_exception>()
	== termHandler<__cxa_dependent_exception>(),
	"__cxa_dependent_exception::termHandler layout must be"
	" consistent with __cxa_exception::termHandler");
#	endif

#	ifndef __ARM_EABI_UNWINDER__
template<typename Ex>
constexpr std::ptrdiff_t adjptr()
{
	return unwindhdr<Ex>() - offsetof(Ex, adjustedPtr);
}

static_assert(adjptr<__cxa_exception>()
	== adjptr<__cxa_dependent_exception>(),
	"__cxa_dependent_exception::adjustedPtr layout must be"
	" consistent with __cxa_exception::adjustedPtr");
#	endif

// FIXME: For platforms with threading but no atomic builtins, e.g. Android
//	ARMv5.
#	if !_GLIBCXX_HAS_GTHREADS || __clang__
template<typename _type>
inline _type
__atomic_add_fetch_(_type* ptr, _type val, int)
{
	*ptr += val;
	return *ptr;
}

template<typename _type>
inline _type
__atomic_sub_fetch_(_type* ptr, _type val, int)
{
	*ptr -= val;
	return *ptr;
}
#		define __atomic_add_fetch __atomic_add_fetch_
#		define __atomic_sub_fetch __atomic_sub_fetch_
#	endif

void
__gxx_dependent_exception_cleanup(_Unwind_Reason_Code code,
	_Unwind_Exception* exc)
{
	const auto dep(__get_dependent_exception_from_ue(exc));
	const auto header(
		__get_refcounted_exception_header_from_obj(dep->primaryException));

	if(code != _URC_FOREIGN_EXCEPTION_CAUGHT && code != _URC_NO_REASON)
		__terminate(header->exc.terminateHandler);
	__cxa_free_dependent_exception(dep);
	if(__atomic_sub_fetch(&header->referenceCount, 1, __ATOMIC_ACQ_REL) == 0)
	{
		if(header->exc.exceptionDestructor)
			header->exc.exceptionDestructor(header + 1);
		__cxa_free_exception(header + 1);
	}
}

//! \since build 715
void
__gxx_exception_cleanup(_Unwind_Reason_Code code, _Unwind_Exception* exc)
{
	const auto header(__get_refcounted_exception_header_from_ue(exc));

	if(code != _URC_FOREIGN_EXCEPTION_CAUGHT && code != _URC_NO_REASON)
		__terminate(header->exc.terminateHandler);

	if(__atomic_sub_fetch(&header->referenceCount, 1, __ATOMIC_ACQ_REL) == 0)
	{
		if(header->exc.exceptionDestructor)
			header->exc.exceptionDestructor(header + 1);

		__cxa_free_exception(header + 1);
	}
}

} // unnamed namespace;

} // namespace __cxxabiv1;

using namespace __cxxabiv1;

#	pragma GCC visibility pop

namespace std
{

namespace __exception_ptr
{

exception_ptr::exception_ptr() noexcept
	: _M_exception_object()
{}
exception_ptr::exception_ptr(void* obj) noexcept
	: _M_exception_object(obj)
{
	// NOTE: Need to hack here because pre-built code in
	//	%__cxxabiv1::__cxa_throw or transactional memory extension
	//	may assume it is not reference-counted so it would crash on %_M_release
	//	or %std::rethrow_exception called after the reference has been cleanup.
	const auto header(__get_refcounted_exception_header_from_obj(obj));

	header->exc.unwindHeader.exception_cleanup = __gxx_exception_cleanup;
	_M_addref();
}
#	ifdef _GLIBCXX_EH_PTR_COMPAT
exception_ptr::exception_ptr(__safe_bool) noexcept
	: _M_exception_object()
{}
#	endif
exception_ptr::exception_ptr(const exception_ptr& other) noexcept
	: _M_exception_object(other._M_exception_object)
{
	_M_addref();
}
exception_ptr::~exception_ptr() noexcept
{
	_M_release();
}

exception_ptr&
exception_ptr::operator=(const exception_ptr& other) noexcept
{
	exception_ptr(other).swap(*this);
	return *this;
}

void
exception_ptr::_M_addref() noexcept
{
	if(_M_exception_object)
	{
		const auto
			eh(__get_refcounted_exception_header_from_obj(_M_exception_object));

		__atomic_add_fetch(&eh->referenceCount, 1, __ATOMIC_ACQ_REL);
	}
}

void
exception_ptr::_M_release() noexcept
{
	if(_M_exception_object)
	{
		const auto eh(
			__get_refcounted_exception_header_from_obj(_M_exception_object));

		if(__atomic_sub_fetch(&eh->referenceCount, 1, __ATOMIC_ACQ_REL) == 0)
		{
			if(eh->exc.exceptionDestructor)
				eh->exc.exceptionDestructor(_M_exception_object);
			__cxa_free_exception(_M_exception_object);
			_M_exception_object = {};
		}
	}
}

void*
exception_ptr::_M_get() const noexcept
{
	return _M_exception_object;
}

void
exception_ptr::swap(exception_ptr& other) noexcept
{
	const auto tmp(_M_exception_object);

	_M_exception_object = other._M_exception_object;
	other._M_exception_object = tmp;
}


#	ifdef _GLIBCXX_EH_PTR_COMPAT
void
exception_ptr::_M_safe_bool_dummy() noexcept
{}

bool
exception_ptr::operator!() const noexcept
{
	return !_M_exception_object;
}

exception_ptr::operator __safe_bool() const noexcept
{
	return _M_exception_object ? &exception_ptr::_M_safe_bool_dummy : nullptr;
}
#	endif

const type_info*
exception_ptr::__cxa_exception_type() const noexcept
{
	return __get_exception_header_from_obj(_M_exception_object)->exceptionType;
}

bool
operator==(const exception_ptr& lhs, const exception_ptr& rhs) noexcept
{
	return lhs._M_exception_object == rhs._M_exception_object;
}

bool
operator!=(const exception_ptr& lhs, const exception_ptr& rhs) noexcept
{
	return !(lhs == rhs);
}

} // namespace __exception_ptr;

exception_ptr
current_exception() noexcept
{
	const auto header(__cxa_get_globals()->caughtExceptions);

	return header && __is_gxx_exception_class(
		header->unwindHeader.exception_class)
		? exception_ptr(__get_object_from_ambiguous_exception(header))
		: exception_ptr();
}

void
rethrow_exception(exception_ptr ep)
{
	void* obj(ep._M_get());
	const auto eh(__get_refcounted_exception_header_from_obj(obj));
	const auto dep(__cxa_allocate_dependent_exception());

	dep->primaryException = obj;
	__atomic_add_fetch(&eh->referenceCount, 1, __ATOMIC_ACQ_REL);
	dep->unexpectedHandler = get_unexpected();
	dep->terminateHandler = get_terminate();
	__GXX_INIT_DEPENDENT_EXCEPTION_CLASS(dep->unwindHeader.exception_class);
	dep->unwindHeader.exception_cleanup = __gxx_dependent_exception_cleanup;
#ifdef _GLIBCXX_SJLJ_EXCEPTIONS
	_Unwind_SjLj_RaiseException(&dep->unwindHeader);
#else
	_Unwind_RaiseException(&dep->unwindHeader);
#endif
	__cxa_begin_catch(&dep->unwindHeader);
	std::terminate();
}

} // namespace std;

#endif

// NOTE: See header <libdefect/exception.h>.
#if defined(__GLIBCXX__) \
	&& (defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L) \
	&& __GNUC__ * 100 + __GNUC_MINOR__ < 409 && !defined(__clang__)

#	include <exception>
#	if ATOMIC_POINTER_LOCK_FREE < 2
// XXX: Reordered to avoid include <exception> too early.
#		include <ext/concurrence.h>
namespace
{

__gnu_cxx::__mutex mx;

} // unnamed namespace;
#	endif

namespace __cxxabiv1
{

extern std::terminate_handler __terminate_handler;
extern std::unexpected_handler __unexpected_handler;

} // namespace __cxxabiv1;

using namespace __cxxabiv1;

namespace std
{

terminate_handler
get_terminate() noexcept
{
	terminate_handler func;

#	if ATOMIC_POINTER_LOCK_FREE > 1
	__atomic_load(&__terminate_handler, &func, __ATOMIC_ACQUIRE);
#	else
	__gnu_cxx::__scoped_lock l(mx);

	func = __terminate_handler;
#	endif
	return func;
}

unexpected_handler
get_unexpected() noexcept
{
	unexpected_handler func;

#	if ATOMIC_POINTER_LOCK_FREE > 1
	__atomic_load(&__unexpected_handler, &func, __ATOMIC_ACQUIRE);
#	else
	__gnu_cxx::__scoped_lock l(mx);

	func = __unexpected_handler;
#	endif
	return func;
}

} // namespace std;

#endif

