/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBlend.cpp
\ingroup Service
\brief 平台无关的图像块操作。
\version r56
\author FrankHB <frankhb1989@gmail.com>
\since build 584
\par 创建时间:
	2015-03-17 06:19:55 +0800
\par 修改时间:
	2015-03-17 06:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YBlend
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_YBlend

using namespace ystdex;

namespace YSLib
{

namespace Drawing
{

void
BlendRect(const Graphics& g, const Rect& r, Color c)
{
	YAssert(bool(g), "Invalid graphics context found.");
	BlendRectRaw(g.GetBufferPtr(), Pixel(c), g.GetSize(), r);
}


void
CompositeRect(const Graphics& g, const Rect& r, Color c)
{
	YAssert(bool(g), "Invalid graphics context found.");
	CompositeRectRaw(g.GetBufferPtr(), Pixel(c), g.GetSize(), r);
}

} // namespace Drawing;

} // namespace YSLib;

