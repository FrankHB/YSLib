/*
	© 2013-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup NPL Naming Protocol Language
\ingroup YFramework
\brief 命名协议语言。
*/

/*!	\file YModules.h
\ingroup YFramework
\brief NPL 库模块配置文件。
\version r56
\author FrankHB <frankhb1989@gmail.com>
\since build 462
\par 创建时间:
	2013-12-23 21:57:14 +0800
\par 修改时间:
	2016-01-07 10:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::YModules
*/


#ifndef YF_INC_NPL_YModules_h_
#define YF_INC_NPL_YModules_h_ 1

#include "../YModules.h"
#include "../YSLib/Service/YModules.h"

//! \since build 472
#define YFM_NPL_NPL <NPL/NPL.h>
//! \since build 663
#define YFM_NPL_NPLA <NPL/NPLA.h>
//! \since build 472
#define YFM_NPL_NPLA1 <NPL/NPLA1.h>
//! \since build 461
//@{
#define YFM_NPL_Lexical <NPL/Lexical.h>
#define YFM_NPL_SContext <NPL/SContext.h>
#define YFM_NPL_Configuration <NPL/Configuration.h>
//@}
//! \since build 623
#define YFM_NPL_Dependency <NPL/Dependency.h>

#endif

