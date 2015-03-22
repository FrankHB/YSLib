/*
	© 2009-2013, 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YResource.cpp
\ingroup Service
\brief 应用程序资源管理模块。
\version r280
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-22 17:28:28 +0800
\par 修改时间:
	2015-03-21 18:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YResource
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_YResource
#include YFM_YSLib_Core_YShell

namespace YSLib
{

ImplDeDtor(IResource)

namespace Drawing
{

Image::Image(ConstBitmapPtr s, SDst w, SDst h)
	: CompactPixmap(s, w, h)
{}
Image::Image(const CompactPixmap& buf)
	: CompactPixmap(buf)
{}
Image::Image(CompactPixmap&& buf)
	: CompactPixmap(std::move(buf))
{}
ImplDeDtor(Image)

} // namespace Drawing;

} // namespace YSLib;

