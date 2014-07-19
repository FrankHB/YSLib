/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstdio.cpp
\ingroup YStandardEx
\brief ISO C 标准输入/输出扩展。
\version r194
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2011-09-21 08:38:51 +0800
\par 修改时间:
	2014-07-14 14:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CStandardIO
*/


#include "ystdex/cstdio.h"
#include "ystdex/cassert.h"
#include <string> // for std::char_traits<char>::length;

namespace ystdex
{

bool
fexists(const char* path) ynothrow
{
	yconstraint(path);
	if(const auto fp = std::fopen(path, "rb"))
	{
		std::fclose(fp);
		return true;
	}
	return {};
}


const char*
openmode_conv(std::ios_base::openmode mode) ynothrow
{
	using namespace std;

	switch(unsigned((mode &= ~ios_base::ate) & ~ios_base::binary))
	{
	case ios_base::out:
	case ios_base::out | ios_base::trunc:
		return mode & ios_base::binary ? "wb" : "w";
	case ios_base::out | ios_base::app:
	case ios_base::app:
		return mode & ios_base::binary ? "ab" : "a";
	case ios_base::in:
		return mode & ios_base::binary ? "rb" : "r";
	case ios_base::in | ios_base::out:
		return mode & ios_base::binary ? "r+b" : "r+";
	case ios_base::in | ios_base::out | ios_base::trunc:
		return mode & ios_base::binary ? "w+b" : "w+";
	case ios_base::in | ios_base::out | ios_base::app:
	case ios_base::in | ios_base::app:
		return mode & ios_base::binary ? "a+b" : "a+";
	default:
		break;
	}
	return {};
}
std::ios_base::openmode
openmode_conv(const char* str) ynothrow
{
	using namespace std;

	if(!str)
	{
		ios_base::openmode mode;

		switch(*str)
		{
		case 'w':
			mode = ios_base::out | ios_base::trunc;
			break;
		case 'r':
			mode = ios_base::in;
			break;
		case 'a':
			mode = ios_base::app;
			break;
		default:
			goto invalid;
		}
		if(str[1] != char())
		{
			auto l(char_traits<char>::length(str));

			if(str[l - 1] == 'x')
			{
				if(mode & ios_base::out)
					mode &= ~ios_base::out;
				else
					goto invalid;
				--l;
			}

			bool b(str[1] == 'b'), p(str[1] == '+');

			switch(l)
			{
			case 2:
				if(b != p)
					break;
				goto invalid;
			case 3:
				yunseq(b = b != (str[2] == 'b'), p = p != (str[2] == '+'));
				if(b && p)
					break;
			default:
				goto invalid;
			}
			if(p)
				mode |= *str == 'r' ? ios::out : ios::in;
			if(b)
				mode |= ios::binary;
		}
		return mode;
	}
invalid:
	return ios_base::openmode();
}


ifile_iterator&
ifile_iterator::operator++()
{
	yassume(stream);

	const auto val(std::fgetc(stream));

	if(YB_UNLIKELY(val == EOF))
		stream = {};
	else
	{
		yassume(byte(val) == val);
		value = byte(val);
	}
	return *this;
}

} // namespace ystdex;

