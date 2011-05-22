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
\version 0.1805;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-06 02:44:31 +0800;
\par 修改时间:
	2011-05-21 23:59 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShellMessage;
*/


#include "ysmsg.h"
#include "../UI/ywindow.h" // for YWindow delete procedure;

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
MessageQueue::FetchMessage()
{
	Message m;

	if(!q.empty())
	{
		if(q.top().IsValid())
			m = q.top();
		q.pop();
	}
	return m;
}

void
MessageQueue::PeekMessage(Message& msg) const
{
	if(!q.empty())
		if(q.top().IsValid())
			msg = q.top();
}

MessageQueue::SizeType
MessageQueue::Clear()
{
	SizeType n = 0;

	while(!IsEmpty())
	{
		q.pop();
		++n;
	}
	return n;
}
void
MessageQueue::Update()
{
	if(!q.empty())
	{
		if(q.top().IsTimeOut())
			q.pop();
	}
}

bool
MessageQueue::Insert(const Message& msg)
{
	if(msg.IsValid())
		q.push(msg);
	return msg.IsValid();
}

void
Merge(MessageQueue& dst, list<Message>& src)
{
	while(!src.empty())
	{
		Message m(src.back());

		dst.Insert(m);
		src.pop_back();
	}
}
void
Merge(MessageQueue& dst, MessageQueue& src)
{
	// TODO: 处理异常安全事项。
	while(!src.IsEmpty())
		dst.Insert(src.FetchMessage());
}

YSL_END_NAMESPACE(Messaging)

YSL_END

