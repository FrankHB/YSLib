/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file config.h
\ingroup Adaptor
\brief 库编译配置。
\version r1585;
\author FrankHB<frankhb1989@gmail.com>
\since build 161 。
\par 创建时间:
	2010-10-09 09:25:26 +0800;
\par 修改时间:
	2012-04-01 08:23 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::Configuration;
*/


#ifndef YSL_INC_ADAPTOR_CONFIG_H_
#define YSL_INC_ADAPTOR_CONFIG_H_

//#define NDEBUG //!< 非调试模式。

#if defined(DEBUG) && defined(NDEBUG)
#error Macro DEBUG cannot be used with NDEBUG. \
	Exactly one of macro DEBUG and NDEBUG should be defined.
#endif
#if !defined(DEBUG) && !defined(NDEBUG)
#error No debug configuration macro found. \
	Exactly one of macro DEBUG and NDEBUG should be defined.
#endif

#ifndef NDEBUG
#	define YCL_USE_YASSERT //!< 使用 YCLib 断言。
#	define YSL_USE_MEMORY_DEBUG //!< 使用 YSLib 内存调试设施。
#endif

// YSLib 选项。

//#define YSL_MULTITHREAD //!< 多线程。

#define YSL_USE_COPY_ON_WRITE //!< 写时复制。

#ifdef YSL_MULTITHREAD
#	undef YSL_USE_COPY_ON_WRITE
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

