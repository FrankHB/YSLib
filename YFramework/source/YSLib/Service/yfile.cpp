/*
	Copyright (C) by Franksoft 2009 - 2011.

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
\version r1390;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-24 23:14:51 +0800;
\par 修改时间:
	2011-12-30 10:12 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFile;
*/


#include "YSLib/Service/yfile.h"

YSL_BEGIN

File::File()
	: fp(), fsize(0)
{}
File::File(const_path_t p, bool is_text)
	: fp(), fsize(0)
{
	if(Open(p, is_text))
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
File::Close()
{
	if(IsValid())
		std::fclose(fp);
}

bool
File::Open(const_path_t p, bool is_text)
{
	Close();
	if((fp = std::fopen(p, is_text ? "r" : "rb")))
	{
		Seek(0, SEEK_END);
		fsize = GetPosition();
		Rewind();
	}
	return IsValid();
}

YSL_END

