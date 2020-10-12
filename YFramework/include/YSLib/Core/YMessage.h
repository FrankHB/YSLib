/*
	© 2009-2018, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YMessage.h
\ingroup Core
\brief 消息处理。
\version r2076
\author FrankHB <frankhb1989@gmail.com>
\since build 586
\par 创建时间:
	2009-12-06 02:44:31 +0800
\par 修改时间:
	2020-10-06 21:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YMessage
*/


#ifndef YSL_INC_Core_YMessage_h_
#define YSL_INC_Core_YMessage_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YObject // for std::piecewise_construct,
//	YSLib::tuple<Priority, YSLib::forward_as_tuple;
#include <ctime>

namespace YSLib
{

namespace Messaging
{

/*!
\brief 消息标识。
\since build 175
*/
using ID = yimpl(std::uint32_t);

/*!
\brief 消息优先级。
\since build 175
*/
using Priority = std::uint8_t;

/*!
\brief 默认消息优先级。
\since build 449
*/
yconstexpr const Priority NormalPriority(0x80);


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
	ValueObject content{}; //消息内容。

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
	//! \since build 768
	//@{
	//! \brief 复制赋值：使用复制和交换。
	PDefHOp(Message&, =, const Message& msg)
		ImplRet(ystdex::copy_and_swap(*this, msg))
	//! \brief 转移赋值：使用交换。
	DefDeMoveAssignment(Message)
	//@}

	/*!
	\brief 判断无效或有效性。
	\since build 319
	*/
	DefBoolNeg(explicit, id)

	/*!
	\brief 比较：相等关系。
	\since build 454
	*/
	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const Message& x, const Message& y)
		ImplRet(x.id == y.id && x.content == y.content)

	//! \brief 取消息标识。
	DefGetter(const ynothrow, ID, MessageID, id)
	//! \brief 取消息内容。
	DefGetter(const ynothrow, const ValueObject&, Content, content) \

	/*!
	\brief 交换。
	\since build 710
	*/
	YF_API friend void
	swap(Message&, Message&) ynothrow;
};


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
	using BaseType = YSLib::multimap<Priority, Message, std::greater<Priority>>;
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
	\brief 以参数指定的优先级插入按指定的参数构造的消息。
	\since build 888
	*/
	template<typename... _tParams>
	void
	Emplace(Priority prior, _tParams&&... args)
	{
		emplace(std::piecewise_construct, YSLib::tuple<Priority>(prior),
			YSLib::forward_as_tuple(yforward(args)...));
	}

	/*!
	\brief 合并消息队列：移动指定消息队列中的所有消息至此消息队列中。
	*/
	void
	Merge(MessageQueue&);

	/*!
	\brief 从消息队列中取优先级最高的消息存至参数指定的消息中。
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
	\brief 以指定优先级插入参数指定的消息至消息队列中。
	\since build 317
	*/
	void
	Push(const Message&, Priority);
	/*!
	\brief 以指定优先级插入参数指定的消息至消息队列中。
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
\ingroup exceptions
\brief 消息异常。
\since build 444
*/
class YF_API MessageException : public LoggedEvent
{
public:
	//! \since build 538
	using LoggedEvent::LoggedEvent;

	//! \since build 586
	DefDeCopyCtor(MessageException)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~MessageException() override;
};


/*!
\ingroup exceptions
\brief 消息信号：表示单一处理中断的异常。
\since build 444
*/
class YF_API MessageSignal : public MessageException
{
public:
	//! \since build 833
	using MessageException::MessageException;
	//! \since build 586
	//@{
	DefDeCopyCtor(MessageSignal)
	//! \brief 虚析构：类定义外默认实现。
	~MessageSignal() override;
	//@}
};

} // namespace Messaging;

using Messaging::Message;

} // namespace YSLib;

#endif

