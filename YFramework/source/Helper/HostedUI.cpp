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
\version r534
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-17 10:22:36 +0800
\par 修改时间:
	2015-05-27 15:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostedUI
*/


#include "Helper/YModules.h"
#include YFM_Helper_HostedUI
#include YFM_Helper_HostRenderer
#include YFM_YSLib_UI_YControl // for UI::FetchEvent;
#include YFM_YSLib_UI_YGUI // for UI::FetchGUIState;
#include YFM_YSLib_UI_Border
#include YFM_YSLib_UI_YPanel
#include YFM_Helper_Environment
#if YCL_Win32
#	include YFM_YSLib_Core_Task
#	include <ystdex/cast.hpp> // for ystdex::pvoid;
#	include YFM_Helper_GUIApplication // for Host::FetchEnvironment;
#endif

namespace YSLib
{

using namespace Drawing;
using namespace UI;

#if YF_Hosted
namespace Host
{

Window&
WaitForHostWindow(IWidget& wgt)
{
	return dynamic_cast<HostRenderer&>(wgt.GetRenderer()).Wait();
}

#	if !YCL_Android
void
AttachToHost(Widget& wgt, Window& wnd, Messaging::Priority prior)
{
#	if YCL_Win32
	using ystdex::pvoid;

	wnd.MessageMap[WM_DESTROY] += [&]{
		PostTask([&, prior]{
			wgt.SetRenderer({});
		}, prior);
	};
#	else
	yunused(wgt), yunused(wnd), yunused(prior);
#	endif
}

void
DragWindow(Window& wnd, CursorEventArgs&& e, bool root)
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

Point
FetchDefaultTopLevelPosition() ynothrow
{
#	if YCL_Win32
	return {SPos(::GetSystemMetrics(SM_CXSCREEN) / 4),
		SPos(::GetSystemMetrics(SM_CYSCREEN) / 4)};
#	else
	return {};
#	endif
}
#	endif

#	if YCL_Win32

HostRenderer&
ShowTopLevel(Widget& wgt, unsigned long wstyle, unsigned long wstyle_ex,
	int n_cmd_show, const wchar_t* title)
{
	return ShowTopLevel(wgt, WindowThread::DefaultGenerateGuard, wstyle,
		wstyle_ex, n_cmd_show, title);
}
HostRenderer&
ShowTopLevel(Widget& wgt, WindowThread::GuardGenerator guard_gen,
	unsigned long wstyle, unsigned long wstyle_ex, int n_cmd_show,
	const wchar_t* title)
{
	if(!bool(wstyle & WS_POPUP) && bool(wstyle_ex & WS_EX_LAYERED))
	{
		wstyle_ex &= ~static_cast<unsigned long>(WS_EX_LAYERED);
		YTraceDe(Informative, "Layered style ignored for non-popup window.");
	}

	auto& res(UI::WrapRenderer<HostRenderer>(wgt, wgt, guard_gen, [=, &wgt]{
		WindowReference wnd_ref(CreateNativeWindow(WindowClassName,
			GetSizeOf(wgt), title, wstyle, wstyle_ex));

		const auto& pt(GetLocationOf(wgt));

		if(pt != Point::Invalid)
			wnd_ref.MoveClient(pt);
		else if(!
			(wstyle & static_cast<unsigned long>(WS_OVERLAPPED | WS_CHILD)))
			wnd_ref.MoveClient(FetchDefaultTopLevelPosition());
		wnd_ref.Show(n_cmd_show);
		return wnd_ref;
	}));

	res.Wait().UseOpacity = bool(wstyle_ex & WS_EX_LAYERED);
	return res;
}
#	endif
#	if !YCL_Android

void
ShowTopLevelDraggable(Widget& wgt)
{
#		if YCL_Win32
	ShowTopLevel(wgt, WS_POPUP);
#		endif
	FetchEvent<TouchHeld>(wgt) += std::bind(Host::DragWindow,
		std::ref(WaitForHostWindow(wgt)), std::placeholders::_1,
		std::ref(Deref(GetHostRendererPtrOf(wgt)).RootMode));
}
#	endif


array<GEvent<HCursorEvent::FuncType>::iterator, 2>
BindTimedTips(TimedHoverState& st, IWidget& wgt, Widget& target)
{
	auto& cursor_over(FetchEvent<CursorOver>(wgt));
	auto& leave(FetchEvent<Leave>(wgt));

	return {{cursor_over.Insert([&](CursorEventArgs&& e){
			if(st.CheckShow(e))
				ActOnHover_ShowTopLevelAt(e.GetSender(), target,
					std::bind(st.Locate, std::ref(e)));
		}), leave.Insert([&](CursorEventArgs&& e){
			if(st.CheckHide(e))
				target.SetRenderer({});
		})}};
}

void
PrepareTopLevelPopupMenu(Menu& mnu, Panel& root)
{
	root.Add(mnu, DefaultMenuZOrder);
#	if YCL_Win32
	ShowTopLevel(mnu, WindowThread::GuardGenerator(), WS_POPUP,
		WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOPMOST, SW_HIDE);
#	endif
}

void
SetupTopLevelTimedTips(IWidget& wgt, TimedHoverState& st, Label& lbl,
	const String& text, const Rect& r, const Font& fnt, const Padding& m)
{
	using namespace UI;

	SetupContentsOf(lbl, text, r, fnt, m);
	// TODO: Border style setting, font, background, allowing host shadow,
	//	etc.
	FetchEvent<Paint>(lbl) += BorderBrush();
#	if YCL_Win32
	BindTimedTips(st, wgt, lbl);
#	else
	yunused(wgt), yunused(st);
#	endif
}

void
SetupTopLevelContextMenu(IWidget& wgt, MenuHost& mh, Menu& mnu)
{
	mh += mnu;
#	if YCL_Win32
	PrepareTopLevelPopupMenu(mnu, FetchEnvironment().Desktop);
	BindTopLevelPopupMenu(mh, mnu, wgt);
#	else
	yunused(wgt), yunused(mh), yunused(mnu);
#	endif
}

} // namespace Host;
#endif

} // namespace YSLib;

