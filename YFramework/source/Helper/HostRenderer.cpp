/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostRenderer.cpp
\ingroup Helper
\brief 宿主渲染器。
\version r468
\author FrankHB <frankhb1989@gmail.com>
\since build 426
\par 创建时间:
	2013-07-09 05:37:27 +0800
\par 修改时间:
	2015-01-26 05:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostRenderer
*/


#include "Helper/YModules.h"
#include YFM_Helper_HostRenderer
#include YFM_Helper_Environment // for Environment;
//#include YFM_Helper_GUIApplication
#if YCL_HostedUI_XCB
#	include <xcb/xcb.h>
#	include YFM_YCLib_XCB
#elif YCL_Win32
#	include YFM_Helper_Environment
#	include YFM_MinGW32_Helper_Win32Control
#endif

namespace YSLib
{

using namespace Drawing;

#if YF_Hosted
namespace Host
{

RenderWindow::RenderWindow(HostRenderer& r, NativeWindowHandle h)
	: Window(h), renderer(r)
{
#	if YCL_HostedUI_XCB
	MessageMap[XCB_EXPOSE] += [this]{
		// TODO: Optimize using event parameter.
		renderer.get().GetBufferRef().UpdateTo(GetNativeHandle());
	};
#	elif YCL_Win32
	MessageMap[WM_PAINT] += [this]{
		// NOTE: Parameters are not used.
		GSurface<WindowRegionDeviceContext> sf(GetNativeHandle());

		renderer.get().UpdateToSurface(sf);
	};
#	endif
}

void
RenderWindow::Refresh()
{
	auto& rd(GetRenderer());
	auto& wgt(rd.GetWidgetRef());

	if(rd.Validate(wgt, wgt,
		{rd.GetContext(), Point(), rd.GetInvalidatedArea()}))
		rd.Update(rd.GetContext().GetBufferPtr());
}


WindowThread::~WindowThread()
{
	const auto p_wnd_val(Nonnull(GetWindowPtr()));

	try
	{
#	if !YCL_Android
		TryExpr(p_wnd_val->Close())
		// TODO: Log.
#		if YCL_HostedUI_XCB
		CatchIgnore(XCB::XCBException&)
#		endif
#		if YCL_Win32
		CatchIgnore(Win32Exception&)
#		else
		CatchIgnore(Exception&) // XXX: Use proper platform-dependent type.
#		endif
#	endif
		thrd.join();
	}
	CatchExpr(std::system_error& e, YTraceDe(Warning,
		"Caught std::system_error: %s.", e.what()), yunused(e))
	CatchExpr(std::exception& e,
		YTraceDe(Alert, "Caught std::exception[%s]: %s.", typeid(e).name(),
		e.what()), yunused(e))
	CatchExpr(..., YTraceDe(Alert,
		"Unknown exception found @ WindowThread::~WindowThread."))
	delete p_wnd_val;
}

void
WindowThread::ThreadLoop(NativeWindowHandle h_wnd)
{
	ThreadLoop(make_unique<Window>(h_wnd));
}
void
WindowThread::ThreadLoop(unique_ptr<Window> p)
{
	YAssert(!p_wnd, "Duplicate window initialization detected.");
	p_wnd = p.release();
	WindowLoop(Deref(p_wnd));
}

void
WindowThread::WindowLoop(Window& wnd)
{
#	if YF_Multithread
	auto& env(wnd.GetEnvironmentRef());

	env.EnterWindowThread();
#	endif
	YTraceDe(Notice, "Host loop began.");
#	if YCL_HostedUI_XCB
	// XXX: Exit on I/O error occurred?
	// TODO: Log I/O error.
	while(const auto p_evt = unique_raw(::xcb_wait_for_event(
		&Deref(wnd.GetNativeHandle().get()).DerefConn()), std::free))
	{
	//	YSL_DEBUG_DECL_TIMER(tmr, std::to_string(msg));
		auto& m(wnd.MessageMap);
		const auto msg(p_evt->response_type & ~0x80);

		if(msg == XCB_CLIENT_MESSAGE)
		{
			auto& cevt(reinterpret_cast<::xcb_client_message_event_t&>(*p_evt));

			if(cevt.type == wnd.WM_PROTOCOLS
				&& cevt.data.data32[0] == wnd.WM_DELETE_WINDOW)
				break;
		}

		const auto i(m.find(msg & ~0x80));

		if(i != m.cend())
			i->second(p_evt.get());
	}
#	elif YCL_Win32
	while(true)
	{
		::MSG msg{nullptr, 0, 0, 0, 0, {0, 0}};

		if(::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			if(msg.message == WM_QUIT)
				break;
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}
		else
			// NOTE: Failure ignored.
			::WaitMessage();
	}
#	endif
	YTraceDe(Notice, "Host loop ended.");
#	if YF_Multithread
	env.LeaveWindowThread();
#	endif
}


HostRenderer::~HostRenderer()
{
#if YCL_Win32
	try
	{
		auto& wnd(Wait());

		wnd.GetEnvironmentRef().Desktop -= widget;
		if(const auto p_wgt = dynamic_cast<UI::Widget*>(&widget.get()))
			p_wgt->SetView({});
	}
	CatchIgnore(Windows::UI::ViewSignal&)
	CatchExpr(std::system_error& e, YTraceDe(Warning,
		"Caught std::system_error: %s.", e.what()), yunused(e))
	CatchExpr(std::exception& e,
		YTraceDe(Alert, "Caught std::exception[%s]: %s.", typeid(e).name(),
		e.what()), yunused(e))
	CatchExpr(..., YTraceDe(Alert,
		"Unknown exception found @ HostRenderer::~HostRenderer."))
#endif
}
void
HostRenderer::SetSize(const Size& s)
{
	BufferedRenderer::SetSize(s),
	rbuf.Resize(s);
}

void
HostRenderer::InitWidgetView()
{
#if YCL_Win32
	auto& wnd(Wait());

	if(const auto p_wgt = dynamic_cast<UI::Widget*>(&widget.get()))
		p_wgt->SetView(
			make_unique<Windows::UI::ControlView>(wnd.GetNativeHandle()));
	wnd.GetEnvironmentRef().Desktop += widget;
#endif
	// FIXME: Allow user to specify which kinds of views should be adjusted.
	RootMode = typeid(widget.get().GetView()) == typeid(UI::View);
}

void
HostRenderer::Update(ConstBitmapPtr p_buf)
{
	YAssert(GetSizeOf(widget) == rbuf.GetSize(), "Mismatched size found.");

	if(const auto p_wnd = GetWindowPtr())
		try
		{
			auto& view(widget.get().GetView());

			if(RootMode)
			{
#	if YCL_HostedUI_XCB
				const Rect& cbounds(p_wnd->GetBounds());
#	elif YCL_Android
				const Rect cbounds(p_wnd->GetSize());
#	else
				const Rect& cbounds(p_wnd->GetClientBounds());
#	endif
				auto bounds(cbounds);
				const auto& loc(view.GetLocation());

				if(!loc.IsZero())
				{
					bounds.GetPointRef() += loc;
					view.SetLocation({});
					rInvalidated = {{}, bounds.GetSize()};
					Validate(widget, widget, {GetContext(), {}, rInvalidated});
				}
				bounds.GetSizeRef() = view.GetSize();
#	if !YCL_Android
				if(bounds != cbounds)
#		if YCL_HostedUI_XCB
					p_wnd->SetBounds(bounds);
#		else
					p_wnd->SetClientBounds(bounds);
#		endif
#	endif
			}
			p_wnd->UpdateFrom(p_buf, rbuf);
		}
#	if YCL_Win32
		CatchIgnore(Win32Exception&)
#	else
		CatchIgnore(Exception&) // XXX: Use proper platform-dependent type.
#	endif
}

Window&
HostRenderer::Wait()
{
	Host::Window* p_wnd{};

	// XXX: Busy wait.
	while(!p_wnd)
		p_wnd = GetWindowPtr();
	return *p_wnd;
}

} // namespace Host;
#endif

} // namespace YSLib;

