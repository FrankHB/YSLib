/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ymsg.h
\ingroup Core
\brief 消息处理。
\version r2429;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-06 02:44:31 +0800;
\par 修改时间:
	2011-12-01 09:48 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YMessage;
*/


#ifndef YSL_INC_CORE_YSMSG_H_
#define YSL_INC_CORE_YSMSG_H_

#include "yobject.h"
#include "../Adaptor/ycont.h"
#include <ctime>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Messaging)

/*!
\brief 消息标识。
\since build 175 。
*/
typedef u32 ID;
/*!
\brief 消息优先级。
\since build 175 。
*/
typedef u8 Priority;

/*!
\brief 消息默认有效期。
\since build 175 。
*/
const std::time_t DefTimeout(0);


/*!
\brief 消息。
\warning 非虚析构。
\since build 145 。
*/
class Message
{
	friend class MessageQueue;

private:
	shared_ptr<Shell> hShl; //!< 目的 Shell 句柄。
	ID id; //!< 消息标识。
	Priority prior; //!< 消息优先级。
	ValueObject content; //消息内容句柄。

public:
	std::clock_t timestamp; //!< 消息时间戳：消息产生的进程时间。

private:
	std::clock_t timeout; //!< 消息有效期。

public:
	/*!
	\brief 构造：使用 Shell 句柄、消息标识、消息优先级和消息内容。
	*/
	Message(const shared_ptr<Shell>& = shared_ptr<Shell>(), ID = 0,
		Priority = 0, const ValueObject& = ValueObject());

	/*!
	\brief 复制构造：默认实现。
	*/
	DefDeCopyCtor(Message)
	/*!
	\brief 转移构造：默认实现。
	*/
	DefDeMoveCtor(Message)

	/*
	\brief 复制赋值。
	*/
	Message&
	operator=(const Message&);
	/*
	\brief 转移赋值。
	*/
	Message&
	operator=(Message&&);
	Message&
	operator=(const ValueObject&);
	Message&
	operator=(ValueObject&&);

	/*!
	\brief 比较：相等关系。
	*/
	friend bool
	operator==(const Message&, const Message&);

	DefPredicate(TimeOut, timestamp + timeout < std::clock()) \
		//!< 判断消息是否过期。
	DefPredicate(Valid, id) //!< 判断消息是否有效。

	DefGetter(shared_ptr<Shell>, ShellHandle, hShl) //!< 取关联的 Shell 句柄。
	DefGetter(ID, MessageID, id) //!< 取消息标识。
	DefGetter(Priority, Priority, prior) //!< 取消息优先级。
	DefGetter(const ValueObject&, Content, content) //!< 取消息内容句柄。

	/*
	\brief 交换。
	*/
	void
	Swap(Message&) ynothrow;

	/*!
	\brief 更新消息时间戳。
	*/
	void
	UpdateTimestamp();
};

inline Message&
Message::operator=(const Message& msg)
{
	Message tmp(msg);

	tmp.Swap(*this);
	return *this;
}
inline Message&
Message::operator=(Message&& msg)
{
	hShl = std::move(msg.hShl);
	id = std::move(msg.id);
	prior = std::move(msg.prior);
	content = std::move(msg.content);
	timestamp = std::move(msg.timestamp);
	timeout = std::move(msg.timeout);
	return *this;
}
inline Message&
Message::operator=(const ValueObject& c)
{
	content = c;
	return *this;
}
inline Message&
Message::operator=(ValueObject&& c)
{
	content = std::move(c);
	return *this;
}

inline void
Message::UpdateTimestamp()
{
	timestamp = std::clock();
}


/*!
\brief 消息队列。
\since build 211 。
*/
class MessageQueue : public noncopyable
{
private:
	//消息优先级比较函数对象。
	struct cmp
	{
		/*!
		\brief 调用比较函数。
		*/
		bool
		operator()(const Message& a, const Message& b)
		{
		//	if(a.prior == b.prior)
			//	return a.time > b.time;
		//		return a.GetObjectID() < b.GetObjectID();
			return a.prior > b.prior;
		}
	};

	multiset<Message, cmp> q; //!< 消息优先队列：使用 multiset 模拟。

	PDefH(const Message&, top) const
		ImplRet(*q.begin())
	PDefH(void, pop)
		ImplExpr(q.erase(q.begin()))
	PDefH(void, push, const Message& msg)
		ImplExpr(q.insert(msg))

public:
	typedef decltype(q.size()) SizeType;

	/*!
	\brief 无参数构造：默认实现。
	*/
	inline DefDeCtor(MessageQueue)
	virtual DefEmptyDtor(MessageQueue)

	DefPredicate(Empty, q.empty()) //!< 判断消息队列是否为空。

	DefGetter(SizeType, Size, q.size()) //!< 取队列中消息容量。
	/*!
	\brief 取消息队列中取优先级最高的消息。
	\note 不在消息队列中保留消息。
	*/
	Message
	GetMessage();

	/*!
	\brief 清除消息队列。
	*/
	PDefH(void, Clear)
		ImplRet(q.clear())

	/*!
	\brief 合并消息队列：移动指定消息队列中的所有消息至此消息队列中。
	*/
	void
	Merge(MessageQueue&);

	/*!
	\brief 从消息队列中取优先级最高的消息存至 msg 中。
	\note 在队列中保留消息；不检查消息是否有效。
	*/
	void
	PeekMessage(Message& msg) const;
	/*
	\brief 从消息队列中取消息。
	\param lpMsg 接收消息信息的 Message 结构指针。
	\param hShl 消息关联（发送目标）的 Shell 的句柄，
		为 nullptr 时无限制（为全局消息）。
	\param bRemoveMsg 确定取得的消息是否消息队列中清除。
	*/
	int
	PeekMessage(Message& msg, const shared_ptr<Shell>& hShl,
		bool bRemoveMsg = false);

	/*!
	\brief 丢弃消息队列中优先级最高的消息。
	\note 消息队列为空时忽略。
	*/
	void
	Pop();

	/*!
	\brief 若消息 msg 有效，插入 msg 至消息队列中。
	*/
	void
	Push(const Message& msg);

	/*!
	\brief 更新消息队列。
	*/
	void
	Update();
};

inline void
MessageQueue::PeekMessage(Message& msg) const
{
	if(!q.empty())
		msg = top();
}

inline void
MessageQueue::Push(const Message& msg)
{
	if(msg.IsValid())
		push(msg);
}

inline void
MessageQueue::Pop()
{
	if(!q.empty())
		pop();
}


YSL_END_NAMESPACE(Messaging)

using Messaging::Message;

YSL_END

#endif

