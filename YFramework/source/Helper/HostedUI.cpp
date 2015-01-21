/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostedUI.cpp
\ingroup Helper
\brief 宿主环境支持的用户界面。
\version r212
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-17 10:22:36 +0800
\par 修改时间:
	2015-01-21 06:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostedUI
*/


#include "Helper/YModules.h"
#include YFM_Helper_HostedUI
#include YFM_Helper_HostRenderer
#include YFM_YSLib_UI_YControl // for UI::FetchEvent;
#include YFM_YSLib_UI_YGUI // for FetchGUIState;

namespace YSLib
{

using namespace Drawing;
using namespace UI;

#if YF_Hosted
namespace Host
{

Window&
WaitForHostWindow(UI::IWidget& wgt)
{
	auto& renderer(dynamic_cast<HostRenderer&>(wgt.GetRenderer()));
	Host::Window* p_wnd{};

	while(!p_wnd)
		p_wnd = renderer.GetWindowPtr();
	return *p_wnd;
}

#	if !YCL_Android

void
DragWindow(Window& wnd, UI::CursorEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct && !e.Handled)
	{
		auto& st(FetchGUIState());

		if(st.CheckDraggingOffset())
		{
			const auto offset(st.CursorLocation + st.DraggingOffset);

			wnd.Move(wnd.GetLocation() + offset);
			st.CursorLocation -= offset - GetLocationOf(e.GetSender());
		}
	}
}
#	endif

#	if YCL_Win32

HostRenderer&
ShowTopLevel(UI::Widget& wgt, unsigned long wstyle, unsigned long wstyle_ex,
	int n_cmd_show, const wchar_t* title, bool mv)
{
	auto& res(UI::WrapRenderer<HostRenderer>(wgt, wgt, [=, &wgt]{
		WindowReference wnd_ref(CreateNativeWindow(WindowClassName,
			GetSizeOf(wgt), title, wstyle, wstyle_ex));

		if(mv)
			wnd_ref.Move(GetLocationOf(wgt));
		wgt.GetView().SetLocation({});
		wnd_ref.Show(n_cmd_show);
		return wnd_ref;
	}));

	WaitForHostWindow(wgt).UseOpacity = bool(wstyle_ex & WS_EX_LAYERED);
	return res;
}
#	endif
#	if !YCL_Android

void
ShowTopLevelDraggable(UI::Widget& wgt)
{
#		if YCL_Win32
	ShowTopLevel(wgt, WS_POPUP);
#		endif
	UI::FetchEvent<UI::TouchHeld>(wgt) += std::bind(Host::DragWindow,
		std::ref(WaitForHostWindow(wgt)), std::placeholders::_1);
}
#	endif

#	if YCL_Win32
void
BindHoverControl(IWidget& sender, Widget& wgt,
	std::function<Point(const Point&)> locator)
{
	yunseq(
	FetchEvent<Enter>(sender) += [&, locator](CursorEventArgs&& e){
		ActOnHover_ShowTopLevelAt(e.GetSender(), wgt,
			std::bind(locator, std::ref(e.Position)));
	},
	FetchEvent<Leave>(sender) += std::bind(OnHover_SetRenderer,
		std::placeholders::_1, std::ref(wgt))
	);
}
#	endif

} // namespace Host;
#endif

} // namespace YSLib;

