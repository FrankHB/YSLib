/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Win32GUI.cpp
\ingroup YCLib
\ingroup MinGW32
\brief Win32 GUI 接口。
\version r334
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 11:31:05 +0800
\par 修改时间:
	2013-08-10 00:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::Win32GUI
*/


#include "YCLib/Win32GUI.h"

using namespace YSLib;
using namespace Drawing;

namespace platform_ex
{

namespace Windows
{

namespace
{

//! \since build 388
void
ResizeWindow(::HWND h_wnd, SDst w, SDst h)
{
	if(YB_UNLIKELY(!::SetWindowPos(h_wnd, {}, 0, 0, w, h,
		SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE
		| SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER)))
		YF_Raise_Win32Exception("SetWindowPos");
}

//! \since build 427
::RECT
FetchWindowRect(::HWND h_wnd)
{
	::RECT rect;

	if(YB_UNLIKELY(!::GetWindowRect(h_wnd, &rect)))
		YF_Raise_Win32Exception("GetWindowRect");
	return rect;
}

} // unnamed namespace;


Size
WindowReference::GetSize() const
{
	const auto& rect(FetchWindowRect(hWindow));

	YAssert(rect.right - rect.left >= 0 && rect.bottom - rect.top >= 0,
		"Invalid boundary found.");

	return {rect.right - rect.left, rect.bottom - rect.top};
}
YSLib::Drawing::AlphaType
WindowReference::GetOpacity() const
{
	ystdex::byte a;

	if(YB_UNLIKELY(!GetLayeredWindowAttributes(hWindow, {}, &a, {})))
		YF_Raise_Win32Exception("GetLayeredWindowAttributes");
	return a;
}
Point
WindowReference::GetLocation() const
{
	const auto& rect(FetchWindowRect(hWindow));

	return {rect.left, rect.top};
}

void
WindowReference::SetOpacity(YSLib::Drawing::AlphaType a)
{
	if(YB_UNLIKELY(!SetLayeredWindowAttributes(hWindow, 0, a, LWA_ALPHA)))
		YF_Raise_Win32Exception("SetLayeredWindowAttributes");
}
void
WindowReference::SetText(const wchar_t* str)
{
	if(YB_UNLIKELY(!::SetWindowTextW(hWindow, str)))
		YF_Raise_Win32Exception("SetWindowTextW");
}

void
WindowReference::Close()
{
	if(YB_UNLIKELY(!::SendNotifyMessageW(hWindow, WM_CLOSE, 0, 0)))
		YF_Raise_Win32Exception("SendNotifyMessageW");
}

void
WindowReference::Invalidate()
{
	if(YB_UNLIKELY(!::InvalidateRect(hWindow, {}, false)))
		YF_Raise_Win32Exception("InvalidateRect");
}

void
WindowReference::Move(const Point& pt)
{
	if(YB_UNLIKELY(!::SetWindowPos(hWindow, {}, pt.X, pt.Y, 0, 0,
		SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOREDRAW
		| SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOZORDER)))
		YF_Raise_Win32Exception("SetWindowPos");
}

void
WindowReference::Resize(const Size& s)
{
	ResizeWindow(hWindow, s.Width, s.Height);
}

void
WindowReference::ResizeClient(const Size& s)
{
	::RECT rect{0, 0, s.Width, s.Height};

	if(YB_UNLIKELY(!::AdjustWindowRect(&rect,
		::GetWindowLongW(hWindow, GWL_STYLE), false)))
		YF_Raise_Win32Exception("AdjustWindowRect");
	ResizeWindow(hWindow, rect.right - rect.left, rect.bottom - rect.top);
}

bool
WindowReference::Show() ynothrow
{
	return ::ShowWindowAsync(hWindow, SW_SHOWNORMAL) != 0;
}


NativeWindowHandle
CreateNativeWindow(const wchar_t* class_name, const Drawing::Size& s,
	const wchar_t* title, ::DWORD wstyle, ::DWORD wstyle_ex)
{
	::RECT rect{0, 0, s.Width, s.Height};

	::AdjustWindowRect(&rect, wstyle, false);
	return ::CreateWindowExW(wstyle_ex, class_name, title, wstyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left,
		rect.bottom - rect.top, HWND_DESKTOP, {}, ::GetModuleHandleW({}), {});
}


ScreenBuffer::ScreenBuffer(const Size& s)
	: size(s), hBitmap([this]{
		// NOTE: Bitmap format is hard coded here for explicit buffer
		//	compatibility. %::CreateCompatibleBitmap is not fit for unknown
		//	windows.
		::BITMAPINFO bmi{{sizeof(::BITMAPINFOHEADER), size.Width,
			-size.Height - 1, 1, 32, BI_RGB,
			sizeof(PixelType) * size.Width * size.Height, 0, 0, 0, 0}, {}};

		return ::CreateDIBSection({}, &bmi, DIB_RGB_COLORS,
			&reinterpret_cast<void*&>(pBuffer), {}, 0);
	}())
{}
ScreenBuffer::ScreenBuffer(ScreenBuffer&& sbuf) ynothrow
	: size(sbuf.size), hBitmap(sbuf.hBitmap)
{
	sbuf.hBitmap = {};
}
ScreenBuffer::~ScreenBuffer()
{
	::DeleteObject(hBitmap);
}

void
ScreenBuffer::Premultiply(BitmapPtr buf) ynothrow
{
	// NOTE: Since the pitch is guaranteed equal to the width, the storage for
	//	pixels can be supposed to be contiguous.
	std::transform(buf, buf + size.Width * size.Height, pBuffer,
		[](const PixelType& pixel){
			const auto a(pixel.rgbReserved);

			return PixelType{MonoType(pixel.rgbBlue * a / 0xFF),
				MonoType(pixel.rgbGreen * a / 0xFF),
				MonoType(pixel.rgbRed * a / 0xFF), a};
	});
}

void
ScreenBuffer::UpdateFrom(BitmapPtr buf) ynothrow
{
	// NOTE: Since the pitch is guaranteed equal to the width, the storage for
	//	pixels can be supposed to be contiguous.
	std::copy_n(buf, size.Width * size.Height, GetBufferPtr());
}


void
ScreenRegionBuffer::UpdateFrom(BitmapPtr buf) ynothrow
{
	std::lock_guard<std::mutex> lck(mtx);

	ScreenBuffer::UpdateFrom(buf);
}

void
ScreenRegionBuffer::UpdatePremultipliedTo(NativeWindowHandle h_wnd, AlphaType a,
	const Point& pt) ynothrow
{
	std::lock_guard<std::mutex> lck(mtx);
	GSurface<> sf(h_wnd);

	sf.UpdatePremultiplied(*this, h_wnd, a, pt);
}

void
ScreenRegionBuffer::UpdateTo(NativeWindowHandle h_wnd, const Point& pt) ynothrow
{
	std::lock_guard<std::mutex> lck(mtx);
	GSurface<> sf(h_wnd);

	sf.Update(*this, pt);
}


void
WindowMemorySurface::Update(ScreenBuffer& sbuf, const Point& pt) ynothrow
{
	const auto h_old(::SelectObject(h_mem_dc, sbuf.GetNativeHandle()));
	const auto& s(sbuf.GetSize());

	// NOTE: Unlocked intentionally for performance.
	::BitBlt(h_owner_dc, pt.X, pt.Y, s.Width, s.Height, h_mem_dc, 0, 0,
		SRCCOPY);
	::SelectObject(h_mem_dc, h_old);
}
void
WindowMemorySurface::UpdatePremultiplied(ScreenBuffer& sbuf,
	NativeWindowHandle h_wnd, YSLib::Drawing::AlphaType a, const Point& pt)
	ynothrow
{
	const auto h_old(::SelectObject(h_mem_dc, sbuf.GetNativeHandle()));
	auto rect(FetchWindowRect(h_wnd));
	::SIZE size{rect.right - rect.left, rect.bottom - rect.top};
	::POINT ptx{pt.X, pt.Y};
	::BLENDFUNCTION bfunc{AC_SRC_OVER, 0, a, AC_SRC_ALPHA};

	// NOTE: Unlocked intentionally for performance.
	if(YB_UNLIKELY(!::UpdateLayeredWindow(h_wnd, h_owner_dc,
		reinterpret_cast< ::POINT*>(&rect), &size, h_mem_dc, &ptx, 0, &bfunc,
		ULW_ALPHA)))
	{
		// TODO: Use RAII.
		::SelectObject(h_mem_dc, h_old);
		YF_Raise_Win32Exception("UpdateLayeredWindow");
	}
	::SelectObject(h_mem_dc, h_old);
}


WindowClass::WindowClass(const wchar_t* class_name, ::WNDPROC wnd_proc,
	::UINT style, ::HBRUSH h_bg)
	: h_instance(::GetModuleHandleW({}))
{
	// NOTE: Intentionally no %CS_OWNDC or %CS_CLASSDC, so %::ReleaseDC
	//	is always needed.
	const ::WNDCLASSW wnd_class{style, wnd_proc, 0, 0, h_instance,
		::LoadIconW({}, IDI_APPLICATION), ::LoadCursorW({}, IDC_ARROW),
		h_bg, nullptr, class_name};

	if(YB_UNLIKELY(::RegisterClassW(&wnd_class) == 0))
		YF_Raise_Win32Exception("RegisterClassW");
	// TODO: Trace class name.
	YTraceDe(Notice, "Window class registered.\n");
}
WindowClass::~WindowClass()
{
	::UnregisterClassW(WindowClassName, h_instance);
	// TODO: Trace class name.
	YTraceDe(Notice, "Window class unregistered.\n");
}


HostWindow::HostWindow(NativeWindowHandle h)
	: WindowReference(h)
{
	YAssert(::IsWindow(h), "Invalid window handle found.");
	YAssert(::GetWindowThreadProcessId(h, {}) == ::GetCurrentThreadId(),
		"Window not created on current thread found.");
	YAssert(::GetWindowLongPtrW(h, GWLP_USERDATA) == 0,
		"Invalid user data of window found.");

	wchar_t buf[ystdex::arrlen(WindowClassName)];

	if(YB_UNLIKELY(!::GetClassNameW(hWindow, buf,
		ystdex::arrlen(WindowClassName))))
		YF_Raise_Win32Exception("GetClassNameW");
	if(std::wcscmp(buf, WindowClassName) != 0)
		throw LoggedEvent("Wrong windows class name found.");
	::SetLastError(0);
	if(YB_UNLIKELY(::SetWindowLongPtrW(hWindow, GWLP_USERDATA,
		::LONG_PTR(this)) == 0 && GetLastError() != 0))
		YF_Raise_Win32Exception("SetWindowLongPtrW");
	if(YB_UNLIKELY(!::SetWindowPos(hWindow, {}, 0, 0, 0, 0, SWP_NOACTIVATE
		| SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_NOSENDCHANGING
		| SWP_NOSIZE | SWP_NOZORDER)))
		YF_Raise_Win32Exception("SetWindowPos");

	::RAWINPUTDEVICE rid{0x01, 0x02, 0, nullptr};

	if(YB_UNLIKELY(!::RegisterRawInputDevices(&rid, 1, sizeof(rid))))
		YF_Raise_Win32Exception("RegisterRawInputDevices");
}
HostWindow::~HostWindow()
{
	::SetWindowLongPtrW(hWindow, GWLP_USERDATA, ::LONG_PTR());
	// NOTE: The window could be already destroyed in window procedure.
	if(::IsWindow(hWindow))
		::DestroyWindow(hWindow);
}

void
HostWindow::OnDestroy()
{
	::PostQuitMessage(0);
}

void
HostWindow::OnLostFocus()
{}

void
HostWindow::OnPaint()
{}

} // namespace Windows;

} // namespace platform_ex;

