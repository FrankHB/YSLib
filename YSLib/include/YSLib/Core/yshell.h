/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yshell.h
\ingroup Core
\brief Shell 定义。
\version 0.2816;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 21:09:15 +0800;
\par 修改时间:
	2011-03-05 17:05 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShell;
*/


#ifndef INCLUDED_YSHELL_H_
#define INCLUDED_YSHELL_H_

#include "ysdef.h"
#include "yobject.h"
#include "ysmsgdef.h"
#include "yfunc.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

//! \brief 外壳程序：实现线程语义。
class YShell : public YObject
{
public:
	typedef YObject ParentType;

	/*!
	\brief 无参数构造。
	*/
	YShell();
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual
	~YShell() ythrow();

	/*!
	\brief 判断 Shell 是否处于激活状态。
	*/
	bool
	IsActive() const;

	/*!
	\brief 默认 Shell 处理函数。
	*/
	static int
	DefShlProc(const Message&);

	// Shell 处理函数：响应线程的直接调用。
	virtual PDefH1(int, ShlProc, const Message& m)
		ImplRet(DefShlProc(m))

	/*!
	\brief 处理线程的激活。
	*/
	virtual int
	OnActivated(const Message&);

	/*!
	\brief 处理线程的停用。
	*/
	virtual int
	OnDeactivated(const Message&);
};


//! \brief 主 Shell 。
class YMainShell : /*public GMCounter<YMainShell>*/ public YShell
{
public:
	typedef YShell ParentType;

	/*!
	\brief 无参数构造。
	\note 向应用程序对象添加自身。
	*/
	YMainShell();
	/*!
	\brief 析构。
	\note 无异常抛出。
			空实现。
	*/
	virtual DefEmptyDtor(YMainShell)

	/*!
	\brief Shell 处理函数。
	*/
	virtual int
	ShlProc(const Message&);
};


/*!
\ingroup HelperFunction
\brief 取当前应用程序活动 Shell 句柄。
*/
GHHandle<YShell>
GetCurrentShellHandle() ythrow();

/*!
\ingroup HelperFunction
\brief 激活 Shell 对象：ShlProc 控制权转移给此对象以维持单线程运行。
*/
bool
Activate(GHHandle<YShell>);


typedef int FSHLPROC(const Message&);
typedef FSHLPROC* PFSHLPROC;


// Shell 处理函数句柄。
struct HShellProc : public GHBase<PFSHLPROC>
{
	/*!
	\brief 构造：使用函数指针。
	*/
	HShellProc(const PFSHLPROC pf = NULL)
	: GHBase<PFSHLPROC>(pf)
	{}

	/*!
	\brief 调用函数。
	*/
	int
	operator()(const Message& msg) const
	{
		if(GetPtr())
			return GetPtr()(msg);
		return -1;
	}
};


/*!
\brief 以优先级 p 发起 Shell 终止请求，返回 nExitCode。
*/
void
PostQuitMessage(int nExitCode, Messaging::Priority p = 0xF0);

/*!
\brief 默认 Shell 处理函数。
\note 调用默认 Shell 函数为应用程序没有处理的 Shell 消息提供默认处理，
		确保每一个消息得到处理。
*/
inline int
DefShellProc(const Message& msg)
{
	return YShell::DefShlProc(msg);
}

/*
\brief 从全局消息队列中取消息。
\param lpMsg 接收消息信息的 Message 结构指针。
\param hShl：消息关联（发送目标）的 Shell 的句柄，
	为 NULL 时无限制（为全局消息）。
\param bRemoveMsg 确定取得的消息是否消息队列中清除。
*/
int
PeekMessage(Message& msg, GHHandle<YShell> hShl = GetCurrentShellHandle(),
	bool bRemoveMsg = false);

/*!
\brief 从全局消息队列中取消息。
\note 若消息队列为空则调用 Idle() 等待消息。取得的消息从消息队列中清除。
*/
int
GetMessage(Message& msg, GHHandle<YShell> hShl = GetCurrentShellHandle());

/*!
\brief 翻译消息。
\note 空实现。
*/
errno_t
TranslateMessage(const Message& msg);

/*!
\brief 分发消息。
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

YSL_END_NAMESPACE(Shells)

YSL_END

#endif

