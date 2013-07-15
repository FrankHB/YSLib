/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostedUI.cpp
\ingroup Helper
\brief 宿主环境支持的用户界面。
\version r105
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-17 10:22:36 +0800
\par 修改时间:
	2013-07-14 20:17 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::HostedUI
*/


#include "Helper/HostedUI.h"
#include "HostRenderer.h"
#include <YSLib/UI/ycontrol.h> // for UI::FetchEvent;

YSL_BEGIN

using namespace Drawing;
using namespace UI;

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

BufferedRenderer*
GetHostRendererPtrOf(UI::IWidget& wgt)
{
	return dynamic_cast<HostRenderer*>(&wgt.GetRenderer());
}

Window*
GetWindowPtrOf(UI::IWidget& wgt)
{
	if(const auto p_r = dynamic_cast<HostRenderer*>(&wgt.GetRenderer()))
		return p_r->GetWindowPtr();
	return nullptr;
}

Window&
WaitForHostWindow(UI::IWidget& wgt)
{
	auto& renderer(dynamic_cast<HostRenderer&>(wgt.GetRenderer()));
	Host::Window* p_wnd{};

	while(!p_wnd)
		p_wnd = renderer.GetWindowPtr();
	return *p_wnd;
}


unique_ptr<BufferedRenderer>
MakeHostRenderer(IWidget& wgt, std::function<NativeWindowHandle()> f)
{
	return unique_ptr<BufferedRenderer>(new HostRenderer(wgt, std::move(f)));
}


void
DragWindow(Window& wnd, UI::CursorEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& st(FetchGUIState());
		const auto offset(st.LastControlLocation + st.DraggingOffset);

		wnd.Move(wnd.GetLocation() + offset);
		st.ControlLocation -= offset - GetLocationOf(e.GetSender());
	}
}

#	if YCL_MINGW32

void
ShowTopLevel(UI::Widget& wgt, ::DWORD wstyle, const wchar_t* title)
{
	WrapRenderer(wgt, CreateNativeWindow, WindowClassName, GetSizeOf(wgt),
		title, wstyle);
}
#	endif

void
ShowTopLevelDraggable(UI::Widget& wgt)
{
#	if YCL_MINGW32
	ShowTopLevel(wgt, WS_POPUP);
#else
#	error "Currently only Windows is supported."
#endif
	UI::FetchEvent<UI::TouchHeld>(wgt) += std::bind(Host::DragWindow,
		std::ref(Host::WaitForHostWindow(wgt)), std::placeholders::_1);
}

YSL_END_NAMESPACE(Host)
#endif

YSL_END

