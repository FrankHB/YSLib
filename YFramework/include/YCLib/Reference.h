﻿/*
	© 2010-2019, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Reference.h
\ingroup YCLib
\brief 指针和引用访问操作模块。
\version r2864
\author FrankHB <frankhb1989@gmail.com>
\since build 593
\par 创建时间:
	2010-03-21 23:09:06 +0800
\par 修改时间:
	2023-03-30 12:12 +0800
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

namespace platform
{

inline namespace references
{

/*!
\note 支持 WG21 P0674R1 对 LWG 2070 的解。
\since build 847
*/
using yimpl(ystdex)::allocate_shared;
//! \since build 847
using ystdex::allocate_unique;
//! \since build 971
using ystdex::allocator_delete;
//! \since build 971
using ystdex::allocator_guard_delete;
using yimpl(std)::bad_weak_ptr;
using yimpl(std)::const_pointer_cast;
//! \since build 669
using yimpl(std)::default_delete;
using yimpl(std)::dynamic_pointer_cast;
using yimpl(std)::enable_shared_from_this;
using yimpl(std)::get_deleter;
//! \since build 422
using ystdex::get_raw;
//! \since build 669
using yimpl(ystdex)::make_observer;
//! \since build 298
using ystdex::make_shared;
//! \since build 292
using yimpl(ystdex)::make_unique;
//! \since build 602
using yimpl(ystdex)::make_unique_default_init;
//! \since build 779
using ystdex::make_weak;
//! \since build 669
using yimpl(ystdex)::observer_ptr;
//! \since build 454
using yimpl(std)::owner_less;
//! \since build 784
using ystdex::owns_any;
//! \since build 784
using ystdex::owns_nonnull;
//! \since build 759
using ystdex::owns_unique;
//! \since build 759
using ystdex::owns_unique_nonnull;
using ystdex::reset;
//! \since build 783
//!@{
using ystdex::share_copy;
using ystdex::share_forward;
using ystdex::share_move;
//!@}
using ystdex::share_raw;
using yimpl(std)::shared_ptr;
using yimpl(std)::static_pointer_cast;
//! \since build 783
//!@{
using ystdex::unique_copy;
using ystdex::unique_forward;
using ystdex::unique_move;
//!@}
using ystdex::unique_raw;
using yimpl(std)::unique_ptr;
/*!
\ingroup metafunctions
\brief 取删除器对应的 unique_ptr 实例。
\sa ystdex::defer_element
\sa ystdex::unique_ptr_pointer
\since build 671

元素类型通过 ystdex::unique_ptr_pointer 按指定的删除器推断，若失败则为第二参数。
*/
template<typename _tDeleter, typename _tDefault = void>
using unique_ptr_from = unique_ptr<ystdex::defer_element<
	ystdex::unique_ptr_pointer<void, _tDeleter>, _tDefault>, _tDeleter>;
using yimpl(std)::weak_ptr;

//! \since build 554
using ystdex::lref;

//! \since build 669
//!@{
using ystdex::nptr;
using ystdex::pointer_iterator;
//!@}
//! \since build 755
using ystdex::tidy_ptr;

} // inline namespace references;

} // namespace platform;

namespace platform_ex
{

//! \since build 593
using namespace platform::references;

} // namespace platform_ex;

#endif

