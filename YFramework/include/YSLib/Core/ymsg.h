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
\version r1795
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-06 02:44:31 +0800
\par 修改时间:
	2012-09-04 12:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YMessage
*/


#ifndef YSL_INC_CORE_YSMSG_H_
#define YSL_INC_CORE_YSMSG_H_ 1

#include "yobject.h"
#include "../Adaptor/ycont.h"
#include <ctime>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Messaging)

/*!
\brief 消息标识。
\since build 175
*/
typedef u32 ID;
/*!
\brief 消息优先级。
\since build 175
*/
typedef u8 Priority;

/*!
\brief 消息默认有效期。
\since build 175
*/
const std::time_t DefTimeout(0);


/*!
\brief 消息。
\warning 非虚析构。
\since build 145
*/
class Message
{
	friend class MessageQueue;

private:
	ID id; //!< 消息标识。
	ValueObject content; //消息内容句柄。

public:
	/*!
	\brief 构造：使用消息标识和消息内容。
	\since build 320
	*/
	//@{
	Message(ID = 0, const ValueObject& = ValueObject());
	Message(ID, ValueObject&&);
	//@}

	/*!
	\brief 复制构造：默认实现。
	*/
	DefDeCopyCtor(Message)
	/*!
	\brief 转移构造。
	\since build 317
	*/
	Message(Message&&);

	Message&
	operator=(const ValueObject& c)
	{
		content = c;
		return *this;
	}
	/*!
	\brief 成员赋值：使用值类型对象。
	\since build 296
	*/
	Message&
	operator=(ValueObject&& c) ynothrow
	{
		content = std::move(c);
		return *this;
	}
	/*
	\brief 统一赋值：使用值参数和交换函数进行复制或转移赋值。
	\since build 331
	*/
	Message&
	operator=(Message msg) ynothrow
	{
		Swap(msg);
		return *this;
	}

	/*!
	\brief 判断无效性。
	\since build 319
	*/
	PDefHOp(bool, !) const ynothrow
		ImplRet(!bool(*this))

	/*!
	\brief 比较：相等关系。
	*/
	friend bool
	operator==(const Message&, const Message&);
	/*!
	\brief 消息优先级比较函数。
	\since build 272
	*/
	friend bool
	operator<(const Message&, const Message&);

	/*!
	\brief 判断有效性。
	\since build 319
	*/
	explicit DefCvt(const ynothrow, bool, id)

	DefGetter(const ynothrow, ID, MessageID, id) //!< 取消息标识。
	DefGetter(const ynothrow, const ValueObject&, Content, content) \
		//!< 取消息内容。

	/*
	\brief 交换。
	*/
	void
	Swap(Message&) ynothrow;
};


/*!
\brief 消息队列。
\note 使用 multiset 模拟。
\warning 非虚析构。
\since build 211
*/
class MessageQueue : private noncopyable,
	private multimap<Priority, Message, std::greater<Priority>>
{
public:
	typedef size_type SizeType;
	/*!
	\brief 迭代器。
	\since build 317
	*/
	typedef const_iterator Iterator;

	/*!
	\brief 无参数构造：默认实现。
	*/
	inline DefDeCtor(MessageQueue)
	DefEmptyDtor(MessageQueue)

	DefPred(const ynothrow, Empty, empty()) //!< 判断消息队列是否为空。

	DefGetter(const ynothrow, SizeType, Size, size()) //!< 取队列中消息容量。
	/*!
	\brief 取队列起始迭代器。
	\since build 317
	*/
	DefGetter(const ynothrow, Iterator, Begin, begin())
	/*!
	\brief 取队列终止迭代器。
	\since build 317
	*/
	DefGetter(const ynothrow, Iterator, End, end())
	/*!
	\brief 取消息队列中消息的最大优先级。
	\return 若消息队列为空则 0 ，否则为最大优先级。
	\since build 288
	*/
	DefGetter(const ynothrow, Priority, MaxPriority,
		empty() ? 0 : begin()->first)

	/*!
	\brief 清除消息队列。
	*/
	PDefH(void, Clear)
		ImplRet(clear())

	/*!
	\brief 从队列中删除迭代器指定的消息。、
	\pre 迭代器从属于本消息队列。
	\since build 317
	*/
	PDefH(bool, Erase, Iterator i)
		ImplRet(erase(i) != end())

	/*!
	\brief 合并消息队列：移动指定消息队列中的所有消息至此消息队列中。
	*/
	void
	Merge(MessageQueue&);

	/*!
	\brief 从消息队列中取优先级最高的消息存至 msg 中。
	\note 在队列中保留消息；不检查消息是否有效。
	\since build 271
	*/
	void
	Peek(Message& msg) const
	{
		if(YB_LIKELY(!empty()))
			msg = begin()->second;
	}

	/*!
	\brief 丢弃消息队列中优先级最高的消息。
	\note 消息队列为空时忽略。
	*/
	void
	Pop()
	{
		if(YB_LIKELY(!empty()))
			erase(begin());
	}

	/*!
	\brief 若消息有效，以指定优先级插入至消息队列中。
	\since build 317
	*/
	void
	Push(const Message& msg, Priority prior)
	{
		if(msg)
			insert(make_pair(prior, msg));
	}
	/*!
	\brief 若消息有效，以指定优先级插入至消息队列中。
	\since build 317
	*/
	void
	Push(const Message&& msg, Priority prior)
	{
		if(msg)
			insert(make_pair(prior, std::move(msg)));
	}

	/*!
	\brief 移除不大于指定优先级的消息。
	\since build 320
	*/
	void
	Remove(Priority);
};

YSL_END_NAMESPACE(Messaging)

using Messaging::Message;

YSL_END

#endif

