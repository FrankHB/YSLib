﻿/*
	© 2009-2016, 2019, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextFile.cpp
\ingroup Service
\brief 平台无关的文本文件抽象。
\version r1412
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 23:14:51 +0800
\par 修改时间:
	2021-12-24 23:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextFile
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_TextFile // for ystdex::min;
#include YFM_CHRLib_Convert
#include <ystdex/utility.hpp> // for ystdex::as_const;

namespace YSLib
{

namespace Text
{

Encoding
VerifyEncoding(std::FILE* fp, char* s, size_t buflen, size_t txt_len,
	Encoding enc)
{
	const auto n(ystdex::min(txt_len, buflen));

	std::char_traits<char>::assign(Nonnull(s) + n, buflen - n, char());
	if(std::fread(s, 1, n, Nonnull(fp)) == n)
	{
		std::rewind(fp);
		if(VerifyUC(&ystdex::as_const(s[0]), ptrdiff_t(n), enc))
			return enc;
	}
	return CharSet::Null;
}
Encoding
VerifyEncoding(std::streambuf& sb, char* s, size_t buflen, size_t txt_len,
	Encoding enc)
{
	const auto n(ystdex::min(txt_len, buflen));

	std::char_traits<char>::assign(Nonnull(s) + n, buflen - n, char());

	if(sb.sgetn(s, std::streamsize(n)) == std::streamsize(n))
	{
		;
		if(sb.pubseekpos(0, std::ios_base::in)
			!= std::streampos(std::streamoff(-1))
			&& VerifyUC(&ystdex::as_const(s[0]), ptrdiff_t(n), enc))
			return enc;
	}
	return CharSet::Null;
}
Encoding
VerifyEncoding(std::istream& is, char* s, size_t buflen, size_t txt_len,
	Encoding enc)
{
	const auto n(ystdex::min(txt_len, buflen));

	std::char_traits<char>::assign(Nonnull(s) + n, buflen - n, char());
	is.read(s, std::streamsize(n));
	if(is)
	{
		is.seekg(0);
		if(VerifyUC(&ystdex::as_const(s[0]), ptrdiff_t(n), enc))
			return enc;
	}
	return CharSet::Null;
}

bool
CheckBOM(std::istream& is, string_view str)
{
	const auto len(str.length());
	// NOTE: Max allowed size in all BOMs is needed.
	array<char, 4> buf;

	YAssert(len <= 4, "BOM length is too long.");
	is.read(buf.data(), std::streamsize(len));
	return is.gcount() == std::streamsize(len)
		? CheckBOM(buf.data(), str.data(), len) : false;
}

pair<Encoding, size_t>
DetectBOM(const char* buf)
{
	YAssertNonnull(buf);
#define YSL_Impl_DetectBOM(_n) \
	if(CheckBOM(buf, BOM_##_n)) \
		return {CharSet::_n, size(BOM_##_n) - 1};
	YSL_Impl_DetectBOM(UTF_32LE)
	YSL_Impl_DetectBOM(UTF_32BE)
	YSL_Impl_DetectBOM(UTF_8)
	YSL_Impl_DetectBOM(UTF_16LE)
	YSL_Impl_DetectBOM(UTF_16BE)
#undef YSL_Impl_DetectBOM
	return {CharSet::Null, 0};
}
pair<Encoding, size_t>
DetectBOM(string_view sv)
{
	YAssertNonnull(sv.data());
#define YSL_Impl_DetectBOM(_n) \
	if(sv.length() >= size(BOM_##_n) - 1 && CheckBOM(sv.data(), BOM_##_n)) \
		return {CharSet::_n, size(BOM_##_n) - 1};
	YSL_Impl_DetectBOM(UTF_32LE)
	YSL_Impl_DetectBOM(UTF_32BE)
	YSL_Impl_DetectBOM(UTF_8)
	YSL_Impl_DetectBOM(UTF_16LE)
	YSL_Impl_DetectBOM(UTF_16BE)
#undef YSL_Impl_DetectBOM
	return {CharSet::Null, 0};
}
pair<Encoding, size_t>
DetectBOM(std::streambuf& sb, std::size_t fsize, Encoding enc)
{
	const auto pos(sb.pubseekpos(0, std::ios_base::in));

	if(pos != std::streampos(std::streamoff(-1)))
	{
		if(fsize > 1U)
		{
			array<char, 4> buf;
			const auto n(sb.sgetn(buf.data(), 4));

			if(n != 0)
			{
				const auto res(DetectBOM(string_view(buf.data(),
					CheckNonnegative<size_t>(n))));

				if(sb.pubseekpos(std::streamoff(res.second), std::ios_base::in)
					!= std::streampos(std::streamoff(-1)) && res.second != 0)
					return res;
			}
			else
				return {CharSet::Null, 0};
		}

		char s[64U];

		return {VerifyEncoding(sb, s, size(s), size_t(fsize), enc), 0};
	}
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
		if(bool(is) || is.eof())
		{
			const auto res(DetectBOM(string_view(buf.data(),
				CheckNonnegative<size_t>(is.gcount()))));

			is.seekg(std::istream::off_type(res.second));
			if(res.second != 0)
				return res;
		}
		else
			return {CharSet::Null, 0};
	}

	char s[64U];

	return {VerifyEncoding(is, s, size(s), size_t(fsize), enc), 0};
}

size_t
WriteBOM(std::ostream& os, Encoding enc)
{
	switch(enc)
	{
#define YSL_Impl_WriteBOM(_n) \
	case CharSet::_n: \
		ystdex::write_literal(os, BOM_##_n); \
		return size(BOM_##_n) - 1;
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

