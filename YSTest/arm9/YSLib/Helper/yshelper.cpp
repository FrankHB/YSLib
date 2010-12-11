/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yshelper.cpp
\ingroup Helper
\brief Shell 助手模块。
\version 0.1186;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-04-04 13:42:15 + 08:00;
\par 修改时间:
	2010-12-08 20:58 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::YShellHelper;
*/


#include "yshelper.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

YImage*
NewScrImage(PPDRAW f, BitmapPtr gbuf)
{
	bool s(!gbuf);

	if(s)
		gbuf = new ScreenBufferType;
	ScrDraw(gbuf, f);

	YImage* pi(NewScrImage(gbuf));

	if(s)
	{
		ydelete_array(gbuf);
		gbuf = NULL;
	}
	return pi;
}

YSL_END_NAMESPACE(Drawing)

YSL_END;

