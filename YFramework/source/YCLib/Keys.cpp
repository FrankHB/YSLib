/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Keys.cpp
\ingroup YCLib
\brief 平台相关的基本按键输入定义。
\version r53
\author FrankHB <frankhb1989@gmail.com>
\since build 313
\par 创建时间:
	2012-06-01 14:32:37 +0800
\par 修改时间:
	2014-03-18 02:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Keys
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Keys

namespace platform
{

YF_API size_t
FindFirstKey(const KeyInput& keys)
{
#if _GLIBCXX_BITSET
	return keys._Find_first();
#else
#	error Only libstdc++ is currently supported.
#endif
}

}

