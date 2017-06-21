/*
	© 2014-2015, 2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YCoreUtilities.cpp
\ingroup Core
\brief 核心实用模块。
\version r60
\author FrankHB <frankhb1989@gmail.com>
\since build 539
\par 创建时间:
	2014-10-01 08:52:17 +0800
\par 修改时间:
	2017-06-19 02:59 +0800
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

void
ArgumentsVector::Reset(int argc, char* argv[])
{
	Arguments.clear();
	Arguments.reserve(CheckNonnegative<size_t>(argc, "argument number"));
	for(size_t i(0); i < size_t(argc); ++i)
		Arguments.push_back(Nonnull(argv[i]));
}


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

