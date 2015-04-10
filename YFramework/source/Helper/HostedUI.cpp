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
\version r451
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-17 10:22:36 +0800
\par 修改时间:
	2015-04-09 11:18 +0800
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
			YTraceDe(Debug, "Ready to reset renderer of widget '%p' from host"
				" window '%p' from the host.", pvoid(&wgt),
				pvoid(GetWindowPtrOf(wgt)));
			wgt.SetRenderer({});
		}, prior);
	};
#	else
	yunused(wgt), yunused(wnd);
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
	auto& res(UI::WrapRenderer<HostRenderer>(wgt, wgt, guard_gen, [=, &wgt]{
		WindowReference wnd_ref(CreateNativeWindow(WindowClassName,
			GetSizeOf(wgt), title, wstyle, wstyle_ex));

		const auto& pt(GetLocationOf(wgt));

		if(pt != Point::Invalid)
			wnd_ref.MoveClient(pt);
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
PrepareTopLevelPopupMenu(MenuHost& mh, Menu& mnu, Panel& root)
{
	mh += mnu;
	root.Add(mnu, DefaultMenuZOrder);
#	if YCL_Win32
	ShowTopLevel(mnu, WS_POPUP,
		WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOPMOST, SW_HIDE);
#	endif
}

//! \since build 583
namespace
{

//! \since build 590
template<typename _func>
bool
DoSetupTopLevel(Widget& top, _func f, Window* p_wnd = {})
{
#	if YCL_Win32
	auto&
		root((p_wnd ? p_wnd->GetEnvironmentRef() : FetchEnvironment()).Desktop);

	if(!p_wnd || FetchContainerPtr(top) == &root)
	{
		auto& wgt(f(root));

		if(p_wnd)
			AttachToHost(wgt, *p_wnd);
		return true;
	}
#	else
	yunused(top, p_wnd);
	yunused(f);
#	endif
	return {};
}

} // unnamed namespace;

bool
SetupTopLevelTimedTips(Widget& top, IWidget& wgt, TimedHoverState& st,
	Label& lbl, const String& text, const Rect& r, const Font& fnt,
	const Padding& m)
{
	return DoSetupTopLevel(top, [&](Panel&)->Widget&{
		using namespace UI;

		SetupContentsOf(lbl, text, r, fnt, m);
		// TODO: Border style setting, font, background, allowing host shadow,
		//	etc.
		FetchEvent<Paint>(lbl) += BorderBrush();
#	if YCL_Win32
		BindTimedTips(st, wgt, lbl);
#	else
		yunused(st, wgt, lbl);
#	endif
		return lbl;
	}, GetWindowPtrOf(top));
}

bool
SetupTopLevelContextMenu(Widget& top, IWidget& wgt, MenuHost& mh, Menu& mnu)
{
	return DoSetupTopLevel(top, [&](Panel& root)->Widget&{
#	if YCL_Win32
		PrepareTopLevelPopupMenu(mh, mnu, root);
		BindTopLevelPopupMenu(mh, mnu, wgt);
#	else
		yunused(mh, mnu, root, wgt);
#	endif
		return mnu;
	}, GetWindowPtrOf(top));
}

} // namespace Host;
#endif

} // namespace YSLib;

