/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file chrproc.cpp
\ingroup CHRLib
\brief 字符编码处理。
\version r2054;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-17 17:53:21 +0800;
\par 修改时间:
	2011-12-29 19:16 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	CHRLib::CharacterProcessing;
*/


#include "CHRLib/chrproc.h"
#include "CHRLib/smap.hpp"
#include <cctype>
#include <cstdlib>
#include <cwchar>
#include <ystdex/cstdio.h>
#include <ystdex/cstring.h>
#include <ystdex/iterator.hpp>

CHRLIB_BEGIN

using std::size_t;
using std::tolower;
using std::malloc;
using std::strlen;
using std::memcpy;
using ystdex::sntctslen;
using ystdex::input_monomorphic_iterator;


ConversionResult
MBCToUC(ucs2_t& uc, const char*& c, const Encoding& cp, ConversionState&& st)
{
	if(const auto pfun = FetchMapperPtr<ConversionResult(ucs2_t&,
		input_monomorphic_iterator&&, ConversionState&&)>(cp))
		return pfun(uc, c, std::move(st));
	return ConversionResult::Unhandled;
}
ConversionResult
MBCToUC(ucs2_t& uc, std::FILE* fp, const Encoding& cp, ConversionState&& st)
{
	if(const auto pfun = FetchMapperPtr<ConversionResult(ucs2_t&,
		input_monomorphic_iterator&&, ConversionState&&)>(cp))
	{
		ystdex::ifile_iterator i(*fp);
		const auto r(pfun(uc, input_monomorphic_iterator(i), std::move(st)));

		if(is_dereferencable(i))
			std::ungetc(*i, fp);
		return r;
	}
	return ConversionResult::Unhandled;
}
ConversionResult
MBCToUC(const char*& c, const Encoding& cp, ConversionState&& st)
{
	if(const auto pfun = FetchMapperPtr<ConversionResult(
		input_monomorphic_iterator&&, ConversionState&&)>(cp))
		return pfun(c, std::move(st));
	return ConversionResult::Unhandled;
}
ConversionResult
MBCToUC(std::FILE* fp, const Encoding& cp, ConversionState&& st)
{
	if(const auto pfun = FetchMapperPtr<ConversionResult(
		input_monomorphic_iterator&&, ConversionState&&)>(cp))
	{
		ystdex::ifile_iterator i(*fp);
		auto r(pfun(input_monomorphic_iterator(i), std::move(st)));

		if(is_dereferencable(i))
			std::ungetc(*i, fp);
		return r;
	}
	return ConversionResult::Unhandled;
}

byte
UCToMBC(char* d, const ucs2_t& s, const Encoding& cp)
{
	byte l(0);
	const auto pfun(FetchMapperPtr<byte(char*, const ucs2_t&)>(cp));

	if(pfun)
		l = pfun(d, s);
	return l;
}


size_t
MBCSToUCS2(ucs2_t* d, const char* s, const Encoding& cp)
{
	ucs2_t* const p(d);

	while(*s)
		MBCToUC(*d++, s, cp);
	*d = 0;
	return d - p;
}

size_t
UCS2ToMBCS(char* d, const ucs2_t* s, const Encoding& cp)
{
	char* const p(d);

	while(*s)
		d += UCToMBC(d, *s++, cp);
	*d = 0;
	return d - p;
}

size_t
UCS4ToUCS2(ucs2_t* d, const ucs4_t* s)
{
	ucs2_t* const p(d);

	while(*s)
		*d++ = *s++;
	*d = 0;
	return d - p;
}


ucs2_t*
ucsdup(const char* s, const Encoding& cp)
{
	ucs2_t* const p(static_cast<ucs2_t*>(malloc((strlen(s) + 1) << 1)));

	if(p)
		MBCSToUCS2(p, s, cp);
	return p;
}
ucs2_t*
ucsdup(const ucs2_t* str)
{
	const size_t n(sntctslen(str) * sizeof(ucs2_t));
	ucs2_t* const p(static_cast<ucs2_t*>(malloc(n + sizeof(ucs2_t))));

	if(p)
		memcpy(p, str, n);
	return p;
}
ucs2_t*
ucsdup(const ucs4_t* s)
{
	ucs2_t* const p(static_cast<ucs2_t*>(malloc((sntctslen(s) + 1)
		* sizeof(ucs2_t))));

	if(p)
		UCS4ToUCS2(p, s);
	return p;
}

CHRLIB_END

