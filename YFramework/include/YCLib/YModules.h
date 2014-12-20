/*
	© 2013-2014 FrankHB.

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

/*!	\defgroup YCLibLimitedPlatforms YCLib Limited Platforms Support
\ingroup YCLib
\brief YCLib 限定平台支持。
*/

/*!	\file YModules.h
\ingroup YCLib
\brief YCLib 库模块配置文件。
\version r81
\author FrankHB <frankhb1989@gmail.com>
\since build 462
\par 创建时间:
	2013-12-23 21:57:20 +0800
\par 修改时间:
	2014-12-19 12:46 +0800
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
#define YFM_YCLib_YCommon <YCLib/YCommon.h>
#define YFM_YCLib_NativeAPI <YCLib/NativeAPI.h>
#define YFM_YCLib_Debug <YCLib/Debug.h>
#define YFM_YCLib_FileSystem <YCLib/FileSystem.h>
#define YFM_YCLib_Timer <YCLib/Timer.h>
#define YFM_YCLib_Video <YCLib/Video.h>
#define YFM_YCLib_Keys <YCLib/Keys.h>
#define YFM_YCLib_Input <YCLib/Input.h>
#define YFM_YCLib_MemoryMapping <YCLib/MemoryMapping.h>
//! \since build 551
#define YFM_YCLib_Mutex <YCLib/Mutex.h>

//! \since build 492
#define YFM_YCLib_Host <YCLib/Host.h>
//! \since build 553
#define YFM_YCLib_POSIXThread <YCLib/POSIXThread.h>
//! \since build 560
#define YFM_YCLib_HostedGUI <YCLib/HostedGUI.h>
//! \since build 552
#define YFM_YCLib_JNI <YCLib/JNI.h>

#define YFM_MinGW32_YCLib_MinGW32 <YCLib/MinGW32.h>
#define YFM_MinGW32_YCLib_COM <YCLib/COM.h>
//@}
//! \since build 556
#define YFM_MinGW32_YCLib_NLS <YCLib/NLS.h>
//! \since build 520
#define YFM_MinGW32_YCLib_Consoles <YCLib/Consoles.h>
//! \since build 492
#define YFM_Android_YCLib_Android <YCLib/Android.h>

#endif

