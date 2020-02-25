/*
	© 2015, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ios.hpp
\ingroup YStandardEx
\brief ISO C++ 标准库输入/输出流基类扩展。
\version r80
\author FrankHB <frankhb1989@gmail.com>
\since build 663
\par 创建时间:
	2015-12-28 20:00:09 +0800
\par 修改时间:
	2020-02-25 20:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::IOS
*/


#ifndef YB_INC_ystdex_ios_hpp_
#define YB_INC_ystdex_ios_hpp_ 1

#include "../ydef.h"
#include <ios> // for std::basic_ios, std::ios_base::iostate;

namespace ystdex
{

/*!
\brief 设置流状态。
\note 无异常抛出：和直接调用 std::basic_ios 的 setstate 成员函数不同。
\note 不复用标准库实现的内部接口。
*/
template<typename _tChar, class _tTraits>
void
setstate(std::basic_ios<_tChar, _tTraits>& ios, std::ios_base::iostate state) ynothrow
{
	const auto except(ios.exceptions());

	ios.exceptions(std::ios_base::goodbit);
	ios.setstate(state);
	ios.exceptions(except);
}

/*!
\brief 设置流状态并重新抛出当前异常。
\note 一个主要用例为实现标准库要求的格式/非格式输入/输出函数。
\see WG21 N4567 27.7.2.2.1[istream.formatted.reqmts] 。
\see WG21 N4567 27.7.2.3[istream.unformatted]/1 。
\see WG21 N4567 27.7.3.6.1[ostream.formatted.reqmts] 。
\see WG21 N4567 27.7.3.7[ostream.unformatted]/1 。
\see LWG 91 。
*/
template<typename _tChar, class _tTraits>
YB_NORETURN void
rethrow_badstate(std::basic_ios<_tChar, _tTraits>& ios,
	std::ios_base::iostate state = std::ios_base::badbit)
{
	setstate(ios, state);
	throw;
}

} // namespace ystdex;

#endif

