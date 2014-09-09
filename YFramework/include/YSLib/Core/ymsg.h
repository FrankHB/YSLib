/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ymsg.h
\ingroup Core
\brief 消息处理。
\version r1984
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-06 02:44:31 +0800
\par 修改时间:
	2014-09-09 23:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YMessage
*/


#ifndef YSL_INC_Core_ymsg_h_
#define YSL_INC_Core_ymsg_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YObject
#include YFM_YSLib_Adaptor_YContainer
#include <ctime>

namespace YSLib
{

namespace Messaging
{

/*!
\brief 消息标识。
\since build 175
*/
using ID = yimpl(u32);

/*!
\brief 消息优先级。
\since build 175
*/
using Priority = u8;

/*!
\brief 默认消息优先级。
\since build 449
*/
yconstexpr Priority NormalPriority(0x80);


/*!
\brief 消息。
\warning 非虚析构。
\since build 145
*/
class YF_API Message
{
	friend class MessageQueue;

private:
	ID id; //!< 消息标识。
	ValueObject content; //消息内容。

public:
	/*!
	\brief 构造：使用消息标识和空消息内容。
	\since build 368
	*/
	Message(ID msg_id = 0)
		: id(msg_id)
	{}
	/*!
	\brief 构造：使用消息标识和消息内容。
	\since build 449
	*/
	//@{
	//! \since build 368
	Message(ID m, const ValueObject& vo)
		: id(m), content(vo)
	{}
	Message(ID m, ValueObject&& vo) ynothrow
		: id(m), content(std::move(vo))
	{}
	//@}

	/*!
	\brief 复制构造：默认实现。
	*/
	DefDeCopyCtor(Message)
	/*!
	\brief 转移构造：默认实现。
	\since build 368
	*/
	DefDeMoveCtor(Message)

	PDefHOp(Message&, = , const ValueObject& c) ynothrow
		ImplRet(content = c, *this)
	/*!
	\brief 成员赋值：使用值类型对象。
	\since build 296
	*/
	PDefHOp(Message&, =, ValueObject&& c) ynothrow
		ImplRet(content = std::move(c), *this)
	/*
	\brief 合一赋值：使用值参数和交换函数进行复制或转移赋值。
	\since build 331
	*/
	PDefHOp(Message&, =, Message msg) ynothrow
		ImplRet(msg.swap(*this), *this)

	/*!
	\brief 判断无效性。
	\since build 319
	*/
	PDefHOp(bool, !, ) const ynothrow
		ImplRet(!bool(*this))

	/*!
	\brief 比较：相等关系。
	\since build 454
	*/
	YF_API friend bool
	operator==(const Message&, const Message&);

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
	\since build 409
	*/
	void
	swap(Message&) ynothrow;
};

/*!
\relates Message
\since build 409
*/
inline DefSwap(ynothrow, Message)


/*!
\brief 消息队列。
\note 使用 multiset 模拟。
\warning 非虚析构。
\since build 211
*/
class YF_API MessageQueue : private noncopyable,
	private multimap<Priority, Message, std::greater<Priority>>
{
public:
	//! \since build 449
	using BaseType = multimap<Priority, Message, std::greater<Priority>>;
	/*!
	\brief 迭代器。
	\since build 460
	*/
	//@{
	using BaseType::iterator;
	using BaseType::const_iterator;
	//@}

	/*!
	\brief 无参数构造：默认实现。
	*/
	DefDeCtor(MessageQueue)
	DefDeDtor(MessageQueue)

	/*!
	\brief 取消息队列中消息的最大优先级。
	\return 若消息队列为空则 0 ，否则为最大优先级。
	\since build 288
	*/
	DefGetter(const ynothrow, Priority, MaxPriority,
		empty() ? 0 : cbegin()->first)

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
	Peek(Message&) const;

	/*!
	\brief 丢弃消息队列中优先级最高的消息。
	\note 消息队列为空时忽略。
	*/
	void
	Pop();

	/*!
	\brief 若消息有效，以指定优先级插入至消息队列中。
	\since build 317
	*/
	void
	Push(const Message&, Priority);
	/*!
	\brief 若消息有效，以指定优先级插入至消息队列中。
	\since build 454
	*/
	void
	Push(Message&&, Priority);

	/*!
	\brief 移除不大于指定优先级的消息。
	\since build 320
	*/
	void
	Remove(Priority);

	//! \since build 449
	//@{
	using BaseType::begin;

	using BaseType::cbegin;

	using BaseType::cend;

	using BaseType::clear;

	using BaseType::crbegin;

	using BaseType::crend;

	using BaseType::empty;

	using BaseType::end;

	using BaseType::erase;

	using BaseType::insert;

	using BaseType::max_size;

	using BaseType::rbegin;

	using BaseType::rend;

	using BaseType::size;
	//@}
};


/*!
\ingroup exception_types
\brief 消息异常。
\since build 444
*/
class YF_API MessageException : public LoggedEvent
{
public:
	MessageException(const std::string& msg = "")
		: LoggedEvent(msg)
	{}
};


/*!
\ingroup exception_types
\brief 消息信号：表示单一处理中断的异常。
\since build 444
*/
class YF_API MessageSignal : public MessageException
{
public:
	MessageSignal(const std::string& msg = "")
		: MessageException(msg)
	{}
};

} // namespace Messaging;

using Messaging::Message;

} // namespace YSLib;

#endif

