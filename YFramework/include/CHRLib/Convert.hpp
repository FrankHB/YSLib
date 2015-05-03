/*
	© 2013, 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Convert.hpp
\ingroup CHRLib
\brief 转换模板。
\version r69
\author FrankHB <frankhb1989@gmail.com>
\since build 400
\par 创建时间:
	2013-04-23 10:18:20 +0800
\par 修改时间:
	2015-04-30 21:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::Convert
*/


#ifndef INC_CHRLib_Convert_hpp
#define INC_CHRLib_Convert_hpp 1

#include "YModules.h"
#include YFM_CHRLib_CharacterMapping
#include <ystdex/any_iterator.hpp> // for ystdex::input_monomorphic_iterator;

namespace CHRLib
{

//! \since build 595
template<typename _tIn, typename _tOut, typename _fConv>
ConversionResult
ConvertCharacter(_fConv f, _tOut&& obj, _tIn&& i, ConversionState&& st)
{
	return f(yforward(obj), ystdex::input_monomorphic_iterator(std::ref(i)),
		std::move(st));
}
template<typename _tIn, typename _fConv>
ConversionResult
ConvertCharacter(_fConv f, _tIn&& i, ConversionState&& st)
{
	return f(ystdex::input_monomorphic_iterator(std::ref(i)), std::move(st));
}

} // namespace CHRLib;

#endif

