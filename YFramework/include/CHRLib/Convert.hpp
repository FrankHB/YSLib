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
\version r141
\author FrankHB <frankhb1989@gmail.com>
\since build 400
\par 创建时间:
	2013-04-23 10:18:20 +0800
\par 修改时间:
	2015-07-17 23:40 +0800
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
//! \since build 400
template<typename _tIn, typename _fConv>
ConversionResult
ConvertCharacter(_fConv f, _tIn&& i, ConversionState&& st)
{
	return f(ystdex::input_monomorphic_iterator(std::ref(i)), std::move(st));
}
//! \since build 615
template<typename _tIn, typename _tOut, typename _fConv>
ConversionResult
ConvertCharacter(_fConv f, _tOut&& obj, GuardPair<_tIn>& i,
	ConversionState&& st)
{
	ystdex::input_monomorphic_iterator it(i.first);
	const auto res(f(yforward(obj), GuardPair<
		ystdex::input_monomorphic_iterator>(it, i.second), std::move(st)));
	auto p_target(it.target<_tIn>());

	yassume(p_target);
	i.first = *p_target;
	return res;
}
//! \since build 615
template<typename _tIn, typename _fConv>
ConversionResult
ConvertCharacter(_fConv f, GuardPair<_tIn>& i, ConversionState&& st)
{
	ystdex::input_monomorphic_iterator it(i.first);
	const auto res(f(GuardPair<
		ystdex::input_monomorphic_iterator>(it, i.second), std::move(st)));
	auto p_target(it.target<_tIn>());

	yassume(p_target);
	i.first = *p_target;
	return res;
}

/*!
\brief 验证指定字符序列是否可转换为 UCS-2 字符串。
\pre 指定范围内的迭代器可解引用。
\note 使用 ADL \c MBCToUC 指定转换迭代器的例程：第一参数应迭代输入参数至下一位置。
\since build 613
*/
//@{
template<typename _tIn>
bool
VerifyUC(_tIn&& first, ystdex::remove_reference_t<_tIn> last, Encoding enc)
{
	auto res(ConversionResult::OK);

	while(first != last && *first != 0
		&& (res = MBCToUC(first, enc), res == ConversionResult::OK))
		;
	return res == ConversionResult::OK || *first == char();
}
/*!
\pre 迭代器为随机迭代器。
\note 使用指定迭代器边界。
\since build 614
*/
template<typename _tIn>
bool
VerifyUC(_tIn&& first, typename std::iterator_traits<ystdex::decay_t<_tIn>>
	::difference_type n, Encoding enc)
{

	if(n != 0)
	{
		auto res(ConversionResult::OK);
		const auto i(first);

		while(*first != 0 && (res = MBCToUC(first, first + n, enc),
			res == ConversionResult::OK))
			;
		return res == ConversionResult::OK
			|| (res == ConversionResult::BadSource && first - i == n)
			|| *first == char();
	}
	return true;
}
//@}

} // namespace CHRLib;

#endif

