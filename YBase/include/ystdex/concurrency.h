/*
	© 2014-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file concurrency.h
\ingroup YStandardEx
\brief 并发操作。
\version r551
\author FrankHB <frankhb1989@gmail.com>
\since build 520
\par 创建时间:
	2014-07-21 18:57:13 +0800
\par 修改时间:
	2018-09-04 15:53 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Concurrency
*/


#ifndef YB_INC_ystdex_concurrency_h_
#define YB_INC_ystdex_concurrency_h_ 1

#include "pseudo_mutex.h" // for threading::unlock_delete;
#include <mutex> // for std::mutex, std::unique_lock;
#include <thread> // for std::thread;
#include <vector> // for std::vector;
#include <queue> // for std::queue;
#include "future.hpp" // for std::packaged_task, future_result_t, pack_task;
#include <condition_variable> // for std::condition_variable;
#include "functional.hpp" // for std::bind, std::function, ystdex::invoke;
#include "cassert.h" // for yassume;

namespace ystdex
{

/*!
\brief 解锁删除器：默认使用标准库互斥量和锁。
\sa threading::unlock_delete
\since build 590
*/
template<class _tMutex = std::mutex,
	class _tLock = std::unique_lock<_tMutex>>
using unlock_delete = threading::unlock_delete<_tMutex, _tLock>;


/*!
\brief 独占所有权的锁定指针：默认使用标准库互斥量和锁。
\sa threading::locked_ptr
\since build 551
*/
template<typename _type, class _tMutex = std::mutex,
	class _tLock = std::unique_lock<_tMutex>>
using locked_ptr = threading::locked_ptr<_type, _tMutex, _tLock>;


/*!
\brief 转换 \c thread::id 为字符串表示。
\note 使用 \c operator<< 实现。
\since build 543
*/
YB_API std::string
to_string(const std::thread::id&);

/*!
\brief 取当前线程标识的字符串表示。
\sa ystdex::to_string
\since build 626
*/
inline std::string
get_this_thread_id()
{
	return ystdex::to_string(std::this_thread::get_id());
}


#	if !__GLIBCXX__ || (defined(_GLIBCXX_HAS_GTHREADS) \
	&& defined(_GLIBCXX_USE_C99_STDINT_TR1) && (ATOMIC_INT_LOCK_FREE > 1))
/*!
\brief 线程池。
\note 除非另行指定，所有公开成员函数线程安全。
\note 未控制线程队列的长度。
\since build 520
*/
class YB_API thread_pool
{
private:
	std::vector<std::thread> workers{};
	//! \since build 624
	std::queue<std::packaged_task<void()>> tasks{};
	mutable std::mutex queue_mutex{};
	std::condition_variable condition{};
	/*!
	\brief 停止状态。
	\note 仅在析构时设置停止。
	\since build 538
	*/
	bool stopped = {};

public:
	/*!
	\brief 构造：使用指定的初始化和退出回调指定数量的工作线程。
	\note 若回调为空则忽略。
	\note 线程数为 0 时无任务，不提供特别的检查。
	\warning 回调的执行不提供顺序和并发安全保证。
	\since build 543
	*/
	thread_pool(size_t, std::function<void()> = {}, std::function<void()> = {});
	/*!
	\brief 析构：设置停止状态并等待所有执行中的线程结束。
	\note 断言设置停止状态时不抛出 std::system_error 。
	\note 可能阻塞。忽略每个线程阻塞的 std::system_error 。
	\note 无异常抛出：断言。
	\since build 552
	*/
	~thread_pool() ynothrow;

	/*!
	\see wait_to_enqueue
	\since build 623
	*/
	template<typename _fCallable, typename... _tParams>
	future_result_t<_fCallable, _tParams...>
	enqueue(_fCallable&& f, _tParams&&... args)
	{
		return wait_to_enqueue([](std::unique_lock<std::mutex>&) ynothrow{
			return true;
		}, yforward(f), yforward(args)...);
	}

	size_t
	size() const;

	/*!
	\warning 非线程安全。
	\since build 552
	*/
	size_t
	size_unlocked() const ynothrow
	{
		return tasks.size();
	}

	/*!
	\brief 等待操作进入队列。
	\param f 准备进入队列的操作
	\param args 进入队列操作时的参数。
	\param waiter 等待操作。
	\pre waiter 调用后满足条件变量后置条件；断言：持有锁。
	\return 任务共享状态（若等待失败则无效）。
	\warning 需要确保未被停止（未进入析构），否则不保证任务被运行。
	\warning 使用非递归锁，等待时不能再次锁定。
	\since build 623
	*/
	template<typename _fWaiter, typename _fCallable, typename... _tParams>
	future_result_t<_fCallable, _tParams...>
	wait_to_enqueue(_fWaiter waiter, _fCallable&& f, _tParams&&... args)
	{
		auto bound(ystdex::pack_task(yforward(f), yforward(args)...));
		auto res(bound.get_future());

		{
			std::unique_lock<std::mutex> lck(queue_mutex);

			if(waiter(lck))
			{
				yassume(lck.owns_lock());
				tasks.push(std::packaged_task<void()>(std::move(bound)));
			}
			else
				return {};
		}
		condition.notify_one();
		return res;
	}
};


/*!
\brief 任务池：带有队列大小限制的线程池。
\note 除非另行指定，所有公开成员函数线程安全。
\since build 538
\todo 允许调整队列大小限制。
*/
class YB_API task_pool : private thread_pool
{
private:
	size_t max_tasks;
	std::condition_variable enqueue_condition{};

public:
	/*!
	\brief 构造：使用指定的初始化和退出回调指定数量的工作线程和最大任务数。
	\sa thread_pool::thread_pool
	\since build 543
	*/
	task_pool(size_t n, std::function<void()> on_enter = {},
		std::function<void()> on_exit = {})
		: thread_pool(std::max<size_t>(n, 1), on_enter, on_exit),
		max_tasks(std::max<size_t>(n, 1))
	{}

	//! \warning 非线程安全。
	bool
	can_enqueue_unlocked() const ynothrow
	{
		return size_unlocked() < max_tasks;
	}

	size_t
	get_max_task_num() const ynothrow
	{
		return max_tasks;
	}

	//! \since build 623
	//@{
	template<typename _func, typename _fCallable, typename... _tParams>
	future_result_t<_fCallable, _tParams...>
	poll(_func poller, _fCallable&& f, _tParams&&... args)
	{
		return wait_to_enqueue([=](std::unique_lock<std::mutex>& lck){
			enqueue_condition.wait(lck, [=]{
				return poller() && can_enqueue_unlocked();
			});
			return true;
		}, yforward(f), yforward(args)...);
	}

	template<typename _func, typename _tDuration, typename _fCallable,
		typename... _tParams>
	future_result_t<_fCallable, _tParams...>
	poll_for(_func poller, const _tDuration& duration, _fCallable&& f,
		_tParams&&... args)
	{
		return wait_to_enqueue([=](std::unique_lock<std::mutex>& lck){
			return enqueue_condition.wait_for(lck, duration, [=]{
				return poller() && can_enqueue_unlocked();
			});
		}, yforward(f), yforward(args)...);
	}

	template<typename _func, typename _tTimePoint, typename _fCallable,
		typename... _tParams>
	future_result_t<_fCallable, _tParams...>
	poll_until(_func poller, const _tTimePoint& abs_time,
		_fCallable&& f, _tParams&&... args)
	{
		return wait_to_enqueue([=](std::unique_lock<std::mutex>& lck){
			return enqueue_condition.wait_until(lck, abs_time, [=]{
				return poller() && can_enqueue_unlocked();
			});
		}, yforward(f), yforward(args)...);
	}
	//@}

	//! \note 阻塞等待当前所有任务完成后重新创建。
	//@{
	//! \brief 重置线程池。
	void
	reset()
	{
		reset(max_tasks);
	}

	/*!
	\build 以指定任务数设置内存池。
	\since build 723
	*/
	void
	reset(size_t);
	//@}

	using thread_pool::size;

	//! \since build 623
	//@{
	template<typename _fCallable, typename... _tParams>
	inline future_result_t<_fCallable, _tParams...>
	wait(_fCallable&& f, _tParams&&... args)
	{
		return poll([]() ynothrow{
			return true;
		}, yforward(f), yforward(args)...);
	}

	template<typename _tDuration, typename _fCallable, typename... _tParams>
	inline future_result_t<_fCallable, _tParams...>
	wait_for(const _tDuration& duration, _fCallable&& f, _tParams&&... args)
	{
		return poll_for([]() ynothrow{
			return true;
		}, duration, yforward(f), yforward(args)...);
	}

	template<typename _fWaiter, typename _fCallable, typename... _tParams>
	future_result_t<_fCallable, _tParams...>
	wait_to_enqueue(_fWaiter waiter, _fCallable&& f, _tParams&&... args)
	{
		return thread_pool::wait_to_enqueue(
			[=](std::unique_lock<std::mutex>& lck) -> bool{
			while(!can_enqueue_unlocked())
				if(!waiter(lck))
					return {};
			return true;
		}, [=](_tParams&&... f_args){
			// TODO: Blocked. Use C++14 lambda initializers to implement
			//	passing %f with %ystdex::decay_copy.
			enqueue_condition.notify_one();
			// XXX: Blocked. 'yforward' cause G++ 5.2 to fail (perhaps silently)
			//	with exit code 1.
			return ystdex::invoke(f, std::forward<_tParams&&>(f_args)...);
		}, yforward(args)...);
	}

	template<typename _tTimePoint, typename _fCallable, typename... _tParams>
	inline future_result_t<_fCallable, _tParams...>
	wait_until(const _tTimePoint& abs_time, _fCallable&& f, _tParams&&... args)
	{
		return poll_until([]() ynothrow{
			return true;
		}, abs_time, yforward(f), yforward(args)...);
	}
	//@}
};
#	endif

} // namespace ystdex;

#endif

