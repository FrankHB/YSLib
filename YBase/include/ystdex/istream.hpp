/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file istream.hpp
\ingroup YStandardEx
\brief ISO C++ 标准库输入流扩展。
\version r55
\author FrankHB <frankhb1989@gmail.com>
\since build 636
\par 创建时间:
	2015-09-22 11:10:45 +0800
\par 修改时间:
	2015-09-22 11:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::InputStream
*/


#ifndef YB_INC_ystdex_istream_hpp_
#define YB_INC_ystdex_istream_hpp_ 1

#include "algorithm.hpp" // for ystdex::equal;
#include <istream> // for std::basic_istream;
#include <iterator> // for std::istream_iterator;

namespace ystdex
{

/*!
\brief 比较指定流的内容相等。
\note 不设置流状态，需要注意是否设置忽略空白符。
\since build 636
*/
template<typename _type, typename _tChar, class _tTraits>
bool
stream_equal(std::basic_istream<_tChar, _tTraits>& a,
	std::basic_istream<_tChar, _tTraits>& b)
{
	using iter_type = std::istream_iterator<_type, _tChar, _tTraits>;

	return ystdex::equal(iter_type(a), iter_type(), iter_type(b), iter_type());
}

} // namespace ystdex;

#endif

