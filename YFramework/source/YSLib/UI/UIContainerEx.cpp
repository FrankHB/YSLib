/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file UIContainerEx.cpp
\ingroup UI
\brief 样式无关的 GUI 附加容器。
\version r279
\author FrankHB <frankhb1989@gmail.com>
\since build 192
\par 创建时间:
	2011-02-21 09:01:13 +0800
\par 修改时间:
	2016-02-12 00:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::UIContainerEx
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_UIContainerEx
#include YFM_YSLib_UI_YGUI
#include YFM_YSLib_UI_Border

namespace YSLib
{

namespace UI
{

DialogBox::DialogBox(const Rect& r)
	: Control(r, MakeBlankBrush()),
	// XXX: Conversion to 'SPos' might be implementation-defined.
	btnClose(Rect(SPos(GetWidth()) - 20, 4, 16, 16), 330)
{
	const auto invalidator([this]{
		Invalidate(*this);
	});

	DecorateAsCloseButton(btnClose),
	SetContainerPtrOf(btnClose, make_observer(this)),
	yunseq(
	FetchEvent<Paint>(*this).Add(BorderBrush(), BackgroundPriority),
	FetchEvent<GotFocus>(*this) += invalidator,
	FetchEvent<LostFocus>(*this) += invalidator
	);
}
ImplDeDtor(DialogBox)


DialogPanel::DialogPanel(const Rect& r)
	: Panel(r),
	// XXX: Conversion to 'SPos' might be implementation-defined.
	btnClose(Rect(SPos(GetWidth()) - 20, 4, 16, 16), 330),
	btnOK(Rect(SPos(GetWidth()) - 40, 4, 16, 16), 120)
{
	const auto invalidator([this]{
		Invalidate(*this);
	});

	AddWidgets(*this, btnClose, btnOK),
	DecorateAsCloseButton(btnClose),
	yunseq(
	FetchEvent<Paint>(*this).Add(BorderBrush(), BackgroundPriority),
	FetchEvent<Click>(btnOK) += [this]{
		Close(*this);
	},
	FetchEvent<Paint>(btnOK) += [this](PaintEventArgs&& e){
		DrawCircle(e.Target, e.ClipArea, {e.Location.X + 8, e.Location.Y + 8},
			4, IsEnabled(btnOK) ? btnOK.ForeColor
			: FetchGUIConfiguration().Colors[Styles::Workspace]);
	},
	FetchEvent<GotFocus>(*this) += invalidator,
	FetchEvent<LostFocus>(*this) += invalidator
	);
}
ImplDeDtor(DialogPanel)

} // namespace UI;

} // namespace YSLib;

