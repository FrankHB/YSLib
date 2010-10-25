// YSLib::Core::YShell by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-13 21:09:15 + 08:00;
// UTime = 2010-10-24 19:55 + 08:00;
// Version = 0.2606;


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
	//********************************
	//名称:		YShell
	//全名:		YSLib::Shells::YShell::YShell
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		发送 SM_CREATE 消息。
	//********************************
	YShell();
	//********************************
	//名称:		~YShell
	//全名:		YSLib::Shells::YShell::~YShell
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//			发送 SM_DESTROY 消息。
	//********************************
	virtual
	~YShell() ythrow();

	//********************************
	//名称:		IsActive
	//全名:		YSLib::Shells::YShell::IsActive
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//功能概要:	判断 Shell 是否处于激活状态。
	//备注:		
	//********************************
	bool
	IsActive() const;

	//********************************
	//名称:		Activate
	//全名:		YSLib::Shells::YShell::Activate
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	激活 Shell 对象： ShlProc 控制权转移给此对象以维持单线程运行。
	//备注:		
	//********************************
	bool
	Activate();

	//********************************
	//名称:		operator+=
	//全名:		YSLib::Shells::YShell::operator+=
	//可访问性:	ImplI(GIContainer<IWindow>) public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	IWindow &
	//功能概要:	向窗口组添加窗口对象。
	//备注:		
	//********************************
	ImplI(GIContainer<IWindow>) void
	operator+=(IWindow&);
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Shells::YShell::operator-=
	//可访问性:	ImplI(GIContainer<IWindow>) public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	IWindow &
	//功能概要:	从窗口组中移除指定窗口对象。
	//备注:		
	//********************************
	ImplI(GIContainer<IWindow>) bool
	operator-=(IWindow&);
	//********************************
	//名称:		RemoveAll
	//全名:		YSLib::Shells::YShell::RemoveAll
	//可访问性:	public 
	//返回类型:	WNDs::size_type
	//修饰符:	
	//形式参数:	IWindow &
	//功能概要:	从窗口组中移除所有指定窗口对象，返回移除的对象数。
	//备注:		
	//********************************
	WNDs::size_type
	RemoveAll(IWindow&);
	//********************************
	//名称:		RemoveWindow
	//全名:		YSLib::Shells::YShell::RemoveWindow
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	移除窗口队列中首个窗口对象。
	//备注:		
	//********************************
	void
	RemoveWindow();

	//********************************
	//名称:		GetFirstWindowHandle
	//全名:		YSLib::Shells::YShell::GetFirstWindowHandle
	//可访问性:	public 
	//返回类型:	YSLib::HWND
	//修饰符:	const
	//功能概要:	取得窗口组中首个窗口对象的句柄。
	//备注:		
	//********************************
	HWND
	GetFirstWindowHandle() const;
	//********************************
	//名称:		GetTopWindowHandle
	//全名:		YSLib::Shells::YShell::GetTopWindowHandle
	//可访问性:	public 
	//返回类型:	YSLib::HWND
	//修饰符:	const
	//功能概要:	取得窗口组中顶端窗口对象的句柄。
	//备注:		
	//********************************
	HWND
	GetTopWindowHandle() const;
	//********************************
	//名称:		GetTopWindowHandle
	//全名:		YSLib::Shells::YShell::GetTopWindowHandle
	//可访问性:	public 
	//返回类型:	YSLib::HWND
	//修饰符:	const
	//形式参数:	YDesktop &
	//形式参数:	const Point &
	//功能概要:	取得窗口组中指定屏幕的指定的点所处的最顶层窗口对象的句柄。
	//备注:		
	//********************************
	HWND
	GetTopWindowHandle(YDesktop&, const Point&) const;

	//********************************
	//名称:		SendWindow
	//全名:		YSLib::Shells::YShell::SendWindow
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	IWindow &
	//功能概要:	向屏幕发送指定窗口对象。
	//备注:		
	//********************************
	bool
	SendWindow(IWindow&);

	//********************************
	//名称:		DispatchWindows
	//全名:		YSLib::Shells::YShell::DispatchWindows
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	向屏幕分发窗口对象。
	//备注:		
	//********************************
	void
	DispatchWindows();

	//********************************
	//名称:		ClearScreenWindows
	//全名:		YSLib::Shells::YShell::ClearScreenWindows
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	YDesktop &
	//功能概要:	清除指定屏幕中属于窗口组的窗口对象。
	//备注:		
	//********************************
	void
	ClearScreenWindows(YDesktop&);

	//********************************
	//名称:		DefShlProc
	//全名:		YSLib::Shells::YShell::DefShlProc
	//可访问性:	public static 
	//返回类型:	LRES
	//修饰符:	
	//形式参数:	const Message &
	//功能概要:	默认 Shell 处理函数。
	//备注:		
	//********************************
	static LRES
	DefShlProc(const Message&);

	// Shell 处理函数：响应线程的直接调用。
	virtual PDefH(LRES, ShlProc, const Message& m)
		ImplRet(DefShlProc(m))

	//********************************
	//名称:		OnActivated
	//全名:		YSLib::Shells::YShell::OnActivated
	//可访问性:	virtual public 
	//返回类型:	LRES
	//修饰符:	
	//形式参数:	const Message &
	//功能概要:	处理线程的激活。
	//备注:		
	//********************************
	virtual LRES
	OnActivated(const Message&);

	//********************************
	//名称:		OnDeactivated
	//全名:		YSLib::Shells::YShell::OnDeactivated
	//可访问性:	virtual public 
	//返回类型:	LRES
	//修饰符:	
	//形式参数:	const Message &
	//功能概要:	处理线程的停用。
	//备注:		
	//********************************
	virtual LRES
	OnDeactivated(const Message&);
};


class YShellMain : /*public GMCounter<YShellMain>*/ public YShell
{
public:
	typedef YShell ParentType;

	//********************************
	//名称:		YShellMain
	//全名:		YSLib::Shells::YShellMain::YShellMain
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		向应用程序对象添加自身。
	//********************************
	YShellMain();
	//********************************
	//名称:		~YShellMain
	//全名:		YSLib::Shells::YShellMain::~YShellMain
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//			空实现。
	//********************************
	virtual
	~YShellMain() ythrow();

	//********************************
	//名称:		ShlProc
	//全名:		YSLib::Shells::YShellMain::ShlProc
	//可访问性:	virtual public 
	//返回类型:	LRES
	//修饰符:	
	//形式参数:	const Message &
	//功能概要:	Shell 处理函数。
	//备注:		
	//********************************
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


// Shell 处理函数句柄。
struct HShellProc : public GHBase<PFSHLPROC>
{
	//********************************
	//名称:		HShellProc
	//全名:		YSLib::Shells::HShellProc::HShellProc
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const PFSHLPROC pf
	//功能概要:	构造：使用函数指针。
	//备注:		
	//********************************
	HShellProc(const PFSHLPROC pf = NULL)
	: GHBase<PFSHLPROC>(pf)
	{}

	//********************************
	//名称:		operator()
	//全名:		YSLib::Shells::HShellProc::operator()
	//可访问性:	public 
	//返回类型:	LRES
	//修饰符:	const
	//形式参数:	const Message & msg
	//功能概要:	调用函数。
	//备注:		
	//********************************
	LRES
	operator()(const Message& msg) const
	{
		if(GetPtr())
			return GetPtr()(msg);
		return -1;
	}
};


//********************************
//名称:		PostQuitMessage
//全名:		YSLib::Shells::PostQuitMessage
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	int
//功能概要:	发起 Shell 终止请求。
//备注:		
//********************************
void
PostQuitMessage(int);

//********************************
//名称:		DefShellProc
//全名:		YSLib::Shells::DefShellProc
//可访问性:	public 
//返回类型:	LRES
//修饰符:	
//形式参数:	const Message & msg
//功能概要:	默认 Shell 处理函数。
//备注:		调用默认 Shell 函数为应用程序没有处理的 Shell 消息提供默认处理，
//			确保每一个消息得到处理。
//********************************
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
	PM_NOREMOVE：PeekMessage处理后，消息不从消息队列中清除。
	PM_REMOVE：PeekMessage处理后，消息从消息队列中清除。
*/

#define PM_NOREMOVE 0x0
#define PM_REMOVE 0x1

//********************************
//名称:		PeekMessage
//全名:		YSLib::Shells::PeekMessage
//可访问性:	public 
//返回类型:	IRES
//修饰符:	
//形式参数:	Message & msg
//形式参数:	HSHL hShl
//形式参数:	MSGID wMsgFilterMin
//形式参数:	MSGID wMsgFilterMax
//形式参数:	u32 wRemoveMsg
//功能概要:	从全局消息队列中取消息。
//备注:		
//********************************
IRES
PeekMessage(Message& msg, HSHL hShl = NULL, MSGID wMsgFilterMin = 0, MSGID wMsgFilterMax = 0, u32 wRemoveMsg = PM_NOREMOVE);

//********************************
//名称:		GetMessage
//全名:		YSLib::Shells::GetMessage
//可访问性:	public 
//返回类型:	IRES
//修饰符:	
//形式参数:	Message & msg
//形式参数:	HSHL hShl
//形式参数:	MSGID wMsgFilterMin
//形式参数:	MSGID wMsgFilterMax
//功能概要:	从全局消息队列中取消息。
//备注:		取得的消息从消息队列中清除。
//********************************
IRES
GetMessage(Message& msg, HSHL hShl = NULL, MSGID wMsgFilterMin = 0, MSGID wMsgFilterMax = 0);

//********************************
//名称:		TranslateMessage
//全名:		YSLib::Shells::TranslateMessage
//可访问性:	public 
//返回类型:	ERRNO
//修饰符:	
//形式参数:	const Message & msg
//功能概要:	翻译消息。
//备注:		空实现。
//********************************
ERRNO
TranslateMessage(const Message& msg);

//********************************
//名称:		DispatchMessage
//全名:		YSLib::Shells::DispatchMessage
//可访问性:	public 
//返回类型:	LRES
//修饰符:	
//形式参数:	const Message & msg
//功能概要:	分发消息。
//备注:		
//********************************
LRES
DispatchMessage(const Message& msg);

//********************************
//名称:		BackupMessage
//全名:		YSLib::Shells::BackupMessage
//可访问性:	public 
//返回类型:	ERRNO
//修饰符:	
//形式参数:	const Message & msg
//功能概要:	备份主消息队列中的消息。
//备注:		
//********************************
ERRNO
BackupMessage(const Message& msg);

//********************************
//名称:		RecoverMessageQueue
//全名:		YSLib::Shells::RecoverMessageQueue
//可访问性:	public 
//返回类型:	void
//修饰符:	
//功能概要:	从备份消息队列恢复所有消息。
//备注:		
//********************************
void
RecoverMessageQueue();

YSL_END_NAMESPACE(Shells)

YSL_END

#endif

