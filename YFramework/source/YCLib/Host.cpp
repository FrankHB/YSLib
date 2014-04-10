/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Host.cpp
\ingroup YCLib
\ingroup MinGW32
\brief YCLib 宿主平台公共扩展。
\version r43
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 19:03:55 +0800
\par 修改时间:
	2014-04-09 19:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Host
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Host

using namespace YSLib;

#if YF_Hosted
namespace platform_ex
{

Exception::Exception(const std::string& s, LevelType l) ynothrow
	: LoggedEvent(s, l)
{}

} // namespace YSLib;
#endif

