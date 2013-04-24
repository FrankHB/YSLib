/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Convert.hpp
\ingroup CHRLib
\brief 转换模版。
\version r52
\author FrankHB <frankhb1989@gmail.com>
\since build 400
\par 创建时间:
	2013-04-23 10:18:20 +0800
\par 修改时间:
	2013-04-23 10:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::Convert
*/


#ifndef CHRLIB_INC_Convert_hpp_
#define CHRLIB_INC_Convert_hpp_ 1

#include "chrmap.h"
#include <ystdex/any_iterator.hpp> // for ystdex::input_monomorphic_iterator;

CHRLIB_BEGIN

template<typename _tIn, typename _fConv>
ConversionResult
ConvertCharacter(_fConv f, ucs2_t& uc, _tIn&& it, ConversionState&& st)
{
	return f(uc, ystdex::input_monomorphic_iterator(std::ref(it)),
		std::move(st));
}
template<typename _tIn, typename _fConv>
ConversionResult
ConvertCharacter(_fConv f, _tIn&& it, ConversionState&& st)
{
	return f(ystdex::input_monomorphic_iterator(std::ref(it)), std::move(st));
}

CHRLIB_END

#endif

