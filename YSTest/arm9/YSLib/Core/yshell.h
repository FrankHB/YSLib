/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yshell.h
\ingroup Core
\brief Shell 定义。
\version 0.2649;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 21:09:15 + 08:00;
\par 修改时间:
	2010-12-11 17:03 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShell;
*/


#ifndef INCLUDED_YSHELL_H_
#define INCLUDED_YSHELL_H_

#include "ysmsg.h"
#include "yfunc.hpp"
//#include <set>
//#include <list>

YSL_BEGIN

/*!
\brief 主 Shell 处理函数。
*/
extern LRES
MainShlProc(const Message&);

YSL_BEGIN_NAMESPACE(Shells)

//外壳程序：实现线程语义。
class YShell : public YObject,
	implements GIContainer<IWindow>
{
public:
	typedef list<HWND> WNDs;

private:
	WNDs sWnds; //!< 窗口组。

public:
	/*!
	\brief 无参数构造。
	\note 发送 SM_CREATE 消息。
	*/
	YShell();
	/*!
	\brief 析构。
	\note 无异常抛出。
		发送 SM_DESTROY 消息。
	*/
	virtual
	~YShell() ythrow();

	/*!
	\brief 判断 Shell 是否处于激活状态。
	*/
	bool
	IsActive() const;

	/*!
	\brief 激活 Shell 对象：ShlProc 控制权转移给此对象以维持单线程运行。
	*/
	bool
	Activate();

	/*!
	\brief 向窗口组添加窗口对象。
	*/
	ImplI(GIContainer<IWindow>) void
	operator+=(IWindow&);
	/*!
	\brief 从窗口组中移除指定窗口对象。
	*/
	ImplI(GIContainer<IWindow>) bool
	operator-=(IWindow&);
	/*!
	\brief 从窗口组中移除所有指定窗口对象，返回移除的对象数。
	*/
	WNDs::size_type
	RemoveAll(IWindow&);
	/*!
	\brief 移除窗口队列中首个窗口对象。
	*/
	void
	RemoveWindow();

	/*!
	\brief 取得窗口组中首个窗口对象的句柄。
	*/
	HWND
	GetFirstWindowHandle() const;
	/*!
	\brief 取得窗口组中顶端窗口对象的句柄。
	*/
	HWND
	GetTopWindowHandle() const;
	/*!
	\brief 取得窗口组中指定屏幕的指定的点所处的最顶层窗口对象的句柄。
	*/
	HWND
	GetTopWindowHandle(YDesktop&, const Point&) const;

	/*!
	\brief 向屏幕发送指定窗口对象。
	*/
	bool
	SendWindow(IWindow&);

	/*!
	\brief 向屏幕分发窗口对象。
	*/
	void
	DispatchWindows();

	/*!
	\brief 清除指定屏幕中属于窗口组的窗口对象。
	*/
	void
	ClearScreenWindows(YDesktop&);

	/*!
	\brief 默认 Shell 处理函数。
	*/
	static LRES
	DefShlProc(const Message&);

	// Shell 处理函数：响应线程的直接调用。
	virtual PDefH(LRES, ShlProc, const Message& m)
		ImplRet(DefShlProc(m))

	/*!
	\brief 处理线程的激活。
	*/
	virtual LRES
	OnActivated(const Message&);

	/*!
	\brief 处理线程的停用。
	*/
	virtual LRES
	OnDeactivated(const Message&);
};


class YShellMain : /*public GMCounter<YShellMain>*/ public YShell
{
public:
	typedef YShell ParentType;

	/*!
	\brief 无参数构造。
	\note 向应用程序对象添加自身。
	*/
	YShellMain();
	/*!
	\brief 析构。
	\note 无异常抛出。
			空实现。
	*/
	virtual
	~YShellMain() ythrow();

	/*!
	\brief Shell 处理函数。
	*/
	virtual LRES
	ShlProc(const Message&);
};

inline
YShellMain::~YShellMain() ythrow()
{}

inline LRES
Shells::YShellMain::ShlProc(const Message& msg)
{
	return MainShlProc(msg);
}


typedef LRES FSHLPROC(const Message&);
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
	LRES
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
PostQuitMessage(int nExitCode, Shells::MSGPRIORITY p = 0xF0);

/*!
\brief 默认 Shell 处理函数。
\note 调用默认 Shell 函数为应用程序没有处理的 Shell 消息提供默认处理，
		确保每一个消息得到处理。
*/
inline LRES
DefShellProc(const Message& msg)
{
	return YShell::DefShlProc(msg);
}

/*
\brief 从全局消息队列中取消息。
\param lpMsg 接收消息信息的 Message 结构指针。
\param hShl：消息关联（发送目标）的 Shell 的句柄，
	为 NULL 时无限制（为全局对象）。
\param wMsgFilterMin 指定被检查的消息范围里的第一个消息。
\param wMsgFilterMax 指定被检查的消息范围里的最后一个消息。
\param wRemoveMsg 确定消息如何被处理。此参数可取下列值之一：
	\arg PM_NOREMOVE PeekMessage处理后，消息不从消息队列中清除。
	\arg PM_REMOVE PeekMessage处理后，消息从消息队列中清除。
*/

#define PM_NOREMOVE 0x0
#define PM_REMOVE 0x1

/*!
\brief 从全局消息队列中取消息。
*/
IRES
PeekMessage(Message& msg, HSHL hShl = NULL, MSGID wMsgFilterMin = 0, MSGID wMsgFilterMax = 0, u32 wRemoveMsg = PM_NOREMOVE);

/*!
\brief 从全局消息队列中取消息。
\note 取得的消息从消息队列中清除。
*/
IRES
GetMessage(Message& msg, HSHL hShl = NULL, MSGID wMsgFilterMin = 0, MSGID wMsgFilterMax = 0);

/*!
\brief 翻译消息。
\note 空实现。
*/
ERRNO
TranslateMessage(const Message& msg);

/*!
\brief 分发消息。
*/
LRES
DispatchMessage(const Message& msg);

/*!
\brief 备份主消息队列中的消息。
*/
ERRNO
BackupMessage(const Message& msg);

/*!
\brief 从备份消息队列恢复所有消息。
*/
void
RecoverMessageQueue();

YSL_END_NAMESPACE(Shells)

YSL_END

#endif

