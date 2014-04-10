/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Hover.cpp
\ingroup UI
\brief 样式无关的指针设备悬停相关功能。
\version r60
\author FrankHB <frankhb1989@gmail.com>
\since build 448
\par 创建时间:
	2013-09-28 12:52:39 +0800
\par 修改时间:
	2014-04-06 17:27 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Hover
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Hover
#include YFM_YSLib_UI_YControl

namespace YSLib
{

namespace UI
{

HoverUpdater::HoverUpdater(IWidget& wgt)
	: Widget(wgt)
{
	yunseq(
	FetchEvent<Enter>(wgt) += [this]{
		entered = true,
		Invalidate(Widget);
	},
	FetchEvent<Leave>(wgt) += [this]{
		entered = false,
		Invalidate(Widget);
	}
	);
}

} // namespace UI;

} // namespace YSLib;

