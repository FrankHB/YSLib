/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup YCLib YCommonLib
\ingroup YFramework
\brief YFramework 公共基础库。
*/

/*!	\file YModules.h
\ingroup YCLib
\brief YCLib 库模块配置文件。
\version r56
\author FrankHB <frankhb1989@gmail.com>
\since build 462
\par 创建时间:
	2013-12-23 21:57:20 +0800
\par 修改时间:
	2013-12-24 10:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::YModules
*/


#ifndef YF_INC_YCLib_YModules_h_
#define YF_INC_YCLib_YModules_h_ 1

#include "../YModules.h"

//! \since build 461
//@{
#define YFM_YCLib_Platform <YCLib/Platform.h>
#define YFM_YCLib_YCommon <YCLib/ycommon.h>
#define YFM_YCLib_NativeAPI <YCLib/NativeAPI.h>
#define YFM_YCLib_Debug <YCLib/Debug.h>
#define YFM_YCLib_FileSystem <YCLib/FileSystem.h>
#define YFM_YCLib_Timer <YCLib/Timer.h>
#define YFM_YCLib_Video <YCLib/Video.h>
#define YFM_YCLib_Keys <YCLib/Keys.h>
#define YFM_YCLib_Input <YCLib/Input.h>
#define YFM_YCLib_MemoryMapping <YCLib/MemoryMapping.h>

#define YFM_MinGW32_YCLib_MinGW32 <YCLib/MinGW32.h>
#define YFM_MinGW32_YCLib_COM <YCLib/COM.h>
#define YFM_MinGW32_YCLib_Win32GUI <YCLib/Win32GUI.h>
//@}

#endif

