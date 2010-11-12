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
\version 0.1912;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-06 02:44:31 + 08:00;
\par 修改时间:
	2010-11-12 18:32 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShellMessage;
*/


#ifndef INCLUDED_YSMSG_H_
#define INCLUDED_YSMSG_H_

#include "ysdef.h"
#include "ysmsgdef.h"
#include "yobject.h"
#include "../Adaptor/cont.h"
//#include <queue>
#include <ctime>

YSL_BEGIN

using Drawing::Point;

YSL_BEGIN_NAMESPACE(Shells)

typedef u32 MSGID;
typedef u8 MSGPRIORITY;

extern const time_t DEF_TIMEOUT;

class Message : public GMCounter<Message> //!< 消息类。
{
	friend class YMessageQueue;

private:
	HSHL hShl; //!<  Shell 句柄。
	MSGID msg; //!< 消息标识。
	MSGPRIORITY prior; //!< 消息优先级。
	WPARAM wParam; //!< 字长相关参数。
	LPARAM lParam; //!< 字长无关参数。

#ifndef YSLIB_NO_CURSOR

	Point pt; //!< 光标位置。

#endif

	std::clock_t timestamp; //!< 消息时间戳：消息产生的进程时间。
	std::clock_t timeout; //!< 消息有效期。

public:

#ifdef YSLIB_NO_CURSOR

	/*!
	\brief 构造：使用 Shell 句柄、消息标识、消息优先级和参数。
	*/
	Message(HSHL = NULL, MSGID = SM_NULL, MSGPRIORITY = 0,
		WPARAM = 0, const LPARAM = 0);

#else

	/*!
	\brief 构造：使用 Shell 句柄、消息标识、消息优先级、光标位置和参数。
	*/
	Message(HSHL = NULL, MSGID = SM_NULL, MSGPRIORITY = 0,
		WPARAM = 0, const LPARAM = 0, const Point& pt = Point::Zero);

#endif

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
	DefPredicate(Valid, msg) //!< 判断消息是否有效。

	DefGetter(HSHL, ShellHandle, hShl) //!< 取关联的 Shell 句柄。
	DefGetter(MSGID, MsgID, msg) //!< 取消息标识。
	DefGetter(MSGPRIORITY, Priority, prior) //!< 取消息优先级。
	DefGetter(WPARAM, WParam, wParam)
	DefGetter(LPARAM, LParam, lParam)

#ifndef YSLIB_NO_CURSOR

	DefGetter(const Point&, CursorLocation, pt)

#endif

	DefGetter(std::clock_t, Timestamp, timestamp) //!< 取消息时间戳。

	/*!
	\brief 设置消息参数。
	*/
	void
	SetParam(WPARAM, LPARAM);
	DefSetter(std::clock_t, Timestamp, timestamp) //!< 设置消息时间戳。

	/*!
	\brief 更新消息时间戳。
	*/
	void
	UpdateTimestamp();
};

inline bool Message::operator!=(const Message& m) const
{
	return !this->operator==(m);
}

inline void
Message::SetParam(WPARAM wp, LPARAM lp)
{
	wParam = wp;
	lParam = lp;
}

inline void
Message::UpdateTimestamp()
{
	SetTimestamp(std::clock());
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
				return i.GetID() > j.GetID();
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
	InsertMessage(const Message& m);
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

YSL_END_NAMESPACE(Shells)

using Shells::Message;

YSL_END

#endif

