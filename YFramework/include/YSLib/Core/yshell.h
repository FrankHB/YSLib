/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yshell.h
\ingroup Core
\brief Shell 抽象。
\version r1934
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-13 21:09:15 +0800
\par 修改时间:
	2012-12-11 21:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YShell
*/


#ifndef YSL_INC_CORE_YSHELL_H_
#define YSL_INC_CORE_YSHELL_H_ 1

#include "ymsgdef.h"
#include "yfunc.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

//! \brief 外壳程序：实现运行期控制流映像语义。
class YF_API Shell : private noncopyable, public enable_shared_from_this<Shell>
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
	\brief 处理消息：响应线程的直接调用。
	\since build 317
	*/
	virtual PDefH(void, OnGotMessage, const Message& msg)
		ImplExpr(DefShlProc(msg))
};

YSL_END_NAMESPACE(Shells)

YSL_END

#endif

