/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ysmsg.cpp
\ingroup Core
\brief 消息处理。
\version 0.2006;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-06 02:44:31 +0800;
\par 修改时间:
	2011-06-06 23:24 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShellMessage;
*/


#include "ysmsg.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Messaging)

Content::Content(const Content& c)
	: manager(c.manager), obj_ptr(nullptr)
{
	if(manager && c.obj_ptr)
		manager(obj_ptr, c.obj_ptr, Clone);
}
Content::Content(Content&& c)
	: manager(c.manager), obj_ptr(c.obj_ptr)
{
	c.obj_ptr = nullptr;
}

bool
Content::operator==(const Content& rhs) const
{
	return (!manager && !rhs.manager) || (manager && rhs.manager
		&& manager == rhs.manager && obj_ptr && rhs.obj_ptr
		&& manager(obj_ptr, rhs.obj_ptr, Equality));
}

Content&
Content::operator=(Content&& c)
{
	if(&c != this)
	{
		Clear();
		manager = c.manager;
		std::swap(obj_ptr, c.obj_ptr);
	}
	return *this;
}

void
Content::Clear()
{
	if(manager)
		manager(obj_ptr, obj_ptr, Destroy);
}

void
Content::Swap(Content& c)
{
	std::swap(manager, c.manager);
	std::swap(obj_ptr, c.obj_ptr);
}


Message::Message(const shared_ptr<YShell>& h, ID m, Priority p,
	const Content& c)
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


Message
MessageQueue::GetMessage()
{
	Message msg;

	if(!q.empty())
	{
		if(top().IsValid())
			msg = top();
		pop();
	}
	return msg;
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
MessageQueue::PeekMessage(Message& msg, const shared_ptr<YShell>& hShl,
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

void
MessageQueue::Update()
{
	if(!q.empty() && top().IsTimeOut());
		pop();
}

YSL_END_NAMESPACE(Messaging)

YSL_END

