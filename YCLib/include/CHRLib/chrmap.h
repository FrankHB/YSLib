/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file chrmap.h
\ingroup CHRLib
\brief 字符映射。
\version r2038;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-17 17:52:35 +0800;
\par 修改时间:
	2011-09-23 10:19 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	CHRLib::CharacterMapping;
*/


#ifndef CHRLIB_INC_CHRMAP_H_
#define CHRLIB_INC_CHRMAP_H_

#include "chrdef.h"
#include <ystdex/cstdio.h>

CHRLIB_BEGIN

CHRLIB_BEGIN_NAMESPACE(CharSet)

/*
编码字符集：使用 IANA 官方字符集名称和 MIBenum ，
详见 http://www.iana.org/assignments/character-sets 。
*/

typedef enum
{
	//保留。
	Null = 0, //!< 空字符集。
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

	GB2312 = 2025, //!< 仅 EUC-CN ，GB2312 最常用实现。
	Big5 = 2026//,

	//其它。
	/*
	HZ_GB_2312 = 52396, //!<  Chinese Simplified (HZ)，GB2312 的另一种实现。
	MIK, //!<  Supports Bulgarian and Russian as well
	*/
} Encoding; //!< 字符流编码标识。

//别名。
const Encoding
	iso_ir_6(US_ASCII), ISO646_US(US_ASCII), ASCII(US_ASCII), us(US_ASCII),
		IBM367(US_ASCII), cp367(US_ASCII), csASCII(US_ASCII),
	MS_Kanji(SHIFT_JIS), csShiftJIS(SHIFT_JIS), windows_932_(SHIFT_JIS),
	iso_ir_149(KS_C_5601_1987), KS_C_5601_1989(KS_C_5601_1987),
		KSC_5601(KS_C_5601_1987), korean(KS_C_5601_1987),
		csKSC56011987(KS_C_5601_1987), windows_949_(KS_C_5601_1987),
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

using CharSet::Encoding;


//默认字符集。
const Encoding CP_Default = CharSet::UTF_8;
const Encoding CP_Local = CharSet::GBK;


//编码转换表。
const ubyte_t cp17[] = {0};
extern "C"
{
	extern const ubyte_t cp113[];
	//extern const ubyte_t cp2026[13658];
}
const ubyte_t cp2026[] = {0};


/*!
\brief 取 c_ptr 指向的大端序双字节字符。
\pre 断言： \c c_ptr 。
*/
inline ucs2_t
FetchBiCharBE(const char* c_ptr)
{
	assert(c_ptr);

	return (*c_ptr << YCL_CHAR_BIT) | c_ptr[1];
}

/*!
\brief 取 c_ptr 指向的小端序双字节字符。
\pre 断言： \c c_ptr 。
*/
inline ucs2_t
FetchBiCharLE(const char* c_ptr)
{
	assert(c_ptr);

	return (c_ptr[1] << YCL_CHAR_BIT) | *c_ptr;
}


//未实现的编码映射函数模板原型。
template<Encoding>
class GUCS2Mapper
{};

//编码映射函数模板特化版本。
template<>
struct GUCS2Mapper<CharSet::SHIFT_JIS>
{
	template<typename _tIn>
	static ubyte_t
	Map(ucs2_t& uc, _tIn it)
	{
		uint_least16_t row(0), col(0), ln(188); // (7E-40 + 1 + FC-80 + 1)
		const unsigned c(*it);

		if((c >= 0xA1) && (c <= 0xC6))
		{
			const unsigned d(*++it);

			row = c - 0xA1 ;
			if(d >= 0x40 && d <= 0x7E)
				col = d - 0x40 ;
			else if(d >= 0xA1 && d <= 0xFE)
				col = d - 0x62;
			uc = cp17[row * ln + col];
		}
		else if(c >= 0xC9 && c <= 0xF9)
		{
			const unsigned d(*++it);

			row = c - 0xA3;
			if(d >= 0x40 && d <= 0x7E)
				col = d - 0x40 ;
			else if(d >= 0xA1 && d <= 0xFE)
				col = d - 0x62;
			uc = cp17[row * ln + col];
		}
		else if(c < 0x80)
		{
			uc = c;
			return 1;
		}
		else
			uc = 0xFFFE;
		return 2;
	}
};

template<>
struct GUCS2Mapper<CharSet::UTF_8>
{
	template<typename _tIn>
	static ubyte_t
	Map(ucs2_t& uc, _tIn it)
	{
		const unsigned c(*it);

		if(c < 0x80)
		{
			uc = c;
			return 1;
		}
		else
		{
			const unsigned d(*++it);

			if(c & 0x20)
			{
				uc = (((c & 0x0F) << 4
					| (d & 0x3C) >> 2) << 8)
					| ((d & 0x3) << 6)
					| (*++it & 0x3F);
				return 3;
			}
			else
			{
				uc = ((c & 0x1C) >> 2 << 8) | ((c & 0x03) << 6) | (d & 0x3F);
				return 2;
			}
		}
		/*
		else if(c0 >= 0xC2 && c0 <= 0xDF)
		{
			if(c0 == '\0' || c[1] == '\0')
			{
				uc = 0xFFFE;
				return 0;
			}
			uc = ((c0 - 192) << 6) | (c[1] - 128);
			return 2;
		}
		else if(c0 >= 0xE0 && c0 <= 0xEF)
		{
			if(c0 == '\0' || c[1] == '\0' || c[2] == '\0')
			{
				uc = 0;
				return 0;
			}
			uc = ((c0 - 224) << 12) | ((c[1] - 128) << 6) | (c[2] - 128);
			return 3;
		}
		else if(c0 >= 0xF0 && c0 <= 0xF4)
		{
			if(c0 == '\0' || c[1] == '\0' || c[2] == '\0' || c[3] == '\0')
			{
				uc = 0;
				return 0;
			}
			uc = ((c0 - 240) << 18)
				| ((c[1] - 128) << 12)
				| ((c[2] - 128) << 6)
				| (c[3] - 128);
			return 4;
		}
		*/
	}

	template<typename _tOut>
	static ubyte_t
	InverseMap(_tOut d, const ucs2_t& s)
	{
		usize_t l(0);

		if(s < 0x80)
		{
			*d = s;
			return 1;
		}
		if(s < 0x800)
			l = 2;
		else
		{
			*d = 0xE0 | s >> 12;
			++d;
			l = 3;
		}
		*d = 0x80 | (s >> 6 & 0x3F);
		*++d = 0x80 | (s & 0x3F);
		return l;
	}
};

template<>
struct GUCS2Mapper<CharSet::GBK>
{
	template<typename _tIn>
	static ubyte_t
	Map(ucs2_t& uc, _tIn it)
	{
		int c(*it);

		if(static_cast<ubyte_t>(c))
		{
			uc = c;
			return 1;
		}
		uc = reinterpret_cast<const ucs2_t*>(cp113 + 0x0100)
			[(c << 8 | *++it)];
		return 2;
	}
};

template<>
struct GUCS2Mapper<CharSet::UTF_16BE>
{
	template<typename _tIn>
	static ubyte_t
	Map(ucs2_t& uc, _tIn it)
	{
		uc = *it << YCL_CHAR_BIT;
		uc |= *++it;
		return 2;
	}
};

template<>
struct GUCS2Mapper<CharSet::UTF_16LE>
{
	template<typename _tIn>
	static ubyte_t
	Map(ucs2_t& uc, _tIn it)
	{
		uc = *it;
		uc |= *++it << YCL_CHAR_BIT;
		return 2;
	}
};

template<>
struct GUCS2Mapper<CharSet::Big5>
{
	template<typename _tIn>
	static ubyte_t
	Map(ucs2_t& uc, _tIn it)
	{
		uint_least16_t row(0), col(0), ln(157); // (7E-40 + FE-A1)
		const unsigned c(*it);

		if(c >= 0xA1 && c <= 0xC6)
		{
			const unsigned d(*++it);

			row = c - 0xA1;
			if(d >= 0x40 && d <= 0x7E)
				col = d - 0x40;
			else if(d >= 0xA1 && d <= 0xFE)
				col = d - 0x62;
			uc = cp2026[row * ln + col];
			return 2;
		}
		else if(c >= 0xC9 && c <= 0xF9)
		{
			const unsigned d(*++it);

			row = c - 0xA3;
			if(d >= 0x40 && d <= 0x7E)
				col = c - 0x40;
			else if(d >= 0xA1 && d <= 0xFE)
				col = d - 0x62;
			uc = cp2026[row * ln + col];
			return 2;
		}
		else if(c < 0x80)
		{
			uc = c;
			return 1;
		}
		else
			uc = 0xFFFE;
		return 2;
	}
};

CHRLIB_END

#endif

