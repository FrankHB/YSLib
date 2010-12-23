/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ysmsg.cpp
\ingroup Core
\brief 消息处理。
\version 0.1572;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-06 02:44:31 + 08:00;
\par 修改时间:
	2010-12-21 16:10 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShellMessage;
*/


#include "ysmsg.h"
#include "../Shell/ydesktop.h" // for YDesktop & YWindow delete procedure;

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Messaging)

Message::Message(GHHandle<YShell> shl, ID m, Priority p,
	SmartPtr<IContext> pCon)
	: hShl(shl), id(m), prior(p), pContext(pCon),
	timestamp(std::clock()), timeout(DefTimeout)
{}

bool Message::operator==(const Message& m) const
{
	return hShl == m.hShl && id == m.id && prior == m.prior
		&& pContext != NULL && m.pContext != NULL && *pContext == *m.pContext;
}

YMessageQueue::YMessageQueue()
	: q()
{}
YMessageQueue::~YMessageQueue() ythrow()
{}

void
YMessageQueue::GetMessage(Message& m)
{
	if(!q.empty())
	{
		if(q.top().IsValid())
			m = q.top();
		q.pop();
	}
}

void
YMessageQueue::PeekMessage(Message& m) const
{
	if(!q.empty())
		if(q.top().IsValid())
			m = q.top();
}

YMessageQueue::size_type
YMessageQueue::Clear()
{
	size_type n = 0;

	while(!empty())
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
YMessageQueue::Insert(const Message& m)
{
	if(m.IsValid())
		q.push(m);
	return m.IsValid();
}

void
Merge(YMessageQueue& dst, vector<Message>& src)
{
	Message m;

	while(!src.empty())
	{
		m = src[src.size() - 1];
		dst.Insert(m);
		src.pop_back();
	}
}
void
Merge(YMessageQueue& dst, YMessageQueue& src)
{
	Message m;

	while(!src.empty())
	{
		src.GetMessage(m);
		dst.Insert(m);
	}
}

YSL_END_NAMESPACE(Messaging)

YSL_END

