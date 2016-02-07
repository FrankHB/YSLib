/*
	© 2010-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Reference.h
\ingroup YCLib
\brief 指针和引用访问操作模块。
\version r2792
\author FrankHB <frankhb1989@gmail.com>
\since build 593
\par 创建时间:
	2010-03-21 23:09:06 +0800
\par 修改时间:
	2016-02-07 17:29 +0800
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
#include <ystdex/ref.hpp>
#include <ystdex/pointer.hpp>

namespace platform
{

inline namespace references
{

using yimpl(std)::bad_weak_ptr;
using yimpl(std)::const_pointer_cast;
//! \since build 669
using yimpl(std)::default_delete;
using yimpl(std)::dynamic_pointer_cast;
using yimpl(std)::enable_shared_from_this;
using yimpl(std)::get_deleter;
//! \since build 529
using yimpl(std)::make_shared;
//! \since build 298
using ystdex::make_shared;
//! \since build 669
using yimpl(ystdex)::make_observer;
//! \since build 292
using yimpl(ystdex)::make_unique;
//! \since build 602
using yimpl(ystdex)::make_unique_default_init;
//! \since build 422
using ystdex::get_raw;
//! \since build 454
using yimpl(std)::owner_less;
//! \since build 669
using yimpl(ystdex)::observer_ptr;
using ystdex::reset;
using ystdex::share_raw;
using yimpl(std)::shared_ptr;
using yimpl(std)::static_pointer_cast;
using ystdex::unique_raw;
using yimpl(std)::unique_ptr;
using yimpl(std)::weak_ptr;

//! \since build 554
using ystdex::lref;

//! \since build 669
//@{
using ystdex::nptr;
template<typename _type>
using tidy_ptr = nptr<observer_ptr<_type>>;
using ystdex::pointer_iterator;
//@}

} // inline namespace references;

} // namespace platform;

namespace platform_ex
{

//! \since build 593
using namespace platform::references;

} // namespace platform_ex;

#endif

