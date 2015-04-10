/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file pseudo_mutex.h
\ingroup YStandardEx
\brief 伪互斥量。
\version r656
\author FrankHB <frankhb1989@gmail.com>
\since build 550
\par 创建时间:
	2014-11-03 13:53:34 +0800
\par 修改时间:
	2015-04-04 03:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::PseudoMutex
*/


#ifndef YB_INC_ystdex_pseudo_mutex_h_
#define YB_INC_ystdex_pseudo_mutex_h_ 1

#include "memory.hpp" // for ydef.h, ystdex::bound_delete, std::declval;
#include "base.h" // for ystdex::noncopyable, ystdex::nonmovable;
#include <chrono>
#include <system_error> // for std::errc, std::system_error;

namespace ystdex
{

/*!
\brief 单线程操作：保证单线程环境下接口及符合对应的 \c std 命名空间下的接口。
\note 不包含本机类型相关的接口。
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


//! \warning 不保证线程安全：多线程环境下假定线程同步语义可能引起未定义行为。
//@{
//! \see ISO C++11 [thread.mutex.requirements.mutex] 。
//@{
class YB_API mutex : private yimpl(noncopyable), private yimpl(nonmovable)
{
public:
	yconstfn
	mutex() yimpl(= default);
	~mutex() yimpl(= default);

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
	: private yimpl(noncopyable), private yimpl(nonmovable)
{
public:
	recursive_mutex() yimpl(= default);
	~recursive_mutex() yimpl(= default);

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
class YB_API timed_mutex : private yimpl(noncopyable), private yimpl(nonmovable)
{
public:
	timed_mutex() yimpl(= default);
	~timed_mutex() yimpl(= default);

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
	: private yimpl(noncopyable), private yimpl(nonmovable)
{
public:
	recursive_timed_mutex() yimpl(= default);
	~recursive_timed_mutex() yimpl(= default);

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
class lock_guard : private yimpl(noncopyable), private yimpl(nonmovable)
{
public:
	using mutex_type = _tMutex;

#ifdef NDEBUG
	explicit
	lock_guard(mutex_type&)
	{}
	lock_guard(mutex_type&, adopt_lock_t)
	{}

#else
private:
	mutex_type& pm;

public:
	/*!
	\pre 若 mutex_type 非递归锁，调用线程不持有锁。
	\post <tt>pm == &m</tt> 。
	*/
	explicit
	lock_guard(mutex_type& m)
		: pm(m)
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
	~lock_guard()
	{
		pm.unlock();
	}
#endif
};


//! \note 定义宏 \c NDEBUG 时不进行检查，优化实现为空操作。
//@{
template<class _tMutex>
class unique_lock : private yimpl(noncopyable)
{
public:
	using mutex_type = _tMutex;

#ifdef NDEBUG
	//! \since build 551
	//@{
	unique_lock() yimpl(= default);
	explicit
	unique_lock(mutex_type&)
	{}
	unique_lock(mutex_type&, defer_lock_t)
	{}
	unique_lock(mutex_type&, try_to_lock_t)
	{}
	unique_lock(mutex_type&, adopt_lock_t)
	{}
	template<typename _tClock, typename _tDuration>
	unique_lock(mutex_type&,
		const std::chrono::time_point<_tClock, _tDuration>&)
	{}
	template<typename _tRep, typename _tPeriod>
	unique_lock(mutex_type&, const std::chrono::duration<_tRep, _tPeriod>&)
	{}
	unique_lock(unique_lock&&) yimpl(= default);

	explicit
	operator bool() const ynothrow
	{
		return true;
	}

	void
	lock()
	{}

	bool
	owns_lock() const ynothrow
	{
		return true;
	}

	mutex_type*
	release() ynothrow
	{
		return {};
	}

	void
	swap(unique_lock&) ynothrow
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

	void
	unlock()
	{}

	mutex_type*
	mutex() const ynothrow
	{
		return {};
	}
	//@}
#else
private:
	mutex_type* pm;
	bool owns;

public:
	unique_lock() ynothrow
		: pm(), owns()
	{}

	//! \post <tt>pm == &m</tt> 。
	//@{
	explicit
	unique_lock(mutex_type& m)
		: unique_lock(m, defer_lock)
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
		using namespace std;

		if(!pm)
			throw system_error(int(errc::operation_not_permitted),
				generic_category());
		if(owns)
			throw system_error(int(errc::resource_deadlock_would_occur),
				generic_category());
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
		check_lock();
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
			throw std::system_error(int(std::errc::operation_not_permitted),
				std::generic_category());
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
#endif
};
//@}

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
//@}

} // namespace single_thread;


/*!
\brief 在单线程环境和多线程环境下都可用的线程同步接口。
\since build 551
*/
namespace threading
{

/*!
\brief 解锁删除器。
\pre _tMutex 满足 \c BasicLockable 要求。
\since build 590
*/
template<class _tMutex = single_thread::mutex,
	class _tLock = single_thread::unique_lock<_tMutex>>
class unlock_delete : private noncopyable
{
public:
	using mutex_type = _tMutex;
	using lock_type = _tLock;

	mutable lock_type lock;

	unlock_delete(mutex_type& mtx)
		: lock(mtx)
	{}
	template<typename
		= yimpl(enable_if_t)<is_nothrow_move_constructible<lock_type>::value>>
	unlock_delete(lock_type&& lck) ynothrow
		: lock(std::move(lck))
	{}
	template<typename... _tParams>
	unlock_delete(mutex_type& mtx, _tParams&&... args) ynoexcept(
		std::declval<mutex_type&>()(std::declval<_tParams&&>()...))
		: lock(mtx, yforward(args)...)
	{}

	//! \brief 删除：解锁。
	template<typename _tPointer>
	void
	operator()(const _tPointer&) const ynothrow
	{
		lock.unlock();
	}
};


/*!
\brief 独占所有权的锁定指针。
\since build 551
*/
template<typename _type, class _tMutex = single_thread::mutex,
	class _tLock = single_thread::unique_lock<_tMutex>>
using locked_ptr = std::unique_ptr<_type, unlock_delete<_tMutex, _tLock>>;

} // namespace threading;

} // namespace ystdex;

#endif

