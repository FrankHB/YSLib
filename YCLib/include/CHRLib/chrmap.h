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
\version r2060;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-17 17:52:35 +0800;
\par 修改时间:
	2011-09-26 15:42 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	CHRLib::CharacterMapping;
*/


#ifndef CHRLIB_INC_CHRMAP_H_
#define CHRLIB_INC_CHRMAP_H_

#include "chrdef.h"

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
yconstexpr Encoding
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
yconstexpr Encoding CP_Default = CharSet::UTF_8;
yconstexpr Encoding CP_Local = CharSet::GBK;


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

CHRLIB_END

#endif

