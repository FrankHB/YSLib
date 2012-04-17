/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ymsg.h
\ingroup Core
\brief 消息处理。
\version r2594;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-06 02:44:31 +0800;
\par 修改时间:
	2012-04-13 19:08 +0800;
\par 文本编码:
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

	Message&
	operator=(const ValueObject& c)
	{
		content = c;
		return *this;
	}
	/*!
	\brief 成员赋值：使用值类型对象。
	\note 无异常抛出。
	\since build 296 。
	*/
	Message&
	operator=(ValueObject&& c) ynothrow
	{
		content = std::move(c);
		return *this;
	}
	/*
	\brief 复制赋值。
	\since build 296 。
	*/
	Message&
	operator=(const Message& msg)
	{
		Message(msg).Swap(*this);
		return *this;
	}
	/*
	\brief 转移赋值。
	\note 无异常抛出。
	\since build 296 。
	*/
	Message&
	operator=(Message&& msg) ynothrow
	{
		msg.Swap(*this);
		return *this;
	}

	/*!
	\brief 比较：相等关系。
	*/
	friend bool
	operator==(const Message&, const Message&);
	/*!
	\brief 消息优先级比较函数。
	\since build 272 。
	*/
	friend bool
	operator<(const Message&, const Message&);

	DefPred(const ynothrow, TimeOut, timestamp + timeout < std::clock()) \
		//!< 判断消息是否过期。
	DefPred(const ynothrow, Valid, id) //!< 判断消息是否有效。

	DefGetter(const ynothrow, shared_ptr<Shell>, ShellHandle, hShl) \
		//!< 取关联的 Shell 句柄。
	DefGetter(const ynothrow, ID, MessageID, id) //!< 取消息标识。
	DefGetter(const ynothrow, Priority, Priority, prior) //!< 取消息优先级。
	DefGetter(const ynothrow, const ValueObject&, Content, content) \
		//!< 取消息内容句柄。

	/*
	\brief 交换。
	*/
	void
	Swap(Message&) ynothrow;

	/*!
	\brief 更新消息时间戳。
	*/
	void
	UpdateTimestamp()
	{
		timestamp = std::clock();
	}
};

/*!
\brief 消息优先级比较函数。
\since build 272 。
*/
inline bool
operator<(const Message& x, const Message& y)
{
//	if(x.prior == y.prior)
	//	return x.time > y.time;
//		return x.GetObjectID() < y.GetObjectID();
	return x.prior > y.prior;
}


/*!
\brief 消息队列。
\note 使用 multiset 模拟。
\warning 非虚析构。
\since build 211 。
*/
class MessageQueue : private noncopyable, private multiset<Message>
{
public:
	typedef size_type SizeType;

	/*!
	\brief 无参数构造：默认实现。
	*/
	inline DefDeCtor(MessageQueue)
	DefEmptyDtor(MessageQueue)

	DefPred(const ynothrow, Empty, empty()) //!< 判断消息队列是否为空。

	DefGetter(const ynothrow, SizeType, Size, size()) //!< 取队列中消息容量。
	/*!
	\brief 取消息队列中消息的最大优先级。
	\return 若消息队列为空则 0 ，否则为最大优先级。
	\since build 288 。
	*/
	DefGetter(const ynothrow, Priority, MaxPriority,
		empty() ? 0 : begin()->GetPriority())

	/*!
	\brief 清除消息队列。
	*/
	PDefH(void, Clear)
		ImplRet(clear())

	/*!
	\brief 合并消息队列：移动指定消息队列中的所有消息至此消息队列中。
	*/
	void
	Merge(MessageQueue&);

	/*!
	\brief 从消息队列中取优先级最高的消息存至 msg 中。
	\note 在队列中保留消息；不检查消息是否有效。
	\since build 271 。
	*/
	void
	Peek(Message& msg) const
	{
		if(YCL_LIKELY(!empty()))
			msg = *begin();
	}
	/*
	\brief 从消息队列中取消息。
	\param lpMsg 接收消息信息的 Message 结构指针。
	\param hShl 消息关联（发送目标）的 Shell 的句柄，
		为 nullptr 时无限制（为全局消息）。
	\param bRemoveMsg 确定取得的消息是否消息队列中清除。
	\since build 271 。
	*/
	int
	Peek(Message& msg, const shared_ptr<Shell>& hShl,
		bool bRemoveMsg = false);

	/*!
	\brief 丢弃消息队列中优先级最高的消息。
	\note 消息队列为空时忽略。
	*/
	void
	Pop()
	{
		if(YCL_LIKELY(!empty()))
			erase(begin());
	}

	/*!
	\brief 若消息 msg 有效，插入 msg 至消息队列中。
	*/
	void
	Push(const Message& msg)
	{
		if(msg.IsValid())
			insert(msg);
	}

	/*!
	\brief 移除指定 Shell 关联的不大于指定优先级的消息。
	\note 若第一参数为空则移除所有 Shell 关联的消息。
	\since build 289 。
	*/
	void
	Remove(Shell*, Priority);
};

YSL_END_NAMESPACE(Messaging)

using Messaging::Message;

YSL_END

#endif

