/*
	© 2009-2016, 2018, 2020-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstdio.cpp
\ingroup YStandardEx
\brief ISO C 标准输入/输出扩展。
\version r352
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2011-09-21 08:38:51 +0800
\par 修改时间:
	2023-03-09 21:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CStandardIO
*/


#include "ystdex/cstdio.h" // for ystdex::call_value_or, std::fclose;
#include "ystdex/cassert.h"
#include <libdefect/string.h> // for std::char_traits<char>::length and
//	possible fix for std::vsnprintf;
#include "ystdex/compose.hpp" // for ystdex::compose;
#include "ystdex/functor.hpp" // for addrof;
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
fexists(const char* path, const char* mode) ynothrowv
{
	yconstraint(path),
	yconstraint(mode);
	return ystdex::call_value_or(ystdex::compose(std::fclose, addrof<>()),
		std::fopen(path, mode), yimpl(1)) == 0;
}
bool
fexists(const char* path, std::ios_base::openmode mode) ynothrowv
{
	const auto mode_str(ystdex::openmode_conv(mode));

	yverify(mode_str);
	return fexists(path, mode_str);
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

