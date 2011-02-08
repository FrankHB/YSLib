/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Helper Helper
\ingroup YSLib
\brief YSLib 助手模块。
*/

/*!	\file yglobal.h
\ingroup Helper
\brief 平台相关的全局对象和函数定义。
\version 0.1978;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 15:14:57 + 08:00;
\par 修改时间:
	2011-01-31 12:46 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::YGlobal;
*/


#ifndef INCLUDED_YGLOBAL_H_
#define INCLUDED_YGLOBAL_H_

#include "../Core/ysmsg.h"
#include "../Core/yexcept.h"

YSL_BEGIN

/*!	\defgroup CustomGlobalConstants Custom Global Constants
\ingroup GlobalObjects
\brief 平台相关的全局常量。
*/
//@{
//@}

/*!	\defgroup CustomGlobalVariables Custom Global Variables
\ingroup GlobalObjects
\brief 平台相关的全局变量。
*/
//@{
extern GHHandle<YScreen> hScreenUp; //<! DS 上屏幕句柄。
extern GHHandle<YScreen> hScreenDown; //<! DS 上屏幕句柄。
extern GHHandle<YDesktop> hDesktopUp; //<! DS 下屏幕默认桌面句柄。
extern GHHandle<YDesktop> hDesktopDown; //<! DS 下屏幕默认桌面句柄。
//@}


YSL_BEGIN_NAMESPACE(Messaging)

//! \brief 输入消息上下文。
class InputContext : implements IContext
{
public:
	Runtime::KeysInfo Key;
	Point CursorLocation;

	explicit
	InputContext(Runtime::KeysInfo, const Point&);
	virtual DefEmptyDtor(InputContext)

	ImplI1(IContext) bool
	operator==(const IContext&) const;
};

inline
InputContext::InputContext(Runtime::KeysInfo k, const Point& pt)
	: Key(k), CursorLocation(pt)
{}

YSL_END_NAMESPACE(Messaging)

/*!
\brief 默认消息发生函数。
*/
void
Idle();

/*!
\brief 以指定前景色和背景色初始化指定屏幕的控制台。
*/
bool
InitConsole(YScreen&, Drawing::PixelType, Drawing::PixelType);

/*!
\brief 全局静态资源释放函数。
*/
void
Destroy_Static(YObject&, EventArgs&);

/*!
\brief 公共消息处理函数。
*/
int
ShlProc(GHHandle<YShell>, const Message&);


//全局函数。

void
ShowFatalError(const char*);

#ifdef YSL_USE_MEMORY_DEBUG

/*!
\brief 内存调试退出函数。
*/
void
OnExit_DebugMemory();

#endif

/*!
\brief 全局 Shell 消息处理函数。
*/
int
MainShlProc(const Message& msg);

/*!	\defgroup HelperFunction Helper Function
\brief 助手功能/函数。

仅帮助简化编码形式，并不包含编译期之后逻辑功能实现的代码设施。
*/

YSL_END

#endif

