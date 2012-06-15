/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ymsg.cpp
\ingroup Core
\brief 消息处理。
\version r2140;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-06 02:44:31 +0800;
\par 修改时间:
	2012-06-15 15:06 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YMessage;
*/


#include "YSLib/Core/ymsg.h"
#include <ystdex/algorithm.hpp>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Messaging)

Message::Message(const weak_ptr<Shell>& wp, ID m, Priority p,
	const ValueObject& c)
	: dest(wp), to_all(wp.expired()), id(m), content(c)
{}
Message::Message(Message&& msg)
	: dest(msg.dest), to_all(msg.to_all), id(msg.id),
	content(std::move(msg.content))
{}

void
Message::Swap(Message& msg) ynothrow
{
	std::swap(dest, msg.dest),
	std::swap(to_all, msg.to_all),
	std::swap(id, msg.id),
	content.Swap(msg.content);
}

bool
operator==(const Message& x, const Message& y)
{
	return x.dest == y.dest && x.to_all == y.to_all && x.id == y.id
		&& x.content == y.content;
}


void
MessageQueue::Merge(MessageQueue& mq)
{
	std::for_each(mq.begin(), mq.end(), [this](decltype(*mq.begin())& pr){
		if(pr.second.IsValid())
			insert(std::move(pr));
	});
	mq.clear();
}

MessageQueue::Iterator
MessageQueue::Peek(const shared_ptr<Shell>& hShl)
{
	auto i(cbegin());

	if(hShl)
		for(; i != cend(); ++i)
			if(i->second.IsToAll() || hShl == i->second.GetDestination().lock())
				break;
	return i;
}

void
MessageQueue::Remove(Shell* pShl, Priority p)
{
	auto i(upper_bound(p));

	if(pShl)
		ystdex::erase_all_if<multimap<Priority, Message,
			std::greater<Priority>>>(*this, i, end(),
			[pShl](const pair<Priority, Message>& pr){
			// NOTE: %raw used here for performance.
			return raw(pr.second.GetDestination()) == pShl;
		});
	else
		erase(i, end());
}

YSL_END_NAMESPACE(Messaging)

YSL_END

