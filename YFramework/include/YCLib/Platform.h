/*
	© 2009-2016, 2018-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Platform.h
\ingroup YCLib
\brief 通用平台描述文件。
\version r842
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 00:05:08 +0800
\par 修改时间:
	2019-12-26 22:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Platform
*/


#ifndef YCL_INC_Platform_h_
#define YCL_INC_Platform_h_ 1

#include <ydef.h> // for std::uintmax_t, ystdex::byte, ystdex::octet,
//	ystdex::ptrdiff_t, ystdex::size_t, ystdex::wint_t, ystdex::nullptr_t;

/*!
\def YF_DLL
\brief 使用 YFramework 动态链接库。
\since build 361
*/

#if defined(YF_DLL) && defined(YF_BUILD_DLL)
#	error "DLL could not be built and used at the same time."
#endif

#if YB_IMPL_MSCPP \
	|| (YB_IMPL_GNUCPP && (defined(__MINGW32__) || defined(__CYGWIN__)))
#	ifdef YF_DLL
#		define YF_API __declspec(dllimport)
#	elif defined(YF_BUILD_DLL)
#		define YF_API __declspec(dllexport)
#	else
#		define YF_API
#	endif
#elif defined(YF_BUILD_DLL) && (YF_IMPL_GNUCPP >= 40000 || YF_IMPL_CLANGPP)
#	define YF_API YF_ATTR(__visibility__("default"))
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

/*!	\defgroup Platforms Platforms
\brief 平台标识和相关接口。
*/
/*!	\defgroup PlatformAPIs Platform APIs
\brief 平台 API 支持。
\note 若未另行指定，仅当定义为非零整数值表示有效。
\since build 553
*/
/*!	\defgroup PlatformEmulation Platforms Emulation
\brief 平台模拟和相关支持。
\since build 562
*/
/*!	\defgroup PlatformOptionalFeatures Platform Optional Features
\brief 平台可选特性支持。
\note 仅当未被定义时 YCLib 提供定义。
\note 若未另行指定，仅当定义为非零整数值表示有效。
\since build 552
*/

/*!	\defgroup DS Nintendo/iQue Dual Screen[L/LL/i]
\ingroup Platforms
\brief DS 平台。
*/
/*!	\defgroup Win32 MinGW for Win32
\ingroup Platforms
\brief Win32 平台。
\since build 633
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

/*!
\ingroup Platforms
\brief 目标平台标识。
\note 替换为整数字面量。
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
\brief Android 抽象平台。
\since build 652
*/
#define YF_Platform_Android 0x4200

/*!
\brief Android ARM 平台。
\note 兼容 API Level 9 。
\since build 492
*/
#define YF_Platform_Android_ARM 0x4201

/*!
\brief Linux 抽象平台。
\since build 652
*/
#define YF_Platform_Linux 0x4300

/*!
\brief Linux x86 平台。
\note 兼容 i686 及以上。
\warning 编译时只检查 i386 。
\since build 559
*/
#define YF_Platform_Linux_x86 0x4301

/*!
\brief Linux x86_64 平台。
\since build 559
*/
#define YF_Platform_Linux_x64 0x4302

/*!
\brief OS X 平台。
\since build 561
*/
#define YF_Platform_OS_X 0x4401

/*!
\def YF_Platform
\brief 目标平台。
\note 注意顺序。
*/
#ifdef __MSYS__
#	error "MSYS is not currently supported. Use MinGW targets instead."
#elif defined(__MINGW64__)
#	define YF_Platform YF_Platform_MinGW64
#elif defined(__MINGW32__)
#	define YF_Platform YF_Platform_MinGW32
#elif defined(_WIN64)
#	define YF_Platform YF_Platform_Win64
#elif defined(_WIN32)
#	define YF_Platform YF_Platform_Win32
#elif defined(__ANDROID__)
// TODO: Precise architecture detection.
#	define YF_Platform YF_Platform_Android_ARM
#elif defined(__linux__)
#	ifdef __i386__
#		define YF_Platform YF_Platform_Linux_x64
#	elif defined(__x86_64__)
#		define YF_Platform YF_Platform_Linux_x86
#	endif
#elif defined(__APPLE__)
#	ifdef __MACH__
#		define YF_Platform YF_Platform_OS_X
#	else
#		error "Apple platforms other than OS X is not supported."
#	endif
#elif !defined(YF_Platform)
//当前默认以 DS 作为目标平台。
#	define YF_Platform YF_Platform_DS
#endif
//@}


//! \since build 553
//@{
/*!
\ingroup PlatformAPI
\def YCL_API_POSIXFileSystem
\brief 使用 POSIX 文件系统 API 。
*/

/*!
\ingroup PlatformAPI
\def YCL_API_Has_dirent_h
\brief API 可用 \c \<dirent.h> 。
*/

/*!
\ingroup PlatformAPI
\def YCL_API_Has_semaphore_h
\brief API 可用 \c \<semaphore.h> 。
*/

/*!
\ingroup PlatformAPI
\def YCL_API_Have_unistd_h
\brief API 可用 \c \<unistd.h> 。
*/

/*!
\ingroup PlatformOptionalFeatures
\def YF_Use_POSIXThread
\brief 使用 POSIX 线程接口。
\note 当前仅支持 \c posix 线程模型 G++ 。
\sa https://gcc.gnu.org/bugzilla/show_bug.cgi?id=11953
*/
//@}

/*!
\ingroup PlatformOptionalFeatures
\def YF_Use_JNI
\brief 使用 Java 本机接口的版本。
\pre 若被定义，替换的记号和在 \c \<jni.h> 中的某个 \c JNI_VERSION_* 宏一致。
\since build 552
*/

/*!
\ingroup PlatformOptionalFeatures
\def YF_Use_XCB
\brief 使用 X C 绑定。
\pre 若被定义，替换的记号为版本号。
\since build 561
*/

/*!
\ingroup PlatformOptionalFeatures
\def YCL_HostedUI
\brief 宿主用户界面支持。
\note 允许预定义。
\since build 563

非零值表示支持宿主用户界面，否则表示不支持。
*/

/*!
\ingroup PlatformOptionalFeatures
\def YCL_HostedUI_XCB
\brief 在宿主用户界面使用 X C 绑定。
\note 取消外部定义。
\since build 563

非零值表示宿主用户界面使用 XCB ，否则表示不使用。
对 Linux 默认定义。若定义后未定义 YF_Use_XCB ，则提供默认定义。
*/
#undef YCL_HostedUI_XCB


/*!
\def YCL_Device_Cursor_FixedKey
\brief 1 表示保证指针输入设备的对应数据结构只保存一个按键状态。
\note 接收指针设备输入时不需要区分多个按键状态，可用于输入处理优化。
\since build 436
*/


#ifdef __linux__
#	define YCL_Linux 1
#	define YF_Hosted 1
#	ifndef YF_Use_POSIXThread
#		define YF_Use_POSIXThread 1
#	endif
#	define YCL_API_POSIXFileSystem 1
#	define YCL_API_Has_dirent_h 1
#	define YCL_API_Has_semaphore_h 1
#	define YCL_API_Has_unistd_h 1
#elif defined(__MACH__)
#	define YF_Hosted 1
#endif
#if YF_Platform == YF_Platform_DS
#	define YCL_DS 1
#	define YF_Hosted 0
#	define YCL_API_POSIXFileSystem 1
#	define YCL_API_Has_dirent_h 1
#	define YCL_API_Has_unistd_h 1
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
#	ifndef YF_Use_POSIXThread
#		define YF_Use_POSIXThread 1
#	endif
#elif YF_Platform == YF_Platform_MinGW64
#	define YCL_MinGW 1
#	define YCL_Win32 1
#	define YCL_Win64 1
#	define YF_Hosted 1
#	ifndef YF_Use_POSIXThread
#		define YF_Use_POSIXThread 1
#	endif
#elif YF_Platform == YF_Platform_Android_ARM
#	define YCL_Android 1
#	ifndef YF_Use_POSIXThread
#		define YF_Use_POSIXThread 1
#	endif
#	ifndef YF_Use_JNI
#		define YF_Use_JNI 0x00010006
#	endif
#elif YF_Platform == YF_Platform_Linux_x86 \
	|| YF_Platform == YF_Platform_Linux_x64
#	define YCL_HostedUI_XCB 1
#elif YF_Platform == YF_Platform_OS_X
#	define YCL_OS_X 1
#	ifndef YF_Use_POSIXThread
#		define YF_Use_POSIXThread 1
#	endif
#	define YCL_API_POSIXFileSystem 1
#	define YCL_API_Has_dirent_h 1
#	define YCL_API_Has_unistd_h 1
#else
#	error "Unsupported platform found."
#endif

#ifndef YCL_HostedUI_XCB
#	define YCL_HostedUI_XCB 0
#endif
#ifndef YCL_HostedUI
#	if YCL_HostedUI_XCB || YCL_Win32 || YCL_Android
#		define YCL_HostedUI 1
#	else
#		define YCL_HostedUI 0
#	endif
#endif

#if YCL_HostedUI_XCB && !defined(YF_Use_XCB)
#	define YF_Use_XCB 0x11100
#endif

// NOTE: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63287.
#if __STDCPP_THREADS__
#	define YF_Multithread 1
#elif YCL_Win32 || YCL_Android || YCL_Linux || YCL_OS_X
#	define YF_Multithread 1
#else
#	define YF_Multithread 0
#endif

/*!	\defgroup TopLevelPlatformNamespaces Top-level Platform Namespaces
\brief 顶级平台命名空间。
\since build 652
*/

//! \ingroup TopLevelPlatformNamespaces
namespace platform
{

//! \since build 593
inline namespace basic_types
{

/*!
\brief 平台通用数据类型。
\since build 209
*/
//@{
//! \since build 417
using ystdex::byte;
//! \since build 417
using ystdex::octet;
using ystdex::ptrdiff_t;
using ystdex::size_t;
//! \since build 245
using ystdex::wint_t;
//! \since build 644
using ystdex::nullptr_t;
//@}

} // inline namespace basic_types;

} // namespace platform;

//! \ingroup TopLevelPlatformNamespaces
namespace platform_ex
{

using namespace platform::basic_types;

} // namespace platform_ex;

#endif

