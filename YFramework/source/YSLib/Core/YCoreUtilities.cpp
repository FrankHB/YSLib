/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YCoreUtilities.cpp
\ingroup Core
\brief 核心实用模块。
\version r51
\author FrankHB <frankhb1989@gmail.com>
\since build 539
\par 创建时间:
	2014-10-01 08:52:17 +0800
\par 修改时间:
	2015-12-12 17:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YCoreUtilities
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YCoreUtilities
#include <cstdlib> // for std::getenv;

namespace YSLib
{

bool
FetchEnvironmentVariable(string& res, const char* var)
{
	if(const auto val = std::getenv(Nonnull(var)))
	{
		res = val;
		return true;
	}
	return {};
}

} // namespace YSLib;

