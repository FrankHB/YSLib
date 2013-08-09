/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yres.cpp
\ingroup Service
\brief 应用程序资源管理模块。
\version r265
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-22 17:28:28 +0800
\par 修改时间:
	2013-08-05 21:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YResource
*/


#include "YSLib/Service/yres.h"
#include "YSLib/Core/yshell.h"

namespace YSLib
{

namespace Drawing
{

Image::Image(ConstBitmapPtr s, SDst w, SDst h)
	: CompactPixmap(s, w, h)
{}

} // namespace Drawing;

} // namespace YSLib;

