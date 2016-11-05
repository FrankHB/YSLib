﻿/*
	© 2014, 2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file exception.cpp
\ingroup YStandardEx
\brief 标准库异常扩展接口。
\version r49
\author FrankHB <frankhb1989@gmail.com>
\since build 556
\par 创建时间:
	2014-11-28 12:15:16 +0800
\par 修改时间:
	2016-11-03 21:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Exception
*/


#include "ystdex/exception.h"

namespace ystdex
{

unsupported::~unsupported() = default;


unimplemented::~unimplemented() = default;


narrowing_error::~narrowing_error() = default;


invalid_construction::invalid_construction()
	: invalid_argument("Violation on construction found.")
{}

invalid_construction::~invalid_construction() = default;

void
throw_invalid_construction()
{
	throw invalid_construction();
}

} // namespace ystdex;

