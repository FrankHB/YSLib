// YSLib::Core::YShellMessage by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-6 2:44:31;
// UTime = 2010-9-2 10:23;
// Version = 0.1770;


#ifndef INCLUDED_YSMSG_H_
#define INCLUDED_YSMSG_H_

// YShellMessage ：消息处理模块。

#include "ysdef.h"
#include "ysmsgdef.h"
#include "yobject.h"
#include <queue>
#include <ctime>

YSL_BEGIN

using Drawing::SPoint;

YSL_BEGIN_NAMESPACE(Shells)

typedef u32 MSGID;
typedef u8 MSGPRIORITY;

extern const time_t DEF_TIMEOUT;

class Message : public GMCounter<Message> //消息类。
{
	friend class YMessageQueue;

private:
	HSHL hShl; // Shell 句柄。
	MSGID msg; //消息标识。
	MSGPRIORITY prior; //消息优先级。
	WPARAM wParam; //字长相关参数。
	LPARAM lParam; //字长无关参数。

#ifndef YSLIB_NO_CURSOR

	SPoint pt; //光标位置。

#endif

	std::clock_t timestamp; //消息时间戳：消息产生的进程时间。
	std::clock_t timeout; //消息有效期。

public:

#ifdef YSLIB_NO_CURSOR

	Message(HSHL = NULL, MSGID = SM_NULL, MSGPRIORITY = 0, WPARAM = 0, const LPARAM = 0);

#else

	Message(HSHL = NULL, MSGID = SM_NULL, MSGPRIORITY = 0, WPARAM = 0, const LPARAM = 0, const SPoint& pt = SPoint::Zero);

#endif

	bool operator==(const Message&) const;
	bool operator!=(const Message&) const;

	DefBoolGetter(TimeOut, timestamp + timeout < std::clock()) //判断消息是否过期。
	DefBoolGetter(Valid, msg) //判断消息是否有效。

	DefGetter(HSHL, ShellHandle, hShl) //取关联的 Shell 句柄。
	DefGetter(MSGID, MsgID, msg) //取消息标识。
	DefGetter(MSGPRIORITY, Priority, prior) //取消息优先级。
	DefGetter(WPARAM, WParam, wParam)
	DefGetter(LPARAM, LParam, lParam)

#ifndef YSLIB_NO_CURSOR

	DefGetter(const SPoint&, CursorLocation, pt)

#endif

	DefGetter(std::clock_t, Timestamp, timestamp) //取消息时间戳。

	void
	SetParam(WPARAM, LPARAM); //设置消息参数。
	DefSetter(std::clock_t, Timestamp, timestamp) //设置消息时间戳。

	void
	UpdateTimestamp(); //更新消息时间戳。
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
		bool
		operator ()(const Message& i, const Message& j)
		{
			if(i.prior == j.prior)
			//	return i.time > j.time;
				return i.GetID() > j.GetID();
			return i.prior < j.prior;
		}
	};

	//消息优先队列。
	std::priority_queue<Message, std::vector<Message>, cmp> q;

	PDefHead(const Message&, top) const
		ImplBodyMember(q, top)
	PDefHead(void, push, const Message& msg)
		ImplBodyMemberVoid(q, push, msg)
	PDefHead(void, pop)
		ImplBodyMemberVoid(q, pop)

public:
	typedef std::priority_queue<int, std::vector<int>, cmp>::size_type size_type;

	YMessageQueue(); //默认构造函数：非内联。
	virtual
	~YMessageQueue() ythrow(); //默认析构函数：非内联。

	PDefHead(bool, empty) const //判断消息队列是否为空。
		ImplBodyMember(q, empty)
	PDefHead(size_type, size) const //取队列中消息容量。
		ImplBodyMember(q, size)

	void
	GetMessage(Message& m); //从消息队列中取优先级最高的消息存至 m 中（不在队列中保留消息）。
	void
	PeekMessage(Message& m) const; //从消息队列中取优先级最高的消息存至 m 中（在队列中保留消息）。

	size_type
	Clear(); //清除消息队列，并返回清除的消息数。
	void
	Update(); //更新消息队列。
	bool
	InsertMessage(const Message& m); //若消息 m 有效，插入 m 至消息队列中。返回 m 是否有效。
};


//合并 src 所有消息至 dst 中。
void
Merge(YMessageQueue& dst, std::vector<Message>& src);
void
Merge(YMessageQueue& dst, YMessageQueue& src);


//全局默认消息插入函数。
inline void
InsertMessage(const Message& msg)
{
	DefaultMQ.InsertMessage(msg);

#if YSLIB_DEBUG_MSG & 1

	void YSDebug_MSG_Insert(Message&);
	YSDebug_MSG_Insert(msg);

#endif

}
inline void
InsertMessage(const HSHL& hShl, const MSGID& id, const MSGPRIORITY& prior, const WPARAM& w = 0, const LPARAM& l = 0, const SPoint& pt = SPoint::Zero)
{

#if YSLIB_DEBUG_MSG & 1

	void YSDebug_MSG_Insert(Message&);
	Message msg(hShl, id, prior, w, l);

	DefaultMQ.InsertMessage(msg);
	YSDebug_MSG_Insert(msg);

#else

	DefaultMQ.InsertMessage(Message(hShl, id, prior, w, l, pt));

#endif

}

YSL_END_NAMESPACE(Shells)

using Shells::Message;
using Shells::InsertMessage;

YSL_END

#endif

