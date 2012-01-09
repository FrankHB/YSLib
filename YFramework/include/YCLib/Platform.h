/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Platform Platform
\brief YSLib 库平台。
*/
/*!	\defgroup DS DS
\ingroup Platform
\brief DS 平台。
*/

/*!	\file Platform.h
\ingroup DS
\brief 通用平台描述文件。
\version r1319;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-24 00:05:08 +0800;
\par 修改时间:
	2012-01-06 13:38 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::Platform;
*/


#ifndef INCLUDED_PLATFORM_H_
#define INCLUDED_PLATFORM_H_

//GNUC __attribute__ 支持。
#ifndef _ATTRIBUTE
#	ifdef __GNUC__
#		define _ATTRIBUTE(attrs) __attribute__ (attrs)
#	else
#		define _ATTRIBUTE(attrs)
#	endif
#endif


//类型。
#include <nds/ndstypes.h>

#endif

