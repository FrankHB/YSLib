// CHRLib -> CharacterMapping by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-17 17:52:35;
// UTime = 2010-6-14 18:57;
// Version = 0.1528;


#ifndef INCLUDED_CHRMAP_H_
#define INCLUDED_CHRMAP_H_

#include "chrdef.h"
#include <cstdio>

// CharacterMapping ：字符映射。

CHRLIB_BEGIN

CHRLIB_BEGIN_NAMESPACE(CharSet)

//编码字符集：使用 IANA 官方字符集名称和 MIBenum ，详见 http://www.iana.org/assignments/character-sets 。
typedef enum
{
	//保留。
	Null = 0, //空字符集。
	Reserved_1 = 1,
	Reserved_2 = 2,

	US_ASCII = 3,
	SHIFT_JIS = 17,
	KS_C_5601_1987 = 36,
	EUC_KR = 38,
//	ASMO_449 = 65,
	UTF_8 = 106,
	GBK = 113,
	GB18030 = 114,

	ISO_10646_UCS_2 = 1000,
	ISO_10646_UCS_4 = 1001,
	ISO_10646_UCS_Basic = 1002,

	UTF_7 = 1012,
	UTF_16BE = 1013,
	UTF_16LE = 1014,
	UTF_16 = 1015,
	UTF_32 = 1017,
	UTF_32BE = 1018,
	UTF_32LE = 1019,

	GB2312 = 2025, //仅 EUC-CN ，GB2312 最常用实现。
	Big5 = 2026,

	//其它。
	/*
	HZ_GB_2312 = 52396, // Chinese Simplified (HZ)，GB2312 的另一种实现。
	MIK, // Supports Bulgarian and Russian as well
	*/
} CSID; //字符集标识。

//别名。
const CSID
	iso_ir_6(US_ASCII), ISO646_US(US_ASCII), ASCII(US_ASCII), us(US_ASCII), IBM367(US_ASCII), cp367(US_ASCII), csASCII(US_ASCII),
	MS_Kanji(SHIFT_JIS), csShiftJIS(SHIFT_JIS), windows_932_(SHIFT_JIS),
	iso_ir_149(KS_C_5601_1987), KS_C_5601_1989(KS_C_5601_1987), KSC_5601(KS_C_5601_1987), korean(KS_C_5601_1987), csKSC56011987(KS_C_5601_1987), windows_949_(KS_C_5601_1987),
	windows_65001_(UTF_8),
	CP936(GBK), MS936(GBK), windows_936(GBK),
	windows_54936_(GB18030),

	csUnicode(ISO_10646_UCS_2),
	csUCS4(ISO_10646_UCS_4),
	csUnicodeASCII(ISO_10646_UCS_Basic),
	windows_1201_(UTF_16BE),
	windows_1200_(UTF_16LE),

	csGB2312(GB2312), EUC_CN_(GB2312), windows_51936_(GB2312),
	windows_950_(Big5);

CHRLIB_END_NAMESPACE(CharSet)

using CharSet::CSID;


//默认字符集。
const CSID CS_Default = CharSet::UTF_8;
const CSID CS_Local = CharSet::GBK;


//编码转换表。
const ubyte_t cp17[] = {0};
extern "C"
{
	extern const ubyte_t cp113[];
	//extern const ubyte_t cp2026[13658];
}
const ubyte_t cp2026[] = {0};


//取小端序双字节字符。
inline uchar_t
getword_LE(const char* c)
{
	return (c[1] << 8) | *c;
}
//取大端序双字节字符。
inline uchar_t
getword_BE(const char* c)
{
	return (*c << 8) | c[1];
}


//typedef uchar_t (*CMF)(ubyte_t&, const char*);

// Shift_JIS / 日文。
uchar_t
codemap_17(ubyte_t&, const char*);
uchar_t
codemap_17(ubyte_t&, std::FILE*);

// UTF-8 / Unicode。
uchar_t
codemap_106(ubyte_t&, const char*);
uchar_t
codemap_106(ubyte_t&, FILE*);

// GBK / 简体中文。
inline uchar_t
codemap_113(ubyte_t& l, const char* c)
{
	return cp113[(ubyte_t)*c] ? (l = 1, *c) : cp113[getword_BE(c) + 0x0080];
}
inline uchar_t
codemap_113(ubyte_t& l, FILE* fp)
{
	char c;
	return cp113[(ubyte_t)(c = getc(fp))] ? (l = 1, c) : cp113[(c << 8 | getc(fp)) + 0x0080];
}

// UCS-2_UTF-16BE / Unicode 双字节字符 - 大端序。
inline uchar_t
codemap_1013(ubyte_t& l, const char* c)
{
	//l = 2;
	return getword_BE(c);
}
inline uchar_t
codemap_1013(ubyte_t& l, FILE* fp)
{
	//l = 2;
	return getc(fp) << 8 | getc(fp);
}

// UCS-2_UTF-16LE / Unicode 双字节字符 - 小端序。
inline uchar_t
codemap_1014(ubyte_t& l, const char* c)
{
	//l = 2;
	return getword_LE(c);
}
inline uchar_t
codemap_1014(ubyte_t& l, FILE* fp)
{
	//l = 2;
	return getc(fp) | getc(fp) << 8;
}

// Big5 / 繁体中文。
uchar_t
codemap_2026(ubyte_t&, const char*);
uchar_t
codemap_2026(ubyte_t&, FILE*);

CHRLIB_END

#endif

