/*
	© 2013-2014 FrankHB.

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
\version r497
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 11:31:05 +0800
\par 修改时间:
	2014-07-12 17:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::Win32GUI
*/


#include "YCLib/YModules.h"
#include YFM_MinGW32_YCLib_Win32GUI
#include YFM_YSLib_Core_YCoreUtilities // for YSLib::CheckPositiveScalar;

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
		SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER
		| SWP_NOSENDCHANGING | SWP_NOZORDER)))
		YF_Raise_Win32Exception("SetWindowPos @ ResizeWindow");
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

//! \since build 445
//@{
Size
FetchSizeFromBounds(const ::RECT& rect)
{
	YAssert(rect.right - rect.left >= 0 && rect.bottom - rect.top >= 0,
		"Invalid boundary found.");
	return {rect.right - rect.left, rect.bottom - rect.top};
}

inline Rect
FetchRectFromBounds(const ::RECT& rect)
{
	return Rect(rect.left, rect.top, FetchSizeFromBounds(rect));
}

inline ::DWORD
FetchWindowStyle(::HWND h_wnd)
{
	return ::GetWindowLongW(h_wnd, GWL_STYLE);
}

void
AdjustWindowBounds(::RECT& rect, ::HWND h_wnd, bool b_menu = false)
{
	if(YB_UNLIKELY(!::AdjustWindowRect(&rect, FetchWindowStyle(h_wnd), b_menu)))
		YF_Raise_Win32Exception("AdjustWindowRect");
	YAssert(rect.right - rect.left >= 0 && rect.bottom - rect.top >= 0,
		"Invalid boundary found.");
}

void
SetWindowBounds(::HWND h_wnd, int x, int y, int cx, int cy)
{
	if(YB_UNLIKELY(!::SetWindowPos(h_wnd, {}, x, y, cx, cy,
		SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE
		| SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER)))
		YF_Raise_Win32Exception("SetWindowPos @ SetWindowBounds");
}
//@}

} // unnamed namespace;


void
BindDefaultWindowProc(NativeWindowHandle h_wnd, MessageMap& m, unsigned msg,
	EventPriority prior)
{
	m[msg].Add([=](::WPARAM w_param, ::LPARAM l_param){
		::DefWindowProcW(h_wnd, msg, w_param, l_param);
	}, prior);
}


Rect
WindowReference::GetClientBounds() const
{
	return {GetClientLocation(), GetClientSize()};
}
Point
WindowReference::GetClientLocation() const
{
	::POINT point{0, 0};

	if(YB_UNLIKELY(!::ClientToScreen(hWindow, &point)))
		YF_Raise_Win32Exception("ClientToScreen");
	return {point.x, point.y};
}
Size
WindowReference::GetClientSize() const
{
	::RECT rect;

	if(YB_UNLIKELY(!::GetClientRect(hWindow, &rect)))
		YF_Raise_Win32Exception("GetClientRect");
	return {rect.right, rect.bottom};
}
Point
WindowReference::GetCursorLocation() const
{
	::POINT cursor;

	::GetCursorPos(&cursor);
	::ScreenToClient(GetNativeHandle(), &cursor);
	return {cursor.x, cursor.y};
}
Point
WindowReference::GetLocation() const
{
	const auto& rect(FetchWindowRect(hWindow));

	return {rect.left, rect.top};
}
YSLib::Drawing::AlphaType
WindowReference::GetOpacity() const
{
	ystdex::byte a;

	if(YB_UNLIKELY(!GetLayeredWindowAttributes(hWindow, {}, &a, {})))
		YF_Raise_Win32Exception("GetLayeredWindowAttributes");
	return a;
}
Size
WindowReference::GetSize() const
{
	return FetchSizeFromBounds(FetchWindowRect(hWindow));
}

void
WindowReference::SetClientBounds(const Rect& r)
{
	::RECT rect{r.X, r.Y, CheckScalar<SPos>(r.X + r.Width, "width"),
		CheckScalar<SPos>(r.Y + r.Height, "height")};

	AdjustWindowBounds(rect, hWindow);
	SetWindowBounds(hWindow, rect.left, rect.top, rect.right - rect.left,
		rect.bottom - rect.top);
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
		YF_Raise_Win32Exception("SetWindowPos @ WindowReference::Move");
}

void
WindowReference::Resize(const Size& s)
{
	ResizeWindow(hWindow, s.Width, s.Height);
}

void
WindowReference::ResizeClient(const Size& s)
{
	::RECT rect{0, 0, CheckScalar<SPos>(s.Width, "width"),
		CheckScalar<SPos>(s.Height, "height")};

	AdjustWindowBounds(rect, hWindow);
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
	::RECT rect{0, 0, CheckScalar<SPos>(s.Width, "width"),
		CheckScalar<SPos>(s.Height, "height")};

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
		::BITMAPINFO bmi{{sizeof(::BITMAPINFOHEADER),
			CheckPositiveScalar<SPos>(size.Width, "width"), 
			-CheckPositiveScalar<SPos>(size.Height, "height") - 1, 1, 32,
			BI_RGB, sizeof(PixelType) * size.Width * size.Height, 0, 0, 0, 0},
			{}};

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

ScreenBuffer&
ScreenBuffer::operator=(ScreenBuffer&& sbuf)
{
	sbuf.swap(*this);
	return *this;
}

void
ScreenBuffer::Resize(const Size& s)
{
	if(s != size)
		*this = ScreenBuffer(s);
}

void
ScreenBuffer::Premultiply(BitmapPtr buf) ynothrow
{
	// NOTE: Since the stride is guaranteed equal to the width, the storage for
	//	pixels can be supposed to be contiguous.
	std::transform(buf, buf + size.Width * size.Height, pBuffer,
		[](const PixelType& pixel){
			const auto a(pixel.GetA());

			return PixelType{MonoType(pixel.GetB() * a / 0xFF),
				MonoType(pixel.GetG() * a / 0xFF),
				MonoType(pixel.GetR() * a / 0xFF), a};
	});
}

void
ScreenBuffer::UpdateFrom(BitmapPtr p_buf) ynothrow
{
	// NOTE: Since the pitch is guaranteed equal to the width, the storage for
	//	pixels can be supposed to be contiguous.
	YAssertNonnull(p_buf);
	std::copy_n(p_buf, size.Width * size.Height, GetBufferPtr());
}

void
ScreenBuffer::swap(ScreenBuffer& sbuf) ynothrow
{
	std::swap(size, sbuf.size),
	std::swap(pBuffer, sbuf.pBuffer),
	std::swap(hBitmap, sbuf.hBitmap);
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
		reinterpret_cast<::POINT*>(&rect), &size, h_mem_dc, &ptx, 0, &bfunc,
		ULW_ALPHA)))
	{
		// TODO: Use RAII.
		::SelectObject(h_mem_dc, h_old);
		YF_Raise_Win32Exception("UpdateLayeredWindow");
	}
	::SelectObject(h_mem_dc, h_old);
}


WindowClass::WindowClass(const wchar_t* class_name, ::WNDPROC wnd_proc,
	unsigned style, ::HBRUSH h_bg)
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
	YTraceDe(Notice, "Window class registered.");
}
WindowClass::~WindowClass()
{
	::UnregisterClassW(WindowClassName, h_instance);
	// TODO: Trace class name.
	YTraceDe(Notice, "Window class unregistered.");
}


HostWindow::HostWindow(NativeWindowHandle h)
	: WindowReference(h),
	MessageMap()
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
		YF_Raise_Win32Exception("SetWindowPos @ HostWindow::HostWindow");

	::RAWINPUTDEVICE rid{0x01, 0x02, 0, nullptr};

	if(YB_UNLIKELY(!::RegisterRawInputDevices(&rid, 1, sizeof(rid))))
		YF_Raise_Win32Exception("RegisterRawInputDevices");
	MessageMap[WM_DESTROY] += []{
		::PostQuitMessage(0);
	};
}
HostWindow::~HostWindow()
{
	::SetWindowLongPtrW(hWindow, GWLP_USERDATA, ::LONG_PTR());
	// NOTE: The window could be already destroyed in window procedure.
	if(::IsWindow(hWindow))
		::DestroyWindow(hWindow);
}

Point
HostWindow::MapPoint(const Point& pt) const
{
	return pt;
}

} // namespace Windows;

} // namespace platform_ex;

