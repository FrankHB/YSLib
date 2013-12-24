/*
	© 2009-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ymsg.cpp
\ingroup Core
\brief 消息处理。
\version r1232
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-06 02:44:31 +0800
\par 修改时间:
	2013-12-24 09:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YMessage
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YMessage
#include <ystdex/algorithm.hpp>

namespace YSLib
{

namespace Messaging
{

void
Message::swap(Message& msg) ynothrow
{
	std::swap(id, msg.id),
	content.swap(msg.content);
}

bool
operator==(const Message& x, const Message& y)
{
	return x.id == y.id && x.content == y.content;
}


void
MessageQueue::Merge(MessageQueue& mq)
{
	std::for_each(mq.begin(), mq.end(), [this](decltype(*mq.begin())& pr){
		if(pr.second)
			insert(std::move(pr));
	});
	mq.clear();
}

void
MessageQueue::Peek(Message& msg) const
{
	if(!empty())
		msg = begin()->second;
}

void
MessageQueue::Pop()
{
	if(!empty())
		erase(begin());
}

void
MessageQueue::Push(const Message& msg, Priority prior)
{
	if(msg)
		insert(make_pair(prior, msg));
}
void
MessageQueue::Push(Message&& msg, Priority prior)
{
	if(msg)
		insert(make_pair(prior, std::move(msg)));
}

void
MessageQueue::Remove(Priority p)
{
	erase(upper_bound(p), end());
}

} // namespace Messaging;

} // namespace YSLib;

