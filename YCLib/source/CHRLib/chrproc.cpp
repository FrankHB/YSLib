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
\version r1969;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-17 17:53:21 +0800;
\par 修改时间:
	2011-10-06 05:02 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	CHRLib::CharacterProcessing;
*/


#include "chrproc.h"
#include "smap.hpp"
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

namespace
{
	template<Encoding, typename _tDst, typename _tSrc, typename _tState>
	yconstexprf ubyte_t
	UCS2Mapper_Map(_tDst, _tSrc, _tState)
	{
		return 0;
	}
	template<Encoding cp, typename _tSrc, typename _tState>
	yconstexprf ubyte_t
	UCS2Mapper_Map(ucs2_t& d, _tSrc&& s, _tState&& st,
		decltype(&GUCS2Mapper<cp>::template Map<_tSrc, _tState>) = nullptr)
	{
		return GUCS2Mapper<cp>::Map(d, s, yforward(st));
	}

	template<Encoding cp, typename _tDst, typename _tSrc>
	yconstexprf ubyte_t
	UCS2Mapper_InverseMap(_tDst, _tSrc)
	{
		return 0;
	}
	template<Encoding cp, typename _tDst>
	yconstexprf ubyte_t
	UCS2Mapper_InverseMap(_tDst d, const ucs2_t& s,
		decltype(&GUCS2Mapper<cp>::template InverseMap<_tDst>) = nullptr)
	{
		return GUCS2Mapper<cp>::InverseMap(d, s);
	}


	template<Encoding cp>
	yconstexpr ubyte_t
	UCS2Mapper(ucs2_t& uc, input_monomorphic_iterator&& i, ConversionState&& st)
	{
		return UCS2Mapper_Map<cp>(uc, i, std::move(st));
	}
	template<Encoding cp>
	ubyte_t
	UCS2Mapper(char* d, const ucs2_t& s)
	{
		assert(d);

		return UCS2Mapper_InverseMap<cp>(d, s);
	}

	template<typename _fCodemapTransform>
	_fCodemapTransform*
	FetchMapperPtr(const Encoding& cp)
	{
		using namespace CharSet;

#define CHR_MapItem(cp) \
	case cp: \
		return UCS2Mapper<cp>;

		switch(cp)
		{
		CHR_MapItem(SHIFT_JIS)
		CHR_MapItem(UTF_8)
		CHR_MapItem(GBK)
		CHR_MapItem(UTF_16BE)
		CHR_MapItem(UTF_16LE)
		CHR_MapItem(Big5)
		default:
			break;
		}

#undef CHR_MapItem

		return nullptr;
	}
}

ubyte_t
MBCToUC(ucs2_t& uc, const char*& c, const Encoding& cp, ConversionState&& st)
{
	const auto pfun(FetchMapperPtr<ubyte_t(ucs2_t&,
		input_monomorphic_iterator&&, ConversionState&&)>(cp));
	ubyte_t l(0);

	if(pfun)
		l = pfun(uc, c, std::move(st));
	return l;
}
ubyte_t
MBCToUC(ucs2_t& uc, std::FILE* fp, const Encoding& cp, ConversionState&& st)
{
	const auto pfun(FetchMapperPtr<ubyte_t(ucs2_t&,
		input_monomorphic_iterator&&, ConversionState&&)>(cp));
	ubyte_t l(0);

	if(pfun)
	{
		ystdex::ifile_iterator i(*fp);

		l = pfun(uc, input_monomorphic_iterator(++i), std::move(st));
		if(!std::feof(fp))
		{
			std::ungetc(*i, fp);
			if(GetCountOf(st) > 0)
				--GetCountOf(st);
		}
	}
	return l;
}

ubyte_t
UCToMBC(char* d, const ucs2_t& s, const Encoding& cp)
{
	ubyte_t l(0);
	const auto pfun(FetchMapperPtr<ubyte_t(char*, const ucs2_t&)>(cp));

	if(pfun)
		l = pfun(d, s);
	return l;
}


usize_t
MBCSToUCS2(ucs2_t* d, const char* s, const Encoding& cp)
{
	ucs2_t* const p(d);

	while(*s)
		MBCToUC(*d++, s, cp);
	*d = 0;
	return d - p;
}

usize_t
UCS2ToMBCS(char* d, const ucs2_t* s, const Encoding& cp)
{
	char* const p(d);

	while(*s)
		d += UCToMBC(d, *s++, cp);
	*d = 0;
	return d - p;
}

usize_t
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

