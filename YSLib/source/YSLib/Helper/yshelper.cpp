/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yshelper.cpp
\ingroup Helper
\brief Shell 助手模块。
\version 0.1253;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-04-04 13:42:15 +0800;
\par 修改时间:
	2011-04-22 21:57 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::YShellHelper;
*/


#include "yshelper.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

GHandle<YImage>
NewScrImage(PPDRAW f, BitmapPtr gbuf)
{
	bool s(!gbuf);

	if(s)
		gbuf = ynew ScreenBufferType;
	ScrDraw(gbuf, f);

	GHandle<YImage> pi(NewScrImage(gbuf));

	if(s)
		safe_delete_obj()(gbuf);
	return pi;
}

YSL_END_NAMESPACE(Drawing)

YSL_END;

