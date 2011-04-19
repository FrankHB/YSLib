/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file uicontx.cpp
\ingroup Shell
\brief 样式无关的图形用户界面附加容器实现。
\version 0.1057;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-02-21 09:01:13 +0800;
\par 修改时间:
	2011-04-15 09:35 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::UIContainerEx;
*/


#include "uicontx.h"
#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

AUIBoxControl::AUIBoxControl(const Rect& r)
	: Control(r), MSimpleFocusResponser()
{}

void
AUIBoxControl::ClearFocusingPtr()
{
	IControl* const p(GetFocusingPtr());

	if(p)
	{
		MSimpleFocusResponser::ClearFocusingPtr();
		EventMap.GetEvent<HVisualEvent>(LostFocus)(*p,
			GetStaticRef<EventArgs>());
	}
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

