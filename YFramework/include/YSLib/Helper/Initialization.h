/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Initialization.h
\ingroup Adaptor
\brief 程序启动时的通用初始化。
\version r1559;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-10-21 23:15:08 +0800;
\par 修改时间:
	2012-03-25 15:56 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Helper::Initialization;
*/


#ifndef YSL_INC_HELPERR_INITIALIZATION_H_
#define YSL_INC_HELPERR_INITIALIZATION_H_

#include "../Core/ysdef.h"

YSL_BEGIN

//默认字型路径和目录。
extern const_path_t DEF_FONT_PATH;
extern const_path_t DEF_FONT_DIRECTORY;

/*!
\brief 初始化系统字体缓存：

以默认字型路径创建默认字体缓存并加载默认字型目录中的字体文件。
*/
void
InitializeSystemFontCache();

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

