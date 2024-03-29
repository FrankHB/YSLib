﻿/*
	© 2014-2015, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cwctype.cpp
\ingroup YStandardEx
\brief ISO C 宽字符分类操作扩展。
\version r71
\author FrankHB <frankhb1989@gmail.com>
\since build 513
\par 创建时间:
	2014-06-29 15:36:20 +0800
\par 修改时间:
	2021-12-21 20:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CWideCharacterType
*/


#include "ystdex/cwctype.h"

namespace ystdex
{

bool
iswprint(wchar_t wc) ynothrow
{
	// TODO: See https://www.sourceware.org/git/?p=newlib-cygwin.git;a=blob;f=newlib/libc/ctype/iswprint.c.
	// NOTE: See http://git.musl-libc.org/cgit/musl/tree/src/ctype/iswprint.c.
	const auto c = unsigned(wc);

	if(c < 0xFFU)
		return ((c + 1) & 0x7F) >= 0x21;
	if(c < 0x2028U || c - 0x202AU < 0xD800 - 0x202A
		|| c - 0xE000U < 0xFFF9 - 0xE000)
		return true;
	return !(c - 0xFFFCU > 0x10FFFF-0xFFFC || (c & 0xFFFE) == 0xFFFE);
}

bool
iswspace(wchar_t wc) ynothrow
{
	// NOTE: See https://www.sourceware.org/git/?p=newlib-cygwin.git;a=blob;f=newlib/libc/ctype/iswprint.c
	return (wc >= 0x0009 && wc <= 0x000D) || wc == 0x0020 || wc == 0x1680
		|| wc == 0x180e || (wc >= 0x2000 && wc <= 0x200A && wc != 2007)
		|| wc == 0x2028 || wc == 0x2029 || wc == 0x205F || wc == 0x3000;
}

} // namespace ystdex;

