// YSLib::Core::YApplication by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-27 17:12:27 + 08:00;
// UTime = 2010-10-24 17:12 + 08:00;
// Version = 0.1884;


#ifndef INCLUDED_YAPP_H_
#define INCLUDED_YAPP_H_

// YApplication ：应用程序实例类抽象。

#include "ydevice.h"
#include "yshell.h"
#include "yfilesys.h"
#include "yevt.hpp"

YSL_BEGIN

//程序日志。
class YLog : public YObject
{
public:
	//********************************
	//名称:		YLog
	//全名:		YSLib::YLog::YLog
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	构造。
	//备注:		
	//********************************
	YLog();
	//********************************
	//名称:		~YLog
	//全名:		YSLib::YLog::~YLog
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	
	//功能概要:	析构。
	//备注:		
	//********************************
	virtual
	~YLog();

	//********************************
	//名称:		operator<<
	//全名:		YSLib::YLog::operator<<
	//可访问性:	public 
	//返回类型:	YLog&
	//修饰符:	
	//形式参数:	char
	//功能概要:	输出 char 字符。
	//备注:		
	//********************************
	YLog&
	operator<<(char);
	//********************************
	//名称:		operator<<
	//全名:		YSLib::YLog::operator<<
	//可访问性:	public 
	//返回类型:	YLog&
	//修饰符:	
	//形式参数:	const char *
	//功能概要:	输出字符指针表示的字符串。
	//备注:		
	//********************************
	YLog&
	operator<<(const char*);
	//********************************
	//名称:		operator<<
	//全名:		YSLib::YLog::operator<<
	//可访问性:	public 
	//返回类型:	YLog&
	//修饰符:	
	//形式参数:	const string &
	//功能概要:	输出字符串。
	//备注:		
	//********************************
	YLog&
	operator<<(const string&);

	//********************************
	//名称:		Error
	//全名:		YSLib::YLog::Error
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const char *
	//功能概要:	提示错误。
	//备注:		
	//********************************
	void
	Error(const char*);
	//********************************
	//名称:		Error
	//全名:		YSLib::YLog::Error
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const string &
	//功能概要:	提示错误。
	//备注:		
	//********************************
	void
	Error(const string&);
	//********************************
	//名称:		FatalError
	//全名:		YSLib::YLog::FatalError
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const char *
	//功能概要:	提示致命错误。
	//备注:		中止程序。
	//********************************
	void
	FatalError(const char*);
	//********************************
	//名称:		FatalError
	//全名:		YSLib::YLog::FatalError
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const string &
	//功能概要:	提示致命错误。
	//备注:		中止程序。
	//********************************
	void
	FatalError(const string&);
};


//程序实例：通过单例实现进程唯一性语义。
class YApplication : public YObject,
	implements GIContainer<YShell>
{
public:
	typedef YObject ParentType;
	typedef set<HSHL> SHLs;

	//全局常量。
	static const IO::Path CommonAppDataPath; //程序默认路径。
	static const String CompanyName; //制作组织名称。
	static const String ProductName; //产品名称。
	static const String ProductVersion; //产品版本。

	//标准程序实例事件。
	DefEvent(EventHandler, ApplicationExit) //资源释放函数。
	DefEvent(EventHandler, Idle)

	//全局资源。
	YLog& Log; //默认程序日志。

private:
	YMessageQueue* pMessageQueue; //主消息队列：在程序实例中实现以保证单线程。
	YMessageQueue* pMessageQueueBackup; \
		//备份消息队列：在程序实例中实现以保证单线程。

public:
	YFontCache* pFontCache; //默认字体缓存。

private:
	SHLs sShls; // Shell 对象组：实现 Shell 存储。
	HSHL hShell;
		//当前 Shell 句柄：指示当前线程空间中运行的 Shell ；
		//应初始化为主 Shell ：全局单线程，生存期与进程相同。

	//********************************
	//名称:		YApplication
	//全名:		YSLib::YApplication::YApplication
	//可访问性:	private 
	//返回类型:	
	//修饰符:	
	//功能概要:	禁止默认构造。
	//备注:		无实现。
	//********************************
	YApplication();
	//********************************
	//名称:		GetInstancePtr
	//全名:		YSLib::YApplication::GetInstancePtr
	//可访问性:	private static 
	//返回类型:	YApplication*
	//修饰符:	
	//功能概要:	静态单例构造：取得自身实例指针。
	//备注:		
	//********************************
	static YApplication*
	GetInstancePtr();

public:
	//********************************
	//名称:		~YApplication
	//全名:		YSLib::YApplication::~YApplication
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构：释放 Shell 所有权和其它资源。
	//备注:		无异常抛出。
	//********************************
	virtual
	~YApplication() ythrow();

	//********************************
	//名称:		operator+=
	//全名:		YSLib::YApplication::operator+=
	//可访问性:	ImplI(GIContainer<YShell>) public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	YShell &
	//功能概要:	
	//备注:		
	//********************************
	ImplI(GIContainer<YShell>) void
	operator+=(YShell&); //添加 Shell 对象。
	//********************************
	//名称:		operator-=
	//全名:		YSLib::YApplication::operator-=
	//可访问性:	ImplI(GIContainer<YShell>) public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	YShell &
	//功能概要:	
	//备注:		
	//********************************
	ImplI(GIContainer<YShell>) bool
	operator-=(YShell&); //移除指定 Shell 对象。

	//********************************
	//名称:		Contains
	//全名:		YSLib::YApplication::Contains
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	HSHL
	//功能概要:	判断包含关系：指定句柄的 Shell 是否被实例所有。
	//备注:		
	//********************************
	bool
	Contains(HSHL) const;

	DefStaticGetter(YApplication&, App, *GetInstancePtr()) //取得自身实例引用。
	DefStaticGetter(HINSTANCE, InstanceHandle, HINSTANCE(GetInstancePtr())) \
		//取得自身实例句柄。
	DefGetter(const SHLs, ShellSet, sShls) //取 Shell 对象组。
	DefGetter(HSHL, ShellHandle, hShell) \
		//取得线程空间中当前运行的 Shell 的句柄。
	//********************************
	//名称:		GetDefaultMessageQueue
	//全名:		YSLib::YApplication::GetDefaultMessageQueue
	//可访问性:	public 
	//返回类型:	YMessageQueue&
	//修饰符:	ythrow(LoggedEvent)
	//功能概要:	取主消息队列。
	//备注:		
	//********************************
	YMessageQueue&
	GetDefaultMessageQueue() ythrow(LoggedEvent);
	//********************************
	//名称:		GetDefaultMessageQueue
	//全名:		YSLib::YApplication::GetBackupMessageQueue
	//可访问性:	public 
	//返回类型:	YMessageQueue&
	//修饰符:	ythrow(LoggedEvent)
	//功能概要:	取备份消息队列。
	//备注:		
	//********************************
	YMessageQueue&
	GetBackupMessageQueue() ythrow(LoggedEvent);

	//********************************
	//名称:		SetShellHandle
	//全名:		YSLib::YApplication::SetShellHandle
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	HSHL h
	//功能概要:	设置线程空间中当前运行的 Shell 的句柄。
	//备注:		
	//********************************
	bool
	SetShellHandle(HSHL h);

	//********************************
	//名称:		ResetShellHandle
	//全名:		YSLib::YApplication::ResetShellHandle
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	ythrow()
	//功能概要:	复位线程：设置当前运行的线程为主线程。
	//备注:		
	//********************************
	void
	ResetShellHandle() ythrow();

	//启动线程消息循环。
//	void
//	Run(HSHL);
};

inline YApplication*
YApplication::GetInstancePtr()
{
	static YApplication instance; //静态单例对象。

	return &instance;
}


//********************************
//名称:		InsertMessage
//全名:		YSLib::InsertMessage
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	const Message & msg
//功能概要:	全局默认消息插入函数。
//备注:		
//********************************
inline void
InsertMessage(const Message& msg)
{
	theApp.GetDefaultMessageQueue().InsertMessage(msg);

#if YSLIB_DEBUG_MSG & 1

	void YSDebug_MSG_Insert(Message&);
	YSDebug_MSG_Insert(msg);

#endif

}
//********************************
//名称:		InsertMessage
//全名:		YSLib::InsertMessage
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	const HSHL & hShl
//形式参数:	const Shells::MSGID & id
//形式参数:	const Shells::MSGPRIORITY & prior
//形式参数:	const WPARAM & w
//形式参数:	const LPARAM & l
//形式参数:	const Point & pt
//功能概要:	全局默认消息插入函数。
//备注:		
//********************************
inline void
InsertMessage(const HSHL& hShl, const Shells::MSGID& id,
	const Shells::MSGPRIORITY& prior, const WPARAM& w = 0, const LPARAM& l = 0,
	const Point& pt = Point::Zero)
{

#if YSLIB_DEBUG_MSG & 1

	void YSDebug_MSG_Insert(Message&);
	Message msg(hShl, id, prior, w, l);

	DefaultMQ.InsertMessage(msg);
	YSDebug_MSG_Insert(msg);

#else

	theApp.GetDefaultMessageQueue().InsertMessage(Message(
		hShl, id, prior, w, l, pt));

#endif

}

YSL_END

#endif

