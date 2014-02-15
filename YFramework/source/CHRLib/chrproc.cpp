/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file chrproc.cpp
\ingroup CHRLib
\brief 字符编码处理。
\version r1206
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-17 17:53:21 +0800
\par 修改时间:
	2014-02-14 22:53 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::CharacterProcessing
*/


#include "CHRLib/YModules.h"
#include YFM_CHRLib_CharacterProcessing
#include YFM_CHRLib_MappingEx
#include <cctype>
#include <cstdlib>
#include <cwchar>
#include <ystdex/cstdio.h>
#include <ystdex/cstring.h>
#include YFM_CHRLib_Convert

namespace CHRLib
{

using std::malloc;
using std::size_t;
using std::tolower;
using ystdex::input_monomorphic_iterator;
using ystdex::is_null;
using ystdex::ntctslen;

ConversionResult
MBCToUC(ucs2_t& uc, const char*& c, Encoding enc, ConversionState&& st)
{
	if(const auto pfun = FetchMapperPtr<ConversionResult(ucs2_t&,
		input_monomorphic_iterator&&, ConversionState&&)>(enc))
		return ConvertCharacter(pfun, uc, c, std::move(st));
	return ConversionResult::Unhandled;
}
ConversionResult
MBCToUC(ucs2_t& uc, std::FILE* fp, Encoding enc, ConversionState&& st)
{
	yconstraint(fp);

	if(const auto pfun = FetchMapperPtr<ConversionResult(ucs2_t&,
		input_monomorphic_iterator&&, ConversionState&&)>(enc))
	{
		ystdex::ifile_iterator i(fp);
		const auto r(ConvertCharacter(pfun, uc, i, std::move(st)));

		std::ungetc(*i, fp);
		return r;
	}
	return ConversionResult::Unhandled;
}
ConversionResult
MBCToUC(const char*& c, Encoding enc, ConversionState&& st)
{
	if(const auto pfun = FetchMapperPtr<ConversionResult(
		input_monomorphic_iterator&&, ConversionState&&)>(enc))
		return ConvertCharacter(pfun, c, std::move(st));
	return ConversionResult::Unhandled;
}
ConversionResult
MBCToUC(std::FILE* fp, Encoding enc, ConversionState&& st)
{
	yconstraint(fp);

	if(const auto pfun = FetchMapperPtr<ConversionResult(
		input_monomorphic_iterator&&, ConversionState&&)>(enc))
	{
		ystdex::ifile_iterator i(fp);
		const auto r(ConvertCharacter(pfun, i, std::move(st)));

		std::ungetc(*i, fp);
		return r;
	}
	return ConversionResult::Unhandled;
}

size_t
UCToMBC(char* d, const ucs2_t& s, Encoding enc)
{
	yconstraint(d);

	size_t l(0);

	if(const auto pfun = FetchMapperPtr<byte(char*, const ucs2_t&)>(enc))
		l = pfun(d, s);
	return l;
}


size_t
MBCSToUCS2(ucs2_t* d, const char* s, Encoding enc)
{
	yconstraint(d),
	yconstraint(s);

	const auto p(d);

	if(const auto pfun = FetchMapperPtr<ConversionResult(ucs2_t&,
		input_monomorphic_iterator&&, ConversionState&&)>(enc))
		while(!is_null(*s))
		{
			ConversionState st;

			ConvertCharacter(pfun, *d++, s, std::move(st));
		}
	*d = 0;
	return d - p;
}

size_t
UCS2ToMBCS(char* d, const ucs2_t* s, Encoding enc)
{
	yconstraint(d),
	yconstraint(s);

	const auto p(d);

	if(const auto pfun = FetchMapperPtr<byte(char*, const ucs2_t&)>(enc))
		while(!is_null(*s))
			d += pfun(d, *s++);
	*d = 0;
	return d - p;
}

size_t
UCS4ToUCS2(ucs2_t* d, const ucs4_t* s)
{
	yconstraint(d),
	yconstraint(s);

	const auto p(d);

	while(!is_null(*s))
		*d++ = *s++;
	*d = 0;
	return d - p;
}


std::string
strdup(const ucs2_t* s, Encoding enc)
{
	yconstraint(s);

	std::string
		str((ntctslen(s) * sizeof(ucsint_t) / sizeof(ucs2_t)) + 1U, char());

	UCS2ToMBCS(&str[0], s, enc);
	return str;
}

std::basic_string<ucs2_t>
ucsdup(const char* s, Encoding enc)
{
	yconstraint(s);

	std::basic_string<ucs2_t> str(ntctslen(s) + 1U, ucs2_t());

	MBCSToUCS2(&str[0], s, enc);
	return str;
}
std::basic_string<ucs2_t>
ucsdup(const ucs2_t* s)
{
	yconstraint(s);

	const size_t n(ntctslen(s) + 1U);
	std::basic_string<ucs2_t> str(n, ucs2_t());

	std::copy_n(s, n, &str[0]);
	return str;
}
std::basic_string<ucs2_t>
ucsdup(const ucs4_t* s)
{
	yconstraint(s);

	std::basic_string<ucs2_t> str(ntctslen(s) + 1U, ucs2_t());

	UCS4ToUCS2(&str[0], s);
	return str;
}

} // namespace CHRLib;

