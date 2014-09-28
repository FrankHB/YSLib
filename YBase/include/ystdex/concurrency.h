/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file concurrency.h
\ingroup YStandardEx
\brief 并发操作。
\version r167
\author FrankHB <frankhb1989@gmail.com>
\since build 520
\par 创建时间:
	2014-07-21 18:57:13 +0800
\par 修改时间:
	2014-09-23 00:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Concurrency
*/


#ifndef YB_INC_ystdex_concurrency_h_
#define YB_INC_ystdex_concurrency_h_ 1

#include "../ydef.h"
#include "functional.hpp" // for std::bind, ystdex::result_of_t;
#include <future> // for std::packaged_task, std::future;
#include <memory> // for std::make_shared;
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace ystdex
{

//! \since build 358
//@{
template<typename _fCallable, typename... _tParams>
using packed_task_t
	= std::packaged_task<result_of_t<_fCallable&&(_tParams&&...)>()>;


template<typename _fCallable, typename... _tParams>
packed_task_t<_fCallable&&(_tParams&&...)>
pack_task(_fCallable&& f, _tParams&&... args)
{
	return packed_task_t<_fCallable&&(_tParams&&...)>(std::bind(yforward(f),
		yforward(args)...));
}

template<typename _fCallable, typename... _tParams>
std::shared_ptr<packed_task_t<_fCallable&&(_tParams&&...)>>
pack_shared_task(_fCallable&& f, _tParams&&... args)
{
	return std::make_shared<packed_task_t<_fCallable&&(_tParams&&...)>>(
		std::bind(yforward(f), yforward(args)...));
}
//@}


/*!
\brief 线程池。
\note 线程安全。
\note 未控制线程队列的长度。
\since build 520
*/
class YB_API thread_pool
{
private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks{};
	mutable std::mutex queue_mutex{};
	std::condition_variable condition{};
	/*!
	\brief 停止状态。
	\note 仅在析构时设置停止。
	\since build 538
	*/
	bool stopped = {};

public:
	thread_pool(size_t);
	thread_pool(const thread_pool&) = delete;
	//! \brief 析构：合并所有执行中的线程，可能阻塞。
	~thread_pool();

	thread_pool&
	operator=(const thread_pool&) = delete;

	//! \see wait_to_enqueue
	template<typename _fCallable, typename... _tParams>
	std::future<result_of_t<_fCallable&&(_tParams&&...)>>
	enqueue(_fCallable&& f, _tParams&&... args)
	{
		return wait_to_enqueue([](std::unique_lock<std::mutex>&){}, yforward(f),
			yforward(args)...);
	}

	size_t
	size() const;

	//! \since build 538
	size_t
	size_unlocked() const
	{
		return tasks.size();
	}

	/*!
	\warning 需要确保未被停止（未进入析构），否则不保证任务被运行。
	\warning 使用非递归锁，等待时不能再次锁定。
	\since build 538
	*/
	template<typename _fWaiter, typename _fCallable, typename... _tParams>
	std::future<result_of_t<_fCallable&&(_tParams&&...)>>
	wait_to_enqueue(_fWaiter wait, _fCallable&& f, _tParams&&... args)
	{
		using return_type = result_of_t<_fCallable&&(_tParams&&...)>;
		auto task(std::make_shared<std::packaged_task<return_type()>>(
			std::bind(yforward(f), yforward(args)...)));
		auto res(task->get_future());

		{
			std::unique_lock<std::mutex> lck(queue_mutex);

			wait(lck);
			tasks.push([task]{
				(*task)();
			});
		}
		condition.notify_one();
		return std::move(res);
	}
};


/*!
\brief 任务池：带有队列大小限制的线程池。
\since build 538
\todo 允许调整队列大小限制。
*/
class YB_API task_pool : private thread_pool
{
private:
	size_t max_tasks;
	std::condition_variable enqueue_condition{};

public:
	task_pool(size_t n)
		: thread_pool(std::max<size_t>(n, 1)), max_tasks(std::max<size_t>(n, 1))
	{}

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

	/*!
	\brief 重置线程池。
	\note 阻塞等待当前所有任务完成后重新创建。
	*/
	void
	reset();

	using thread_pool::size;

	template<typename _tDuration, typename _fCallable, typename... _tParams>
	auto
	wait_for(const _tDuration& duration, _fCallable&& f, _tParams&&... args)
		-> decltype(enqueue(yforward(f), yforward(args)...))
	{
		return wait_to_enqueue([=](std::unique_lock<std::mutex>& lck){
			enqueue_condition.wait_for(lck, duration, [this]{
				return can_enqueue_unlocked();
			});
		}, yforward(f), yforward(args)...);
	}

	template<typename _fWaiter, typename _fCallable, typename... _tParams>
	auto
	wait_to_enqueue(_fWaiter wait, _fCallable&& f, _tParams&&... args)
		-> decltype(enqueue(yforward(f), yforward(args)...))
	{
		return thread_pool::wait_to_enqueue(
			[=](std::unique_lock<std::mutex>& lck){
			while(!can_enqueue_unlocked())
				wait(lck);
		}, yforward(f), yforward(args)...);
	}
};

} // namespace ystdex;

#endif

