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
\version r1485
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 11:31:05 +0800
\par 修改时间:
	2015-09-07 12:48 +0800
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
#	include "YSLib/Service/YModules.h"
#	include YFM_YSLib_Service_YBlit
#	include <Shellapi.h> // for ::ShellExecuteW;
#	include <ystdex/mixin.hpp> // for ystdex::wrap_mixin_t;
#elif YCL_Android
#	include YFM_Android_YCLib_Android
#	include <android/native_window.h>
#	include "YSLib/Service/YModules.h"
#	include YFM_YSLib_Service_YBlit
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

#	if YCL_Win32
void
GDIObjectDelete::operator()(pointer h) const ynothrow
{
	static_assert(std::is_same<pointer, ::HGDIOBJ>::value,
		"Mismatched type found.");

	YCL_CallWin32_Trace(DeleteObject, "GDIObjectDelete::operator()", h);
}
#	endif

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
//! \since build 570
void
MoveWindow(::HWND h_wnd, SPos x, SPos y)
{
	YCL_CallWin32(SetWindowPos, "MoveWindow", h_wnd, {},
		x, y, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOOWNERZORDER
		| SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOZORDER);
}

//! \since build 388
void
ResizeWindow(::HWND h_wnd, SDst w, SDst h)
{
	YCL_CallWin32(SetWindowPos, "ResizeWindow", h_wnd, {},
		0, 0, int(w), int(h), SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOMOVE
		| SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER);
}

//! \since build 427
::RECT
FetchWindowRect(::HWND h_wnd)
{
	::RECT rect;

	YCL_CallWin32(GetWindowRect, "FetchWindowRect", h_wnd, &rect);
	return rect;
}

//! \since build 445
//@{
Size
FetchSizeFromBounds(const ::RECT& rect)
{
	return {CheckScalar<SDst>(rect.right - rect.left, "width"),
		CheckScalar<SDst>(rect.bottom - rect.top, "height")};
}

//! \since build 591
Rect
FetchRectFromBounds(const ::RECT& rect)
{
	return {rect.left, rect.top, FetchSizeFromBounds(rect)};
}

//! \since build 564
inline unsigned long
FetchWindowStyle(::HWND h_wnd)
{
	return static_cast<unsigned long>(::GetWindowLongW(h_wnd, GWL_STYLE));
}

void
AdjustWindowBounds(::RECT& rect, ::HWND h_wnd, bool b_menu = false)
{
	YCL_CallWin32(AdjustWindowRect, "AdjustWindowBounds", &rect,
		FetchWindowStyle(h_wnd), b_menu);
	YAssert(rect.right - rect.left >= 0 && rect.bottom - rect.top >= 0,
		"Invalid boundary found.");
}

//! \since build 587
void
SetWindowBounds(::HWND h_wnd, int x, int y, SDst w, SDst h)
{
	YCL_CallWin32(SetWindowPos, "SetWindowBounds", h_wnd, {}, x, y, int(w),
		int(h), SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOOWNERZORDER
		| SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER);
}
//@}


//! \since build 593
inline unique_ptr<void, GlobalDelete>
MakeMoveableGlobalMemory(size_t size)
{
	auto p(unique_raw(::GlobalAlloc(GMEM_MOVEABLE, size), GlobalDelete()));

	if(YB_UNLIKELY(!p))
		// TODO: Use inherited class of exception.
		throw std::bad_alloc();
	return p;
}

//! \since build 593
template<typename _tChar, class _tString>
YB_ALLOCATOR YB_ATTR(returns_nonnull) void*
CopyGlobalString(const _tString& str)
{
	const auto len(str.length());
	auto p(MakeMoveableGlobalMemory((len + 1) * sizeof(_tChar)));
	{
		const GlobalLocked gl(Nonnull(p));
		const auto p_buf(gl.GetPtr<_tChar>());

		ystdex::ntctscpy(p_buf, str.data(), len);
	}
	return p.release();
}
#	endif

} // unnamed namespace;


#	if YCL_Win32
void
BindDefaultWindowProc(NativeWindowHandle h_wnd, MessageMap& m, unsigned msg,
	EventPriority prior)
{
	m[msg].Add(
		[=](::WPARAM w_param, ::LPARAM l_param, ::LRESULT& res) ynothrow{
		res = ::DefWindowProcW(h_wnd, msg, w_param, l_param);
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
WindowReference::IsMaximized() const ynothrow
{
	return ::IsZoomed(GetNativeHandle());
}
bool
WindowReference::IsMinimized() const ynothrow
{
	return ::IsIconic(GetNativeHandle());
}
bool
WindowReference::IsVisible() const ynothrow
{
	return ::IsWindowVisible(GetNativeHandle());
}

Rect
WindowReference::GetBounds() const
{
	return FetchRectFromBounds(FetchWindowRect(GetNativeHandle()));
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

	YCL_CallWin32(ClientToScreen, "WindowReference::GetClientLocation",
		GetNativeHandle(), &point);
	return {point.x, point.y};
}
Size
WindowReference::GetClientSize() const
{
	::RECT rect;

	YCL_CallWin32(GetClientRect, "WindowReference::GetClientSize",
		GetNativeHandle(), &rect);
	return {rect.right, rect.bottom};
}
Point
WindowReference::GetCursorLocation() const
{
	::POINT cursor;

	YCL_CallWin32(GetCursorPos, "WindowReference::GetCursorLocation",
		&cursor);
	YCL_CallWin32(ScreenToClient, "WindowReference::GetCursorLocation",
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
	byte a;

	YCL_CallWin32(GetLayeredWindowAttributes,
		"WindowReference::GetOpacity", GetNativeHandle(), {}, &a, {});
	return a;
}
Size
WindowReference::GetSize() const
{
	return FetchSizeFromBounds(FetchWindowRect(GetNativeHandle()));
}

void
WindowReference::SetBounds(const Rect& r)
{
	SetWindowBounds(GetNativeHandle(), r.X, r.Y, r.Width, r.Height);
}
void
WindowReference::SetClientBounds(const Rect& r)
{
	::RECT rect{r.X, r.Y, CheckScalar<SPos>(r.GetRight(), "width"),
		CheckScalar<SPos>(r.GetBottom(), "height")};
	const auto h_wnd(GetNativeHandle());

	AdjustWindowBounds(rect, h_wnd);
	SetWindowBounds(h_wnd, rect.left, rect.top, SDst(rect.right - rect.left),
		SDst(rect.bottom - rect.top));
}
void
WindowReference::SetOpacity(YSLib::Drawing::AlphaType a)
{
	YCL_CallWin32(SetLayeredWindowAttributes,
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
	YCL_CallWin32(SetWindowTextW, "WindowReference::SetText",
		GetNativeHandle(), str);
}

void
WindowReference::Close()
{
	YCL_CallWin32(SendNotifyMessageW, "WindowReference::Close",
		GetNativeHandle(), WM_CLOSE, 0, 0);
}

void
WindowReference::Invalidate()
{
	YCL_CallWin32(InvalidateRect, "WindowReference::Invalidate",
		GetNativeHandle(), {}, {});
}

void
WindowReference::Move(const Point& pt)
{
	MoveWindow(GetNativeHandle(), pt.X, pt.Y);
}

void
WindowReference::MoveClient(const Point& pt)
{
	::RECT rect{pt.X, pt.Y, pt.X, pt.Y};
	const auto h_wnd(GetNativeHandle());

	AdjustWindowBounds(rect, h_wnd);
	MoveWindow(h_wnd, rect.left, rect. top);
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
	const auto h_wnd(GetNativeHandle());

	AdjustWindowBounds(rect, h_wnd);
	ResizeWindow(h_wnd, SDst(rect.right - rect.left),
		SDst(rect.bottom - rect.top));
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
YF_API ::HBITMAP
CreateCompatibleDIBSection(const YSLib::Drawing::Size& s, BitmapPtr& p_buffer)
{
	// NOTE: There is no resolution information created. See https://msdn.microsoft.com/en-us/library/dd183494.aspx .
	// NOTE: It would return %ERROR_INVALID_PARAMETER for many calls if
	//	allocated memory is not on 32-bit boundary. Anyway it is not a matter
	//	here because the pixel is at least 32-bit. See
	//	http://msdn2.microsoft.com/en-us/library/ms532292.aspx and https://msdn.microsoft.com/en-us/library/dd183494.aspx .
	// NOTE: Bitmap format is hard coded here for explicit buffer
	//	compatibility. %::CreateCompatibleBitmap is not fit for unknown
	//	windows.
	::BITMAPINFO bmi{{sizeof(::BITMAPINFOHEADER), CheckPositiveScalar<long>(
		s.Width, "width"), -CheckPositiveScalar<long>(s.Height,
		"height") - 1, 1, 32, BI_RGB, static_cast<unsigned long>(
		sizeof(Pixel) * s.Width * s.Height), 0, 0, 0, 0}, {}};
	void* p_buf{};
	const auto h(YCL_CallWin32(CreateDIBSection, "ScreenBuffer::ScreenBuffer",
		{}, &bmi, DIB_RGB_COLORS, &p_buf, {}, 0));

	if(YB_LIKELY(p_buf))
		p_buffer = static_cast<BitmapPtr>(p_buf);
	else
		// XXX: This should not occur unless there are bugs in implementation of
		//	%::CreateCompatibleDIBSection.
		throw std::runtime_error("Failed writing pointer"
			" @ CreateCompatibleDIBSection.");
	return h;
}

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
	const auto res(::ANativeWindow_lock(Nonnull(h_wnd), &abuf, &arect));

	if(YB_LIKELY(res == 0))
	{
		BlitLines<false, false>(CopyLine<true>(), BitmapPtr(abuf.bits),
			g.GetBufferPtr(), WindowReference(h_wnd).GetSize(), g.GetSize(),
			r.GetPoint(), {}, r.GetSize());
		::ANativeWindow_unlockAndPost(h_wnd);
	}
	else
		YTraceDe(Warning, "::ANativeWindow_lock failed, error = %d.", int(res));
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
	: size(s), p_bitmap(CreateCompatibleDIBSection(s, p_buffer))
{}
ScreenBuffer::ScreenBuffer(ScreenBuffer&&) ynothrow = default;
#	endif
ImplDeDtor(ScreenBuffer)

ScreenBuffer&
ScreenBuffer::operator=(ScreenBuffer&& sbuf) ynothrow
{
	sbuf.swap(*this);
	return *this;
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
void
ScreenBuffer::Resize(const Size& s)
{
	if(s != size)
		*this = ScreenBuffer(s);
}

void
ScreenBuffer::Premultiply(ConstBitmapPtr p_buf) ynothrow
{
	YAssertNonnull(p_buf);
	// NOTE: Since the stride is guaranteed equal to the width, the storage for
	//	pixels can be supposed to be contiguous.
	std::transform(p_buf, p_buf + size.Width * size.Height, p_buffer,
		[](const Pixel& pixel) ynothrow{
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
	CopyBitmapBuffer(Deref(p_impl).GetBufferPtr(), p_buf, GetSize());
#	elif YCL_Win32
	// NOTE: Since the pitch is guaranteed equal to the width, the storage for
	//	pixels can be supposed to be contiguous.
	CopyBitmapBuffer(GetBufferPtr(), p_buf, size);
#	endif
}

#	if YCL_Win32
void
ScreenBuffer::UpdateFromBounds(ConstBitmapPtr p_buf, const Rect& r) ynothrow
{
	BlitLines<false, false>(CopyLine<true>(), GetBufferPtr(), Nonnull(p_buf),
		size, size, r.GetPoint(), r.GetPoint(), r.GetSize());
}

void
ScreenBuffer::UpdatePremultipliedTo(NativeWindowHandle h_wnd, AlphaType a,
	const Point& pt)
{
	GSurface<> sf(h_wnd);

	sf.UpdatePremultiplied(*this, h_wnd, a, pt);
}
#	endif

void
ScreenBuffer::UpdateTo(NativeWindowHandle h_wnd, const Point& pt) ynothrow
{
#	if YCL_HostedUI_XCB || YCL_Android
	UpdateContentTo(h_wnd, {pt, GetSize()}, GetContext());
#	elif YCL_Win32
	GSurface<> sf(h_wnd);

	sf.Update(*this, pt);
#	endif
}
#if YCL_Win32
void
ScreenBuffer::UpdateToBounds(NativeWindowHandle h_wnd, const Rect& r,
	const Point& sp) ynothrow
{
	GSurface<> sf(h_wnd);

	sf.UpdateBounds(*this, r, sp);
}
#endif

void
ScreenBuffer::swap(ScreenBuffer& sbuf) ynothrow
{
#	if YCL_HostedUI_XCB || YCL_Android
	Deref(p_impl).swap(Deref(sbuf.p_impl)),
	std::swap(width, sbuf.width);
#	elif YCL_Win32
	std::swap(size, sbuf.size),
	std::swap(p_buffer, sbuf.p_buffer),
	std::swap(p_bitmap, sbuf.p_bitmap);
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
	UpdateBounds(sbuf, {pt, sbuf.GetSize()});
}
void
WindowMemorySurface::UpdateBounds(ScreenBuffer& sbuf, const Rect& r,
	const Point& sp) ynothrow
{
	const auto h_old(::SelectObject(h_mem_dc, sbuf.GetNativeHandle()));

	YCL_CallWin32_Trace(BitBlt, "WindowMemorySurface::UpdateBounds", h_owner_dc,
		int(r.X), int(r.Y), int(r.Width), int(r.Height), h_mem_dc, int(sp.X),
		int(sp.Y), SRCCOPY);
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

	YCL_CallWin32_Trace(UpdateLayeredWindow,
		"WindowMemorySurface::UpdatePremultiplied", h_wnd, h_owner_dc,
		reinterpret_cast<::POINT*>(&rect), &size, h_mem_dc, &ptx, 0, &bfunc,
		ULW_ALPHA);
	::SelectObject(h_mem_dc, h_old);
}


WindowDeviceContext::WindowDeviceContext(NativeWindowHandle h_wnd)
	// NOTE: Painting using %::GetDC and manually managing clipping areas
	//	instead of %::GetDCEx, for performance and convenience calculation
	//	of input boundary.
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
	static_assert(ystdex::is_aligned_storable<::PAINTSTRUCT, decltype(ps)>(),
		"Invalid type found.");

	::EndPaint(hWindow, reinterpret_cast<::PAINTSTRUCT*>(ps));
}

Rect
WindowRegionDeviceContext::GetInvalidatedArea() const
{
	// XXX: To workaround [-fstrict-aliasing].
	const auto p(reinterpret_cast<const ::PAINTSTRUCT*>(ps));

	return FetchRectFromBounds((*p).rcPaint);
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
	: WindowClass(wc.lpszClassName, YCL_CallWin32(RegisterClassW,
	"WindowClass::WindowClass", &wc), wc.hInstance)
{}
WindowClass::WindowClass(const ::WNDCLASSEXW& wc)
	: WindowClass(wc.lpszClassName, YCL_CallWin32(RegisterClassExW,
	"WindowClass::WindowClass", &wc), wc.hInstance)
{}
WindowClass::WindowClass(const wstring& class_name,
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
	FilterExceptions([this]{
		YTraceDe(Notice, "Window class '%s' of atom '%hu' unregistered.",
			name.empty() ? "<unknown>" : WCSToUTF8(name).c_str(), atom);
	}, "WindowClass::~WindowClass");
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

	wchar_t buf[arrlen(WindowClassName)];

	YCL_CallWin32(GetClassNameW, "HostWindow::HostWindow", GetNativeHandle(),
		buf, arrlen(WindowClassName));
	if(std::wcscmp(buf, WindowClassName) != 0)
		throw GeneralEvent("Wrong windows class name found.");
	::SetLastError(0);
	if(YB_UNLIKELY(::SetWindowLongPtrW(GetNativeHandle(), GWLP_USERDATA,
		::LONG_PTR(this)) == 0 && GetLastError() != 0))
		YCL_Raise_Win32Exception("SetWindowLongPtrW");
	YCL_CallWin32(SetWindowPos, "HostWindow::HostWindow",
		GetNativeHandle(), {}, 0, 0, 0, 0,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREDRAW
		| SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOZORDER);

	::RAWINPUTDEVICE rid{0x01, 0x02, 0, nullptr};

	YCL_CallWin32(RegisterRawInputDevices, "HostWindow::HostWindow",
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
#	endif


#	if YCL_Win32
Clipboard::Clipboard(NativeWindowHandle h_wnd)
{
	// FIXME: Spin for remote desktops?
	YCL_CallWin32(OpenClipboard, "Clipboard::Clipboard", h_wnd);
}
Clipboard::~Clipboard()
{
	YCL_CallWin32_Trace(CloseClipboard, "Clipboard::~Clipboard", );
}

bool
Clipboard::IsAvailable(FormatType fmt) ynothrow
{
	return bool(::IsClipboardFormatAvailable(fmt));
}

void
Clipboard::CheckAvailable(FormatType fmt)
{
	YCL_CallWin32(IsClipboardFormatAvailable, "Clipboard::CheckAvailable", fmt);
}

void
Clipboard::Clear() ynothrow
{
	YCL_CallWin32_Trace(EmptyClipboard, "Clipboard::Clear", );
}

NativeWindowHandle
Clipboard::GetOpenWindow() ynothrow
{
	return ::GetOpenClipboardWindow();
}

bool
Clipboard::Receive(YSLib::string& str)
{
	return ReceiveRaw(CF_TEXT, [&](const void* p) ynothrowv{
		str = Deref(static_cast<const GlobalLocked*>(p)).GetPtr<char>();
	});
}
bool
Clipboard::Receive(YSLib::String& str)
{
	return ReceiveRaw(CF_UNICODETEXT, [&](const void* p) ynothrowv{
		str = Deref(static_cast<const GlobalLocked*>(p)).GetPtr<ucs2_t>();
	});
}

bool
Clipboard::ReceiveRaw(FormatType fmt, std::function<void(const void*)> f)
{
	if(IsAvailable(fmt))
		if(const auto h = ::GetClipboardData(fmt))
		{
			const GlobalLocked gl(h);

			Nonnull(f)(&gl);
			return true;
		}
	return {};
}

void
Clipboard::Send(const string& str)
{
	SendRaw(CF_TEXT, CopyGlobalString<char>(str));
}
void
Clipboard::Send(const String& str)
{
	SendRaw(CF_UNICODETEXT, CopyGlobalString<ucs2_t>(str));
}
void
Clipboard::Send(ConstBitmapPtr p_bmp, const Size& s)
{
	YAssertNonnull(p_bmp),
	YAssert(s != Size::Invalid, "Invalid size found.");

	auto p(MakeMoveableGlobalMemory(sizeof(::BITMAPV5HEADER)
		+ GetAreaOf(s) * sizeof(Pixel)));
	{
		const GlobalLocked gl(Nonnull(p));
		const auto p_buf(gl.GetPtr<::BITMAPV5HEADER>());

		*p_buf = {sizeof(::BITMAPV5HEADER), CheckPositiveScalar<long>(s.Width,
			"width"), -CheckPositiveScalar<long>(s.Height, "height"), 1, 32,
			BI_BITFIELDS, static_cast<unsigned long>(sizeof(Pixel)
			* GetAreaOf(s)), 0, 0, 0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF,
			0xFF000000, 0x73524742/*LCS_sRGB*/, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}
			}, 0, 0, 0, LCS_GM_IMAGES, 0, 0, 0},
		CopyBitmapBuffer(reinterpret_cast<Pixel*>(p_buf + 1), p_bmp, s);
	}
	SendRaw(CF_DIBV5, p.release());
}

void
Clipboard::SendRaw(FormatType fmt, void* h)
{
	Clear();
	YCL_CallWin32(SetClipboardData, "Clipboard::SendRaw", fmt, h);
}


void
ExecuteShellCommand(const wchar_t* cmd, const wchar_t* args, bool use_admin,
	const wchar_t* dir, int n_cmd_show, NativeWindowHandle h_parent)
{
	// TODO: Set current working directory as %USERPROFILE%?
	auto res(int(std::intptr_t(::ShellExecuteW(h_parent,
		use_admin ? L"runas" : nullptr, Nonnull(cmd), args, dir, n_cmd_show))));

	switch(res)
	{
	case 0:
	case SE_ERR_OOM:
		// TODO: Use inherited class of exception.
		throw std::bad_alloc();
	case SE_ERR_SHARE:
	case SE_ERR_DLLNOTFOUND:
		res = SE_ERR_SHARE ? ERROR_SHARING_VIOLATION : ERROR_DLL_NOT_FOUND;
	case ERROR_FILE_NOT_FOUND: // NOTE: Same as %SE_ERR_FNF.
	case ERROR_PATH_NOT_FOUND: // NOTE: Same as %SE_ERR_PNF.
	case ERROR_ACCESS_DENIED: // NOTE: Same as %SE_ERR_ACCESSDENIED.
	case ERROR_BAD_FORMAT:
		throw Win32Exception(Win32Exception::ErrorCode(res), "ShellExecuteW",
			Err);
	case SE_ERR_ASSOCINCOMPLETE:
	case SE_ERR_NOASSOC:
	case SE_ERR_DDETIMEOUT:
	case SE_ERR_DDEFAIL:
	case SE_ERR_DDEBUSY:
	{
		using boxed_exception = ystdex::wrap_mixin_t<std::runtime_error, int>;
		const auto throw_ex([=](int ec) YB_ATTR(noreturn){
			std::throw_with_nested(Win32Exception(Win32Exception::ErrorCode(ec),
				ystdex::sfmt("ShellExecuteW: %d", res), Err));
		});

		TryExpr(throw boxed_exception{std::runtime_error("ShellExecuteW"), res})
		catch(boxed_exception& e)
		{
			switch(e.value)
			{
			case SE_ERR_ASSOCINCOMPLETE:
			case SE_ERR_NOASSOC:
				throw_ex(ERROR_NO_ASSOCIATION);
			case SE_ERR_DDETIMEOUT:
			case SE_ERR_DDEFAIL:
			case SE_ERR_DDEBUSY:
				throw_ex(ERROR_DDE_FAIL);
			default:
				break;
			}
		}
		YAssert(false, "Invalid state found.");
	}
	default:
		if(res > 32)
			YTraceDe(Informative, "ExecuteShellCommand: ::ShellExecute call"
				" succeed with return value %d.", res);
		else
			throw LoggedEvent(ystdex::sfmt("ExecuteShellCommand:"
				" ::ShellExecuteW call failed" " with unknown error '%d'.",
				res), Err);
	}
}
#	endif

} // namespace platform_ex;

#endif

