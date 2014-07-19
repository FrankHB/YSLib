/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file test.cpp
\ingroup YTest
\brief 基础测试工具。
\version r53
\author FrankHB <frankhb1989@gmail.com>
\since build 519
\par 创建时间:
	2014-07-17 03:59:38 +0800
\par 修改时间:
	2014-07-17 04:22 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YTest::Test
*/


#include "ytest/test.h"

namespace ytest
{

group_guard::group_guard(const std::string& name)
	: subject(name)
{}
group_guard::group_guard(const std::string& name, std::function<void(
	group_guard&)> on_b, std::function<void(group_guard&)> on_e)
	: subject(name), on_end(on_e)
{
	if(on_b)
		on_b(*this);
}
group_guard::~group_guard()
{
	try
	{
		on_end(*this);
	}
	catch(...)
	{}
}

} // namespace ytest;

