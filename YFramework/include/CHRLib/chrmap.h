/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file chrmap.h
\ingroup CHRLib
\brief 字符映射。
\version r2200;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-17 17:52:35 +0800;
\par 修改时间:
	2011-12-03 15:13 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	CHRLib::CharacterMapping;
*/


#ifndef CHRLIB_INC_CHRMAP_H_
#define CHRLIB_INC_CHRMAP_H_

#include "encoding.h"

CHRLIB_BEGIN

using CharSet::Encoding;


//默认字符集。
yconstexpr Encoding CP_Default = CharSet::UTF_8;
yconstexpr Encoding CP_Local = CharSet::GBK;


/*!
\brief 取 c_ptr 指向的大端序双字节字符。
\pre 断言： \c c_ptr 。
*/
inline ucs2_t
FetchBiCharBE(const char* c_ptr)
{
	assert(c_ptr);

	return (*c_ptr << CHAR_BIT) | c_ptr[1];
}

/*!
\brief 取 c_ptr 指向的小端序双字节字符。
\pre 断言： \c c_ptr 。
*/
inline ucs2_t
FetchBiCharLE(const char* c_ptr)
{
	assert(c_ptr);

	return (c_ptr[1] << CHAR_BIT) | *c_ptr;
}


/*!
\brief 转换状态。
*/
struct ConversionState
{
	/*!
	\brief 当前转换字符记数。

	\note 等于 -1 时表示转换状态错误。
	\note 等于 -2 时表示转换失败（如与源数据越界等）。
	*/
	std::int_fast8_t Count;
	union
	{
		ucsint_t Wide;
		byte Sequence[4];
	} Value;

	ConversionState(usize_t = 0);
};

inline
ConversionState::ConversionState(usize_t n)
	: Count(n), Value()
{}

inline std::int_fast8_t&
GetCountOf(ConversionState& st)
{
	return st.Count;
}
inline ucsint_t&
GetWideOf(ConversionState& st)
{
	return st.Value.Wide;
}
inline byte*
GetSequenceOf(ConversionState& st)
{
	return st.Value.Sequence;
}

CHRLIB_END

#endif

