/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystdex.cpp
\ingroup YCLib
\brief YCLib C++ 标准库扩展。
\version 0.2030;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-27 17:31:20 +0800; 
\par 修改时间:
	2011-04-25 14:14 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YStandardExtend;
*/


#include "ystdex.h"
#include <cstdio>
#include <cstring>
#include "ystdex/util.hpp" // for nullptr;

namespace ystdex
{
	std::size_t
	strlen_n(const char* s)
	{
		return s ? std::strlen(s) : 0;
	}

	char*
	strcpy_n(char* d, const char* s)
	{
		return d && s ? std::strcpy(d, s) : nullptr;
	}

	char*
	stpcpy_n(char* d, const char* s)
	{
		return d && s ? stpcpy(d, s) : nullptr;
	}

	int
	stricmp_n(const char* s1, const char* s2)
	{
		return s1 && s2 ? stricmp(s1, s2) : EOF;
	}

	char*
	strdup_n(const char* s)
	{
		return s ? strdup(s) : nullptr;
	}

	char*
	strcpycat(char* d, const char* s1, const char* s2)
	{
		if(!d)
			return nullptr;
		if(s1)
			std::strcpy(d, s1);
		if(s2)
			std::strcat(d, s2);
		return d;
	}

	char*
	strcatdup(const char* s1, const char* s2, void*(*fun)(std::size_t))
	{
		char* d(static_cast<char*>(
			fun((strlen(s1) + strlen(s2) + 1) * sizeof(char))));

		return strcpycat(d, s1, s2);
	}


	bool
	fexists(CPATH path)
	{
		FILE* file = fopen(path, "rb");
		if(file)
		{
			std::fclose(file);
			return true;
		}
		return false;
	}
}

