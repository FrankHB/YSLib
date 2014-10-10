/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file concurrency.cpp
\ingroup YStandardEx
\brief 并发操作。
\version r115
\author FrankHB <frankhb1989@gmail.com>
\since build 520
\par 创建时间:
	2014-07-21 19:09:18 +0800
\par 修改时间:
	2014-10-09 15:56 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Concurrency
*/


#include "ystdex/any.h"
#include <sstream>
#if YB_IMPL_GNUCPP && defined(_GLIBCXX_HAS_GTHREADS) \
	&& defined(_GLIBCXX_USE_C99_STDINT_TR1)
#include "ystdex/concurrency.h"

namespace ystdex
{

std::string
to_string(const std::thread::id& id)
{
	std::ostringstream oss;

	oss << id;
	return oss.str();
}


thread_pool::thread_pool(size_t n, std::function<void()> on_enter,
	std::function<void()> on_exit)
	: workers(n)
{
	for(size_t i = 0; i < n; ++i)
		workers.emplace_back([=]{
			if(on_enter)
				on_enter();
			while(true)
			{
				std::unique_lock<std::mutex> lck(queue_mutex);

				condition.wait(lck, [this]{
					return stopped || !tasks.empty();
				});
				if(tasks.empty())
				{
					// NOTE: Do nothing for spurious wake up.
					if(stopped)
						return;
				}
				else
				{
					const auto task(std::move(tasks.front()));

					tasks.pop();
					lck.unlock();
					task();
				}
			}
			if(on_exit)
				on_exit();
		});
}
thread_pool::~thread_pool()
{
	{
		std::unique_lock<std::mutex> lck(queue_mutex);

		stopped = true;
	}
	condition.notify_all();
	for(auto& worker : workers)
		if(worker.joinable())
			worker.join();
}

size_t
thread_pool::size() const
{
	std::unique_lock<std::mutex> lck(queue_mutex);

	return size_unlocked();
}


void
task_pool::reset()
{
	auto& threads(static_cast<thread_pool&>(*this));

	threads.~thread_pool();
	::new(&threads) thread_pool(max_tasks);
}

} // namespace ystdex;

#endif

