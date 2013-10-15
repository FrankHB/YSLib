/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ContentType.cpp
\ingroup Service
\brief 内容类型接口。
\version r85
\author FrankHB <frankhb1989@gmail.com>
\since build 449
\par 创建时间:
	2013-10-10 06:04:40 +0800
\par 修改时间:
	2013-10-12 12:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::ContentType
*/


#include "YSLib/Service/ContentType.h"

namespace YSLib
{

void
MIMEBiMapping::operator+=(const pair<ExtensionType, MIMEType>& pr)
{
	ext_map.insert(pr),
	inv_map.insert(make_pair(pr.second, pr.first));
}
void
MIMEBiMapping::operator+=(const pair<MIMEType, ExtensionType>& pr)
{
	ext_map.insert(make_pair(pr.second, pr.first)),
	inv_map.insert(pr);
}

void
MIMEBiMapping::operator-=(const pair<ExtensionType, MIMEType>& pr)
{
	ext_map.insert(pr),
	inv_map.insert(make_pair(pr.second, pr.first));
}
void
MIMEBiMapping::operator-=(const pair<MIMEType, ExtensionType>& pr)
{
	ext_map.insert(make_pair(pr.second, pr.first)),
	inv_map.insert(pr);
}


void
AddMIMEItems(MIMEBiMapping& m, const ValueNode& node,
	MIMEBiMapping::MIMEType&& pth)
{
	if(node.GetContainerPtr())
		for(auto& nd : node)
			try
			{
				pth.push_back(nd.GetName());
				AddMIMEItems(m, nd, std::move(pth));
				pth.pop_back();
			}
			catch(ystdex::bad_any_cast&)
			{}
	else
		try
		{
			m += make_pair(Access<string>(node), !pth.empty()
				&& IsPrefixedIndex(pth.back()) ? MIMEBiMapping::MIMEType{
				pth.cbegin(), std::prev(pth.cend())} : pth);
		}
		catch(ystdex::bad_any_cast&)
		{}
}

} // namespace YSLib;

