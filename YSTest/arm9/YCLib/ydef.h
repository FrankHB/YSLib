// YCommon 基础库 DS -> YDefinition by Franksoft 2009 - 2010
// CodePage = UTF-8;
// UTime = 2009-12-02 21:42:44 + 08:00;
// UTime = 2010-10-23 23:53 + 08:00;
// Version = 0.2277;


#ifndef INCLUDED_YDEF_H_
#define INCLUDED_YDEF_H_

// YDefinition ：系统环境和公用变量类型的基础定义。

#ifdef __cplusplus
extern "C" {
#endif

#if defined __GNUC__ \
	|| (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)

#	include <stddef.h>
#	include <stdint.h>

typedef intptr_t INT_PTR;
typedef uintptr_t UINT_PTR;

#else

typedef int INT_PTR;
typedef unsigned int UINT_PTR;

#endif

typedef long LONG_PTR;
typedef unsigned long ULONG_PTR;


#define _nullterminated

#undef FALSE
#define FALSE 0

#undef TRUE
#define TRUE 1

#ifndef BASETYPES
#define BASETYPES
typedef unsigned char UCHAR, *PUCHAR;
typedef unsigned short USHORT, *PUSHORT;
typedef unsigned long ULONG, *PULONG;
typedef char* PSZ;
#endif

typedef int             BOOL;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;


typedef void FVOID(void);
typedef FVOID *PFVOID;


// UCS(Universal Character Set) 字符类型定义。

typedef unsigned long UCSCHAR;

#define UCSCHAR_INVALID_CHARACTER (0xFFFFFFFF)
#define MIN_UCSCHAR (0)
#define MAX_UCSCHAR (0x0010FFFF)


typedef INT_PTR			IRES;
typedef UINT_PTR		URES;
typedef LONG_PTR		LRES;

typedef UINT_PTR        WPARAM;
typedef LONG_PTR        LPARAM;

typedef IRES			ERRNO;

#define LOBYTE(w)       ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)       ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))
#define LOWORD(l)       ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)       ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define MAKEWORD(a, b)  ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) \
	| ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)  ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) \
	| ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))


typedef char* STR;
typedef const char* CSTR;

//路径类型定义。
typedef STR PATH;
typedef CSTR CPATH;

#ifdef __cplusplus
}
#endif

#endif

