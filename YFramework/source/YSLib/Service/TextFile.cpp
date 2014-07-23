/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextFile.cpp
\ingroup Service
\brief 平台无关的文本文件抽象。
\version r982
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 23:14:51 +0800
\par 修改时间:
	2014-07-22 15:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextFile
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_TextFile

namespace YSLib
{

using namespace Text;

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
		if(bl == 0)
		{
#define YSL_TXT_CHECK_ENCODING_N std::size_t(64U)
			char s[YSL_TXT_CHECK_ENCODING_N + 6];
			const auto n(min(tf.GetTextSize(), YSL_TXT_CHECK_ENCODING_N));
#undef YSL_TXT_CHECK_ENCODING_N

			std::char_traits<char>::assign(s + n, arrlen(s) - n, 0);
			tf.Read(s, 1, n);
			tf.Rewind();
			tf.Encoding = CheckEncoding(s, n);
		}
	}
}

} // unnamed namespace;


TextFile::TextFile(const char* filename, std::ios_base::openmode mode,
	Text::Encoding enc)
	: File(filename, mode),
	bl(0), Encoding(enc)
{
	if(bool(*this) && GetSize() == 0 && mode & std::ios_base::out)
		switch(enc)
		{
		case CharSet::UTF_16LE:
			yunseq(*this << BOM_UTF_16LE, bl = 2);
			break;
		case CharSet::UTF_16BE:
			yunseq(*this << BOM_UTF_16BE, bl = 2);
			break;
		case CharSet::UTF_8:
			yunseq(*this << BOM_UTF_8, bl = 3);
			break;
		case CharSet::UTF_32LE:
			yunseq(*this << BOM_UTF_32LE, bl = 4);
			break;
		case CharSet::UTF_32BE:
			yunseq(*this << BOM_UTF_32BE, bl = 4);
		default:
			break;
		}
	else
		InitializeTextFile(*this, bl);
}
TextFile::TextFile(const String& filename)
	: File(filename, u"r"),
	bl(0), Encoding(CharSet::Null)
{
	InitializeTextFile(*this, bl);
}

string
TextFile::GetBOM() const
{
	const size_t s(GetBOMSize());
	string str(s, char());

	File::Rewind();
	for(size_t i(0); i != s; ++i)
		str[i] = std::fgetc(GetPtr());
	return str;
}

size_t
TextFile::CheckBOM(Text::Encoding& cp)
{
	using std::char_traits;

	Rewind();
	if(GetSize() < 2)
		return 0;
	char tmp[4];
	Read(tmp, 1, 4);

	if(char_traits<char>::compare(tmp, BOM_UTF_16LE, 2) == 0)
	{
		cp = CharSet::UTF_16LE;
		return 2;
	}
	if(char_traits<char>::compare(tmp, BOM_UTF_16BE, 2) == 0)
	{
		cp = CharSet::UTF_16BE;
		return 2;
	}
	if(char_traits<char>::compare(tmp, BOM_UTF_8, 3) == 0)
	{
		cp = CharSet::UTF_8;
		return 3;
	}
	if(char_traits<char>::compare(tmp, BOM_UTF_32LE, 4) == 0)
	{
		cp = CharSet::UTF_32LE;
		return 4;
	}
	if(char_traits<char>::compare(tmp, BOM_UTF_32BE, 4) == 0)
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

bool
TextFile::Truncate(size_t size) const
{
	return File::Truncate(GetBOMSize() + size);
}

} // namespace YSLib;

