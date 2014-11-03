/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file pseudo_mutex.hpp
\ingroup YStandardEx
\brief 伪互斥量。
\version r462
\author FrankHB <frankhb1989@gmail.com>
\since build 520
\par 创建时间:
	2014-11-03 13:53:34 +0800
\par 修改时间:
	2014-11-04 00:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::PseudoMutex
*/


#ifndef YB_INC_ystdex_pseudo_mutex_h_
#define YB_INC_ystdex_pseudo_mutex_h_ 1

#include "../ydef.h"
#include <system_error> // for std::errc, std::system_error;

namespace ystdex
{

/*!
\brief 单线程操作：保证单线程环境下接口及符合对应的 \c std 命名空间下的接口。
\note 不包含本机类型相关的接口。
\warning 多线程环境下可能引起未定义行为。
\since build 550
\todo 添加 ISO C++ 14 共享锁。
*/
namespace single_thread
{

//! \since build 550
//@{
struct defer_lock_t
{};

struct try_to_lock_t
{};

struct adopt_lock_t
{};

yconstexpr defer_lock_t defer_lock{};
yconstexpr try_to_lock_t try_to_lock{};
yconstexpr adopt_lock_t adopt_lock{};


//! \see ISO C++11 [thread.mutex.requirements.mutex] 。
//@{
class YB_API mutex
{
public:
	yconstfn
	mutex() yimpl(= default);
	mutex(const mutex&) = delete;
	~mutex() yimpl(= default);

	mutex&
	operator=(const mutex&) = delete;

	//! \pre 调用线程不持有锁。
	void
	lock()
	{}

	bool
	try_lock()
	{
		return true;
	}

	//! \pre 调用线程持有锁。
	void
	unlock()
	{}
};


class YB_API recursive_mutex
{
public:
	recursive_mutex() yimpl(= default);
	recursive_mutex(const recursive_mutex&) = delete;
	~recursive_mutex() yimpl(= default);

	recursive_mutex&
	operator=(const recursive_mutex&) = delete;

	void
	lock()
	{}

	bool
	try_lock()
	{
		return true;
	}

	//! \pre 调用线程持有锁。
	void
	unlock()
	{}
};


//! \see ISO C++11 [thread.timedmutex.requirements] 。
//@{
class YB_API timed_mutex
{
public:
	timed_mutex() yimpl(= default);
	timed_mutex(const timed_mutex&) = delete;
	~timed_mutex() yimpl(= default);

	timed_mutex&
	operator=(const timed_mutex&) = delete;

	//! \pre 调用线程不持有锁。
	//@{
	void
	lock()
	{}

	bool
	try_lock()
	{
		return true;
	}

	template<typename _tRep, typename _tPeriod>
	bool
	try_lock_for(const std::chrono::duration<_tRep, _tPeriod>&)
	{
		return true;
	}

	template<typename _tClock, typename _tDuration>
	bool
	try_lock_until(const std::chrono::time_point<_tClock, _tDuration>&)
	{
		return true;
	}
	//@}

	//! \pre 调用线程持有锁。
	void
	unlock()
	{}
};


class YB_API recursive_timed_mutex
{
public:
	recursive_timed_mutex() yimpl(= default);
	recursive_timed_mutex(const recursive_timed_mutex&) = delete;
	~recursive_timed_mutex() yimpl(= default);

	recursive_timed_mutex&
	operator=(const recursive_timed_mutex&) = delete;

	void
	lock()
	{}

	bool
	try_lock()
	{
		return true;
	}

	template<typename _tRep, typename _tPeriod>
	bool
	try_lock_for(const std::chrono::duration<_tRep, _tPeriod>&)
	{
		return true;
	}

	template<typename _tClock, typename _tDuration>
	bool
	try_lock_until(const std::chrono::time_point<_tClock, _tDuration>&)
	{
		return true;
	}

	//! \pre 调用线程持有锁。
	void
	unlock()
	{}
};
//@}
//@}


template<class _tMutex>
class lock_guard
{
private:
	mutex_type& pm;

public:
	using mutex_type = _tMutex;

	/*!
	\pre 若 mutex_type 非递归锁，调用线程不持有锁。
	\post <tt>pm == &m</tt> 。
	*/
	explicit
	lock_guard(mutex_type& m)
		: pm(&m)
	{
		m.lock();
	}
	/*!
	\pre 调用线程持有锁。
	\post <tt>pm == &m</tt> 。
	*/
	lock_guard(mutex_type& m, adopt_lock_t)
		: pm(&m)
	{}
	lock_guard(const lock_guard&) = delete;
	~lock_guard()
	{
		pm.unlock();
	}

	lock_guard&
	operator=(const lock_guard&) = delete;
};


template<class _tMutex>
class unique_lock
{
private:
	mutex_type* pm;
	bool owns;

public:
	using mutex_type = _tMutex;

	unique_lock() ynothrow
		: pm(), owns()
	{}

	//! \post <tt>pm == &m</tt> 。
	//@{
	explicit
	unique_lock(mutex_type& m)
		: unique_lock(defer_lock)
	{
		lock();
		owns = true;
	}
	unique_lock(mutex_type& m, defer_lock_t) ynothrow
		: pm(&m), owns()
	{}
	/*!
	\pre mutex_type 满足 Lockable 要求。
	\pre 若 mutex_type 非递归锁，调用线程不持有锁。
	*/
	unique_lock(mutex_type& m, try_to_lock_t)
		: pm(&m), owns(pm->try_lock())
	{}
	//! \pre 调用线程持有锁。
	unique_lock(mutex_type& m, adopt_lock_t)
		: pm(&m), owns(true)
	{}
	/*!
	\pre mutex_type 满足 TimedLockable 要求。
	\pre 若 mutex_type 非递归锁，调用线程不持有锁。
	*/
	//@{
	template<typename _tClock, typename _tDuration>
	unique_lock(mutex_type& m,
		const std::chrono::time_point<_tClock, _tDuration>& abs_time)
		: pm(&m), owns(pm->try_lock_until(abs_time))
	{}
	template<typename _tRep, typename _tPeriod>
	unique_lock(mutex_type& m,
		const std::chrono::duration<_tRep, _tPeriod>& rel_time)
		: pm(&m), owns(pm->try_lock_for(rel_time))
	{}
	//@}
	//@}
	unique_lock(const unique_lock&) = delete;
	/*!
	\post <tt>pm == u_p.pm && owns == u_p.owns</tt> 当 \c u_p 是 u 之前的状态。
	\post <tt>!u.pm && !u.owns</tt> 。
	*/
	unique_lock(unique_lock&& u) ynothrow
		: pm(u.pm), owns(u.owns)
	{
		yunseq(u.pm = {}, u.owns = {});
	}
	~unique_lock()
	{
		if(owns)
			unlock();
	}

	unique_lock&
	operator=(const unique_lock&) = delete;
	/*!
	\post <tt>pm == u_p.pm && owns == u_p.owns</tt> 当 \c u_p 是 u 之前的状态。
	\post <tt>!u.pm && !u.owns</tt> 。
	\see http://wg21.cmeerw.net/lwg/issue2104 。
	*/
	unique_lock&
	operator=(unique_lock&& u)
	{
		if(owns)
			unlock();
		unique_lock(std::move(u)).swap(*this);
		u.clear_members();
		return *this;
	}

	explicit
	operator bool() const ynothrow
	{
		return owns;
	}

private:
	void
	check_lock()
	{
		if(!pm)
			throw std::system_error(errc::operation_not_permitted);
		if(!owns)
			throw std::system_error(errc::resource_deadlock_would_occur);
	}

	void
	clear_members()
	{
		yunseq(pm = {}, owns = {});
	}

public:
	void
	lock()
	{
		if(!pm)
			throw std::system_error(errc::operation_not_permitted);
		if(!owns)
			throw std::system_error(errc::resource_deadlock_would_occur);
		pm->lock();
		owns = true;
	}

	bool
	owns_lock() const ynothrow
	{
		return owns;
	}

	mutex_type*
	release() ynothrow
	{
		const auto res(pm);

		clear_members();
		return res;
	}

	void
	swap(unique_lock& u) ynothrow
	{
		std::swap(pm, u.pm),
		std::swap(owns, u.owns);
	}

	//! \pre mutex_type 满足 Lockable 要求。
	bool
	try_lock()
	{
		check_lock();
		return owns = pm->lock();
	}

	//! \pre mutex_type 满足 TimedLockable 要求。
	//@{
	template<typename _tRep, typename _tPeriod>
	bool
	try_lock_for(const std::chrono::duration<_tRep, _tPeriod>& rel_time)
	{
		check_lock();
		return owns = pm->try_lock_for(rel_time);
	}

	template<typename _tClock, typename _tDuration>
	bool
	try_lock_until(const std::chrono::time_point<_tClock, _tDuration>& abs_time)
	{
		check_lock();
		return owns = pm->try_lock_until(abs_time);
	}
	//@}

	void
	unlock()
	{
		if(!owns)
			throw std::system_error(errc::operation_not_permitted);
		if(pm)
		{
			pm->unlock();
			owns = {};
		}
	}

	mutex_type*
	mutex() const ynothrow
	{
		return pm;
	}
};

//! \relates unique_lock
template<class _tMutex>
inline void
swap(unique_lock<_tMutex>& x, unique_lock<_tMutex>& y) ynothrow
{
	x.swap(y);
}


template<class _tLock1, class _tLock2, class... _tLocks>
void
lock(_tLock1&&, _tLock2&&, _tLocks&&...)
{}

template<class _tLock1, class _tLock2, class... _tLocks>
yconstfn int
try_lock(_tLock1&&, _tLock2&&, _tLocks&&...)
{
	return -1;
}
//@}

} // namespace single_thread;

} // namespace ystdex;

#endif

