/*
	© 2013-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostRenderer.cpp
\ingroup Helper
\brief 宿主渲染器。
\version r714
\author FrankHB <frankhb1989@gmail.com>
\since build 426
\par 创建时间:
	2013-07-09 05:37:27 +0800
\par 修改时间:
	2018-12-14 08:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostRenderer
*/


#include "Helper/YModules.h"
#include YFM_Helper_HostRenderer // for ystdex::pvoid;
#include YFM_Helper_Environment
#include YFM_Helper_GUIApplication // for GUIHost;
#if YCL_HostedUI_XCB
#	include <xcb/xcb.h>
#	include YFM_YCLib_XCB
#elif YCL_Win32
#	include YFM_Win32_Helper_Win32Control
#endif
#include <ystdex/scope_guard.hpp> // for ystdex::unique_guard;

namespace YSLib
{

using namespace Drawing;

#if YF_Hosted
namespace Host
{

RenderWindow::RenderWindow(HostRenderer& rd, NativeWindowHandle h)
	: Window(h), renderer(rd)
{
#	if YCL_HostedUI_XCB
	MessageMap[XCB_EXPOSE] += [this]{
		// TODO: Optimize using event parameter.
		YSLib::Deref(renderer.get().GetBufferRef().Lock()).UpdateTo(
			Nonnull(GetNativeHandle()));
	};
#	elif YCL_Win32
	yunseq(
	MessageMap[WM_ERASEBKGND]
		+= [](::WPARAM, ::LPARAM, ::LRESULT& res) ynothrow{
		res = 1;
	},
	MessageMap[WM_PAINT] += [this]{
		// NOTE: Parameters are not used.
		GSurface<WindowRegionDeviceContext> sf(Nonnull(GetNativeHandle()));
		const Rect& bounds(sf.GetInvalidatedArea());

		renderer.get().UpdateBoundsToSurface(sf, bounds, bounds.GetPoint());
	}
	);
#	endif
}

void
RenderWindow::Refresh()
{
	GetRenderer().RefreshForWidget();
}


WindowThread::~WindowThread()
{
	const unique_ptr<Window> p_hold(Nonnull(GetWindowPtr().get()));

	YTraceDe(Debug, "Ready to close window '%p' on leaving window"
		" thread.", ystdex::pvoid(p_hold.get()));

	FilterExceptions([&, this]{
#	if !YCL_Android
		TryExpr(p_hold->Close())
		// TODO: Log.
#		if YCL_HostedUI_XCB
		CatchIgnore(XCB::XCBException&)
#		endif
#		if YCL_Win32
		CatchIgnore(Win32Exception&)
#		else
		CatchIgnore(Exception&) // TODO: Use proper platform-dependent type.
#		endif
#	endif
		YTraceDe(Informative, "Ready to join the window thread '%p' of closed"
			" window '%p'.", ystdex::pvoid(&thrd), ystdex::pvoid(p_hold.get()));
		thrd.join();
		YTraceDe(Debug, "Window thread '%p' joined.",
			ystdex::pvoid(&thrd));
	}, yfsig);
}

WindowThread::Guard
WindowThread::DefaultGenerateGuard(Window& wnd)
{
#if YF_Multithread == 1
	wnd.GetGUIHostRef().EnterWindowThread();
	return ystdex::unique_guard([&]() ynothrow{
		FilterExceptions([&]{
			wnd.GetGUIHostRef().LeaveWindowThread();
		}, "default event guard destructor");
	});
#else
	return {};
#endif
}

void
WindowThread::ThreadLoop(NativeWindowHandle h_wnd)
{
	ThreadLoop(make_unique<Window>(h_wnd));
}
void
WindowThread::ThreadLoop(unique_ptr<Window> p_wnd)
{
	YAssert(!p_window, "Duplicate window initialization detected.");

	auto& wnd(Deref(p_wnd));

	p_window = p_wnd.release();
	WindowLoop(wnd, GenerateGuard);
}

void
WindowThread::WindowLoop(Window& wnd)
{
	YTraceDe(Informative, "Host loop began.");
#	if YCL_HostedUI_XCB
	// XXX: Exit on I/O error occurred?
	// TODO: Log I/O error.
	// TODO: Extract the loop to module %YCLib::XCB?
	while(const auto p_evt = unique_raw(::xcb_wait_for_event(
		&Deref(wnd.GetNativeHandle()).DerefConn()), std::free))
	{
	//	YSL_DEBUG_DECL_TIMER(tmr, to_string(msg));
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
#	else
	yunused(wnd);
#		if YCL_Win32

	::MSG msg{nullptr, 0, 0, 0, 0, {0, 0}};
	int res;

	while((res = ::GetMessageW(&msg, nullptr, 0, 0)) != 0)
	{

		// XXX: Error ignored.
		if(res != -1)
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}
		else
			YCL_Trace_Win32E(Warning, GetMessageW, yfsig);
	}
#		endif
#	endif
	YTraceDe(Informative, "Host loop ended.");
}
void
WindowThread::WindowLoop(Window& wnd, GuardGenerator gd_gen)
{
	const auto gd(gd_gen ? gd_gen(wnd) : Guard());

	WindowLoop(wnd);
}


HostRenderer::~HostRenderer()
{
#if YCL_Win32
	FilterExceptions([this]{
		try
		{
			auto& wnd(Wait());

			wnd.GetGUIHostRef().Desktop -= widget;
			if(const auto p_wgt = dynamic_cast<UI::Widget*>(&widget.get()))
				p_wgt->SetView({});
		}
		CatchIgnore(Windows::UI::ViewSignal&)
	}, yfsig);
#endif
}

void
HostRenderer::SetSize(const Size& s)
{
	BufferedRenderer::SetSize(s),
	Deref(rbuf.Lock()).Resize(s);
}

bool
HostRenderer::AdjustSize()
{
	const auto& view_size(GetSizeOf(widget));
	const auto& buf_size(GetContext().GetSize());

	if(YB_UNLIKELY(view_size != buf_size))
	{
		YTraceDe(Warning, "Mismatched host renderer buffer size %s found.",
			to_string(buf_size).c_str());
		SetSize(view_size);
		YTraceDe(Warning, "Host renderer buffer size has adjusted to view size"
			" %s.", to_string(view_size).c_str());
		return true;
	}
	return {};
}

void
HostRenderer::InitWidgetView()
{
#if YCL_Win32
	auto& wnd(Wait());

	if(const auto p_wgt = dynamic_cast<UI::Widget*>(&widget.get()))
		p_wgt->SetView(
			make_unique<Windows::UI::ControlView>(wnd.GetNativeHandle()));
	wnd.GetGUIHostRef().Desktop += widget;
#endif
	// FIXME: Allow user to specify which kinds of views should be adjusted.
	auto& view_ref(widget.get().GetView());

	RootMode = typeid(view_ref) == typeid(UI::View);
}

void
HostRenderer::RefreshForWidget()
{
	if(const auto p_wnd = GetWindowPtr())
	{
#if YCL_Win32
		if(!p_wnd->IsMinimized())
#else
		yunused(p_wnd);
#endif
		{
			// TODO: Determine what should be painted for thumbnail.
			// TODO: Provide interface for custom painting.
			AdjustSize();

			auto& wgt(GetWidgetRef());
			const auto& g(GetContext());
			const auto r(GetInvalidatedArea());

			if(Validate(wgt, wgt, {g, {}, r}))
				Update(g.GetBufferPtr(), r);
		}
	}
}

void
HostRenderer::Update(ConstBitmapPtr p, const Rect& r)
{
	YAssertNonnull(p);
	if(const auto p_wnd = GetWindowPtr())
		try
		{
			auto& view(widget.get().GetView());
			const auto& view_size(view.GetSize());
			auto p_buf(rbuf.Lock());
			auto& buf(Deref(p_buf));
			const auto& buf_size(buf.GetSize());

			if(YB_UNLIKELY(view_size != buf_size))
				throw LoggedEvent(ystdex::sfmt("Mismatched host renderer buffer"
					" size %s and view size %s found.",
					to_string(buf_size).c_str(), to_string(view_size).c_str()));
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
				bounds.GetSizeRef() = view_size;
#	if !YCL_Android
				if(bounds != cbounds)
#		if YCL_HostedUI_XCB
					p_wnd->SetBounds(bounds);
#		else
					p_wnd->SetClientBounds(bounds);
#		endif
#	endif
			}
#if YCL_Win32
			p_wnd->UpdateFromBounds(p, buf, r, r.GetPoint());
#else
			yunused(r);
			p_wnd->UpdateFrom(p, buf);
#endif
		}
		// TODO: Trace?
#	if YCL_Win32
		CatchIgnore(Win32Exception&)
#	else
		CatchIgnore(Exception&) // XXX: Use proper platform-dependent type.
#	endif
		CatchExpr(LoggedEvent& e, ExtractAndTrace(e, Warning))
}

Window&
HostRenderer::Wait()
{
	observer_ptr<Host::Window> p_wnd;

	// XXX: Busy wait.
	while(!p_wnd)
		// FIXME: This thread will hang if window pointer is always not ready.
		p_wnd = GetWindowPtr();
	return *p_wnd;
}

} // namespace Host;
#endif

} // namespace YSLib;

