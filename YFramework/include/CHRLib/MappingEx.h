/*
	© 2012-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MappingEx.h
\ingroup CHRLib
\brief 附加编码映射。
\version r420
\author FrankHB <frankhb1989@gmail.com>
\since build 324
\par 创建时间:
	2012-07-09 09:04:36 +0800
\par 修改时间:
	2017-06-05 02:13 +0800
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
\def CHRLib_NoDynamicMapping
\brief 无动态映射：不关联外部文件的编码映射表。
\since build 646
*/
#ifndef CHRLib_NoDynamicMapping
#	define CHRLib_NoDynamicMapping 0
#endif

#if CHRLib_NoDynamicMapping
/*!
\brief 编码转换表。
\since 早于 build 132
*/
//@{
yconstexpr const byte cp17[]{0};
extern "C"
{
	extern const byte cp113[];
	//extern const byte cp2026[13658];
}
yconstexpr const byte cp2026[]{0};
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
\brief 查找编码转换序列的实现。
\note 默认使用编码转换表指针。
\since build 641
*/
extern char16_t(*cp113_lkp)(byte, byte);


/*!
\brief 非 Unicode 编码映射模板特化。
\since build 594
*/
//@{
template<>
struct GUCSMapper<CharSet::SHIFT_JIS> : UCSMapperBase
{
	using UCSMapperBase::Assign;

#if 0
	template<typename _tObj, typename _tIn, typename _tState>
	static byte
	Decode(_tObj&& uc, _tIn&& i, _tState&& st) ynoexcept(noexcept(GetSequenceOf(
		st)) && noexcept(GetIndexOf(st)) && noexcept(!FillByte(i, st)))
	{
		uint_least16_t row(0), col(0), ln(188); // (7E-40 + 1 + FC-80 + 1)
		const auto c(FillByte(i, st));

		if(c >= 0xA1 && c <= 0xC6)
		{
			const auto d(FillByte(i, st));

			row = c - 0xA1 ;
			if(d >= 0x40 && d <= 0x7E)
				col = d - 0x40 ;
			else if(d >= 0xA1 && d <= 0xFE)
				col = d - 0x62;
			Assign(uc, cp17[row * ln + col]);
		}
		else if(c >= 0xC9 && c <= 0xF9)
		{
			const auto d(FillByte(i, st));

			row = c - 0xA3;
			if(d >= 0x40 && d <= 0x7E)
				col = d - 0x40 ;
			else if(d >= 0xA1 && d <= 0xFE)
				col = d - 0x62;
			Assign(uc, cp17[row * ln + col]);
		}
		else if(c < 0x80)
		{
			Assign(uc, c);
			return 1;
		}
		else
			uc = 0xFFFE;
		return 2;
	}
#endif
};

template<>
struct GUCSMapper<CharSet::GBK> : UCSMapperBase
{
	using UCSMapperBase::Assign;

	/*!
	\bug 大端序下输出缺少转换。
	\since build 599
	*/
	template<typename _tObj, typename _tIn, typename _tState>
	static ConversionResult
	Decode(_tObj&& uc, _tIn&& i, _tState&& st) ynoexcept(noexcept(GetSequenceOf(
		st)) && noexcept(GetIndexOf(st)) && noexcept(!FillByte(i, st)))
	{
		yassume(cp113_lkp);

		const auto seq(GetSequenceOf(st));

		switch(GetIndexOf(st))
		{
		case 0:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			if(seq[0] < 0x80)
			{
				Assign(uc, seq[0]);
				break;
			}
			YB_ATTR_fallthrough;
		case 1:
			if(YB_UNLIKELY(!FillByte(i, st)))
				return ConversionResult::BadSource;
			{
				const auto t(cp113_lkp(seq[0], seq[1]));

				if(YB_LIKELY(t != 0))
				{
					Assign(uc, t);
					break;
				}
			}
			return ConversionResult::Unhandled;
		default:
			return ConversionResult::BadState;
		}
		return ConversionResult::OK;
	}
};

template<>
struct GUCSMapper<CharSet::Big5> : UCSMapperBase
{
	using UCSMapperBase::Assign;

#if 0
	template<typename _tObj, typename _tIn, typename _tState>
	static byte
	Decode(_tObj&& uc, _tIn&& i, _tState&& st) ynoexcept(noexcept(GetSequenceOf(
		st)) && noexcept(GetIndexOf(st)) && noexcept(!FillByte(i, st)))
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
			Assign(uc, cp2026[row * ln + col]);
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
			Assign(uc, cp2026[row * ln + col]);
			return 2;
		}
		else if(c < 0x80)
		{
			Assign(uc, c);
			return 1;
		}
		else
			Assign(uc, 0xFFFE);
		return 2;
	}
#endif
};
//@}


// NOTE: See https://llvm.org/bugs/show_bug.cgi?id=25306.
#if YB_IMPL_CLANGPP
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wignored-attributes"
#endif

//! \since build 756
//@{
template<Encoding, typename _tRet, typename... _tParams>
yconstfn YB_STATELESS ystdex::add_ptr_t<_tRet(_tParams...)>
FetchMapperPtr_TryUCSMapper(...) ynothrow
{
	return {};
}
template<Encoding _vEnc, typename _tRet, typename... _tParams>
yconstfn YB_ATTR_returns_nonnull YB_STATELESS
	ystdex::add_ptr_t<_tRet(_tParams...)>
FetchMapperPtr_TryUCSMapper(yimpl(ystdex::enable_if_convertible_t<
	decltype(GUCSMapper<_vEnc>::Decode(std::declval<_tParams>()...)), _tRet>*
	= {})) ynothrow
{
	return GUCSMapper<_vEnc>::Decode;
}
template<Encoding _vEnc, typename _tRet, typename... _tParams>
yconstfn YB_ATTR_returns_nonnull YB_STATELESS
	ystdex::add_ptr_t<_tRet(_tParams...)>
FetchMapperPtr_TryUCSMapper(yimpl(ystdex::enable_if_convertible_t<
	decltype(GUCSMapper<_vEnc>::Encode(std::declval<_tParams>()...)), _tRet,
	int>* = {})) ynothrow
{
	return GUCSMapper<_vEnc>::Encode;
}

#if YB_IMPL_CLANGPP
#	pragma GCC diagnostic pop
#endif

template<typename _tRet, typename... _tParams>
yconstfn YB_STATELESS ystdex::add_ref_t<_tRet(_tParams...)>
FetchMapper_Default() ynothrow
{
	return *FetchMapperPtr_TryUCSMapper<CS_Default, _tRet, _tParams...>(nullptr);
}

//! \brief 取指定编码映射的转换函数指针。
template<typename _tRet, typename... _tParams>
yconstfn_relaxed ystdex::add_ptr_t<_tRet(_tParams...)>
FetchMapperPtr(Encoding enc)
{
	using namespace CharSet;

#define CHR_MapItem(enc) \
case enc: \
	return FetchMapperPtr_TryUCSMapper<enc, _tRet, _tParams...>(nullptr);

	switch(enc)
	{
	CHR_MapItem(SHIFT_JIS)
	CHR_MapItem(UTF_8)
	CHR_MapItem(GBK)
	CHR_MapItem(UTF_16BE)
	CHR_MapItem(UTF_16LE)
	CHR_MapItem(UTF_32BE)
	CHR_MapItem(UTF_32LE)
	CHR_MapItem(Big5)
	default:
		break;
	}
#undef CHR_MapItem
	return {};
}
//@}

} // namespace CHRLib;

#endif

