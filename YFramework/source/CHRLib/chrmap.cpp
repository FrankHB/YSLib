/*
	© 2009-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file chrmap.cpp
\ingroup CHRLib
\brief 字符映射。
\version r762
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-17 17:53:21 +0800
\par 修改时间:
	2013-12-24 00:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::CharacterMapping
*/


#include "CHRLib/YModules.h"
#include YFM_CHRLib_CharacterMapping

namespace CHRLib
{

using namespace CharSet;

size_t
FetchFixedCharWidth(Encoding cp)
{
	switch(cp)
	{
	case csASCII:
		return 1;
	case csUnicode:
	case csUTF16BE:
	case csUTF16LE:
	case csUTF16:
		return 2;
	case csUCS4:
	case csUTF32:
	case csUTF32BE:
	case csUTF32LE:
		return 4;
	default:
		return 0;
	}
}

size_t
FetchMaxCharWidth(Encoding cp)
{
	const auto r = FetchFixedCharWidth(cp);

	return r == 0 ? FetchMaxVariantCharWidth(cp) : r;
}

size_t
FetchMaxVariantCharWidth(Encoding cp)
{
	switch(cp)
	{
	case csGBK:
		return 2;
	case csGB18030:
	case csUTF8:
		return 4;
	default:
		return 0;
	}
}

} // namespace CHRLib;

