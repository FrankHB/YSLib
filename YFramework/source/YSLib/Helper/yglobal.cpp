/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yglobal.cpp
\ingroup Helper
\brief 平台相关的全局对象和函数定义。
\version r3920;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-22 15:28:52 +0800;
\par 修改时间:
	2012-04-01 08:45 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Helper::YGlobal;
*/


#include "YSLib/Helper/yglobal.h"
#include "YSLib/Helper/DSMain.h" // TODO: to be removed;
#include "YSLib/UI/ydesktop.h"
//#include <clocale>

YSL_BEGIN

//全局常量。
extern const char* DEF_DIRECTORY; //!< 默认目录。
extern const char* G_COMP_NAME; //!< 制作组织名称。
extern const char* G_APP_NAME; //!< 产品名称。
extern const char* G_APP_VER; //!< 产品版本。
const IO::Path Application::CommonAppDataPath(DEF_DIRECTORY);
const String Application::CompanyName(G_COMP_NAME);
const String Application::ProductName(G_APP_NAME);
const String Application::ProductVersion(G_APP_VER);

//全局变量。
#ifdef YSL_USE_MEMORY_DEBUG
//MemoryList DebugMemory(nullptr);
#endif

/*!
\ingroup PublicObject
\brief 全局变量映射。
*/
//@{
//@}


Application&
FetchAppInstance()
{
	return FetchGlobalInstance();
}


bool
InitConsole(Devices::Screen& scr, Drawing::PixelType fc, Drawing::PixelType bc)
{
	using namespace platform;

	if(&FetchGlobalInstance().GetScreenUp() == &scr)
		YConsoleInit(true, fc, bc);
	else if(&FetchGlobalInstance().GetScreenDown() == &scr)
		YConsoleInit(false, fc, bc);
	else
		return false;
	return true;
}

YSL_END

