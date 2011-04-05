/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yres.cpp
\ingroup Core
\brief 应用程序资源管理模块。
\version 0.1228;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 17:28:28 +0800;
\par 修改时间:
	2011-03-31 20:04 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YResource;
*/


#include "yres.h"
#include "yshell.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

YImage::YImage(ConstBitmapPtr s, SDst w, SDst h)
	: BitmapBuffer(s, w, h)
{}

void
YImage::SetImage(ConstBitmapPtr s, SDst w, SDst h)
{
	SetSize(w, h);
	if(pBuffer && s)
		mmbcpy(pBuffer, s, GetSizeOfBuffer());
}

YSL_END_NAMESPACE(Drawing)

YSL_END

