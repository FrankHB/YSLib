/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Mutex.h
\ingroup YCLib
\brief 互斥量。
\version r78
\author FrankHB <frankhb1989@gmail.com>
\since build 551
\par 创建时间:
	2014-11-04 05:17:14 +0800
\par 修改时间:
	2014-11-05 14:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Mutex
*/


#ifndef YCL_INC_Mutex_h_
#define YCL_INC_Mutex_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon
#if YF_Multithread == 1
#	include <ystdex/concurrency.h>
#	define YCL_Mutex_Namespace std
#	define YCL_Threading_Namespace ystdex
#else
#	include <ystdex/pseudo_mutex.h>
#	define YCL_Mutex_Namespace ystdex::single_thread
#	define YCL_Threading_Namespace ystdex::threading
#endif

namespace platform
{

//! \since build 551
namespace Concurrency
{

using YCL_Mutex_Namespace::mutex;
using YCL_Mutex_Namespace::recursive_mutex;
// TODO: Check %_GTHREAD_USE_MUTEX_TIMEDLOCK defined or %YCL_*.
//using YCL_Mutex_Namespace::timed_mutex;
//using YCL_Mutex_Namespace::recursive_timed_mutex;

using YCL_Mutex_Namespace::lock_guard;
using YCL_Mutex_Namespace::unique_lock;

using YCL_Mutex_Namespace::lock;
using YCL_Mutex_Namespace::try_lock;

} // namespace Concurrency;

//! \since build 551
namespace Threading
{

using YCL_Threading_Namespace::unlock_deleter;
using YCL_Threading_Namespace::locked_ptr;

} // namespace Threading;

} // namespace platform;

#undef YCL_Threading_Namespace
#undef YCL_Mutex_Namespace

#endif

