/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file smap.hpp
\ingroup CHRLib
\brief 字符映射静态函数。
\version r2245;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-17 17:53:21 +0800;
\par 修改时间:
	2011-09-30 21:59 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	CHRLib::StaticMapping;
*/


#ifndef CHRLIB_INC_SMAP_HPP_
#define CHRLIB_INC_SMAP_HPP_

#include "chrmap.h"
#include <ystdex/cstdio.h>

CHRLIB_BEGIN

//编码转换表。
yconstexpr ubyte_t cp17[] = {0};
extern "C"
{
	extern const ubyte_t cp113[];
	//extern const ubyte_t cp2026[13658];
}
yconstexpr ubyte_t cp2026[] = {0};


template<typename _tIn>
inline byte
GetByteOf(_tIn& i)
{
	static_assert(std::is_explicitly_convertible<decltype(*i), byte>::value,
		"Invalid mapping source type found @ CHRLib::GetByteOf;");

	auto r(static_cast<byte>(*i));

	++i;
	return r;
}


//静态编码映射函数模板原型。
template<Encoding>
class GUCS2Mapper
{};

//编码映射函数模板特化版本。
template<>
struct GUCS2Mapper<CharSet::SHIFT_JIS>
{
	template<typename _tIn>
	static ubyte_t
	Map(ucs2_t& uc, _tIn&& i)
	{
		uint_least16_t row(0), col(0), ln(188); // (7E-40 + 1 + FC-80 + 1)
		const auto c(GetByteOf(i));

		if((c >= 0xA1) && (c <= 0xC6))
		{
			const auto d(GetByteOf(i));

			row = c - 0xA1 ;
			if(d >= 0x40 && d <= 0x7E)
				col = d - 0x40 ;
			else if(d >= 0xA1 && d <= 0xFE)
				col = d - 0x62;
			uc = cp17[row * ln + col];
		}
		else if(c >= 0xC9 && c <= 0xF9)
		{
			const auto d(GetByteOf(i));

			row = c - 0xA3;
			if(d >= 0x40 && d <= 0x7E)
				col = d - 0x40 ;
			else if(d >= 0xA1 && d <= 0xFE)
				col = d - 0x62;
			uc = cp17[row * ln + col];
		}
		else if(c < 0x80)
		{
			uc = c;
			return 1;
		}
		else
			uc = 0xFFFE;
		return 2;
	}
};

template<>
struct GUCS2Mapper<CharSet::UTF_8>
{
	template<typename _tIn>
	static ubyte_t
	Map(ucs2_t& uc, _tIn&& i)
	{
		const auto c(GetByteOf(i));

		if(c < 0x80)
		{
			uc = c;
			return 1;
		}
		else
		{
			const auto d(GetByteOf(i));

			if(c & 0x20)
			{
				uc = (((c & 0x0F) << 4
					| (d & 0x3C) >> 2) << 8)
					| ((d & 0x3) << 6)
					| (GetByteOf(i) & 0x3F);
				return 3;
			}
			else
			{
				uc = ((c & 0x1C) >> 2 << 8) | ((c & 0x03) << 6) | (d & 0x3F);
				return 2;
			}
		}
	}

	template<typename _tOut>
	static ubyte_t
	InverseMap(_tOut d, const ucs2_t& s)
	{
		usize_t l(0);

		if(s < 0x80)
		{
			*d = s;
			return 1;
		}
		if(s < 0x800)
			l = 2;
		else
		{
			*d = 0xE0 | s >> 12;
			++d;
			l = 3;
		}
		*d = 0x80 | (s >> 6 & 0x3F);
		*++d = 0x80 | (s & 0x3F);
		return l;
	}
};

template<>
struct GUCS2Mapper<CharSet::GBK>
{
	template<typename _tIn>
	static ubyte_t
	Map(ucs2_t& uc, _tIn&& i)
	{
		const std::uint_fast8_t c(GetByteOf(i));

		if(cp113[c] != 0)
		{
			uc = c;
			return 1;
		}

		const std::uint_fast8_t d(GetByteOf(i));

		// assert((c << 8 | d) < 0xFF7E);

		uc = reinterpret_cast<const ucs2_t*>(cp113 + 0x0100)[c << 8 | d];
		return 2;
	}
};

template<>
struct GUCS2Mapper<CharSet::UTF_16BE>
{
	template<typename _tIn>
	static ubyte_t
	Map(ucs2_t& uc, _tIn&& i)
	{
		uc = GetByteOf(i) << YCL_CHAR_BIT;
		uc |= GetByteOf(i);
		return 2;
	}
};

template<>
struct GUCS2Mapper<CharSet::UTF_16LE>
{
	template<typename _tIn>
	static ubyte_t
	Map(ucs2_t& uc, _tIn&& i)
	{
		uc = GetByteOf(i);
		uc |= GetByteOf(i) << YCL_CHAR_BIT;
		return 2;
	}
};

template<>
struct GUCS2Mapper<CharSet::Big5>
{
	template<typename _tIn>
	static ubyte_t
	Map(ucs2_t& uc, _tIn&& i)
	{
		uint_least16_t row(0), col(0), ln(157); // (7E-40 + FE-A1)
		const auto c(GetByteOf(i));

		if(c >= 0xA1 && c <= 0xC6)
		{
			const auto d(GetByteOf(i));

			row = c - 0xA1;
			if(d >= 0x40 && d <= 0x7E)
				col = d - 0x40;
			else if(d >= 0xA1 && d <= 0xFE)
				col = d - 0x62;
			uc = cp2026[row * ln + col];
			return 2;
		}
		else if(c >= 0xC9 && c <= 0xF9)
		{
			const auto d(GetByteOf(i));

			row = c - 0xA3;
			if(d >= 0x40 && d <= 0x7E)
				col = c - 0x40;
			else if(d >= 0xA1 && d <= 0xFE)
				col = d - 0x62;
			uc = cp2026[row * ln + col];
			return 2;
		}
		else if(c < 0x80)
		{
			uc = c;
			return 1;
		}
		else
			uc = 0xFFFE;
		return 2;
	}
};

CHRLIB_END

#endif

