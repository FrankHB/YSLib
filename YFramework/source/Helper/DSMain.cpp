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
\version r1920;
\author FrankHB<frankhb1989@gmail.com>
\since build 296 。
\par 创建时间:
	2012-03-25 12:48:49 +0800;
\par 修改时间:
	2012-04-24 21:26 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Helper::DSMain;
*/


#include "Helper/DSMain.h"
#include "Helper/yglobal.h"
#include "Helper/Initialization.h"
#include <YSLib/UI/ydesktop.h>
#include "Helper/shlds.h"
#include <YSLib/UI/ygui.h>
#include <YCLib/Debug.h>
#ifdef YCL_MINGW32
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#endif

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Devices)
class DSScreen;
YSL_END_NAMESPACE(Devices)

using Devices::DSScreen;
using namespace Drawing;

namespace
{

#ifdef YCL_MINGW32

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
			-h - 1, 1, 32, BI_RGB, sizeof(PixelType) * w * h}, {}};

		return ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pBuffer,
			NULL, 0);
	}
};


/*!
\brief 本机主窗口。
\since build 299 。
*/
std::atomic< ::HWND> hWindow;
/*!
\brief 宿主环境互斥量。
\since build 299 。
*/
std::mutex g_mutex;
/*!
\brief 宿主环境就绪条件。
\since build 299 。
*/
std::condition_variable g_cond;

/*!
\brief 宿主背景线程指针。
\since build 300 。
*/
std::thread* pHostThread;

#endif

/*!
\brief 默认消息发生函数。
*/
void
Idle()
{
	//指示等待图形用户界面输入。
	PostMessage(FetchShellHandle(), SM_INPUT, 0x40);
#ifdef YCL_MINGW32
	//	std::this_thread::yield();
		std::this_thread::sleep_for(idle_sleep);
#endif
}

//注册的应用程序指针。
DSApplication* pApp;

/*
\brief 全局生存期屏幕。
\since build 297 。
*/
//@{
DSScreen* pScreenUp;
DSScreen* pScreenDown;
//@}

} // unnamed namespace;

YSL_BEGIN_NAMESPACE(Devices)

/*!
\brief DS 屏幕。
\since 早于 build 218 。
*/
class DSScreen : public Screen
{
#ifdef YCL_DS
public:
	typedef int BGType;

private:
	BGType bg;

public:
	/*!
	\brief 构造：指定宽度和高度，从指定缓冲区指针。
	*/
	DSScreen(SDst, SDst, Drawing::BitmapPtr = nullptr);

	/*!
	\brief 复位。
	\note 无条件初始化。
	*/
	static void
	Reset();

	/*!
	\brief 取指针。
	\note 无异常抛出。
	\note 进行状态检查。
	*/
	virtual Drawing::BitmapPtr
	GetCheckedBufferPtr() const ynothrow;
	DefGetter(const ynothrow, const BGType&, BgID, bg)

	/*!
	\brief 更新。
	\note 复制到屏幕。
	*/
	void
	Update(Drawing::BitmapPtr);
	/*!
	\brief 更新。
	\note 以纯色填充屏幕。
	*/
	void
	Update(Drawing::Color = Drawing::Color());
#elif defined(YCL_MINGW32)
public:
	Point Offset;

private:
	ScreenBuffer gbuf;

protected:
	Drawing::BitmapPtr pSrc;

public:
	DSScreen(SDst, SDst);

	/*!
	\brief 更新。
	\note 复制到屏幕。
	\since build 299 。
	*/
	void
	Update(Drawing::BitmapPtr);

	/*!
	\brief 更新到宿主。
	\param hDC 宿主窗口设备上下文句柄。
	\param hMemDC 内存设备上下文句柄。
	\note 复制到宿主窗口。
	\since build 299 。
	*/
	void
	UpdateToHost(::HDC hDC, ::HDC hMemDC);
#else
#	error Unsupported platform found!
#endif
};

#ifdef YCL_DS
DSScreen::DSScreen(SDst w, SDst h, BitmapPtr p)
	: Devices::Screen(w, h, p),
	bg(-1)
{}

BitmapPtr
DSScreen::GetCheckedBufferPtr() const ynothrow
{
	if(YCL_UNLIKELY(!GetBufferPtr()))
	{
		InitVideo();

		// NOTE: assert(YSL_ pScreenUp && YSL_ pScreenDown);
		yunseq(YSL_ pScreenUp->pBuffer = DS::InitScrUp(YSL_ pScreenUp->bg),
			YSL_ pScreenDown->pBuffer = DS::InitScrDown(YSL_ pScreenDown->bg));
	}
	return Devices::Screen::GetCheckedBufferPtr();
}

void
DSScreen::Update(BitmapPtr buf)
{
	DS::ScreenSynchronize(GetCheckedBufferPtr(), buf);
}
void
DSScreen::Update(Color c)
{
	FillPixel<PixelType>(GetCheckedBufferPtr(), GetAreaOf(GetSize()), c);
}
#elif defined(YCL_MINGW32)
DSScreen::DSScreen(SDst w, SDst h)
	: Devices::Screen(w, h),
	Offset(), gbuf(Size(w, h)), pSrc()
{
	pBuffer = gbuf.pBuffer;
}

void
DSScreen::Update(Drawing::BitmapPtr p)
{
	pSrc = p;
//	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	std::printf("Screen updated.\n");

	if(hWindow)
	{
		::HDC hDC(::GetDC(hWindow));
		::HDC hMemDC(::CreateCompatibleDC(hDC));

		UpdateToHost(hDC, hMemDC);
		::DeleteDC(hMemDC);
		::ReleaseDC(hWindow, hDC);
	}
}

void
DSScreen::UpdateToHost(::HDC hDC, ::HDC hMemDC)
{
	if(pSrc)
	{
		const auto& size(GetSize());

		::SelectObject(hMemDC, gbuf.hBitmap);
		// NOTE: unlocked intentionally for efficiency;
		std::memcpy(gbuf.pBuffer, pSrc,
			sizeof(PixelType) * size.Width * size.Height);
		::BitBlt(hDC, Offset.X, Offset.Y, size.Width, size.Height,
			hMemDC, 0, 0, SRCCOPY);
	}
}
#else
#	error Unsupported platform found!
#endif

YSL_END_NAMESPACE(Devices)


#ifdef YCL_MINGW32
namespace
{

::LARGE_INTEGER liFrequency;
::LARGE_INTEGER liStart;
::LARGE_INTEGER liEnd;

void
StartClock()
{
	::QueryPerformanceFrequency(&liFrequency);
	::QueryPerformanceCounter(&liStart);
}

void
EndClock()
{
	::QueryPerformanceCounter(&liEnd);
	std::printf("Painted performed in: %f milliseconds.\n",
		double(1000 * 1.0 / liFrequency.QuadPart
		* (liEnd.QuadPart - liStart.QuadPart)));
}

LRESULT CALLBACK
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		YAssert(hWnd == hWindow, "Wrong native main window found.");

		StartClock();
		{
			::PAINTSTRUCT ps;
			::HDC hDC(::BeginPaint(hWindow, &ps));
			::HDC hMemDC(::CreateCompatibleDC(hDC));

			pScreenUp->UpdateToHost(hDC, hMemDC),
			pScreenDown->UpdateToHost(hDC, hMemDC);
			::DeleteDC(hMemDC);
			::EndPaint(hWindow, &ps);
		}
		EndClock();
		break;
	case WM_KILLFOCUS:
		yunseq(platform_ex::KeyState.reset(), platform_ex::OldKeyState.reset());
		break;
	case WM_DESTROY:
		::PostQuitMessage(0),
		YSL_ PostQuitMessage(0);
		// NOTE: make sure all shells are released before destructing the
		//	instance of %DSApplication;
		break;
	default:
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void
InitializeMainWindow()
{
	::HINSTANCE hInstance(::GetModuleHandleW(NULL));
	::WNDCLASSEX wCl;
	const auto wnd_class_name(L"YSTest_Class");
	const auto wnd_title(L"YSTest");
	const u16 wnd_w(256), wnd_h(384);

	yunseq(wCl.hInstance = hInstance,
		wCl.lpszClassName = wnd_class_name,
		wCl.lpfnWndProc = WndProc,
		wCl.style = CS_DBLCLKS,
//		wCl.style = CS_HREDRAW | CS_VREDRAW,
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

	::RECT rect{0, 0, wnd_w, wnd_h};
	const ::DWORD wstyle(WS_TILED | WS_CAPTION | WS_SYSMENU
		| WS_MINIMIZEBOX);

	::AdjustWindowRect(&rect, wstyle, FALSE);
	{
		std::lock_guard<std::mutex> lck(g_mutex);

		hWindow = ::CreateWindowEx(0, wnd_class_name, wnd_title,
			wstyle, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left,
			rect.bottom - rect.top, HWND_DESKTOP, NULL, hInstance, NULL);
		// NOTE: currently only one client;
		g_cond.notify_one();
	//	g_cond.notify_all();
	}
	::ShowWindow(hWindow, SW_SHOWNORMAL);
}


/*!
\since build 300 。
\todo 使用 g++ 4.7 之后版本直接创建线程。
*/
void
HostTask()
{
	YSL_ InitializeMainWindow();

	::MSG host_msg; //!< 本机消息类型。

	while(true)
	{
		if(::PeekMessage(&host_msg, NULL, 0, 0, PM_REMOVE))
			::DispatchMessage(&host_msg);
		else
		//	std::this_thread::yield();
			std::this_thread::sleep_for(host_sleep);
	}
}


} // unnamed namespace;
#endif


DSApplication::DSApplication()
	: pFontCache(), UIResponseLimit(0x40)
{
	YAssert(!YSL_ pApp, "Duplicate instance found.");

	//注册全局应用程序实例。
	YSL_ pApp = this;

	//全局初始化。
	InitializeEnviornment();
#if defined(YCL_MINGW32)
	//启动本机消息循环线程后完成应用程序实例其它部分的初始化（注意顺序）。
	pHostThread = new std::thread(HostTask);
#endif
	//检查程序是否被正确安装。
	CheckInstall();

	//初始化系统设备。
	try
	{
		pScreenUp = new DSScreen(MainScreenWidth, MainScreenHeight);
		pScreenDown = new DSScreen(MainScreenWidth, MainScreenHeight);
	}
	catch(...)
	{
		throw LoggedEvent("Screen initialization failed.");
	}
	//初始化系统字体资源。
	InitializeSystemFontCache();
#ifdef YCL_MINGW32
	pScreenDown->Offset.Y = MainScreenHeight;

	//等待宿主环境就绪。
	{
		std::unique_lock<std::mutex> lck(g_mutex);

		while(!hWindow)
			g_cond.wait(lck, []{return bool(hWindow);});
	}
#endif
}

DSApplication::~DSApplication()
{
#ifdef YCL_MINGW32
	YAssert(pHostThread, "Null pointer found.");

	pHostThread->detach();
	delete pHostThread;
#endif

	//等待并确保所有 Shell 被释放。
//	hShell = nullptr;

	//释放全局非静态资源。

	//清理消息队列（必要，保证所有Shell在Application前析构）。
	Queue.Clear();

	//当主 Shell 句柄为静态存储期对象时需要通过 reset 释放。

	//释放默认字体资源。
	ydelete(pFontCache);
	pFontCache = nullptr;

	//释放设备。
	delete pScreenUp,
	delete pScreenDown;
}

FontCache&
DSApplication::GetFontCache() const ythrow(LoggedEvent)
{
	if(YCL_UNLIKELY(!pFontCache))
		throw LoggedEvent("Null pointer found.");
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
		Idle();
	else
	{
		if(Queue.Peek(msg, hShell, true) == SM_QUIT)
			return false;
		if(msg.GetPriority() < UIResponseLimit)
			Idle();
		Dispatch(msg);
	}
	return true;
}

void
DSApplication::ResetFontCache(const_path_t path) ythrow(LoggedEvent)
{
	try
	{
		ydelete(pFontCache);
		pFontCache = ynew FontCache(path);
	}
	catch(...)
	{
		throw LoggedEvent("Error occured @ YApplication::ResetFontCache;");
	}
}


void
DispatchInput(Desktop& dsk)
{
#ifdef YCL_MINGW32
	if(hWindow != ::GetForegroundWindow())
		return;
#endif

	using namespace platform::KeyCodes;
	using namespace YSL_ Components;

	// NOTE: no real necessity to put input content into message queue,
	//	for the content is serialized in form of exactly one instance
	//	to be accepted at one time and no input signal is handled
	//	through interrupt to be buffered.
	static Drawing::Point cursor_pos;

	// FIXME: [DS] crashing after sleeping(default behavior of closing then
	// reopening lid) on real machine due to LibNDS default interrupt
	// handler for power management;
//	platform::AllowSleep(true);
	platform_ex::UpdateKeyStates();

	KeyInput keys(platform_ex::FetchKeyUpState());

#ifdef YCL_DS
#	define YCL_KEY_Touch KeyCodes::Touch
#	define YCL_CURSOR_VALID
	if(platform_ex::KeyState[YCL_KEY_Touch])
	{
		CursorInfo cursor;

		platform_ex::WriteCursor(cursor);
		yunseq(cursor_pos.X = cursor.GetX(),
			cursor_pos.Y = cursor.GetY());
	}
#elif defined(YCL_MINGW32)
#	define YCL_KEY_Touch VK_LBUTTON
#	define YCL_CURSOR_VALID if(cursor_pos != Point::Invalid)
	if(platform_ex::KeyState[VK_LBUTTON])
	{
		::POINT pt;

		::GetCursorPos(&pt);
		::ScreenToClient(hWindow, &pt);
		yunseq(cursor_pos.X = pt.x,
			cursor_pos.Y = pt.y - MainScreenHeight);
		if(!Rect(Point::Zero, MainScreenWidth, MainScreenHeight)
			.Contains(cursor_pos))
			cursor_pos = Point::Invalid;
	}
#else
#	error Unsupported platform found!
#endif

	auto& st(FetchGUIState());

	if(keys[YCL_KEY_Touch])
	{
		YCL_CURSOR_VALID
		{
			TouchEventArgs e(dsk, cursor_pos);

			st.ResponseTouch(e, TouchUp);
		}
	}
	else if(keys.any())
	{
		KeyEventArgs e(dsk, keys);

		st.ResponseKey(e, KeyUp);
	}
	keys = platform_ex::FetchKeyDownState();
	if(keys[YCL_KEY_Touch])
	{
		YCL_CURSOR_VALID
		{
			TouchEventArgs e(dsk, cursor_pos);

			st.ResponseTouch(e, TouchDown);
		}
	}
	else if(keys.any())
	{
		KeyEventArgs e(dsk, keys);

		st.ResponseKey(e, KeyDown);
	}
	if(platform_ex::KeyState[YCL_KEY_Touch])
	{
		YCL_CURSOR_VALID
		{
			TouchEventArgs e(dsk, cursor_pos);

			st.ResponseTouch(e, TouchHeld);
		}
	}
	else if(platform_ex::KeyState.any())
	{
		KeyEventArgs e(dsk, platform_ex::KeyState);

		st.ResponseKey(e, KeyHeld);
	}
#undef YCL_CURSOR_VALID
#undef YCL_KEY_Touch
}

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
InitConsole(Devices::Screen& scr, Drawing::PixelType fc, Drawing::PixelType bc)
{
#ifdef YCL_DS
	using namespace platform;

	if(&FetchGlobalInstance().GetScreenUp() == &scr)
		YConsoleInit(true, fc, bc);
	else if(&FetchGlobalInstance().GetScreenDown() == &scr)
		YConsoleInit(false, fc, bc);
	else
		return false;
#elif defined(YCL_MINGW32)
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

