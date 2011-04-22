/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file config.h
\ingroup Adaptor
\brief 库编译配置。
\version 0.1564;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-09 09:25:26 +0800;
\par 修改时间:
	2011-04-20 11:02 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::Config;
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

#define YSL_USE_EXCEPTION_SPECIFICATION //!< 使用 YSLib 异常规范。

//#define YSL_MULTITHREAD //!< 多线程。

#define YSL_USE_COPY_ON_WRITE //!< 写时复制。

#ifdef YSL_MULTITHREAD
#	undef YSL_USE_COPY_ON_WRITE
#endif

//#define YSL_USE_YASLI_VECTOR //!< 使用 yasli::vector 。
//#define YSL_USE_FLEX_STRING //!< 使用 flex_string 。
#define YSL_OPT_SMALL_STRING_LENGTH 16 //!< 使用小字符串优化。

/*!
\brief YSLib No_Cursor
定义无指针设备。
消息成员列表中不会包含 Point pt 。
*/
//#define YSLIB_NO_CURSOR

// YSLib Debug 选项。

/*!
\brief YSLib MSG Debug
\li bit0 ：SendMessage 。
\li bit1 ：PeekMessage 。
\note PeekMessage 包含 SendMessage 。
*/
#define YSL_DEBUG_MSG 0

//! \brief 使用多播事件。
#define YSL_EVENT_MULTICAST

#endif

