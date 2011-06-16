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
\version 0.2308;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-27 17:12:27 +0800;
\par 修改时间:
	2011-06-16 03:30 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YApplication;
*/


#ifndef YSL_INC_CORE_YAPP_H_
#define YSL_INC_CORE_YAPP_H_

#include "yfilesys.h"
#include "yshell.h"
#include "yevt.hpp"

YSL_BEGIN

using Drawing::FontCache;

using Messaging::MessageQueue;

//! \brief 程序日志类。
class YLog : public YObject
{
public:
	/*!
	\brief 无参数构造：默认实现。
	*/
	YLog() = default;
	/*!
	\brief 析构：空实现。
	*/
	virtual
	~YLog()
	{}

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


//! \brief 程序实例类。
class YApplication : public YObject
{
public:
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
	MessageQueue* pMessageQueue; //!< 主消息队列：在程序实例中实现以保证单线程。
	MessageQueue* pMessageQueueBackup; \
		//!< 备份消息队列：在程序实例中实现以保证单线程。
	shared_ptr<YShell> hShell;
		/*!<
		当前 Shell 句柄：指示当前线程空间中运行的 Shell ；
		全局单线程，生存期与进程相同。
		*/
	FontCache* pFontCache; //!< 默认字体缓存。

public:
	/*!
	\brief 无参数构造。
	*/
	YApplication();

	/*!
	\brief 析构：释放 Shell 所有权和其它资源。
	\note 无异常抛出。
	*/
	virtual
	~YApplication() ynothrow;

	/*!
	\brief 取得线程空间中当前运行的 Shell 的句柄。
	*/
	DefGetter(shared_ptr<YShell>, ShellHandle, hShell) \
	/*!
	\brief 取主消息队列。
	\throw LoggedEvent 记录异常事件。
	\note 仅抛出以上异常。
	*/
	MessageQueue&
	GetDefaultMessageQueue() ythrow(LoggedEvent);
	/*!
	\brief 取备份消息队列。
	\throw LoggedEvent 记录异常事件。
	\note 仅抛出以上异常。
	*/
	MessageQueue&
	GetBackupMessageQueue() ythrow(LoggedEvent);
	/*!
	\brief 取字体缓存引用。
	\throw LoggedEvent 记录异常事件。
	\note 仅抛出以上异常。
	*/
	FontCache&
	GetFontCache() const ythrow(LoggedEvent);

	/*!
	\brief 设置线程空间中当前运行的 Shell 的句柄。
	\return 参数是否有效。
	\warning 空句柄在此处是可接受的，但继续运行可能会导致断言失败。
	*/
	bool
	SetShellHandle(const shared_ptr<YShell>&);

	/*!
	\brief 复位默认字体缓存：使用指定路径。
	\throw LoggedEvent 记录异常事件。
	\note 仅抛出以上异常。
	*/
	void
	ResetFontCache(const_path_t) ythrow(LoggedEvent);

	/*!
	\brief 注销字体缓存。
	*/
	void
	DestroyFontCache();

	//启动线程消息循环。
//	void
//	Run(shared_ptr<YShell>);
};


/*!
\brief 取应用程序实例。
\note 保证在平台相关的全局资源初始化之后初始化此实例。
*/
extern YApplication&
FetchAppInstance();

/*!
\ingroup HelperFunction
\brief 取当前应用程序线程空间中活动的 Shell 句柄。
*/
inline shared_ptr<YShell>
FetchShellHandle() ynothrow
{
	return FetchAppInstance().GetShellHandle();
}

/*!
\ingroup HelperFunction
\brief 激活 Shell 对象：ShlProc 控制权转移给此对象以维持单线程运行。
*/
inline bool
Activate(const shared_ptr<YShell>& h)
{
	return FetchAppInstance().SetShellHandle(h);
}


/*
\brief 从全局消息队列中取消息。
\note 参数含义见 %MessageQueue::PeekMessage 。
*/
int
PeekMessage(Message& msg,
	const shared_ptr<YShell>& hShl = FetchShellHandle(),
	bool bRemoveMsg = false);

/*!
\brief 从全局消息队列中取消息。
\note 若消息队列中消息数小于或等于给定阈值则调用 Idle() 等待消息。
	取得的消息从消息队列中清除。
*/
int
FetchMessage(Message& msg, MessageQueue::SizeType = 0,
	const shared_ptr<YShell>& hShl = FetchShellHandle());

/*!
\brief 翻译消息：空实现。
*/
errno_t
TranslateMessage(const Message& msg);

/*!
\brief 分发消息。
\note 断言检查：当前 Shell 句柄有效。
*/
int
DispatchMessage(const Message& msg);

/*!
\brief 备份主消息队列中的消息。
*/
errno_t
BackupMessageQueue(const Message& msg);

/*!
\brief 从备份消息队列恢复所有消息。
*/
void
RecoverMessageQueue();


/*!
\brief 全局默认队列消息发送函数。
*/
//@{
void
SendMessage(const Message&) ynothrow;
void
SendMessage(const shared_ptr<YShell>&, Messaging::ID, Messaging::Priority,
	const ValueObject& = ValueObject()) ynothrow;
template<Messaging::MessageID _vID>
inline void
SendMessage(const shared_ptr<YShell>& hShl, Messaging::Priority prior,
	const typename Messaging::SMessageMap<_vID>::TargetType& target) ynothrow
{
	SendMessage(hShl, _vID, prior, ValueObject(target));
}
//@}

/*!
\brief 以优先级 p 发起 Shell 终止请求，返回 nExitCode。
*/
void
PostQuitMessage(int nExitCode, Messaging::Priority p = 0xF0);

YSL_END

#endif

