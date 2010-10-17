// YSLib::Core::YApplication by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-27 17:12:27 + 08:00;
// UTime = 2010-10-17 00:47 + 08:00;
// Version = 0.1851;


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
	YLog();
	virtual
	~YLog();

	YLog&
	operator<<(char);
	YLog&
	operator<<(const char*);
	YLog&
	operator<<(const string&);

	void
	Error(const char*);
	void
	Error(const string&);
	void
	FatalError(const char*);
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
	YMessageQueue* pMessageQueueBackup; //备份消息队列：在程序实例中实现以保证单线程。

public:
	YFontCache* pFontCache; //默认字体缓存。

private:
	SHLs sShls; // Shell 对象组：实现 Shell 存储。
	HSHL hShell; //当前 Shell 句柄：指示当前线程空间中运行的 Shell ；应初始化为主 Shell ：全局单线程，生存期与进程相同。

	YApplication();
	static YApplication*
	GetInstancePtr(); //单例构造函数：取得自身实例指针。

public:
	virtual
	~YApplication() ythrow();

	ImplI(GIContainer<YShell>) void
	operator+=(YShell&); //添加 Shell 对象。
	ImplI(GIContainer<YShell>) bool
	operator-=(YShell&); //移除指定 Shell 对象。

	bool
	Contains(HSHL) const; //判断给定句柄的 Shell 是否被实例所有。

	DefStaticGetter(YApplication&, App, *GetInstancePtr()) //取得自身实例引用。
	DefStaticGetter(HINSTANCE, InstanceHandle, HINSTANCE(GetInstancePtr())) //取得自身实例句柄。
	DefGetter(const SHLs, ShellSet, sShls) //取 Shell 对象组。
	DefGetter(HSHL, ShellHandle, hShell) //取得线程空间中当前运行的 Shell 的句柄。
	YMessageQueue&
	GetDefaultMessageQueue() ythrow(LoggedEvent);
	YMessageQueue&
	GetBackupMessageQueue() ythrow(LoggedEvent);

	bool
	SetShellHandle(HSHL h); //设置线程空间中当前运行的 Shell 的句柄。

	void
	ResetShellHandle() ythrow(); //复位线程：设置当前运行的线程为主线程。
//	void
//	Run(HSHL); //启动线程消息循环。
};

inline YApplication*
YApplication::GetInstancePtr()
{
	static YApplication instance; //静态单例对象。

	return &instance;
}


//全局默认消息插入函数。
inline void
InsertMessage(const Message& msg)
{
	theApp.GetDefaultMessageQueue().InsertMessage(msg);

#if YSLIB_DEBUG_MSG & 1

	void YSDebug_MSG_Insert(Message&);
	YSDebug_MSG_Insert(msg);

#endif

}
inline void
InsertMessage(const HSHL& hShl, const Shells::MSGID& id, const Shells::MSGPRIORITY& prior, const WPARAM& w = 0, const LPARAM& l = 0, const Point& pt = Point::Zero)
{

#if YSLIB_DEBUG_MSG & 1

	void YSDebug_MSG_Insert(Message&);
	Message msg(hShl, id, prior, w, l);

	DefaultMQ.InsertMessage(msg);
	YSDebug_MSG_Insert(msg);

#else

	theApp.GetDefaultMessageQueue().InsertMessage(Message(hShl, id, prior, w, l, pt));

#endif

}

YSL_END

#endif

