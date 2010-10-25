// CHRLib -> CHRDefinition by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-17 17:52:35 + 08:00;
// UTime = 2010-10-23 22:53 + 08:00;
// Version = 0.1544;


#ifndef INCLUDED_CHRDEF_H_
#define INCLUDED_CHRDEF_H_

// CHRDefinition ：类型定义。

#include <platform.h>

#define CHRLIB_BEGIN	namespace CHRLib {
#define CHRLIB_END		}
#define _CHRLIB_			::CHRLib::
#define _CHRLIB			::CHRLib

#define CHRLIB_BEGIN_NAMESPACE(s)	namespace s {
#define CHRLIB_END_NAMESPACE(s)	}

CHRLIB_BEGIN

typedef unsigned char ubyte_t;
typedef unsigned long usize_t;

//字符类型定义。
typedef std::char32_t fchar_t; // UCS-4 字符。
typedef std::char16_t uchar_t; // UCS-2 字符。
typedef s32 uint_t;

//宽字符串转换宏。
#define FS(str) reinterpret_cast<const CHRLib::fchar_t*>(L##str)

CHRLIB_END

#endif

