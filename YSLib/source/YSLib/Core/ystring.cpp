/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystring.cpp
\ingroup Core
\brief 基础字符串管理。
\version 0.2672;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-05 22:44:04 +0800;
\par 修改时间:
	2011-03-05 17:05 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YString;
*/


#include "ystring.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

uchar_t* String::s_str(NULL);


String
MBCSToString(const char* s, const CSID& cp)
{
	uchar_t* t(ucsdup(s, cp));
	String str(t);

	std::free(t);
	return str;
}

string
StringToMBCS(const ustring& s, const CSID& cp)
{
	char* t(static_cast<char*>(std::malloc((s.length() + 1) << 2)));

	if(t)
		UTF16LEToMBCS(t, s.c_str(), cp);

	string str(t);

	std::free(t);
	return str;
}

YSL_END_NAMESPACE(Text)

YSL_END

