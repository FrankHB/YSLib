// YSLib::Core::YShellMessage by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-06 02:44:31 + 08:00;
// UTime = 2010-10-22 13:17 + 08:00;
// Version = 0.1441;


#include "ysmsg.h"
#include "../Shell/ywindow.h" // for HWND delete procedure;

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

const time_t DEF_TIMEOUT(0);

#ifdef YSLIB_NO_CURSOR

Message::Message(HSHL shl, MSGID m, MSGPRIORITY p, WPARAM w, const LPARAM l)
	: hShl(shl), msg(m), prior(p), wParam(w), lParam(l), timestamp(std::clock()), timeout(DEF_TIMEOUT)
{}

#else

Message::Message(HSHL shl, MSGID m, MSGPRIORITY p, WPARAM w, const LPARAM l, const Point& _pt)
	: hShl(shl), msg(m), prior(p), wParam(w), lParam(l), pt(_pt), timestamp(std::clock()), timeout(DEF_TIMEOUT)
{}

#endif

bool Message::operator==(const Message& m) const
{

#ifdef YSLIB_NO_CURSOR

	return hShl == m.hShl && msg == m.msg && prior == m.prior && wParam == m.wParam && lParam == m.lParam;

#else

	return hShl == m.hShl && msg == m.msg && prior == m.prior && wParam == m.wParam && lParam == m.lParam && pt == m.pt;

#endif

}


YMessageQueue::YMessageQueue()
	: q()
{}
YMessageQueue::~YMessageQueue() ythrow()
{}

void
YMessageQueue::GetMessage(Message& m)
{
	if(!q.empty())
	{
		if(q.top().IsValid())
			m = q.top();
		q.pop();
	}
}

void
YMessageQueue::PeekMessage(Message& m) const
{
	if(!q.empty())
		if(q.top().IsValid())
			m = q.top();
}

YMessageQueue::size_type
YMessageQueue::Clear()
{
	size_type n = 0;

	while(!empty())
	{
		q.pop();
		++n;
	}
	return n;
}
void
YMessageQueue::Update()
{
	if(!q.empty())
	{
		if(q.top().IsTimeOut())
			q.pop();
	}
}

bool
YMessageQueue::InsertMessage(const Message& m)
{
	if(m.IsValid())
		q.push(m);
	return m.IsValid();
}

void
Merge(YMessageQueue& dst, vector<Message>& src)
{
	Message m;

	while(!src.empty())
	{
		m = src[src.size() - 1];
		dst.InsertMessage(m);
		src.pop_back();
	}
}
void
Merge(YMessageQueue& dst, YMessageQueue& src)
{
	Message m;

	while(!src.empty())
	{
		src.GetMessage(m);
		dst.InsertMessage(m);
	}
}

YSL_END_NAMESPACE(Shells)

YSL_END

