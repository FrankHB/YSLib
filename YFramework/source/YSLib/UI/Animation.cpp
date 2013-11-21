/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Animation.cpp
\ingroup UI
\brief 样式无关的动画实现。
\version r110
\author FrankHB <frankhb1989@gmail.com>
\since build 443
\par 创建时间:
	2013-10-06 22:12:10 +0800
\par 修改时间:
	2013-11-21 20:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Animation
*/


#include "YSLib/UI/Animation.h"

namespace YSLib
{

namespace UI
{

bool
InvalidationUpdater::operator()() const
{
	if(bool(Invalidate) && !Invalidate(WidgetPtr))
		Ready = false;
	return Ready;
}

bool
InvalidationUpdater::DefaultInvalidate(IWidget* p_wgt)
{
	if(p_wgt)
	{
		InvalidateVisible(*p_wgt);
		return true;
	}
	return false;
}

} // namespace UI;

} // namespace YSLib;

