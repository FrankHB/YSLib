/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MinGW32.cpp
\ingroup YCLib
\ingroup MinGW32
\brief YCLib MinGW32 平台扩展公共头文件。
\version r53
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 15:35:19 +0800
\par 修改时间:
	2013-07-10 15:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::MinGW32
*/


#include "YCLib/MinGW32.h"

using namespace YSLib;

namespace platform_ex
{

Exception::Exception(const std::string& s, LevelType l) ynothrow
	: LoggedEvent(s, l)
{}

namespace Windows
{

Win32Exception::Win32Exception(::DWORD e, const std::string& s, LevelType l)
	ynothrow
	: Exception(s, l),
	err(e)
{
	YAssert(e != 0, "No error should be thrown.");
}

} // namespace Windows;

} // namespace YSLib;

