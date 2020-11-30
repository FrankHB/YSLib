/*
	© 2009-2016, 2018, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstdio.cpp
\ingroup YStandardEx
\brief ISO C 标准输入/输出扩展。
\version r253
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2011-09-21 08:38:51 +0800
\par 修改时间:
	2020-11-29 21:00 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CStandardIO
*/


#include "ystdex/cstdio.h" // for ystdex::call_value_or;
#include "ystdex/cassert.h"
#include <libdefect/string.h> // for std::char_traits<char>::length and
//	possible fix for std::vsnprintf;
#include "ystdex/functional.hpp" // for ystdex::compose, addrof;
#include "ystdex/algorithm.hpp" // for ystdex::trivially_copy_n;

namespace ystdex
{

size_t
vfmtlen(const char* fmt, std::va_list args) ynothrowv
{
	yconstraint(fmt);

	const int l(std::vsnprintf({}, 0, fmt, args));

	return size_t(l < 0 ? -1 : l);
}
size_t
vfmtlen(const wchar_t* fmt, std::va_list args) ynothrowv
{
	yconstraint(fmt);

	const int l(std::vswprintf({}, 0, fmt, args));

	return size_t(l < 0 ? -1 : l);
}


bool
fexists(const char* path, bool create) ynothrowv
{
	yconstraint(path);
	return ystdex::call_value_or(ystdex::compose(std::fclose, addrof<>()),
		std::fopen(path, create ? "w+b" : "rb"), yimpl(1)) == 0;
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

	if(str)
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

	const int val(std::fgetc(stream));

	if(val != EOF)
	{
		yassume(int(byte(val)) == val);
		value = byte(val);
	}
	else
		stream = {};
	return *this;
}


void
block_buffer::fill(size_t offset, size_t n, byte value) ynothrowv
{
	yconstraint(get());

	yunseq(ystdex::trivially_fill_n(get() + offset, n, value),
		need_flush = true);
}

void
block_buffer::read(void* dst, size_t offset, size_t n) const ynothrowv
{
	yunseq((yconstraint(get()), 0), (yconstraint(dst), 0));

	ystdex::trivially_copy_n(get() + offset, n, static_cast<byte*>(dst));
}

void
block_buffer::write(size_t offset, const void* src, size_t n) ynothrowv
{
	yunseq((yconstraint(get()), 0), (yconstraint(src), 0));

	yunseq(ystdex::trivially_copy_n(static_cast<const byte*>(src), n,
		get() + offset), need_flush = true);
}

} // namespace ystdex;

