/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any.cpp
\ingroup YStandardEx
\brief 动态泛型类型。
\version r115
\author FrankHB<frankhb1989@gmail.com>
\since build 352
\par 创建时间:
	2012-11-05 11:12:01 +0800
\par 修改时间:
	2012-11-08 17:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Any
*/


#include "ystdex/any.h"

namespace ystdex
{

any::any(const any& a)
	: any()
{
	if(a)
	{
		manager = a.manager,
		a.manager(storage, a.storage, any_ops::clone);
	}
}
any::~any() ynothrow
{
	if(manager)
		manager(storage, storage, any_ops::destroy);
}

void*
any::get() const ynothrow
{
	if(manager)
	{
		any_ops::any_storage t;

		manager(t, storage, any_ops::get_ptr);
		return t.access<void*>();
	}
	return nullptr;
}

any_ops::holder*
any::get_holder() const
{
	if(manager)
	{
		any_ops::any_storage t;

		manager(t, storage, any_ops::get_holder_ptr);
		return t.access<any_ops::holder*>();
	}
	return nullptr;
}

void
any::clear() ynothrow
{
	if(manager)
	{
		manager(storage, storage, any_ops::destroy);
		manager = nullptr;
	}
}

void
any::swap(any& a) ynothrow
{
	std::swap(storage, a.storage),
	std::swap(manager, a.manager);
}

const std::type_info&
any::type() const ynothrow
{
	if(manager)
	{
		any_ops::any_storage t;

		manager(t, storage, any_ops::get_type);
		return *t.access<const std::type_info*>();
	}
	return typeid(void);
}

} // namespace ystdex;

