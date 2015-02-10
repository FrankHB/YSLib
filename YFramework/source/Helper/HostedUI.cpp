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
\version r310
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-17 10:22:36 +0800
\par 修改时间:
	2015-02-09 22:54 +0800
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
#include YFM_YSLib_UI_Border
#include YFM_YSLib_UI_YPanel
#include YFM_Helper_Environment

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
	return dynamic_cast<HostRenderer&>(wgt.GetRenderer()).Wait();
}

#	if !YCL_Android

void
DragWindow(Window& wnd, UI::CursorEventArgs&& e, bool root)
{
	if(e.Strategy == RoutedEventArgs::Direct && !e.Handled)
	{
		auto& st(FetchGUIState());

		if(st.CheckDraggingOffset())
		{
			const auto offset(st.CursorLocation + st.DraggingOffset);

			if(root)
			{
#		if YCL_Win32
				wnd.MoveClient(wnd.GetClientLocation() + offset);
#		else
				wnd.Move(wnd.GetLocation() + offset);
#		endif
				st.CursorLocation -= offset - GetLocationOf(e.GetSender());
			}
			else
#		if YCL_Win32
				wnd.MoveClient(offset);
#		else
				wnd.Move(offset);
#		endif
		}
	}
}
#	endif

#	if YCL_Win32

HostRenderer&
ShowTopLevel(UI::Widget& wgt, unsigned long wstyle, unsigned long wstyle_ex,
	int n_cmd_show, const wchar_t* title)
{
	auto& res(UI::WrapRenderer<HostRenderer>(wgt, wgt, [=, &wgt]{
		WindowReference wnd_ref(CreateNativeWindow(WindowClassName,
			GetSizeOf(wgt), title, wstyle, wstyle_ex));

		const auto& pt(GetLocationOf(wgt));

		if(pt != Point::Invalid)
			wnd_ref.MoveClient(pt);
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
		std::ref(WaitForHostWindow(wgt)), std::placeholders::_1,
		std::ref(Deref(GetHostRendererPtrOf(wgt)).RootMode));
}
#	endif

array<GEvent<UI::HCursorEvent::FuncType>::iterator, 2>
BindTimedTips(TimedHoverState& st, IWidget& sender, Widget& wgt)
{
	auto& cursor_over(FetchEvent<CursorOver>(sender));
	auto& leave(FetchEvent<Leave>(sender));

	return {{cursor_over.Insert([&](CursorEventArgs&& e){
			if(st.CheckShow(e))
				ActOnHover_ShowTopLevelAt(e.GetSender(), wgt,
					std::bind(st.Locate, std::ref(e)));
		}), leave.Insert([&](CursorEventArgs&& e){
			if(st.CheckHide(e))
				OnHover_SetRenderer(std::move(e), wgt);
		})}};
}

void
SetupTimedTips(UI::TimedHoverState& st, UI::IWidget& wgt, UI::Label& lbl,
	const String& text, const Drawing::Rect& r, const Drawing::Font& fnt,
	const Drawing::Padding& m)
{
	using namespace UI;

	// NOTE: For tool tips, Control + MLabel can be used but is likely not good
	//	enough, because the top level control boundary would likely behave
	//	unexpectedly when the boundary is overlapped with underlying controls.
	SetupContentsOf(lbl, text, r, fnt, m);
	// TODO: Border style setting, font, background, allowing host shadow, etc.
	FetchEvent<Paint>(lbl) += BorderBrush();
	BindTimedTips(st, wgt, lbl);
}

void
PrepareTopLevelPopupMenu(MenuHost& mh, Menu& mnu, Panel& root, Window* p_wnd)
{
	mh += mnu;
	root.Add(mnu, DefaultMenuZOrder);
#	if YCL_Win32
	ShowTopLevel(mnu, WS_POPUP,
		WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOPMOST, SW_HIDE);
	if(p_wnd)
		p_wnd->MessageMap[WM_DESTROY] += [&]{
			mnu.SetRenderer({});
		};
#	else
	yunused(p_wnd);
#	endif
}

bool
SetupTopLevelContextMenu(MenuHost& mh, Menu& mnu, Widget& top, IWidget& wgt,
	bool reset_renderer)
{
	if(const auto p_wnd = GetWindowPtrOf(top))
	{
#	if YCL_Win32
		auto& root(p_wnd->GetEnvironmentRef().Desktop);

		if(FetchContainerPtr(top) == &root)
		{
			PrepareTopLevelPopupMenu(mh, mnu, root,
				reset_renderer ? p_wnd : nullptr);
			UI::BindTopLevelPopupMenu(mh, mnu, wgt);
			return true;
		}
#	else
		yunused(mh), yunused(mnu), yunused(wgt), yunused(reset_renderer);
#	endif
	}
	return {};
}


} // namespace Host;
#endif

} // namespace YSLib;

