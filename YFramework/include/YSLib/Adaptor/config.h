/*
	© 2010-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file config.h
\ingroup Adaptor
\brief YSLib 库配置。
\version r629
\author FrankHB <frankhb1989@gmail.com>
\since build 161
\par 创建时间:
	2010-10-09 09:25:26 +0800
\par 修改时间:
	2013-12-24 00:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::Configuration
*/


#ifndef YSL_INC_Adaptor_config_h_
#define YSL_INC_Adaptor_config_h_ 1

#include "YCLib/YModules.h"
#include YFM_YCLib_Platform

//#define NDEBUG //!< 非调试模式。

#ifndef NDEBUG
#	define YSL_USE_MEMORY_DEBUG //!< 使用 YSLib 内存调试设施。
#endif

// YSLib 选项。

//#define YSL_MULTITHREAD //!< 多线程。

/*!
\def YSL_USE_COPY_ON_WRITE
\brief 写时复制 。
\since build 161
*/
#undef YSL_USE_COPY_ON_WRITE
#if YF_Multithread != 0
#	define YSL_USE_COPY_ON_WRITE 0
#else
#	define YSL_USE_COPY_ON_WRITE 1
#endif

//#define YSL_USE_YASLI_VECTOR //!< 使用 yasli::vector 。
//#define YSL_USE_FLEX_STRING //!< 使用 flex_string 。
#define YSL_OPT_SMALL_STRING_LENGTH 16 //!< 使用小字符串优化。

// YSLib Debug 选项。

/*!
\brief YSLib MSG Debug
\note 当前版本忽略。
*/
#define YSL_DEBUG_MSG 0

#endif

