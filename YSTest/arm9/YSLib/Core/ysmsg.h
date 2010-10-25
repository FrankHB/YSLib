// YSLib::Core::YShellMessage by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-06 02:44:31 + 08:00;
// UTime = 2010-10-24 19:55 + 08:00;
// Version = 0.1887;


#ifndef INCLUDED_YSMSG_H_
#define INCLUDED_YSMSG_H_

// YShellMessage ：消息处理模块。

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

	Point pt; //光标位置。

#endif

	std::clock_t timestamp; //消息时间戳：消息产生的进程时间。
	std::clock_t timeout; //消息有效期。

public:

#ifdef YSLIB_NO_CURSOR

	//********************************
	//名称:		Message
	//全名:		YSLib::Shells::Message::Message
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	HSHL
	//形式参数:	MSGID
	//形式参数:	MSGPRIORITY
	//形式参数:	WPARAM
	//形式参数:	const LPARAM
	//功能概要:	构造：使用 Shell 句柄、消息标识、消息优先级和参数。
	//备注:		
	//********************************
	Message(HSHL = NULL, MSGID = SM_NULL, MSGPRIORITY = 0,
		WPARAM = 0, const LPARAM = 0);

#else

	//********************************
	//名称:		Message
	//全名:		YSLib::Shells::Message::Message
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	HSHL
	//形式参数:	MSGID
	//形式参数:	MSGPRIORITY
	//形式参数:	WPARAM
	//形式参数:	const LPARAM
	//形式参数:	const Point & pt
	//功能概要:	构造：使用 Shell 句柄、消息标识、消息优先级、光标位置和参数。
	//备注:		
	//********************************
	Message(HSHL = NULL, MSGID = SM_NULL, MSGPRIORITY = 0,
		WPARAM = 0, const LPARAM = 0, const Point& pt = Point::Zero);

#endif

	//********************************
	//名称:		operator==
	//全名:		YSLib::Shells::Message::operator==
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const Message &
	//功能概要:	比较：相等关系。
	//备注:		
	//********************************
	bool
	operator==(const Message&) const;
	//********************************
	//名称:		operator!=
	//全名:		YSLib::Shells::Message::operator!=
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const Message &
	//功能概要:	比较：不等关系。
	//备注:		
	//********************************
	bool
	operator!=(const Message&) const;

	DefPredicate(TimeOut, timestamp + timeout < std::clock()) //判断消息是否过期。
	DefPredicate(Valid, msg) //判断消息是否有效。

	DefGetter(HSHL, ShellHandle, hShl) //取关联的 Shell 句柄。
	DefGetter(MSGID, MsgID, msg) //取消息标识。
	DefGetter(MSGPRIORITY, Priority, prior) //取消息优先级。
	DefGetter(WPARAM, WParam, wParam)
	DefGetter(LPARAM, LParam, lParam)

#ifndef YSLIB_NO_CURSOR

	DefGetter(const Point&, CursorLocation, pt)

#endif

	DefGetter(std::clock_t, Timestamp, timestamp) //取消息时间戳。

	//********************************
	//名称:		SetParam
	//全名:		YSLib::Shells::Message::SetParam
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	WPARAM
	//形式参数:	LPARAM
	//功能概要:	设置消息参数。
	//备注:		
	//********************************
	void
	SetParam(WPARAM, LPARAM);
	DefSetter(std::clock_t, Timestamp, timestamp) //设置消息时间戳。

	//********************************
	//名称:		UpdateTimestamp
	//全名:		YSLib::Shells::Message::UpdateTimestamp
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	更新消息时间戳。
	//备注:		
	//********************************
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
		//********************************
		//名称:		operator()
		//全名:		YSLib::Shells::YMessageQueue::cmp::operator()
		//可访问性:	public 
		//返回类型:	bool
		//修饰符:	
		//形式参数:	const Message & i
		//形式参数:	const Message & j
		//功能概要:	调用比较函数。
		//备注:		
		//********************************
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

	//********************************
	//名称:		YMessageQueue
	//全名:		YSLib::Shells::YMessageQueue::YMessageQueue
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		非内联。
	//********************************
	YMessageQueue();
	//********************************
	//名称:		~YMessageQueue
	//全名:		YSLib::Shells::YMessageQueue::~YMessageQueue
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//			非内联。
	//********************************
	virtual
	~YMessageQueue() ythrow();

	PDefH(bool, empty) const //判断消息队列是否为空。
		ImplBodyMember(q, empty)
	PDefH(size_type, size) const //取队列中消息容量。
		ImplBodyMember(q, size)
		
	//********************************
	//名称:		GetMessage
	//全名:		YSLib::Shells::YMessageQueue::GetMessage
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	Message & m
	//功能概要:	从消息队列中取优先级最高的消息存至 m 中。
	//备注:		不在消息队列中保留消息。
	//********************************
	void
	GetMessage(Message& m);

	//********************************
	//名称:		PeekMessage
	//全名:		YSLib::Shells::YMessageQueue::PeekMessage
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	const
	//形式参数:	Message & m
	//功能概要:	从消息队列中取优先级最高的消息存至 m 中。
	//备注:		在队列中保留消息。
	//********************************
	void
	PeekMessage(Message& m) const;

	//********************************
	//名称:		Clear
	//全名:		YSLib::Shells::YMessageQueue::Clear
	//可访问性:	public 
	//返回类型:	YSLib::Shells::YMessageQueue::size_type
	//修饰符:	
	//功能概要:	清除消息队列，返回清除的消息数。
	//备注:		
	//********************************
	size_type
	Clear();

	//********************************
	//名称:		Update
	//全名:		YSLib::Shells::YMessageQueue::Update
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	更新消息队列。
	//备注:		
	//********************************
	void
	Update();

	//********************************
	//名称:		InsertMessage
	//全名:		YSLib::Shells::YMessageQueue::InsertMessage
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	const Message & m
	//功能概要:	若消息 m 有效，插入 m 至消息队列中。返回 m 是否有效。
	//备注:		
	//********************************
	bool
	InsertMessage(const Message& m);
};


//********************************
//名称:		Merge
//全名:		YSLib::Shells::Merge
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	YMessageQueue & dst
//形式参数:	vector<Message> & src
//功能概要:	合并 src 所有消息至 dst 中。
//备注:		
//********************************
void
Merge(YMessageQueue& dst, vector<Message>& src);
//********************************
//名称:		Merge
//全名:		YSLib::Shells::Merge
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	YMessageQueue & dst
//形式参数:	YMessageQueue & src
//功能概要:	合并 src 所有消息至 dst 中。
//备注:		
//********************************
void
Merge(YMessageQueue& dst, YMessageQueue& src);

YSL_END_NAMESPACE(Shells)

using Shells::Message;

YSL_END

#endif

