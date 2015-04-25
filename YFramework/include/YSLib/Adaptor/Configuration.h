/*
	© 2010-2013, 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Configuration.h
\ingroup Adaptor
\brief YSLib 库配置。
\version r655
\author FrankHB <frankhb1989@gmail.com>
\since build 161
\par 创建时间:
	2010-10-09 09:25:26 +0800
\par 修改时间:
	2015-04-24 03:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::Configuration
*/


#ifndef YSL_INC_Adaptor_Configuration_h_
#define YSL_INC_Adaptor_Configuration_h_ 1

#include "YCLib/YModules.h"
#include YFM_YCLib_Platform

//#define NDEBUG //!< 非调试模式。

#ifndef NDEBUG
#	define YSL_USE_MEMORY_DEBUG //!< 使用 YSLib 内存调试设施。
#endif

#endif

