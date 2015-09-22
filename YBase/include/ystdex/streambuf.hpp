/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file streambuf.hpp
\ingroup YStandardEx
\brief ISO C++ 标准库标准流缓冲扩展。
\version r55
\author FrankHB <frankhb1989@gmail.com>
\since build 636
\par 创建时间:
	2015-09-22 11:19:27 +0800
\par 修改时间:
	2015-09-22 11:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::StreamBuffer
*/


#ifndef YB_INC_ystdex_streambuf_hpp_
#define YB_INC_ystdex_streambuf_hpp_ 1

#include "algorithm.hpp" // for ystdex::equal;
#include <streambuf> // for std::basic_streambuf;
#include <iterator> // for std::istreambuf_iterator;

namespace ystdex
{

/*!
\brief 比较指定流缓冲区的内容相等。
\note 流读取失败视为截止。
\since build 636
*/
template<typename _tChar, class _tTraits>
bool
streambuf_equal(std::basic_streambuf<_tChar, _tTraits>& a,
	std::basic_streambuf<_tChar, _tTraits>& b)
{
	using iter_type = std::istreambuf_iterator<_tChar, _tTraits>;

	return ystdex::equal(iter_type(&a), iter_type(), iter_type(&b), iter_type());
}

} // namespace ystdex;

#endif

