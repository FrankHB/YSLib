// CHRLib -> CharacterMapping by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-17 17:53:21;
// UTime = 2010-6-14 18:48;
// Version = 0.1398;


#include "chrmap.h"

CHRLIB_BEGIN

uchar_t
codemap_17(ubyte_t& l, const char* c)
{
	unsigned short row(0), col(0), ln(188); // (7E-40 + 1 + FC-80 + 1)

	//l = 2;
	if((*c >= 0xA1) && (*c <= 0xC6))
	{
		row = *c - 0xA1 ;
		if(c[1] >= 0x40 && c[1] <= 0x7E)
			col = c[1] - 0x40 ;
		else if(c[1] >= 0xA1 && c[1] <= 0xFE)
			col = c[1] - 0x62;
		return cp17[row * ln + col];
	}
	else if(*c >= 0xC9 && *c <= 0xF9)
	{
		row = *c - 0xA3 ;
		if(c[1] >= 0x40 && c[1] <= 0x7E)
			col = c[1] - 0x40;
		else if(c[1] >= 0xA1 && c[1] <= 0xFE)
			col = c[1] - 0x62;
		return cp17[row * ln + col];
	}
	else if(*c < 0x80)
	{
		l = 1;
		return *c;
	}
	else
		return 0xFFFE;
}

uchar_t
codemap_17(ubyte_t& l, FILE* fp)
{
	unsigned short row(0), col(0), ln(188); // (7E-40 + 1 + FC-80 + 1)
	char c(getc(fp));

	//l = 2;
	if((c >= 0xA1) && (c <= 0xC6))
	{
		char d(getc(fp));

		row = c - 0xA1 ;
		if(d >= 0x40 && d <= 0x7E)
			col = d - 0x40 ;
		else if(d >= 0xA1 && d <= 0xFE)
			col = d - 0x62;
		return cp17[row * ln + col];
	}
	else if(c >= 0xC9 && c <= 0xF9)
	{
		char d(getc(fp));

		row = c - 0xA3;
		if(d >= 0x40 && d <= 0x7E)
			col = d - 0x40 ;
		else if(d >= 0xA1 && d <= 0xFE)
			col = d - 0x62;
		return cp17[row * ln + col];
	}
	else if(c < 0x80)
	{
		l = 1;
		return c;
	}
	else
		return 0xFFFE;
}

uchar_t
codemap_106(ubyte_t& l, const char* c)
{
	const char c0(*c);

	if(c0 < 0x80)
	{
		l = 1;
		return c0;
	}
	else if(c0 & 0x20)
	{
		l = 3;
		return (((c0 & 0xF) << 4 | (c[1] & 0x3C) >> 2) << 8) | ((c[1] & 0x3) << 6) | (c[2] & 0x3F);
	}
	else
	{
	//	l = 2;
		return ((c0 & 0x1C) >> 2 << 8) | ((c0 & 0x3) << 6) | (c[1] & 0x3F);
	}
	/*
	else if(c0 >= 0xC2 && c0 <= 0xDF)
	{
		if(c0 == '\0' || c[1] == '\0')
		{
			l = 0;
			return 0xFFFE;
		}
	//	l = 2;
		return ((c0 - 192) << 6) | (c[1] - 128);
	}
	else if(c0 >= 0xE0 && c0 <= 0xEF)
	{
		if(c0 == '\0' || c[1] == '\0' || c[2] == '\0')
		{
			l = 0;
			return 0;
		}
		l = 3;
		return ((c0 - 224) << 12) | ((c[1] - 128) << 6) | (c[2] - 128);
	}
	else if(c0 >= 0xF0 && c0 <= 0xF4)
	{
		if(c0 == '\0' || c[1] == '\0' || c[2] == '\0' || c[3] == '\0')
		{
			l = 0;
			return 0;
		}
		l = 4;
		return ((c0 - 240) << 18) | ((c[1] - 128) << 12) | ((c[2] - 128) << 6) | (c[3] - 128);
	}
	*/
}

uchar_t
codemap_106(ubyte_t& l, FILE* fp)
{
	char c(getc(fp));

	if(c < 0x80)
	{
		l = 1;
		return c;
	}
	else
	{
		char d(getc(fp));

		if(c & 0x20)
		{
			l = 3;
			return (((c & 0x0F) << 4 | (d & 0x3C) >> 2) << 8) | ((d & 0x3) << 6) | (getc(fp) & 0x3F);
		}
		else
		{
			//l = 2;
			return ((c & 0x1C) >> 2 << 8) | ((c & 0x03) << 6) | (d & 0x3F);
		}
	}
}

/*
uchar_t
codemap_113(ubyte_t& l, const char* c)
{
	return cp113[static_cast<uchar_t>(*c)] ? (l = 1 ,*c) : (static_cast<uchar_t*>cp113)[getword_BE(c) + 0x0080];
}

uchar_t
codemap_113(ubyte_t& l, FILE* fp)
{
	char c;
	return cp113[static_cast<uchar_t>(c = getc(fp))] ? (l = 1, c) : (static_cast<uchar_t*>cp113)[(c << 8 | getc(fp)) + 0x0080];
}

uchar_t
codemap_1013(ubyte_t& l, const char* c)
{
	//l = 2;
	return getword_BE(c);
}

uchar_t
codemap_1013(ubyte_t& l, FILE* fp)
{
	//l = 2;
	return getc() << 8 | getc();
}

uchar_t
codemap_1014(ubyte_t& l, const char* c)
{
	//l = 2;
	return getword_LE(c);
}

uchar_t
codemap_1014(ubyte_t& l, FILE* fp)
{
	//l = 2;
	return getc() | getc() << 8;
}
*/

uchar_t
codemap_2026(ubyte_t& l, const char* c)
{
	unsigned short row(0), col(0), ln(157); // (7E-40 + FE-A1)

	//l = 2;
	if(*c >= 0xA1 && *c <= 0xC6)
	{
		row = *c - 0xA1;
		if(c[1] >= 0x40 && c[1] <= 0x7E)
			col = c[1] - 0x40;
		else if(c[1] >= 0xA1 && c[1] <= 0xFE)
			col = c[1] - 0x62;
		return cp2026[row * ln + col];
	}
	else if(*c >= 0xC9 && *c <= 0xF9)
	{
		row = *c - 0xA3;
		if(c[1] >= 0x40 && c[1] <= 0x7E)
			col = c[1] - 0x40;
		else if(c[1] >= 0xA1 && c[1] <= 0xFE)
			col = c[1] - 0x62;
		return cp2026[row * ln + col];
	}
	else if(*c < 0x80)
	{
		l = 1;
		return *c;
	}
	else
		return 0xFFFE;
}

uchar_t
codemap_2026(ubyte_t& l, FILE* fp)
{
	unsigned short row(0), col(0), ln(157); // (7E-40 + FE-A1)
	char c(getc(fp));

	//l = 2;
	if(c >= 0xA1 && c <= 0xC6)
	{
		char d(getc(fp));

		row = c - 0xA1;
		if(d >= 0x40 && d <= 0x7E)
			col = d - 0x40;
		else if(d >= 0xA1 && d <= 0xFE)
			col = d - 0x62;
		return cp2026[row * ln + col];
	}
	else if(c >= 0xC9 && c <= 0xF9)
	{
		char d(getc(fp));

		row = c - 0xA3;
		if(d >= 0x40 && d <= 0x7E)
			col = c - 0x40;
		else if(d >= 0xA1 && d <= 0xFE)
			col = d - 0x62;
		return cp2026[row * ln + col];
	}
	else if(c < 0x80)
	{
		l = 1;
		return c;
	}
	else
		return 0xFFFE;
}

CHRLIB_END

