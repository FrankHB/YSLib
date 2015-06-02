/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Reference.h
\ingroup YCLib
\brief 指针和引用访问操作模块。
\version r2775
\author FrankHB <frankhb1989@gmail.com>
\since build 593
\par 创建时间:
	2010-03-21 23:09:06 +0800
\par 修改时间:
	2015-06-01 16:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Reference
*/


#ifndef YCL_INC_Reference_h_
#define YCL_INC_Reference_h_ 1

#include "YModules.h"
#include <utility>
#include <ystdex/memory.hpp>
#include <ystdex/functional.hpp>

namespace platform
{

inline namespace references
{

using yimpl(std)::bad_weak_ptr;
using yimpl(std)::const_pointer_cast;
using yimpl(std)::dynamic_pointer_cast;
using yimpl(std)::enable_shared_from_this;
using yimpl(std)::get_deleter;
//! \since build 529
using yimpl(std)::make_shared;
//! \since build 298
using ystdex::make_shared;
//! \since build 292
using yimpl(ystdex)::make_unique;
//! \since build 601
using yimpl(ystdex)::make_unique_default_init;
//! \since build 422
using ystdex::get_raw;
//! \since build 454
using yimpl(std)::owner_less;
using ystdex::reset;
using ystdex::share_raw;
using yimpl(std)::shared_ptr;
using yimpl(std)::static_pointer_cast;
using ystdex::unique_raw;
using yimpl(std)::unique_ptr;
using yimpl(std)::weak_ptr;

//! \since build 554
using ystdex::lref;

} // inline namespace references;

} // namespace platform;

namespace platform_ex
{

//! \since build 593
using namespace platform::references;

} // namespace platform_ex;

#endif

