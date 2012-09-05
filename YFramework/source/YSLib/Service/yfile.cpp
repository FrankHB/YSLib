/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfile.cpp
\ingroup Core
\brief 平台无关的文件抽象。
\version r496
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 23:14:51 +0800
\par 修改时间:
	2012-09-04 12:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YFile
*/


#include "YSLib/Service/yfile.h"
#include "YSLib/Core/yfilesys.h"

YSL_BEGIN

File::File()
	: fp(), fsize(0)
{}
File::File(const_path_t filename, const char* mode)
	: File()
{
	if(Open(filename, mode))
	{
		Seek(0, SEEK_END);
		fsize = GetPosition();
		Rewind();
	}
}
File::File(const_path_t filename, std::ios_base::openmode mode)
	: File(filename, ystdex::openmode_conv(mode))
{}
File::File(const String& filename, const ucs2_t* mode)
	: File()
{
	if(Open(filename, mode))
	{
		Seek(0, SEEK_END);
		fsize = GetPosition();
		Rewind();
	}
}
File::File(const String& filename, std::ios_base::openmode mode)
	: File(filename, String(ystdex::openmode_conv(mode)).c_str())
{}

File::~File()
{
	Close();
}

void
File::CheckSize()
{
	Seek(0, SEEK_END);
	fsize = GetPosition();
	Rewind();
}

void
File::Close()
{
	if(*this)
		std::fclose(fp);
}

bool
File::Open(const_path_t filename, const char* mode)
{
	Close();
	if((fp = ufopen(filename, mode)))
		CheckSize();
	return fp;
}
bool
File::Open(const_path_t filename, std::ios_base::openmode mode)
{
	return Open(filename, ystdex::openmode_conv(mode));
}
bool
File::Open(const String& filename, const ucs2_t* mode)
{
	Close();
	if((fp = ufopen(filename.c_str(), mode)))
		CheckSize();
	return fp;
}
bool
File::Open(const String& filename, std::ios_base::openmode mode)
{
	return Open(filename, String(ystdex::openmode_conv(mode)).c_str());
}

YSL_END

