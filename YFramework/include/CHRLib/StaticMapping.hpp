﻿/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file StaticMapping.hpp
\ingroup CHRLib
\brief 静态编码映射。
\version r2009
\author FrankHB <frankhb1989@gmail.com>
\since build 587
\par 创建时间:
	2009-11-17 17:53:21 +0800
\par 修改时间:
	2015-04-10 01:34 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::StaticMapping
*/


#ifndef INC_CHRLib_StaticMapping_hpp_
#define INC_CHRLib_StaticMapping_hpp_ 1

#include "YModules.h"
#include YFM_CHRLib_CharacterMapping
#include <ystdex/cstdio.h>
#include <ystdex/any.h> // for ystdex::pseudo_object;

namespace CHRLib
{

/*!
\brief 以输入迭代器指向内容填充有效输入迭代器指定的字节。
\since build 273
*/
template<typename _tIn, typename _tState>
inline bool
FillByte(_tIn& i, _tState& st)
{
	static_assert(std::is_constructible<const byte, decltype(*i)>(),
		"Invalid mapping source type found.");
	static_assert(!std::is_volatile<ystdex::remove_reference_t<_tState>>(),
		"Volatile state is not supported.");

	if(YB_UNLIKELY(is_undereferenceable(i)))
		return {};

	const byte r(*i);

	yunseq(++i, GetSequenceOf(st)[GetCountOf(st)++] = r);
	return true;
}


//! \since build 552
//@{
//! \brief 判断八元组是否组成 UTF-16 代理对起始代码的高 8 位。
yconstfn bool
IsSurrogateHead(octet x)
{
	return x >= 0xD8 && x < 0xE0;
}

//! \brief 判断八元组是否组成 UTF-16 有效代理对起始代码的高 8 位。
yconstfn bool
IsValidSurrogateHead(octet x)
{
	return x >= 0xD8 && x < 0xDE;
}

//! \brief 判断八元组是否组成 UTF-16 有效代理对终止代码的高 8 位。
yconstfn bool
IsValidSurrogateTail(octet x)
{
	return x >= 0xDC && x < 0xE0;
}
//@}


/*!
\brief 静态编码映射模板及 Unicode 编码特化。
\since build 245
*/
//@{
template<Encoding>
class GUCS2Mapper
{};

template<>
struct GUCS2Mapper<CharSet::UTF_8>
{
	/*!
	\brief 检查 UTF-8 文本序列中非法字节。
	\note 包括： C0 、 C1 、 F5 至 FF 。
	\since build 552
	*/
	static yconstfn bool
	IsInvalid(octet x)
	{
		return x == 0xC0 || x == 0xC1 || x > 0xF4;
	}

	/*!
	\brief 映射： UTF-8 。
	\warning 当前实现假定编码序列完整。
	\warning 使用 UCS-2 时， 4 字节代码点可能溢出。
	\see 参考规范： RFC 3629 ，见 http://tools.ietf.org/html/rfc3629 。

	实现 UTF-8 到 Unicode 代码点的映射。
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
				uc = (((seq[0] & 0x0F) << 4 | (seq[1] & 0x3C) >> 2) << 8)
					| ((seq[1] & 0x3) << 6) | (seq[2] & 0x3F);
				break;
			}
		case 3:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(IsInvalid(seq[3]) || ((seq[3] & 0xC0) != 0x80)))
				return ConversionResult::Invalid;
			if(YB_LIKELY(((seq[0] ^ 0xF0) & 0xF8) == 0))
			{
				uc = (((seq[0] & 0x0F) << 4 | (seq[1] & 0x3C) >> 2) << 8)
					| ((seq[1] & 0x3) << 6) | (seq[2] & 0x3F);
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
			*d = char(0xE0 | s >> 12U);
			++d;
			l = 3;
		}
		*d = char(0x80 | (s >> 6 & 0x3F));
		*++d = char(0x80 | (s & 0x3F));
		return l;
	}
};

template<>
struct GUCS2Mapper<CharSet::UTF_16BE>
{
	/*!
	\brief 映射： UTF-16BE 。
	\see 参考规范： RFC 2781 ，见 http://tools.ietf.org/html/rfc2781 。
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
		case 1:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(IsSurrogateHead(seq[0])))
			{
				if(YB_UNLIKELY(!IsValidSurrogateHead(seq[0])))
					return ConversionResult::Invalid;
			}
			else
			{
				uc = seq[0] << 8 | seq[1];
				break;
			}
		case 2:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
		case 3:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(!IsValidSurrogateTail(seq[2])))
				return ConversionResult::Invalid;
			uc = (seq[0] & 0x03) << 18 | seq[1] << 10 | (seq[2] & 0x03) << 8
				| seq[3];
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
	/*!
	\brief 映射： UTF-16LE 。
	\see 参考规范： RFC 2781 ，见 http://tools.ietf.org/html/rfc2781 。
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
		case 1:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(IsSurrogateHead(seq[1])))
			{
				if(YB_UNLIKELY(!IsValidSurrogateHead(seq[1])))
					return ConversionResult::Invalid;
			}
			else
			{
				uc = seq[0] | seq[1] << 8;
				break;
			}
		case 2:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
		case 3:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(!IsValidSurrogateTail(seq[3])))
				return ConversionResult::Invalid;
			uc = seq[0] << 10 | (seq[1] & 0x03) << 18 | seq[2]
				| (seq[3] & 0x03) << 8;
			break;
		default:
			return ConversionResult::BadState;
		}
		return ConversionResult::OK;
	}
};

//! \since build 552
template<>
struct GUCS2Mapper<CharSet::UTF_32BE>
{
	/*!
	\brief 映射： UTF-32BE 。
	\see 参考规范： UAX #19 ，见 http://www.unicode.org/unicode/reports/tr19/ 。
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
		case 1:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
		case 2:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
		case 3:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			uc = seq[0] << 24 | seq[1] << 16 | seq[2] << 8 | seq[3];
			break;
		default:
			return ConversionResult::BadState;
		}
		return ConversionResult::OK;
	}
};

//! \since build 552
template<>
struct GUCS2Mapper<CharSet::UTF_32LE>
{
	/*!
	\brief 映射： UTF-32LE 。
	\see 参考规范： UAX #19 ，见 http://www.unicode.org/unicode/reports/tr19/ 。
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
		case 1:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
		case 2:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
		case 3:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			uc = seq[2] | seq[3] << 8 | seq[0] << 16 | seq[1] << 24;
			break;
		default:
			return ConversionResult::BadState;
		}
		return ConversionResult::OK;
	}
};
//@}


/*!
\brief 取映射函数。
\since build 273
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
	&GUCS2Mapper<_vEnc>::template Map<_tDst, _tSrc, _tState>) = {})
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
	decltype(&GUCS2Mapper<_vEnc>::template InverseMap<_tDst>) = {})
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
//@}

} // namespace CHRLib;

#endif
