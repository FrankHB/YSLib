// CHRLib -> CharacterProcessing by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-17 17:52:35;
// UTime = 2010-9-10 23:55;
// Version = 0.1446;


#ifndef INCLUDED_CHRPROC_H_
#define INCLUDED_CHRPROC_H_

#include "chrdef.h"
#include "chrmap.h"

// CharacterProcessing ：字符编码处理。

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

//判断是否为 ASCII 字符。
template<typename _charT>
inline bool
IsASCII(_charT c)
{
	return !(c & ~0x7F);
}

//计算 UCS-2 字符串长度。
usize_t
ucslen(const uchar_t*);

//逐字节比较 UCS-2 字符串。
uint_t
ucscmp(const uchar_t*, const uchar_t*);

//逐字节比较 UCS-2 字符串（忽略大小写）。
uint_t
ucsicmp(const uchar_t*, const uchar_t*);

//字符转换为 ASCII 字符（截取低字节）。
template<typename _charT>
inline char
ToASCII(_charT c)
{
	return c & 0x7F;
}

//按字符集转换字符串中字符为 UCS-2 字符，返回转换的字节数。
ubyte_t
ToUTF(const char*, uchar_t&, const CSID&);
//按字符集转换字符流中字符为 UCS-2 字符，返回转换的字节数。
ubyte_t
ToUTF(FILE*, uchar_t&, const CSID&);

//转换 MBCS 字符串为 ANSI 字符串（非 ANSI 字符替换为第三参数），返回转换的串长。
usize_t
MBCSToANSI(char*, const char*, char = ' ');
//转换 UCS2 字符串为 ANSI 字符串（非 UTF16LE 字符替换为第三参数），返回转换的串长。
usize_t
UCS2ToANSI(char*, const uchar_t*, char = ' ');
//转换 UCS4 字符串为 ANSI 字符串（非 UTF16BE 字符替换为第三参数），返回转换的串长。
usize_t
UCS4ToANSI(char*, const wchar_t*, char = ' ');

//按字符集转换 MBCS 字符串为 UTF-16LE 字符串，返回转换的串长。
usize_t
MBCSToUTF16LE(uchar_t*, const char*, const CSID& = CS_Default);
//按字符集转换 MBCS 字符串为 UCS-4 字符串，返回转换的串长。
usize_t
MBCSToUCS(wchar_t*, const char*, const CSID& = CS_Default);
//按字符集转换 UTF-16LE 字符串为 MBCS 字符串，返回转换的串长。
usize_t
UTF16LEToMBCS(char*, const uchar_t*, const CSID& = CS_Default);
//转换 UCS-4 字符串为 UCS-2 字符串，返回转换的串长。
usize_t
UCS4ToUCS2(uchar_t*, const wchar_t*);

//复制 MBCS 字符串为 ANSI 字符串（非 ANSI 字符替换为第二参数）。空间由 free() 释放。
char*
sdup(const char*, char = ' ');
//复制 UCS-2 字符串为 ANSI 字符串（非 ANSI 字符替换为第二参数）。空间由 free() 释放。
char*
sdup(const uchar_t*, char = ' ');
//复制 UCS-4 字符串为 ANSI 字符串（非 ANSI 字符替换为第二参数）。空间由 free() 释放。
char*
sdup(const wchar_t*, char = ' ');

//复制 MBCS 字符串为 UCS-2/UTF-16LE 字符串。空间由 free() 释放。
uchar_t*
ucsdup(const char*, const CSID& = CS_Default);
//复制 UCS-2 字符串。空间由 free() 释放。
uchar_t*
ucsdup(const uchar_t*);
//复制 UCS-4 字符串为 UCS-2 字符串。空间由 free() 释放。
uchar_t*
ucsdup(const wchar_t*);

/*
ubyte_t    IsFullChar(char*);
*/

CHRLIB_END

#endif

