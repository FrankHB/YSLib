/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Initialization.h
\ingroup Helper
\brief 程序启动时的通用初始化。
\version r1597;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-10-21 23:15:08 +0800;
\par 修改时间:
	2012-06-11 08:22 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Helper::Initialization;
*/


#ifndef YSL_INC_HELPERR_INITIALIZATION_H_
#define YSL_INC_HELPERR_INITIALIZATION_H_

#include <YSLib/Core/ysdef.h>

YSL_BEGIN

/*!
\brief 初始化环境。
\note 无异常抛出。
\since build 303 。
*/
void
InitializeEnviornment() ynothrow;

/*!
\brief 初始化系统字体缓存。
\note 无异常抛出。
\since build 303 。

以默认字型路径创建默认字体缓存并加载默认字型目录中的字体文件。
*/
void
InitializeSystemFontCache() ynothrow;

/*!
\brief 检查安装完整性。
\note 无异常抛出。
\since build 303 。
*/
void
CheckInstall() ynothrow;

YSL_END

#endif

