/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NativeAPI.h
\ingroup DS
\brief 通用平台应用程序接口描述文件。
\version r1030;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-13 20:26:21 +0800;
\par 修改时间:
	2012-01-06 13:39 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::NativeAPI;
*/


#ifndef INCLUDED_API_H_
#define INCLUDED_API_H_

#include <nds.h>
#include <fat.h>
#include <unistd.h>
#include <sys/dir.h>

#define YCL_YCOMMON_H <ycommon.h>
#define YSL_YSBUILD_H <YSLib/Helper/yshelper.h>

//包含 PALib 。
//#include <PA9.h>

#ifdef USE_EFS
//包含 EFSLib 。
#include "efs_lib.h"
#endif

#endif

