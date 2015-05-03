/*
	© 2013, 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup CHRLib CHRLib
\ingroup YFramework
\brief 字符编码处理库。
*/

/*!	\file YModules.h
\ingroup CHRLib
\brief CHRLib 库模块配置文件。
\version r56
\author FrankHB <frankhb1989@gmail.com>
\since build 462
\par 创建时间:
	2013-12-23 21:57:08 +0800
\par 修改时间:
	2015-05-04 01:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::YModules
*/


#ifndef YF_INC_CHRLib_YModules_h_
#define YF_INC_CHRLib_YModules_h_ 1

#include "../YModules.h"
#include "YCLib/YModules.h"

//! \since build 461
//@{
#define YFM_CHRLib_CHRDefinition <CHRLib/chrdef.h>
#define YFM_CHRLib_CharacterMapping <CHRLib/CharacterMapping.h>
#define YFM_CHRLib_CharacterProcessing <CHRLib/CharacterProcessing.h>
#define YFM_CHRLib_Encoding <CHRLib/Encoding.h>
#define YFM_CHRLib_Convert <CHRLib/Convert.hpp>
#define YFM_CHRLib_StaticMapping <CHRLib/StaticMapping.hpp>
#define YFM_CHRLib_MappingEx <CHRLib/MappingEx.h>
//@}

#endif

