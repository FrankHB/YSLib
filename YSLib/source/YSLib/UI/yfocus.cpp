/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfocus.cpp
\ingroup Shell
\brief 图形用户界面焦点特性。
\version 0.1388;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-01 13:52:56 +0800;
\par 修改时间:
	2011-05-14 20:38 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YFocus;
*/


#include "yfocus.h"
#include "ycontrol.h"
#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

void
MSimpleFocusResponser::ClearFocusingPtr()
{
	pFocusing = nullptr;
}

bool
MSimpleFocusResponser::ResponseFocusRequest(AFocusRequester& w)
{
	pFocusing = dynamic_cast<IControl*>(&w);
	return pFocusing;
}

bool
MSimpleFocusResponser::ResponseFocusRelease(AFocusRequester& w)
{
	if(pFocusing == dynamic_cast<IControl*>(&w))
	{
		pFocusing = nullptr;
	//	w.ReleaseFocusRaw();
		return true;
	}
	return false;
}

YSL_END_NAMESPACE(Components)

YSL_END

