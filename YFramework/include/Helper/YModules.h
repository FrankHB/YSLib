/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Helper Helper
\ingroup YFramework
\brief YFramework 助手模块。
*/

/*!	\file YModules.h
\ingroup Helper
\brief Helper 库模块配置文件。
\version r61
\author FrankHB <frankhb1989@gmail.com>
\since build 462
\par 创建时间:
	2013-12-23 21:57:11 +0800
\par 修改时间:
	2014-06-04 23:43 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::YModules
*/


#ifndef YF_INC_Helper_YModules_h_
#define YF_INC_Helper_YModules_h_ 1

#include "../YModules.h"
#include "../YSLib/UI/YModules.h"
#include "../NPL/YModules.h"

//! \since build 461
//@{
#define YFM_Helper_YGlobal <Helper/yglobal.h>
#define YFM_Helper_Initialization <Helper/Initialization.h>
#define YFM_Helper_InputManager <Helper/InputManager.h>
#define YFM_Helper_GUIShell <Helper/GUIShell.h>
#define YFM_Helper_GUIApplication <Helper/GUIApplication.h>
#define YFM_Helper_Host <Helper/Host.h>
#define YFM_Helper_HostedUI <Helper/HostedUI.h>
#define YFM_Helper_HostRenderer <Helper/HostRenderer.h>
#define YFM_Helper_ScreenBuffer <Helper/ScreenBuffer.h>
#define YFM_Helper_HostWindow <Helper/HostWindow.h>
#define YFM_Helper_ShellHelper <Helper/ShellHelper.h>

#define YFM_DS_Helper_DSMain <Helper/DSMain.h>
#define YFM_DS_Helper_Shell_DS <Helper/shlds.h>

//! \since build 502
#define YFM_Android_Helper_AndroidHost <Helper/AndroidHost.h>
//@}

#endif

