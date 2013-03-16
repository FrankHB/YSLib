/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Helper Helper
\ingroup YFramework
\brief YFramework 助手模块。
*/

/*!	\file yglobal.h
\ingroup Helper
\brief 平台相关的全局对象和函数定义。
\version r1827
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-22 15:14:57 +0800
\par 修改时间:
	2013-03-13 15:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::YGlobal
*/


#ifndef INC_Helper_YGlobal_h_
#define INC_Helper_YGlobal_h_ 1

//包含 YCLib 公用部分。
#include "YCLib/ycommon.h"

#include "YSLib/Core/ygdibase.h"
#include "YSLib/Core/ymsgdef.h"
#include "YSLib/Core/ydevice.h"

YSL_BEGIN

/*!	\defgroup CustomGlobalVariables Custom Global Variables
\ingroup GlobalObjects
\brief 平台相关的全局变量。
\since build 173
*/
//@{
//@}


//前向声明。
YSL_BEGIN_NAMESPACE(Drawing)
YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Messaging)
YSL_END_NAMESPACE(Messaging)

//! \since build 388
YSL_BEGIN_NAMESPACE(UI)
class Desktop;
YSL_END_NAMESPACE(UI)

#if YCL_HOSTED
/*!
\brief 宿主环境支持。
\since build 378
*/
YSL_BEGIN_NAMESPACE(Host)
//! \since build 381
class Environment;
YSL_END_NAMESPACE(Host)
#endif

//! \since build 388
using UI::Desktop;

YSL_END

#endif

