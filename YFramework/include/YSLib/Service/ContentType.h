/*
	© 2013-2015, 2021, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ContentType.h
\ingroup Service
\brief 内容类型接口。
\version r132
\author FrankHB <frankhb1989@gmail.com>
\since build 449
\par 创建时间:
	2013-10-10 06:03:37 +0800
\par 修改时间:
	2023-02-20 18:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::ContentType
*/


#ifndef YSL_INC_Service_ContentType_h_
#define YSL_INC_Service_ContentType_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_ValueNode // for string, ystdex::path, vector,
//	value_multimap;

namespace YSLib
{

/*!
\brief MIME 和文件扩展名双向映射。
\warning 非虚析构。
\since build 449
\todo 使用 Boost.Bimap 类似接口简化实现。
*/
class YF_API MIMEBiMapping
{
public:
	using ExtensionType = string;
	using MIMEType = ystdex::path<vector<string>>;
	using ExtensionMap = value_multimap<ExtensionType, MIMEType>;
	using MIMEMap = value_multimap<MIMEType, ExtensionType>;
	//! \since build 968
	using allocator_type = typename MIMEMap::allocator_type;

private:
	ExtensionMap ext_map;
	MIMEMap inv_map;

public:
	DefDeCtor(MIMEBiMapping)
	//! \since build 968
	//!@{
	explicit
	MIMEBiMapping(allocator_type a)
		: ext_map(a), inv_map(a)
	{}
	template<typename _tIn>
	MIMEBiMapping(_tIn first, _tIn last, allocator_type a = {})
		: MIMEBiMapping(a)
	{
		std::for_each(first, last, [this](decltype(*first) pr){
			*this += pr;
		});
	}
	template<typename _tElem>
	MIMEBiMapping(std::initializer_list<_tElem> il, allocator_type a = {})
		: MIMEBiMapping(il.begin(), il.end(), a)
	{}
	//!@}

	DefDeMoveCtor(MIMEBiMapping)

	void
	operator+=(const pair<ExtensionType, MIMEType>&);
	void
	operator+=(const pair<MIMEType, ExtensionType>&);

	void
	operator-=(const pair<ExtensionType, MIMEType>&);
	void
	operator-=(const pair<MIMEType, ExtensionType>&);

	YB_ATTR_nodiscard
		DefGetter(const ynothrow, const ExtensionMap&, ExtensionMap, ext_map)
	YB_ATTR_nodiscard
		DefGetter(const ynothrow, const MIMEMap&, MIMEMap, inv_map)

	//! \since build 968
	YB_ATTR_nodiscard YB_PURE
		PDefH(allocator_type, get_allocator, ) const ynothrow
		ImplRet(inv_map.get_allocator())
};


/*!
\brief 根据 NPLA1 节点内容增加 MIME 映射。
\since build 450
*/
YF_API void
AddMIMEItems(MIMEBiMapping&, const ValueNode&, MIMEBiMapping::MIMEType&& = {});

} // namespace YSLib;

#endif

