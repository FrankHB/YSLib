/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ysinit.h
\ingroup Service
\brief 程序启动时的通用初始化。
\version 0.1531;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-10-21 23:15:08 + 08:00;
\par 修改时间:
	2010-11-12 19:08 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::YShellInitialization;
*/


#ifndef INCLUDED_YSINIT_H_
#define INCLUDED_YSINIT_H_

#include "../Core/ysdef.h"

YSL_BEGIN

/*!
\brief 初始化主控制台。
*/
void
InitYSConsole();


//默认字型路径和目录。
extern CPATH DEF_FONT_PATH;
extern CPATH DEF_FONT_DIRECTORY;

/*!
\brief 初始化系统字体缓存：
//			以默认字型路径创建默认字体缓存并加载默认字型目录中的字体文件。
*/
void
InitializeSystemFontCache();

/*!
\brief 注销系统字体缓存。
*/
void
DestroySystemFontCache();

/*!
\brief 初始化失败公用程序。
*/
void
EpicFail();

/*!
\brief LibFAT 失败。
*/
void
LibfatFail();

/*!
\brief 检查安装完整性。
*/
void
CheckInstall();

/*!
\brief 检查系统字体缓存。
*/
void
CheckSystemFontCache();

YSL_END

#endif

