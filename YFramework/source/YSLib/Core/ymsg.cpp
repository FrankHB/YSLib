/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ymsg.cpp
\ingroup Core
\brief 消息处理。
\version r2029;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-06 02:44:31 +0800;
\par 修改时间:
	2011-12-23 07:02 +0800;
\par 字符集:
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
Message::Swap(Message& rhs) ynothrow
{
	std::swap(hShl, rhs.hShl);
	std::swap(id, rhs.id);
	std::swap(prior, rhs.prior);
	std::swap(content, rhs.content);
	std::swap(timestamp, rhs.timestamp);
	std::swap(timeout, rhs.timeout);
}

bool
operator==(const Message& lhs, const Message& rhs)
{
	return lhs.hShl == rhs.hShl && lhs.id == rhs.id && lhs.prior == rhs.prior
		&& lhs.content == rhs.content;
}


void
MessageQueue::Merge(MessageQueue& mq)
{
	for(auto i(mq.q.begin()); i != mq.q.end(); ++i)
		if(i->IsValid())
			q.insert(std::move(*i));
	mq.q.clear();
}

int
MessageQueue::Peek(Message& msg, const shared_ptr<Shell>& hShl,
	bool bRemoveMsg)
{
	for(auto i(q.cbegin()); i != q.cend(); ++i)
	{
		const Message& m(*i);

		if(!hShl || !m.GetShellHandle() || hShl == m.GetShellHandle())
		{
			msg = m;
			if(bRemoveMsg)
				q.erase(i);
			return msg.GetMessageID();
		}
	}
	return -1;
}

YSL_END_NAMESPACE(Messaging)

YSL_END

