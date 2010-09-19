// 通用平台描述文件 -> DS by Franksoft 2009 - 2010
// CodePage = UTF-8;
// UTime = 2009-11-24 0:05:08;
// UTime = 2010-9-20 6:56;
// Version = 0.1237;


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


//字符兼容性补充。

#include <stdint.h>

namespace std
{
	typedef ::uint16_t char16_t;
	typedef ::uint32_t char32_t;
}


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

#endif

