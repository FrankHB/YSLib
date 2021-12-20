/*
	© 2009-2013, 2015, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Encoding.h
\ingroup CHRLib
\brief 字符编码定义。
\version r733
\author FrankHB <frankhb1989@gmail.com>
\since build 242
\par 创建时间:
	2009-11-17 17:52:35 +0800
\par 修改时间:
	2021-12-20 22:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::Encoding
*/


#ifndef INC_CHRLib_encoding_h_
#define INC_CHRLib_encoding_h_ 1

#include "YModules.h"
#include YFM_CHRLib_CharacterDefinition
#include <ystdex/bit.hpp> // for ystdex::byte_order;

namespace CHRLib
{

//! \since build 594
using ByteOrder = ystdex::byte_order;

namespace CharSet
{

/*!
\brief 字符流编码标识。
\note MIB(management information base) 是
	SNMP(simple network management protocal ，简单网络管理协议) 和
	OSI/ISO 网络管理模型上下文中的虚拟数据库。
\note 由于历史原因， CharSet（字符集）和相关概念在此作为文本编码的同义词，
	但不是确切的用法。
\see http://www.iana.org/assignments/character-sets 。
\see http://www.iana.org/assignments/ianacharset-mib 。
\see http://en.wikipedia.org/wiki/Management_information_base 。
\since build 416

编码字符集：使用 IANA 官方字符集名称和 MIBenum（MIB 枚举）。
*/
enum Encoding
{
	//! \note 保留。
	//@{
	//! \brief 空字符集。
	Null = 0,
	//! \since build 594
	other = 1,
	//! \since build 594
	unknown = 2,
	//@}
	csASCII = 3,
	csISOLatin1 = 4,
	csISOLatin2 = 5,
	csISOLatin3 = 6,
	csISOLatin4 = 7,
	csISOLatinCyrillic = 8,
	csISOLatinArabic = 9,
	csISOLatinGreek = 10,
	csISOLatinHebrew = 11,
	csISOLatin5 = 12,
	csISOLatin6 = 13,
	csISOTextComm = 14,
	csHalfWidthKatakana = 15,
	csJISEncoding = 16,
	csShiftJIS = 17,
	csEUCPkdFmtJapanese = 18,
	csEUCFixWidJapanese = 19,
	csISO4UnitedKingdom = 20,
	csISO11SwedishForNames = 21,
	csISO15Italian = 22,
	csISO17Spanish = 23,
	csISO21German = 24,
	csISO60DanishNorwegian = 25,
	csISO69French = 26,
	csISO10646UTF1 = 27,
	csISO646basic1983 = 28,
	csINVARIANT = 29,
	csISO2IntlRefVersion = 30,
	csNATSSEFI = 31,
	csNATSSEFIADD = 32,
	csNATSDANO = 33,
	csNATSDANOADD = 34,
	csISO10Swedish = 35,
	csKSC56011987 = 36,
	csISO2022KR = 37,
	csEUCKR = 38,
	csISO2022JP = 39,
	csISO2022JP2 = 40,
	csISO13JISC6220jp = 41,
	csISO14JISC6220ro = 42,
	csISO16Portuguese = 43,
	csISO18Greek7Old = 44,
	csISO19LatinGreek = 45,
	csISO25French = 46,
	csISO27LatinGreek1 = 47,
	csISO5427Cyrillic = 48,
	csISO42JISC62261978 = 49,
	csISO47BSViewdata = 50,
	csISO49INIS = 51,
	csISO50INIS8 = 52,
	csISO51INISCyrillic = 53,
	csISO54271981 = 54,
	csISO5428Greek = 55,
	csISO57GB1988 = 56,
	csISO58GB231280 = 57,
	csISO61Norwegian2 = 58,
	csISO70VideotexSupp1 = 59,
	csISO84Portuguese2 = 60,
	csISO85Spanish2 = 61,
	csISO86Hungarian = 62,
	csISO87JISX0208 = 63,
	csISO88Greek7 = 64,
	csISO89ASMO449 = 65,
	csISO90 = 66,
	csISO91JISC62291984a = 67,
	csISO92JISC62991984b = 68,
	csISO93JIS62291984badd = 69,
	csISO94JIS62291984hand = 70,
	csISO95JIS62291984handadd = 71,
	csISO96JISC62291984kana = 72,
	csISO2033 = 73,
	csISO99NAPLPS = 74,
	csISO102T617bit = 75,
	csISO103T618bit = 76,
	csISO111ECMACyrillic = 77,
	csa71 = 78,
	csa72 = 79,
	csISO123CSAZ24341985gr = 80,
	csISO88596E = 81,
	csISO88596I = 82,
	csISO128T101G2 = 83,
	csISO88598E = 84,
	csISO88598I = 85,
	csISO139CSN369103 = 86,
	csISO141JUSIB1002 = 87,
	csISO143IECP271 = 88,
	csISO146Serbian = 89,
	csISO147Macedonian = 90,
	csISO150 = 91,
	csISO151Cuba = 92,
	csISO6937Add = 93,
	csISO153GOST1976874 = 94,
	csISO8859Supp = 95,
	csISO10367Box = 96,
	csISO158Lap = 97,
	csISO159JISX02121990 = 98,
	csISO646Danish = 99,
	csUSDK = 100,
	csDKUS = 101,
	csKSC5636 = 102,
	csUnicode11UTF7 = 103,
	csISO2022CN = 104,
	csISO2022CNEXT = 105,
	csUTF8 = 106,
	csISO885913 = 109,
	csISO885914 = 110,
	csISO885915 = 111,
	csISO885916 = 112,
	csGBK = 113,
	csGB18030 = 114,
	csOSDEBCDICDF0415 = 115,
	csOSDEBCDICDF03IRV = 116,
	csOSDEBCDICDF041 = 117,
	csISO115481 = 118,
	csKZ1048 = 119,
	csUnicode = 1000,
	csUCS4 = 1001,
	csUnicodeASCII = 1002,
	csUnicodeLatin1 = 1003,
	csUnicodeIBM1261 = 1005,
	csUnicodeIBM1268 = 1006,
	csUnicodeIBM1276 = 1007,
	csUnicodeIBM1264 = 1008,
	csUnicodeIBM1265 = 1009,
	csUnicode11 = 1010,
	csSCSU = 1011,
	csUTF7 = 1012,
	csUTF16BE = 1013,
	csUTF16LE = 1014,
	csUTF16 = 1015,
	csCESU8 = 1016,
	csUTF32 = 1017,
	csUTF32BE = 1018,
	csUTF32LE = 1019,
	csBOCU1 = 1020,
	csWindows30Latin1 = 2000,
	csWindows31Latin1 = 2001,
	csWindows31Latin2 = 2002,
	csWindows31Latin5 = 2003,
	csHPRoman8 = 2004,
	csAdobeStandardEncoding = 2005,
	csVenturaUS = 2006,
	csVenturaInternational = 2007,
	csDECMCS = 2008,
	csPC850Multilingual = 2009,
	csPCp852 = 2010,
	csPC8CodePage437 = 2011,
	csPC8DanishNorwegian = 2012,
	csPC862LatinHebrew = 2013,
	csPC8Turkish = 2014,
	csIBMSymbols = 2015,
	csIBMThai = 2016,
	csHPLegal = 2017,
	csHPPiFont = 2018,
	csHPMath8 = 2019,
	csHPPSMath = 2020,
	csHPDesktop = 2021,
	csVenturaMath = 2022,
	csMicrosoftPublishing = 2023,
	csWindows31J = 2024,
	csGB2312 = 2025,
	csBig5 = 2026,
	csMacintosh = 2027,
	csIBM037 = 2028,
	csIBM038 = 2029,
	csIBM273 = 2030,
	csIBM274 = 2031,
	csIBM275 = 2032,
	csIBM277 = 2033,
	csIBM278 = 2034,
	csIBM280 = 2035,
	csIBM281 = 2036,
	csIBM284 = 2037,
	csIBM285 = 2038,
	csIBM290 = 2039,
	csIBM297 = 2040,
	csIBM420 = 2041,
	csIBM423 = 2042,
	csIBM424 = 2043,
	csIBM500 = 2044,
	csIBM851 = 2045,
	csIBM855 = 2046,
	csIBM857 = 2047,
	csIBM860 = 2048,
	csIBM861 = 2049,
	csIBM863 = 2050,
	csIBM864 = 2051,
	csIBM865 = 2052,
	csIBM868 = 2053,
	csIBM869 = 2054,
	csIBM870 = 2055,
	csIBM871 = 2056,
	csIBM880 = 2057,
	csIBM891 = 2058,
	csIBM903 = 2059,
	csIBBM904 = 2060,
	csIBM905 = 2061,
	csIBM918 = 2062,
	csIBM1026 = 2063,
	csIBMEBCDICATDE = 2064,
	csEBCDICATDEA = 2065,
	csEBCDICCAFR = 2066,
	csEBCDICDKNO = 2067,
	csEBCDICDKNOA = 2068,
	csEBCDICFISE = 2069,
	csEBCDICFISEA = 2070,
	csEBCDICFR = 2071,
	csEBCDICIT = 2072,
	csEBCDICPT = 2073,
	csEBCDICES = 2074,
	csEBCDICESA = 2075,
	csEBCDICESS = 2076,
	csEBCDICUK = 2077,
	csEBCDICUS = 2078,
	csUnknown8BiT = 2079,
	csMnemonic = 2080,
	csMnem = 2081,
	csVISCII = 2082,
	csVIQR = 2083,
	csKOI8R = 2084,
	csHZGB2312 = 2085,
	csIBM866 = 2086,
	csPC775Baltic = 2087,
	csKOI8U = 2088,
	csIBM00858 = 2089,
	csIBM00924 = 2090,
	csIBM01140 = 2091,
	csIBM01141 = 2092,
	csIBM01142 = 2093,
	csIBM01143 = 2094,
	csIBM01144 = 2095,
	csIBM01145 = 2096,
	csIBM01146 = 2097,
	csIBM01147 = 2098,
	csIBM01148 = 2099,
	csIBM01149 = 2100,
	csBig5HKSCS = 2101,
	csIBM1047 = 2102,
	csPTCP154 = 2103,
	csAmiga1251 = 2104,
	csKOI7switched = 2105,
	csBRF = 2106,
	csTSCII = 2107,
	csCP51932 = 2108,
	cswindows874 = 2109,
	cswindows1250 = 2250,
	cswindows1251 = 2251,
	cswindows1252 = 2252,
	cswindows1253 = 2253,
	cswindows1254 = 2254,
	cswindows1255 = 2255,
	cswindows1256 = 2256,
	cswindows1257 = 2257,
	cswindows1258 = 2258,
	csTIS620 = 2259,
	cs50220 = 2260,
	reserved = 3000
};

//! \brief 通用别名。
yconstexpr const Encoding US_ASCII(csASCII), SHIFT_JIS(csShiftJIS),
	KS_C_5601_1987(csKSC56011987), EUC_KR(csEUCKR), ASMO_449(csISO89ASMO449),
	UTF_8(csUTF8), GBK(csGBK), GB18030(csGB18030), ISO_10646_UCS_2(csUnicode),
	ISO_10646_UCS_4(csUCS4), ISO_10646_UCS_Basic(csUnicodeASCII), UTF_7(csUTF7),
	UTF_16BE(csUTF16BE), UTF_16LE(csUTF16LE), UTF_16(csUTF16), UTF_32(csUTF32),
	UTF_32BE(csUTF32BE), UTF_32LE(csUTF32LE),
	GB2312(csGB2312), //!< 仅 EUC-CN ，GB2312 最常用实现。
	Big5(csBig5)//,
	;
	//未归类在 MIB 枚举中。
	/*
	HZ_GB_2312 = 52396, //!<  Chinese Simplified (HZ)，GB2312 的另一种实现。
	MIK; //!<  Supports Bulgarian and Russian as well
	*/

//! \brief 其它别名。
yconstexpr const Encoding iso_ir_6(US_ASCII), ISO646_US(US_ASCII),
	ASCII(US_ASCII), us(US_ASCII), IBM367(US_ASCII), cp367(US_ASCII),
	MS_Kanji(SHIFT_JIS), windows_932_(SHIFT_JIS), iso_ir_149(KS_C_5601_1987),
	KS_C_5601_1989(KS_C_5601_1987), KSC_5601(KS_C_5601_1987),
	korean(KS_C_5601_1987), windows_949_(KS_C_5601_1987), windows_65001_(UTF_8),
	CP936(GBK), MS936(GBK), windows_936(GBK), windows_54936_(GB18030),
	windows_1201_(UTF_16BE), windows_1200_(UTF_16LE), EUC_CN_(GB2312),
	windows_51936_(GB2312), windows_950_(Big5);

/*!
\ingroup metafunctions
\brief 取编码对应的字符类型。
\note 对变长编码取最小宽度对应的字符类型。
\since build 594
\todo 添加特化。
*/
//@{
template<Encoding>
struct EncodingTraits
{
	static yconstexpr const ByteOrder byte_order = ByteOrder::unknown;
	using type = char;
};

template<>
struct EncodingTraits<csUTF8>
{
	static yconstexpr const ByteOrder byte_order = ByteOrder::neutral;
	using type = char;
};

template<>
struct EncodingTraits<csUnicode>
{
	static yconstexpr const ByteOrder byte_order = ByteOrder::unknown;
	using type = char16_t;
};

template<>
struct EncodingTraits<csUCS4>
{
	static yconstexpr const ByteOrder byte_order = ByteOrder::unknown;
	using type = char32_t;
};

template<>
struct EncodingTraits<csUTF16>
{
	static yconstexpr ByteOrder byte_order = ByteOrder::unknown;
	using type = char16_t;
};

template<>
struct EncodingTraits<csUTF16BE> : EncodingTraits<csUTF16>
{
	static yconstexpr ByteOrder byte_order = ByteOrder::big;
};

template<>
struct EncodingTraits<csUTF16LE> : EncodingTraits<csUTF16>
{
	static yconstexpr ByteOrder byte_order = ByteOrder::little;
};

template<>
struct EncodingTraits<csUTF32>
{
	static yconstexpr ByteOrder byte_order = ByteOrder::unknown;
	using type = char32_t;
};

template<>
struct EncodingTraits<csUTF32BE> : EncodingTraits<csUTF32>
{
	static yconstexpr ByteOrder byte_order = ByteOrder::big;
};

template<>
struct EncodingTraits<csUTF32LE> : EncodingTraits<csUTF32>
{
	static yconstexpr ByteOrder byte_order = ByteOrder::little;
};
//@}

} // namespace CharSet;

} // namespace CHRLib;

#endif

