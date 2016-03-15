/*
	© 2012-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any.cpp
\ingroup YStandardEx
\brief 动态泛型类型。
\version r197
\author FrankHB <frankhb1989@gmail.com>
\since build 352
\par 创建时间:
	2012-11-05 11:12:01 +0800
\par 修改时间:
	2016-03-15 10:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Any
*/


#include "ystdex/any.h"

namespace ystdex
{

namespace any_ops
{

holder::~holder() = default;

} // namespace any_ops;


//! \since build 677
namespace
{

template<typename _type>
_type
unchecked_access(any_ops::any_manager m, const any_ops::any_storage& s,
	any_ops::base_op op)
{
	any_ops::any_storage t;

	yconstraint(m);
	m(t, s, op);
	return t.access<_type>();
}

} // unnamed namespace;


bad_any_cast::~bad_any_cast() = default;

const char*
bad_any_cast::from() const ynothrow
{
	return from_type() == typeid(void) ? "unknown" : from_type().name();
}

const char*
bad_any_cast::to() const ynothrow
{
	return to_type() == typeid(void) ? "unknown" : to_type().name();
}

const char*
bad_any_cast::what() const ynothrow
{
	return "Failed conversion: any_cast.";
}


any::any(const any& a)
	: any()
{
	if(a.manager)
	{
		manager = a.manager,
		a.manager(storage, a.storage, any_ops::clone);
	}
}
any::~any()
{
	if(manager)
		manager(storage, storage, any_ops::destroy);
}

void
any::clear() ynothrow
{
	if(manager)
	{
		manager(storage, storage, any_ops::destroy);
		manager = {};
	}
}

void
any::swap(any& a) ynothrow
{
	std::swap(storage, a.storage),
	std::swap(manager, a.manager);
}

void*
any::unchecked_get() const ynothrowv
{
	return unchecked_access<void*>(manager, storage, any_ops::get_ptr);
}

any_ops::holder*
any::unchecked_get_holder() const
{
	return unchecked_access<any_ops::holder*>(manager, storage,
		any_ops::get_holder_ptr);
}

const std::type_info&
any::unchecked_type() const ynothrowv
{
	return *unchecked_access<const std::type_info*>(manager, storage,
		any_ops::get_type);
}

} // namespace ystdex;

