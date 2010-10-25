// CHRLib -> CharacterProcessing by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-17 17:52:35 + 08:00;
// UTime = 2010-10-24 13:45 + 08:00;
// Version = 0.1476;


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

//********************************
//名称:		IsASCII
//全名:		CHRLib::IsASCII<_tChar>
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	_tChar c
//功能概要:	判断整数类型字符是否为 ASCII 字符。
//备注:		
//********************************
template<typename _tChar>
inline bool
IsASCII(_tChar c)
{
	return !(c & ~0x7F);
}

//********************************
//名称:		ucslen
//全名:		CHRLib::ucslen
//可访问性:	public 
//返回类型:	CHRLib::usize_t
//修饰符:	
//形式参数:	const uchar_t *
//功能概要:	计算 UCS-2 字符串长度。
//备注:		
//********************************
usize_t
ucslen(const uchar_t*);

//********************************
//名称:		ucscmp
//全名:		CHRLib::ucscmp
//可访问性:	public 
//返回类型:	CHRLib::uint_t
//修饰符:	
//形式参数:	const uchar_t *
//形式参数:	const uchar_t *
//功能概要:	按字典序比较 UCS-2 字符串。
//备注:		
//********************************
uint_t
ucscmp(const uchar_t*, const uchar_t*);

//********************************
//名称:		ucsicmp
//全名:		CHRLib::ucsicmp
//可访问性:	public 
//返回类型:	CHRLib::uint_t
//修饰符:	
//形式参数:	const uchar_t *
//形式参数:	const uchar_t *
//功能概要:	按字典序比较 UCS-2 字符串（忽略大小写）。
//备注:		
//********************************
uint_t
ucsicmp(const uchar_t*, const uchar_t*);

//********************************
//名称:		ToASCII
//全名:		CHRLib::ToASCII<_tChar>
//可访问性:	public 
//返回类型:	char
//修饰符:	
//形式参数:	_tChar c
//功能概要:	任意整数类型字符转换为 ASCII 字符。
//备注:		截取低字节。
//********************************
template<typename _tChar>
inline char
ToASCII(_tChar c)
{
	return c & 0x7F;
}

//********************************
//名称:		ToUTF
//全名:		CHRLib::ToUTF
//可访问性:	public 
//返回类型:	CHRLib::ubyte_t
//修饰符:	
//形式参数:	const char *
//形式参数:	uchar_t &
//形式参数:	const CSID &
//功能概要:	按字符集转换字符串中字符为 UCS-2 字符，返回转换的字节数。
//备注:		
//********************************
ubyte_t
ToUTF(const char*, uchar_t&, const CSID&);
//********************************
//名称:		ToUTF
//全名:		CHRLib::ToUTF
//可访问性:	public 
//返回类型:	CHRLib::ubyte_t
//修饰符:	
//形式参数:	FILE *
//形式参数:	uchar_t &
//形式参数:	const CSID &
//功能概要:	按字符集转换字符流中字符为 UCS-2 字符，返回转换的字节数。
//备注:		
//********************************
ubyte_t
ToUTF(FILE*, uchar_t&, const CSID&);

//********************************
//名称:		MBCSToANSI
//全名:		CHRLib::MBCSToANSI
//可访问性:	public 
//返回类型:	CHRLib::usize_t
//修饰符:	
//形式参数:	char *
//形式参数:	const char *
//形式参数:	char
//功能概要:	转换 MBCS 字符串为 ANSI 字符串（非 ANSI 字符替换为第三参数），
//			返回转换的串长。
//备注:		
//********************************
usize_t
MBCSToANSI(char*, const char*, char = ' ');
//********************************
//名称:		UCS2ToANSI
//全名:		CHRLib::UCS2ToANSI
//可访问性:	public 
//返回类型:	CHRLib::usize_t
//修饰符:	
//形式参数:	char *
//形式参数:	const uchar_t *
//形式参数:	char
//功能概要:	转换 UCS2 字符串为 ANSI 字符串（非 UTF16LE 字符替换为第三参数），
//			返回转换的串长。
//备注:		
//********************************
usize_t
UCS2ToANSI(char*, const uchar_t*, char = ' ');
//********************************
//名称:		UCS4ToANSI
//全名:		CHRLib::UCS4ToANSI
//可访问性:	public 
//返回类型:	CHRLib::usize_t
//修饰符:	
//形式参数:	char *
//形式参数:	const fchar_t *
//形式参数:	char
//功能概要:	转换 UCS4 字符串为 ANSI 字符串（非 UTF16BE 字符替换为第三参数），
//			返回转换的串长。
//备注:		
//********************************
usize_t
UCS4ToANSI(char*, const fchar_t*, char = ' ');

//********************************
//名称:		MBCSToUTF16LE
//全名:		CHRLib::MBCSToUTF16LE
//可访问性:	public 
//返回类型:	CHRLib::usize_t
//修饰符:	
//形式参数:	uchar_t *
//形式参数:	const char *
//形式参数:	const CSID &
//功能概要:	按字符集转换 MBCS 字符串为 UTF-16LE 字符串，返回转换的串长。
//备注:		
//********************************
usize_t
MBCSToUTF16LE(uchar_t*, const char*, const CSID& = CS_Default);
//********************************
//名称:		MBCSToUCS
//全名:		CHRLib::MBCSToUCS
//可访问性:	public 
//返回类型:	CHRLib::usize_t
//修饰符:	
//形式参数:	fchar_t *
//形式参数:	const char *
//形式参数:	const CSID &
//功能概要:	按字符集转换 MBCS 字符串为 UCS-4 字符串，返回转换的串长。
//备注:		
//********************************
usize_t
MBCSToUCS(fchar_t*, const char*, const CSID& = CS_Default);
//********************************
//名称:		UTF16LEToMBCS
//全名:		CHRLib::UTF16LEToMBCS
//可访问性:	public 
//返回类型:	CHRLib::usize_t
//修饰符:	
//形式参数:	char *
//形式参数:	const uchar_t *
//形式参数:	const CSID &
//功能概要:	按字符集转换 UTF-16LE 字符串为 MBCS 字符串，返回转换的串长。
//备注:		
//********************************
usize_t
UTF16LEToMBCS(char*, const uchar_t*, const CSID& = CS_Default);
//********************************
//名称:		UCS4ToUCS2
//全名:		CHRLib::UCS4ToUCS2
//可访问性:	public 
//返回类型:	CHRLib::usize_t
//修饰符:	
//形式参数:	uchar_t *
//形式参数:	const fchar_t *
//功能概要:	转换 UCS-4 字符串为 UCS-2 字符串，返回转换的串长。
//备注:		
//********************************
usize_t
UCS4ToUCS2(uchar_t*, const fchar_t*);

//********************************
//名称:		sdup
//全名:		CHRLib::sdup
//可访问性:	public 
//返回类型:	char*
//修饰符:	
//形式参数:	const char *
//形式参数:	char
//功能概要:	复制 MBCS 字符串为 ANSI 字符串（非 ANSI 字符替换为第二参数）。
//注释:	空间由 free() 释放。	
//********************************
char*
sdup(const char*, char = ' ');
//********************************
//名称:		sdup
//全名:		CHRLib::sdup
//可访问性:	public 
//返回类型:	char*
//修饰符:	
//形式参数:	const uchar_t *
//形式参数:	char
//功能概要:	复制 UCS-2 字符串为 ANSI 字符串（非 ANSI 字符替换为第二参数）。
//备注:		空间由 free() 释放。
//********************************
char*
sdup(const uchar_t*, char = ' ');
//********************************
//名称:		sdup
//全名:		CHRLib::sdup
//可访问性:	public 
//返回类型:	char*
//修饰符:	
//形式参数:	const fchar_t *
//形式参数:	char
//功能概要:	复制 UCS-4 字符串为 ANSI 字符串（非 ANSI 字符替换为第二参数）。
//备注:		空间由 free() 释放。
//********************************
char*
sdup(const fchar_t*, char = ' ');

//********************************
//名称:		ucsdup
//全名:		CHRLib::ucsdup
//可访问性:	public 
//返回类型:	uchar_t*
//修饰符:	
//形式参数:	const char *
//形式参数:	const CSID &
//功能概要:	复制 MBCS 字符串为 UCS-2/UTF-16LE 字符串。
//备注:		空间由 free() 释放。
//********************************
uchar_t*
ucsdup(const char*, const CSID& = CS_Default);
//********************************
//名称:		ucsdup
//全名:		CHRLib::ucsdup
//可访问性:	public 
//返回类型:	uchar_t*
//修饰符:	
//形式参数:	const uchar_t *
//功能概要:	复制 UCS-2 字符串。
//备注:		空间由 free() 释放。
//********************************
uchar_t*
ucsdup(const uchar_t*);
//********************************
//名称:		ucsdup
//全名:		CHRLib::ucsdup
//可访问性:	public 
//返回类型:	uchar_t*
//修饰符:	
//形式参数:	const fchar_t *
//功能概要:	复制 UCS-4 字符串为 UCS-2 字符串。
//备注:		空间由 free() 释放。
//********************************
uchar_t*
ucsdup(const fchar_t*);

/*
ubyte_t    IsFullChar(char*);
*/

CHRLIB_END

#endif

