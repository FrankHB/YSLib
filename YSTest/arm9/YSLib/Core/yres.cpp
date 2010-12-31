/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yres.cpp
\ingroup Core
\brief 资源管理模块。
\version 0.1219;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 17:28:28 + 08:00;
\par 修改时间:
	2010-12-31 12:17 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YResource;
*/


#include "yres.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

YImage::YImage(ConstBitmapPtr s, SDST w, SDST h)
	: BitmapBuffer(s, w, h)
{}

void
YImage::SetImage(ConstBitmapPtr s, SDST w, SDST h)
{
	SetSize(w, h);
	if(pBuffer && s)
		mmbcpy(pBuffer, s, GetSizeOfBuffer());
}

YSL_END_NAMESPACE(Drawing)

YSL_END

