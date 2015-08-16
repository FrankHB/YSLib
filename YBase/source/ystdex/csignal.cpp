/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file csignal.cpp
\ingroup YStandardEx
\brief ISO C 标准信号扩展操作。
\version r40
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2015-08-13 10:17:21 +0800
\par 修改时间:
	2015-08-13 10:17 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CSignal
*/


#include "ystdex/csignal.h"

namespace ystdex
{

void
reraise(int sig) ynothrow
{
	restore_signal(sig);
	std::raise(sig);
}

} // namespace ystdex;

