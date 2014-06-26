﻿/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file chrproc.h
\ingroup CHRLib
\brief 字符编码处理。
\version r934
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-17 17:52:35 +0800
\par 修改时间:
	2014-06-26 08:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::CharacterProcessing
*/


#ifndef INC_CHRLib_chrproc_h_
#define INC_CHRLib_chrproc_h_ 1

#include "YModules.h"
#include YFM_CHRLib_CharacterMapping
#include <cstdio> // for std::FILE;
#include <memory> // for std::move;
#include <ystdex/string.hpp> // for ystdex::string_traits;
#include <algorithm> // for std::copy_n;

namespace CHRLib
{

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
	static_assert(std::is_integral<_tChar>::value, "Invalid type found.");

	return c & 0x7F;
}


/*!
\brief 按指定编码和转换状态转换字符串中字符为 UCS-2 字符，返回转换的字节数。
\since build 291
*/
//@{
YF_API ConversionResult
MBCToUC(ucs2_t&, const char*&, Encoding, ConversionState&& = {});
inline PDefH(ConversionResult, MBCToUC, ucs2_t& uc, const char*& c,
	Encoding enc, ConversionState& st)
	ImplRet(MBCToUC(uc, c, enc, std::move(st)))
//@}
/*!
\brief 按指定编码和转换状态转换字符流中字符为 UCS-2 字符，返回转换的字节数。
\pre 断言：指针参数非空。
\since build 291
*/
//@{
YF_API ConversionResult
MBCToUC(ucs2_t&, std::FILE*, Encoding, ConversionState&& = {});
inline PDefH(ConversionResult, MBCToUC, ucs2_t& uc, std::FILE* fp, Encoding enc,
	ConversionState& st)
	ImplRet(MBCToUC(uc, fp, enc, std::move(st)))
//@}
/*!
\brief 按指定编码和转换状态返回转换字符为 UCS-2 字符的字节数。
\since build 291
*/
//@{
YF_API ConversionResult
MBCToUC(const char*&, Encoding, ConversionState&& = {});
inline PDefH(ConversionResult, MBCToUC, const char*& c, Encoding enc,
	ConversionState& st)
	ImplRet(MBCToUC(c, enc, std::move(st)))
//! \pre 断言：指针参数非空。
//@{
YF_API ConversionResult
MBCToUC(std::FILE*, Encoding, ConversionState&& = {});
inline PDefH(ConversionResult, MBCToUC, std::FILE* fp, Encoding enc,
	ConversionState& st)
	ImplRet(MBCToUC(fp, enc, std::move(st)))
//@}
//@}

/*!
\brief 按指定编码转换 UCS-2 字符为字符串表示的多字节字符，返回转换的字节数。
\pre 断言： 指针参数非空 。
\since build 305
*/
YF_API size_t
UCToMBC(char*, const ucs2_t&, Encoding);


//! \note 编码字节序同实现的 ucs2_t 存储字节序。
//@{
/*!
\brief 按指定编码转换 MBCS 字符串为 UCS-2 字符串，返回转换的串长。
\pre 断言： 指针参数非空 。
\since build 291
*/
YF_API size_t
MBCSToUCS2(ucs2_t*, const char*, Encoding = CS_Default);

/*!
\brief 按指定编码转换 UCS-2 字符串为 MBCS 字符串，返回转换的串长。
\pre 断言： 指针参数非空 。
\since build 291
*/
YF_API size_t
UCS2ToMBCS(char*, const ucs2_t*, Encoding = CS_Default);

/*!
\brief 转换 UCS-4 字符串为 UCS-2 字符串，返回转换的串长。
\pre 断言： 指针参数非空 。
*/
YF_API size_t
UCS4ToUCS2(ucs2_t*, const ucs4_t*);

/*!
\brief 取 UCS-2 字符串转换的指定编码的多字节字符串。
\since build 305
*/
template<class _tDst, class _tSrc>
_tDst
GetMBCSOf(const _tSrc& src, Encoding enc = CS_Default)
{
	// FIXME: size for max MBC sequence length > 4;
	_tDst str(src.length() << 2,
		typename ystdex::string_traits<_tDst>::value_type());

	str.resize(UCS2ToMBCS(&str[0], src.c_str(), enc));
	return str;
}


/*!
\pre 输入字符串最大多字节不超过 <tt>sizeof(ucsint_t)</tt> 字节。
\since build 475
*/
//@{
//! \brief 复制 UCS-2 字符串为多字节字符串。
//@{
//! \pre 断言： 指针参数非空 。
YF_API std::string
strdup(const ucs2_t*, Encoding = CS_Default);
template<class _tString>
std::string
strdup(const _tString& str)
{
	return CHRLib::strdup(str.c_str());
}
//@}

//! \since build 475
//@{
//! \pre 断言： 指针参数非空 。
//@{
//! \brief 复制多字节字符串为 UCS-2 字符串。
YF_API std::basic_string<ucs2_t>
ucsdup(const char*, Encoding = CS_Default);
//! \brief 复制 UCS-2 字符串。
YF_API std::basic_string<ucs2_t>
ucsdup(const ucs2_t*);
//! \brief 复制 UCS-4 字符串为 UCS-2 字符串。
YF_API std::basic_string<ucs2_t>
ucsdup(const ucs4_t*);
//@}
template<class _tString>
std::basic_string<ucs2_t>
ucsdup(const _tString& str)
{
	return CHRLib::ucsdup(str.c_str());
}
//@}
//@}


//! \since build 402
//@{
//! \brief 复制指定编码的多字节字符串为指定类型的 UCS-2 字符串。
template<class _tDst>
_tDst
MakeUCS2LEString(const char* s, Encoding enc = CS_Default)
{
	yconstraint(s);

	_tDst str(std::char_traits<char>::length(s),
		typename ystdex::string_traits<_tDst>::value_type());

	str.resize(MBCSToUCS2(&str[0], s, enc));
	return str;
}
//! \brief 复制指定类型的 UCS-2 字符串。
template<class _tDst>
_tDst
MakeUCS2LEString(const ucs2_t* s, Encoding = CharSet::ISO_10646_UCS_2)
{
	yconstraint(s);

	_tDst str(std::char_traits<ucs2_t>::length(s),
		typename ystdex::string_traits<_tDst>::value_type());
	using std::begin;

	std::copy_n(s, str.size(), begin(str));
	return str;
}
//! \brief 复制 UCS-4 字符串为指定类型的 UCS-2 字符串。
template<class _tDst>
_tDst
MakeUCS2LEString(const ucs4_t* s, Encoding = CharSet::ISO_10646_UCS_4)
{
	yconstraint(s);

	_tDst str(std::char_traits<ucs4_t>::length(s),
		typename ystdex::string_traits<_tDst>::value_type());

	str.resize(UCS4ToUCS2(&str[0], s));
	return str;
}
//@}
//@}

} // namespace CHRLib;

#endif

