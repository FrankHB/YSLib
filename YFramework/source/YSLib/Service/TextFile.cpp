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
\version r1300
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 23:14:51 +0800
\par 修改时间:
	2015-08-05 09:44 +0800
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

namespace Text
{

Encoding
VerifyEncoding(std::FILE* fp, char* s, size_t buflen, size_t txt_len,
	Encoding enc)
{
	const auto n(min(txt_len, buflen));

	std::char_traits<char>::assign(Nonnull(s) + n, buflen - n, char());
	std::fread(s, 1, n, Nonnull(fp));
	std::rewind(fp);
	return VerifyUC(&ystdex::as_const(s[0]), ptrdiff_t(n), enc) ? enc
		: CharSet::Null;
}
Encoding
VerifyEncoding(std::istream& stream, char* s, size_t buflen, size_t txt_len,
	Encoding enc)
{
	const auto n(min(txt_len, buflen));

	std::char_traits<char>::assign(Nonnull(s) + n, buflen - n, char());
	stream.read(s, std::streamsize(n));
	stream.seekg(0);
	return VerifyUC(&ystdex::as_const(s[0]), ptrdiff_t(n), enc) ? enc
		: CharSet::Null;
}

pair<Encoding, size_t>
DetectBOM(const char* buf)
{
#define YSL_Impl_DetectBOM(_n) \
	if(CheckBOM(buf, BOM_##_n)) \
		return {CharSet::_n, arrlen(BOM_##_n) - 1};
	YSL_Impl_DetectBOM(UTF_16LE)
	YSL_Impl_DetectBOM(UTF_16BE)
	YSL_Impl_DetectBOM(UTF_8)
	YSL_Impl_DetectBOM(UTF_32LE)
	YSL_Impl_DetectBOM(UTF_32BE)
#undef YSL_Impl_DetectBOM
	return {CharSet::Null, 0};
}
pair<Encoding, size_t>
DetectBOM(std::istream& is, std::size_t fsize, Encoding enc)
{
	is.seekg(0);
	if(fsize > 1U)
	{
		array<char, 4> buf;

		is.read(buf.data(), 4);
		if(is)
		{
			const auto res(DetectBOM(buf.data()));

			is.seekg(std::istream::pos_type(res.second));
			if(res.second != 0)
				return res;
		}
		else if(is.eof())
			is.seekg(0);
		else
			return {CharSet::Null, 0};
	}

	char s[64U];

	return {VerifyEncoding(is, s, arrlen(s), size_t(fsize), enc), 0};
}

size_t
WriteBOM(std::ostream& os, Encoding enc)
{
	switch(enc)
	{
#define YSL_Impl_WriteBOM(_n) \
	case CharSet::_n: \
		ystdex::write_literal(os, BOM_##_n); \
		return arrlen(BOM_##_n) - 1;
	YSL_Impl_WriteBOM(UTF_16LE)
	YSL_Impl_WriteBOM(UTF_16BE)
	YSL_Impl_WriteBOM(UTF_8)
	YSL_Impl_WriteBOM(UTF_32LE)
	YSL_Impl_WriteBOM(UTF_32BE)
#undef YSL_Impl_WriteBOM
	default:
		return 0;
	}
}

} // namespace Text;

} // namespace YSLib;

