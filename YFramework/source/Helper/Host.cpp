/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Host.cpp
\ingroup Helper
\brief 宿主环境。
\version r1284
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:27:29 +0800
\par 修改时间:
	2014-03-01 09:43 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Host
*/


#include "Helper/YModules.h"
#include YFM_Helper_Host
#include YFM_Helper_ShellHelper // for YCL_DEBUG_PUTS, YSL_DEBUG_DECL_TIMER;

namespace YSLib
{

using namespace Drawing;

namespace
{

#if YCL_Win32 && 0
yconstexpr double g_max_free_fps(1000);
std::chrono::nanoseconds host_sleep(u64(1000000000 / g_max_free_fps));
#endif

} // unnamed namespace;


#if YF_Hosted
namespace Host
{

namespace
{

#	if YCL_Win32
::LRESULT CALLBACK
WndProc(::HWND h_wnd, ::UINT msg, ::WPARAM w_param, ::LPARAM l_param)
{
#		define YSL_WndProc_BeginCase(_n) \
			case YPP_Join(WM_, _n): \
				if(YB_LIKELY(p)) \
				{
#		define YSL_WndProc_BeginCase_Trace(_n) \
			case YPP_Join(WM_, _n): \
				YCL_DEBUG_PUTS("Handling of WM_" #_n "."); \
				if(YB_LIKELY(p)) \
				{ \
					YSL_DEBUG_DECL_TIMER(tmr, "WM_" #_n)
#		define YSL_WndProc_EndCase() \
		} \
		break;

//	YSL_DEBUG_DECL_TIMER(tmr, "WndProc")
	const auto p(reinterpret_cast<Window*>(::GetWindowLongPtrW(h_wnd,
		GWLP_USERDATA)));

	switch(msg)
	{
	YSL_WndProc_BeginCase_Trace(PAINT)
		p->OnPaint();
	YSL_WndProc_EndCase()
	YSL_WndProc_BeginCase_Trace(KILLFOCUS)
		p->OnLostFocus();
	YSL_WndProc_EndCase()
	YSL_WndProc_BeginCase_Trace(DESTROY)
		p->OnDestroy();
	YSL_WndProc_EndCase()
	YSL_WndProc_BeginCase(INPUT)
		::UINT size(sizeof(::RAWINPUT));
		byte lpb[sizeof(::RAWINPUT)]{};

		if(YB_LIKELY(::GetRawInputData(::HRAWINPUT(l_param), RID_INPUT, lpb,
			&size, sizeof(::RAWINPUTHEADER)) != ::UINT(-1)))
		{
			const auto p_raw(reinterpret_cast< ::RAWINPUT*>(lpb));

			if(YB_LIKELY(p_raw->header.dwType == RIM_TYPEMOUSE))
			{
				if(p_raw->data.mouse.usButtonFlags == RI_MOUSE_WHEEL)
					p->GetHost().RawMouseButton
						= p_raw->data.mouse.usButtonData;
			}
		}
	YSL_WndProc_EndCase()
	default:
	//	YCL_DEBUG_PUTS("Handling of default procedure.");
		return ::DefWindowProcW(h_wnd, msg, w_param, l_param);
	}
	return 0;
#		undef YSL_WndProc_BeginCase
#		undef YSL_WndProc_BeginCase_Trace
#		undef YSL_WndProc_EndCase
}
#	endif

} // unnamed namespace;


Environment::Environment()
	: wnd_map(), wmap_mtx()
#	if YF_Multithread == 1
	, wnd_thrd_count(),
#		if YCL_Win32
	RawMouseButton(0),
#		endif
	ExitOnAllWindowThreadCompleted()
#		if YCL_Win32
	, window_class(WindowClassName, WndProc)
#		endif
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
}

Window*
Environment::GetForegroundWindow() const ynothrow
{
#ifdef YCL_Win32
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
#	if YCL_Win32
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

#	if YF_Multithread == 1
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
		if(pr.second)
			pr.second->Refresh();
}

} // namespace Host;
#endif

} // namespace YSLib;

