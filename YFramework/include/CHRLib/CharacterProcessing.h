/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file CharacterProcessing.h
\ingroup CHRLib
\brief 字符编码处理。
\version r1806
\author FrankHB <frankhb1989@gmail.com>
\since build 565
\par 创建时间:
	2009-11-17 17:52:35 +0800
\par 修改时间:
	2015-10-11 06:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::CharacterProcessing
*/


#ifndef INC_CHRLib_CharacterProcessing_h_
#define INC_CHRLib_CharacterProcessing_h_ 1

#include "YModules.h"
#include YFM_CHRLib_CharacterMapping // for ystdex::is_null,
//	ystdex::ntctslen ystdex::string_traits;
#include <cstdio> // for std::FILE;
#include <memory> // for std::move;

namespace CHRLib
{

//! \since build 630
using ystdex::uchar_t;


/*!
\brief 判断整数类型字符在 ASCII 字符取值范围内。
\note 截取低 7 位。
*/
template<typename _tChar>
yconstfn bool
IsASCII(_tChar c)
{
	return !(c & ~0x7F);
}

/*!
\brief 任意整数类型字符转换为 ASCII 取值范围兼容的字符。
\note 截取低 7 位。
*/
template<typename _tChar>
yconstfn char
ToASCII(_tChar c)
{
	static_assert(std::is_integral<_tChar>(), "Invalid type found.");

	return c & 0x7F;
}


//! \return 转换的字节数。
//@{
//! \since build 641
//@{
//! \brief 按指定编码和转换状态转换字符串中的字符为 UCS-2 字符。
//@{
YF_API ConversionResult
MBCToUC(char16_t&, const char*&, Encoding, ConversionState&& = {});
YF_API ConversionResult
MBCToUC(char16_t&, const char*&, const char*, Encoding, ConversionState&& = {});
inline PDefH(ConversionResult, MBCToUC, char16_t& uc, const char*& c,
	Encoding enc, ConversionState& st)
	ImplRet(MBCToUC(uc, c, enc, std::move(st)))
//@}
/*!
\brief 按指定编码和转换状态转换字符流中的字符为 UCS-2 字符。
\pre 断言：指针参数非空。
*/
//@{
YF_API YB_NONNULL(2) ConversionResult
MBCToUC(char16_t&, std::FILE*, Encoding, ConversionState&& = {});
inline PDefH(ConversionResult, MBCToUC, char16_t& uc, std::FILE* fp, Encoding enc,
	ConversionState& st)
	ImplRet(MBCToUC(uc, fp, enc, std::move(st)))
//@}
//@}
/*!
\brief 按指定编码和转换状态返回转换字符为 UCS-2 字符的字节数。
\since build 291
*/
//@{
YF_API ConversionResult
MBCToUC(const char*&, Encoding, ConversionState&& = {});
//! \since build 614
YF_API ConversionResult
MBCToUC(const char*&, const char*, Encoding, ConversionState&& = {});
inline PDefH(ConversionResult, MBCToUC, const char*& c, Encoding enc,
	ConversionState& st)
	ImplRet(MBCToUC(c, enc, std::move(st)))
//! \since build 614
inline PDefH(ConversionResult, MBCToUC, const char*& c, const char* e,
	Encoding enc, ConversionState& st)
	ImplRet(MBCToUC(c, e, enc, std::move(st)))
//! \pre 断言：指针参数非空。
//@{
YF_API YB_NONNULL(1) ConversionResult
MBCToUC(std::FILE*, Encoding, ConversionState&& = {});
inline YB_NONNULL(1) PDefH(ConversionResult, MBCToUC, std::FILE* fp,
	Encoding enc, ConversionState& st)
	ImplRet(MBCToUC(fp, enc, std::move(st)))
//@}
//@}

/*!
\brief 按指定编码转换 UCS-2 字符为字符串表示的多字节字符。
\pre 断言：指针参数非空 。
\pre 第一参数指向的缓冲区能容纳转换后字符序列。
\since build 641
*/
YF_API YB_NONNULL(1) size_t
UCToMBC(char*, const char16_t&, Encoding);
//@}


//! \note 编码字节序同实现的 char16_t 存储字节序。
//@{
//! \since build 644
//@{
template<typename _func, typename _tDst, typename _tSrc>
YB_NONNULL(2, 3) size_t
DecodeWith(_func f, _tDst* d, const _tSrc* s)
{
	yconstraint(d),
	yconstraint(s);

	const auto p(d);

	while(!ystdex::is_null(*s) && f(*d, s))
		++d;
	*d = _tDst();
	return size_t(d - p);
}
template<typename _func, typename _tDst, typename _tSrc>
YB_NONNULL(2, 3, 4) size_t
DecodeWith(_func f, _tDst* d, const _tSrc* s, const _tSrc* e)
{
	yconstraint(d),
	yconstraint(s),
	yconstraint(e),
	yconstraint(s <= e);

	const auto p(d);

	while(!ystdex::is_null(*s) && f(*d, s, e))
		++d;
	return size_t(d - p);
}

template<typename _func, typename _tDst, typename _tSrc>
YB_NONNULL(2, 3) size_t
EncodeWith(_func f, _tDst* d, const _tSrc* s)
{
	yconstraint(d),
	yconstraint(s);

	const auto p(d);

	f(d, s);
	*d = _tDst();
	return size_t(d - p);
}
template<typename _func, typename _tDst, typename _tSrc>
YB_NONNULL(2, 3, 4) size_t
EncodeWith(_func f, _tDst* d, const _tSrc* s, const _tSrc* e)
{
	yconstraint(d),
	yconstraint(s),
	yconstraint(e),
	yconstraint(s <= e);

	const auto p(d);

	f(d, s, e);
	return size_t(d - p);
}
//@}

/*!
\pre 断言：指针参数非空 。
\pre 第一参数指向的缓冲区能容纳转换后的 NTCTS （包括结尾的空字符）。
\pre 指针参数指向的缓冲区不重叠。
\return 转换的串长。
\since build 641
*/
//@{
//! \brief 按指定编码转换 MBCS 字符串为 UCS-2 字符串。
//@{
//! \since build 644
//@{
template<typename _func, typename _tState = ConversionState>
YB_NONNULL(2, 3) size_t
MBCSToUCS2(_func f, char16_t* d, const char* s, _tState&& st = _tState())
{
	return DecodeWith([&](char16_t& dc, const char*& src) -> bool{
		return f(dc, src, yforward(st)) == ConversionResult::OK;
	}, d, s);
}
template<typename _func, typename _tState = ConversionState>
YB_NONNULL(2, 3, 4) size_t
MBCSToUCS2(_func f, char16_t* d, const char* s, const char* e,
	_tState&& st = _tState())
{
	return DecodeWith(
		[&](char16_t& dc, const char*& src, const char*& end) -> bool{
		return f(dc, {src, end}, yforward(st)) == ConversionResult::OK;
	}, d, s, e);
}
//@}
YF_API YB_NONNULL(1, 2) size_t
MBCSToUCS2(char16_t*, const char*, Encoding = CS_Default);
YF_API YB_NONNULL(1, 2, 3) size_t
MBCSToUCS2(char16_t*, const char*, const char* e, Encoding = CS_Default);
//@}

//! \brief 按指定编码转换 MBCS 字符串为 UCS-4 字符串。
//@{
//! \since build 644
//@{
template<typename _func, typename _tState = ConversionState>
YB_NONNULL(2, 3) size_t
MBCSToUCS4(_func f, char32_t* d, const char* s, _tState&& st = _tState())
{
	return DecodeWith([&](char32_t& dc, const char*& src) -> bool{
		// TODO: Necessary initialization?
		char16_t c;

		if(f(c, src, yforward(st)) == ConversionResult::OK)
		{
			dc = c;
			return true;
		}
		return {};
	}, d, s);
}
template<typename _func, typename _tState = ConversionState>
YB_NONNULL(2, 3, 4) size_t
MBCSToUCS4(_func f, char32_t* d, const char* s, const char* e,
	_tState&& st = _tState())
{
	return DecodeWith(
		[&](char32_t& dc, const char*& src, const char*& end) -> bool{
		// TODO: Necessary initialization?
		char16_t c;

		if(f(c, {src, end}, yforward(st)) == ConversionResult::OK)
		{
			dc = c;
			return true;
		}
		return {};
	}, d, s, e);
}
//@}
YF_API YB_NONNULL(1, 2) size_t
MBCSToUCS4(char32_t*, const char*, Encoding = CS_Default);
YF_API YB_NONNULL(1, 2, 3) size_t
MBCSToUCS4(char32_t*, const char*, const char*, Encoding = CS_Default);
//@}

//! \brief 按指定编码转换 UCS-2 字符串为 MBCS 字符串。
//@{
//! \since build 644
//@{
template<typename _func>
YB_NONNULL(2, 3) size_t
UCS2ToMBCS(_func f, char* d, const char16_t* s)
{
	return EncodeWith([&](char*& dst, const char16_t*& src){
		while(!ystdex::is_null(*src))
			dst += f(dst, *src++);
	}, d, s);
}
template<typename _func>
YB_NONNULL(2, 3, 4) size_t
UCS2ToMBCS(_func f, char* d, const char16_t* s, const char16_t* e)
{
	return EncodeWith(
		[&](char*& dst, const char16_t*& src, const char16_t* end){
		// TODO: Deferred. Use guard for encoding.
		while(src < end)
			dst += f(dst, *src++);
	}, d, s, e);
}
//@}
YF_API YB_NONNULL(1, 2) size_t
UCS2ToMBCS(char*, const char16_t*, Encoding = CS_Default);
YF_API YB_NONNULL(1, 2, 3) size_t
UCS2ToMBCS(char*, const char16_t*, const char16_t*, Encoding = CS_Default);
//@}

//! \brief 转换 UCS-2 字符串为 UCS-4 字符串。
//@{
YF_API YB_NONNULL(1, 2) size_t
UCS2ToUCS4(char32_t*, const char16_t*);
YF_API YB_NONNULL(1, 2, 3) size_t
UCS2ToUCS4(char32_t*, const char16_t*, const char16_t*);
//@}

//! \brief 按指定编码转换 UCS-4 字符串为 MBCS 字符串。
//@{
//! \since build 644
//@{
template<typename _func>
YB_NONNULL(2, 3) size_t
UCS4ToMBCS(_func f, char* d, const char32_t* s)
{
	return EncodeWith([&](char*& dst, const char32_t*& src){
		while(!ystdex::is_null(*src))
			dst += f(dst, char16_t(*src++));
	}, d, s);
}
template<typename _func>
YB_NONNULL(2, 3, 4) size_t
UCS4ToMBCS(_func f, char* d, const char32_t* s, const char32_t* e)
{
	return EncodeWith(
		[&](char*& dst, const char32_t*& src, const char32_t* end){
		// TODO: Deferred. Use guard for encoding.
		while(src < end)
			dst += f(dst, char16_t(*src++));
	}, d, s, e);
}
//@}
YF_API YB_NONNULL(1, 2) size_t
UCS4ToMBCS(char*, const char32_t*, Encoding = CS_Default);
YF_API YB_NONNULL(1, 2, 3) size_t
UCS4ToMBCS(char*, const char32_t*, const char32_t*, Encoding = CS_Default);
//@}

//! \brief 转换 UCS-4 字符串为 UCS-2 字符串。
//@{
YF_API YB_NONNULL(1, 2) size_t
UCS4ToUCS2(char16_t*, const char32_t*);
YF_API YB_NONNULL(1, 2, 3) size_t
UCS4ToUCS2(char16_t*, const char32_t*, const char32_t*);
//@}
//@}


/*!
\pre 输入字符串的每个字符不超过 \c sizeof(ucsint_t) 字节。
\pre 断言：指针参数非空。
\since build 641
*/
//@{
//! \brief 转换指定编码的多字节字符串为指定类型的 UCS-2 字符串。
//@{
//! \since build 594
template<class _tDst = std::basic_string<char16_t>>
YB_NONNULL(1) _tDst
MakeUCS2LE(const char* s, Encoding enc = CS_Default)
{
	_tDst str(ystdex::ntctslen(s),
		typename ystdex::string_traits<_tDst>::value_type());

	str.resize(MBCSToUCS2(&str[0], s, enc));
	return str;
}
template<class _tDst = std::basic_string<char16_t>>
_tDst
MakeUCS2LE(string_view sv, Encoding enc = CS_Default)
{
	const auto s(sv.data());

	yconstraint(s);

	const auto l(sv.length());
	_tDst str(l, typename ystdex::string_traits<_tDst>::value_type());

	str.resize(MBCSToUCS2(&str[0], s, s + l, enc));
	return str;
}
//@}
//! \brief 构造指定类型的 UCS-2 字符串。
template<class _tDst = std::basic_string<char16_t>>
inline _tDst
MakeUCS2LE(u16string_view sv, Encoding = CharSet::ISO_10646_UCS_2)
{
	const auto s(sv.data());

	yconstraint(s);
	// FIXME: Correct conversion for encoding other than UCS-2LE.
	return {s, sv.length()};
}
//! \brief 转换 UCS-4 字符串为指定类型的 UCS-2 字符串。
//@{
template<class _tDst = std::basic_string<char16_t>>
YB_NONNULL(1) _tDst
MakeUCS2LE(const char32_t* s, Encoding = CharSet::ISO_10646_UCS_4)
{
	_tDst str(ystdex::ntctslen(s),
		typename ystdex::string_traits<_tDst>::value_type());

	str.resize(UCS4ToUCS2(&str[0], s));
	return str;
}
template<class _tDst = std::basic_string<char16_t>>
_tDst
MakeUCS2LE(u32string_view sv, Encoding = CharSet::ISO_10646_UCS_4)
{
	const auto s(sv.data());

	yconstraint(s);

	const auto l(sv.length());
	_tDst str(l, typename ystdex::string_traits<_tDst>::value_type());

	str.resize(UCS4ToUCS2(&str[0], s, s + l));
	return str;
}
//@}

//! \brief 转换指定编码的多字节字符串为指定类型的 UCS-4 字符串。
//@{
//! \since build 594
template<class _tDst = std::basic_string<char32_t>>
YB_NONNULL(1) _tDst
MakeUCS4LE(const char* s, Encoding enc = CS_Default)
{
	_tDst str(ystdex::ntctslen(s),
		typename ystdex::string_traits<_tDst>::value_type());

	str.resize(MBCSToUCS4(&str[0], s, enc));
	return str;
}
template<class _tDst = std::basic_string<char32_t>>
_tDst
MakeUCS4LE(string_view sv, Encoding enc = CS_Default)
{
	const auto s(sv.data());

	yconstraint(s);

	const auto l(sv.length());
	_tDst str(l, typename ystdex::string_traits<_tDst>::value_type());

	str.resize(MBCSToUCS4(&str[0], s, s + l, enc));
	return str;
}
//@}
//! \brief 转换 UCS-2 字符串为指定类型的 UCS-4 字符串。
//@{
template<class _tDst = std::basic_string<char32_t>>
YB_NONNULL(1) _tDst
MakeUCS4LE(const char16_t* s, Encoding = CharSet::ISO_10646_UCS_2)
{
	_tDst str(ystdex::ntctslen(s),
		typename ystdex::string_traits<_tDst>::value_type());

	str.resize(UCS2ToUCS4(&str[0], s));
	return str;
}
template<class _tDst = std::basic_string<char32_t>>
_tDst
MakeUCS4LE(u16string_view sv, Encoding = CharSet::ISO_10646_UCS_2)
{
	const auto s(sv.data());

	yconstraint(s);

	const auto l(sv.length());
	_tDst str(l, typename ystdex::string_traits<_tDst>::value_type());

	str.resize(UCS2ToUCS4(&str[0], s, s + l));
	return str;
}
//@}
//! \brief 构造指定类型的 UCS-4 字符串。
template<class _tDst = std::basic_string<char32_t>>
inline _tDst
MakeUCS4LE(u32string_view sv, Encoding = CharSet::ISO_10646_UCS_4)
{
	const auto s(sv.data());

	yconstraint(s);
	// FIXME: Correct conversion for encoding other than UCS-4LE.
	return {s, sv.length()};
}

//@{
//! \brief 转换 UTF-8 字符串为指定编码的多字节字符串。
//@{
//! \since build 544
template<class _tDst = std::string>
inline YB_NONNULL(1) _tDst
MakeMBCS(const char* s, Encoding enc)
{
	return enc = CS_Default ? MakeMBCS<_tDst>(s)
		: MakeMBCS<_tDst>(MakeUCS2LE(s, CS_Default), enc);
}
template<class _tDst = std::string>
inline _tDst
MakeMBCS(string_view sv, Encoding enc)
{
	return enc = CS_Default ? MakeMBCS<_tDst>(sv)
		: MakeMBCS<_tDst>(MakeUCS2LE(sv, CS_Default), enc);
}
//@}
//! \brief 转换 UCS-2LE 字符串为指定编码的多字节字符串。
//@{
template<class _tDst = std::string>
YB_NONNULL(1) _tDst
MakeMBCS(const char16_t* s, Encoding enc = CS_Default)
{
	const auto w(FetchMaxCharWidth(enc));
	_tDst str(ystdex::ntctslen(s) * (w == 0 ? sizeof(ucsint_t) : w),
		typename ystdex::string_traits<_tDst>::value_type());

	str.resize(UCS2ToMBCS(&str[0], s, enc));
	return str;
}
template<class _tDst = std::string>
_tDst
MakeMBCS(u16string_view sv, Encoding enc = CS_Default)
{
	const auto s(sv.data());

	yconstraint(s);

	const auto l(sv.length());
	const auto w(FetchMaxCharWidth(enc));
	_tDst str(l * (w == 0 ? sizeof(ucsint_t) : w),
		typename ystdex::string_traits<_tDst>::value_type());

	str.resize(UCS2ToMBCS(&str[0], s, s + l, enc));
	return str;
}
//@}
//! \brief 转换 UCS-4LE 字符串为指定编码的多字节字符串。
//@{
template<class _tDst = std::string>
YB_NONNULL(1) _tDst
MakeMBCS(const char32_t* s, Encoding enc = CS_Default)
{
	_tDst str(ystdex::ntctslen(s) * FetchMaxCharWidth(enc),
		typename ystdex::string_traits<_tDst>::value_type());

	str.resize(UCS4ToMBCS(&str[0], s, enc));
	return str;
}
template<class _tDst = std::string>
_tDst
MakeMBCS(u32string_view sv, Encoding enc = CS_Default)
{
	const auto s(sv.data());

	yconstraint(s);

	const auto l(sv.length());
	_tDst str(l * FetchMaxCharWidth(enc),
		typename ystdex::string_traits<_tDst>::value_type());

	str.resize(UCS4ToMBCS(&str[0], s, s + l, enc));
	return str;
}
//@}
//@}
//@}

} // namespace CHRLib;

#endif

