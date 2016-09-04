/*
	© 2014-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Mutex.h
\ingroup YCLib
\brief 互斥量。
\version r164
\author FrankHB <frankhb1989@gmail.com>
\since build 551
\par 创建时间:
	2014-11-04 05:17:14 +0800
\par 修改时间:
	2016-09-01 00:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Mutex
*/


#ifndef YCL_INC_Mutex_h_
#define YCL_INC_Mutex_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon
#include <ystdex/type_op.hpp> // for ystdex::cond_or_t, ystdex::or_,
//	std::is_integral, std::is_pointer;
#if YF_Multithread == 1
#	include <atomic> // for std::atomic;
#	include <ystdex/concurrency.h>
#	define YCL_Impl_Atomic_Tmpl std::atomic
#	define YCL_Impl_Ns_Mutex std
#	define YCL_Impl_Ns_Threading ystdex
#else
#	include <ystdex/pseudo_mutex.h> // for ystdex::identity_t,
//	ystdex::single_thread, ystdex::threading;
#	define YCL_Impl_Atomic_Tmpl ystdex::identity_t
#	define YCL_Impl_Ns_Mutex ystdex::single_thread
#	define YCL_Impl_Ns_Threading ystdex::threading
#endif

namespace platform
{

//! \since build 551
namespace Concurrency
{

/*!
\brief 支持的原子性的整数或指针类型。
\note 为保证可移植性，应仅使用和 std::atomic 以及基本类型兼容的操作。
\since build 725
\todo 包装基本类型。
*/
template<typename _type>
using atomic = ystdex::cond_or_t<ystdex::or_<std::is_integral<_type>,
	std::is_pointer<_type>>, void, YCL_Impl_Atomic_Tmpl, _type>;

using YCL_Impl_Ns_Mutex::mutex;
using YCL_Impl_Ns_Mutex::recursive_mutex;
// TODO: Check %_GTHREAD_USE_MUTEX_TIMEDLOCK defined or %YCL_*.
//using YCL_Impl_Ns_Mutex::timed_mutex;
//using YCL_Impl_Ns_Mutex::recursive_timed_mutex;

using YCL_Impl_Ns_Mutex::lock_guard;
using YCL_Impl_Ns_Mutex::unique_lock;
//! \since build 723
using ystdex::threading::lockable_adaptor;
//! \since build 723
using ystdex::threading::shared_lockable_adaptor;
//! \since build 722
//@{
#if YF_Multithread == 1 || !defined(NDEBUG)
yconstfn bool UseLockDebug(true);
#else
yconstfn bool UseLockDebug = {};
#endif
template<class _tMutex>
using shared_lock = ystdex::threading::shared_lock<_tMutex, UseLockDebug>;

template<class _tMutex>
using shared_lock_guard = ystdex::threading::lock_guard<_tMutex,
	UseLockDebug, shared_lockable_adaptor<_tMutex>>;
//@}

using YCL_Impl_Ns_Mutex::lock;
using YCL_Impl_Ns_Mutex::try_lock;

//! \since build 692
using YCL_Impl_Ns_Mutex::once_flag;
//! \since build 692
using YCL_Impl_Ns_Mutex::call_once;


//! \since build 723
//@{
template<class _type, typename _tReference = ystdex::lref<_type>>
using AdaptedLock = ystdex::threading::lock_base<_type, UseLockDebug,
	lockable_adaptor<_type, _tReference>>;

template<class _type, typename _tReference = ystdex::lref<_type>>
using SharedAdaptedLock = ystdex::threading::lock_base<_type, UseLockDebug,
	shared_lockable_adaptor<_type, _tReference>>;

template<class _type, typename _tReference = ystdex::lref<_type>>
using AdaptedLockGuard = ystdex::threading::lock_guard<_type, UseLockDebug,
	lockable_adaptor<_type, _tReference>>;

template<class _type, typename _tReference = ystdex::lref<_type>>
using SharedAdaptedLockGuard = ystdex::threading::lock_guard<_type,
	UseLockDebug, shared_lockable_adaptor<_type, _tReference>>;

template<class _type>
using IndirectLock = AdaptedLock<_type, ystdex::indirect_ref_adaptor<_type>>;

template<class _type>
using SharedIndirectLock
	= SharedAdaptedLock<_type, ystdex::indirect_ref_adaptor<_type>>;

template<class _type>
using IndirectLockGuard
	= AdaptedLockGuard<_type, ystdex::indirect_ref_adaptor<_type>>;

template<class _type>
using SharedIndirectLockGuard
	= SharedAdaptedLockGuard<_type, ystdex::indirect_ref_adaptor<_type>>;
//@}

} // namespace Concurrency;

//! \since build 551
namespace Threading
{

//! \since build 590
using YCL_Impl_Ns_Threading::unlock_delete;
using YCL_Impl_Ns_Threading::locked_ptr;

} // namespace Threading;

} // namespace platform;

#undef YCL_Impl_Ns_Threading
#undef YCL_Impl_Ns_Mutex

#endif

