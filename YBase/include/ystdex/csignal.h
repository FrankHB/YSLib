/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file csignal.h
\ingroup YStandardEx
\brief ISO C 标准信号扩展操作。
\version r58
\author FrankHB <frankhb1989@gmail.com>
\since build 623
\par 创建时间:
	2015-08-13 10:15:01 +0800
\par 修改时间:
	2015-08-13 10:18 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CSignal
*/


#ifndef YB_INC_ystdex_csignal_h_
#define YB_INC_ystdex_csignal_h_ 1

#include "../ydef.h"
#include <csignal>

namespace ystdex
{

//! \since build 623
//@{
//! \brief 调用 \c signal 重置默认信号处理器。
inline void
restore_signal(int sig) ynothrow
{
	std::signal(sig, SIG_DFL);
}

/*!
\brief 调用 restore_signal 重置默认信号处理器后调用 \c raise 引发信号。
\note 异步信号安全。
\note 仅在确保依赖 POSIX 或其它扩展定义的确定行为时允许在多线程环境下使用。
\warning  ISO C11 指定多线程环境调用 \c signal 行为未定义。
*/
YB_API void
reraise(int sig) ynothrow;
//@}

} // namespace ystdex;

#endif

