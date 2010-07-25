// 通用平台描述文件 -> DS by Franksoft 2009 - 2010
// CodePage = UTF-8;
// UTime = 2009-11-24 0:05:08;
// UTime = 2010-7-22 12:09;
// Version = 0.1200;


#ifndef _PLATFORM_H_
#define _PLATFORM_H_

//GNUC __attribute__ 支持。
#ifndef _ATTRIBUTE
#	ifdef __GNUC__
#		define _ATTRIBUTE(attrs) __attribute__ (attrs)
#	else
#		define _ATTRIBUTE(attrs)
#	endif
#endif

#include <nds.h>
#include <fat.h>
#include <unistd.h>
#include <sys/dir.h>

#define YC_YCOMMON_H "../YCLib/ycommon.h"
#define YS_YSBUILD_H "../YSLib/Helper/yshelper.h"

//包含 PALib 。
//#include <PA9.h>

#ifdef USE_EFS
//包含 EFSLib 。
#include "efs_lib.h"
#endif

//定义使用 Unicode 编码字符串。
#ifndef UNICODE
#define UNICODE
// Unicode 宽字符支持。
#endif

typedef u16 PIXEL; //像素。
typedef PIXEL SCRBUF[SCREEN_WIDTH * SCREEN_HEIGHT]; //主显示屏缓冲区。
#define BITALPHA BIT(15) // Alpha 位。

#define SCR_MAIN 0x1
#define SCR_SUB 0x2
#define SCR_BOTH 0x3

#endif

