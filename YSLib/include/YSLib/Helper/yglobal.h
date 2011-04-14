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
\version 0.2065;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 15:14:57 +0800;
\par 修改时间:
	2011-04-13 11:22 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::YGlobal;
*/


#ifndef INCLUDED_YGLOBAL_H_
#define INCLUDED_YGLOBAL_H_

#include "../Core/ysdef.h"
#include "../Core/ysmsg.h"
#include "../Core/ycutil.h"
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
//@}

/*!
\brief 平台相关的全局资源类。
\note 含默认接口。
*/
class Global : public NonCopyable
{
	friend class YApplication;

public:
	//! \brief 屏幕大小。
	static const SDst MainScreenWidth, MainScreenHeight;

private:
	GHandle<YScreen> hScreenUp; //<! DS 上屏幕句柄。
	GHandle<YScreen> hScreenDown; //<! DS 上屏幕句柄。
	GHandle<YDesktop> hDesktopUp; //<! DS 下屏幕默认桌面句柄。
	GHandle<YDesktop> hDesktopDown; //<! DS 下屏幕默认桌面句柄。

	Global(); //!< 构造函数：非内联。
	~Global(); //!< 析构函数：非内联。

public:
	DefGetter(const GHandle<YScreen>&, ScreenUpHandle, hScreenUp)
	DefGetter(const GHandle<YScreen>&, ScreenDownHandle, hScreenDown)
	DefGetter(const GHandle<YDesktop>&, DesktopUpHandle, hDesktopUp)
	DefGetter(const GHandle<YDesktop>&, DesktopDownHandle, hDesktopDown)
	/*!
	\brief 取上屏幕。
	\note 断言检查：句柄非空。
	\note 无异常抛出。
	*/
	YScreen&
	GetScreenUp() const ythrow();
	/*!
	\brief 取下屏幕。
	\note 断言检查：句柄非空。
	\note 无异常抛出。
	*/
	YScreen&
	GetScreenDown() const ythrow();
	/*!
	\brief 取上桌面。
	\note 断言检查：句柄非空。
	\note 无异常抛出。
	*/
	YDesktop&
	GetDesktopUp() const ythrow();
	/*!
	\brief 取下桌面。
	\note 断言检查：句柄非空。
	\note 无异常抛出。
	*/
	YDesktop&
	GetDesktopDown() const ythrow();
	/*!
	\brief 取默认屏幕。
	\note 无异常抛出。
	*/
	PDefH0(YScreen&, GetDefaultScreen)
		ImplRet(GetScreenUp())
	/*!
	\brief 取默认桌面。
	\note 默认接口：可能会被 YSLib 本体使用。
	\note 无异常抛出。
	*/
	PDefH0(YDesktop&, GetDefaultDesktop)
		ImplRet(GetDesktopUp())
	/*!
	\brief 取触摸屏所在桌面。
	\note 默认接口：可能会被 YSLib 本体使用。
	\note 断言检查：句柄非空。
	\note 无异常抛出。
	*/
	PDefH0(YDesktop&, GetTouchableDesktop)
		ImplRet(GetDesktopDown())

	/*!
	\brief 初始化设备。
	\note 无异常抛出。
	*/
	void
	InitializeDevices() ythrow();

	/*!
	\brief 释放设备。
	\note 无异常抛出。
	*/
	void
	ReleaseDevices() ythrow();
};


YSL_BEGIN_NAMESPACE(Messaging)

//! \brief 输入消息上下文。
class InputContext : implements IContext
{
public:
	Runtime::KeysInfo Key;
	Point CursorLocation;

	explicit
	InputContext(Runtime::KeysInfo, const Point&);

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
ShlProc(GHandle<YShell>, const Message&);


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

