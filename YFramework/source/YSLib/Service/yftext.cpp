/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yftext.cpp
\ingroup Core
\brief 平台无关的文本文件抽象。
\version r1834;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-24 23:14:51 +0800;
\par 修改时间:
	2012-02-21 15:10 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFile_(Text);
*/


#include "YSLib/Service/yftext.h"

YSL_BEGIN

using namespace Text;
using std::memcmp;

TextFile::TextFile(const_path_t p)
	: File(p, true),
	bl(0), cp(CharSet::Null)
{
	if(IsValid())
	{
		Seek(0, SEEK_END);
		bl = CheckBOM(cp);
		Rewind();
	}
	// TODO: encoding checking for text stream without BOM;
	if(bl == 0)
		cp = CharSet::GBK;
}

size_t
TextFile::CheckBOM(Encoding& cp)
{
	Rewind();
	if(fsize < 2)
		return 0;
	char tmp[4];
	Read(tmp, 1, 4);

	if(!memcmp(tmp, BOM_UTF_16LE, 2))
	{
		cp = CharSet::UTF_16LE;
		return 2;
	}
	if(!memcmp(tmp, BOM_UTF_16BE, 2))
	{
		cp = CharSet::UTF_16BE;
		return 2;
	}
	if(!memcmp(tmp, BOM_UTF_8, 3))
	{
		cp = CharSet::UTF_8;
		return 3;
	}
	if(!memcmp(tmp, BOM_UTF_32LE, 4))
	{
		cp = CharSet::UTF_32LE;
		return 4;
	}
	if(!memcmp(tmp, BOM_UTF_32BE, 4))
	{
		cp = CharSet::UTF_32BE;
		return 4;
	}
	return 0;
}

void
TextFile::Locate(u32 pos) const
{
	Seek(bl + pos, SEEK_SET);
}

void
TextFile::Rewind() const
{
	Seek(bl, SEEK_SET);
}

size_t
TextFile::Read(void* s, u32 n) const
{
	return Read(s, n, 1);
}

YSL_END

