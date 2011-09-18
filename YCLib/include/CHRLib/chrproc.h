/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file chrproc.cpp
\ingroup CHRLib
\brief 字符编码处理。
\version r1503;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-17 17:52:35 +0800;
\par 修改时间:
	2011-09-19 04:32 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	CHRLib::CharacterProcessing;
*/


#ifndef CHRLIB_INC_CHRPROC_H_
#define CHRLIB_INC_CHRPROC_H_

#include "chrdef.h"
#include "chrmap.h"

CHRLIB_BEGIN

//extern char U[255], N[255];

/*
char*			U2A(char*);
usize_t			UTFLen(char*);
unsigned short	WordWidth(char*, CHRSTAT*, const CSID& cp = DefCSID);
usize_t			UTF16toUTF8(ubyte_t*, const u16*);
usize_t			UTF8toUTF16(u16*, const ubyte_t*);
bool			strComp(char* tgr, char* src);
ubyte_t*		ValueStr(usize_t v);
*/

/*!
\brief 判断整数类型字符是否为 ASCII 字符。
*/
template<typename _tChar>
inline bool
IsASCII(_tChar c)
{
	return !(c & ~0x7F);
}

/*!
\brief 计算 UCS-2 字符串长度。
*/
usize_t
ucslen(const uchar_t*);

/*!
\brief 按字典序比较 UCS-2 字符串。
*/
uint_t
ucscmp(const uchar_t*, const uchar_t*);

/*!
\brief 按字典序比较 UCS-2 字符串（忽略大小写）。
*/
uint_t
ucsicmp(const uchar_t*, const uchar_t*);

/*!
\brief 任意整数类型字符转换为 ASCII 字符。
\note 截取低字节。
*/
template<typename _tChar>
inline char
ToASCII(_tChar c)
{
	return c & 0x7F;
}

/*!
\brief 按字符集转换字符串中字符为 UCS-2 字符，返回转换的字节数。
*/
ubyte_t
ToUTF(const char*, uchar_t&, const CSID&);
/*!
\brief 按字符集转换字符流中字符为 UCS-2 字符，返回转换的字节数。
*/
ubyte_t
ToUTF(FILE*, uchar_t&, const CSID&);

/*!
\brief 转换 MBCS 字符串为 ANSI 字符串（非 ANSI 字符替换为第三参数），
\return 转换的串长。
*/
usize_t
MBCSToANSI(char*, const char*, char = ' ');
/*!
\brief 转换 UCS2 字符串为 ANSI 字符串（非 UTF16LE 字符替换为第三参数），
\return 转换的串长。
*/
usize_t
UCS2ToANSI(char*, const uchar_t*, char = ' ');
/*!
\brief 转换 UCS4 字符串为 ANSI 字符串（非 UTF16BE 字符替换为第三参数），
\return 转换的串长。
*/
usize_t
UCS4ToANSI(char*, const fchar_t*, char = ' ');

/*!
\brief 按字符集转换 MBCS 字符串为 UTF-16LE 字符串，返回转换的串长。
*/
usize_t
MBCSToUTF16LE(uchar_t*, const char*, const CSID& = CS_Default);
/*!
\brief 按字符集转换 MBCS 字符串为 UCS-4 字符串，返回转换的串长。
*/
usize_t
MBCSToUCS(fchar_t*, const char*, const CSID& = CS_Default);
/*!
\brief 按字符集转换 UTF-16LE 字符串为 MBCS 字符串，返回转换的串长。
*/
usize_t
UTF16LEToMBCS(char*, const uchar_t*, const CSID& = CS_Default);
/*!
\brief 转换 UCS-4 字符串为 UCS-2 字符串，返回转换的串长。
*/
usize_t
UCS4ToUCS2(uchar_t*, const fchar_t*);

/*!
\brief 复制 MBCS 字符串为 ANSI 字符串（非 ANSI 字符替换为第二参数）。
//注释:	空间由 free() 释放。	
*/
char*
sdup(const char*, char = ' ');
/*!
\brief 复制 UCS-2 字符串为 ANSI 字符串（非 ANSI 字符替换为第二参数）。
\note 空间由 std::free 释放。
*/
char*
sdup(const uchar_t*, char = ' ');
/*!
\brief 复制 UCS-4 字符串为 ANSI 字符串（非 ANSI 字符替换为第二参数）。
\note 空间由 std::free 释放。
*/
char*
sdup(const fchar_t*, char = ' ');

/*!
\brief 复制 MBCS 字符串为 UCS-2/UTF-16LE 字符串。
\note 空间由 std::free 释放。
*/
uchar_t*
ucsdup(const char*, const CSID& = CS_Default);
/*!
\brief 复制 UCS-2 字符串。
\note 空间由 std::free 释放。
*/
uchar_t*
ucsdup(const uchar_t*);
/*!
\brief 复制 UCS-4 字符串为 UCS-2 字符串。
\note 空间由 std::free 释放。
*/
uchar_t*
ucsdup(const fchar_t*);

/*
ubyte_t    IsFullChar(char*);
*/

CHRLIB_END

#endif

