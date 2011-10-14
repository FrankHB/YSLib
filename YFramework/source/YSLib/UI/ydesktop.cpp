/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydesktop.cpp
\ingroup UI
\brief 平台无关的桌面抽象层。
\version r2356;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-02 12:00:08 +0800;
\par 修改时间:
	2011-09-08 02:12 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Desktop;
*/


#include "ydesktop.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

Desktop::Desktop(Devices::Screen& s, Color c, const shared_ptr<Image>& hImg)
	: Frame(Rect::FullScreen, hImg),
	screen(s)
{
	BackColor = c;
}

/*void
Desktop::RemoveTopDesktopObject()
{
	if(!sDOs.empty())
	{
		sDOs.back()->ReleaseFocus();
		sDOs.pop_back();
		bRefresh = true;
	}
}*/

void
Desktop::ClearContents()
{
	ClearFocusingPtrOf(*this);
	sWidgets.clear();
	SetInvalidationOf(*this);
}

void
Desktop::Update()
{
	if(!GetRenderer().RequiresRefresh())
		screen.Update(FetchContext(*this).GetBufferPtr());
}

YSL_END_NAMESPACE(Components)

YSL_END

