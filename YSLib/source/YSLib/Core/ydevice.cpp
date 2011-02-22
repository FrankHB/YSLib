/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydevice.cpp
\ingroup Core
\brief 平台无关的设备抽象层。
\version 0.2710;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-28 16:39:51 + 08:00;
\par 修改时间:
	2011-02-20 15:31 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YDevice;
*/


#include "ydevice.h"
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

BitmapPtr
YScreen::GetCheckedBufferPtr() const ythrow()
{
	CheckInitialization();

	YAssert(GetBufferPtr(), "Screen buffer initializition checking failed.");

	return GetBufferPtr();
}

void
YScreen::Update(BitmapPtr buf)
{
	platform::ScreenSynchronize(GetCheckedBufferPtr(), buf);
}
void
YScreen::Update(Color c)
{
	FillPixel<PixelType>(GetCheckedBufferPtr(), GetAreaFrom(GetSize()), c);
}

YSL_END_NAMESPACE(Device)

YSL_END

