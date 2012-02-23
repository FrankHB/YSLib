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
\version r1646;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-17 17:52:35 +0800;
\par 修改时间:
	2012-02-21 15:02 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	CHRLib::CharacterProcessing;
*/


#ifndef CHRLIB_INC_CHRPROC_H_
#define CHRLIB_INC_CHRPROC_H_

#include "chrmap.h"
#include <cstdio>
#include <memory> // for std::move;

CHRLIB_BEGIN

//
/*
char*			U2A(char*);
size_t			UTF16toUTF8(byte*, const u16*);
size_t			UTF8toUTF16(u16*, const byte*);
bool			ustrcmp(char* tgr, char* src);
byte*		str_value(size_t v);
*/

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
\since build 273 。
*/
//@{
ConversionResult
MBCToUC(ucs2_t&, const char*&, const Encoding&,
	ConversionState&& = ConversionState());
inline ConversionResult
MBCToUC(ucs2_t& uc, const char*& c, const Encoding& cp, ConversionState& st)
{
	return MBCToUC(uc, c, cp, std::move(st));
}
//@}
/*!
\brief 按指定编码和转换状态转换字符流中字符为 UCS-2 字符，返回转换的字节数。
\since build 273 。
*/
//@{
ConversionResult
MBCToUC(ucs2_t&, std::FILE*, const Encoding&,
	ConversionState&& = ConversionState());
inline ConversionResult
MBCToUC(ucs2_t& uc, std::FILE* fp, const Encoding& cp, ConversionState& st)
{
	return MBCToUC(uc, fp, cp, std::move(st));
}
//@}
/*!
\brief 按指定编码和转换状态返回转换字符为 UCS-2 字符的字节数。
\since build 273 。
*/
//@{
ConversionResult
MBCToUC(const char*&, const Encoding&, ConversionState&& = ConversionState());
inline ConversionResult
MBCToUC(const char*& c, const Encoding& cp, ConversionState& st)
{
	return MBCToUC(c, cp, std::move(st));
}
ConversionResult
MBCToUC(std::FILE*, const Encoding&, ConversionState&& = ConversionState());
inline ConversionResult
MBCToUC(std::FILE* fp, const Encoding& cp, ConversionState& st)
{
	return MBCToUC(fp, cp, std::move(st));
}
//@}

/*!
\brief 按指定编码转换 UCS-2 字符中字符为字符串表示的多字节字符，返回转换的字节数。
*/
byte
UCToMBC(char*, const ucs2_t&, const Encoding&);


/*!
\brief 按指定编码转换 MBCS 字符串为 UTF-16LE 字符串，返回转换的串长。
*/
size_t
MBCSToUCS2(ucs2_t*, const char*, const Encoding& = CS_Default);

/*!
\brief 按指定编码转换 UTF-16LE 字符串为 MBCS 字符串，返回转换的串长。
*/
size_t
UCS2ToMBCS(char*, const ucs2_t*, const Encoding& = CS_Default);

/*!
\brief 转换 UCS-4 字符串为 UCS-2 字符串，返回转换的串长。
*/
size_t
UCS4ToUCS2(ucs2_t*, const ucs4_t*);


/*!
\brief 复制 MBCS 字符串为 UCS-2/UTF-16LE 字符串。
\note 空间由 std::free 释放。
*/
ucs2_t*
ucsdup(const char*, const Encoding& = CS_Default);
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

