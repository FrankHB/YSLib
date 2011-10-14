/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yref.hpp
\ingroup Adaptor
\brief 用于提供指针和引用访问的间接访问类模块。
\version r3556;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-21 23:09:06 +0800;
\par 修改时间:
	2011-09-12 23:42 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YReference;
*/


#ifndef INCLUDED_ADAPTOR_YREF_HPP_
#define INCLUDED_ADAPTOR_YREF_HPP_

#include "yadaptor.h"
#include <memory>
#include <utility>
#include <ystdex/memory.hpp>

YSL_BEGIN

using std::bad_weak_ptr;
using std::const_pointer_cast;
using std::dynamic_pointer_cast;
using std::enable_shared_from_this;
using std::get_deleter;
using std::shared_ptr;
using std::static_pointer_cast;
using std::unique_ptr;
using std::weak_ptr;
using ystdex::is_not_null;
using ystdex::is_null;
using ystdex::raw;
using ystdex::reset;
using ystdex::share_raw;
using ystdex::unique_raw;


/*!	\defgroup reset Reset Pointers
\brief 安全删除指定引用的句柄指向的对象。
\post 指定引用的句柄值等于 nullptr 。
*/
//@{
PDefTH1(_type)
inline bool
reset(_type*& p) ynothrow
{
	bool b(p);

	ydelete(p);
	p = nullptr;
	return b;
}
//@}

PDefTH1(_type)
bool
operator==(const shared_ptr<_type>& sp, _type* p)
{
	return sp.get() == p;
}
PDefTH1(_type)
bool
operator==(shared_ptr<_type>&& sp, _type* p)
{
	return sp.get() == p;
}

PDefTH1(_type)
bool
operator!=(const shared_ptr<_type>& sp, _type* p)
{
	return sp.get() != p;
}
PDefTH1(_type)
bool
operator!=(shared_ptr<_type>&& sp, _type* p)
{
	return sp.get() != p;
}

YSL_END

#endif

