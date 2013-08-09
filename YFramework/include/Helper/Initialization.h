/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Initialization.h
\ingroup Helper
\brief 程序启动时的通用初始化。
\version r712
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-10-21 23:15:08 +0800
\par 修改时间:
	2013-08-05 20:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Initialization
*/


#ifndef INC_Helper_Initialization_h_
#define INC_Helper_Initialization_h_ 1

#include "NPL/Configuration.h"
#include "YSLib/Service/yftext.h"
#include "YSLib/Adaptor/Font.h" // for Drawing::FontCache;

namespace YSLib
{

/*!
\brief 处理致命错误：显示错误信息并终止程序。
\since build 342
*/
YB_NORETURN YF_API void
HandleFatalError(const FatalError&) ynothrow;


/*!
\brief 从文本文件中读取配置。
\throw LoggedEvent 文件无效导致的读取失败。
\since build 344
*/
YF_API ValueNode
ReadConfiguration(TextFile&);

/*!
\brief 写入配置至文本文件。
\throw LoggedEvent 文件无效导致的写入失败。
\since build 344
*/
YF_API void
WriteConfiguration(TextFile&, const ValueNode&);

/*!
\brief 载入默认配置。
\param bInfo 是否在标准输出中显示信息。
\return 读取的配置。
\note 预设行为、配置文件和配置项参考 YSLib.txt 。
\since build 344
*/
YF_API ValueNode
LoadConfiguration(bool bInfo = {});

/*!
\brief 保存默认配置。
\return 是否保存成功。
\since build 344
*/
YF_API void
SaveConfiguration(const ValueNode&);


/*!
\brief 初始化环境。
\since build 342
*/
YF_API void
InitializeEnviornment();

/*!
\brief 初始化已安装项：检查安装完整性并载入主配置文件的配置项。
\return 被载入的配置项。
\warning 不保证涉及 CHRLib 初始化的线程安全性。
\since build 344
*/
YF_API ValueNode
InitializeInstalled();

/*!
\brief 初始化系统字体缓存。
\pre 默认字体缓存已初始化。
\since build 398

加载默认字体文件路径和默认字体目录中的字型至默认字体缓存。
*/
YF_API void
InitializeSystemFontCache(Drawing::FontCache&, const string&, const string&);

/*!
\brief 反初始化。
\warning 不保证线程安全性。
\since build 324

释放初始化的资源。
*/
YF_API void
Uninitialize() ynothrow;


/*!
\brief 取值类型根节点。
\since build 425
*/
YF_API ValueNode&
FetchRoot();

/*!
\brief 取默认字体缓存。
\since build 425
*/
YF_API Drawing::FontCache&
FetchDefaultFontCache();

} // namespace YSLib;

#endif

