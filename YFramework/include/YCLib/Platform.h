/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Platforms Platforms
\brief YSLib 库平台。
*/
/*!	\defgroup DS Nintendo/iQue Dual Screen[L/LL/i]
\ingroup Platforms
\brief DS 平台。
*/
/*!	\defgroup MinGW32 MinGW for Win32
\ingroup Platforms
\brief MinGW 实现的 Win32 平台。
\since build 296
*/

/*!	\file Platform.h
\ingroup YCLib
\brief 通用平台描述文件。
\version r419
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 00:05:08 +0800
\par 修改时间:
	2012-09-04 12:27 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Platform
*/


#ifndef YCL_INC_PLATFORM_H_
#define YCL_INC_PLATFORM_H_ 1

/*!
\def YCL_DLL
\brief 使用动态链接库。
\since build 306
*/

/*!
\def YCL_FUNCTION_NO_EQUALITY_GUARANTEE
\brief 相同函数指针不保证相等。
\since build 306
*/

/*!
\def YCL_MULTITHREAD
\brief 多线程环境。
\li 0 不支持多线程。
\li 1 支持 ISO C++11 多线程。
\since build 321
*/

/*!
\def YCL_HOSTED
\brief 宿主环境。
\li 0 非宿主支持。
\li 1 单一宿主支持。
\since build 322
*/

//#define YCL_DLL

#ifdef YCL_DLL
#	define YCL_FUNCTION_NO_EQUALITY_GUARANTEE 1
#endif

/*!
\ingroup Platforms
\brief 平台标识。
\since build 296
*/
//@{
/*!
\brief DS 平台。
*/
#define YCL_PLATFORM_DS 0x4001

/*!
\brief MinGW32 平台。
*/
#define YCL_PLATFORM_MINGW32 0x4002

/*!
\def YCL_PLATFORM
\brief 目标平台。
*/
#ifdef __MINGW32__
//#ifdef _WIN32
#	define YCL_PLATFORM YCL_PLATFORM_MINGW32
#elif !defined(YCL_PLATFORM)
//当前默认以 DS 作为目标平台。
#	define YCL_PLATFORM YCL_PLATFORM_DS
#endif
//@}


#if YCL_PLATFORM == YCL_PLATFORM_DS
#	define YCL_DS 1
#	define YCL_HOSTED 0
#	define YCL_MULTITHREAD 0
#	define YCL_API_FILESYSTEM_POSIX //!< 文件系统 API 支持。
#	define YCL_API_USE_UNISTD
#	define YCL_API_USE_SYS_DIR
#elif YCL_PLATFORM == YCL_PLATFORM_MINGW32
#	define YCL_MINGW32 1
#	define YCL_HOSTED 1
#	define YCL_MULTITHREAD 1
#else
#	error Unsupported platform found!
#endif

#endif

