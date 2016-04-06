/*
	© 2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file typeinfo.h
\ingroup YStandardEx
\brief ISO C++ 类型信息扩展。
\version r161
\author FrankHB <frankhb1989@gmail.com>
\since build 247
\par 创建时间:
	2016-04-01 08:56:47 +0800
\par 修改时间:
	2016-04-01 12:25 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TypeInfo
*/


#ifndef YB_INC_ystdex_typeinfo_h_
#define YB_INC_ystdex_typeinfo_h_ 1

#include "base.h" // for noncopyable;
#include "type_traits.hpp" // for is_void;
#include <typeinfo> // for std::type_info, typeid;
#include <functional> // for std::hash;

namespace ystdex
{

//! \since build 683
//@{
/*!
\brief 类型信息。
\note 类似 std::type_info ，但不使用 \c name 时可避免引入冗余符号。
\warning 非虚析构。
*/
class YB_API type_id_info : private yimpl(noncopyable)
{
public:
	using id_t = size_t;

private:
	id_t id;

	yimpl(yconstfn)
	type_id_info(id_t i = 0)
		: id(i)
	{}

public:
	yimpl(yconstfn) bool
	operator==(const type_id_info& rhs) const ynothrow
	{
		return id == rhs.id;
	}

	yimpl(yconstfn) bool
	operator!=(const type_id_info& rhs) const ynothrow
	{
		return id != rhs.id;
	}

	yimpl(yconstfn) bool
	before(const type_id_info& rhs) const ynothrow
	{
		return id < rhs.id;
	}

	id_t
	get() const ynothrow
	{
		return id;
	}

	size_t
	hash_code() const ynothrow
	{
		return std::hash<id_t>()(id);
	}

	template<typename _type>
	static const type_id_info&
	make()
	{
		return is_void<_type>() ? xref_void() : xref<remove_cv_t<_type>>();
	}

	yimpl(yconstfn) const char*
	name() const ynothrow
	{
		return "<unknown>";
	}

private:
	static id_t
	push()
	{
		static id_t id;

		return ++id;
	}

	template<typename _type>
	static const type_id_info&
	xref()
	{
		static_assert(is_same<_type, remove_cv_t<_type>>(),
			"Invalid type found.");
		static type_id_info info(push());

		return info;
	}

	static const type_id_info&
	xref_void()
	{
		static type_id_info info;

		return info;
	}
};


/*!
\def YB_Use_LightweightTypeID
\brief 使用轻量类型标识代替 typeid 。
\warning 不依赖 RTTI ；跨模块使用行为依赖实现，不保证唯一性。
\since build 683
*/
#if YB_Use_LightweightTypeID
using type_info = type_id_info;
#else
using std::type_info;
#endif


//! \sa type_info
template<typename _type>
const type_info&
type_id()
{
#if YB_Use_LightweightTypeID
	return type_info::make<_type>();
#else
	return typeid(_type);
#endif
}
//@}

} // namespace ystdex;

#endif

