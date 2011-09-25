/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yftext.cpp
\ingroup Core
\brief 平台无关的文本文件抽象。
\version r1812;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-24 23:14:51 +0800;
\par 修改时间:
	2011-09-24 20:19 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFile_(Text);
*/


#include "yftext.h"

YSL_BEGIN

using namespace Text;
using std::memcmp;

TextFile::TextFile(const_path_t p)
	: File(p, true),
	bl(0), cp(CharSet::Null)
{
	if(IsValid())
	{
		SetPosition(0, SEEK_END);
		bl = CheckBOM(cp);
		Rewind();
	}
	if(bl == 0)
		cp = CP_Local;
}

u8
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
TextFile::Rewind() const
{
	SetPosition(bl, SEEK_SET);
}

void
TextFile::SetPos(u32 pos) const
{
	SetPosition(bl + pos, SEEK_SET);
}

void
TextFile::Seek(long offset, int whence) const
{
	if(whence == SEEK_SET)
		SetPos(offset);
	else
		SetPosition(offset, whence);
}

TextFile::SizeType
TextFile::Read(void* s, u32 n) const
{
	return Read(s, n, 1);
}

TextFile::SizeType
TextFile::ReadS(ucs2_t* s, u32 n) const
{
	u32 l(0);

	if(IsValid())
	{
		u32 i(0);

		while(i < n)
		{
			if(CheckEOF())
				break;
			i += MBCToUC(s[l++], fp, cp);
		}
	}
	return l;
}

YSL_END

