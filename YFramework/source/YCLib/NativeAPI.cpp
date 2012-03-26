/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NativeAPI.cpp
\ingroup YCLib
\brief 通用平台应用程序接口描述。
\version r1265;
\author FrankHB<frankhb1989@gmail.com>
\since build 296 。
\par 创建时间:
	2012-03-26 13:36:28 +0800;
\par 修改时间:
	2012-03-26 14:45 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::YCommon;
*/


#include "YCLib/NativeAPI.h"
#include <ydef.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace
{
}

#ifdef YCL_DS

static int t; //object file place holder;

#elif defined(YCL_MINGW32)

DIR*
opendir(const char *name)
{
	const auto dir(static_cast<DIR*>(std::malloc(sizeof(DIR))));

	std::sprintf(dir->dirname, "%s\\*", name),
	yunseq(dir->h = 0, dir->posixdir.pwindir = &dir->windir);
	return dir;
}

dirent*
readdir(DIR* dir)
{
	if(dir->h == 0)
	{
		dir->h = ::FindFirstFile(dir->dirname, &dir->windir);
		if(dir->h == reinterpret_cast<void*>(0xFFFFFFFF))
			dir->h = 0;
	}
	else if(!::FindNextFile(dir->h,&dir->windir))
		dir->h = 0;
	if(dir->h && dir->h != reinterpret_cast<void*>(0xFFFFFFFF))
	{
		std::strcpy(dir->posixdir.d_name, dir->windir.cFileName);
		dir->posixdir.d_reclen = std::strlen(dir->posixdir.d_name);
		yunseq(dir->posixdir.d_off = 0, dir->posixdir.d_ino = 0);
	}
	return dir->h == 0 ? 0 : &dir->posixdir;
}

void
rewinddir(DIR* dir)
{
	if(dir->h)
	{
		::FindClose(dir->h);
		dir->h = 0;
	}
}

int
closedir(DIR *dir)
{
	if(dir)
	{
		if(dir->h)
			::FindClose(dir->h);
		std::free(dir);
		return 0;
	}
	return -1;
}

char*
getcwd(char *buf,size_t size)
{
	return ::GetCurrentDirectory(size, buf) ? buf : 0;
}

int
chdir(char *path)
{
	return ::SetCurrentDirectory(path) ? 0 : -1;
}

int
rmdir(char *path)
{
	return ::RemoveDirectory(path) ? 0 : -1;
}

int
mkdir(char *path, mode_t mode)
{
	::SECURITY_ATTRIBUTES secatt;

	std::memset((&secatt), 0, sizeof(::SECURITY_ATTRIBUTES));
	return ::CreateDirectory(path, &secatt) ? 0 : -1;
}


int
stat(char *filename,struct stat *buf)
{
	::WIN32_FIND_DATA windat;
	HANDLE h(::FindFirstFile(filename, &windat));
	if(!h)
		return 1;
	buf->st_size = windat.nFileSizeHigh << 16 | windat.nFileSizeLow;
	return 0;
}

#endif

namespace platform
{
}

