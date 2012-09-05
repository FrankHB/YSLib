/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yref.hpp
\ingroup Adaptor
\brief 用于提供指针和引用访问的间接访问类模块。
\version r2620
\author FrankHB<frankhb1989@gmail.com>
\since build 176
\par 创建时间:
	2010-03-21 23:09:06 +0800
\par 修改时间:
	2012-09-04 12:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::YReference
*/


#ifndef INCLUDED_ADAPTOR_YREF_HPP_
#define INCLUDED_ADAPTOR_YREF_HPP_ 1

#include "yadaptor.h"
#include <utility>
#include <ystdex/memory.hpp>

YSL_BEGIN

using std::bad_weak_ptr;
using std::const_pointer_cast;
using std::dynamic_pointer_cast;
using std::enable_shared_from_this;
using std::get_deleter;
//! \since build 298
using ystdex::make_shared;
//! \since build 292
using ystdex::make_unique;
using ystdex::raw;
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
	p = nullptr;
	return b;
}
//@}

/*!
\brief 比较： shared_ptr 左值和内建指针类型的相等关系。
\since build 209
*/
template<typename _type>
bool
operator==(const shared_ptr<_type>& sp, _type* p)
{
	return sp.get() == p;
}
/*!
\brief 比较： shared_ptr 右值和内建指针类型的相等关系。
\since build 209
*/
template<typename _type>
bool
operator==(shared_ptr<_type>&& sp, _type* p)
{
	return sp.get() == p;
}

/*!
\brief 比较： shared_ptr 左值和内建指针类型的不等关系。
\since build 209
*/
template<typename _type>
bool
operator!=(const shared_ptr<_type>& sp, _type* p)
{
	return sp.get() != p;
}
/*!
\brief 比较： shared_ptr 右值和内建指针类型的不等关系。
\since build 209
*/
template<typename _type>
bool
operator!=(shared_ptr<_type>&& sp, _type* p)
{
	return sp.get() != p;
}

YSL_END

#endif

