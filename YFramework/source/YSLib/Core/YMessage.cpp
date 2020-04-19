/*
	© 2009-2016, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YMessage.cpp
\ingroup Core
\brief 消息处理。
\version r1263
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-06 02:44:31 +0800
\par 修改时间:
	2020-04-18 23:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YMessage
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YMessage

namespace YSLib
{

namespace Messaging
{

void
swap(Message& x, Message& y) ynothrow
{
	std::swap(x.id, y.id),
	swap(x.content, y.content);
}


void
MessageQueue::Merge(MessageQueue& mq)
{
	std::for_each(mq.begin(), mq.end(), [this](decltype(*mq.begin()) pr){
		if(pr.second)
			insert(std::move(pr));
	});
	mq.clear();
}

void
MessageQueue::Peek(Message& msg) const
{
	if(!empty())
		msg = cbegin()->second;
}

void
MessageQueue::Pop()
{
	if(!empty())
		erase(cbegin());
}

void
MessageQueue::Push(const Message& msg, Priority prior)
{
	emplace(prior, msg);
}
void
MessageQueue::Push(Message&& msg, Priority prior)
{
	emplace(prior, std::move(msg));
}

void
MessageQueue::Remove(Priority p)
{
	erase(upper_bound(p), end());
}


ImplDeDtor(MessageException)


ImplDeDtor(MessageSignal)

} // namespace Messaging;

} // namespace YSLib;

