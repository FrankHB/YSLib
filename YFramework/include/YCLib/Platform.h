﻿/*
	© 2009-2013 FrankHB.

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
\version r490
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 00:05:08 +0800
\par 修改时间:
	2013-11-27 21:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Platform
*/


#ifndef YCL_INC_Platform_h_
#define YCL_INC_Platform_h_ 1

/*!
\def YF_DLL
\brief 使用 YFramework 动态链接库。
\since build 361
*/

#if defined(YF_DLL) && defined(YF_BUILD_DLL)
#	error "DLL could not be built and used at the same time."
#endif

#ifdef YF_DLL
#	define YF_API __declspec(dllimport)
#elif defined(YF_BUILD_DLL)
#	define YF_API __declspec(dllexport)
#else
#	define YF_API
#endif

/*!
\def YF_Multithread
\brief 多线程环境。
\li 0 不支持多线程。
\li 1 支持 ISO C++11 多线程。
\since build 458
*/

/*!
\def YF_Hosted
\brief 宿主环境。
\li 0 非宿主支持。
\li 1 单一宿主支持。
\since build 458
*/

/*!
\ingroup Platforms
\brief 目标平台标识。
\since build 458
*/
//@{
/*!
\brief DS 平台。
*/
#define YF_Platform_DS 0x4001

//! \brief MinGW32 平台。
#define YF_Platform_Win32 0x4101

/*!
\brief MinGW32 平台。
*/
#define YF_Platform_MinGW32 0x4102

/*!
\def YF_Platform
\brief 目标平台。
*/
#ifdef __MINGW32__
//#ifdef _WIN32
#	define YF_Platform YF_Platform_MinGW32
#elif defined(_WIN32)
#	define YF_Platform YF_Platform_Win32
#elif !defined(YF_Platform)
//当前默认以 DS 作为目标平台。
#	define YF_Platform YF_Platform_DS
#endif
//@}


/*
\def YCL_Device_Cursor_FixedKey
\brief 1 表示保证指针输入设备的对应数据结构只保存一个按键状态。
\note 接收指针设备输入时不需要区分多个按键状态，可用于输入处理优化。
\since build 436
*/


#if YF_Platform == YF_Platform_DS
#	define YCL_DS 1
#	define YF_Hosted 0
#	define YF_Multithread 0
#	define YCL_API_FILESYSTEM_POSIX //!< 文件系统 API 支持。
#	define YCL_API_USE_UNISTD
#	define YCL_API_USE_SYS_DIR
#	define YCL_Device_Cursor_FixedKey 1
#elif YF_Platform == YF_Platform_MinGW32
#	define YCL_MinGW32 1
#	define YCL_Win32 1
#	define YF_Hosted 1
#elif YF_Platform == YF_Platform_Win32
#	define YCL_Win32 1
#	define YF_Hosted 1
#else
#	error "Unsupported platform found."
#endif

#if YCL_Win32
#	ifdef _MT
#		define YF_Multithread 1
#	else
#		define YF_Multithread 0
#	endif
#endif

#endif

