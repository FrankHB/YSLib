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
\brief Shell 抽象。
\version 0.2879;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 21:09:15 +0800;
\par 修改时间:
	2011-05-23 20:19 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShell;
*/


#ifndef YSL_INC_CORE_YSHELL_H_
#define YSL_INC_CORE_YSHELL_H_

#include "ysmsgdef.h"
#include "yfunc.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

//! \brief 外壳程序：实现线程语义。
class YShell : public YObject
{
public:
	/*!
	\brief 无参数构造。
	*/
	YShell();
	/*!
	\brief 析构。
	*/
	virtual
	~YShell();

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
	virtual PDefH1(int, ShlProc, const Message& msg)
		ImplRet(DefShlProc(msg))

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
class YMainShell : public YShell
{
public:
	/*!
	\brief 无参数构造。
	\note 向应用程序对象添加自身。
	*/
	YMainShell();

	/*!
	\brief Shell 处理函数。
	*/
	virtual int
	ShlProc(const Message&);
};


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

YSL_END_NAMESPACE(Shells)

YSL_END

#endif

