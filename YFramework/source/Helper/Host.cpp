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
\version r862
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:27:29 +0800
\par 修改时间:
	2013-03-23 21:18 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::Host
*/


#include "Host.h"
#include "DSScreen.h"
#include "Helper/DSMain.h"
#include "Helper/ShellHelper.h" // for YCL_DEBUG_PUTS, YSL_DEBUG_DECL_TIMER;

YSL_BEGIN

using namespace Drawing;

namespace
{

#if YCL_MINGW32
yconstexpr double g_max_free_fps(1000);
std::chrono::nanoseconds host_sleep(u64(1000000000 / g_max_free_fps));
#endif

} // unnamed namespace;


#if YCL_HOSTED
using Devices::DSScreen;

YSL_BEGIN_NAMESPACE(Host)

namespace
{

#if YCL_MINGW32
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

//! \since build 381
::HWND
InitializeMainWindow(const wchar_t* wnd_title, u16 wnd_w, u16 wnd_h,
	::DWORD wstyle = WS_TILED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
{
	::RECT rect{0, 0, wnd_w, wnd_h};

	::AdjustWindowRect(&rect, wstyle, FALSE);
	return ::CreateWindowW(WindowClassName, wnd_title, wstyle, CW_USEDEFAULT,
		CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		HWND_DESKTOP, nullptr, ::GetModuleHandleW(nullptr), nullptr);
}


/*!
\brief 双屏幕宿主窗口。
\since build 383
*/
class DSWindow : public Window
{
public:
	DSWindow(NativeWindowHandle h,
		Environment& e = FetchGlobalInstance().GetHost())
		: Window(h, e)
	{}

	pair<Point, Point>
	GetInputBounds() const ynothrow override
	{
		return {Point(0, MainScreenHeight),
			Point(MainScreenWidth, MainScreenHeight << 1)};
	}

	void
	OnDestroy() override
	{
		Window::OnDestroy(),
		YSLib::PostQuitMessage(0);
		// NOTE: Try to make sure all shells are released before destructing the
		//	instance of %DSApplication.
	}

	void
	OnPaint() override
	{
		GSurface<WindowRegionDeviceContext> sf(GetNativeHandle());
		auto& app(FetchGlobalInstance());

		app.GetDSScreenUp().UpdateToSurface(sf),
		app.GetDSScreenDown().UpdateToSurface(sf);
	}
};
#endif

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
	thrd.join();
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
	wnd.Show();
	Environment::HostLoop();
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
	, p_main_wnd()
#		endif
	, host_thrd(std::thread(std::mem_fn(&Environment::HostTask), this))
#	endif
{}
Environment::~Environment()
{
	YCL_DEBUG_PUTS("Host environment lifetime ended.");

	using ystdex::get_value;

	std::for_each(wnd_map.cbegin() | get_value, wnd_map.cend() | get_value,
		[](Window* const& p){
			p->Close();
	});
#	if YCL_MULTITHREAD == 1
	// TODO: Exception safety: add either assertion or logging when throwing.
	host_thrd.join();
	YCL_DEBUG_PUTS("Host thread dropped.");
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

Window&
Environment::GetMainWindow() const ynothrow
{
	YAssert(bool(p_main_wnd), "Null pointer found.");

	return *p_main_wnd;
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

#	if YCL_MULTITHREAD == 1
void
Environment::HostLoop()
{
	YCL_DEBUG_PUTS("Host loop beginned.");
#		if YCL_MINGW32
	while(true)
	{
		::MSG msg; //!< 本机消息。

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
#		endif
	YCL_DEBUG_PUTS("Host loop ended.");
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

#	if YCL_MULTITHREAD == 1
void
Environment::HostTask()
{
#		if YCL_MINGW32
	class reg_wnd
	{
	protected:
		::HINSTANCE hInstance;

	public:
		reg_wnd()
			: hInstance(::GetModuleHandleW(nullptr))
		{
			const ::WNDCLASS wnd_class{CS_DBLCLKS/* | CS_HREDRAW | CS_VREDRAW*/,
				WndProc, 0, 0, hInstance, ::LoadIconW(nullptr, IDI_APPLICATION),
				::LoadCursorW(nullptr, IDC_ARROW), ::HBRUSH(COLOR_MENU + 1),
				nullptr, WindowClassName};

			if(!::RegisterClassW(&wnd_class))
				throw LoggedEvent("Windows registration failed.");
			//	::MessageBox(nullptr, "This program requires Windows NT!",
				//	wnd_title, MB_ICONERROR);
			YCL_DEBUG_PUTS("Window class registered.");
		}
		~reg_wnd()
		{
			::UnregisterClassW(WindowClassName, hInstance);
			YCL_DEBUG_PUTS("Window class unregistered.");
		}
	} guard;
	const auto h_wnd(Host::InitializeMainWindow(L"YSTest", 256, 384));

	// TODO: Handle window creation failure.
	p_main_wnd.reset(new DSWindow(h_wnd, *this));
	p_main_wnd->Show();
	{
		auto& app(FetchGlobalInstance());

		Devices::InitDSScreen(app.scrs[0], app.scrs[1]);

		YAssert(FetchGlobalInstance().IsScreenReady(), "Screen is not ready.");
	}
	HostLoop();
	YCL_DEBUG_PUTS("Host task ended.");
#		endif
}
#	endif

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
				{rd.GetContext(), Point(), GetBoundsOf(wgt)}))
				rd.Update(rd.GetContext().GetBufferPtr());
		}
}

YSL_END_NAMESPACE(Host)
#endif

YSL_END

