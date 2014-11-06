/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yref.hpp
\ingroup Adaptor
\brief 用于提供指针和引用访问的间接访问类模块。
\version r2668
\author FrankHB <frankhb1989@gmail.com>
\since build 176
\par 创建时间:
	2010-03-21 23:09:06 +0800
\par 修改时间:
	2014-11-05 01:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::YReference
*/


#ifndef YSL_INC_Adaptor_yref_hpp_
#define YSL_INC_Adaptor_yref_hpp_ 1

#include "YModules.h"
#include YFM_YSLib_Adaptor_YAdaptor
#include <utility>
#include <ystdex/memory.hpp>

namespace YSLib
{

using std::bad_weak_ptr;
using std::const_pointer_cast;
using std::dynamic_pointer_cast;
using std::enable_shared_from_this;
using std::get_deleter;
//! \since build 529
using std::make_shared;
//! \since build 298
using ystdex::make_shared;
//! \since build 292
using ystdex::make_unique;
//! \since build 422
using ystdex::get_raw;
//! \since build 454
using std::owner_less;
using ystdex::reset;
using ystdex::share_raw;
using std::shared_ptr;
using std::static_pointer_cast;
using ystdex::unique_raw;
using std::unique_ptr;
using std::weak_ptr;


/*!	\defgroup reset Reset Pointers
\brief 安全删除指定引用的句柄指向的对象。
\post 指定引用的句柄值等于 nullptr 。
\since build 209
*/
//@{
template<typename _type>
inline bool
reset(_type*& p) ynothrow
{
	bool b(p);

	ydelete(p);
	p = {};
	return b;
}
//@}

/*!
\brief 比较： shared_ptr 和内建指针类型的相等关系。
\since build 209
*/
template<typename _type>
inline bool
operator==(const shared_ptr<_type>& sp, _type* p)
{
	return sp.get() == p;
}
/*!
\brief 比较： weak_ptr 相等关系。
\note 注意和 shared_ptr 比较托管对象指针的语义不同。
\since build 454
*/
template<typename _type1, typename _type2>
inline bool
operator==(const weak_ptr<_type1>& x, const weak_ptr<_type2>& y)
{
	return !x.owner_before(y) && !y.owner_before(x);
}

/*!
\brief 比较： shared_ptr 左值和内建指针类型的不等关系。
\since build 209
*/
template<typename _type>
inline bool
operator!=(const shared_ptr<_type>& sp, _type* p)
{
	return sp.get() != p;
}
/*!
\brief 比较： weak_ptr 不等关系。
\note 注意和 shared_ptr 比较托管对象指针的语义不同。
\since build 209
*/
template<typename _type1, typename _type2>
inline bool
operator!=(const weak_ptr<_type1>& x, const weak_ptr<_type2>& y)
{
	return !(x == y);
}

/*!
\brief 解锁删除器：使用线程模型对应的互斥量和锁。
\since build 551
*/
using platform::Threading::unlock_deleter;


/*!
\brief 独占所有权的锁定指针：使用线程模型对应的互斥量和锁以及 YSLib::unique_ptr 。
\sa threading::locked_ptr
\since build 551
*/
template<typename _type, class _tMutex = typename unlock_deleter<>::mutex_type,
	class _tLock = typename unlock_deleter<_tMutex>::lock_type>
using locked_ptr = unique_ptr<_type, unlock_deleter<_tMutex, _tLock>>;

} // namespace YSLib;

#endif

