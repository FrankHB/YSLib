/*
	Copyright by FrankHB 2011 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NativeAPI.h
\ingroup YCLib
\brief 通用平台应用程序接口描述。
\version r536
\author FrankHB <frankhb1989@gmail.com>
\since build 202
\par 创建时间:
	2011-04-13 20:26:21 +0800
\par 修改时间:
	2013-02-14 04:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::NativeAPI
*/


#ifndef YCL_INC_NATIVEAPI_H_
#define YCL_INC_NATIVEAPI_H_ 1

//平台定义。
#include "Platform.h"

#ifndef YCL_PLATFORM
#	error Unknown platform!
#endif

#define YCL_YCOMMON_H <ycommon.h>
#define YSL_YSBUILD_H <ysbuild.h>

/*!	\defgroup workarounds Workarounds
\brief 替代方案。
\since build 297
*/

/*!	\defgroup name_collision_workarounds Name collision workarounds
\brief 名称冲突替代方案。
\note Windows API 冲突时显式使用带 A 或 W 的全局函数名称。
\since build 381
*/


#ifdef YCL_API_USE_UNISTD
#include <unistd.h>
#endif

#ifdef YCL_API_USE_SYS_DIR
#include <sys/dir.h>
#endif


#if YCL_DS

#include <nds.h>
#include <fat.h>
//包含 PALib 。
//#include <PA9.h>

#ifdef USE_EFS
//包含 EFSLib 。
#include "efs_lib.h"
#endif

namespace platform_ex
{

/*!
\brief 判断 ::dirent 指定的节点是否为目录。
\since build 312
*/
inline bool
IsDirectory(const ::dirent& d)
{
	return d.d_type & DT_DIR;
}

} // namespace platform_ex;


#elif defined(YCL_PLATFORM_MINGW32)

#ifndef UNICODE
#	define UNICODE 1
#endif

#include <Windows.h>
#include <Windowsx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

/*!
\ingroup name_collision_workarounds
\brief 禁止使用 DialogBox 宏。
\since build 297
*/
#undef DialogBox

/*!
\ingroup name_collision_workarounds
\brief 禁止使用 DrawText 宏。
\since build 298
*/
#undef DrawText

/*!
\ingroup name_collision_workarounds
\brief 禁止使用 FindWindow 宏。
\since build 381
*/
#undef FindWindow

/*!
\ingroup name_collision_workarounds
\brief 禁止使用 GetObject 宏。
\since build 313
*/
#undef GetObject

/*!
\ingroup name_collision_workarounds
\brief 禁止使用 PostMessage 宏。
\since build 298
*/
#undef PostMessage


#define NAME_MAX 256

#define S_IFIFO		_S_IFIFO

#define	S_IRWXG		(S_IRGRP | S_IWGRP | S_IXGRP)
#define		S_IRGRP	0
#define		S_IWGRP	0
#define		S_IXGRP 0
#define	S_IRWXO		(S_IROTH | S_IWOTH | S_IXOTH)
#define		S_IROTH	0
#define		S_IWOTH	0
#define		S_IXOTH 0


/*!
\brief 平台替代命名空间。
\since build 298
*/
namespace platform_replace
{

/*!
\brief 修正 MinGW 中的 mkdir 参数问题。

忽略第二参数。
*/
inline int
makedir(char const* dir, mode_t)
{
	return ::mkdir(dir);
}

} // namespace platform_replace;


extern "C"
{

typedef struct dirent
{
	long d_ino;
	off_t d_off;
	unsigned short d_reclen;
	char d_name[NAME_MAX + 1];
	/*!
	\brief Win32 文件查找信息指针。
	\since build 299
	*/
	::LPWIN32_FIND_DATAW lpWinDir;
} dirent;


typedef struct DIR
{
	/*!
	\brief 目录名称。
	\since build 298
	*/
	char Name[NAME_MAX];
	/*!
	\brief 节点句柄。
	\since build 298
	*/
	::HANDLE hNode;
	/*!
	\brief Win32 文件查找信息。
	\since build 299
	*/
	::WIN32_FIND_DATAW WinDir;
	/*!
	\brief POSIX 目录信息。
	\since build 298
	*/
	dirent POSIXDir;
} DIR;


YF_API DIR*
opendir(const char*);

YF_API dirent*
readdir(DIR*);

YF_API void
rewinddir(DIR*);

YF_API int
closedir(DIR*);

/*!
\def mkdir
\brief 修正 MinGW 中的 mkdir 参数问题。
\see platform_replace::makedir
\since build 298
*/
#define mkdir platform_replace::makedir

} // extern "C";


namespace platform_ex
{

/*!
\brief 判断 WIN32_FIND_DATAA 指定的节点是否为目录。
\since build 298
*/
inline bool
IsDirectory(const ::WIN32_FIND_DATAA& d)
{
	return d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}
/*!
\brief 判断 WIN32_FIND_DATAW 指定的节点是否为目录。
\since build 299
*/
inline bool
IsDirectory(const ::WIN32_FIND_DATAW& d)
{
	return d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}
/*!
\brief 判断 ::dirent 指定的节点是否为目录。
\since build 312
*/
inline bool
IsDirectory(const ::dirent& d)
{
	return d.lpWinDir && IsDirectory(*d.lpWinDir);
}
/*!
\brief 判断 ::DIR 指定的节点是否为目录。
\since build 312
*/
inline bool
IsDirectory(const ::DIR& d)
{
	return IsDirectory(d.WinDir);
}

} // namespace platform_ex;

#endif

#endif

