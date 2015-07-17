/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextFile.cpp
\ingroup Service
\brief 平台无关的文本文件抽象。
\version r1077
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 23:14:51 +0800
\par 修改时间:
	2015-07-18 00:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextFile
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_TextFile
#include YFM_CHRLib_Convert
#include <ystdex/cast.hpp> // for ystdex::as_const;

namespace YSLib
{

using namespace Text;

namespace
{

YB_NONNULL(1, 2) Encoding
VerifyEncoding(std::FILE* fp, char* s, size_t buflen, size_t txt_len)
{
	const auto n(std::min(txt_len, buflen));

	std::char_traits<char>::assign(Nonnull(s) + n, buflen - n, char());
	std::fread(s, 1, n, Nonnull(fp));
	std::rewind(fp);
	return VerifyUC(&ystdex::as_const(s[0]), ptrdiff_t(n), CharSet::UTF_8)
		? CharSet::UTF_8 : CharSet::Null;
}

void
InitializeTextFile(TextFile& tf, size_t& bl)
{
	bl = tf.CheckBOM(tf.Encoding);
	tf.Rewind();
	if(bl == 0)
	{
		char s[64U];

		// TODO: More accurate encoding checking for text stream without BOM.
		tf.Encoding = VerifyEncoding(tf.GetStream(), s, arrlen(s),
			tf.GetTextSize()) != CharSet::Null ? CharSet::UTF_8 : CharSet::GBK;
	}
}

} // unnamed namespace;


TextFile::TextFile(const char* filename, std::ios_base::openmode mode,
	Text::Encoding enc)
	: File(filename, mode),
	bl(0), Encoding(enc)
{
	if(bool(*this))
	{
		if(GetSize() == 0 && mode & std::ios_base::out)
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
}
TextFile::TextFile(const String& filename)
	: File(filename, u"r"),
	bl(0), Encoding(CharSet::Null)
{
	if(bool(*this))
		InitializeTextFile(*this, bl);
}
ImplDeDtor(TextFile)

string
TextFile::GetBOM() const
{
	const size_t s(GetBOMSize());
	string str(s, char());

	File::Rewind();
	for(size_t i(0); i != s; ++i)
		str[i] = std::fgetc(GetStream());
	return str;
}

size_t
TextFile::CheckBOM(Text::Encoding& cp)
{
	Rewind();
	if(GetSize() < 2)
		return 0;

	using std::char_traits;
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
TextFile::Locate(size_t pos) const
{
	// XXX: Conversion to 'long' might be implementation-defined.
	std::fseek(GetStream(), long(bl + pos), SEEK_SET);
}

void
TextFile::Rewind() const
{
	// XXX: Conversion to 'long' might be implementation-defined.
	std::fseek(GetStream(), long(bl), SEEK_SET);
}

} // namespace YSLib;

