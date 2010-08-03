// YSLib::Core::YApplication by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-27 17:12:27;
// UTime = 2010-8-2 14:57;
// Version = 0.1696;


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
	operator<<(const std::string&);

	void
	Error(const char*);
	void
	Error(const std::string&);
	void
	FatalError(const char*);
	void
	FatalError(const std::string&);
};


//程序实例：通过单例实现进程唯一性语义。
class YApplication : public YObject,
	implements GIContainer<YShell>
{
public:
	typedef YObject ParentType;
	typedef std::set<HSHL> SHLs;

	//全局常量。
	static const IO::YPath CommonAppDataPath; //程序默认路径。
	static const MString CompanyName; //制作组织名称。
	static const MString ProductName; //产品名称。
	static const MString ProductVersion; //产品版本。

	//标准程序实例事件。
	DefEvent(YEventHandler, ApplicationExit) //资源释放函数。
	DefEvent(YEventHandler, Idle)

	//全局资源。
	YLog& Log; //默认程序日志。
	YMessageQueue DefaultMQ; //主消息队列：在程序实例中实现以保证单线程。
	YMessageQueue DefaultMQ_Backup; //备份消息队列：在程序实例中实现以保证单线程。
	YFontCache* FontCache; //默认字体缓存。

private:
	SHLs sShls; // Shell 对象组：实现 Shell 存储。
	HSHL hShell; //当前 Shell 句柄：指示当前线程空间中运行的 Shell ；应初始化为主 Shell ：全局单线程，生存期与进程相同。

	YApplication();
	static YApplication*
	GetInstancePtr(); //单例构造函数：取得自身实例指针。

public:
	virtual
	~YApplication();

	void
	operator+=(YShell&); //添加 Shell 对象。
	bool
	operator-=(YShell&); //移除指定 Shell 对象。

	bool
	Contains(HSHL) const; //判断给定句柄的 Shell 是否被实例所有。

	DefStaticGetter(YApplication&, App, *GetInstancePtr()) //取得自身实例引用。
	DefStaticGetter(HINSTANCE, InstanceHandle, HINSTANCE(GetInstancePtr())) //取得自身实例句柄。
	DefGetter(const SHLs, ShellSet, sShls) //取 Shell 对象组。
	DefGetter(HSHL, ShellHandle, hShell) //取得线程空间中当前运行的 Shell 的句柄。

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

YSL_END

#endif

