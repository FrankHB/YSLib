/*
	© 2014-2016, 2019, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file pseudo_mutex.h
\ingroup YStandardEx
\brief 伪互斥量。
\version r1648
\author FrankHB <frankhb1989@gmail.com>
\since build 550
\par 创建时间:
	2014-11-03 13:53:34 +0800
\par 修改时间:
	2021-09-24 02:43 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::PseudoMutex
*/


#ifndef YB_INC_ystdex_pseudo_mutex_h_
#define YB_INC_ystdex_pseudo_mutex_h_ 1

#include "ref.hpp" // for lref, std::declval;
#include "base.h" // for noncopyable, nonmovable;
#include <chrono> // for std::chrono::duration, std::chrono::time_point;
#include "exception.h" // for std::addressof, throw_error, std::errc;

namespace ystdex
{

/*!
\brief 在单线程环境和多线程环境下都可用的线程同步接口。
\since build 551
*/
namespace threading
{

/*!
\see ISO C++14 30.4.2[thread.lock] 。
\since build 550
*/
//@{
yconstexpr_inline const struct defer_lock_t{} defer_lock{};
yconstexpr_inline const struct try_to_lock_t{} try_to_lock{};
yconstexpr_inline const struct adopt_lock_t{} adopt_lock{};
//@}

/*!
\note 第一模板参数不需要保证满足 BasicLockable 要求。
\warning 非虚析构。
\since build 723
*/
//@{
//! \pre _tReference 值的 get() 返回的类型满足 BasicLockable 要求。
template<class _type, typename _tReference = lref<_type>>
class lockable_adaptor
{
public:
	//! \since build 722
	using holder_type = _type;
	using reference = _tReference;

private:
	reference ref;

public:
	//! \since build 722
	//@{
	lockable_adaptor(holder_type& m) ynothrow
		: ref(m)
	{}

	explicit
	operator holder_type&() ynothrow
	{
		return ref;
	}
	explicit
	operator const holder_type&() const ynothrow
	{
		return ref;
	}

	void
	lock()
	{
		ref.get().lock();
	}

	//! \pre holder_type 满足 Lockable 要求。
	bool
	try_lock()
	{
		return ref.get().try_lock();
	}

	//! \pre holder_type 满足 TimedLockable 要求。
	//@{
	template<typename _tRep, typename _tPeriod>
	bool
	try_lock_for(const std::chrono::duration<_tRep, _tPeriod>& rel_time)
	{
		return ref.get().try_lock_for(rel_time);
	}

	template<typename _tClock, typename _tDuration>
	bool
	try_lock_until(const std::chrono::time_point<_tClock, _tDuration>& abs_time)
	{
		return ref.get().try_lock_until(abs_time);
	}
	//@}

	void
	unlock() ynothrowv
	{
		ref.get().unlock();
	}
	//@}
};


//! \pre _tReference 值的 get() 返回的类型满足共享 BasicLockable 要求。
template<class _type, typename _tReference = lref<_type>>
class shared_lockable_adaptor
{
public:
	//! \since build 722
	using holder_type = _type;
	using reference = _tReference;

private:
	reference ref;

public:
	shared_lockable_adaptor(holder_type& m) ynothrow
		: ref(m)
	{}

	explicit
	operator holder_type&() ynothrow
	{
		return ref;
	}
	explicit
	operator const holder_type&() const ynothrow
	{
		return ref;
	}

	//! \since build 723
	//@{
	void
	lock()
	{
		ref.get().lock_shared();
	}

	//! \pre holder_type 满足 Lockable 要求。
	bool
	try_lock()
	{
		return ref.get().try_lock_shared();
	}

	//! \pre holder_type 满足 TimedLockable 要求。
	//@{
	template<typename _tRep, typename _tPeriod>
	bool
	try_lock_for(const std::chrono::duration<_tRep, _tPeriod>& rel_time)
	{
		return ref.get().try_lock_shared_for(rel_time);
	}

	template<typename _tClock, typename _tDuration>
	bool
	try_lock_until(const std::chrono::time_point<_tClock, _tDuration>& abs_time)
	{
		return ref.get().try_lock_shared_until(abs_time);
	}
	//@}

	void
	unlock() ynothrow
	{
		ref.get().unlock_shared();
	}
	//@}
};
//@}


//! \since build 722
//@{
/*!
\note 第一参数为存储的互斥量类型。
\note 第二参数决定是否进行检查，不检查时优化实现为空操作。
\warning 不检查时不保证线程安全：多线程环境下假定线程同步语义可能引起未定义行为。
*/
//@{
/*!
\pre _tReference 满足 BasicLockable 要求。
\pre 静态断言： _tReference 使用 _tMutex 左值初始化保证无异常抛出。
\note 第三参数为调用的引用类型。
\warning 非虚析构。
*/
//@{
template<class _tMutex, bool = true, typename _tReference = _tMutex&>
class lock_guard : private yimpl(noncopyable), private yimpl(nonmovable)
{
	ynoexcept_assert("Invalid type found.",
		_tReference(std::declval<_tMutex&>()));

public:
	using mutex_type = _tMutex;
	using reference = _tReference;

private:
	mutex_type& owned;

public:
	/*!
	\pre 若 mutex_type 非递归锁，调用线程不持有锁。
	\post <tt>std::addressof(owned) == std::addressof(m)</tt> 。
	*/
	explicit
	lock_guard(mutex_type& m)
		: owned(m)
	{
		reference(owned).lock();
	}
	/*!
	\pre 调用线程持有锁。
	\post <tt>std::addressof(owned) == std::addressof(m)</tt> 。
	*/
	lock_guard(mutex_type& m, adopt_lock_t) yimpl(ynothrow)
		: owned(m)
	{}
	~lock_guard()
	{
		reference(owned).unlock();
	}
};

template<class _tMutex, typename _tReference>
class lock_guard<_tMutex, false, _tReference>
	: private yimpl(noncopyable), private yimpl(nonmovable)
{
public:
	using mutex_type = _tMutex;

	explicit
	lock_guard(mutex_type&) yimpl(ynothrow)
	{}
	lock_guard(mutex_type&, adopt_lock_t) yimpl(ynothrow)
	{}
};


//! \brief 锁基类：可适配独占锁和共享锁。
//@{
template<class _tMutex, bool _bEnableCheck = true, class _tReference = _tMutex&>
class lock_base : private noncopyable
{
	ynoexcept_assert("Invalid type found.",
		_tReference(std::declval<_tMutex&>()));

public:
	using mutex_type = _tMutex;
	using reference = _tReference;

private:
	mutex_type* pm;
	bool owns;

public:
	lock_base() ynothrow
		: pm(), owns()
	{}

	//! \post <tt>pm == std::addressof(m)</tt> 。
	//@{
	explicit
	lock_base(mutex_type& m)
		: lock_base(m, defer_lock)
	{
		lock();
		owns = true;
	}
	lock_base(mutex_type& m, defer_lock_t) ynothrow
		: pm(std::addressof(m)), owns()
	{}
	/*!
	\pre mutex_type 满足 Lockable 要求。
	\pre 若 mutex_type 非递归锁，调用线程不持有锁。
	*/
	lock_base(mutex_type& m, try_to_lock_t)
		: pm(std::addressof(m)), owns(get_ref().try_lock())
	{}
	//! \pre 调用线程持有锁。
	lock_base(mutex_type& m, adopt_lock_t) yimpl(ynothrow)
		: pm(std::addressof(m)), owns(true)
	{}
	/*!
	\pre mutex_type 满足 TimedLockable 要求。
	\pre 若 mutex_type 非递归锁，调用线程不持有锁。
	*/
	//@{
	template<typename _tClock, typename _tDuration>
	lock_base(mutex_type& m, const std::chrono::time_point<_tClock,
		_tDuration>& abs_time)
		: pm(std::addressof(m)), owns(get_ref().try_lock_until(abs_time))
	{}
	template<typename _tRep, typename _tPeriod>
	lock_base(mutex_type& m,
		const std::chrono::duration<_tRep, _tPeriod>& rel_time)
		: pm(std::addressof(m)), owns(get_ref().try_lock_for(rel_time))
	{}
	//@}
	//@}
	/*!
	\post <tt>pm == u_p.pm && owns == u_p.owns</tt> 当 \c u_p 是 u 之前的状态。
	\post <tt>!u.pm && !u.owns</tt> 。
	*/
	lock_base(lock_base&& u) ynothrow
		: pm(u.pm), owns(u.owns)
	{
		yunseq(u.pm = {}, u.owns = {});
	}
	~lock_base()
	{
		if(owns)
			unlock();
	}

	/*!
	\post <tt>pm == u_p.pm && owns == u_p.owns</tt> 当 \c u_p 是 u 之前的状态。
	\post <tt>!u.pm && !u.owns</tt> 。
	\see LWG 2104 。
	*/
	lock_base&
	operator=(lock_base&& u) yimpl(ynothrow)
	{
		if(owns)
			unlock();
		swap(u, *this);
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
			throw_error(errc::operation_not_permitted);
		if(owns)
			throw_error(errc::resource_deadlock_would_occur);
	}

	void
	clear_members() yimpl(ynothrow)
	{
		yunseq(pm = {}, owns = {});
	}

	reference
	get_ref() ynothrow
	{
		yassume(pm);
		return reference(*pm);
	}

public:
	void
	lock()
	{
		check_lock();
		get_ref().lock();
		owns = true;
	}

	//! \pre reference 满足 Lockable 要求。
	bool
	try_lock()
	{
		check_lock();
		return owns = get_ref().try_lock();
	}

	//! \pre reference 满足 TimedLockable 要求。
	//@{
	template<typename _tRep, typename _tPeriod>
	bool
	try_lock_for(const std::chrono::duration<_tRep, _tPeriod>& rel_time)
	{
		check_lock();
		return owns = get_ref().try_lock_for(rel_time);
	}

	template<typename _tClock, typename _tDuration>
	bool
	try_lock_until(const std::chrono::time_point<_tClock, _tDuration>& abs_time)
	{
		check_lock();
		return owns = get_ref().try_lock_until(abs_time);
	}
	//@}

	void
	unlock()
	{
		if(!owns)
			throw_error(std::errc::operation_not_permitted);
		if(pm)
		{
			get_ref().unlock();
			owns = {};
		}
	}

	friend void
	swap(lock_base& x, lock_base& y) ynothrow
	{
		std::swap(x.pm, y.pm),
		std::swap(x.owns, y.owns);
	}

	mutex_type*
	release() ynothrow
	{
		const auto res(pm);

		clear_members();
		return res;
	}

	bool
	owns_lock() const ynothrow
	{
		return owns;
	}

	mutex_type*
	mutex() const ynothrow
	{
		return pm;
	}
};

template<class _tMutex, class _tReference>
class lock_base<_tMutex, false, _tReference>
{
public:
	using mutex_type = _tMutex;

	lock_base() ynothrow yimpl(= default);
	explicit
	lock_base(mutex_type&) yimpl(ynothrow)
	{}
	lock_base(mutex_type&, defer_lock_t) ynothrow
	{}
	lock_base(mutex_type&, try_to_lock_t) yimpl(ynothrow)
	{}
	lock_base(mutex_type&, adopt_lock_t) yimpl(ynothrow)
	{}
	template<typename _tClock, typename _tDuration>
	lock_base(mutex_type&,
		const std::chrono::time_point<_tClock, _tDuration>&) yimpl(ynothrow)
	{}
	template<typename _tRep, typename _tPeriod>
	lock_base(mutex_type&, const std::chrono::duration<_tRep, _tPeriod>&)
		yimpl(ynothrow)
	{}
	lock_base(lock_base&&) yimpl(= default);

	//! \since build 724
	lock_base&
	operator=(lock_base&&) = yimpl(default);

	explicit
	operator bool() const ynothrow
	{
		return true;
	}

	void
	lock()
	{}

	bool
	try_lock() yimpl(ynothrow)
	{
		return true;
	}

	template<typename _tRep, typename _tPeriod>
	bool
	try_lock_for(const std::chrono::duration<_tRep, _tPeriod>&) yimpl(ynothrow)
	{
		return true;
	}

	template<typename _tClock, typename _tDuration>
	bool
	try_lock_until(const std::chrono::time_point<_tClock, _tDuration>&)
		yimpl(ynothrow)
	{
		return true;
	}

	void
	unlock() yimpl(ynothrow)
	{}

	friend void
	swap(lock_base&, lock_base&) ynothrow
	{}

	mutex_type*
	release() ynothrow
	{
		return {};
	}

	bool
	owns_lock() const ynothrow
	{
		return true;
	}

	mutex_type*
	mutex() const ynothrow
	{
		return {};
	}
};
//@}
//@}


//! \warning 非虚析构。
template<class _tMutex, bool _bEnableCheck = true>
class unique_lock : private yimpl(lock_base<_tMutex, _bEnableCheck>)
{
private:
	using base = yimpl(lock_base<_tMutex, _bEnableCheck>);

public:
	using mutex_type = _tMutex;

public:
	unique_lock() ynothrow yimpl(= default);
	using yimpl(base::base);
	unique_lock(unique_lock&&) yimpl(= default);

	/*!
	\post <tt>pm == u_p.pm && owns == u_p.owns</tt> 当 \c u_p 是 u 之前的状态。
	\post <tt>!u.pm && !u.owns</tt> 。
	\see LWG 2104 。
	\since build 722
	*/
	unique_lock&
	operator=(unique_lock&&) yimpl(= default);

	using yimpl(base)::operator bool;

	using yimpl(base)::lock;

	//! \pre mutex_type 满足 Lockable 要求。
	using yimpl(base)::try_lock;

	//! \pre mutex_type 满足 TimedLockable 要求。
	//@{
	using yimpl(base)::try_lock_for;

	using yimpl(base)::try_lock_until;
	//@}

	using yimpl(base)::unlock;

	void
	swap(unique_lock& u) ynothrow
	{
		swap(static_cast<base&>(*this), static_cast<base&>(u));
	}

	using yimpl(base)::release;

	using yimpl(base)::owns_lock;

	using yimpl(base)::mutex;
};

//! \relates unique_lock
template<class _tMutex, bool _bEnableCheck>
inline void
swap(unique_lock<_tMutex, _bEnableCheck>& x,
	unique_lock<_tMutex, _bEnableCheck>& y) ynothrow
{
	x.swap(y);
}


//! \warning 非虚析构。
template<class _tMutex, bool _bEnableCheck = true>
class shared_lock : private yimpl(lock_base<_tMutex, _bEnableCheck,
	shared_lockable_adaptor<_tMutex>>)
{
private:
	using base = yimpl(lock_base<_tMutex, _bEnableCheck,
		shared_lockable_adaptor<_tMutex>>);

public:
	using mutex_type = _tMutex;

public:
	shared_lock() ynothrow yimpl(= default);
	using yimpl(base::base);
	shared_lock(shared_lock&&) yimpl(= default);

	/*!
	\post <tt>pm == u_p.pm && owns == u_p.owns</tt> 当 \c u_p 是 u 之前的状态。
	\post <tt>!u.pm && !u.owns</tt> 。
	\see LWG 2104 。
	*/
	shared_lock&
	operator=(shared_lock&&) yimpl(= default);

	using yimpl(base)::operator bool;

	//! \since build 723
	//@{
	void
	lock()
	{
		base::lock_shared();
	}

	//! \pre mutex_type 满足 Lockable 要求。
	bool
	try_lock()
	{
		return base::try_lock_shared();
	}

	//! \pre mutex_type 满足 TimedLockable 要求。
	//@{
	template<typename _tRep, typename _tPeriod>
	bool
	try_lock_shared_for(const std::chrono::duration<_tRep, _tPeriod>& rel_time)
	{
		return base::template try_lock_shared_for(rel_time);
	}

	template<typename _tClock, typename _tDuration>
	bool
	try_lock_shared_until(const
		std::chrono::time_point<_tClock, _tDuration>& abs_time)
	{
		return base::template try_lock_shared_until(abs_time);
	}
	//@}
	//@}

	using yimpl(base)::unlock;

	void
	swap(shared_lock& u) ynothrow
	{
		swap(static_cast<base&>(*this), static_cast<base&>(u));
	}

	using yimpl(base)::release;

	using yimpl(base)::owns_lock;

	using yimpl(base)::mutex;
};

//! \relates shared_lock
template<class _tMutex, bool _bEnableCheck>
inline void
swap(shared_lock<_tMutex, _bEnableCheck>& x,
	shared_lock<_tMutex, _bEnableCheck>& y) ynothrow
{
	x.swap(y);
}
//@}
//@}

} // namespace threading;

/*!
\brief 单线程操作：保证单线程环境下接口及符合对应的 std 命名空间下的接口。
\note 不包含本机类型相关的接口。
\since build 550
\todo 添加 ISO C++1z 共享锁。
*/
namespace single_thread
{

/*!
\see ISO C++14 30.4.2[thread.lock] 。
\since build 722
*/
//@{
using threading::defer_lock;
using threading::defer_lock_t;
using threading::try_to_lock;
using threading::try_to_lock_t;
using threading::adopt_lock;
using threading::adopt_lock_t;
//@}


//! \warning 不保证线程安全：多线程环境下假定线程同步语义可能引起未定义行为。
//@{
/*!
\warning 非虚析构。
\see ISO C++14 30.4.1.2[thread.mutex.requirements.mutex] 。
\see LWG 2577.
*/
//@{
class YB_API mutex : private yimpl(noncopyable), private yimpl(nonmovable)
{
public:
	yconstfn
	mutex() yimpl(= default);
	~mutex() yimpl(= default);

	//! \pre 调用线程不持有锁。
	void
	lock() yimpl(ynothrow)
	{}

	bool
	try_lock() yimpl(ynothrow)
	{
		return true;
	}

	//! \pre 调用线程持有锁。
	void
	unlock() yimpl(ynothrow)
	{}
};


class YB_API recursive_mutex
	: private yimpl(noncopyable), private yimpl(nonmovable)
{
public:
	recursive_mutex() yimpl(= default);
	~recursive_mutex() yimpl(= default);

	void
	lock() yimpl(ynothrow)
	{}

	bool
	try_lock() yimpl(ynothrow)
	{
		return true;
	}

	//! \pre 调用线程持有锁。
	void
	unlock() yimpl(ynothrow)
	{}
};


//! \see ISO C++14 30.4.1.3[thread.timedmutex.requirements] 。
//@{
class YB_API timed_mutex : private yimpl(noncopyable), private yimpl(nonmovable)
{
public:
	timed_mutex() yimpl(= default);
	~timed_mutex() yimpl(= default);

	//! \pre 调用线程不持有锁。
	//@{
	void
	lock() yimpl(ynothrow)
	{}

	bool
	try_lock() yimpl(ynothrow)
	{
		return true;
	}

	template<typename _tRep, typename _tPeriod>
	bool
	try_lock_for(const std::chrono::duration<_tRep, _tPeriod>&) yimpl(ynothrow)
	{
		return true;
	}

	template<typename _tClock, typename _tDuration>
	bool
	try_lock_until(const std::chrono::time_point<_tClock, _tDuration>&)
		yimpl(ynothrow)
	{
		return true;
	}
	//@}

	//! \pre 调用线程持有锁。
	void
	unlock() yimpl(ynothrow)
	{}
};


class YB_API recursive_timed_mutex
	: private yimpl(noncopyable), private yimpl(nonmovable)
{
public:
	recursive_timed_mutex() yimpl(= default);
	~recursive_timed_mutex() yimpl(= default);

	void
	lock() yimpl(ynothrow)
	{}

	bool
	try_lock() yimpl(ynothrow)
	{
		return true;
	}

	template<typename _tRep, typename _tPeriod>
	bool
	try_lock_for(const std::chrono::duration<_tRep, _tPeriod>&) yimpl(ynothrow)
	{
		return true;
	}

	template<typename _tClock, typename _tDuration>
	bool
	try_lock_until(const std::chrono::time_point<_tClock, _tDuration>&)
		yimpl(ynothrow)
	{
		return true;
	}

	//! \pre 调用线程持有锁。
	void
	unlock() yimpl(ynothrow)
	{}
};
//@}


/*!
\see ISO C++14 30.4.1.4[thread.sharedtimedmutex.requirements] 。
\since build 722
*/
class shared_timed_mutex : private yimpl(timed_mutex)
{
private:
	using base = yimpl(timed_mutex);

public:
	shared_timed_mutex() yimpl(= default);
	~shared_timed_mutex() yimpl(= default);

	//! \pre 调用线程不持有锁。
	//@{
	using yimpl(base)::lock;

	using yimpl(base)::try_lock;

	using yimpl(base)::try_lock_for;

	using yimpl(base)::try_lock_until;
	//@}

	//! \pre 调用线程持有锁。
	using yimpl(base)::unlock;

	//! \pre 调用线程不持有锁。
	//@{
	void
	lock_shared() yimpl(ynothrow)
	{}

	bool
	try_lock_shared() yimpl(ynothrow)
	{
		return true;
	}

	template<typename _tRep, typename _tPeriod>
	bool
	try_lock_shared_for(const std::chrono::duration<_tRep, _tPeriod>&)
		yimpl(ynothrow)
	{
		return true;
	}

	template<typename _tClock, typename _tDuration>
	bool
	try_lock_shared_until(const std::chrono::time_point<_tClock, _tDuration>&)
		yimpl(ynothrow)
	{
		return true;
	}
	//@}

	//! \pre 调用线程持有锁。
	void
	unlock_shared()
	{}
};
//@}


/*!
\note 定义宏 \c NDEBUG 时不进行检查，优化实现为空操作。
\warning 非虚析构。
\since build 722
*/
//@{
template<class _tMutex>
using lock_guard = threading::lock_guard
#ifdef NDEBUG
	<_tMutex, false>;
#else
	<_tMutex>;
#endif


template<class _tMutex>
using unique_lock = threading::unique_lock
#ifdef NDEBUG
	<_tMutex, false>;
#else
	<_tMutex>;
#endif


template<class _tMutex>
using shared_lock = threading::shared_lock
#ifdef NDEBUG
	<_tMutex, false>;
#else
	<_tMutex>;
#endif
//@}


/*!
\since build 550
\see ISO C++14 30.4.3[thread.lock.algorithm] 。
*/
//@{
template<class _tLock1, class _tLock2, class... _tLocks>
yconstfn int
try_lock(_tLock1&&, _tLock2&&, _tLocks&&...) yimpl(ynothrow)
{
	return -1;
}

template<class _tLock1, class _tLock2, class... _tLocks>
void
lock(_tLock1&&, _tLock2&&, _tLocks&&...) yimpl(ynothrow)
{}
//@}


/*!
\see ISO C++14 30.4.4[thread.once] 。
\since build 692
*/
//@{
struct YB_API once_flag : private yimpl(noncopyable), private yimpl(nonmovable)
{
	yimpl(bool) state = {};

	yconstfn
	once_flag() ynothrow yimpl(= default);
};

/*!
\brief 按标识调用函数，保证调用一次。
\note 类似 std::call_once ，但不保证线程安全性。
\note ISO C++11（至 N3691 ） 30.4 synopsis 处的声明存在错误。
\bug 未实现支持成员指针。
\see https://github.com/cplusplus/draft/issues/151 。
\see CWG 1591 。
\see CWG 2442 。

当标识为非初值时候无作用，否则调用函数。
*/
template<typename _fCallable, typename... _tParams>
inline void
call_once(once_flag& flag, _fCallable&& f, _tParams&&... args)
{
	if(!flag.state)
	{
		f(yforward(args)...);
		flag.state = true;
	}
}
//@}
//@}

} // namespace single_thread;


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
		std::declval<mutex_type&>()(std::declval<_tParams>()...))
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

