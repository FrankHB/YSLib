/*
	© 2014-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file concurrency.cpp
\ingroup YStandardEx
\brief 并发操作。
\version r165
\author FrankHB <frankhb1989@gmail.com>
\since build 520
\par 创建时间:
	2014-07-21 19:09:18 +0800
\par 修改时间:
	2016-08-27 15:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Concurrency
*/


#include "ydef.h"
#if __STDCPP_THREADS__ || (defined(__GLIBCXX__) \
	&& defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)) \
	|| (defined(_LIBCPP_VERSION) && !defined(_LIBCPP_HAS_NO_THREADS))
#include <sstream>
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


#	if !__GLIBCXX__ || (defined(_GLIBCXX_HAS_GTHREADS) \
	&& defined(_GLIBCXX_USE_C99_STDINT_TR1) && (ATOMIC_INT_LOCK_FREE > 1))
thread_pool::thread_pool(size_t n, std::function<void()> on_enter,
	std::function<void()> on_exit)
{
	workers.reserve(n);
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
					// NOTE: Do nothing for spurious wakeup.
					if(stopped)
						break;
				}
				else
				{
					auto task(std::move(tasks.front()));

					tasks.pop();
					lck.unlock();
					try
					{
						task();
					}
					catch(std::future_error&)
					{
						yassume(false);
					}
				}
			}
			if(on_exit)
				on_exit();
		});
}
thread_pool::~thread_pool() ynothrow
{
	try
	{
		try
		{
			std::lock_guard<std::mutex> lck(queue_mutex);

			stopped = true;
		}
		catch(std::system_error&)
		{
			yassume(false);
		}
		condition.notify_all();
		for(auto& worker : workers)
			try
			{
				worker.join();
			}
			catch(std::system_error&)
			{}
	}
	catch(...)
	{
		yassume(false);
	}
}

size_t
thread_pool::size() const
{
	std::unique_lock<std::mutex> lck(queue_mutex);

	return size_unlocked();
}

void
task_pool::reset(size_t tasks_num)
{
	auto& threads(static_cast<thread_pool&>(*this));

	threads.~thread_pool();
	::new(&threads) thread_pool(tasks_num);
}
#	endif

} // namespace ystdex;

#endif

