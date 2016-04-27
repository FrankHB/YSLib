/*
	© 2013-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Environment.cpp
\ingroup Helper
\brief 环境。
\version r1761
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:27:29 +0800
\par 修改时间:
	2016-04-27 15:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Environment
*/


#include "Helper/YModules.h"
#include YFM_Helper_Environment
#include YFM_Helper_Initialization // for InitializeEnvironment;
#if YCL_Win32
#	include YFM_Win32_YCLib_MinGW32 // for platform_ex::FixConsoleHandler,
//	platform_ex::Win32Exception;
#endif

namespace YSLib
{

using namespace platform_ex;
#if YF_Hosted
using namespace Host;
#endif

Environment::Environment()
{
#if YCL_Win32
	TryExpr(FixConsoleHandler())
	CatchExpr(Win32Exception&,
		YTraceDe(Warning, "Console handler setup failed."))
#endif
	InitializeEnvironment(*this);
	YCL_Trace(Debug, "Base environment lifetime began.");
}
Environment::~Environment()
{
	YCL_Trace(Debug, "Base environment lifetime ended.");
	YTraceDe(Notice, "Uninitialization entered with %zu handler(s) to be"
		" called.\n", app_exit.size());
	while(!app_exit.empty())
		app_exit.pop();
}

} // namespace YSLib;

