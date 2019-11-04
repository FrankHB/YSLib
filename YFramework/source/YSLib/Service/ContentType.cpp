/*
	© 2013-2016, 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ContentType.cpp
\ingroup Service
\brief 内容类型接口。
\version r118
\author FrankHB <frankhb1989@gmail.com>
\since build 449
\par 创建时间:
	2013-10-10 06:04:40 +0800
\par 修改时间:
	2019-11-04 17:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::ContentType
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_ContentType

namespace YSLib
{

void
MIMEBiMapping::operator+=(const pair<ExtensionType, MIMEType>& pr)
{
	ext_map.insert(pr),
	inv_map.emplace(pr.second, pr.first);
}
void
MIMEBiMapping::operator+=(const pair<MIMEType, ExtensionType>& pr)
{
	ext_map.emplace(pr.second, pr.first),
	inv_map.insert(pr);
}

void
MIMEBiMapping::operator-=(const pair<ExtensionType, MIMEType>& pr)
{
	ext_map.erase(pr.first),
	inv_map.erase(pr.second);
}
void
MIMEBiMapping::operator-=(const pair<MIMEType, ExtensionType>& pr)
{
	ext_map.erase(pr.second),
	inv_map.erase(pr.first);
}


void
AddMIMEItems(MIMEBiMapping& m, const ValueNode& node,
	MIMEBiMapping::MIMEType&& pth)
{
	if(!node.empty())
		for(auto& nd : node)
			try
			{
				pth.push_back(nd.GetName());
				AddMIMEItems(m, nd, std::move(pth));
				pth.pop_back();
			}
			CatchIgnore(bad_any_cast&)
	else if(const auto p = AccessPtr<string>(node))
		m += {*p, !pth.empty() && IsPrefixedIndex(pth.back())
			? MIMEBiMapping::MIMEType{pth.cbegin(), std::prev(pth.cend())}
			: pth};
}

} // namespace YSLib;

