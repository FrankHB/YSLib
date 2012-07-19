/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSMain.cpp
\ingroup Helper
\brief DS 平台框架。
\version r3011;
\author FrankHB<frankhb1989@gmail.com>
\since build 296 。
\par 创建时间:
	2012-03-25 12:48:49 +0800;
\par 修改时间:
	2012-07-17 01:01 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Helper::DSMain;
*/


#include "Helper/DSMain.h"
#include "Helper/yglobal.h"
#include "Helper/Initialization.h"
#include <YSLib/Adaptor/Font.h>
#include <YSLib/Service/ytimer.h>
#include <YCLib/Debug.h>
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;
#ifdef YCL_DS
#include <YSLib/Service/yblit.h> // for Drawing::FillPixel;
#endif

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Devices)
class DSScreen;
YSL_END_NAMESPACE(Drawing)

using Devices::DSScreen;
using namespace Drawing;

namespace
{

#if YCL_MINGW32
yconstexpr double g_max_free_fps(1000);
std::chrono::nanoseconds host_sleep(u64(1000000000 / g_max_free_fps));
std::chrono::nanoseconds idle_sleep(u64(1000000000 / g_max_free_fps));


/*!
\brief 虚拟屏幕缓存。
\since build 299 。
*/
struct ScreenBuffer
{
	BitmapPtr pBuffer;
	::HBITMAP hBitmap;

	ScreenBuffer(const Size& s)
		: hBitmap(InitializeDIB(reinterpret_cast<void*&>(pBuffer),
		s.Width, s.Height))
	{}
	~ScreenBuffer()
	{
		::DeleteObject(hBitmap);
	}

private:
	::HBITMAP
	InitializeDIB(void*& pBuffer, SDst w, SDst h)
	{
		::BITMAPINFO bmi{{sizeof(::BITMAPINFO::bmiHeader), w,
			-h - 1, 1, 32, BI_RGB, sizeof(PixelType) * w * h, 0, 0, 0, 0}, {}};

		return ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pBuffer,
			NULL, 0);
	}
};
#endif

//注册的应用程序指针。
DSApplication* pApp;

#if YCL_HOSTED && YCL_MULTITHREAD == 1
//! \since build 325 。
void(DSApplication::*g_pm)();

//! \since build 325 。
void
HostThreadFunc()
{
	YAssert(pApp && g_pm, "Null pointer found.");

	(pApp->*g_pm)();
}
#endif


#ifndef NDEBUG
/*!
\brief 调试计时器。
\since build 314 。
*/
class DebugTimer
{
protected:
	std::string event_info;
	Timers::HighResolutionClock::time_point base_tick;

public:
	DebugTimer(const std::string& str = "")
		: event_info(str), base_tick()
	{
		std::printf("Start tick of [%s] :\n", event_info.c_str());
		base_tick = Timers::HighResolutionClock::now();
	}
	~DebugTimer()
	{
		const double t((Timers::HighResolutionClock::now() - base_tick).count()
			/ 1e6);

		std::printf("Performed [%s] in: %f milliseconds.\n",
			event_info.c_str(), t);
	}
};
#define YSL_DEBUG_DECL_TIMER(_name, ...) DebugTimer name(__VA_ARGS__);
#else
#define YSL_DEBUG_DECL_TIMER(...)
#endif

} // unnamed namespace;

YSL_BEGIN_NAMESPACE(Devices)

/*!
\brief DS 屏幕。
\since 早于 build 218 。
*/
class DSScreen : public Screen
{
#if YCL_DS
public:
	typedef int BGType;

private:
	BGType bg;

public:
	/*!
	\brief 构造：指定是否为下屏。
	\since build 325 。
	*/
	DSScreen(bool) ynothrow;

	DefGetter(const ynothrow, const BGType&, BgID, bg)

	/*!
	\brief 更新。
	\note 复制到屏幕。
	\since build 319 。
	*/
	void
	Update(Drawing::BitmapPtr) ynothrow override;
	/*!
	\brief 更新。
	\note 以纯色填充屏幕。
	*/
	void
	Update(Drawing::Color = Drawing::Color());
#elif YCL_MINGW32
public:
	Point Offset;

protected:
	/*!
	\brief 宿主窗口句柄。
	\since build 322 。
	*/
	::HWND hHost;

private:
	ScreenBuffer gbuf;
	//! \since build 322 。
	std::mutex update_mutex;

public:
	//! \since build 325 。
	DSScreen(bool, ::HWND) ynothrow;

	/*!
	\brief 更新。
	\note 复制到屏幕或屏幕缓冲区。
	\note 线程安全。
	\since build 319 。
	*/
	void
	Update(Drawing::BitmapPtr) ynothrow override;

	/*!
	\brief 更新到宿主。
	\param hDC 宿主窗口设备上下文句柄。
	\param hMemDC 内存设备上下文句柄。
	\note 复制到宿主窗口。
	\since build 319 。
	*/
	void
	UpdateToHost(::HDC hDC, ::HDC hMemDC) ynothrow;
#else
#	error Unsupported platform found!
#endif
};

#if YCL_DS
DSScreen::DSScreen(bool b) ynothrow
	: Devices::Screen(MainScreenWidth, MainScreenHeight)
{
	pBuffer = (b ? DS::InitScrDown : DS::InitScrUp)(bg);
}

void
DSScreen::Update(BitmapPtr buf) ynothrow
{
	DS::ScreenSynchronize(GetCheckedBufferPtr(), buf);
}
void
DSScreen::Update(Color c)
{
	FillPixel<PixelType>(GetCheckedBufferPtr(), GetAreaOf(GetSize()), c);
}
#elif YCL_MINGW32
DSScreen::DSScreen(bool b, ::HWND hWnd) ynothrow
	: Devices::Screen(MainScreenWidth, MainScreenHeight),
	Offset(), hHost(hWnd), gbuf(Size(MainScreenWidth, MainScreenHeight)),
	update_mutex()
{
	YAssert(bool(hWnd), "Null handle found.");

	pBuffer = gbuf.pBuffer;
	if(b)
		Offset.Y = MainScreenHeight;
}

void
DSScreen::Update(Drawing::BitmapPtr buf) ynothrow
{
	std::lock_guard<std::mutex> lck(update_mutex);

	std::memcpy(gbuf.pBuffer, buf,
		sizeof(PixelType) * size.Width * size.Height);
//	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	YCL_DEBUG_PUTS("Screen buffer updated.");
	YSL_DEBUG_DECL_TIMER(tmr, "DSScreen::Update")

	::HDC hDC(::GetDC(hHost));
	::HDC hMemDC(::CreateCompatibleDC(hDC));

	UpdateToHost(hDC, hMemDC);
	::DeleteDC(hMemDC);
	::ReleaseDC(hHost, hDC);
}

void
DSScreen::UpdateToHost(::HDC hDC, ::HDC hMemDC) ynothrow
{
	const auto& size(GetSize());

	::SelectObject(hMemDC, gbuf.hBitmap);
	// NOTE: Unlocked intentionally for performance.
	::BitBlt(hDC, Offset.X, Offset.Y, size.Width, size.Height,
		hMemDC, 0, 0, SRCCOPY);
}
#else
#	error Unsupported platform found!
#endif

YSL_END_NAMESPACE(Devices)

namespace
{

#if YCL_MINGW32
LRESULT CALLBACK
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
//	YSL_DEBUG_DECL_TIMER(tmr, "WndProc");
	switch(msg)
	{
	case WM_PAINT:
		YCL_DEBUG_PUTS("Handling of WM_PAINT.");
		{
			YSL_DEBUG_DECL_TIMER(tmr, "WM_PAINT");
			::PAINTSTRUCT ps;
			::HDC hDC(::BeginPaint(hWnd, &ps));
			::HDC hMemDC(::CreateCompatibleDC(hDC));

			ystdex::polymorphic_downcast<DSScreen&>(FetchGlobalInstance()
				.GetScreenUp()).UpdateToHost(hDC, hMemDC),
			ystdex::polymorphic_downcast<DSScreen&>(FetchGlobalInstance()
				.GetScreenDown()).UpdateToHost(hDC, hMemDC);
			::DeleteDC(hMemDC);
			::EndPaint(hWnd, &ps);
		}
		break;
	case WM_KILLFOCUS:
		YCL_DEBUG_PUTS("Handling of WM_KILLFOCUS.");
		platform_ex::ClearKeyStates();
		break;
	case WM_DESTROY:
		YCL_DEBUG_PUTS("Handling of WM_DESTROY.");
		::PostQuitMessage(0),
		YSL_ PostQuitMessage(0);
		// NOTE: Try to make sure all shells are released before destructing the
		//	instance of %DSApplication.
		break;
	default:
	//	YCL_DEBUG_PUTS("Handling of default procedure.");
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
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

/*!
\brief 取空闲消息。
\since build 320 。
*/
inline Message
FetchIdleMessage()
{
	return Message(SM_INPUT);
}

/*!
\brief 后台消息处理程序。
\since build 320 。
*/
inline void
Idle(Messaging::Priority prior)
{
	// Note: Wait for GUI input of any shells. Post message for specific shell
	//	would cause low performance when there are many candidate messages
	//	of distinct shells.
	PostMessage(FetchIdleMessage(), prior);
}

} // unnamed namespace;


DSApplication::DSApplication()
	:
#if YCL_HOSTED && YCL_MULTITHREAD == 1
	thread(),
#if YCL_MINGW32
	hHost(),
#endif
	mtx(), init(), full_init(),
#endif
	pFontCache(), pScreenUp(), pScreenDown(), UIResponseLimit(0x40)
{
	YAssert(!YSL_ pApp, "Duplicate instance found.");

	//注册全局应用程序实例。
	YSL_ pApp = this;

	//全局初始化。
	InitializeEnviornment();
	//若有必要，启动本机消息循环线程后完成应用程序实例其它部分的初始化（注意顺序）。
// NOTE: libstdc++ @ G++ 4.7.1 bug,
//	see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=53872 .
#if YCL_HOSTED && YCL_MULTITHREAD == 1
	g_pm = &DSApplication::HostTask;
	thread = std::thread(HostThreadFunc);
#endif


	//检查程序是否被正确安装。
	CheckInstall();
	//初始化系统字体资源。
	try
	{
		pFontCache = unique_raw(new FontCache());
	}
	catch(...)
	{
		throw LoggedEvent("Error occurred in creating font cache.");
	}
	InitializeSystemFontCache();
	//初始化系统设备。
#if YCL_DS
	InitVideo();
	try
	{
		pScreenUp = unique_raw(new DSScreen(false));
		pScreenDown = unique_raw(new DSScreen(true));
	}
#elif YCL_MINGW32
	{
		std::unique_lock<std::mutex>lck(mtx);

		init.wait(lck, [this]{return bool(hHost);});

		YAssert(bool(hHost), "Null handle found.");
	}
	try
	{
		pScreenUp = unique_raw(new DSScreen(false, hHost));
		pScreenDown = unique_raw(new DSScreen(true, hHost));
	}
#endif
	catch(...)
	{
		throw LoggedEvent("Screen initialization failed.");
	}

#if YCL_DS
	ystdex::polymorphic_downcast<DSScreen&>(*pScreenUp)
		.Update(ColorSpace::Blue),
	ystdex::polymorphic_downcast<DSScreen&>(*pScreenDown)
		.Update(ColorSpace::Green);
#elif YCL_MINGW32
	full_init.notify_one();
#endif
}

DSApplication::~DSApplication()
{
#if YCL_MINGW32
	thread.detach();
#endif

	//等待并确保所有 Shell 被释放。
//	hShell = nullptr;

	//释放全局非静态资源。

	//清理消息队列（必要，保证所有Shell在Application前析构）。
	Queue.Clear();

	//当主 Shell 句柄为静态存储期对象时需要通过 reset 释放。

	//释放默认字体资源。
	reset(pFontCache);

	//释放设备。
	reset(pScreenUp),
	reset(pScreenDown);
	Uninitialize();
}

FontCache&
DSApplication::GetFontCache() const ynothrow
{
	YAssert(bool(pFontCache), "Null pointer found.");

	return *pFontCache;
}

Devices::Screen&
DSApplication::GetScreenUp() const ynothrow
{
	YAssert(bool(pScreenUp), "Null pointer found.");

	return *pScreenUp;
}
Devices::Screen&
DSApplication::GetScreenDown() const ynothrow
{
	YAssert(bool(pScreenDown), "Null pointer found.");

	return *pScreenDown;
}

bool
DSApplication::DealMessage()
{
	using namespace Shells;

	if(Queue.IsEmpty())
	{
	//	Idle(UIResponseLimit);
		OnGotMessage(FetchIdleMessage());
#if YCL_MINGW32
	//	std::this_thread::yield();
		std::this_thread::sleep_for(idle_sleep);
#endif
	}
	else
	{
		// TODO: Consider the application queue to be locked for thread safety.
		const auto i(Queue.GetBegin());

		if(YB_UNLIKELY(i->second.GetMessageID() == SM_QUIT))
			return false;
		if(i->first < UIResponseLimit)
		{
			Idle(UIResponseLimit);
#if YCL_MINGW32
			std::this_thread::sleep_for(idle_sleep);
#endif
		}
		OnGotMessage(i->second);
		Queue.Erase(i);
	}
	return true;
}

#if YCL_MINGW32
void
DSApplication::HostTask()
{
	{
		std::lock_guard<std::mutex> lck(mtx);

		hHost = YSL_ InitializeMainWindow();
		// NOTE: Currently there is only one client.
		init.notify_one();
	//	Initialized.notify_all();
	}
	::ShowWindow(hHost, SW_SHOWNORMAL);
	{
		std::unique_lock<std::mutex>lck(mtx);

		full_init.wait(lck,
			[this]{return bool(pScreenUp) && bool (pScreenDown);});

		YAssert(bool(pScreenUp) && bool(pScreenDown), "Null pointer found.");
	}

	::MSG host_msg; //!< 本机消息类型。

	while(true)
		if(::PeekMessage(&host_msg, NULL, 0, 0, PM_REMOVE))
			::DispatchMessage(&host_msg);
		else
		//	std::this_thread::yield();
			std::this_thread::sleep_for(host_sleep);
}
#endif


Application&
FetchAppInstance()
{
	return FetchGlobalInstance();
}

DSApplication&
FetchGlobalInstance() ynothrow
{
	YAssert(pApp, "Null pointer found.");

	return *pApp;
}

bool
#if YCL_DS
InitConsole(Devices::Screen& scr, Drawing::PixelType fc, Drawing::PixelType bc)
{
	using namespace platform;

	if(&FetchGlobalInstance().GetScreenUp() == &scr)
		YConsoleInit(true, fc, bc);
	else if(&FetchGlobalInstance().GetScreenDown() == &scr)
		YConsoleInit(false, fc, bc);
	else
		return false;
#elif YCL_MINGW32
InitConsole(Devices::Screen&, Drawing::PixelType, Drawing::PixelType)
{
#else
#	error Unsupported platform found!
#endif
	return true;
}

void
ShowFatalError(const char* s)
{
	using namespace platform;

	YDebugSetStatus();
	YDebugBegin();
	std::printf("Fatal Error:\n%s\n", s);
	terminate();
}

YSL_END

