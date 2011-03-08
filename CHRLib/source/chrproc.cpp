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
\version 0.1729;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-17 17:53:21 +0800; 
\par 修改时间:
	2011-03-05 17:05 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	CHRLib::CharacterProcessing;
*/


#include "chrproc.h"
#include "chrlib.h"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cwchar>

CHRLIB_BEGIN

using std::size_t;
using std::tolower;
using std::malloc;
using std::strlen;
using std::memcpy;

usize_t
ucslen(const uchar_t* s)
{
	const uchar_t* p(s);

	while(*p)
		++p;
	return p - s;
}

uint_t
ucscmp(const uchar_t* s1, const uchar_t* s2)
{
	uint_t d(0);

	while(!(d = *s1 - *s2))
	{
		++s1;
		++s2;
	}
	return d;
}

uint_t
ucsicmp(const uchar_t* s1, const uchar_t* s2)
{
	uint_t d(0);

	while(!(d = tolower(*s1) - tolower(*s2)))
	{
		++s1;
		++s2;
	}
	return d;
}

namespace
{
	template<typename _fCodemapTransform>
	_fCodemapTransform*
	GetCodeMapFuncPtr(const CSID& cp)
	{
		using namespace CharSet;
		_fCodemapTransform* pfun(NULL);

		switch(cp)
		{
		case SHIFT_JIS:
			pfun = &codemap<SHIFT_JIS>;
			break;
		case UTF_8:
			pfun = &codemap<UTF_8>;
			break;
		case GBK:
			pfun = &codemap<GBK>;
			break;
		case UTF_16BE:
			pfun = &codemap<UTF_16BE>;
			break;
		case UTF_16LE:
			pfun = &codemap<UTF_16LE>;
			break;
		case Big5:
			pfun = &codemap<Big5>;
			break;
		default:
			break;
		}
		return pfun;
	}
}

ubyte_t
ToUTF(const char* chr, uchar_t& uchr, const CSID& cp)
{
	ubyte_t len(2);
	CMF* pfun(GetCodeMapFuncPtr<CMF>(cp));

	if(pfun)
		uchr = pfun(len, chr);
	return len;
}
ubyte_t
ToUTF(FILE* fp, uchar_t& uchr, const CSID& cp)
{
	ubyte_t len(2);
	CMF_File* pfun(GetCodeMapFuncPtr<CMF_File>(cp));

	if(pfun)
		uchr = pfun(len, fp);
	return feof(fp) ? 0 : len;
}

namespace
{
	template<typename _tChar>
	usize_t
	StrToANSI(char* d, const _tChar* s, char c = ' ')
	{
		char* const p(d);

		if(c)
			while(*s)
			{
				*d++ = IsASCII(*s) ? *s : c;
				++s;
			}
		else
			while(*s)
				*d++ = ToASCII(*s++);
		*d = 0;
		return d - p;
	}
}

usize_t
MBCSToANSI(char* d, const char* s, char c)
{
	return StrToANSI(d, s, c);
}

usize_t
UCS2ToANSI(char* d, const uchar_t* s, char c)
{
	return StrToANSI(d, s, c);
}

usize_t
UCS4ToANSI(char* d, const fchar_t* s, char c)
{
	return StrToANSI(d, s, c);
}

usize_t
MBCSToUTF16LE(uchar_t* d, const char* s, const CSID& cp)
{
	uchar_t* const p(d);

	while(*s)
		s += ToUTF(s, *d++, cp);
	*d = 0;
	return d - p;
}
usize_t
MBCSToUCS(fchar_t* d, const char* s, const CSID& cp)
{
	fchar_t* const p(d);
	uchar_t t(0);

	while(*s)
	{
		s += ToUTF(s, t, cp);
		*d++ = t;
	}
	*d = 0;
	return d - p;
}
usize_t
UTF16LEToMBCS(char* d, const uchar_t* s, const CSID& /*cp*/)
{
	// TODO: impl;
	char* const p(d);
//	char t;

	while(*s)
	{
	//	d += ToMBCS(*s++, d, cp);
		*d++ = *s++ & 0x7F;
	}
	*d = 0;
	return d - p;
}
usize_t
UCS4ToUCS2(uchar_t* d, const fchar_t* s)
{
	uchar_t* const p(d);

	while(*s)
		*d++ = *s++;
	*d = 0;
	return d - p;
}

namespace
{
	std::size_t
	wcslen(const fchar_t* s)
	{
		return std::wcslen(reinterpret_cast<const wchar_t*>(s));
	}
}

char*
sdup(const char* s, char c)
{
	char* p(static_cast<char*>(malloc((strlen(s) + 1))));

	if(p)
		MBCSToANSI(p, s, c);
	return p;
}
char*
sdup(const uchar_t* s, char c)
{
	char* p(static_cast<char*>(malloc(((ucslen(s) >> 1) + 1))));

	if(p)
		UCS2ToANSI(p, s, c);
	return p;
}
char*
sdup(const fchar_t* s, char c)
{
	char* p(static_cast<char*>(malloc(((wcslen(s) >> 2) + 1))));

	if(p)
		UCS4ToANSI(p, s, c);
	return p;
}

uchar_t*
ucsdup(const char* s, const CSID& cp)
{
	uchar_t* p(static_cast<uchar_t*>(malloc((strlen(s) + 1) << 1)));

	if(p)
		MBCSToUTF16LE(p, s, cp);
	return p;
}
uchar_t*
ucsdup(const uchar_t* s)
{
	const size_t n(ucslen(s) * sizeof(uchar_t));
	uchar_t* p(static_cast<uchar_t*>(malloc(n + 1)));

	if(p)
		memcpy(p, s, n);
	return p;
}
uchar_t*
ucsdup(const fchar_t* s)
{
	uchar_t* p(static_cast<uchar_t*>(malloc((wcslen(s) + 1) << 1)));

	if(p)
		UCS4ToUCS2(p, s);
	return p;
}

/*
unsigned short
WordWidth(char* word, CHRSTAT *cStat, CSID cp)
{
	uchar_t* ptrW(static_cast<uchar_t*>(word));
	ubyte_t* ptr(static_cast<ubyte_t*>(word));
	unsigned short Width(0);

	switch(cp)
	{
	case UCS2_LE :
		while(*ptrW)
		{
			if(*ptrW < 128)
				Width += (cStat->Font->sWidth + cStat->WSpc);
			else
				Width += (cStat->Font->Width + cStat->WSpc);
			ptrW++;
		}
		break;
	case UTF8 :
		while(*ptr)
		{
			if(*ptr < 128)
			{
				Width += (cStat->Font->sWidth + cStat->WSpc);
				ptr++;
			}
			else
			{
				if(*ptr > 0xE0)
				{
					Width += (cStat->Font->Width + cStat->WSpc);
					ptr += 3;
				}
				else
				{
					Width += (cStat->Font->Width + cStat->WSpc);
					ptr += 2;
				}
			}
		}
		break;
	case Big5 :
	case GBK :
	case JIS :
		while(*ptr)
		{
			if(*ptr < 128)
			{
				Width += (cStat->Font->sWidth + cStat->WSpc);
				++ptr;
			}
			else
			{
				Width += (cStat->Font->Width + cStat->WSpc);
				ptr += 2;
			}
		}
		break;
	}
	return Width;
}

usize_t
UTF16toUTF8(ubyte_t* Utf, const u16* Uni)
{
	usize_t l(0); //len
	usize_t i(0);

	while(Uni[l])
	{
		if((Uni[l] > 0) && (Uni[l] <= 0x7F))
		{
			Utf[i++] =  Uni[l++];
		}
		else if((Uni[l] > 0x7F) && (Uni[l] <= 0x7FF))
		{
			Utf[i++] =  (Uni[l  ] >> 6 )         | 0xC0;
			Utf[i++] =  (Uni[l++]       & 0x3F)  | 0x80;
		}
		else
		{
			Utf[i++] =  (Uni[l  ] >> 12)         | 0xE0;
			Utf[i++] = ((Uni[l  ] >> 6 ) & 0x3F) | 0x80;
			Utf[i++] = ( Uni[l++]       & 0x3F)  | 0x80;
		}
	}
	Utf[i] = 0;
	Utf[i + 1] = 0;
	return l;
}

usize_t
UTF8toUTF16(uchar_t* U, const char* U8)
{
	usize_t i(0), l(0);

	while(U8[i])
		i += ToUTF(static_cast<char*>(&U8[i]), &U[l++], UTF8);
	U[l] = 0;
	return l;
}

ubyte_t STR[16];

bool
strComp(char* tgr, char* s)
{
	u16 tl(0); //len(tgr);
	u16 sl(0); //len(s);
	u16 t;
	if(tl == sl)
	{
		for(t = 0;t < tl; ++t)
			if(tgr[t] != s[t])
				return false;
	}
	else
		return false;
	return true;
}

ubyte_t*
ValueStr(unsigned long v)
{
	ubyte_t t(15);
	STR[t] = 0;
	while(v)
	{
		STR[--t] = (v % 10) + 48;
		v /= 10;
	}
	return &STR[t];
}

usize_t
ustrlen(char* s)
{
	usize_t l(0); //length
	ubyte_t b(1); //below
	while(*s && b)
	{
		++l;
		++s;
		if(l == 0x10000)
			b = 0;
	}
	return l;
}
ubyte_t
IsFullChar(char* s)
////
{}
*/

CHRLIB_END

