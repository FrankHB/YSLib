/*
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
\version r2194
\author FrankHB <frankhb1989@gmail.com>
\since build 587
\par 创建时间:
	2009-11-17 17:53:21 +0800
\par 修改时间:
	2015-04-30 10:11 +0800
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

	yunseq(++i, GetSequenceOf(st)[GetIndexOf(st)++] = r);
	return true;
}


//! \since build 594
//@{
//! \brief 判断八元组是否组成 UTF-16 代理对起始代码的高 8 位。
yconstfn PDefH(bool, IsSurrogateHead, octet x) ynothrow
	ImplRet(x >= 0xD8 && x < 0xE0)

//! \brief 判断八元组是否组成 UTF-16 有效代理对起始代码的高 8 位。
yconstfn PDefH(bool, IsValidSurrogateHead, octet x) ynothrow
	ImplRet(x >= 0xD8 && x < 0xDE)

//! \brief 判断八元组是否组成 UTF-16 有效代理对终止代码的高 8 位。
yconstfn PDefH(bool, IsValidSurrogateTail, octet x) ynothrow
	ImplRet(x >= 0xDC && x < 0xE0)


//! \brief 提供编码映射公共操作默认实现的基类。
struct UCSMapperBase
{
	static void
	Assign(ystdex::pseudo_output, ucs4_t) ynothrow
	{}
	template<typename _tObj>
	static void
	Assign(_tObj& uc, ucs4_t c) ynothrow
	{
		ynoexcept_assert("Invalid type found", uc = _tObj(c));

		uc = _tObj(c);
	}
};


/*!
\brief 静态编码操作模板及 Unicode 编码特化。
\note 使用 ADL FillByte 填充字节。
\note 使用 ADL GetIndexOf 和 GetSequenceOf 操作状态。
\sa UCSMapperBase

提供编码转换的映射模板。
假定通过外部指定具体字符类型的中间状态为 Unicode 代码点。
默认使用小端序（即 UCS-2LE 或 UCS-4LE ），对应字符类型 ucs2_t 和 ucs4_t 。
注意保留超出基本多文种平面的代码点需要使用范围足够大的字符类型。
若使用 ucs2_t ，对越界部分不保证能保留正确的结果，取决于中间状态的字符赋值时的转换。
一般接口（静态成员函数模板）如下：
IsInvalid ：判断八元组是否被禁止的接口。
Decode ：解码操作：从指定编码的字符序列映射到中间状态的字符。
Encode ：编码操作：从中间状态的字符逆向映射到指定编码的字符序列。
解码操作允许接受 ystdex::pseudo_object 忽略输出。
使用模板参数决定状态类型中，状态索引等于已解码的输入字节数；否则不予保证。
*/
//@{
template<Encoding>
class GUCSMapper
{};

/*!
\warning 使用 UCS-2 时， 4 字节代码点可能溢出。
\see 参考规范： RFC 3629 ，见 http://tools.ietf.org/html/rfc3629 。
*/
template<>
struct GUCSMapper<CharSet::UTF_8> : UCSMapperBase
{
	using UCSMapperBase::Assign;

private:
	template<typename _tObj, typename _tSeq>
	static void
	Assign1(_tObj& uc, const _tSeq& seq) ynothrow
	{
		ynoexcept_assert("Invalid type found", Assign(uc, ucs4_t()));

		Assign(uc, (ucs4_t(seq[0] & 0x1CU) >> 2U << 8U)
			| (ucs4_t(seq[0] & 0x03U) << 6) | (ucs4_t(seq[1]) & 0x3FU));
	}

	template<typename _tObj, typename _tSeq>
	static void
	Assign2(_tObj& uc, const _tSeq& seq) ynothrow
	{
		ynoexcept_assert("Invalid type found", Assign(uc, ucs4_t()));

		Assign(uc, (((ucs4_t(seq[0]) & 0x0FU) << 4U
			| ucs4_t(seq[1] & 0x3CU) >> 2U) << 8U)
			| (ucs4_t(seq[1] & 0x3U) << 6U) | ucs4_t(seq[2] & 0x3FU));
	}

	static yconstfn bool
	CheckInvalid(octet x, octet y = 0x80) ynothrow
	{
		return IsInvalid(x) || ((x & 0xC0) != y);
	}

public:
	/*!
	\brief 检查 UTF-8 文本序列中非法字节。
	\note 包括： C0 、 C1 、 F5 至 FF 。
	*/
	static yconstfn bool
	IsInvalid(octet x) ynothrow
	{
		return x == 0xC0 || x == 0xC1 || x > 0xF4;
	}

	//! \brief 解码： UTF-8 。
	template<typename _tObj, typename _tIn, typename _tState>
	static ConversionResult
	Decode(_tObj& uc, _tIn&& i, _tState&& st)
		ynoexcept(noexcept(GetSequenceOf(st)) && noexcept(GetIndexOf(st))
		&& noexcept(!FillByte(i, st)))
	{
		const auto seq(GetSequenceOf(st));

		switch(GetIndexOf(st))
		{
		case 0:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if((seq[0] & 0x80) == 0)
			{
				Assign(uc, seq[0]);
				break;
			}
			if(YB_UNLIKELY(CheckInvalid(seq[0], 0xC0)))
				return ConversionResult::Invalid;
		case 1:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(CheckInvalid(seq[1])))
				return ConversionResult::Invalid;
			if(((seq[0] ^ 0xC0) & 0xE0) == 0)
			{
				Assign1(uc, seq);
				break;
			}
		case 2:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(CheckInvalid(seq[2])))
				return ConversionResult::Invalid;
			if(((seq[0] ^ 0xE0) & 0xF0) == 0)
			{
				Assign2(uc, seq);
				break;
			}
		case 3:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(CheckInvalid(seq[3])))
				return ConversionResult::Invalid;
			if(YB_LIKELY(((seq[0] ^ 0xF0) & 0xF8) == 0))
			{
				Assign2(uc, seq);
				break;
			}
			return ConversionResult::Unhandled;
		default:
			return ConversionResult::BadState;
		}
		return ConversionResult::OK;
	}

	template<typename _tOut>
	static size_t
	Encode(_tOut d, ucs4_t s) ynothrow
	{
		ynoexcept_assert("Invalid type found.", *d = char());

		size_t l(0);

		if(s < 0x80U)
		{
			*d = s;
			return 1;
		}
		if(s < 0x800U)
			l = 2;
		else
		{
			*d = char(0xE0U | s >> 12U);
			++d;
			l = 3;
		}
		*d = char(0x80U | (s >> 6U & 0x3FU));
		*++d = char(0x80U | (s & 0x3FU));
		return l;
	}
};

//! \see 参考规范： RFC 2781 ，见 http://tools.ietf.org/html/rfc2781 。
template<>
struct GUCSMapper<CharSet::UTF_16BE> : UCSMapperBase
{
	using UCSMapperBase::Assign;

	//! \brief 解码： UTF-16BE 。
	template<typename _tObj, typename _tIn, typename _tState>
	static ConversionResult
	Decode(_tObj& uc, _tIn&& i, _tState&& st)
		ynoexcept(noexcept(GetSequenceOf(st))
		&& noexcept(GetIndexOf(st)) && noexcept(!FillByte(i, st)))
	{
		const auto seq(GetSequenceOf(st));

		switch(GetIndexOf(st))
		{
		case 0:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
		case 1:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(CHRLib::IsSurrogateHead(seq[0])))
			{
				if(YB_UNLIKELY(!CHRLib::IsValidSurrogateHead(seq[0])))
					return ConversionResult::Invalid;
			}
			else
			{
				Assign(uc, ucs4_t(seq[0]) << 8U | ucs4_t(seq[1]));
				break;
			}
		case 2:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
		case 3:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(!CHRLib::IsValidSurrogateTail(seq[2])))
				return ConversionResult::Invalid;
			Assign(uc, ucs4_t(seq[0] & 0x03U) << 18U | ucs4_t(seq[1]) << 10U
				| ucs4_t(seq[2] & 0x03U) << 8U | ucs4_t(seq[3]));
			break;
		default:
			return ConversionResult::BadState;
		}
		return ConversionResult::OK;
	}
};

//! \see 参考规范： RFC 2781 ，见 http://tools.ietf.org/html/rfc2781 。
template<>
struct GUCSMapper<CharSet::UTF_16LE> : UCSMapperBase
{
	using UCSMapperBase::Assign;

	//! \brief 解码： UTF-16LE 。
	template<typename _tObj, typename _tIn, typename _tState>
	static ConversionResult
	Decode(_tObj& uc, _tIn&& i, _tState&& st)
		ynoexcept(noexcept(GetSequenceOf(st))
		&& noexcept(GetIndexOf(st)) && noexcept(!FillByte(i, st)))
	{
		const auto seq(GetSequenceOf(st));

		switch(GetIndexOf(st))
		{
		case 0:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
		case 1:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(CHRLib::IsSurrogateHead(seq[1])))
			{
				if(YB_UNLIKELY(!CHRLib::IsValidSurrogateHead(seq[1])))
					return ConversionResult::Invalid;
			}
			else
			{
				Assign(uc, ucs4_t(seq[0]) | ucs4_t(seq[1]) << 8U);
				break;
			}
		case 2:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
		case 3:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(YB_UNLIKELY(!CHRLib::IsValidSurrogateTail(seq[3])))
				return ConversionResult::Invalid;
			Assign(uc, ucs4_t(seq[0] << 10U) | ucs4_t(seq[1] & 0x03U) << 18U
				| ucs4_t(seq[2]) | ucs4_t(seq[3] & 0x03U) << 8U);
			break;
		default:
			return ConversionResult::BadState;
		}
		return ConversionResult::OK;
	}
};

//! \see 参考规范： UAX #19 ，见 http://www.unicode.org/unicode/reports/tr19/ 。
template<>
struct GUCSMapper<CharSet::UTF_32BE> : UCSMapperBase
{
	using UCSMapperBase::Assign;

	//! \brief 解码： UTF-32BE 。
	template<typename _tObj, typename _tIn, typename _tState>
	static ConversionResult
	Decode(_tObj& uc, _tIn&& i, _tState&& st)
		ynoexcept(noexcept(GetSequenceOf(st))
		&& noexcept(GetIndexOf(st)) && noexcept(!FillByte(i, st)))
	{
		const auto seq(GetSequenceOf(st));

		switch(GetIndexOf(st))
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
			Assign(uc, ucs4_t(seq[0]) << 24U | ucs4_t(seq[1]) << 16U
				| ucs4_t(seq[2]) << 8U | ucs4_t(seq[3]));
			break;
		default:
			return ConversionResult::BadState;
		}
		return ConversionResult::OK;
	}
};

//! \see 参考规范： UAX #19 ，见 http://www.unicode.org/unicode/reports/tr19/ 。
template<>
struct GUCSMapper<CharSet::UTF_32LE> : UCSMapperBase
{
	using UCSMapperBase::Assign;

	//! \brief 解码： UTF-32LE 。
	template<typename _tObj, typename _tIn, typename _tState>
	static ConversionResult
	Decode(_tObj& uc, _tIn&& i, _tState&& st)
		ynoexcept(noexcept(GetSequenceOf(st))
		&& noexcept(GetIndexOf(st)) && noexcept(!FillByte(i, st)))
	{
		const auto seq(GetSequenceOf(st));

		switch(GetIndexOf(st))
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
			Assign(uc, ucs4_t(seq[2]) | ucs4_t(seq[3]) << 8U
				| ucs4_t(seq[0]) << 16U | ucs4_t(seq[1]) << 24U);
			break;
		default:
			return ConversionResult::BadState;
		}
		return ConversionResult::OK;
	}
};
//@}


//! \brief 取映射函数。
//@{
template<Encoding, typename... _tParams>
yconstfn ConversionResult
UCSMapper_Decode(_tParams&&...) ynothrow
{
	return ConversionResult::Unhandled;
}
template<Encoding _vEnc, typename _tDst, typename _tSrc, typename _tState>
yconstfn ConversionResult
UCSMapper_Decode(_tDst&& d, _tSrc&& s, _tState&& st, decltype(
	&GUCSMapper<_vEnc>::template Decode<_tDst, _tSrc, _tState>) = {})
	ynoexcept_spec(GUCSMapper<_vEnc>::Decode(d, s, st))
{
	return GUCSMapper<_vEnc>::Decode(d, s, st);
}

template<Encoding _vEnc, typename _tDst, typename _tSrc>
yconstfn byte
UCSMapper_Encode(_tDst, _tSrc) ynothrow
{
	return 0;
}
template<Encoding _vEnc, typename _tDst>
yconstfn byte
UCSMapper_Encode(_tDst&& d, const ucs2_t& s,
	decltype(&GUCSMapper<_vEnc>::template Encode<_tDst>) = {})
	ynoexcept_spec(GUCSMapper<_vEnc>::Encode(d, s))
{
	return GUCSMapper<_vEnc>::Encode(d, s);
}

template<Encoding _vEnc, typename _tIn, typename _tState>
yconstexpr ConversionResult
UCSMapper(ucs2_t& uc, _tIn&& i, _tState&& st)
	ynoexcept_spec(CHRLib::UCSMapper_Decode<_vEnc>(uc, i, std::move(st)))
{
	return CHRLib::UCSMapper_Decode<_vEnc>(uc, i, std::move(st));
}
template<Encoding _vEnc, typename _tIn, typename _tState>
yconstexpr ConversionResult
UCSMapper(_tIn&& i, _tState&& st)
	ynoexcept_spec(CHRLib::UCSMapper_Decode<_vEnc>(ystdex::pseudo_output(), i, st))
{
	return CHRLib::UCSMapper_Decode<_vEnc>(ystdex::pseudo_output(), i, st);
}
template<Encoding _vEnc>
byte
UCSMapper(char* d, const ucs2_t& s)
	ynoexcept_spec(CHRLib::UCSMapper_Encode<_vEnc>(d, s))
{
	yconstraint(d);
	return CHRLib::UCSMapper_Encode<_vEnc>(d, s);
}
//@}
//@}

} // namespace CHRLib;

#endif

