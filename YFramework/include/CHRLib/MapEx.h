/*
	© 2012-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MapEx.h
\ingroup CHRLib
\brief 附加编码映射。
\version r266
\author FrankHB <frankhb1989@gmail.com>
\since build 324
\par 创建时间:
	2012-07-09 09:04:36 +0800
\par 修改时间:
	2013-12-24 09:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::MappingEx
*/


#ifndef INC_CHRLib_MapEx_h_
#define INC_CHRLib_MapEx_h_ 1

#include "YModules.h"
#include YFM_CHRLib_StaticMapping

namespace CHRLib
{

/*!
\def CHRLIB_NODYNAMIC_MAPPING
\since build 324

无动态映射。
*/

#define CHRLIB_NODYNAMIC_MAPPING 0

#if CHRLIB_NODYNAMIC_MAPPING
/*!
\brief 编码转换表。
\since 早于 build 132
*/
//@{
yconstexpr byte cp17[]{0};
extern "C"
{
	extern const byte cp113[];
	//extern const byte cp2026[13658];
}
yconstexpr byte cp2026[]{0};
//@}
#else

/*!
\brief 动态加载的编码转换表。
\warning 使用转换映射算法前必须保证非空初始化，否则会断言失败或引起未定义行为。
\warning 无线程安全保护。
\since build 324
*/
//@{
extern byte* cp17;
extern byte* cp113;
extern byte* cp2026;
//@}

#endif


/*!
\brief 非 Unicode 编码映射模板特化。
\since build 245
*/
//@{
template<>
struct GUCS2Mapper<CharSet::SHIFT_JIS>
{
/*	template<typename _tObj, typename _tIn, typename _tState>
	static byte
	Map(_tObj& uc, _tIn&& i, _tState&& st)
	{
		uint_least16_t row(0), col(0), ln(188); // (7E-40 + 1 + FC-80 + 1)
		const auto c(FillByte(i, st));

		if((c >= 0xA1) && (c <= 0xC6))
		{
			const auto d(FillByte(i, st));

			row = c - 0xA1 ;
			if(d >= 0x40 && d <= 0x7E)
				col = d - 0x40 ;
			else if(d >= 0xA1 && d <= 0xFE)
				col = d - 0x62;
			uc = cp17[row * ln + col];
		}
		else if(c >= 0xC9 && c <= 0xF9)
		{
			const auto d(FillByte(i, st));

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
	}*/
};

template<>
struct GUCS2Mapper<CharSet::GBK>
{
	template<typename _tObj, typename _tIn, typename _tState>
	static ConversionResult
	Map(_tObj& uc, _tIn&& i, _tState&& st)
	{
		yassume(cp113);

		const auto seq(GetSequenceOf(st));

		switch(GetCountOf(st))
		{
		case 0:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(cp113[seq[0]] != 0)
			{
				uc = seq[0];
				break;
			}
		case 1:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_LIKELY((seq[0] << 8 | seq[1]) < 0xFF7E))
			{
				uc = reinterpret_cast<const ucs2_t*>(cp113 + 0x0100)[
					seq[0] << 8 | seq[1]];
				break;
			}
			return ConversionResult::Unhandled;
		default:
			return ConversionResult::BadState;
		}
		return ConversionResult::OK;
	}
};

template<>
struct GUCS2Mapper<CharSet::Big5>
{
/*	template<typename _tObj, typename _tIn, typename _tState>
	static byte
	Map(_tObj& uc, _tIn&& i, _tState&& st)
	{
		uint_least16_t row(0), col(0), ln(157); // (7E-40 + FE-A1)
		const auto c(FillByte(i, st));

		if(c >= 0xA1 && c <= 0xC6)
		{
			const auto d(FillByte(i, st));

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
			const auto d(FillByte(i, st));

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
	}*/
};
//@}


/*!
\brief 取指定编码映射的转换函数指针。
\since build 291
*/
template<typename _fCodemapTransform>
_fCodemapTransform*
FetchMapperPtr(Encoding enc)
{
	using namespace CharSet;

#define CHR_MapItem(enc) \
case enc: \
	return UCS2Mapper<enc>;

	switch(enc)
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

} // namespace CHRLib;

#endif

