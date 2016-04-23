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
\version r297
\author FrankHB <frankhb1989@gmail.com>
\since build 352
\par 创建时间:
	2012-11-05 11:12:01 +0800
\par 修改时间:
	2016-04-23 08:37 +0800
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

invalid_construction::invalid_construction()
	: invalid_argument("Violation on construction found.")
{}

void
throw_invalid_construction()
{
	throw invalid_construction();
}


holder::~holder() = default;

} // namespace any_ops;


bad_any_cast::~bad_any_cast() = default;

const char*
bad_any_cast::from() const ynothrow
{
	return
		from_type() == ystdex::type_id<void>() ? "unknown" : from_type().name();
}

const char*
bad_any_cast::to() const ynothrow
{
	return to_type() == ystdex::type_id<void>() ? "unknown" : to_type().name();
}

const char*
bad_any_cast::what() const ynothrow
{
	return "Failed conversion: any_cast.";
}


namespace details
{

any_ops::any_storage&
any_base::call(any_ops::any_storage& t, any_ops::op_code op) const
{
	yconstraint(manager);

	manager(t, get_storage(), op);
	return t;
}

void
any_base::clear() ynothrowv
{
	yconstraint(manager);

	manager(storage, storage, any_ops::destroy);
	manager = {};
}

void
any_base::copy(const any_base& a)
{
	yconstraint(manager);

	manager(storage, a.storage, any_ops::clone);
}

void
any_base::destroy() ynothrowv
{
	yconstraint(manager);

	manager(storage, storage, any_ops::destroy);
}

void*
any_base::get() const ynothrowv
{
	return unchecked_access<void*>(any_ops::get_ptr);
}

any_ops::holder*
any_base::get_holder() const
{
	return unchecked_access<any_ops::holder*>(any_ops::get_holder_ptr);
}

void
any_base::swap(any_base& a) ynothrow
{
	std::swap(storage, a.storage),
	std::swap(manager, a.manager);
}

const type_info&
any_base::type() const ynothrowv
{
	return *unchecked_access<const type_info*>(any_ops::get_type);
}

} // namespace details;


any::any(const any& a)
	: any_base(a)
{
	if(manager)
		copy(a);
}
any::~any()
{
	if(manager)
		destroy();
}

void
any::clear() ynothrow
{
	if(manager)
		any_base::clear();
}

} // namespace ystdex;

