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
\since 早于 build 132 。
\version r1450;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-24 23:14:51 +0800;
\par 修改时间:
	2012-06-22 12:12 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFile;
*/


#include "YSLib/Service/yfile.h"
#include "YSLib/Core/yfilesys.h"

YSL_BEGIN

File::File()
	: fp(), fsize(0)
{}
File::File(const_path_t filename, bool is_text)
	: fp(), fsize(0)
{
	if(Open(filename, is_text))
	{
		Seek(0, SEEK_END);
		fsize = GetPosition();
		Rewind();
	}
}
File::File(const String& filename, bool is_text)
	: fp(), fsize(0)
{
	if(Open(filename, is_text))
	{
		Seek(0, SEEK_END);
		fsize = GetPosition();
		Rewind();
	}
}

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
File::Open(const_path_t filename, bool is_text)
{
	Close();
	if((fp = ufopen(filename, is_text ? "r" : "rb")))
		CheckSize();
	return bool(*this);
}

bool
File::Open(const String& filename, bool is_text)
{
	Close();
	if((fp = ufopen(filename.c_str(), is_text ? u"r" : u"rb")))
		CheckSize();
	return bool(*this);
}

YSL_END

