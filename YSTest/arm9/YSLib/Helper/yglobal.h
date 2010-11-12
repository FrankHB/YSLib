/*
	Copyright (C) by Franksoft 2009 - 2010.

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
\version 0.1737;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 15:14:57 + 08:00;
\par 修改时间:
	2010-11-12 18:43 + 08:00;
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

//平台相关的全局常量。
#define MAX_FILENAME_LENGTH MAXPATHLEN
#define MAX_PATH_LENGTH MAX_FILENAME_LENGTH
#define DEF_SHELL_FONTSIZE 16
#define DEF_SHELL_FONTSIZE_MIN 4
#define DEF_SHELL_FONTSIZE_MAX 72

const char DEF_PATH_DELIMITER = '/'; //!< 文件路径分隔符。
const char* const DEF_PATH_SEPERATOR = "/"; //!< 文件路径分隔字符串。
#define DEF_PATH_ROOT DEF_PATH_SEPERATOR

//平台相关的全局变量。
extern YScreen *pScreenUp, *pScreenDown;
extern YDesktop *pDesktopUp, *pDesktopDown;

//类型定义。
typedef char PATHSTR[MAX_PATH_LENGTH];
//typedef char PATHSTR[MAXPATHLEN];
typedef char FILENAMESTR[MAX_FILENAME_LENGTH];

struct Def
{
	/*!
	\brief 默认消息发生函数。
	*/
	static void
	Idle();

	/*!
	\brief 初始化视频输出。
	*/
	static bool
	InitVideo();

	/*!
	\brief 默认屏幕初始化函数。
	*/
	static void
	InitScreenAll();

	/*!
	\brief 默认上屏初始化函数。
	*/
	static void
	InitScrUp();

	/*!
	\brief 默认下屏初始化函数。
	*/
	static void
	InitScrDown();

	/*!
	\brief 以指定前景色和背景色初始化指定屏幕的控制台。
	*/
	static bool
	InitConsole(YScreen&, Drawing::PixelType, Drawing::PixelType);

	/*!
	\brief 默认输入等待函数。
	*/
	static void
	WaitForInput();

	/*!
	\brief 全局资源释放函数。
	*/
	static void
	Destroy(YObject&, EventArgs&);

	/*!
	\brief 公共消息处理函数。
	*/
	static LRES
	ShlProc(HSHL, const Message&);
};

//全局函数。

YSL_BEGIN_SHELL(YSL_SHLMAIN_NAME)

LRES
YSL_SHLMAIN_SHLPROC(const Message&);

YSL_END_SHELL(YSL_SHLMAIN_NAME)


/*!
\brief 全局默认 Shell 消息处理函数。
*/
inline LRES
DefaultMainShlProc(const Message& msg)
{
	return YSL_MAIN_SHLPROC(msg);
}

YSL_END

#endif

