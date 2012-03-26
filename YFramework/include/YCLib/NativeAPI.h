/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NativeAPI.h
\ingroup YCLib
\brief 通用平台应用程序接口描述。
\version r1295;
\author FrankHB<frankhb1989@gmail.com>
\since build 201 。
\par 创建时间:
	2011-04-13 20:26:21 +0800;
\par 修改时间:
	2012-03-26 16:45 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::NativeAPI;
*/


#ifndef YCL_INC_NATIVEAPI_H_
#define YCL_INC_NATIVEAPI_H_

//平台定义。
#include "Platform.h"

#ifndef YCL_PLATFORM
#	error Unknown platform!
#endif

#define YCL_YCOMMON_H <ycommon.h>
#define YSL_YSBUILD_H <YSLib/Helper/yshelper.h>


#ifdef YCL_DS

#include <nds.h>
#include <fat.h>
//包含 PALib 。
//#include <PA9.h>

#ifdef USE_EFS
//包含 EFSLib 。
#include "efs_lib.h"
#endif

#elif defined(YCL_PLATFORM_MINGW32)

#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>


#define NAME_MAX 256


#define S_IFMT		_S_IFMT
#define S_IFDIR		_S_IFDIR
#define S_IFCHR		_S_IFCHR
#define S_IFIFO		_S_IFIFO
#define S_IFREG		_S_IFREG
#define S_IREAD		_S_IREAD
#define S_IWRITE	_S_IWRITE
#define S_IEXEC		_S_IEXEC

#define	S_IRWXG		(S_IRGRP | S_IWGRP | S_IXGRP)
#define		S_IRGRP	0
#define		S_IWGRP	0
#define		S_IXGRP 0
#define	S_IRWXO		(S_IROTH | S_IWOTH | S_IXOTH)
#define		S_IROTH	0
#define		S_IWOTH	0
#define		S_IXOTH 0


typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;




typedef struct dirent
{
	long d_ino;
	off_t d_off;
	unsigned short d_reclen;
	char d_name[NAME_MAX + 1];
	::LPWIN32_FIND_DATA pwindir;
} dirent;


typedef struct DIR
{
	char dirname[NAME_MAX];
	HANDLE h;
	::WIN32_FIND_DATA windir;
	dirent posixdir;
} DIR;


DIR*
opendir(const char*);

dirent*
readdir(DIR*);

void
rewinddir(DIR*);

int
closedir(DIR*);

char*
getcwd(char*, size_t);

int
chdir(char*);

int
rmdir(char*);

int
mkdir(char*, mode_t);

int
stat(char*, struct stat*);

#endif

#ifdef YCL_API_USE_UNISTD
#include <unistd.h>
#endif

#ifdef YCL_API_USE_SYS_DIR
#include <sys/dir.h>
#endif

#endif

