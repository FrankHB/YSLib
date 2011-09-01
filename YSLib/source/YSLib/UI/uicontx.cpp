/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file uicontx.cpp
\ingroup UI
\brief 样式无关的图形用户界面附加容器。
\version r1063;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-02-21 09:01:13 +0800;
\par 修改时间:
	2011-09-01 22:08 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::UIContainerEx;
*/


#include "uicontx.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

AUIBoxControl::AUIBoxControl(const Rect& r)
	: Control(r), MSimpleFocusResponser()
{}

void
AUIBoxControl::ClearFocusingPtr()
{
	if(const auto p = GetFocusingPtr())
	{
		MSimpleFocusResponser::ClearFocusingPtr();
		CallEvent<LostFocus>(*p, *this, EventArgs());
	}
}

YSL_END_NAMESPACE(Components)

YSL_END

