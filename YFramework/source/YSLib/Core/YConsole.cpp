/*
	© 2020-2021, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YConsole.cpp
\ingroup Core
\brief 通用控制台接口。
\version r85
\author FrankHB <frankhb1989@gmail.com>
\since build 969
\par 创建时间:
	2023-03-06 18:35:15 +0800
\par 修改时间:
	2023-03-07 06:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YConsole
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YConsole // for std::fprintf, Nonnull, YAssertNonnull;
#include YFM_YSLib_Core_YCoreUtilities // for FetchCachedCommandResult;

namespace YSLib
{

namespace Consoles
{

bool
AllowsColorByEnvironment() ynothrow
{
	static const bool no_color(DetectsNoColor());

	return !no_color;
}


ImplDeDtor(IOutputTerminal)


ImplDeDtor(TPutTerminal)


bool
AOutputTerminal::WriteString(const char* s, size_t len)
{
	const int n(std::fprintf(GetStream(), "%s", Nonnull(s)));

	return n >= 0 && size_t(n) == len;
}


bool
TPutTerminal::ExecuteCachedCommand(string_view cmd) const
{
	YAssertNonnull(cmd.data());
	if(!cmd.empty())
	{
		const auto& str(FetchCachedCommandResult(cmd));

		if(!str.empty())
		{
			// XXX: Error from 'std::fprintf' is ignored.
			std::fprintf(GetStream(), "%s", str.c_str());
			return true;
		}
	}
	return {};
}


ImplDeDtor(ISO6429Terminal)

} // namespace Consoles;

} // namespace YSLib;

