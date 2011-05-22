/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ysmsg.h
\ingroup Core
\brief 消息处理。
\version 0.2308;
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


#ifndef YSL_INC_CORE_YSMSG_H_
#define YSL_INC_CORE_YSMSG_H_

#include "yobject.h"
#include "../Adaptor/cont.h"
#include <ctime>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Messaging)

typedef u32 ID;
typedef u8 Priority;

const std::time_t DefTimeout(0);


//! \brief 消息内容类。
class Content
{
public:
	typedef enum
	{
	//	Create = 0,
		Destroy = 1,
		Clone = 2,
		Equality = 3
	} OpType;
	typedef bool (*ManagerType)(void*&, void*&, OpType);

private:
	template<typename _type>
	struct GManager
	{
		static bool
		Do(void*& lhs, void*& rhs, OpType op)
		{
			switch(op)
			{
		//	case Create:
		//		lhs = new _type();
		//		return false;
			case Destroy:
				delete static_cast<_type*>(lhs);
				return false;
			case Clone:
				YAssert(rhs, "Null pointer found"
					" @ Messaging::Content::GManager::Do#Clone;");

				lhs = new _type(*static_cast<_type*>(rhs));
				return false;
			case Equality:
				YAssert(lhs && rhs, "Null pointer found"
					" @ Messaging::Content::GManager::Do#Equlitiy;");

				return *static_cast<_type*>(lhs) == *static_cast<_type*>(rhs);
			default:
				return false;
			}
			return false;
		}
	};

	ManagerType manager;
	mutable void* obj_ptr;

public:
	/*!
	\brief 无参数构造。
	\note 得到空实例。
	*/
	Content();
	/*!
	\brief 构造：使用对象引用。
	\note 对象需要是可复制构造的。
	\note 得到包含指定对象的实例。
	*/
	template<typename _type>
	Content(const _type& obj)
		: manager(&GManager<_type>::Do), obj_ptr(new _type(obj))
	{}
	Content(const Content&);
	Content(Content&&);
	~Content();

	Content&
	operator=(const Content&);
	Content&
	operator=(Content&&);

	bool
	operator==(const Content&) const;

	template<typename _type>
	const _type&
	GetObject() const
	{
		YAssert(obj_ptr, "Null pointer found @ Messaging::Content::GetObject;");
		YAssert(GManager<_type>::Do == manager, "Invalid type found"
			" @ Messaging::Content::GetObject;");

		return *static_cast<const _type*>(obj_ptr);
	}

	void
	Clear();

	void
	Swap(Content&);
};

inline
Content::Content()
	: manager(nullptr), obj_ptr(nullptr)
{}
inline
Content::~Content()
{
	Clear();
}

inline Content&
Content::operator=(const Content& c)
{
	Content(c).Swap(*this);
	return *this;
}


//! \brief 消息类。
class Message : public GMCounter<Message>
{
	friend class MessageQueue;

private:
	shared_ptr<YShell> hShl; //!< 目的 Shell 句柄。
	ID id; //!< 消息标识。
	Priority prior; //!< 消息优先级。
	Content content; //消息内容句柄。

public:
	std::clock_t timestamp; //!< 消息时间戳：消息产生的进程时间。

private:
	std::clock_t timeout; //!< 消息有效期。

public:
	/*!
	\brief 构造：使用 Shell 句柄、消息标识、消息优先级和消息内容。
	*/
	Message(const shared_ptr<YShell>& = shared_ptr<YShell>(), ID = 0,
		Priority = 0, const Content& = Content());

	/*!
	\brief 复制构造：默认实现。
	*/
	Message(const Message&) = default;
	/*!
	\brief 移动构造：默认实现。
	*/
	Message(Message&&) = default;

	/*
	\brief 复制赋值。
	*/
	Message&
	operator=(const Message&);
	/*
	\brief 移动赋值。
	*/
	Message&
	operator=(Message&&);
	Message&
	operator=(const Content&);
	Message&
	operator=(Content&&);

	/*!
	\brief 比较：相等关系。
	*/
	friend bool
	operator==(const Message&, const Message&);

	DefPredicate(TimeOut, timestamp + timeout < std::clock()) \
		//!< 判断消息是否过期。
	DefPredicate(Valid, id) //!< 判断消息是否有效。

	DefGetter(shared_ptr<YShell>, ShellHandle, hShl) //!< 取关联的 Shell 句柄。
	DefGetter(ID, MessageID, id) //!< 取消息标识。
	DefGetter(Priority, Priority, prior) //!< 取消息优先级。
	DefGetter(const Content&, Content, content) //!< 取消息内容句柄。

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
Message::operator=(const Content& c)
{
	content = c;
	return *this;
}
inline Message&
Message::operator=(Content&& c)
{
	content = std::move(c);
	return *this;
}

inline void
Message::UpdateTimestamp()
{
	timestamp = std::clock();
}


//消息队列类。
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

	PDefH0(const Message&, top) const
		ImplBodyMember0(q, top)
	PDefH1(void, push, const Message& msg)
		ImplBodyMember1(q, push, msg)
	PDefH0(void, pop)
		ImplBodyMember0(q, pop)

public:
	typedef priority_queue<int, vector<int>, cmp>::size_type SizeType;

	/*!
	\brief 无参数构造：默认实现。
	*/
	inline
	MessageQueue() = default;
	virtual DefEmptyDtor(MessageQueue)

	DefPredicate(Empty, q.empty()) //!< 判断消息队列是否为空。

	DefGetter(SizeType, Size, q.size()) //!< 取队列中消息容量。
	/*!
	\brief 取消息队列中取优先级最高的消息。
	\note 不在消息队列中保留消息。
	*/
	Message
	FetchMessage();

	/*!
	\brief 从消息队列中取优先级最高的消息存至 msg 中。
	\note 在队列中保留消息。
	*/
	void
	PeekMessage(Message& msg) const;

	/*!
	\brief 清除消息队列，返回清除的消息数。
	*/
	SizeType
	Clear();

	/*!
	\brief 更新消息队列。
	*/
	void
	Update();

	/*!
	\brief 若消息 msg 有效，插入 msg 至消息队列中。返回 msg 是否有效。
	*/
	bool
	Insert(const Message& msg);
};


/*!
\brief 合并 src 所有消息至 dst 中。
*/
void
Merge(MessageQueue& dst, list<Message>& src);
/*!
\brief 合并 src 所有消息至 dst 中。
*/
void
Merge(MessageQueue& dst, MessageQueue& src);

YSL_END_NAMESPACE(Messaging)

using Messaging::Message;

YSL_END

#endif

