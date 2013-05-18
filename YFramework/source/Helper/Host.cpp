/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Host.cpp
\ingroup Helper
\brief DS 平台框架。
\version r1058
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:27:29 +0800
\par 修改时间:
	2013-05-17 20:29 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::Host
*/


#include "Host.h"
#include "Helper/GUIApplication.h"
#include "Helper/ShellHelper.h" // for YCL_DEBUG_PUTS, YSL_DEBUG_DECL_TIMER;

YSL_BEGIN

using namespace Drawing;

namespace
{

#if YCL_MINGW32 && 0
yconstexpr double g_max_free_fps(1000);
std::chrono::nanoseconds host_sleep(u64(1000000000 / g_max_free_fps));
#endif

} // unnamed namespace;


#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

namespace
{

#	if YCL_MINGW32
::LRESULT CALLBACK
WndProc(::HWND h_wnd, ::UINT msg, ::WPARAM w_param, ::LPARAM l_param)
{
//	YSL_DEBUG_DECL_TIMER(tmr, "WndProc")
	const auto p(reinterpret_cast<Window*>(::GetWindowLongPtrW(h_wnd,
		GWLP_USERDATA)));

	switch(msg)
	{
	case WM_PAINT:
		YCL_DEBUG_PUTS("Handling of WM_PAINT.");
		if(p)
		{
			YSL_DEBUG_DECL_TIMER(tmr, "WM_PAINT")

			p->OnPaint();
		}
		break;
	case WM_KILLFOCUS:
		YCL_DEBUG_PUTS("Handling of WM_KILLFOCUS.");
		if(p)
			p->OnLostFocus();
		break;
	case WM_DESTROY:
		YCL_DEBUG_PUTS("Handling of WM_DESTROY.");
		if(p)
			p->OnDestroy();
		break;
	default:
	//	YCL_DEBUG_PUTS("Handling of default procedure.");
		return ::DefWindowProcW(h_wnd, msg, w_param, l_param);
	}
	return 0;
}
#	endif

} // unnamed namespace;


void
RenderWindow::OnPaint()
{
	GSurface<WindowRegionDeviceContext> sf(GetNativeHandle());

	renderer.get().UpdateToSurface(sf);
}


WindowThread::~WindowThread()
{
	YAssert(bool(p_wnd), "Null pointer found.");

	p_wnd->Close();
	// NOTE: If the thread has been already completed there is no effect.
	// TODO: Exception safety: add either assertion or logging when throwing
	//	other exceptions.
	try
	{
		thrd.join();
	}
	catch(std::invalid_argument&)
	{}
}

void
WindowThread::ThreadLoop(NativeWindowHandle h_wnd)
{
	p_wnd.reset(new Window(h_wnd));
	WindowLoop(*p_wnd);
}
void
WindowThread::ThreadLoop(unique_ptr<Window> p)
{
	p_wnd = std::move(p);
	WindowLoop(*p_wnd);
}

void
WindowThread::WindowLoop(Window& wnd)
{
#	if YCL_MULTITHREAD
	auto& env(wnd.GetHost());

	env.EnterWindowThread();
#	endif
	wnd.Show();
	Environment::HostLoop();
#	if YCL_MULTITHREAD
	env.LeaveWindowThread();
#	endif
}


void
HostRenderer::SetSize(const Size& s)
{
	BufferedRenderer::SetSize(s);
}

void
HostRenderer::Update(BitmapPtr buf)
{
	YAssert(GetSizeOf(widget) == rbuf.GetSize(), "Mismatched size found.");

	rbuf.UpdateFrom(buf);
	if(const auto p_wnd = GetWindowPtr())
		rbuf.UpdateTo(p_wnd->GetNativeHandle());
}


Environment::Environment()
	: wnd_map(), wmap_mtx()
#	if YCL_MULTITHREAD == 1
#		if YCL_MINGW32
	, h_instance(::GetModuleHandleW(nullptr))
#		endif
	, wnd_thrd_count(), ExitOnAllWindowThreadCompleted()
#	endif
{
#	if YCL_MINGW32
	// NOTE: Intentionally no %CS_OWNDC or %CS_CLASSDC, so %::ReleaseDC
	//	is always needed.
	const ::WNDCLASS wnd_class{CS_DBLCLKS/* | CS_HREDRAW | CS_VREDRAW*/,
		WndProc, 0, 0, h_instance, ::LoadIconW(nullptr, IDI_APPLICATION),
		::LoadCursorW(nullptr, IDC_ARROW), ::HBRUSH(COLOR_MENU + 1),
		nullptr, WindowClassName};

	if(!::RegisterClassW(&wnd_class))
		throw LoggedEvent("Windows registration failed.");
	//	::MessageBox(nullptr, "This program requires Windows NT!",
		//	wnd_title, MB_ICONERROR);
	YCL_DEBUG_PUTS("Window class registered.");
#	endif
}
Environment::~Environment()
{
	YCL_DEBUG_PUTS("Host environment lifetime ended.");

	using ystdex::get_value;

	std::for_each(wnd_map.cbegin() | get_value, wnd_map.cend() | get_value,
		[](Window* const& p){
			p->Close();
	});
#	if YCL_MULTITHREAD == 1
#		if YCL_MINGW32
	::UnregisterClassW(WindowClassName, h_instance);
	YCL_DEBUG_PUTS("Window class unregistered.");
#		endif
#	endif
}

Window*
Environment::GetForegroundWindow() const ynothrow
{
#ifdef YCL_MINGW32
	return FindWindow(::GetForegroundWindow());
#endif
	return nullptr;
}

void
Environment::AddMappedItem(NativeWindowHandle h, Window* p)
{
	std::unique_lock<std::mutex> lck(wmap_mtx);

	// TODO: Use %emplace.
	wnd_map.insert(make_pair(h, p));
}

Window*
Environment::FindWindow(NativeWindowHandle h) const ynothrow
{
	std::unique_lock<std::mutex> lck(wmap_mtx);
	const auto i(wnd_map.find(h));

	return i == wnd_map.end() ? nullptr : i->second;
}

void
Environment::HostLoop()
{
	YCL_DEBUG_PUTS("Host loop beginned.");
#	if YCL_MINGW32
	while(true)
	{
		::MSG msg{nullptr, 0, 0, 0, 0, {0, 0}}; //!< 本机消息。

		if(::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			if(msg.message == WM_QUIT)
				break;
		//	if(!PreTranslateMessage(&msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessageW(&msg);
			}
		//	if(CheckCloseDialog(frm, false))
			//	break;
		}
		else
		//	std::this_thread::yield();
		//	std::this_thread::sleep_for(host_sleep);
			// NOTE: Failure ignored.
			::WaitMessage();
	}
#	endif
	YCL_DEBUG_PUTS("Host loop ended.");
}

#	if YCL_MULTITHREAD == 1
void
Environment::LeaveWindowThread()
{
	if(--wnd_thrd_count == 0 && ExitOnAllWindowThreadCompleted)
		YSLib::PostQuitMessage(0);
}
#	endif

void
Environment::RemoveMappedItem(NativeWindowHandle h) ynothrow
{
	std::unique_lock<std::mutex> lck(wmap_mtx);
	const auto i(wnd_map.find(h));

	if(i != wnd_map.end())
		wnd_map.erase(i);
}

void
Environment::UpdateRenderWindows()
{
	std::unique_lock<std::mutex> lck(wmap_mtx);

	for(const auto& pr : wnd_map)
		if(auto p_wnd = dynamic_cast<RenderWindow*>(pr.second))
		{
			auto& rd(p_wnd->GetRenderer());
			auto& wgt(rd.GetWidgetRef());

			if(rd.Validate(wgt, wgt,
				{rd.GetContext(), Point(), rd.GetInvalidatedArea()}))
				rd.Update(rd.GetContext().GetBufferPtr());
		}
}

YSL_END_NAMESPACE(Host)
#endif

YSL_END

