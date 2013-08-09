/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ymsg.cpp
\ingroup Core
\brief 消息处理。
\version r1199
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-06 02:44:31 +0800
\par 修改时间:
	2013-08-05 21:27 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YMessage
*/


#include "YSLib/Core/ymsg.h"
#include <ystdex/algorithm.hpp>

namespace YSLib
{

namespace Messaging
{

Message::Message(ID m, const ValueObject& c)
	: id(m), content(c)
{}
Message::Message(ID m, ValueObject&& c)
	: id(m), content(std::move(c))
{}

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
MessageQueue::Remove(Priority p)
{
	erase(upper_bound(p), end());
}

} // namespace Messaging;

} // namespace YSLib;

