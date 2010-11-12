/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydevice.cpp
\ingroup Core
\brief 平台无关的设备抽象层。
\version 0.2676;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-28 16:39:51 + 08:00;
\par 修改时间:
	2010-11-12 15:35 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YDevice;
*/


#include "ydevice.h"
#include "yshell.h"
#include "../Shell/ygdi.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Device)

bool YScreen::S_InitScr(true);

YScreen::YScreen(SDST w, SDST h, BitmapPtr p)
	: YGraphicDevice(w, h, p),
bg(-1)
{}

void
YScreen::CheckInitialization()
{
	if(S_InitScr)
		InitScreen();
}

void
YScreen::Reset()
{
	InitScreen();
}

void
YScreen::Update(BitmapPtr buf)
{
	platform::ScreenSychronize(GetPtr(), buf);
}
void
YScreen::Update(Color c)
{
	FillSeq<PixelType>(GetPtr(), GetAreaFrom(*this), c);
}

YSL_END_NAMESPACE(Device)

YSL_END

