/*
	Copyright by FrankHB 2011 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file uicontx.cpp
\ingroup UI
\brief 样式无关的图形用户界面附加容器。
\version r217
\author FrankHB <frankhb1989@gmail.com>
\since build 192
\par 创建时间:
	2011-02-21 09:01:13 +0800
\par 修改时间:
	2013-03-23 10:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::UIContainerEx
*/


#include "YSLib/UI/uicontx.h"
#include "YSLib/UI/ygui.h"
#include "YSLib/UI/YBrush.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(UI)

DialogBox::DialogBox(const Rect& r)
	: Control(r),
	btnClose(Rect(GetWidth() - 20, 4, 16, 16), 330)
{
	DecorateAsCloseButton(btnClose),
	SetContainerPtrOf(btnClose, this),
	FetchEvent<Paint>(*this).Add(BorderBrush(), BackgroundPriority);
}


DialogPanel::DialogPanel(const Rect& r)
	: Panel(r),
	btnClose(Rect(GetWidth() - 20, 4, 16, 16), 330),
	btnOK(Rect(GetWidth() - 40, 4, 16, 16), 120)
{
	AddWidgets(*this, btnClose, btnOK),
	DecorateAsCloseButton(btnClose),
	yunseq(
		btnOK.Text = u"○",
		FetchEvent<Paint>(*this).Add(BorderBrush(), BackgroundPriority),
		FetchEvent<Click>(btnOK) += [this](TouchEventArgs&&){
			Close(*this);
		}
	);
}

YSL_END_NAMESPACE(UI)

YSL_END

