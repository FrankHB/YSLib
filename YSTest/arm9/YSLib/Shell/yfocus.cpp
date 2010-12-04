/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfocus.cpp
\ingroup Shell
\brief GUI 焦点特性实现。
\version 0.1301;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-01 13:52:56 + 08:00;
\par 修改时间:
	2010-11-26 23:15 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YFocus;
*/


#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

void
MSimpleFocusResponser::ClearFocusingPtr()
{
	pFocusing = NULL;
}

bool
MSimpleFocusResponser::ResponseFocusRequest(AFocusRequester& w)
{
	pFocusing = dynamic_cast<IVisualControl*>(&w);
	return pFocusing;
}

bool
MSimpleFocusResponser::ResponseFocusRelease(AFocusRequester& w)
{
	if(pFocusing == dynamic_cast<IVisualControl*>(&w))
	{
		pFocusing = NULL;
	//	w.ReleaseFocusRaw();
		return true;
	}
	return false;
}


bool
AFocusRequester::CheckRemoval(GMFocusResponser<AFocusRequester>& c) const
{
	if(IsFocusOfContainer(c))
	{
		c.ClearFocusingPtr();
		return true;
	}
	return false;
}

bool
AFocusRequester::RequestFocus(GMFocusResponser<AFocusRequester>& c)
{
	return !(bFocused && IsFocusOfContainer(c))
		&& (bFocused = c.SetFocusingPtr(this));
}

bool
AFocusRequester::ReleaseFocus(GMFocusResponser<AFocusRequester>& c)
{
	return bFocused && IsFocusOfContainer(c)
		&& (bFocused = NULL, !(c.ClearFocusingPtr()));
}

YSL_END_NAMESPACE(Components)

YSL_END

