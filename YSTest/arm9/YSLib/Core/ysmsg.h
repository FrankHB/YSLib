/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ysmsg.h
\ingroup Core
\brief 消息处理。
\version 0.2037;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-06 02:44:31 + 08:00;
\par 修改时间:
	2010-12-17 13:43 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShellMessage;
*/


#ifndef INCLUDED_YSMSG_H_
#define INCLUDED_YSMSG_H_

#include "ysdef.h"
#include "yobject.h"
#include "../Adaptor/cont.h"
#include <ctime>

YSL_BEGIN

using Drawing::Point;

YSL_BEGIN_NAMESPACE(Messaging)

typedef u32 ID;
typedef u8 Priority;

const std::time_t DefTimeout(0);


//! \brief 消息上下文接口。
DeclInterface(IContext)
	DeclIEntry(bool operator==(const IContext&) const)
EndDecl


//! \brief 消息类。
class Message : public GMCounter<Message>
{
	friend class YMessageQueue;

private:
	GHHandle<YShell> hShl; //!< 目的 Shell 句柄。
	ID id; //!< 消息标识。
	Priority prior; //!< 消息优先级。
	SmartPtr<IContext> pContext; //消息上下文指针。

public:
	std::clock_t timestamp; //!< 消息时间戳：消息产生的进程时间。

private:
	std::clock_t timeout; //!< 消息有效期。

public:
	/*!
	\brief 构造：
		使用 Shell 句柄、消息标识、消息优先级、光标位置和消息上下文指针。
	*/
	Message(GHHandle<YShell> = NULL, ID = 0, Priority = 0,
		SmartPtr<IContext> = NULL);

	/*!
	\brief 比较：相等关系。
	*/
	bool
	operator==(const Message&) const;
	/*!
	\brief 比较：不等关系。
	*/
	bool
	operator!=(const Message&) const;

	DefPredicate(TimeOut, timestamp + timeout < std::clock()) //!< 判断消息是否过期。
	DefPredicate(Valid, id) //!< 判断消息是否有效。

	DefGetter(GHHandle<YShell>, ShellHandle, hShl) //!< 取关联的 Shell 句柄。
	DefGetter(ID, MessageID, id) //!< 取消息标识。
	DefGetter(Priority, Priority, prior) //!< 取消息优先级。
	DefGetter(SmartPtr<IContext>, ContextPtr, pContext) //!< 取消息上下文。

	/*!
	\brief 更新消息时间戳。
	*/
	void
	UpdateTimestamp();
};

inline bool Message::operator!=(const Message& m) const
{
	return !(*this == m);
}

inline void
Message::UpdateTimestamp()
{
	timestamp = std::clock();
}


/*!
\brief 取指定消息的消息上下文指针并用 dynamic_cast 转换为制定类型的内建指针。
*/
template<class _type>
_type*
FetchContextRawPtr(const Message& m)
{
	return dynamic_cast<_type*>(GetPointer(m.GetContextPtr()));
}


//消息队列类。
class YMessageQueue : public YObject
{
private:
	//消息优先级比较函数对象。
	struct cmp
	{
		/*!
		\brief 调用比较函数。
		*/
		bool
		operator()(const Message& i, const Message& j)
		{
			if(i.prior == j.prior)
			//	return i.time > j.time;
				return i.GetObjectID() > j.GetObjectID();
			return i.prior < j.prior;
		}
	};

	//消息优先队列。
	priority_queue<Message, vector<Message>, cmp> q;

	PDefH(const Message&, top) const
		ImplBodyMember(q, top)
	PDefH(void, push, const Message& msg)
		ImplBodyMemberVoid(q, push, msg)
	PDefH(void, pop)
		ImplBodyMemberVoid(q, pop)

public:
	typedef priority_queue<int, vector<int>, cmp>::size_type size_type;

	/*!
	\brief 无参数构造。
	\note 非内联。
	*/
	YMessageQueue();
	/*!
	\brief 析构。
	\note 无异常抛出。
	\note 非内联。
	*/
	virtual
	~YMessageQueue() ythrow();

	PDefH(bool, empty) const //!< 判断消息队列是否为空。
		ImplBodyMember(q, empty)
	PDefH(size_type, size) const //!< 取队列中消息容量。
		ImplBodyMember(q, size)

	/*!
	\brief 从消息队列中取优先级最高的消息存至 m 中。
	\note 不在消息队列中保留消息。
	*/
	void
	GetMessage(Message& m);

	/*!
	\brief 从消息队列中取优先级最高的消息存至 m 中。
	\note 在队列中保留消息。
	*/
	void
	PeekMessage(Message& m) const;

	/*!
	\brief 清除消息队列，返回清除的消息数。
	*/
	size_type
	Clear();

	/*!
	\brief 更新消息队列。
	*/
	void
	Update();

	/*!
	\brief 若消息 m 有效，插入 m 至消息队列中。返回 m 是否有效。
	*/
	bool
	Insert(const Message& m);
};


/*!
\brief 合并 src 所有消息至 dst 中。
*/
void
Merge(YMessageQueue& dst, vector<Message>& src);
/*!
\brief 合并 src 所有消息至 dst 中。
*/
void
Merge(YMessageQueue& dst, YMessageQueue& src);

YSL_END_NAMESPACE(Messaging)

using Messaging::Message;

YSL_END

#endif

