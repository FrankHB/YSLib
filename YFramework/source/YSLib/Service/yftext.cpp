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
\version r903
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 23:14:51 +0800
\par 修改时间:
	2012-09-04 12:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YFile_(Text)
*/


#include "YSLib/Service/yftext.h"

YSL_BEGIN

using namespace Text;
using std::memcmp;

namespace
{

bool
CheckUTF8(const char* s, const char* g)
{
	while(s < g && *s != 0
		&& MBCToUC(s, CharSet::UTF_8) == ConversionResult::OK);
	return s == g || *s == 0;
}

// TODO: More accurate encoding checking for text stream without BOM.
Encoding
CheckEncoding(const char* s, size_t n)
{
	return CheckUTF8(s, s + n) ? CharSet::UTF_8 : CharSet::GBK;
}

void
InitializeTextFile(TextFile& tf, size_t& bl)
{
	if(tf)
	{
		tf.Seek(0, SEEK_END);
		bl = tf.CheckBOM(tf.Encoding);
		tf.Rewind();
	}
	if(bl == 0)
	{
#define YSL_TXT_CHECK_ENCODING_N 64U
		char s[YSL_TXT_CHECK_ENCODING_N + 6];
		const auto n(min(tf.GetTextSize(), YSL_TXT_CHECK_ENCODING_N));
#undef YSL_TXT_CHECK_ENCODING_N

		std::memset(s + n, 0, arrlen(s) - n);
		tf.Read(s, 1, n);
		tf.Rewind();
		tf.Encoding = CheckEncoding(s, n);
	//	tf.Encoding = CS_Default;
	}
}

} // unnamed namespace;


TextFile::TextFile(const_path_t filename)
	: File(filename, "r"),
	bl(0), Encoding(CharSet::Null)
{
	InitializeTextFile(*this, bl);
}
TextFile::TextFile(const String& filename)
	: File(filename, u"r"),
	bl(0), Encoding(CharSet::Null)
{
	InitializeTextFile(*this, bl);
}

size_t
TextFile::CheckBOM(Text::Encoding& cp)
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

YSL_END

