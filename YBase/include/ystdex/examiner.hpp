/*
	© 2012-2013, 2015-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file examiner.hpp
\ingroup YStandardEx
\brief C++ 类型操作检测。
\version r110
\author FrankHB <frankhb1989@gmail.com>
\since build 348
\par 创建时间:
	2012-10-17 01:21:01 +0800
\par 修改时间:
	2016-02-03 22:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Examiner
*/


#ifndef YB_INC_ystdex_examiner_hpp_
#define YB_INC_ystdex_examiner_hpp_ 1

#include "../ydef.h" // for ynoexcept_spec;

namespace ystdex
{

/*!
\brief 操作检测命名空间。
\since build 348
*/
namespace examiners
{

/*!
\brief 基本等于操作检测。
\since build 348
*/
struct equal
{
	//! \since build 668
	template<typename _type1, typename _type2>
	static yconstfn YB_PURE auto
	are_equal(_type1&& x, _type2&& y)
		ynoexcept_spec(bool(x == y)) -> decltype(bool(x == y))
	{
		return bool(x == y);
	}
};


/*!
\brief 基本等于操作检测：总是相等。
\since build 348
*/
struct always_equal
{
	//! \since build 668
	template<typename _type, typename... _types>
	static yconstfn YB_STATELESS bool
	are_equal(const _type&, yimpl(_types&&...)) ynothrow
	{
		return true;
	}
};


/*!
\brief 等于操作检测。
\since build 348
*/
struct equal_examiner : public equal, public always_equal
{
	using equal::are_equal;
	using always_equal::are_equal;
};

} // namespace examiners;

} // namespace ystdex;

#endif

