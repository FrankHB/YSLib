// YSLib::Core::YShell by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-13 21:09:15 + 08:00;
// UTime = 2010-10-17 00:31 + 08:00;
// Version = 0.2472;


#ifndef INCLUDED_YSHELL_H_
#define INCLUDED_YSHELL_H_

// Shell ： Shell 定义。

#include "../Helper/yglobal.h"
#include "yfunc.hpp"
//#include <set>
//#include <list>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

//外壳程序：实现线程语义。
class YShell : public YObject,
	implements GIContainer<IWindow>
{
public:
	typedef list<HWND> WNDs;

private:
	WNDs sWnds; //窗口组。

public:
	YShell();
	virtual
	~YShell() ythrow();

	bool
	IsActive() const; //判断 Shell 是否处于激活状态。

	bool
	Activate(); //激活 Shell 对象： shlProc 控制权转移给此对象以维持单线程运行。

	ImplI(GIContainer<IWindow>) void
	operator+=(IWindow&); //向窗口组添加窗口对象。
	ImplI(GIContainer<IWindow>) bool
	operator-=(IWindow&); //从窗口组中移除指定窗口对象。
	WNDs::size_type
	RemoveAll(IWindow&); //从窗口组中移除所有指定窗口对象，返回移除的对象数。
	void RemoveWindow(); //移除窗口队列中首个窗口对象。
	HWND
	GetFirstWindowHandle() const; //取得窗口组中首个窗口对象的句柄。
	HWND
	GetTopWindowHandle() const; //取得窗口组中顶端窗口对象的句柄。
	HWND
	GetTopWindowHandle(YDesktop&, const Point&) const; //取得窗口组中指定屏幕的指定的点所处的最顶层窗口对象的句柄。

	bool
	SendWindow(IWindow&); //向屏幕发送指定窗口对象。

	void
	DispatchWindows(); //向屏幕发送窗口对象。

	void
	ClearScreenWindows(YDesktop&); //清除指定屏幕中属于窗口组的窗口对象。

	static LRES
	DefShlProc(const Message&); //默认 Shell 处理函数。

	virtual PDefH(LRES, ShlProc, const Message& m) // Shell 处理函数：响应线程的直接调用。
		ImplRet(DefShlProc(m))

	virtual LRES
	OnActivated(const Message&); // Shell 处理函数：响应线程的激活。

	virtual LRES
	OnDeactivated(const Message&); // Shell 处理函数：响应线程的撤销。
};


class YShellMain : /*public GMCounter<YShellMain>*/ public YShell
{
public:
	typedef YShell ParentType;

	YShellMain();
	virtual
	~YShellMain() ythrow();

	virtual LRES
	ShlProc(const Message&);
};

inline
YShellMain::~YShellMain() ythrow()
{}

inline LRES
Shells::YShellMain::ShlProc(const Message& msg)
{
	return DefaultMainShlProc(msg);
}


typedef LRES FSHLPROC(const Message&);
typedef FSHLPROC* PFSHLPROC;


struct HShellProc : public GHBase<PFSHLPROC>
{
	HShellProc(const PFSHLPROC pf = NULL)
	: GHBase<PFSHLPROC>(pf)
	{}

	LRES
	operator()(const Message& msg) const
	{
		if(GetPtr())
			return GetPtr()(msg);
		return -1;
	}
};


// Shell 终止请求。
void
PostQuitMessage(int);

//默认 Shell 处理函数：调用默认 Shell 过程来为应用程序没有处理的任何 Shell 消息提供缺省的处理，以确保每一个消息得到处理。
inline LRES
DefShellProc(const Message& msg)
{
	return YShell::DefShlProc(msg);
}

/*
PeekMessage
//从全局消息队列中取消息。
lpMsg：接收消息信息的 Message 结构指针。
hShl：消息关联（发送目标）的 Shell 的句柄，为 NULL 时无限制（为全局对象）。
wMsgFilterMin：指定被检查的消息范围里的第一个消息。
wMsgFilterMax：指定被检查的消息范围里的最后一个消息。
wRemoveMsg：确定消息如何被处理。此参数可取下列值之一：
	PM_NOREMOVE：PeekMessage处理后，消息不从队列中清除。
	PM_REMOVE：PeekMessage处理后，消息从队列中清除。
*/
#define PM_NOREMOVE 0x0
#define PM_REMOVE 0x1
IRES
PeekMessage(Message& msg, HSHL hShl = NULL, MSGID wMsgFilterMin = 0, MSGID wMsgFilterMax = 0, u32 wRemoveMsg = PM_NOREMOVE);

IRES
GetMessage(Message& msg, HSHL hShl = NULL, MSGID wMsgFilterMin = 0, MSGID wMsgFilterMax = 0);

ERRNO
TranslateMessage(const Message& msg);

LRES
DispatchMessage(const Message& msg);

//备份队列消息。
ERRNO
BackupMessage(const Message& msg);

//恢复消息队列。
void
RecoverMessageQueue();

YSL_END_NAMESPACE(Shells)

YSL_END

#endif

