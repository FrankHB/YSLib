/*
	© 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file typeindex.h
\ingroup YStandardEx
\brief ISO C++ 类型索引扩展。
\version r110
\author FrankHB <frankhb1989@gmail.com>
\since build 892
\par 创建时间:
	2020-06-12 20:28:55 +0800
\par 修改时间:
	2020-06-12 21:01 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TypeIndex
*/


#ifndef YB_INC_ystdex_typeindex_h_
#define YB_INC_ystdex_typeindex_h_ 1

#include "typeinfo.h" // for "type_info.h", type_id_info,
//	YB_Use_LightweightTypeID, std::hash;
#include <typeindex> // for <typeindex>, std::type_index;
#include "operators.hpp" // for totally_ordered;

namespace ystdex
{

//! \since build 892
//@{
/*!
\brief 类型索引。
\note 类似 std::type_index ，但包装 type_id_info 而不是 std::type_info 。
\warning 非虚析构。
*/
class YB_API type_id_index : private totally_ordered<type_id_index>
{
private:
	const type_id_info* target;

public:
	type_id_index(const type_id_info& info) ynothrow
		: target(&info)
	{}

	YB_ATTR_nodiscard size_t
	hash_code() const ynothrow
	{
		return target->hash_code();
	}

	YB_ATTR_nodiscard YB_ATTR_returns_nonnull const char*
	name() const ynothrow
	{
		return target->name();
	}

	YB_ATTR_nodiscard yimpl(yconstfn) friend bool
	operator==(const type_id_index& x, const type_id_index& y) ynothrow
	{
		return *x.target == *y.target;
	}

	YB_ATTR_nodiscard yimpl(yconstfn) friend bool
	operator<(const type_id_index& x, const type_id_index& y) ynothrow
	{
		return x.target->before(*y.target);
	}
};


#if YB_Use_LightweightTypeID
using type_index = type_id_index;
#else
using std::type_index;
#endif
//@}

} // namespace ystdex;

namespace std
{

/*!
\brief ystdex::type_id_info 散列支持。
\since build 892
*/
template<>
struct hash<ystdex::type_id_index>
{
	YB_ATTR_nodiscard YB_PURE size_t
	operator()(const ystdex::type_id_index& k) const ynothrow
	{
		return k.hash_code();
	}
};

}

#endif

