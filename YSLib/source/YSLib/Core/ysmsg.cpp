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
\version 0.1759;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-06 02:44:31 +0800;
\par 修改时间:
	2011-05-17 09:30 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShellMessage;
*/


#include "ysmsg.h"
#include "../UI/ywindow.h" // for YWindow delete procedure;

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Messaging)

Content::~Content()
{
	manager(obj_ptr, obj_ptr, Destroy);
}
Content::Content(const Content& rhs)
	: manager(rhs.manager)
{
	manager(obj_ptr, rhs.obj_ptr, Clone);
}
Content::Content(Content&& rhs)
	: manager(rhs.manager), obj_ptr(rhs.obj_ptr)
{}

bool
Content::operator==(const Content& rhs) const
{
	return manager && rhs.manager && manager == rhs.manager
		&& obj_ptr && rhs.obj_ptr && manager(obj_ptr, rhs.obj_ptr, Equality);
}


Message::Message(const shared_ptr<YShell>& h, ID m, Priority p,
	const shared_ptr<Content>& hCon)
	: hShl(h), id(m), prior(p), hContent(hCon),
	timestamp(std::clock()), timeout(DefTimeout)
{}

Message&
Message::operator=(const Message& rhs)
{
	Message m(rhs);

	m.Swap(*this);
	return *this;
}

void
Message::Swap(Message& rhs) ynothrow
{
	std::swap(hShl, rhs.hShl);
	std::swap(id, rhs.id);
	std::swap(prior, rhs.prior);
	std::swap(hContent, rhs.hContent);
	std::swap(timestamp, rhs.timestamp);
	std::swap(timeout, rhs.timeout);
}

bool
operator==(const Message& lhs, const Message& rhs)
{
	return lhs.hShl == rhs.hShl && lhs.id == rhs.id && lhs.prior == rhs.prior
		&& lhs.hContent && rhs.hContent && *lhs.hContent == *rhs.hContent;
}


Message
YMessageQueue::GetMessage()
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
YMessageQueue::PeekMessage(Message& msg) const
{
	if(!q.empty())
		if(q.top().IsValid())
			msg = q.top();
}

YMessageQueue::SizeType
YMessageQueue::Clear()
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
YMessageQueue::Update()
{
	if(!q.empty())
	{
		if(q.top().IsTimeOut())
			q.pop();
	}
}

bool
YMessageQueue::Insert(const Message& msg)
{
	if(msg.IsValid())
		q.push(msg);
	return msg.IsValid();
}

void
Merge(YMessageQueue& dst, list<Message>& src)
{
	while(!src.empty())
	{
		Message m(src.back());

		dst.Insert(m);
		src.pop_back();
	}
}
void
Merge(YMessageQueue& dst, YMessageQueue& src)
{
	// TODO: 处理异常安全事项。
	while(!src.IsEmpty())
		dst.Insert(src.GetMessage());
}

YSL_END_NAMESPACE(Messaging)

YSL_END

