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
\version r91
\author FrankHB <frankhb1989@gmail.com>
\since build 520
\par 创建时间:
	2014-07-21 18:57:13 +0800
\par 修改时间:
	2014-07-22 10:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Concurrency
*/


#ifndef YB_INC_ystdex_concurrency_h_
#define YB_INC_ystdex_concurrency_h_ 1

#include "../ydef.h"
#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <future>

namespace ystdex
{

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
	bool stop = {};

public:
	thread_pool(size_t);
	thread_pool(const thread_pool&) = delete;
	//! \brief 析构：合并所有执行中的线程，可能阻塞。
	~thread_pool();

	thread_pool&
	operator=(const thread_pool&) = delete;

	template<typename _fCallable, typename... _tParams>
	std::future<ystdex::result_of_t<_fCallable&&(_tParams&&...)>>
	enqueue(_fCallable&& f, _tParams&&... args)
	{
		using return_type = ystdex::result_of_t<_fCallable&&(_tParams&&...)>;
		auto task(std::make_shared<std::packaged_task<return_type()>>(
			std::bind(yforward(f), yforward(args)...)));
		auto res(task->get_future());

		{
			std::unique_lock<std::mutex> lck(queue_mutex);

			tasks.push([task]{
				(*task)();
			});
		}
		condition.notify_one();
		return std::move(res);
	}

	size_t
	size() const;
};

} // namespace ystdex;

#endif

