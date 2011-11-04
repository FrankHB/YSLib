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
\version r2910;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 21:09:15 +0800;
\par 修改时间:
	2011-11-04 19:19 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShell;
*/


#ifndef YSL_INC_CORE_YSHELL_H_
#define YSL_INC_CORE_YSHELL_H_

#include "ymsgdef.h"
#include "yfunc.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

//! \brief 外壳程序：实现运行期控制流映像语义。
class Shell : public noncopyable
{
public:
	/*!
	\brief 无参数构造。
	*/
	DefDeCtor(Shell)
	/*!
	\brief 析构。
	*/
	virtual
	~Shell();

	/*!
	\brief 判断 Shell 是否处于激活状态。
	*/
	bool
	IsActive() const;

	/*!
	\brief 默认 Shell 处理函数。
	\note 调用默认 Shell 函数为应用程序没有处理的 Shell 消息提供默认处理，
		确保每一个消息得到处理。
	*/
	static int
	DefShlProc(const Message&);

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

	/*!
	\brief 消息处理函数：响应线程的直接调用。
	*/
	virtual PDefH(int, OnGotMessage, const Message& msg)
		ImplRet(DefShlProc(msg))
};

YSL_END_NAMESPACE(Shells)

YSL_END

#endif

