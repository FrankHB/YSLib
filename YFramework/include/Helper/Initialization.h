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
\version r678
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-10-21 23:15:08 +0800
\par 修改时间:
	2012-09-27 14:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Initialization
*/


#ifndef INC_HELPER_INITIALZATION_H_
#define INC_HELPER_INITIALZATION_H_ 1

#include "NPL/Configuration.h"
#include "YSLib/Service/yftext.h"

YSL_BEGIN

/*!
\brief 处理致命错误：显示错误信息并终止程序。
\since build 342
\todo 使用 [[noreturn]] 。
*/
void
HandleFatalError(const FatalError&) ynothrow;


/*!
\brief 从文本文件中读取配置。
\throw LoggedEvent 文件无效导致的读取失败。
\since build 344
*/
ValueNode
ReadConfiguration(TextFile&);

/*!
\brief 写入配置至文本文件。
\throw LoggedEvent 文件无效导致的写入失败。
\since build 344
*/
void
WriteConfiguration(TextFile&, const ValueNode&);

/*!
\brief 载入默认配置。
\param bInfo 是否在标准输出中显示信息。
\return 读取的配置。
\note 预设行为、配置文件和配置项参考 YSLib.txt 。
\since build 344
*/
ValueNode
LoadConfiguration(bool bInfo = false);

/*!
\brief 保存默认配置。
\return 是否保存成功。
\since build 344
*/
void
SaveConfiguration(const ValueNode&);


/*!
\brief 初始化环境。
\since build 342
*/
void
InitializeEnviornment();

/*!
\brief 初始化已安装项：检查安装完整性并载入主配置文件的配置项。
\return 被载入的配置项。
\warning 不保证涉及 CHRLib 初始化的线程安全性。
\since build 344
*/
ValueNode
InitializeInstalled();

/*!
\brief 初始化系统字体缓存。
\pre 默认字体缓存已初始化。
\since build 342

加载默认字体文件路径和默认字体目录中的字型至默认字体缓存。
*/
void
InitializeSystemFontCache(const string&, const string&);

/*!
\brief 反初始化。
\warning 不保证线程安全性。
\since build 324

释放初始化的资源。
*/
void
Uninitialize() ynothrow;

YSL_END

#endif

