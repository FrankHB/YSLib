/*
	© 2009-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yglobal.cpp
\ingroup Helper
\brief 平台相关的全局对象和函数定义。
\version r2982
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-22 15:28:52 +0800
\par 修改时间:
	2013-11-26 19:56 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::YGlobal
*/


#include "Helper/yglobal.h"
//#include <clocale>

namespace YSLib
{

//全局变量。
#ifdef YSL_USE_MEMORY_DEBUG
//MemoryList DebugMemory(nullptr);
#endif

/*!
\ingroup PublicObject
\brief 全局变量映射。
*/
//@{
//@}

#if YF_Hosted
namespace Host
{

} // namespace Host;
#endif

} // namespace YSLib;

