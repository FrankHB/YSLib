/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file chrproc.cpp
\ingroup CHRLib
\brief 字符编码处理。
\version r718
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-17 17:52:35 +0800
\par 修改时间:
	2012-09-04 12:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::CharacterProcessing
*/


#ifndef CHRLIB_INC_CHRPROC_H_
#define CHRLIB_INC_CHRPROC_H_ 1

#include "chrmap.h"
#include <cstdio>
#include <memory> // for std::move;
#include <ystdex/string.hpp> // for ystdex::string_traits;

CHRLIB_BEGIN

#if 0
size_t	UTF16toUTF8(byte*, const u16*);
size_t	UTF8toUTF16(u16*, const byte*);
bool	u8cmp(char*, char*);
#endif

/*!
\brief 判断整数类型字符是否为 ASCII 字符。
\note 截取低 7 位。
*/
template<typename _tChar>
yconstfn bool
IsASCII(_tChar c)
{
	return !(c & ~0x7F);
}

/*!
\brief 任意整数类型字符转换为 ASCII 字符。
\note 截取低 7 位。
*/
template<typename _tChar>
yconstfn char
ToASCII(_tChar c)
{
	return c & 0x7F;
}


/*!
\brief 按指定编码和转换状态转换字符串中字符为 UCS-2 字符，返回转换的字节数。
\since build 291
*/
//@{
ConversionResult
MBCToUC(ucs2_t&, const char*&, Encoding, ConversionState&& = ConversionState());
inline ConversionResult
MBCToUC(ucs2_t& uc, const char*& c, Encoding enc, ConversionState& st)
{
	return MBCToUC(uc, c, enc, std::move(st));
}
//@}
/*!
\brief 按指定编码和转换状态转换字符流中字符为 UCS-2 字符，返回转换的字节数。
\since build 291
*/
//@{
ConversionResult
MBCToUC(ucs2_t&, std::FILE*, Encoding, ConversionState&& = ConversionState());
inline ConversionResult
MBCToUC(ucs2_t& uc, std::FILE* fp, Encoding enc, ConversionState& st)
{
	return MBCToUC(uc, fp, enc, std::move(st));
}
//@}
/*!
\brief 按指定编码和转换状态返回转换字符为 UCS-2 字符的字节数。
\since build 291
*/
//@{
ConversionResult
MBCToUC(const char*&, Encoding, ConversionState&& = ConversionState());
inline ConversionResult
MBCToUC(const char*& c, Encoding enc, ConversionState& st)
{
	return MBCToUC(c, enc, std::move(st));
}
ConversionResult
MBCToUC(std::FILE*, Encoding, ConversionState&& = ConversionState());
inline ConversionResult
MBCToUC(std::FILE* fp, Encoding enc, ConversionState& st)
{
	return MBCToUC(fp, enc, std::move(st));
}
//@}

/*!
\brief 按指定编码转换 UCS-2 字符中字符为字符串表示的多字节字符，返回转换的字节数。
\since build 305
*/
size_t
UCToMBC(char*, const ucs2_t&, Encoding);


/*!
\brief 按指定编码转换 MBCS 字符串为 UTF-16LE 字符串，返回转换的串长。
\since build 291
*/
size_t
MBCSToUCS2(ucs2_t*, const char*, Encoding = CS_Default);

/*!
\brief 按指定编码转换 UTF-16LE 字符串为 MBCS 字符串，返回转换的串长。
\since build 291
*/
size_t
UCS2ToMBCS(char*, const ucs2_t*, Encoding = CS_Default);

/*!
\brief 转换 UCS-4 字符串为 UCS-2 字符串，返回转换的串长。
*/
size_t
UCS4ToUCS2(ucs2_t*, const ucs4_t*);

/*!
\brief 取指定编码的多字节字符串。
\since build 305
*/
template<class _tDst, class _tSrc>
_tDst
GetMBCSOf(const _tSrc& src, Encoding enc = CS_Default)
{
	// FIXME: size for max MBC sequence length > 4;
	_tDst str(src.length() << 2,
		typename ystdex::string_traits<_tSrc>::value_type());

	str.resize(UCS2ToMBCS(&str[0], src.c_str(), enc));
	return str;
}


/*!
\brief 复制 UCS-2LE 字符串为多字节字符串。
\note 空间由 std::free 释放。
\since build 305
*/
char*
strdup(const ucs2_t*, Encoding = CS_Default);

/*!
\brief 复制多字节字符串为 UCS-2/UTF-16LE 字符串。
\note 空间由 std::free 释放。
\since build 291
*/
ucs2_t*
ucsdup(const char*, Encoding = CS_Default);
/*!
\brief 复制 UCS-2 字符串。
\note 空间由 std::free 释放。
*/
ucs2_t*
ucsdup(const ucs2_t*);
/*!
\brief 复制 UCS-4 字符串为 UCS-2 字符串。
\note 空间由 std::free 释放。
*/
ucs2_t*
ucsdup(const ucs4_t*);

CHRLIB_END

#endif

