/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydesktop.cpp
\ingroup UI
\brief 平台无关的桌面抽象层。
\version r1432
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-05-02 12:00:08 +0800
\par 修改时间:
	2013-05-21 21:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Desktop
*/


#include "YSLib/UI/ydesktop.h"
#include "YSLib/UI/YBrush.h"
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(UI)

namespace
{

//! \since build 377
BufferedRenderer&
GetBufferedRendererOf(const IWidget& wgt) ynothrow
{
	return ystdex::polymorphic_downcast<BufferedRenderer&>(wgt.GetRenderer());
}

} // unnamed namespace;


Desktop::Desktop(Devices::Screen& s, Color c, const shared_ptr<Image>& hImg,
	unique_ptr<BufferedRenderer> p)
	: Window(Rect(s.GetSize()), p ? std::move(p) : std::move(
	make_unique<BufferedRenderer>(true, std::move(s.GetBackBuffer())))),
	screen(s)
{
	Background = hImg ? HBrush(ImageBrush(hImg)) : HBrush(SolidBrush(c)),
	GetBufferedRendererOf(*this).IgnoreBackground = true;
}

void
Desktop::Update()
{
//	if(!GetBufferedRendererOf(*this).RequiresRefresh())
	screen.Update(GetBufferedRendererOf(*this).GetContext().GetBufferPtr());
}

Rect
Desktop::Validate()
{
	auto& rd(GetBufferedRendererOf(*this));

	return rd.Validate(*this, *this,
		{rd.GetContext(), Point(), GetBoundsOf(*this)});
}

YSL_END_NAMESPACE(UI)

YSL_END

