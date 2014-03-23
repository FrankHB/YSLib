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
\version r85
\author FrankHB <frankhb1989@gmail.com>
\since build 313
\par 创建时间:
	2012-06-01 14:32:37 +0800
\par 修改时间:
	2014-03-22 13:27 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Keys
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Keys
#include YFM_YCLib_NativeAPI

namespace platform
{

size_t
FindFirstKey(const KeyInput& keys)
{
#if _GLIBCXX_BITSET
	return keys._Find_first();
#else
#	error Only libstdc++ is currently supported.
#endif
}

size_t
FindNextKey(const KeyInput& keys, size_t key)
{
#if _GLIBCXX_BITSET
	return keys._Find_next(key);
#else
#	error Only libstdc++ is currently supported.
#endif
}

#if YCL_Win32
char
MapKeyChar(size_t code)
{
	return ::MapVirtualKeyW(unsigned(code), MAPVK_VK_TO_CHAR) & 0x7F;
}
char
MapKeyChar(const KeyInput& keys)
{
	auto code(FindFirstKey(keys));

	if(YB_LIKELY(code != KeyBitsetWidth))
	{
		const bool shift(code == VK_SHIFT);

		if(shift)
			code = FindNextKey(keys, VK_SHIFT);

		const char c(MapKeyChar(code));

		return shift ? c : std::tolower(c);
	}
	return char();
}
#endif

}

