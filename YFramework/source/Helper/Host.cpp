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
\version r301
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:27:29 +0800
\par 修改时间:
	2013-02-08 02:50 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::Host
*/


#include "Host.h"
#include "DSScreen.h"
#include "Helper/DSMain.h"
#include "Helper/ShellHelper.h" // for YCL_DEBUG_PUTS, YSL_DEBUG_DECL_TIMER;
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;

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

YSL_BEGIN_NAMESPACE(Host)

namespace
{

#if YCL_MINGW32
Host::Window&
FetchMappedWindow(::HWND h)
{
	const auto p(FetchGlobalInstance().GetHost().WindowsMap[h]);

	YAssert(p, "Unmapped window handle found.");
	YAssert(&p->GetHost() == &FetchGlobalInstance().GetHost(),
		"Mismatched host environment found.");
	return *p;
}

::LRESULT CALLBACK
WndProc(::HWND h_wnd, ::UINT msg, ::WPARAM w_param, ::LPARAM l_param)
{
//	YSL_DEBUG_DECL_TIMER(tmr, "WndProc")

	switch(msg)
	{
	case WM_PAINT:
		YCL_DEBUG_PUTS("Handling of WM_PAINT.");
		{
			YSL_DEBUG_DECL_TIMER(tmr, "WM_PAINT")
			FetchMappedWindow(h_wnd).OnPaint();
		}
		break;
	case WM_KILLFOCUS:
		YCL_DEBUG_PUTS("Handling of WM_KILLFOCUS.");
		FetchMappedWindow(h_wnd).OnLostFocus();
		break;
	case WM_DESTROY:
		YCL_DEBUG_PUTS("Handling of WM_DESTROY.");
		FetchMappedWindow(h_wnd).OnDestroy();
		break;
	default:
	//	YCL_DEBUG_PUTS("Handling of default procedure.");
		return ::DefWindowProcW(h_wnd, msg, w_param, l_param);
	}
	return 0;
}

::HWND
InitializeMainWindow()
{
	yconstexpr auto wnd_class_name(L"YSTest_Class");
	yconstexpr auto wnd_title(L"YSTest");
	::WNDCLASSEX wCl;

	yunseq(wCl.hInstance = ::GetModuleHandleW(NULL),
		wCl.lpszClassName = wnd_class_name,
		wCl.lpfnWndProc = WndProc,
		wCl.style = CS_DBLCLKS,
	//	wCl.style = CS_HREDRAW | CS_VREDRAW,
		wCl.cbSize = sizeof(wCl),
		wCl.hIcon = ::LoadIcon(NULL, IDI_APPLICATION),
		wCl.hIconSm = ::LoadIcon(NULL, IDI_APPLICATION),
		wCl.hCursor = ::LoadCursor(NULL, IDC_ARROW),
		wCl.lpszMenuName = NULL,
		wCl.cbClsExtra = 0,
		wCl.cbWndExtra = 0,
		wCl.hbrBackground = ::GetSysColorBrush(COLOR_MENU)
	//	wCl.hbrBackground = ::HBRUSH(COLOR_MENU + 1)
	);

	if(!::RegisterClassEx(&wCl))
		throw LoggedEvent("This program requires Windows NT!");
	//	::MessageBox(NULL, "This program requires Windows NT!",
		//	wnd_title, MB_ICONERROR);

	yconstexpr ::DWORD wstyle(WS_TILED | WS_CAPTION | WS_SYSMENU
		| WS_MINIMIZEBOX);
	yconstexpr u16 wnd_w(256), wnd_h(384);
	::RECT rect{0, 0, wnd_w, wnd_h};

	::AdjustWindowRect(&rect, wstyle, FALSE);
	return ::CreateWindowEx(0, wnd_class_name, wnd_title,
		wstyle, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left,
		rect.bottom - rect.top, HWND_DESKTOP, NULL, wCl.hInstance, NULL);
}
#endif

} // unnamed namespace;


Window::Window(NativeHandle h, Environment& env)
	: p_env(&env), h_wnd(h)
{
	YAssert(::IsWindow(h), "Invalid window handle found.");
	YAssert(::GetWindowThreadProcessId(h, NULL) == ::GetCurrentThreadId(),
		"Window not created on current thread found.");
	YAssert(env.WindowsMap.find(h) == env.WindowsMap.end()
		|| !env.WindowsMap[h] || env.WindowsMap[h] == this,
		"Mismatch registered window pointer found.");

	p_env->WindowsMap[h_wnd] = this;
}
Window::~Window()
{
	YAssert(p_env, "Null pointer found.");

	p_env->WindowsMap[h_wnd] = nullptr;
	// Note: The window could be already destroyed in window procedure.
	if(::IsWindow(h_wnd))
		::DestroyWindow(h_wnd);
}

void
Window::Close()
{
	::SendNotifyMessageW(h_wnd, WM_CLOSE, 0, 0);
}

void
Window::OnDestroy()
{
	::PostQuitMessage(0),
	YSLib::PostQuitMessage(0);
	// NOTE: Try to make sure all shells are released before destructing the
	//	instance of %DSApplication.
}

void
Window::OnLostFocus()
{
	platform_ex::ClearKeyStates();
}

void
Window::OnPaint()
{
	using Devices::DSScreen;

	::PAINTSTRUCT ps;
	::HDC hDC(::BeginPaint(h_wnd, &ps));
	::HDC hMemDC(::CreateCompatibleDC(hDC));

	ystdex::polymorphic_downcast<DSScreen&>(FetchGlobalInstance()
		.GetScreenUp()).UpdateToHost(hDC, hMemDC),
	ystdex::polymorphic_downcast<DSScreen&>(FetchGlobalInstance()
		.GetScreenDown()).UpdateToHost(hDC, hMemDC);
	::DeleteDC(hMemDC);
	::EndPaint(h_wnd, &ps);
}

void
Window::Show()
{
	::ShowWindowAsync(h_wnd, SW_SHOWNORMAL);
}


Environment::Environment()
#	if YCL_MULTITHREAD == 1
	: mtx(), init(), full_init(),
#		if YCL_MINGW32
	p_main_wnd(),
#		endif
	host_thrd(std::thread(std::mem_fn(&Environment::HostTask), this)),
#	endif
	WindowsMap()
{}
Environment::~Environment()
{
	host_thrd.detach();
}

#	if YCL_MULTITHREAD == 1
void
Environment::HostLoop()
{
#		if YCL_MINGW32
	while(true)
	{
		::MSG msg; //!< 本机消息。

		if(::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) != 0)
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
}

void
Environment::HostTask()
{
#		if YCL_MINGW32
	{
		std::lock_guard<std::mutex> lck(mtx);
		const auto h_wnd(Host::InitializeMainWindow());

		// TODO: Exception safety.
		// TODO: Handle window creation failure.
		p_main_wnd.reset(new Window(h_wnd, *this));
		init.notify_all();
	}
	p_main_wnd->Show();
	{
		std::unique_lock<std::mutex>lck(mtx);

		full_init.wait(lck, []{return FetchGlobalInstance().IsScreenReady();});

		YAssert(FetchGlobalInstance().IsScreenReady(), "Screen is not ready.");
	}
	HostLoop();
#		endif
}

void
Environment::Notify()
{
	full_init.notify_one();
}

const shared_ptr<Window>&
Environment::Wait()
{
	std::unique_lock<std::mutex>lck(mtx);

#		if YCL_MINGW32
	init.wait(lck, [this]{return bool(p_main_wnd);});

	YAssert(bool(p_main_wnd), "Null pointer found.");
#		endif

	return p_main_wnd;
}
#	endif

YSL_END_NAMESPACE(Host)
#endif

YSL_END

