/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yapp.h
\ingroup Core
\brief 应用程序实例类抽象。
\version 0.2062;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-27 17:12:27 + 08:00;
\par 修改时间:
	2011-01-21 22:13 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YApplication;
*/


#ifndef INCLUDED_YAPP_H_
#define INCLUDED_YAPP_H_

#include "ydevice.h"
#include "yshell.h"
#include "yfilesys.h"
#include "yevt.hpp"

YSL_BEGIN

//! \brief 程序日志。
class YLog : public YObject
{
public:
	/*!
	\brief 构造。
	*/
	YLog();
	/*!
	\brief 析构。
	*/
	virtual
	~YLog();

	/*!
	\brief 输出 char 字符。
	*/
	YLog&
	operator<<(char);
	/*!
	\brief 输出字符指针表示的字符串。
	*/
	YLog&
	operator<<(const char*);
	/*!
	\brief 输出字符串。
	*/
	YLog&
	operator<<(const string&);

	/*!
	\brief 提示错误。
	*/
	void
	Error(const char*);
	/*!
	\brief 提示错误。
	*/
	void
	Error(const string&);
	/*!
	\brief 提示致命错误。
	\note 中止程序。
	*/
	void
	FatalError(const char*);
	/*!
	\brief 提示致命错误。
	\note 中止程序。
	*/
	void
	FatalError(const string&);
};


//! \brief 程序实例：通过单例实现进程唯一性语义。
class YApplication : public YObject
{
public:
	typedef YObject ParentType;

	//全局常量。
	static const IO::Path CommonAppDataPath; //!< 程序默认路径。
	static const String CompanyName; //!< 制作组织名称。
	static const String ProductName; //!< 产品名称。
	static const String ProductVersion; //!< 产品版本。

	//标准程序实例事件。
	DeclEvent(HEvent, ApplicationExit) //!< 资源释放函数。
	DeclEvent(HEvent, Idle)

	//全局资源。
	YLog& Log; //!< 默认程序日志。
	GHHandle<YScreen>& hDefaultScreen;
	GHHandle<YDesktop>& hDefaultDesktop;
	static const GHHandle<YShell> DefaultShellHandle; //!< 主 Shell 句柄。

private:
	YMessageQueue* pMessageQueue; //!< 主消息队列：在程序实例中实现以保证单线程。
	YMessageQueue* pMessageQueueBackup; \
		//!< 备份消息队列：在程序实例中实现以保证单线程。

	GHHandle<YShell> hShell;
		/*!<
		当前 Shell 句柄：指示当前线程空间中运行的 Shell ；
		全局单线程，生存期与进程相同。
		*/

public:
	YFontCache* pFontCache; //!< 默认字体缓存。

private:
	/*!
	\brief 构造：使用指定默认屏幕句柄、默认桌面句柄和默认 Shell 句柄。
	*/
	YApplication(GHHandle<YScreen>&, GHHandle<YDesktop>&);
	/*!
	\brief 静态单例构造：取自身实例指针。
	*/
	static YApplication*
	GetInstancePtr(GHHandle<YScreen>&, GHHandle<YDesktop>&);

public:
	/*!
	\exception ythrow()
	\brief 析构：释放 Shell 所有权和其它资源。
	\note 无异常抛出。
	*/
	virtual
	~YApplication() ythrow();

	static PDefH(YApplication&, GetApp, GHHandle<YScreen>& hScr,
		GHHandle<YDesktop>& hDsk)
		ImplRet(*GetInstancePtr(hScr, hDsk)) //!< 取得自身实例引用。
	DefGetter(GHHandle<YShell>, ShellHandle, hShell) \
		//!< 取得线程空间中当前运行的 Shell 的句柄。
	/*!
	\brief 取主消息队列。
	\exception ythrow(LoggedEvent)
	*/
	YMessageQueue&
	GetDefaultMessageQueue() ythrow(LoggedEvent);
	/*!
	\brief 取备份消息队列。
	\exception ythrow(LoggedEvent)
	*/
	YMessageQueue&
	GetBackupMessageQueue() ythrow(LoggedEvent);

	/*!
	\brief 设置线程空间中当前运行的 Shell 的句柄。
	*/
	bool
	SetShellHandle(GHHandle<YShell> h);

	/*!
	\brief 复位线程：设置当前运行的线程为主线程。
	*/
	void
	ResetShellHandle() ythrow();

	//启动线程消息循环。
//	void
//	Run(GHHandle<YShell>);
};


/*!
\brief 全局默认队列消息发送函数。
*/
void
SendMessage(const Message&) ythrow();
/*!
\brief 全局默认队列消息发送函数。
*/
void
SendMessage(GHHandle<YShell>, Messaging::ID, Messaging::Priority,
	Messaging::IContext* = NULL) ythrow();

YSL_END

#endif

