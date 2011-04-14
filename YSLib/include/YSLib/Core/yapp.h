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
\brief 系统资源和应用程序实例抽象。
\version 0.2155;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-27 17:12:27 +0800;
\par 修改时间:
	2011-04-13 11:25 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YApplication;
*/


#ifndef INCLUDED_YAPP_H_
#define INCLUDED_YAPP_H_

#include "yobject.h"
#include "yfilesys.h"
#include "ystring.h"
#include "yevt.hpp"
#include "ysmsg.h"
#include "yexcept.h"

YSL_BEGIN

class Global; //!< 平台资源类前向声明。


//! \brief 程序日志类。
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


//! \brief 程序实例类：通过单例实现进程唯一性语义。
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
	YLog Log; //!< 默认程序日志。

private:
	Global* pResource; //!< 平台相关的全局资源。

public:
	static const GHandle<YShell> DefaultShellHandle; //!< 主 Shell 句柄。

private:
	YMessageQueue* pMessageQueue; //!< 主消息队列：在程序实例中实现以保证单线程。
	YMessageQueue* pMessageQueueBackup; \
		//!< 备份消息队列：在程序实例中实现以保证单线程。
	GHandle<YShell> hShell;
		/*!<
		当前 Shell 句柄：指示当前线程空间中运行的 Shell ；
		全局单线程，生存期与进程相同。
		*/
	YFontCache* pFontCache; //!< 默认字体缓存。

	/*!
	\brief 无参数构造。
	*/
	YApplication();
	/*!
	\brief 静态单例构造：取自身实例指针。
	*/
	static YApplication*
	GetInstancePtr() ythrow();

public:
	/*!
	\brief 析构：释放 Shell 所有权和其它资源。
	\note 无异常抛出。
	*/
	virtual
	~YApplication() ythrow();

	/*!
	\brief 取得自身实例引用。
	\note 断言检查：指针非空。
	*/
	static YApplication&
	GetInstance() ythrow();
	/*!
	\brief 取平台相关的全局资源实例。
	\note 断言检查：指针非空。
	\note 无异常抛出。
	*/
	Global&
	GetPlatformResource() ythrow();
	DefGetter(GHandle<YShell>, ShellHandle, hShell) \
		//!< 取得线程空间中当前运行的 Shell 的句柄。
	/*!
	\brief 取主消息队列。
	\throw LoggedEvent 记录异常事件。
	\note 仅抛出以上异常。
	*/
	YMessageQueue&
	GetDefaultMessageQueue() ythrow(LoggedEvent);
	/*!
	\brief 取备份消息队列。
	\throw LoggedEvent 记录异常事件。
	\note 仅抛出以上异常。
	*/
	YMessageQueue&
	GetBackupMessageQueue() ythrow(LoggedEvent);
	/*!
	\brief 取字体缓存引用。
	\throw LoggedEvent 记录异常事件。
	\note 仅抛出以上异常。
	*/
	YFontCache&
	GetFontCache() const ythrow(LoggedEvent); 

	/*!
	\brief 设置线程空间中当前运行的 Shell 的句柄。
	*/
	bool
	SetShellHandle(GHandle<YShell> h);

	/*!
	\brief 复位线程：设置当前运行的线程为主线程。
	*/
	void
	ResetShellHandle() ythrow();

	/*!
	\brief 复位默认字体缓存：使用指定路径。
	\throw LoggedEvent 记录异常事件。
	\note 仅抛出以上异常。
	*/
	void
	ResetFontCache(CPATH) ythrow(LoggedEvent);

	/*!
	\brief 注销字体缓存。
	*/
	void
	DestroyFontCache();

	//启动线程消息循环。
//	void
//	Run(GHandle<YShell>);
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
SendMessage(GHandle<YShell>, Messaging::ID, Messaging::Priority,
	Messaging::IContext* = NULL) ythrow();

YSL_END

#endif

