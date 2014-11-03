/*
	© 2009-2014 FrankHB.

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
\brief x86 MinGW 实现的 Win32 平台。
\since build 296
*/
/*!	\defgroup MinGW64 MinGW for Win32
\ingroup Platforms
\brief x86_64 MinGW 实现的 Win32 平台。
\since build 520
*/
/*!	\defgroup Android Android
\ingroup Platforms
\brief Android 平台。
\since build 492
*/

/*!	\file Platform.h
\ingroup YCLib
\brief 通用平台描述文件。
\version r562
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 00:05:08 +0800
\par 修改时间:
	2014-11-03 10:42 +0800
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
\note 只保证不同的目标平台值为不同的非零整数。具体值可能随版本不同变动。
\since build 458
*/
//@{
/*!
\brief DS 平台。
*/
#define YF_Platform_DS 0x4001

/*!
\brief Win32 平台。
\note 注意 _WIN32 被预定义，但没有 _WIN64 。
*/
#define YF_Platform_Win32 0x4101

/*!
\brief MinGW32 平台。
\note 注意 MinGW-w64 和 MinGW.org 同时预定义了 __MINGW32__ ，但没有 __MINGW64__ 。
*/
#define YF_Platform_MinGW32 0x4102

/*!
\brief Win32 x86_64 平台。
\note 通称 Win64 平台。注意 Win32 指 Windows32 子系统，并非体系结构名称。
\note 注意 _WIN32 和 _WIN64 被同时预定义。
\since build 520
*/
#define YF_Platform_Win64 0x4111

/*!
\brief MinGW-w64 x86_64 平台。
\note 和 Win64 基本兼容的 MinGW32 平台。注意 MinGW32 指系统名称，并非体系结构名称。
\note 注意 MinGW-w64 在 x86_64 上同时预定义了 __MINGW32__ 和 __MINGW64__ 。
\since build 520
*/
#define YF_Platform_MinGW64 0x4112

/*!
\brief Android ARM 平台。
\note 兼容 API Level 9 。
\since build 492
*/
#define YF_Platform_Android_ARM 0x4201

/*!
\def YF_Platform
\brief 目标平台。
\note 注意顺序。
*/
#ifdef __MINGW64__
#	define YF_Platform YF_Platform_MinGW64
#elif defined(__MINGW32__)
#	define YF_Platform YF_Platform_MinGW32
#elif defined(_WIN64)
#	define YF_Platform YF_Platform_Win64
#elif defined(_WIN32)
#	define YF_Platform YF_Platform_Win32
#elif defined(__ANDROID__)
// FIXME: Architecture detection.
#	define YF_Platform YF_Platform_Android_ARM
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
#elif YF_Platform == YF_Platform_Win32
#	define YCL_Win32 1
#	define YF_Hosted 1
#elif YF_Platform == YF_Platform_MinGW32
#	define YCL_MinGW 1
#	define YCL_Win32 1
#	define YF_Hosted 1
#elif YF_Platform == YF_Platform_Win64
#	define YCL_Win32 1
#	define YCL_Win64 1
#	define YF_Hosted 1
#elif YF_Platform == YF_Platform_MinGW64
#	define YCL_MinGW 1
#	define YCL_Win32 1
#	define YCL_Win64 1
#	define YF_Hosted 1
#elif YF_Platform == YF_Platform_Android_ARM
#	define YCL_Android 1
#	define YF_Hosted 1
#	define YCL_API_FILESYSTEM_POSIX
#else
#	error "Unsupported platform found."
#endif

// NOTE: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63287 .
#if __STDCPP_THREADS__
#	define YF_Multithread 1
#elif YCL_Win32 || YCL_Android && defined(_MT)
#	define YF_Multithread 1
#else
#	define YF_Multithread 0
#endif

#endif

