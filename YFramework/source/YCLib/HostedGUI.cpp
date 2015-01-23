/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostedGUI.cpp
\ingroup YCLib
\ingroup YCLibLimitedPlatforms
\brief 宿主 GUI 接口。
\version r975
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 11:31:05 +0800
\par 修改时间:
	2015-01-23 20:44 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::HostedGUI
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_HostedGUI
#include YFM_YSLib_Core_YCoreUtilities // for YSLib::CheckPositiveScalar;
#if YCL_Win32
#	include YFM_MinGW32_YCLib_MinGW32
#	include <ystdex/exception.h> // for ystdex::unimplemented;
#	if SW_SHOWNORMAL != 1 || WS_POPUP != 0x80000000L || WS_EX_LTRREADING != 0L
#		error "Wrong macro defined."
#	endif
#elif YCL_Android
#	include YFM_Android_YCLib_Android
#	include <android/native_window.h>
#endif
#if YCL_HostedUI_XCB || YCL_Android
#	include "YSLib/Service/YModules.h"
#	include YFM_YSLib_Service_YGDI
#endif

using namespace YSLib;
using namespace Drawing;

#if YF_Hosted

namespace platform_ex
{

namespace
{

#	if YCL_HostedUI_XCB || YCL_Android
//! \since build 498
SDst
CheckStride(SDst buf_stride, SDst w)
{
	if(buf_stride < w)
		// XXX: Use more specified exception type.
		throw std::runtime_error("Stride is small than width.");
	return buf_stride;
}
#	elif YCL_Win32
#		define YCL_Impl_CallWin32(_fn, _msg, ...) \
	if(YB_UNLIKELY(!::_fn(__VA_ARGS__))) \
		YF_Raise_Win32Exception(#_fn " @ " _msg)

//! \since build 388
void
ResizeWindow(::HWND h_wnd, SDst w, SDst h)
{
	YCL_Impl_CallWin32(SetWindowPos, "ResizeWindow", h_wnd, {},
		0, 0, w, h, SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOMOVE
		| SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER);
}

//! \since build 427
::RECT
FetchWindowRect(::HWND h_wnd)
{
	::RECT rect;

	YCL_Impl_CallWin32(GetWindowRect, "FetchWindowRect", h_wnd, &rect);
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

//! \since build 564
inline unsigned long
FetchWindowStyle(::HWND h_wnd)
{
	return ::GetWindowLongW(h_wnd, GWL_STYLE);
}

void
AdjustWindowBounds(::RECT& rect, ::HWND h_wnd, bool b_menu = false)
{
	YCL_Impl_CallWin32(AdjustWindowRect, "AdjustWindowBounds", &rect,
		FetchWindowStyle(h_wnd), b_menu);
	YAssert(rect.right - rect.left >= 0 && rect.bottom - rect.top >= 0,
		"Invalid boundary found.");
}

void
SetWindowBounds(::HWND h_wnd, int x, int y, int cx, int cy)
{
	YCL_Impl_CallWin32(SetWindowPos, "SetWindowBounds", h_wnd, {}, x, y, cx,
		cy, SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOOWNERZORDER
		| SWP_NOSENDCHANGING | SWP_NOZORDER);
}
//@}
#	endif

} // unnamed namespace;


#	if YCL_Win32
void
BindDefaultWindowProc(NativeWindowHandle h_wnd, MessageMap& m, unsigned msg,
	EventPriority prior)
{
	m[msg].Add([=](::WPARAM w_param, ::LPARAM l_param){
		::DefWindowProcW(h_wnd, msg, w_param, l_param);
	}, prior);
}
#	endif


#	if YCL_HostedUI_XCB
XCB::WindowData&
WindowReference::Deref() const
{
	if(const auto h = GetNativeHandle().get())
		return *h;
	throw std::runtime_error("Null window reference found.");
}
#	elif YCL_Win32
bool
WindowReference::IsVisible() const ynothrow
{
	return ::IsWindowVisible(GetNativeHandle());
}

Rect
WindowReference::GetBounds() const
{
	const auto& rect(FetchWindowRect(GetNativeHandle()));

	return {rect.left, rect.top, FetchSizeFromBounds(rect)};
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

	YCL_Impl_CallWin32(ClientToScreen, "WindowReference::GetClientLocation",
		GetNativeHandle(), &point);
	return {point.x, point.y};
}
Size
WindowReference::GetClientSize() const
{
	::RECT rect;

	YCL_Impl_CallWin32(GetClientRect, "WindowReference::GetClientSize",
		GetNativeHandle(), &rect);
	return {rect.right, rect.bottom};
}
Point
WindowReference::GetCursorLocation() const
{
	::POINT cursor;

	YCL_Impl_CallWin32(GetCursorPos, "WindowReference::GetCursorLocation",
		&cursor);
	YCL_Impl_CallWin32(ScreenToClient, "WindowReference::GetCursorLocation",
		GetNativeHandle(), &cursor);
	return {cursor.x, cursor.y};
}
Point
WindowReference::GetLocation() const
{
	const auto& rect(FetchWindowRect(GetNativeHandle()));

	return {rect.left, rect.top};
}
YSLib::Drawing::AlphaType
WindowReference::GetOpacity() const
{
	ystdex::byte a;

	YCL_Impl_CallWin32(GetLayeredWindowAttributes,
		"WindowReference::GetOpacity", GetNativeHandle(), {}, &a, {});
	return a;
}
Size
WindowReference::GetSize() const
{
	return FetchSizeFromBounds(FetchWindowRect(GetNativeHandle()));
}

void
WindowReference::SetClientBounds(const Rect& r)
{
	::RECT rect{r.X, r.Y, CheckScalar<SPos>(r.X + r.Width, "width"),
		CheckScalar<SPos>(r.Y + r.Height, "height")};
	const auto h_wnd(GetNativeHandle());

	AdjustWindowBounds(rect, h_wnd);
	SetWindowBounds(h_wnd, rect.left, rect.top, rect.right - rect.left,
		rect.bottom - rect.top);
}
void
WindowReference::SetOpacity(YSLib::Drawing::AlphaType a)
{
	YCL_Impl_CallWin32(SetLayeredWindowAttributes,
		"WindowReference::SetOpacity", GetNativeHandle(), 0, a, LWA_ALPHA);
}
WindowReference
WindowReference::GetParent() const
{
	// TODO: Implementation.
	throw ystdex::unimplemented();
}

void
WindowReference::SetText(const wchar_t* str)
{
	YCL_Impl_CallWin32(SetWindowTextW, "WindowReference::SetText",
		GetNativeHandle(), str);
}

void
WindowReference::Close()
{
	YCL_Impl_CallWin32(SendNotifyMessageW, "WindowReference::Close",
		GetNativeHandle(), WM_CLOSE, 0, 0);
}

void
WindowReference::Invalidate()
{
	YCL_Impl_CallWin32(InvalidateRect, "WindowReference::Invalidate",
		GetNativeHandle(), {}, {});
}

void
WindowReference::Move(const Point& pt)
{
	YCL_Impl_CallWin32(SetWindowPos, "WindowReference::Move",
		GetNativeHandle(), {}, pt.X, pt.Y, 0, 0,
		SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOREDRAW
		| SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOZORDER);
}

void
WindowReference::Resize(const Size& s)
{
	ResizeWindow(GetNativeHandle(), s.Width, s.Height);
}

void
WindowReference::ResizeClient(const Size& s)
{
	::RECT rect{0, 0, CheckScalar<SPos>(s.Width, "width"),
		CheckScalar<SPos>(s.Height, "height")};

	AdjustWindowBounds(rect, GetNativeHandle());
	ResizeWindow(GetNativeHandle(), rect.right - rect.left,
		rect.bottom - rect.top);
}

bool
WindowReference::Show(int n_cmd_show) ynothrow
{
	return ::ShowWindowAsync(GetNativeHandle(), n_cmd_show) != 0;
}
#	elif YCL_Android
SDst
WindowReference::GetWidth() const
{
	return CheckPositiveScalar<SDst>(
		::ANativeWindow_getWidth(GetNativeHandle()), "width");
}
SDst
WindowReference::GetHeight() const
{
	return CheckPositiveScalar<SDst>(::ANativeWindow_getHeight(
		GetNativeHandle()), "height");
}
#	endif


#	if YCL_HostedUI_XCB
void
UpdateContentTo(NativeWindowHandle h_wnd, const Rect& r, const ConstGraphics& g)
{
	XCB::UpdatePixmapBuffer(Deref(h_wnd.get()), r, g);
}
#	elif YCL_Win32
NativeWindowHandle
CreateNativeWindow(const wchar_t* class_name, const Drawing::Size& s,
	const wchar_t* title, unsigned long wstyle, unsigned long wstyle_ex)
{
	::RECT rect{0, 0, CheckScalar<SPos>(s.Width, "width"),
		CheckScalar<SPos>(s.Height, "height")};

	::AdjustWindowRect(&rect, wstyle, false);
	return ::CreateWindowExW(wstyle_ex, class_name, title, wstyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom
		- rect.top, {}/*HWND_DESKTOP*/, {}, ::GetModuleHandleW({}), {});
}
#	elif YCL_Android
void
UpdateContentTo(NativeWindowHandle h_wnd, const Rect& r, const ConstGraphics& g)
{
	::ANativeWindow_Buffer abuf;
	::ARect arect{r.X, r.Y, r.X + r.Width, r.Y + r.Height};

	::ANativeWindow_lock(Nonnull(h_wnd), &abuf, &arect);
	CopyTo(static_cast<BitmapPtr>(abuf.bits), g,
		WindowReference(h_wnd).GetSize(), r.GetPoint(), {}, r.GetSize());
	::ANativeWindow_unlockAndPost(h_wnd);
}
#	endif


#	if YCL_HostedUI_XCB || YCL_Android
class ScreenBufferData : public CompactPixmap
{
public:
	ScreenBufferData(const Size&, SDst);

	DefDeMoveCtor(ScreenBufferData)
};

ScreenBufferData::ScreenBufferData(const Size& s, SDst buf_stride)
	: CompactPixmap({}, CheckStride(buf_stride, s.Width), s.Height)
{}
#	endif


#	if YCL_HostedUI_XCB || YCL_Android
ScreenBuffer::ScreenBuffer(const Size& s)
	: ScreenBuffer(s, s.Width)
{}
ScreenBuffer::ScreenBuffer(const Size& s, SDst buf_stride)
	: p_impl(new ScreenBufferData(s, buf_stride)), width(s.Width)
{}
ScreenBuffer::ScreenBuffer(ScreenBuffer&& sbuf) ynothrow
	: p_impl(new ScreenBufferData(std::move(*sbuf.p_impl))), width(sbuf.width)
{
	sbuf.width = 0;
}
#	elif YCL_Win32
ScreenBuffer::ScreenBuffer(const Size& s)
	: size(s), hBitmap([this]{
		// NOTE: Bitmap format is hard coded here for explicit buffer
		//	compatibility. %::CreateCompatibleBitmap is not fit for unknown
		//	windows.
		::BITMAPINFO bmi{{sizeof(::BITMAPINFOHEADER), CheckPositiveScalar<SPos>(
			size.Width, "width"), -CheckPositiveScalar<SPos>(size.Height,
			"height") - 1, 1, 32, BI_RGB, static_cast<unsigned long>(
			sizeof(Pixel) * size.Width * size.Height), 0, 0, 0, 0}, {}};

		return ::CreateDIBSection({}, &bmi, DIB_RGB_COLORS,
			&reinterpret_cast<void*&>(pBuffer), {}, 0);
	}())
{}
ScreenBuffer::ScreenBuffer(ScreenBuffer&& sbuf) ynothrow
	: size(sbuf.size), hBitmap(sbuf.hBitmap)
{
	sbuf.hBitmap = {};
}
#	endif
ScreenBuffer::~ScreenBuffer()
{
#	if YCL_Win32
	::DeleteObject(hBitmap);
#	endif
}

#	if YCL_HostedUI_XCB || YCL_Android
BitmapPtr
ScreenBuffer::GetBufferPtr() const ynothrow
{
	return Deref(p_impl).GetBufferPtr();
}
YSLib::Drawing::Graphics
ScreenBuffer::GetContext() const ynothrow
{
	return Deref(p_impl).GetContext();
}
Size
ScreenBuffer::GetSize() const ynothrow
{
	return {width, Deref(p_impl).GetHeight()};
}
YSLib::SDst
ScreenBuffer::GetStride() const ynothrow
{
	return Deref(p_impl).GetWidth();
}

void
ScreenBuffer::Resize(const Size& s)
{
	// TODO: Expand stride for given width using a proper strategy.
	Deref(p_impl).SetSize(s);
	width = s.Width;
}
#	elif YCL_Win32
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
ScreenBuffer::Premultiply(ConstBitmapPtr p_buf) ynothrow
{
	// NOTE: Since the stride is guaranteed equal to the width, the storage for
	//	pixels can be supposed to be contiguous.
	std::transform(p_buf, p_buf + size.Width * size.Height, pBuffer,
		[](const Pixel& pixel){
			const auto a(pixel.GetA());

			return Pixel{MonoType(pixel.GetB() * a / 0xFF),
				MonoType(pixel.GetG() * a / 0xFF),
				MonoType(pixel.GetR() * a / 0xFF), a};
	});
}
#	endif

void
ScreenBuffer::UpdateFrom(ConstBitmapPtr p_buf) ynothrow
{
#	if YCL_HostedUI_XCB || YCL_Android
	// TODO: Expand stride for given width using a proper strategy.
	std::copy_n(Nonnull(p_buf), GetAreaOf(GetSize()),
		Deref(p_impl).GetBufferPtr());
#	elif YCL_Win32
	// NOTE: Since the pitch is guaranteed equal to the width, the storage for
	//	pixels can be supposed to be contiguous.
	std::copy_n(Nonnull(p_buf), size.Width * size.Height, GetBufferPtr());
#	endif
}

void
ScreenBuffer::swap(ScreenBuffer& sbuf) ynothrow
{
#	if YCL_HostedUI_XCB || YCL_Android
	Deref(p_impl).swap(Deref(sbuf.p_impl)),
	std::swap(width, sbuf.width);
#	elif YCL_Win32
	std::swap(size, sbuf.size),
	std::swap(pBuffer, sbuf.pBuffer),
	std::swap(hBitmap, sbuf.hBitmap);
#	endif
}


void
ScreenRegionBuffer::UpdateFrom(ConstBitmapPtr p_buf) ynothrow
{
	lock_guard<mutex> lck(mtx);

	ScreenBuffer::UpdateFrom(p_buf);
}

#	if YCL_HostedUI_XCB || YCL_Android
ScreenRegionBuffer::ScreenRegionBuffer(const Size& s)
	: ScreenRegionBuffer(s, s.Width)
{}
ScreenRegionBuffer::ScreenRegionBuffer(const Size& s, SDst buf_stride)
	: ScreenBuffer(s, buf_stride)
{}
#	elif YCL_Win32
void
ScreenRegionBuffer::UpdatePremultipliedTo(NativeWindowHandle h_wnd, AlphaType a,
	const Point& pt)
{
	lock_guard<mutex> lck(mtx);
	GSurface<> sf(h_wnd);

	sf.UpdatePremultiplied(*this, h_wnd, a, pt);
}
#	endif

void
ScreenRegionBuffer::UpdateTo(NativeWindowHandle h_wnd, const Point& pt) ynothrow
{
	lock_guard<mutex> lck(mtx);
#	if YCL_HostedUI_XCB || YCL_Android
	UpdateContentTo(h_wnd, {pt, GetSize()}, GetContext());
#	elif YCL_Win32
	GSurface<> sf(h_wnd);

	sf.Update(*this, pt);
#	endif
}


#	if YCL_Win32
WindowMemorySurface::WindowMemorySurface(::HDC h_dc)
	: h_owner_dc(h_dc), h_mem_dc(::CreateCompatibleDC(h_dc))
{}
WindowMemorySurface::~WindowMemorySurface()
{
	::DeleteDC(h_mem_dc);
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


WindowDeviceContext::WindowDeviceContext(NativeWindowHandle h_wnd)
	: WindowDeviceContextBase(h_wnd, ::GetDC(h_wnd))
{}
WindowDeviceContext::~WindowDeviceContext()
{
	::ReleaseDC(hWindow, hDC);
}


WindowRegionDeviceContext::WindowRegionDeviceContext(NativeWindowHandle h_wnd)
	: WindowDeviceContextBase(h_wnd,
	::BeginPaint(h_wnd, reinterpret_cast<::PAINTSTRUCT*>(ps)))
{}
WindowRegionDeviceContext::~WindowRegionDeviceContext()
{
	static_assert(ystdex::is_aligned_storable<decltype(ps),
		::PAINTSTRUCT>::value, "Invalid type found.");

	::EndPaint(hWindow, reinterpret_cast<::PAINTSTRUCT*>(ps));
}


WindowClass::WindowClass(const wchar_t* class_name, ::WNDPROC wnd_proc,
	unsigned style, ::HBRUSH h_bg, ::HINSTANCE h_inst)
	// NOTE: Intentionally no %CS_OWNDC or %CS_CLASSDC, so %::ReleaseDC
	//	is always needed.
	: WindowClass(::WNDCLASSW{style, wnd_proc, 0, 0, h_inst ? h_inst
		: ::GetModuleHandleW({}), ::LoadIconW({}, IDI_APPLICATION),
		::LoadCursorW({}, IDC_ARROW), h_bg, nullptr, Nonnull(class_name)})
{}
WindowClass::WindowClass(const ::WNDCLASSW& wc)
	: WindowClass(wc.lpszClassName, [&]{
		const auto a(::RegisterClassW(&wc));

		if(YB_UNLIKELY(a == 0))
			YF_Raise_Win32Exception("RegisterClassW");
		return a;
	}(), wc.hInstance)
{}
WindowClass::WindowClass(const ::WNDCLASSEXW& wc)
	: WindowClass(wc.lpszClassName, [&]{
		const auto a(::RegisterClassExW(&wc));

		if(YB_UNLIKELY(a == 0))
			YF_Raise_Win32Exception("RegisterClassExW");
		return a;
	}(), wc.hInstance)
{}
WindowClass::WindowClass(const std::wstring& class_name,
	unsigned short class_atom, ::HINSTANCE h_inst)
	: name(class_name), atom(class_atom), h_instance(h_inst)
{
	if(YB_UNLIKELY(atom == 0))
		throw std::invalid_argument("Invalid atom value found.");
	YTraceDe(Notice, "Window class '%s' of atom '%hu' registered.",
		name.empty() ? "<unknown>" : WCSToUTF8(name).c_str(), atom);
}
WindowClass::~WindowClass()
{
	::UnregisterClassW(reinterpret_cast<const wchar_t*>(atom), h_instance);
	TryExpr(YTraceDe(Notice, "Window class '%s' of atom '%hu' unregistered.",
		name.empty() ? "<unknown>" : WCSToUTF8(name).c_str(), atom))
	CatchExpr(std::exception& e,
		YTraceDe(Alert, "Caught std::exception[%s]: %s.", typeid(e).name(),
		e.what()), yunused(e))
	CatchExpr(..., YTraceDe(Alert,
		"Unknown exception found @ WindowClass::~WindowClass."))
}
#	endif


HostWindow::HostWindow(NativeWindowHandle h)
	: WindowReference(h)
#	if YCL_HostedUI_XCB
	, WM_PROTOCOLS(platform::Deref(h.get()).LookupAtom("WM_PROTOCOLS"))
	, WM_DELETE_WINDOW(h.get()->LookupAtom("WM_DELETE_WINDOW"))
#	endif
#	if YCL_HostedUI_XCB || YCL_Win32
	, MessageMap()
#	endif
{
#	if YCL_HostedUI_XCB
	h.get()->GetConnectionRef().Check();
#	elif YCL_Win32
	YAssert(::IsWindow(h), "Invalid window handle found.");
	YAssert(::GetWindowThreadProcessId(h, {}) == ::GetCurrentThreadId(),
		"Window not created on current thread found.");
	YAssert(::GetWindowLongPtrW(h, GWLP_USERDATA) == 0,
		"Invalid user data of window found.");

	wchar_t buf[ystdex::arrlen(WindowClassName)];

	YCL_Impl_CallWin32(GetClassNameW, "HostWindow::HostWindow",
		GetNativeHandle(), buf, ystdex::arrlen(WindowClassName));
	if(std::wcscmp(buf, WindowClassName) != 0)
		throw GeneralEvent("Wrong windows class name found.");
	::SetLastError(0);
	if(YB_UNLIKELY(::SetWindowLongPtrW(GetNativeHandle(), GWLP_USERDATA,
		::LONG_PTR(this)) == 0 && GetLastError() != 0))
		YF_Raise_Win32Exception("SetWindowLongPtrW");
	YCL_Impl_CallWin32(SetWindowPos, "HostWindow::HostWindow",
		GetNativeHandle(), {}, 0, 0, 0, 0,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREDRAW
		| SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOZORDER);

	::RAWINPUTDEVICE rid{0x01, 0x02, 0, nullptr};

	YCL_Impl_CallWin32(RegisterRawInputDevices, "HostWindow::HostWindow",
		&rid, 1, sizeof(rid));
	MessageMap[WM_DESTROY] += []{
		::PostQuitMessage(0);
	};
#	elif YCL_Android
	::ANativeWindow_acquire(GetNativeHandle());
#	endif
}

HostWindow::~HostWindow()
{
#	if YCL_HostedUI_XCB
	delete GetNativeHandle().get();
#	elif YCL_Win32
	const auto h_wnd(GetNativeHandle());

	::SetWindowLongPtrW(h_wnd, GWLP_USERDATA, ::LONG_PTR());
	// NOTE: The window could be already destroyed in window procedure.
	if(::IsWindow(h_wnd))
		::DestroyWindow(h_wnd);
#	elif YCL_Android
	::ANativeWindow_release(GetNativeHandle());
#	endif
}

#	if YCL_Win32
Point
HostWindow::MapPoint(const Point& pt) const
{
	return pt;
}
#		undef YCL_Impl_CallWin32
#	endif

} // namespace platform_ex;

#endif

