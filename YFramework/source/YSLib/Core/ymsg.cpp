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
\version r2049;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-06 02:44:31 +0800;
\par 修改时间:
	2012-02-26 15:25 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YMessage;
*/


#include "YSLib/Core/ymsg.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Messaging)

Message::Message(const shared_ptr<Shell>& h, ID m, Priority p,
	const ValueObject& c)
	: hShl(h), id(m), prior(p), content(c), timestamp(std::clock()),
	timeout(DefTimeout)
{}

void
Message::Swap(Message& msg) ynothrow
{
	std::swap(hShl, msg.hShl);
	std::swap(id, msg.id);
	std::swap(prior, msg.prior);
	std::swap(content, msg.content);
	std::swap(timestamp, msg.timestamp);
	std::swap(timeout, msg.timeout);
}

bool
operator==(const Message& x, const Message& y)
{
	return x.hShl == y.hShl && x.id == y.id && x.prior == y.prior
		&& x.content == y.content;
}


void
MessageQueue::Merge(MessageQueue& mq)
{
	std::for_each(mq.begin(), mq.end(), [this](decltype(*mq.begin())& msg){
		if(msg.IsValid())
			insert(std::move(msg));
	});
	mq.clear();
}

int
MessageQueue::Peek(Message& msg, const shared_ptr<Shell>& hShl, bool bRemoveMsg)
{
	for(auto i(cbegin()); i != cend(); ++i)
	{
		const Message& m(*i);

		if(!hShl || !m.GetShellHandle() || hShl == m.GetShellHandle())
		{
			if(bRemoveMsg)
			{
				// NOTE: it's safe because the priority is not changed
				//	and it should be removed immediately;
				msg = std::move(const_cast<Message&>(m));
				erase(i);
			}
			else
				msg = m;
			return msg.GetMessageID();
		}
	}
	return -1;
}

YSL_END_NAMESPACE(Messaging)

YSL_END

