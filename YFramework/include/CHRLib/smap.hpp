/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file smap.hpp
\ingroup CHRLib
\brief 静态编码映射。
\version r2652;
\author FrankHB<frankhb1989@gmail.com>
\since build 247 。
\par 创建时间:
	2009-11-17 17:53:21 +0800;
\par 修改时间:
	2012-06-01 16:58 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	CHRLib::StaticMapping;
*/


#ifndef CHRLIB_INC_SMAP_HPP_
#define CHRLIB_INC_SMAP_HPP_

#include "chrmap.h"
#include <ystdex/cstdio.h>
#include <ystdex/any.h> // for ystdex::pseudo_object;

CHRLIB_BEGIN

/*!
\brief 编码转换表。
\since 早于 build 132 。
*/
//@{
yconstexpr byte cp17[] = {0};
extern "C"
{
	extern const byte cp113[];
	//extern const byte cp2026[13658];
}
yconstexpr byte cp2026[] = {0};
//@}


/*!
\brief 以输入迭代器指向内容填充有效输入迭代器指定的字节。
\since build 273 。
*/
template<typename _tIn, typename _tState>
inline bool
FillByte(_tIn& i, _tState& st)
{
	static_assert(std::is_explicitly_convertible<decltype(*i), byte>::value,
		"Invalid mapping source type found;");
	static_assert(!std::is_volatile<typename std::remove_reference<
		_tState>::type>::value, "Volatile state is unsupported;");

	if(YB_UNLIKELY(is_undereferencable(i)))
		return false;

	const auto r(static_cast<byte>(*i));

	yunseq(++i, GetSequenceOf(st)[GetCountOf(st)++] = r);
	return true;
}


/*!
\brief 静态编码映射函数模板。
\since build 245 。
*/
//@{
template<Encoding>
class GUCS2Mapper
{};

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
struct GUCS2Mapper<CharSet::UTF_8>
{
	/*!
	\brief 检查 UTF-8 文本序列中非法字节。
	\note 包括： C0 、 C1 、 F5 至 FF 。
	*/
	static yconstfn bool
	IsInvalid(byte b)
	{
		return b == 0xC0 || b == 0xC1 || b > 0xF4;
	}

	/*!
	\brief 映射： UTF-8 。

	实现 UTF-8 到 Unicode 编码点的映射。
	\warning 当前实现假定编码序列完整。
	\warning 使用 UCS-2LE 时， 4 字节编码点可能溢出。
	\note 参考规范： RFC 3629 ，见 http://tools.ietf.org/html/rfc3629 。
	*/
	template<typename _tObj, typename _tIn, typename _tState>
	static ConversionResult
	Map(_tObj& uc, _tIn&& i, _tState&& st)
	{
		const auto seq(GetSequenceOf(st));

		switch(GetCountOf(st))
		{
		case 0:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(seq[0] < 0x80)
			{
				uc = seq[0];
				break;
			}
			if(YB_UNLIKELY(IsInvalid(seq[0]) || ((seq[0] & 0xC0) != 0xC0)))
				return ConversionResult::Invalid;
		case 1:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(IsInvalid(seq[1]) || ((seq[1] & 0xC0) != 0x80)))
				return ConversionResult::Invalid;
			if(((seq[0] ^ 0xC0) & 0xE0) == 0)
			{
				uc = ((seq[0] & 0x1C) >> 2 << 8)
					| ((seq[0] & 0x03) << 6)
					| (seq[1] & 0x3F);
				break;
			}
		case 2:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(IsInvalid(seq[2]) || ((seq[2] & 0xC0) != 0x80)))
				return ConversionResult::Invalid;
			if(((seq[0] ^ 0xE0) & 0xF0) == 0)
			{
				uc = (((seq[0] & 0x0F) << 4
					| (seq[1] & 0x3C) >> 2) << 8)
					| ((seq[1] & 0x3) << 6)
					| (seq[2] & 0x3F);
				break;
			}
		case 3:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(IsInvalid(seq[3]) || ((seq[3] & 0xC0) != 0x80)))
				return ConversionResult::Invalid;
			if(YB_LIKELY(((seq[0] ^ 0xF0) & 0xF8) == 0))
			{
				uc = (((seq[0] & 0x0F) << 4
					| (seq[1] & 0x3C) >> 2) << 8)
					| ((seq[1] & 0x3) << 6)
					| (seq[2] & 0x3F);
				break;
			}
			return ConversionResult::Unhandled;
		default:
			return ConversionResult::BadState;
		}
		return ConversionResult::OK;
	}

	template<typename _tOut>
	static byte
	InverseMap(_tOut d, const ucs2_t& s)
	{
		size_t l(0);

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
	template<typename _tObj, typename _tIn, typename _tState>
	static ConversionResult
	Map(_tObj& uc, _tIn&& i, _tState&& st)
	{
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
struct GUCS2Mapper<CharSet::UTF_16BE>
{
	template<typename _tObj, typename _tIn, typename _tState>
	static ConversionResult
	Map(_tObj& uc, _tIn&& i, _tState&& st)
	{
		const auto seq(GetSequenceOf(st));

		switch(GetCountOf(st))
		{
		case 0:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
		case 1:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			uc = seq[0] << 8 | seq[1];
			break;
		default:
			return ConversionResult::BadState;
		}
		return ConversionResult::OK;
	}
};

template<>
struct GUCS2Mapper<CharSet::UTF_16LE>
{
	template<typename _tObj, typename _tIn, typename _tState>
	static ConversionResult
	Map(_tObj& uc, _tIn&& i, _tState&& st)
	{
		const auto seq(GetSequenceOf(st));

		switch(GetCountOf(st))
		{
		case 0:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
		case 1:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			uc = seq[0] | seq[1] << 8;
			break;
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
\brief 取映射函数。
\since build 273 。
*/
//@{
template<Encoding, typename... _tParams>
yconstfn ConversionResult
UCS2Mapper_Map(_tParams&&...)
{
	return ConversionResult::Unhandled;
}
template<Encoding _vEnc, typename _tDst, typename _tSrc, typename _tState>
yconstfn ConversionResult
UCS2Mapper_Map(_tDst&& d, _tSrc&& s, _tState&& st, decltype(
	&GUCS2Mapper<_vEnc>::template Map<_tDst, _tSrc, _tState>) = nullptr)
{
	return GUCS2Mapper<_vEnc>::Map(d, s, st);
}

template<Encoding _vEnc, typename _tDst, typename _tSrc>
yconstfn byte
UCS2Mapper_InverseMap(_tDst, _tSrc)
{
	return 0;
}
template<Encoding _vEnc, typename _tDst>
yconstfn byte
UCS2Mapper_InverseMap(_tDst&& d, const ucs2_t& s,
	decltype(&GUCS2Mapper<_vEnc>::template InverseMap<_tDst>) = nullptr)
{
	return GUCS2Mapper<_vEnc>::InverseMap(d, s);
}


template<Encoding _vEnc, typename _tIn, typename _tState>
yconstexpr ConversionResult
UCS2Mapper(ucs2_t& uc, _tIn&& i, _tState&& st)
{
	return UCS2Mapper_Map<_vEnc>(uc, i, std::move(st));
}
template<Encoding _vEnc, typename _tIn, typename _tState>
yconstexpr ConversionResult
UCS2Mapper(_tIn&& i, _tState&& st)
{
	return UCS2Mapper_Map<_vEnc>(ystdex::pseudo_output(), i, st);
}
template<Encoding _vEnc>
byte
UCS2Mapper(char* d, const ucs2_t& s)
{
	yconstraint(d);

	return UCS2Mapper_InverseMap<_vEnc>(d, s);
}

/*!
\brief 取指定编码映射的转换函数指针。
\since build 291 。
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
//@}

CHRLIB_END

#endif

