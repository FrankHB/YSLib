/*
	© 2009-2013, 2015-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YObject.cpp
\ingroup Core
\brief 平台无关的基础对象。
\version r900
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2017-05-09 10:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YObject
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YObject // for ystdex::invoke_value_or;
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;

namespace YSLib
{

ImplDeDtor(IValueHolder)


//! \since build 752
namespace
{

//! \since build 759
inline PDefH(IValueHolder&, HolderDownCast, ystdex::any_ops::holder& h)
	ynothrowv
	ImplRet(ystdex::polymorphic_downcast<IValueHolder&>(h))
//! \since build 759
inline PDefH(const IValueHolder&, HolderDownCast,
	const ystdex::any_ops::holder& h) ynothrowv
	ImplRet(ystdex::polymorphic_downcast<const IValueHolder&>(h))

inline PDefH(bool, HolderEquals, ystdex::any_ops::holder& h, const void* p)
	ImplRet(HolderDownCast(h).Equals(p))

} // unnamed namespace;

IValueHolder*
ValueObject::GetHolderPtr() const
{
	return ystdex::polymorphic_downcast<IValueHolder*>(content.get_holder());
}
IValueHolder&
ValueObject::GetHolderRef() const
{
	return HolderDownCast(Deref(content.get_holder()));
}

ValueObject
ValueObject::Create(IValueHolder::Creation c) const
{
	return ystdex::call_value_or([c](const IValueHolder& h){
		return ValueObject(h, c);
	}, GetHolderPtr());
}

bool
ValueObject::Equals(const ValueObject& x) const
{
	const auto p_holder(content.get_holder());
 
	if(const auto q = x.content.get_holder())
		return p_holder == q || (p_holder && p_holder->type() == q->type()
			&& HolderEquals(*p_holder, q->get()));
	return !p_holder;
}

bool
ValueObject::EqualsRaw(const void* p) const
{
	if(const auto p_holder = content.get_holder())
		return HolderEquals(*p_holder, p);
	return !p;
}

bool
ValueObject::EqualsUnchecked(const void* p) const
{
	return HolderEquals(GetHolderRef(), p);
}

size_t
ValueObject::OwnsCount() const ynothrow
{
	return ystdex::invoke_value_or(&IValueHolder::OwnsCount, GetHolderPtr());
}

} // namespace YSLib;

