/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystring.cpp
\ingroup Core
\brief 基础字符串管理。
\version r1785
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-05 22:44:04 +0800
\par 修改时间:
	2013-06-15 00:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YString
*/


#include "YSLib/Core/ystring.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

String&
String::operator*=(size_t n)
{
	switch(n)
	{
	case 0:
		clear();
	case 1:
		break;
	default:
		reserve(length() * n);
		ystdex::concat(*this, n);
	}
	return *this;
}

YSL_END_NAMESPACE(Text)

YSL_END

