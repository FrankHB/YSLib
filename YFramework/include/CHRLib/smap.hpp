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
\version r2405;
\author FrankHB<frankhb1989@gmail.com>
\since build 247 。
\par 创建时间:
	2009-11-17 17:53:21 +0800;
\par 修改时间:
	2011-12-24 16:47 +0800;
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

/*!
\brief 编码转换表。
\since 早于 build 132 。
*/
//@{
yconstexpr byte cp17[] = {0};
extern "C"
{
	extern const byte cp113[];
	//extern const byte cp2026[13658];
}
yconstexpr byte cp2026[] = {0};
//@}


/*!
\brief 以输入迭代器指向内容填充字节。
\since build 249 。
*/
template<typename _tIn, typename _tState>
inline byte
FillByte(_tIn& i, _tState& st)
{
	static_assert(std::is_explicitly_convertible<decltype(*i), byte>::value,
		"Invalid mapping source type found @ CHRLib::GetByteOf;");

	if(is_undereferencable(i))
		return false;

	const auto r(static_cast<byte>(*i));

	yunseq(++i, GetSequenceOf(st)[GetCountOf(st)++] = r);
	return true;
}


/*!
\brief 静态编码映射函数模板。
\since build 245 。
*/
//@{
template<Encoding>
class GUCS2Mapper
{};

template<>
struct GUCS2Mapper<CharSet::SHIFT_JIS>
{
/*	template<typename _tIn, typename _tState>
	static byte
	Map(ucs2_t& uc, _tIn&& i, _tState&& st)
	{
		uint_least16_t row(0), col(0), ln(188); // (7E-40 + 1 + FC-80 + 1)
		const auto c(FillByte(i, st));

		if((c >= 0xA1) && (c <= 0xC6))
		{
			const auto d(FillByte(i, st));

			row = c - 0xA1 ;
			if(d >= 0x40 && d <= 0x7E)
				col = d - 0x40 ;
			else if(d >= 0xA1 && d <= 0xFE)
				col = d - 0x62;
			uc = cp17[row * ln + col];
		}
		else if(c >= 0xC9 && c <= 0xF9)
		{
			const auto d(FillByte(i, st));

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
	}*/
};

template<>
struct GUCS2Mapper<CharSet::UTF_8>
{
	template<typename _tIn, typename _tState>
	static byte
	Map(ucs2_t& uc, _tIn&& i, _tState&& st)
	{
		auto& cnt(GetCountOf(st));

		if(cnt < 0 || cnt > 3)
		{
			cnt = -1;
			return 0;
		}

		const auto seq(GetSequenceOf(st));
		bool b(true);

		switch(cnt)
		{
		case 0:
			if(!(b = FillByte(i, st)))
				break;
			if(seq[0] < 0x80)
			{
				uc = seq[0];
				break;
			}
		case 1:
			if(!(b = FillByte(i, st)))
				break;
			if((seq[0] & 0x20) == 0)
			{
				uc = ((seq[0] & 0x1C) >> 2 << 8)
					| ((seq[0] & 0x03) << 6)
					| (seq[1] & 0x3F);
				break;
			}
		case 2:
			if(!(b = FillByte(i, st)))
				break;
			uc = (((seq[0] & 0x0F) << 4
				| (seq[1] & 0x3C) >> 2) << 8)
				| ((seq[1] & 0x3) << 6)
				| (seq[2] & 0x3F);
			break;
		default:
			b = false;
		}

		const auto r(cnt + !b);

		cnt = b ? 0 : -2;
		return r;
	}

	template<typename _tOut>
	static byte
	InverseMap(_tOut d, const ucs2_t& s)
	{
		size_t l(0);

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
	template<typename _tIn, typename _tState>
	static byte
	Map(ucs2_t& uc, _tIn&& i, _tState&& st)
	{
		auto& cnt(GetCountOf(st));

		if(cnt < 0 || cnt > 1)
		{
			cnt = -1;
			return 0;
		}

		const auto seq(GetSequenceOf(st));
		bool b(true);

		switch(cnt)
		{
		case 0:
			if(!(b = FillByte(i, st)))
				break;
			if(cp113[seq[0]] != 0)
			{
				uc = seq[0];
				break;
			}
		case 1:
			if(!(b = FillByte(i, st)))
				break;
			if((seq[0] << 8 | seq[1]) < 0xFF7E)
			{
				uc = reinterpret_cast<const ucs2_t*>(cp113 + 0x0100)[
					seq[0] << 8 | seq[1]];
				break;
			}
		default:
			b = false;
		}

		const auto r(cnt + !b);

		cnt = b ? 0 : -2;
		return r;
	}
};

template<>
struct GUCS2Mapper<CharSet::UTF_16BE>
{
	template<typename _tIn, typename _tState>
	static byte
	Map(ucs2_t& uc, _tIn&& i, _tState&& st)
	{
		auto& cnt(GetCountOf(st));

		if(cnt < 0 || cnt > 1)
		{
			cnt = -1;
			return 0;
		}

		const auto seq(GetSequenceOf(st));
		bool b(true);

		switch(cnt)
		{
		case 0:
			if(!(b = FillByte(i, st)))
				break;
		case 1:
			if(!(b = FillByte(i, st)))
				break;
			uc = seq[0] << 8 | seq[1];
			break;
		default:
			b = false;
		}

		const auto r(cnt + !b);

		cnt = b ? 0 : -2;
		return r;
	}
};

template<>
struct GUCS2Mapper<CharSet::UTF_16LE>
{
	template<typename _tIn, typename _tState>
	static byte
	Map(ucs2_t& uc, _tIn&& i, _tState&& st)
	{
		auto& cnt(GetCountOf(st));

		if(cnt < 0 || cnt > 1)
		{
			cnt = -1;
			return 0;
		}

		const auto seq(GetSequenceOf(st));
		bool b(true);

		switch(cnt)
		{
		case 0:
			if(!(b = FillByte(i, st)))
				break;
		case 1:
			if(!(b = FillByte(i, st)))
				break;
			uc = seq[0] | seq[1] << 8;
			break;
		default:
			b = false;
		}

		const auto r(cnt + !b);

		cnt = b ? 0 : -2;
		return r;
	}
};

template<>
struct GUCS2Mapper<CharSet::Big5>
{
/*	template<typename _tIn, typename _tState>
	static byte
	Map(ucs2_t& uc, _tIn&& i, _tState&& st)
	{
		uint_least16_t row(0), col(0), ln(157); // (7E-40 + FE-A1)
		const auto c(FillByte(i, st));

		if(c >= 0xA1 && c <= 0xC6)
		{
			const auto d(FillByte(i, st));

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
			const auto d(FillByte(i, st));

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
	}*/
};
//@}

CHRLIB_END

#endif

