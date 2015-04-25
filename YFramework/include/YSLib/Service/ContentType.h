/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ContentType.h
\ingroup Service
\brief 内容类型接口。
\version r103
\author FrankHB <frankhb1989@gmail.com>
\since build 449
\par 创建时间:
	2013-10-10 06:03:37 +0800
\par 修改时间:
	2015-04-24 03:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::ContentType
*/


#ifndef YSL_INC_Service_ContentType_h_
#define YSL_INC_Service_ContentType_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_ValueNode
#include <ystdex/path.hpp>

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
	using ExtensionMap = multimap<ExtensionType, MIMEType>;
	using MIMEMap = multimap<MIMEType, ExtensionType>;

private:
	ExtensionMap ext_map;
	MIMEMap inv_map;

public:
	DefDeCtor(MIMEBiMapping)
	template<typename _tIn>
	MIMEBiMapping(_tIn first, _tIn last)
	{
		std::for_each(first, last, [this](decltype(*first) pr){
			*this += pr;
		});
	}
	template<typename _type>
	MIMEBiMapping(std::initializer_list<_type> il)
		: MIMEBiMapping(il.begin(), il.end())
	{}

	DefDeMoveCtor(MIMEBiMapping)

	void
	operator+=(const pair<ExtensionType, MIMEType>&);
	void
	operator+=(const pair<MIMEType, ExtensionType>&);

	void
	operator-=(const pair<ExtensionType, MIMEType>&);
	void
	operator-=(const pair<MIMEType, ExtensionType>&);

	DefGetter(const ynothrow, const ExtensionMap&, ExtensionMap, ext_map)
	DefGetter(const ynothrow, const MIMEMap&, MIMEMap, inv_map)
};


/*!
\brief 根据 NPLA1 节点内容增加 MIME 映射。
\since build 450
*/
YF_API void
AddMIMEItems(MIMEBiMapping&, const ValueNode&, MIMEBiMapping::MIMEType&& = {});

} // namespace YSLib;

#endif

