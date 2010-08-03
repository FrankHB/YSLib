// CHRLib -> CharacterProcessing by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-17 17:53:21;
// UTime = 2010-8-2 13:58;
// Version = 0.1582;


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

/*
char* U2A(char* Ui)
{
	ubyte_t t;
	char* R(Ui);
	const char S[]("0123456789ABCDEF");

	for(t(0); t < 4; ++t)
	{
		U[t << 1] = S[(*R & 0xF0) >> 4];
		U[(t << 1) + 1] = S[*R & 0xF];
		++R;
		//++t;
	}
	U[(t + 1) << 1] = 0;
	return U;
}
*/

usize_t
ucslen(const uchar_t* s)
{
	const uchar_t* p(s);

	while(*p)
		++p;
	return p - s;
}

uchardiff_t
ucscmp(const uchar_t* s1, const uchar_t* s2)
{
	uchardiff_t d(0);

	while(!(d = *s1 - *s2))
	{
		++s1;
		++s2;
	}
	return d;
}

uchardiff_t
ucsicmp(const uchar_t* s1, const uchar_t* s2)
{
	uchardiff_t d(0);

	while(!(d = tolower(*s1) - tolower(*s2)))
	{
		++s1;
		++s2;
	}
	return d;
}

ubyte_t
ToUTF(const char* chr, uchar_t& uchr, const CSID& cp)
{
	ubyte_t len(2);

	switch(cp)
	{
	case CharSet::SHIFT_JIS:
		uchr = codemap_17(len, chr);
		break;
	case CharSet::UTF_8:
		uchr = codemap_106(len, chr);
		break;
	case CharSet::GBK:
		uchr = codemap_113(len, chr);
		break;
	case CharSet::UTF_16BE:
		uchr = codemap_1013(len, chr);
		break;
	case CharSet::UTF_16LE:
		uchr = codemap_1014(len, chr);
		break;
	case CharSet::Big5:
		uchr = codemap_2026(len, chr);
		break;
	default:
		break;
	}
	return len;
}
ubyte_t
ToUTF(FILE* fp, uchar_t& uchr, const CSID& cp)
{
	ubyte_t len(2);

	switch(cp)
	{
	case CharSet::SHIFT_JIS:
		uchr = codemap_17(len, fp);
		break;
	case CharSet::UTF_8:
		uchr = codemap_106(len, fp);
		break;
	case CharSet::GBK:
		uchr = codemap_113(len, fp);
		break;
	case CharSet::UTF_16BE:
		uchr = codemap_1013(len, fp);
		break;
	case CharSet::UTF_16LE:
		uchr = codemap_1014(len, fp);
		break;
	case CharSet::Big5:
		uchr = codemap_2026(len, fp);
		break;
	default:
		break;
	}
	return feof(fp) ? 0 : len;
}

template<typename _charType>
static inline usize_t
StrToANSI(char* d, const _charType* s, char c = ' ')
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
UCS4ToANSI(char* d, const wchar_t* s, char c)
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
MBCSToUCS(wchar_t* d, const char* s, const CSID& cp)
{
	wchar_t* const p(d);
	uchar_t t;

	while(*s)
	{
		s += ToUTF(s, t, cp);
		*d++ = t;
	}
	*d = 0;
	return d - p;
}
usize_t
UTF16LEToMBCS(char* d, const uchar_t* s, const CSID& cp)
{
	// Not implemented.
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
UCS4ToUCS2(uchar_t* d, const wchar_t* s)
{
	uchar_t* const p(d);

	while(*s)
		*d++ = *s++;
	*d = 0;
	return d - p;
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
sdup(const wchar_t* s, char c)
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
ucsdup(const wchar_t* s)
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
{
}
*/

CHRLIB_END

